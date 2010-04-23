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


#define __INCLUDE_CAPABILITY_NAMES__

#include <e32test.h>
#include <f32file.h>

#include "asyncc-tui.h"

#include <swi/asynclauncher.h>
#include <e32capability.h>

using namespace Swi;




CUIScriptAdaptor::CUIScriptAdaptor()
	{
	}

CUIScriptAdaptor* CUIScriptAdaptor::NewLC()
	{
	CUIScriptAdaptor* self = new(ELeave) CUIScriptAdaptor();
	CleanupStack::PushL(self);
	return self;
	}

CUIScriptAdaptor* CUIScriptAdaptor::NewL()
	{
	CUIScriptAdaptor* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
CUIScriptAdaptor::~CUIScriptAdaptor()
	{
	}



TBool CUIScriptAdaptor::DisplayInstallL(const CAppInfo& /*aAppInfo*/,
										const CApaMaskedBitmap* /*aLogo*/,
										const RPointerArray<CCertificateInfo>& /*aCertificates*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayGrantCapabilitiesL(const CAppInfo& /*aAppInfo*/, 
	const TCapabilitySet& /*aCapabilitySet*/)
	{
	return ETrue;
	}


TInt CUIScriptAdaptor::DisplayLanguageL(const CAppInfo& /*aAppInfo*/, 
						 const RArray<TLanguage>& /*aLanguages*/)
	{
	return 0;
	}


TInt CUIScriptAdaptor::DisplayDriveL(const CAppInfo& /*aAppInfo*/,
						 TInt64 /*aSize*/, const RArray<TChar>& /*aDriveLetters*/,
						 const RArray<TInt64>& /*aDriveSpaces*/)
	{
	return 0; // Use first drive
	}


TBool CUIScriptAdaptor::DisplayUpgradeL(const CAppInfo& /*aAppInfo*/,
	const CAppInfo& /*aExistingAppInfo*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayOptionsL(const CAppInfo& /*aAppInfo*/,
										const RPointerArray<TDesC>& aOptions,
										RArray<TBool>& aSelections)
	{
	// Select all the options
	for (TInt i=0; i < aOptions.Count();)
		{
		aSelections[i]= ETrue;
		if (++i  >= aOptions.Count())
			{
			break;
			}
		}
	return 	ETrue;
	}

TBool CUIScriptAdaptor::HandleInstallEventL(
						const CAppInfo& /*aAppInfo*/,
						TInstallEvent   /*aEvent*/, 
						TInt            /*aValue*/, 
						const TDesC&    /*aDes*/)
	{
	return ETrue;
	}

void CUIScriptAdaptor::HandleCancellableInstallEventL(
						const CAppInfo&          /*aAppInfo*/,
						TInstallCancellableEvent /*aEvent*/, 
						MCancelHandler&          /*aCancelHandler*/,
						TInt                     /*aValue*/, 
						const TDesC&             /*aDes*/)
	{
	}

TBool CUIScriptAdaptor::DisplaySecurityWarningL(
		const CAppInfo& /*aAppInfo*/,
		TSignatureValidationResult /*aSigValidationResult*/,
		RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		TBool /*aInstallAnyway*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayOcspResultL(const CAppInfo& /*aAppInfo*/,
										   TRevocationDialogMessage /*aMessage*/, 
										   RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
										   RPointerArray<CCertificateInfo>& /*aCertificates*/,
										   TBool /*aWarningOnly*/)
	{
	return ETrue;
	}

void CUIScriptAdaptor::DisplayCannotOverwriteFileL(const CAppInfo& /*aAppInfo*/,
						 const CAppInfo& /*aInstalledAppInfo*/,
						 const TDesC& /*aFileName*/)
	{
	}

TBool CUIScriptAdaptor::DisplayUninstallL(const CAppInfo& /*aAppInfo*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayTextL(const CAppInfo& /*aAppInfo*/, 
									 TFileTextOption /*aOption*/, const TDesC& /*aText*/)
	{
	return ETrue;
	}

void CUIScriptAdaptor::DisplayErrorL(const CAppInfo& /*aAppInfo*/,
						 TErrorDialog /*aType*/, const TDesC& /*aDes*/)
	{
	}

TBool CUIScriptAdaptor::DisplayDependencyBreakL(const CAppInfo& /*aAppInfo*/,
					      const RPointerArray<TDesC>& /*aComponents*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayApplicationsInUseL(const CAppInfo& /*aAppInfo*/, 
							const RPointerArray<TDesC>& /*aAppNames*/)
	{
	return ETrue;
	}

TBool CUIScriptAdaptor::DisplayQuestionL(const CAppInfo& /*aAppInfo*/, 
							TQuestionDialog /*aQuestion*/, const TDesC& /*aDes*/)
	{
	return ETrue;
	}


TBool CUIScriptAdaptor::DisplayMissingDependencyL(const CAppInfo& /*aAppInfo*/,
	const TDesC& /*aDependencyName*/,TVersion /*aWantedVersionFrom*/,
	TVersion /*aWantedVersionTo*/,TVersion /*aInstalledVersion*/)
	{
	return ETrue;
	}
// End of file
