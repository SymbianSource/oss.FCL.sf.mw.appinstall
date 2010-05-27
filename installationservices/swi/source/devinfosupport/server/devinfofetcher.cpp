/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Device information fetching active object
*
*/


#include "devinfofetcher.h"

#include <metadatabase.h>
#include <commsdattypesv1_1.h>

#include <e32property.h> // RProperty
#include <connect/sbdefs.h>// KUidBackupRestoreKey 
#include "swi/securitypolicy.h" // CSecurityPolicy

#include "log.h"
#include "deviceidlist.h"

namespace Swi
{

_LIT(KDevInfoSupportPanic, "devinfosupport");
static const TInt KLeaveInRunL = 1;

/******************************************************************************
 * Test functions that can simulate multiple phones (or be implemented
 * in terms of real code).  Use the
 * DEVINFOFETCHER_TIMING_AND_SIZING_TEST macro to use the stubbed out
 * multiple devices code.  When you use this macro, the stub code
 * simulates (a) multiple phones, so that multiple devices can be
 * returned across the IPC boundary and (b) a delay in response from
 * the etel server.  This macro should be used from manual testing of
 * the server, in conjunction with the tdevinfosupport executable.
 */
static inline TInt EnumeratePhones(RTelServer& aTelServer, TInt& aNumPhones);
static inline TInt GetPhoneInfo(RTelServer& aTelServer, TInt aIndex, RTelServer::TPhoneInfo& aPhoneInfo);
static inline TInt GetTsyName(RTelServer& aTelServer, TInt aIndex, TDes& aTsyName);
static inline TInt PhoneOpen(RMobilePhone& aPhone, RTelServer& aTelServer, TDesC& aPhoneName);
static inline TInt GetIdentityCaps(RMobilePhone& aPhone, TUint32& aCaps);
static inline void GetPhoneId(RMobilePhone& aPhone, TRequestStatus& aReqStatus, RMobilePhone::TMobilePhoneIdentityV1& aMobilePhoneId);
static inline void PhoneClose(RMobilePhone& aPhone);
static inline void PhoneCancelGetPhoneId(RMobilePhone& aPhone);



CDeviceInfoFetcher* CDeviceInfoFetcher::NewL(MDeviceInfoReceiver& aClient, TPriority aPriority)
	{
	CDeviceInfoFetcher* self=CDeviceInfoFetcher::NewLC(aClient, aPriority);
	CleanupStack::Pop(self);
	return self;
	}
	
CDeviceInfoFetcher* CDeviceInfoFetcher::NewLC(MDeviceInfoReceiver& aClient, TPriority aPriority)
	{
	CDeviceInfoFetcher* self=new(ELeave)CDeviceInfoFetcher(aClient, aPriority);
	CleanupStack::PushL(self);
	self->StartL();
	return self;
	}

CDeviceInfoFetcher::CDeviceInfoFetcher(MDeviceInfoReceiver& aClient, TPriority aPriority)
	: CActive(aPriority), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

CDeviceInfoFetcher::~CDeviceInfoFetcher()
	{
	Deque();
	PhoneClose(iMobilePhone);
	iTelServer.Close();
	delete iDeviceIdList;
	}

void CDeviceInfoFetcher::GetTsyNameL()
	{	
	// verify for restore P&S flag
	TInt restorePropertyVal= 0; 	
	// Ignore the error.
	RProperty::Get(TUid::Uid(KUidSystemCategoryValue), conn::KUidBackupRestoreKey,restorePropertyVal);	
	TBool restore = (restorePropertyVal & conn::EBURRestoreFull) || (restorePropertyVal & conn::EBURRestorePartial);

	// Don not use CommDB at restore time to get TSY name,central repository will be in locked state. use swipolicy.ini setting instead.(INC095248) 
	if (!restore)
		{
		using namespace CommsDat;
 		CMDBSession* db = CMDBSession::NewLC(KCDLatestVersion);

		CMDBField<TUint32> globalSettingField(KCDTIdModemPhoneServicesSMS);
		globalSettingField.SetRecordId(1);
		globalSettingField.LoadL(*db);
		TUint32 modemId = globalSettingField;
		CCDModemBearerRecord* modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
		CleanupStack::PushL(modemRecord);
		modemRecord->SetRecordId(modemId);
		modemRecord->LoadL(*db);
		iTsyName = modemRecord->iTsyName;
		CleanupStack::PopAndDestroy(2, db);
		}
	else
		{
		DEBUG_PRINTF(_L("Using CSecurityPolicy to get TSY name"));
		// get the TSY Name from swipolicy.ini file
		Swi::RSecPolHandle secPol;
		secPol.OpenL();		
		iTsyName.Copy(secPol().PhoneTsyName());
		secPol.Close();	
		if(iTsyName.Length() == 0)
			{
			DEBUG_PRINTF(_L("Leaving due to Empty TSY name"));
		 	User::Leave(KErrNotFound);
		 	}
		}
	}

void CDeviceInfoFetcher::ConnectToEtelL()
	{
	DEBUG_PRINTF(_L8("Device Info Server - Connecting to ETEL"));
	
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(iTsyName));
	}

