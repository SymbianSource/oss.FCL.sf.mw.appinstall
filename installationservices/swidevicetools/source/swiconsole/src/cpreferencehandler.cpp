/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This file provides the class declaration of CPreferencesHandler.
* @internalComponent
*
*/


#include "cpreferencehandler.h"
#include "cpreferences.h"


// Constants and Literals
_LIT(KLogFileName, "SwiConsole.log");


CPreferenceHandler::CPreferenceHandler():
				iMode(ESwiNormalMode),
				iOperation(ESwiHelp),
				iLogFileName(KLogFileName),
				iProgessInfoType(EShowProgress),
				iIsLoggingEnabled(ETrue),
				iIsVerboseEnabled(EFalse),
				iEnableOcspCheck(EFalse)
	{
	return;
	}
	
CPreferenceHandler::~CPreferenceHandler()
	{
	iPreferenceList.ResetAndDestroy();
	}
	
CPreferenceHandler* CPreferenceHandler::NewL()
	{
	CPreferenceHandler* self = CPreferenceHandler::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CPreferenceHandler* CPreferenceHandler::NewLC()
	{
	CPreferenceHandler* self = new (ELeave) CPreferenceHandler;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CPreferenceHandler::ConstructL()
	{
	CreatePreferencesL();
	}

CPreferences* CPreferenceHandler::CreatePreferencesL()
	{
	CPreferences* preferences = CPreferences::NewL();
	// Ownership transfered to iPreferenceList
	iPreferenceList.AppendL(preferences);
	return preferences;
	}

CPreferences* CPreferenceHandler::GetPreferences(const Swi::CAppInfo& aAppInfo) const
	{
	for(TInt i = iPreferenceList.Count() - 1; i > 0; --i)
		{
		if(iPreferenceList[i]->IsPackageNamePresent(aAppInfo.AppName()) && 
		   iPreferenceList[i]->IsVendorNamePresent(aAppInfo.AppVendor()))
			{
			return iPreferenceList[i];
			}
		}
		
		// By default it will return the main CPrefence class stored
		// at the first index
		return iPreferenceList[0];
	}
	
CPreferences* CPreferenceHandler::GetDefaultPreference() const
	{
		return iPreferenceList[0];
	}
