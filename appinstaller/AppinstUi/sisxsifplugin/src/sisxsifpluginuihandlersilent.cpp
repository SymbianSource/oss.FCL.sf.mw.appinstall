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
#include "sisxsifpluginerrors.h"            // Error codes

using namespace Usif;

#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x)
#define FLOG_1(x,y)     RDebug::Print((x),(y))
#define FLOG_2(x,y,z)   RDebug::Print((x),(y),(z))
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#endif


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
        Swi::TFileTextOption aOption, const TDesC& aText )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayText") );
	TBool okToContinue = EFalse;

	if( iInstallParams )
		{
		switch( iInstallParams->PackageInfo() )
			{
			case EAllowed:
				okToContinue = ETrue;
				break;
			case EUserConfirm:
			    switch( aOption )
			        {
			        case Swi::EInstFileTextOptionContinue:
			            ShowQuestionWithContinueL( aText );
                        okToContinue = ETrue;
			            break;
			        case Swi::EInstFileTextOptionSkipOneIfNo:
			            okToContinue = ShowQuestionL( aText );
			            break;
			        case Swi::EInstFileTextOptionAbortIfNo:
			        case Swi::EInstFileTextOptionExitIfNo:
			            okToContinue = ShowQuestionL( aText );
			            break;
			        case Swi::EInstFileTextOptionForceAbort:
			            ShowQuestionWithContinueL( aText );
			            break;
			        default:
			            break;
			        }
				break;
			case ENotAllowed:
			default:
		        SetErrorL( KErrPermissionDenied, ESifUiNeedsPackageInfoParameter, aText );
				break;
			}
		}

	FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayText, okToContinue=%d"), okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TErrorDialog aType, const TDesC& aParam )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayErrorL") );
    SetErrorSwiErrorL( aType, aParam );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aComponents*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL") );
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
			    SetErrorL( KErrPermissionDenied, ESifUiNeedsAllowAppBreakDependencyParameter );
				break;
			}
		}

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL") );
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
			    SetErrorL( KErrPermissionDenied, ESifUiNeedsAllowAppShutdownParameter );
				break;
			}
		}

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TQuestionDialog aQuestion, const TDesC& aDes )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayQuestionL") );
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
        	if( !okToContinue )
        	    {
                SetErrorL( KErrPermissionDenied, ESifUiNeedsAllowIncompatibleParameter, aDes );
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
        	if( !okToContinue )
        	    {
                SetErrorL( KErrPermissionDenied, ESifUiNeedsAllowOverwriteParameter, aDes );
                }
        	break;

        default:
            SetErrorL( KErrNotSupported, KErrNotSupported, aDes );
            break;
        }

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayQuestionL, okToContinue=%d"),
        okToContinue );
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
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayInstallL") );
    ASSERT( iOperationPhase == EInstalling );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/,
        const TCapabilitySet& /*aCapabilitySet*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL") );
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
			    SetErrorL( KErrPermissionDenied, ESifUiNeedsGrantCapabilitiesParameter );
				break;
			}
		}

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayGrantCapabilitiesL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayLanguageL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandlerSilent::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/,
        const RArray<TLanguage>& aLanguages )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayLanguageL") );
	TInt languageIndex = 0;
	TBool found = EFalse;
	TInt languageCount = aLanguages.Count();
	if( iInstallParams )
		{
		const RArray<TLanguage>& preferredLanguages = iInstallParams->Languages();
		for( TInt prefIndex = 0; prefIndex < preferredLanguages.Count() && !found; prefIndex++ )
		    {
	        TInt selectedIndex = aLanguages.Find( preferredLanguages[ prefIndex ] );
	        if( selectedIndex >= 0 && selectedIndex < languageCount )
	            {
	            languageIndex = selectedIndex;
	            found = ETrue;
	            }
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
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayLanguageL, languageIndex=%d"),
        languageIndex );
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
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayDriveL") );
	TInt driveIndex = 0;
	TBool found = EFalse;
	TInt driveCount = aDriveLetters.Count();
	if( iInstallParams )
		{
	    const RArray<TUint>& preferredDrives = iInstallParams->Drives();
		TChar driveLetter = 0;
		TIdentityRelation<TChar> relation(CompareDriveLetters);
		for( TInt prefIndex = 0; prefIndex < preferredDrives.Count() && !found; prefIndex++ )
		    {
		    RFs::DriveToChar( preferredDrives[ prefIndex ], driveLetter );
		    TInt index = aDriveLetters.Find( driveLetter, relation );
            if( index >= 0 && index < driveCount )
                {
                if( aDriveSpaces[ index ] > aSize )
                    {
                    driveIndex = index;
                    found = ETrue;
                    }
                }
		    }
		}
	if( !found )
		{
		driveIndex = KErrNotFound;
		}
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayDriveL, driveIndex=%d"),
        driveIndex );
    return driveIndex;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUpgradeL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayUpgradeL") );
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
			    // TODO: SetError
				break;
			}
		}
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayUpgradeL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayOptionsL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayOptionsL") );
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
			    // TODO: SetError
				break;
			}
		}
	for( TInt index = 0; index < aOptions.Count(); ++index )
		{
		aSelections[ index ] = allowOptions;
		}

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayOptionsL, allowOptions=%d"), allowOptions );
    return ETrue;		// always ok to continue
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::HandleInstallEventL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::HandleInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TInstallEvent aEvent, TInt aValue, const TDesC& /*aDes*/ )
    {
    FLOG_2( _L("CSisxSifPluginUiHandlerSilent::HandleInstallEventL, aEvent=%d, aValue=%d"),
        aEvent, aValue );
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
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL, aEvent=%d"),
        aEvent );
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
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL, aSigValidationResult=%d"),
        aSigValidationResult );
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
            FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL, aInstallAnyway=%d"),
                aInstallAnyway );
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
					    // TODO: SetError
						break;
					}
                }
            break;

        default:
            break;
        }

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL, okToContinue=%d"),
        okToContinue );
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
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayOcspResultL") );
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
                    // TODO: SetError
                    break;
                }
	        }
		}

	if( !okToContinue )
	    {
	    SetOcspErrorL( aMessage );
	    }

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayOcspResultL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/, const TDesC& aFileName )
    {
    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL, aFileName=%S"),
        &aFileName );
    // TODO: localized UI string needed: "The package tries to overwrite file '%1' it does not own."
	TName detailsString;
	detailsString.Format( _L("The package tries to overwrite file '%S' it does not own."), &aFileName );
    SetErrorL( KErrPermissionDenied, ESifUiCannotOverwriteFile, detailsString );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
        const TDesC& /*aDependencyName*/, TVersion /*aWantedVersionFrom*/,
        TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL") );
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
			    // TODO: SetError
				break;
			}
		}

	if( !okToContinue )
	    {
        SetErrorL( KErrSifMissingDependencies, 0, KNullDesC );
	    }

    FLOG_1( _L("CSisxSifPluginUiHandlerSilent::DisplayMissingDependencyL, okToContinue=%d"),
        okToContinue );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUninstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUninstallL( const Swi::CAppInfo& /*aAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayUninstallL") );
    ASSERT( iOperationPhase == EUninstalling );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayPreparingInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayPreparingInstallL( const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayPreparingInstallL") );
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayCompleteL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayCompleteL()
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayCompleteL") );
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayFailedL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayFailedL(
        const CSisxSifPluginErrorHandler& /*aError*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayFailedL") );
    // nothing displayed in silent mode
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::CancelDialogs()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::CancelDialogs()
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::CancelDialogs") );
    // nothing to do
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