void CDeviceInfoFetcher::GetNextDeviceIdL()
	{
	if (!iDeviceIdList)
		{
		iDeviceIdList = CDeviceIdList::NewL();
		}

	TBool foundPhone = EFalse;
	for (; iCurrentPhoneIndex < iNumPhones && !foundPhone; ++iCurrentPhoneIndex)
		{
		RTelServer::TPhoneInfo phoneInfo;
		TBuf<KCommsDbSvrMaxFieldLength> phoneTsyName;
		User::LeaveIfError(GetPhoneInfo(iTelServer, iCurrentPhoneIndex, phoneInfo));

		User::LeaveIfError(GetTsyName(iTelServer, iCurrentPhoneIndex, phoneTsyName));

		if (phoneTsyName.CompareF(iTsyName) == 0)
			{
			DEBUG_PRINTF3(_L("Device Info Server - Opening phone at index %d, Name: '%S'."),
				iCurrentPhoneIndex, &(phoneInfo.iName));
			
			User::LeaveIfError(PhoneOpen(iMobilePhone, iTelServer, phoneInfo.iName));

			TUint32 caps = 0;
			TInt err = GetIdentityCaps(iMobilePhone, caps);
			
			DEBUG_PRINTF3(_L8("Device Info Server - Return code from fetching identity capabilities was %d, caps: 0x08x"),
				err, caps);
			
			if(err != KErrNone && err != KErrNotSupported)
				{
				// An unexpected error occurred
				User::Leave(err);
				}
			else if(err == KErrNotSupported)
				{
				// The phone does not support Identity capabilities
				// therefore just assume it has no DeviceId
				PhoneClose(iMobilePhone);
				}
			else if (caps & RMobilePhone::KCapsGetSerialNumber)
				{
				DEBUG_PRINTF(_L8("Device Info Server - This phone supports serial number fetch"));
				
				foundPhone = ETrue;
				}
			else 
				{
				DEBUG_PRINTF(_L8("Device Info Server - This phone does not support serial number fetch"));
				
				PhoneClose(iMobilePhone);
				}
			}
		else
			{
			PhoneClose(iMobilePhone);
			}
		}
	if (foundPhone)
		{
		// we've found a phone that matches the TSY we're using - so
		// make the async call to get its device id.
		iStatus=KRequestPending;
		SetActive();
		GetPhoneId(iMobilePhone, iStatus, iMobilePhoneId);
		
		DEBUG_PRINTF(_L8("Device Info Server - Fetching serial number for this phone"));
		}
	else
		{
		DEBUG_PRINTF(_L8("Device Info Server - Last phone queried. Device ID list complete."));
		
		// no more phones to process, complete the client request.
		// Note, this transfers ownership to the client - hense
		// assignment to NULL.
		iClient.AcceptDeviceIdsL(iDeviceIdList);
		iDeviceIdList = NULL;
		}

	}

void CDeviceInfoFetcher::StartL()
	{
	GetTsyNameL();
	ConnectToEtelL();

	User::LeaveIfError(EnumeratePhones(iTelServer, iNumPhones));
	DEBUG_PRINTF2(_L8("Device Info Server - Current device has %d phones"), iNumPhones);

	GetNextDeviceIdL();
	}

void CDeviceInfoFetcher::RunL()
	{
	// GetNextDeviceIdL() Request has finished
	PhoneClose(iMobilePhone);
	if (iStatus.Int() == KErrNone)
		{
		DEBUG_PRINTF2(_L("Device Info Server - Device ID fetch suceeded. ID: '%S'."),
			&(iMobilePhoneId.iSerialNumber));
		
		iDeviceIdList->AppendL(iMobilePhoneId.iSerialNumber);
		GetNextDeviceIdL();
		}
	else
		{
		DEBUG_PRINTF2(_L8("Device Info server - Device ID fetch failed. Error code: %d."), iStatus.Int());
		
		iClient.HandleDeviceIdErrorL(iStatus.Int());
		delete this;
		}
	}

TInt CDeviceInfoFetcher::RunError(TInt)
	{
	User::Panic(KDevInfoSupportPanic, KLeaveInRunL);
	return KErrNone;
	}

void CDeviceInfoFetcher::DoCancel()
	{
	delete iDeviceIdList;
	iDeviceIdList = NULL;
	PhoneCancelGetPhoneId(iMobilePhone);
	}



#ifdef DEVINFOFETCHER_TIMING_AND_SIZING_TEST
static const TInt numPhones = 4;
_LIT(KPhoneId1, "Phone1");
_LIT(KPhoneId2, "Phone2");
_LIT(KPhoneId3, "Phone3");
_LIT(KPhoneId4, "Phone4");
_LIT(KTsy1, "sim");
_LIT(KTsy2, "sim");
_LIT(KTsy3, "sim");
_LIT(KTsy4, "sim");
_LIT(KDevId1, "Dev1");
_LIT(KDevId2, "Dev2");
_LIT(KDevId3, "Dev3");
_LIT(KDevId4, "Dev4");

static TInt selectedPhone = -1;
static RTimer timer;


static inline TInt EnumeratePhones(RTelServer& /*aTelServer*/, TInt& aNumPhones) 
	{
	aNumPhones = numPhones;
	return KErrNone;
	}
static inline TInt GetPhoneInfo(RTelServer& /*aTelServer*/, TInt aIndex, RTelServer::TPhoneInfo& aPhoneInfo) 
	{
	TInt err = KErrNone;
	switch(aIndex)
		{
	case 0:
		aPhoneInfo.iName = KPhoneId1;
		break;
	case 1:
		aPhoneInfo.iName = KPhoneId2;
		break;
	case 2:
		aPhoneInfo.iName = KPhoneId3;
		break;
	case 3:
		aPhoneInfo.iName = KPhoneId4;
		break;
	default:
		err = KErrArgument;
		break;
		}
	return err;
	}
static inline TInt GetTsyName(RTelServer& /*aTelServer*/, TInt aIndex, TDes& aTsyName) 
	{
	TInt err = KErrNone;
	switch(aIndex)
		{
	case 0:
		aTsyName = KTsy1;
		break;
	case 1:
		aTsyName = KTsy2;
		break;
	case 2:
		aTsyName = KTsy3;
		break;
	case 3:
		aTsyName = KTsy4;
		break;
	default:
		err = KErrArgument;
		break;
		}
	return err;
	}
