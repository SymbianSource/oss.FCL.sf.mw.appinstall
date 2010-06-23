/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "fileextractor.h"
#include "progressbar.h"
#include "sishelper.h"
#include "sisuihandler.h"

#include "sisregistryfiledescription.h"

namespace Swi
{

/*static*/ CFileExtractor* CFileExtractor::NewLC(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo)
	{
	CFileExtractor* self=new(ELeave) CFileExtractor(aFs, aSisHelper, aUiHandler, aAppInfo);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CFileExtractor* CFileExtractor::NewL(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo)
	{
	CFileExtractor* self=NewLC(aFs, aSisHelper, aUiHandler, aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}
	
CFileExtractor::~CFileExtractor()
	{
	Deque();
	
	if (iManagedFileHandle)
		{
		iCurrentFile->Close();
		delete iCurrentFile;
		}
	delete iFileDescription;
	}

CFileExtractor::CFileExtractor(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo)
	: CActive(EPriorityStandard), iFs(aFs), iSisHelper(aSisHelper), iUiHandler(aUiHandler), iAppInfo(aAppInfo)
	{
	CActiveScheduler::Add(this);
	}

void CFileExtractor::ConstructL()
	{
	User::LeaveIfError(iFs.ShareProtected());
	}

// public methods

void CFileExtractor::ExtractFileL(TInt aDataUnit, const CSisRegistryFileDescription& aFileDescription, const TDesC& aFileName, TRequestStatus& aStatus)
	{
	iManagedFileHandle=ETrue;
	
	iCurrentFile=new(ELeave) RFile;
	User::LeaveIfError(iCurrentFile->Replace(iFs, aFileName, EFileWrite));
	
	ExtractFileL(aDataUnit, aFileDescription, *iCurrentFile, aStatus);
	}


void CFileExtractor::ExtractFileL(TInt aDataUnit, const CSisRegistryFileDescription& aFileDescription, RFile& aFile, TRequestStatus& aStatus)
	{
	iCancelled=EFalse;
	iClientStatus=&aStatus;
	iCurrentFile=&aFile;
	iBytesToExtract=aFileDescription.UncompressedLength();
	iDataUnit=aDataUnit;
	
	delete iFileDescription;
	iFileDescription=0;
	iFileDescription=CSisRegistryFileDescription::NewL(aFileDescription);
	
	
	*iClientStatus=KRequestPending;
	
	iSisHelper.SetupAsyncExtractionL(iFs, *iCurrentFile, iFileDescription->Index(), iDataUnit);

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
		
	
TInt CFileExtractor::FinishExtraction(TInt aResult)
	{
	TRAPD(err, iSisHelper.EndAsyncExtractionL());
	if (err != KErrNone)
		return err;

	if (iManagedFileHandle)
		{
		iCurrentFile->Close();
		delete iCurrentFile;
		iCurrentFile=0;
		iManagedFileHandle = EFalse;
		}
		
	if (aResult==KErrNone)
		{
		// The progress bar should always be incremented at the end of a successful extraction
		TRAPD(err, iUiHandler.UpdateProgressBarL(iAppInfo, KProgressBarEndIncrement));
		if (err != KErrNone)
			return err;
		}
		
	// notify the user
	User::RequestComplete(iClientStatus, aResult);
	iClientStatus = NULL;
	return KErrNone;
	}

// CActive methods

void CFileExtractor::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::LeaveIfError(FinishExtraction(iStatus.Int()));
		return;
		}
	
	if (iCancelled)
		{
		User::LeaveIfError(FinishExtraction(KErrCancel));
		return;
		}

	TInt32 chunkSize=iBytesToExtract > KProgressBarInstallChunkSize ? KProgressBarInstallChunkSize : iBytesToExtract; 	

	if (chunkSize==0)
		{
		User::LeaveIfError(FinishExtraction(KErrNone));
		return;
		}

	// Extract the next chunk
	iUiHandler.UpdateProgressBarL(iAppInfo, ProgressBarChunkIncrement(chunkSize));
	iSisHelper.AsyncExtractionL(chunkSize, iStatus);
	iBytesToExtract-=chunkSize;
	SetActive();
	}

void CFileExtractor::DoCancel()
	{
	iCancelled=ETrue;
	if (iClientStatus)
		{
		TInt err = FinishExtraction(KErrCancel);
		if (err != KErrNone)
			{
			User::RequestComplete(iClientStatus, KErrCancel);
			iClientStatus=NULL;
			}
		}
	}
	
TInt CFileExtractor::RunError(TInt aError)
	{
	iCancelled = ETrue;
	if (iClientStatus)
		{
		TInt err = FinishExtraction(aError);
		if (err != KErrNone)
			{
			User::RequestComplete(iClientStatus, aError);
			iClientStatus = NULL;
			}
		}
	return KErrNone;
	}
	

}

