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


#include "installer.h"
#include <swi/asynclauncher.h>



CInstaller::CInstaller()
	:CActive(EPriorityNormal)
	{
	iFinishedInstallation=1;
	iFinishedCanceling=0;
	CActiveScheduler::Add(this);
	}

CInstaller* CInstaller::NewLC(CInstallPrefs* aPrefs)
	{
	CInstaller* result= new (ELeave) CInstaller;
	CleanupStack::PushL(result);
	result->ConstructL(aPrefs);
	return result;
	}

CInstaller* CInstaller::NewL(CInstallPrefs* aPrefs)
	{
	CInstaller *result= CInstaller::NewLC(aPrefs);
	CleanupStack::Pop(result);
	return result;
	}
	
void CInstaller::ConstructL(CInstallPrefs* aPrefs)
	{
	iLauncher = CAsyncLauncher::NewL();
	if (aPrefs==NULL)
		{
		iDeletePrefs=1;
		iPrefs = CInstallPrefs::NewL();	
		}
	else 
		{
		iDeletePrefs=0;
		iPrefs=aPrefs;	
		}
	iUiHandler=CUIScriptAdaptor::NewL();
	}

CInstaller::~CInstaller()
	{
	delete iLauncher;
	if (iDeletePrefs==1) 
		{
		delete iPrefs;
		}
	delete iUiHandler;
	}

void CInstaller::StartL(const TDesC& aSisToInstall)
	{
	iCancelationSuccess=0;
	iFinishedInstallation=1; // will be set to 0 when CancelInstallation is called
	iFinishedCanceling=0;
	iEndStatus=iStatus.Int();
	
	TFileName name(aSisToInstall);

	iLauncher->InstallL( *iUiHandler , name, *iPrefs, iStatus) ;

	
	SetActive();
	}

void CInstaller::CancelInstallation()
	{
	iFinishedInstallation=0;
	iLauncher->CancelOperation();
	}

void CInstaller::RunL()
	{
	iEndStatus=iStatus.Int();
	
	if (iFinishedInstallation==1) 
		{
		// We didn't make it in time to cancel installation, but that isn't an error
		iCancelationSuccess=(iStatus==KErrNone)?1:iStatus.Int();
		}
	else 
		{
		if (iFinishedInstallation==0 && iStatus==KErrCancel) 
			{
			iCancelationSuccess=1;
			}
		else 
			{
			if (iStatus==KErrNone) 
				{
				iCancelationSuccess=1;
				}
			else 
				{
				iCancelationSuccess=0;
				}
			}
		}
	
	// This will be checked by the canceltimer to determine if cancellation has finished
	iFinishedCanceling=1;
	}

void CInstaller::DoCancel()
	{
	}
		
// End of file