static const int KInvalidTestState = 1000;
static inline TInt PhoneOpen(RMobilePhone& /*aPhone*/, RTelServer& /*aTelServer*/, TDesC& aPhoneName) 
	{
	TInt err = KErrNone;
	if (selectedPhone != -1)
		{
		User::Panic(KDevInfoSupportPanic, KInvalidTestState);
		}
	if (aPhoneName == KPhoneId1)
		{
		selectedPhone = 0;
		}
	else if (aPhoneName == KPhoneId2)
		{
		selectedPhone = 1;
		}
	else if (aPhoneName == KPhoneId3)
		{
		selectedPhone = 2;
		}
	else if (aPhoneName == KPhoneId4)
		{
		selectedPhone = 3;
		}
	else 
		{
		err = KErrArgument;
		}
	timer.CreateLocal();
	return err;
}
static inline TInt GetIdentityCaps(RMobilePhone& /*aPhone*/, TUint32& aCaps)
	{
	aCaps = 0xFFFFFFFF;
	return KErrNone;
	}
static inline void GetPhoneId(RMobilePhone& /*aPhone*/, TRequestStatus& aReqStatus, RMobilePhone::TMobilePhoneIdentityV1& aMobilePhoneId)
	{
	TInt err = KErrNone;
	switch (selectedPhone)
		{
	case 0:
		aMobilePhoneId.iSerialNumber = KDevId1;
		break;
	case 1:
		aMobilePhoneId.iSerialNumber = KDevId2;
		break;
	case 2:
		aMobilePhoneId.iSerialNumber = KDevId3;
		break;
	case 3:
		aMobilePhoneId.iSerialNumber = KDevId4;
		break;
	default:
		err = KErrArgument;
		break;
		}
	if (err == KErrNone)
		{
		// complete the request after a (relatively long) time interval,
		// to make sure that the client waits for the server to start up.
		TTimeIntervalMicroSeconds32 interval = 1000000; // 1 secs
		timer.After(aReqStatus, interval);
		}
	else
		{
		TRequestStatus* status = &aReqStatus;
		User::RequestComplete(status, err);
		}
	}
static inline void PhoneClose(RMobilePhone& /*aPhone*/)
	{
	if (selectedPhone != -1)
		{
		timer.Close();
		selectedPhone = -1;
		}
	}
static inline void PhoneCancelGetPhoneId(RMobilePhone& /*aPhone*/)
	{
	timer.Cancel();
	}


#else
static inline TInt EnumeratePhones(RTelServer& aTelServer, TInt& aNumPhones)
	{
	return aTelServer.EnumeratePhones(aNumPhones);
	}

static inline TInt GetPhoneInfo(RTelServer& aTelServer, TInt aIndex, RTelServer::TPhoneInfo& aPhoneInfo)
	{
	return aTelServer.GetPhoneInfo(aIndex, aPhoneInfo);
	}

static inline TInt GetTsyName(RTelServer& aTelServer, TInt aIndex, TDes& aTsyName)
	{
	return aTelServer.GetTsyName(aIndex, aTsyName);
	}

static inline TInt PhoneOpen(RMobilePhone& aPhone, RTelServer& aTelServer, TDesC& aPhoneName)
	{
	return aPhone.Open(aTelServer, aPhoneName);
	}

static inline TInt GetIdentityCaps(RMobilePhone& aPhone, TUint32& aCaps)
	{
	return aPhone.GetIdentityCaps(aCaps);
	}

static inline void GetPhoneId(RMobilePhone& aPhone, TRequestStatus& aReqStatus, RMobilePhone::TMobilePhoneIdentityV1& aMobilePhoneId)
	{
	aPhone.GetPhoneId(aReqStatus, aMobilePhoneId);
	}

static inline void PhoneClose(RMobilePhone& aPhone)
	{
	aPhone.Close();
	}

static inline void PhoneCancelGetPhoneId(RMobilePhone& aPhone)
	{
	aPhone.CancelAsyncRequest(EMobilePhoneGetPhoneId); 
	}

#endif

} // namespace Swi
