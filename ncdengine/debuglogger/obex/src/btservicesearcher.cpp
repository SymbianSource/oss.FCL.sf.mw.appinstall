/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include "BTServiceSearcher.h"
#include "BTServiceSearcher.pan"


CBTServiceSearcher::CBTServiceSearcher()
:  iIsDeviceSelectorConnected(EFalse)   
    {
    }

CBTServiceSearcher::~CBTServiceSearcher()
{
#ifdef _0
    if (iIsDeviceSelectorConnected)
	{
        iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
        iDeviceSelector.Close();
	}
#endif
    
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;
	
    delete iAgent;
    iAgent = NULL;
    
	// An outstanding FindServiceL request must be completed upon object delete
	if (iStatusObserver) {
		User::RequestComplete(iStatusObserver, KErrNone); // everything ~OK
	}
}

void CBTServiceSearcher::Cancel() {

#ifdef _0
    if (iIsDeviceSelectorConnected)
	{
        iDeviceSelector.CancelNotifier(KDeviceSelectionNotifierUid);
    
	}
#endif
    
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;
	
    delete iAgent;
    iAgent = NULL;
    
	// An outstanding FindServiceL request must be completed upon object delete
	if (iStatusObserver) {
		User::RequestComplete(iStatusObserver, KErrCancel); // everything ~OK
	}



}

void CBTServiceSearcher::SelectDeviceByDiscoveryL(TRequestStatus& /* aObserverRequestStatus */)
    {
    
#ifdef _0
    if (!iIsDeviceSelectorConnected)
        {
        User::LeaveIfError(iDeviceSelector.Connect());
        iIsDeviceSelectorConnected = ETrue;
        }
#endif

    //  Request a device selection 
    TBTDeviceSelectionParamsPckg selectionFilter;
    selectionFilter().SetUUID(ServiceClass());
    
#ifdef _0
    iDeviceSelector.StartNotifierAndGetResponse(
        aObserverRequestStatus, 
        KDeviceSelectionNotifierUid, 
        selectionFilter, 
        iResponse);
#endif
    }


void CBTServiceSearcher::FindServiceL(TRequestStatus& aObserverRequestStatus)
    {
    if (!iResponse().IsValidBDAddr())
        {
        User::Leave(KErrNotFound);
        }
    iHasFoundService = EFalse;

    // delete any existing agent and search pattern
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;

    iAgent = CSdpAgent::NewL(*this, BTDevAddr());

    iSdpSearchPattern = CSdpSearchPattern::NewL();
   
    iSdpSearchPattern->AddL(ServiceClass()); 
    // return code is the position in the list that the UUID is inserted at 
    // and is intentionally ignored
  
    iAgent->SetRecordFilterL(*iSdpSearchPattern);

    iStatusObserver = &aObserverRequestStatus;

    iAgent->NextRecordRequestL();
    }

void CBTServiceSearcher::NextRecordRequestComplete(
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount)
    {
    TRAPD(error,
        NextRecordRequestCompleteL(aError, aHandle, aTotalRecordsCount);
    );

    if (error != KErrNone) 
        {
        Panic(EBTServiceSearcherNextRecordRequestComplete);
        }
    }

void CBTServiceSearcher::NextRecordRequestCompleteL(
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount
)
    {
    if (aError == KErrEof)
        {
        Finished();
        return;
        }

    if (aError != KErrNone)
        {        
        Finished(aError);
        return;
        }

    if (aTotalRecordsCount == 0)
        {        
        Finished(KErrNotFound);
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL(aHandle, KSdpAttrIdProtocolDescriptorList);
    }

void CBTServiceSearcher::AttributeRequestResult(
    TSdpServRecordHandle aHandle, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue
)
    {
    TRAPD(error,
        AttributeRequestResultL(aHandle, aAttrID, aAttrValue);
    );
    if (error != KErrNone)
        {
        Panic(EBTServiceSearcherAttributeRequestResult);
        }

    // Ownership has been transferred
    delete aAttrValue;
    }

void CBTServiceSearcher::AttributeRequestResultL(
    TSdpServRecordHandle /*aHandle*/, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue
)
    {
    __ASSERT_ALWAYS(aAttrID == KSdpAttrIdProtocolDescriptorList, User::Leave( KErrNotFound ));

    TSdpAttributeParser parser(ProtocolList(), *this);

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL(parser);

    if (parser.HasFinished())
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }

void CBTServiceSearcher::AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
    {
    TRAPD(error,
        AttributeRequestCompleteL(aHandle, aError);
    );
    if (error != KErrNone)
        {
        Panic(EBTServiceSearcherAttributeRequestComplete);
        }
    }

void CBTServiceSearcher::AttributeRequestCompleteL(TSdpServRecordHandle /*aHandle*/, TInt aError)
    {
    if (aError != KErrNone)
        {        
        }
    else if (!HasFinishedSearching())
        {
        // have not found a suitable record so request another
        iAgent->NextRecordRequestL();
        }
    else
        {        
        Finished();
        }
    }

void CBTServiceSearcher::Finished(TInt aError /* default = KErrNone */)
    {
    if (aError == KErrNone && !HasFoundService())
        {
        aError = KErrNotFound;
        }
    User::RequestComplete(iStatusObserver, aError);
    }

TBool CBTServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }

const TBTDevAddr& CBTServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }

const TBTDeviceResponseParams& CBTServiceSearcher::ResponseParams()
    {
    return iResponse();
    }

TBool CBTServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }
