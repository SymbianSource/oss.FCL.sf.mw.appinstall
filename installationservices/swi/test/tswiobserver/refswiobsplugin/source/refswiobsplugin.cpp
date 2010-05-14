/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements Reference SWI Observer ECOM plug-in interface.
*
*/


/**
 @file
 @test
*/
 
#include "refswiobsplugin.h"


static const TUint KRefSwiObsImplementationId = 0x102857A2;

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KRefSwiObsImplementationId, CRefSwiObsPlugin::CreateRefSwiObserverL)
	};
	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	
//
//CRefSwiObsPlugin
//

CRefSwiObsPlugin::CRefSwiObsPlugin()
/**
	Constructor.
 */
:	CSwiObserverPlugin()
 	{
 	
 	}
 	

CRefSwiObsPlugin::~CRefSwiObsPlugin()
/**
	Destructor
 */
	{
	iStream.Close();
	}


CSwiObserverPlugin* CRefSwiObsPlugin::CreateRefSwiObserverL()
/**
	Factory method that instantiates a new SWI Observer ECOM plug-in.

	@return A pointer to the new UPS SWI Observer object.
*/
	{
	CRefSwiObsPlugin *self = new(ELeave) CRefSwiObsPlugin();
	CleanupStack::PushL(self);
	CleanupStack::Pop(self);
	return self;
	}
	

// From CActive
void CRefSwiObsPlugin::DoCancel()
	{
	}
		
		
void CRefSwiObsPlugin::RunL()
	{
	User::LeaveIfError(KErrNone);
	}
		
TInt CRefSwiObsPlugin::RunError(TInt aError)
	{
	(void)aError;
	return KErrNone;
	}
	

void CRefSwiObsPlugin::StartL(RFs& aFs)
	{
	TFileName logFile;
	User::LeaveIfError(aFs.PrivatePath(logFile));
	_LIT(KLogFileName,"refswiobs.log");
	logFile.Append(KLogFileName);
	User::LeaveIfError(iStream.Replace(aFs,logFile,EFileWrite));
	
	//Keep the coverage people happy
	TAny* extensionPtr(0);
	(void)GetExtension(0, extensionPtr, NULL);
	TAny* extensionPtr2(0);
	(void)Extension_(0, extensionPtr, extensionPtr2);
	}
 		
void CRefSwiObsPlugin::NextObservedHeaderL(const CObservationHeader& aHeader, TRequestStatus& aStatus)
	{
	aHeader.ExternalizeL(iStream);
	
	iClientStatus = &aStatus;
	User::RequestComplete(iClientStatus,KErrNone);
	}
 
void CRefSwiObsPlugin::NextObservedDataL(const CObservationData& aData, TInt aFilterIndex, TRequestStatus& aStatus)
	{
	(void)aFilterIndex;
	aData.ExternalizeL(iStream);
	
	iClientStatus = &aStatus;
	User::RequestComplete(iClientStatus,KErrNone);
	}
	
void CRefSwiObsPlugin::FinalizeL(TRequestStatus& aStatus)
 	{
 	iStream.Close();
 	iClientStatus = &aStatus;
	User::RequestComplete(iClientStatus,KErrNone);
	User::LeaveIfError(KErrNone);
 	}
 
 
CObservationFilter* CRefSwiObsPlugin::GetFilterL() const
 	{
 	CObservationFilter *filter = CObservationFilter::NewLC();
 	HBufC* filterData = HBufC::NewLC(1);
 	//Empty filter in order to log every notification
 	filter->AddFilterL(filterData);
	
	CleanupStack::Pop(filterData);
 	CleanupStack::Pop(filter);
 	return filter;
 	}
