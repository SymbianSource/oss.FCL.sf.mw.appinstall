/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Native SISX SIF (Software Install Framework) plugin.
*
*/

#include "sisxsifpluginuihandlersilent.h"   // CSisxSifPluginUiHandlerSilent
#include "sisxsifplugininstallparams.h"     // CSisxSifPluginInstallParams
#include "sisxsifpluginerrorhandler.h"      // CSisxSifPluginErrorHandler
#include "sisxsifplugin.pan"                // Panic codes
#include "sisxsifcleanuputils.h"            // CleanupResetAndDestroyPushL

using namespace Usif;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerSilent* CSisxSifPluginUiHandlerSilent::NewL( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler )
    {
    CSisxSifPluginUiHandlerSilent *self = new( ELeave ) CSisxSifPluginUiHandlerSilent(
            aFs, aErrorHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::~CSisxSifPluginUiHandlerSilent()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerSilent::~CSisxSifPluginUiHandlerSilent()
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayTextL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayTextL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TFileTextOption /*aOption*/, const TDesC& /*aText*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->PackageInfo() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
				// TODO: EPolicyUserConfirm
				break;
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TErrorDialog aType, const TDesC& aParam )
    {
    SetDisplayErrorL( aType, aParam );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aComponents*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->AllowAppBreakDependency() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aAppNames*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->AllowAppShutdown() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TQuestionDialog aQuestion, const TDesC& /*aDes*/ )
    {
	TBool okToContinue = EFalse;
    switch( aQuestion )
        {
        case Swi::EQuestionIncompatible:
        	if( iInstallParams )
        		{
				switch( iInstallParams->AllowIncompatible() )
					{
					case EAllowed:
						okToContinue = ETrue;
						break;
                    case EUserConfirm:
					case ENotAllowed:
					default:
						break;
					}
        		}
            break;

        case Swi::EQuestionOverwriteFile:
        	if( iInstallParams )
        		{
				switch( iInstallParams->AllowOverwrite() )
					{
					case EAllowed:
						okToContinue = ETrue;
						break;
					case EUserConfirm:
					case ENotAllowed:
					default:
						break;
					}
        		}
        	break;

        default:
            break;
        }
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayInstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayInstallL( const Swi::CAppInfo& /*aAppInfo*/,
        const CApaMaskedBitmap* /*aLogo*/,
        const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    iOperationPhase = EInstalling;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/,
        const TCapabilitySet& /*aCapabilitySet*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->GrantCapabilities() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayLanguageL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandlerSilent::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/,
        const RArray<TLanguage>& aLanguages )
    {
	TInt languageIndex = 0;
	TBool found = EFalse;
	TInt languageCount = aLanguages.Count();
	if( iInstallParams )
		{
		TLanguage lang = iInstallParams->Language();
		TInt index = aLanguages.Find( lang );
		if( index >= 0 && index < languageCount )
			{
			languageIndex = index;
			found = ETrue;
			}
		}
	if( !found )
		{
		TLanguage lang = User::Language();
		TInt index = aLanguages.Find( lang );
		if( index > 0 && index < languageCount )
			{
			languageIndex = index;
			}
		}
    return languageIndex;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayDriveL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandlerSilent::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/,
        TInt64 aSize, const RArray<TChar>& aDriveLetters,
        const RArray<TInt64>& aDriveSpaces )
    {
	TInt driveIndex = 0;
	TBool found = EFalse;
	TInt driveCount = aDriveLetters.Count();
	if( iInstallParams )
		{
		TChar driveLetter = 0;
		RFs::DriveToChar( iInstallParams->Drive(), driveLetter );
		TInt index = aDriveLetters.Find( driveLetter );
		if( index >= 0 && index < driveCount )
			{
			if( aDriveSpaces[ index ] > aSize )
				{
				driveIndex = index;
				found = ETrue;
				}
			}
		}
	// TODO: should there be some default drive?
	if( !found )
		{
		User::Leave( KErrNoMemory );
		}
    return driveIndex;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUpgradeL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->AllowUpgrade() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayOptionsL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections )
    {
	TBool allowOptions = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->InstallOptionalItems() )
			{
			case EAllowed:
				allowOptions = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
	for( TInt index = 0; index < aOptions.Count(); ++index )
		{
		aSelections[ index ] = allowOptions;
		}
    return ETrue;		// always ok to continue
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::HandleInstallEventL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::HandleInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TInstallEvent aEvent, TInt aValue, const TDesC& /*aDes*/ )
    {
    switch( aEvent )
        {
        case Swi::EEventSetProgressBarFinalValue:
            iProgressBarFinalValue = aValue;
            break;

        case Swi::EEventUpdateProgressBar:
            iProgressBarCurrentValue += aValue;
            PublishProgressL( EFileOperation );
            break;

        case Swi::EEventDevCert:
        case Swi::EEventOcspCheckEnd:
        case Swi::EEventAbortedInstall:
        case Swi::EEventAbortedUnInstall:
        case Swi::EEventCompletedInstall:
        case Swi::EEventCompletedUnInstall:
        case Swi::EEventCompletedNoMessage:
        case Swi::EEventLeave:
        default:
            break;
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL(
        const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallCancellableEvent aEvent,
        Swi::MCancelHandler& /*aCancelHandler*/, TInt /*aValue*/, const TDesC& /*aDes*/ )
    {
    switch( aEvent )
        {
        case Swi::EEventOcspCheckStart:
            PublishProgressL( EOCSPCheck );
            break;

        case Swi::EEventRemovingFiles:
        case Swi::EEventCopyingFiles:
        case Swi::EEventShuttingDownApps:
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TSignatureValidationResult aSigValidationResult,
        RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,
        TBool aInstallAnyway )
    {
    TBool okToContinue = EFalse;

    switch( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:
        	okToContinue = ETrue;
            break;

        // TODO: check these, now same functionality as before
        case Swi::ESignatureSelfSigned:
        case Swi::ENoCertificate:
        case Swi::ECertificateValidationError:
        case Swi::ESignatureNotPresent:
        case Swi::ESignatureCouldNotBeValidated:
        case Swi::ENoCodeSigningExtension:
        case Swi::ENoSupportedPolicyExtension:
        case Swi::EMandatorySignatureMissing:
            if( aInstallAnyway && iInstallParams )
                {
				switch( iInstallParams->AllowUntrusted() )
					{
					case EAllowed:
						okToContinue = ETrue;
						break;
					case EUserConfirm:
					case ENotAllowed:
					default:
						break;
					}
                }
            break;

        default:
            break;
        }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayOcspResultL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TRevocationDialogMessage aMessage, RPointerArray<TOCSPOutcome>& /*aOutcomes*/,
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool aWarningOnly )
    {
	TBool okToContinue = EFalse;

	if( aWarningOnly && !IsOcspMandatoryL() )
		{
	    if( iInstallParams )
	        {
            switch( iInstallParams->IgnoreOCSPWarnings() )
                {
                case EAllowed:
                    okToContinue = ETrue;
                    break;
                case EUserConfirm:
                case ENotAllowed:
                default:
                    break;
                }
	        }
		}

	if( !okToContinue )
	    {
	    SetOcspErrorL( aMessage );
	    }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/, const TDesC& /*aFileName*/ )
    {
	// TODO: error handling
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
        const TDesC& /*aDependencyName*/, TVersion /*aWantedVersionFrom*/,
        TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/ )
    {
	TBool okToContinue = EFalse;
	if( iInstallParams )
		{
		switch( iInstallParams->AllowAppBreakDependency() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			case ENotAllowed:
			default:
				break;
			}
		}
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUninstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUninstallL( const Swi::CAppInfo& /*aAppInfo*/ )
    {
    iOperationPhase = EUninstalling;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayPreparingInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayPreparingInstallL( const TDesC& /*aFileName*/ )
    {
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayCompleteL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayCompleteL()
    {
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayFailedL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayFailedL(
        const CSisxSifPluginErrorHandler& /*aError*/ )
    {
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::CSisxSifPluginUiHandlerSilent()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerSilent::CSisxSifPluginUiHandlerSilent( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler ) :
        CSisxSifPluginUiHandlerBase( aFs, aErrorHandler )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::ConstructL()
    {
    }

