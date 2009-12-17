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

#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <swi/msisuihandlers.h>
#include <e32capability.h>

#include "tuiadaptors.h"

// Dummy test implementation of UI interfaces.

using namespace Swi;
using namespace Swi::Test;

_LIT(KNewLine, "\n");

/** Cancel handler **/
void CancelAdapter::HandleCancel()
	{
	}

/*static*/ SisUiAdapter* SisUiAdapter::NewLC(RConsoleServerSession& aConsole)
	{
	SisUiAdapter* self = new (ELeave) SisUiAdapter(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} 

/*static*/ SisUiAdapter* SisUiAdapter::NewL(RConsoleServerSession& aConsole)
	{
	SisUiAdapter* self = SisUiAdapter::NewLC(aConsole);  
	CleanupStack::Pop(self);
	return self;
	} 

SisUiAdapter::SisUiAdapter(RConsoleServerSession& aConsole) : iConsole(aConsole)
	{
	}


void SisUiAdapter::ConstructL()
	{
	} 

TBool SisUiAdapter::DisplayInstallL(const CAppInfo& aAppInfo,
									const CApaMaskedBitmap* /*aLogo*/,
									const RPointerArray<CCertificateInfo>& /*aCertificates*/)
	{
	TBuf<256> msg;
	_LIT(KTemplate, "Do you wish to install %S by %S version %S");
	TVersion version = aAppInfo.AppVersion();
	TDesC appName = aAppInfo.AppName();
 	TDesC appVendor = aAppInfo.AppVendor();
 	TDesC appVername = version.Name();
 	msg.Format(KTemplate, &appName, &appVendor, &appVername);	
	return AskQuestion(msg);
	}


TBool SisUiAdapter::DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo, 
											  const TCapabilitySet& aCapabilitySet)
	{
	TBuf<256> msg;
	for (TInt k = 0; k < ECapability_HardLimit; k++) 
		{
		if (aCapabilitySet.HasCapability(static_cast<TCapability>(k)))
			{
			//TPtr8 ptr(CapabilityNames[k], 32);
			//iConsole.Printf(ptr);
			msg.Format(KNewLine);
			iConsole.Printf(msg);
			}
		}
	_LIT(KTemplate, "Do you wish to grant the above capabilities to %S by %S version %S");
	TVersion version = aAppInfo.AppVersion();
	TDesC appName = aAppInfo.AppName();
	TDesC appVendor = aAppInfo.AppVendor();
	TDesC appVersion = version.Name();
	msg.Format(KTemplate, &appName, &appVendor, &appVersion);
	return AskQuestion(msg);
	}
    
TInt SisUiAdapter::DisplayLanguageL(const CAppInfo& /*aAppInfo*/, 
									const RArray<TLanguage>& aLanguages)
	{
	TBuf<128> msg;
	msg.Format(_L("\nChoose an installation language:\n"));
	iConsole.Printf(msg);
	for (TInt k = 0; k < aLanguages.Count(); k++)
		{
		msg.Format(_L("\n%d\n"), aLanguages[k]);
		iConsole.Printf(msg);
		}	
	return (iConsole.Getch()-49); // Horrible but hey, this is test code and Epoc does not help!
	}
    
TInt SisUiAdapter::DisplayDriveL(const CAppInfo& /*aAppInfo*/, 
								 TInt64 /*aSize*/,
								 const RArray<TChar>& aDriveLetters,
								 const RArray<TInt64>& aDriveSpaces)
	{
	TBuf<128> msg;
	msg.Format(_L("\nChoose an installation drive:\n"));
	iConsole.Printf(msg);
	TInt letter;
	for (TInt k = 0; k < aDriveLetters.Count(); k++)
		{
        letter = aDriveLetters[k];
		msg.Format(_L("\n %d) Drive %c  --- Space Available = %d"), (k+1), letter, aDriveSpaces[k]);
		iConsole.Printf(msg);
		}
	msg.Format(_L("\n\n"));
	iConsole.Printf(msg);	
	return (iConsole.Getch() - 49);
	}

    	
TBool SisUiAdapter::DisplayUpgradeL(const CAppInfo& aAppInfo, 
									const CAppInfo& aExistingAppInfo)
	{
	TBuf<256> msg;
	_LIT(KTemplate, "Do you wish to upgrade %S by %S version %S to %S by %S version %S");
	TVersion version = aAppInfo.AppVersion();
	TVersion existingVersion = aExistingAppInfo.AppVersion();
	TDesC extAppname = aExistingAppInfo.AppName();
    TDesC extAppven = aExistingAppInfo.AppVendor();
    TDesC extAppver = existingVersion.Name() ;
 	TDesC appName = aAppInfo.AppName();
 	TDesC appVen = aAppInfo.AppVendor();
 	TDesC verName = version.Name();
 	msg.Format(KTemplate, &extAppname, &extAppven, &extAppver, &appName, &appVen,  &verName);	
	return AskQuestion(msg);
	}



TBool SisUiAdapter::DisplayQuestionL(const CAppInfo& /*aAppInfo*/, TQuestionDialog /*aQuestion*/, 
									 const TDesC& /*aDes*/)
	{
	_LIT(KMsg, "\nDisplayQuestionL implemented yet...\n");
	TBuf<80> msg;
	msg.Format(KMsg);
	iConsole.Printf(msg);
	return ETrue;
	}

		
TBool SisUiAdapter::DisplayOptionsL(const CAppInfo& /*aAppInfo*/,
									const RPointerArray<TDesC>& aOptions,
									RArray<TBool>& aSelections)
	{

	TBuf<128> msg;
	msg.Format(_L("\nChoose the installation options. Digit an option number followed by <enter> (0 to cancel, enter to continue):\n"));
	iConsole.Printf(msg);
	for (TInt k = 0; k < aOptions.Count(); k++)
		{
		msg.Format(_L("\n%d) %S\n"), (k+1), (aOptions[k]));
		iConsole.Printf(msg);
		aSelections[k] = EFalse;
		}	
	TInt choice = iConsole.Getch() - 49;
	while ((choice >= 0) && (choice < aOptions.Count()))
		{
		aSelections[choice]	= aSelections[choice] ? EFalse : ETrue;
		msg.Format(_L("\nChoose the installation options. Digit an option number followed by <enter> (0 to cancel, enter to continue):\n"));
		iConsole.Printf(msg);
		for (TInt k = 0; k < aOptions.Count(); k++)
			{
			msg.Format(_L("\n%d) %S "), (k+1), (aOptions[k]));
			iConsole.Printf(msg);
			if (aSelections[k])
				{
				msg.Format(_L("\tX\n"));
				}
			else
				{
				msg.Format(_L("\t-\n"));				
				}
			iConsole.Printf(msg);				
			}	
		choice = iConsole.Getch() - 49;				
		}
	if (choice == -1)
		{
		return EFalse;
		}
	return ETrue;
	}

    
TBool SisUiAdapter::HandleInstallEventL(const CAppInfo& /*aAppInfo*/,
									   TInstallEvent aEvent, 
									   TInt /*aValue*/, 
									   const TDesC& /*aDes*/)
	{
	TBuf<80> msg;
	switch (aEvent)
		{
	case EEventSetProgressBarFinalValue :
		{
		break;
		}
	case EEventUpdateProgressBar :
		{
		_LIT(KMsg, ".");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventAbortedInstall :
		{
		_LIT(KMsg, "\n***** !!!Installation Aborted :( !!! *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventAbortedUnInstall :
		{
		_LIT(KMsg, "\n***** !!!UNInstallation Aborted :( !!! *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;		
		}
	case EEventLeave :
		{
		_LIT(KMsg, "\n***** !!!UNHANDLED LEAVE :( !!! *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;		
		}		
	case EEventCompletedInstall :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Installation Completed :) ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventCompletedUnInstall :
		{
		_LIT(KMsg, "\n***** ^^^^^^ UNInstallation Completed :) ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventCompletedNoMessage :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Completed :) ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}		
	case EEventOcspCheckEnd :
		{
		_LIT(KMsg, "\n***** ^^^^^^ OCSP Check Completed :) ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventDevCert:
		{
		_LIT(KMsg, "\n***** ^^^^^^ !! Developer Certificate found !! :) ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;			
		}
	default :
		{
		_LIT(KMsg, "\n***** !!!!! UNKNOWN EVENT :( !!!!! *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
		}
	return ETrue;
	}

void SisUiAdapter::HandleCancellableInstallEventL(const CAppInfo& /*aAppInfo*/,
												  TInstallCancellableEvent aEvent, 
												  MCancelHandler& /*aCancelHandler*/,
												  TInt /*aValue*/, 
												  const TDesC& /*aDes*/)
	{
	TBuf<80> msg;
	switch (aEvent)
		{
	case EEventRemovingFiles :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Removing Files -- Stand by -- ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventShuttingDownApps :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Shutting down applications -- Stand by -- ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventCopyingFiles :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Copying Files -- Stand by -- ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	case EEventOcspCheckStart :
		{
		_LIT(KMsg, "\n***** ^^^^^^ Starting OCSP Checks -- Stand by -- ^^^^^^ *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
	default:
		{
		_LIT(KMsg, "\n***** !!!!! UNKNOWN CANCELLABLE EVENT :( !!!!! *****\n");
		msg.Format(KMsg);
		iConsole.Printf(msg);
		break;
		}
		}
	}
		
TBool SisUiAdapter::DisplaySecurityWarningL(const CAppInfo& /*aAppInfo*/,
		TSignatureValidationResult /*aSigVaidationResult*/,
		RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		TBool aInstallAnyway)
	{	
	TBuf<256> msg;
	if (aInstallAnyway)
		{
		_LIT(KMsg, "\nInstall Anyway");
		msg.Format(KMsg);
		return AskQuestion(msg);
		}
	else
		{
		return EFalse;
		}
	}

TBool SisUiAdapter::DisplayOcspResultL(const CAppInfo& /*aAppInfo*/,
		const TRevocationDialogMessage /*aMessage*/, 
		RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		const TBool aWarningOnly)
	{
	TBuf<256> msg;
	if (aWarningOnly)
		{
		_LIT(KMsg, "\nInstall Anyway");
		msg.Format(KMsg);
		return AskQuestion(msg);
		}
	else
		{
		return EFalse;
		}
	}
       
void SisUiAdapter::DisplayCannotOverwriteFileL(const CAppInfo& /*aAppInfo*/,
											   const CAppInfo& /*aInstalledAppInfo*/,
											   const TDesC& /*aFileName*/)
	{
	_LIT(KMsg, "\n DisplayCannotOverwriteFileL Dialog not implemented yet...\n");
	TBuf<80> msg;
	msg.Format(KMsg);
	iConsole.Printf(msg);
	}


TBool SisUiAdapter::DisplayUninstallL(const CAppInfo& aAppInfo)
	{
	TBuf<256> msg;
	_LIT(KTemplate, "Do you wish to UNinstall %S by %S version %S");
	TVersion version = aAppInfo.AppVersion();
	TDesC appName = aAppInfo.AppName();
 	TDesC appVendor =  aAppInfo.AppVendor();
    TDesC verName = version.Name();
 	msg.Format(KTemplate, &appName ,&appVendor, &verName);	
	return AskQuestion(msg);
	}


TBool SisUiAdapter::DisplayTextL(const CAppInfo& /*aAppInfo*/, 
								 TFileTextOption /*aOption*/,
								 const TDesC& /*aText*/)
	{
	_LIT(KMsg, "\n DisplayTextL Dialog not implemented yet...\n");
	TBuf<80> msg;
	msg.Format(KMsg);
	iConsole.Printf(msg);
	return ETrue;
	}

void SisUiAdapter::DisplayErrorL(const CAppInfo& /*aAppInfo*/,
								 TErrorDialog aType, 
								 const TDesC& aParam)
	{
	TBuf<256> msg;
	switch (aType)
		{
	case EUiAlreadyInRom :
		{
		_LIT(KMsg, "\n***** !!!!! CANNOT OVERWRITE A ROM COMPONENT :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiMissingDependency :
		{
		_LIT(KMsg, "\n***** !!!!! MISSING DEPENDENCY :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiRequireVer :
		{
		_LIT(KMsg, "\n***** !!!!! REQUIRED VERSION IS DIFFERENT FROM THE ONE PRESENT :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiRequireVerOrGreater :
		{
		_LIT(KMsg, "\n***** !!!!! REQUIRED VERSION NEWER THAN THE ONE PRESENT :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiFileCorrupt :
		{
		_LIT(KMsg, "\n***** !!!!! SISX FILE CORRUPT :( %S !!!!! *****\n");
//		_LIT(KMsgNoFile, "\n***** !!!!! SISX FILE CORRUPT :( !!!!! *****\n");
		if (aParam.Length())
			{
			msg.Format(KMsg, &aParam);
			}
		else
			{
			}
		break;
		}
	case EUiDiskNotPresent :
		{
		_LIT(KMsg, "\n***** !!!!! DISK NOT PRESENT :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiCannotRead :
		{
		_LIT(KMsg, "\n***** !!!!! CANNOT READ THE FILE :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiCannotDelete :
		{
		_LIT(KMsg, "\n***** !!!!! CANNOT DELETE THE FILE :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiInvalidFileName :
		{
		_LIT(KMsg, "\n***** !!!!! INVALID FILE NAME SPECIFIED :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiFileNotFound :
		{
		_LIT(KMsg, "\n***** !!!!! FILE NOT FOUND :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
	case EUiInsufficientSpaceOnDrive :
		{
		_LIT(KMsg, "\n***** !!!!! INSUFFICIENT SPACE :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;		
		}
	case EUiCapabilitiesCannotBeGranted :
		{
		_LIT(KMsg, "\n***** !!!!! CANNOT GRANT THE REQUIRED CAPABILITIES :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;		
		}
	case EUiUnknownFile :
		{
		_LIT(KMsg, "\n***** !!!!! CANNOT RECOGNIZE THE GIVEN SISX FILE :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;		
		}
	default :
		{
		_LIT(KMsg, "\n***** !!!!! UNKNOWN ERROR :( %S !!!!! *****\n");
		msg.Format(KMsg, &aParam);
		iConsole.Printf(msg);		
		break;
		}
		}
	}


TBool SisUiAdapter::DisplayDependencyBreakL(const CAppInfo& /*aAppInfo*/,
											const RPointerArray<TDesC>& /*aComponents*/)
	{
	_LIT(KMsg, "\n DisplayTextLDisplayDependencyBreakL not implemented yet...\n");
	TBuf<80> msg;
	msg.Format(KMsg);
	iConsole.Printf(msg);
	return ETrue;
	}


TBool SisUiAdapter::DisplayApplicationsInUseL(const CAppInfo& /*aAppInfo*/,
											  const RPointerArray<TDesC>& aAppNames)
	{
	_LIT(KMsg, "\nThe following applications must be shut down to proceed:\n");
	TBuf<256> msg;
	msg.Format(KMsg);
	iConsole.Printf(msg);

	for (TInt k = 0; k < aAppNames.Count(); k++)
		{
		iConsole.Printf(_L("\t"));
		iConsole.Printf(*aAppNames[k]);
		iConsole.Printf(_L("\n"));
		}	
	_LIT(KMsg2, "Do you wish to continue");
	msg.Format(KMsg2);
	return AskQuestion(msg);
	}

TBool SisUiAdapter::DisplayMissingDependencyL(
	const CAppInfo& /*aAppInfo*/,
	const TDesC& aDependencyName,
	TVersion aWantedVersionFrom,
	TVersion aWantedVersionTo,
	TVersion aInstalledVersion)
	{
	TBuf<512> msg;
	_LIT(KMsg, "\nMissing dependency:\n\t            Name: %S\n\t    From version: %u.%u.%u\n\t      To version: %u.%u.%u\n\tExisting version: %u.%u.%u\n\nDo you wish to continue?");
	msg.Format(KMsg,&aDependencyName,
		aWantedVersionFrom.iMajor,aWantedVersionFrom.iMinor,
			aWantedVersionFrom.iBuild,
		aWantedVersionTo.iMajor,aWantedVersionTo.iMinor,	aWantedVersionTo.iBuild,
		aInstalledVersion.iMajor,aInstalledVersion.iMinor,
			aInstalledVersion.iBuild);
	return AskQuestion(msg);
	}


TBool SisUiAdapter::AskQuestion(TDesC& aQuestion) 
	{
	TBuf<128> msg;
	_LIT(KQ, "%S (y/n) ?");
	msg.Format(KQ, &aQuestion);
	iConsole.Printf(msg);

	TChar answer = iConsole.Getch();
	if ((answer == 'y')||(answer == 'y')) 
		{
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
	}
