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

#include "sisxsifpluginuihandler.h"     // CSisxSifPluginUiHandler
#include "sisxsifplugin.pan"            // Panic codes
#include "sisxsifcleanuputils.h"        // CleanupResetAndDestroyPushL
#include "sisxsifuiselectioncache.h"    // CSisxUISelectionCache
#include "sisxsifuilangname.h"          // CLangName
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <sisxsifuidata.rsg>            // Resource IDs
#include <sifui.h>                      // CSifUi
#include <bautils.h>                    // BaflUtils
#include <featmgr.h>                    // FeatureManager
//#include <csxhelp/am.hlp.hrh>           // Help IDs
#include <hb/hbcore/hbsymbiandevicedialog.h> // CHbDeviceDialog
#include <hb/hbcore/hbsymbianvariant.h> // CHbSymbianVariantMap

using namespace Usif;

_LIT( KSisxUIResourceFileName, "sisxsifuidata.rsc" );

// TODO: replace with proper tracing support
#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x);
#define FLOG_1(x,y)     RDebug::Print(x, y);
#define FLOG_2(x,y,z)   RDebug::Print(x, y, z);
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#endif


// ======== LOCAL FUNCTIONS =========

TBool HasLangId( const TLanguage* aId, const CLangName& aLang )
    {
    return( *aId == aLang.Id() );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler* CSisxSifPluginUiHandler::NewL( RFs& aFs )
	{
    FLOG( _L("CSisxSifPluginUiHandler::NewL") );
	CSisxSifPluginUiHandler *self = new( ELeave ) CSisxSifPluginUiHandler( aFs );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler()
	{
    FLOG( _L("CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler") );
    delete iSelectionCache;
    delete iSifUi;
    CloseResourceFile();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayTextL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayTextL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TFileTextOption /*aOption*/, const TDesC& /*aText*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayTextL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TErrorDialog /*aType*/, const TDesC& /*aParam*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayErrorL") );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aComponents*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayDependencyBreakL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayApplicationsInUseL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayApplicationsInUseL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TQuestionDialog aQuestion, const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayQuestionL") );

    TBool result = ETrue;
    switch( aQuestion )
        {
        case Swi::EQuestionIncompatible:
            if( !iQuestionIncompatibleDisplayed )
                {
                // TODO: show dialog
                iQuestionIncompatibleDisplayed = ETrue;
                }
            break;
        case Swi::EQuestionOverwriteFile:
        default:
            // silently accepted
            break;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayInstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayInstallL( const Swi::CAppInfo& /*aAppInfo*/,
        const CApaMaskedBitmap* /*aLogo*/,
        const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayInstallL") );

    iSifUi->SetMode( CSifUi::EInstalling );

    // TODO: show preparing note -- unless it can be displayed already earlier.
    // Preparing note should not have any buttons yet, but it might display some
    // application details already.

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/,
        const TCapabilitySet& /*aCapabilitySet*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayLanguageL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandler::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/,
        const RArray<TLanguage>& /*aLanguages*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayLanguageL") );

    return 0;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayDriveL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandler::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/,
        TInt64 /*aSize*/, const RArray<TChar>& /*aDriveLetters*/,
        const RArray<TInt64>& /*aDriveSpaces*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayDriveL") );

    return 0;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayUpgradeL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayUpgradeL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayOptionsL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aOptions*/, RArray<TBool>& /*aSelections*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayOptionsL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::HandleInstallEventL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::HandleInstallEventL( const Swi::CAppInfo& aAppInfo,
        Swi::TInstallEvent aEvent, TInt aValue, const TDesC& /*aDes*/ )
    {
    FLOG_2( _L("CSisxSifPluginUiHandler::HandleInstallEventL: aEvent %d, aValue %d"), aEvent, aValue );

    // TODO: application size?
    TInt appSize = 0;

    switch( aEvent )
        {
        case Swi::EEventSetProgressBarFinalValue:
            iSifUi->ShowProgressL( aAppInfo, appSize, aValue );
            break;

        case Swi::EEventUpdateProgressBar:
            iSifUi->IncreaseProgressBarValueL( aValue );
            break;

        case Swi::EEventOcspCheckEnd:
            // TODO: do something
            break;

        case Swi::EEventDevCert:
            // TODO: show "developer certificate" warning note
            break;

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
// CSisxSifPluginUiHandler::HandleCancellableInstallEventL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::HandleCancellableInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TInstallCancellableEvent /*aEvent*/, Swi::MCancelHandler& /*aCancelHandler*/,
        TInt /*aValue*/,const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::HandleCancellableInstallEventL") );

    // TODO: Swi::EEventOcspCheckStart
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplaySecurityWarningL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
        Swi::TSignatureValidationResult aSigValidationResult,
        RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
        RPointerArray<Swi::CCertificateInfo>& aCertificates,
        TBool aInstallAnyway )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplaySecurityWarningL") );
    TBool result = EFalse;

    // TODO: add application size
    TInt appSize = 0;

    switch( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:
            result = iSifUi->ShowConfirmationL( aAppInfo, appSize, iLogo, aCertificates );
            break;

        case Swi::ESignatureSelfSigned:
        case Swi::ENoCertificate:
        case Swi::ECertificateValidationError:
        case Swi::ESignatureNotPresent:
        case Swi::ESignatureCouldNotBeValidated:
        case Swi::ENoCodeSigningExtension:
        case Swi::ENoSupportedPolicyExtension:
        case Swi::EMandatorySignatureMissing:
            if( aInstallAnyway )
                {
                result = iSifUi->ShowConfirmationL( aAppInfo, appSize, iLogo, aCertificates );
                }
            break;

        default:
            break;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayOcspResultL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TRevocationDialogMessage /*aMessage*/, RPointerArray<TOCSPOutcome>& /*aOutcomes*/,
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,TBool /*aWarningOnly*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayOcspResultL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/,const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL") );

    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayMissingDependencyL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
        const TDesC& /*aDependencyName*/, TVersion /*aWantedVersionFrom*/,
        TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayMissingDependencyL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayUninstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayUninstallL( const Swi::CAppInfo& /*aAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayUninstallL") );

    iSifUi->SetMode( CSifUi::EUninstalling );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCompleteL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCompleteL()
    {
    if( iSifUi )
        {
        iSifUi->ShowCompleteL();
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayFailedL
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayFailedL( TInt aErrorCode )
    {
    if( iSifUi )
        {
        iSifUi->ShowFailedL( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::CSisxSifPluginUiHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler::CSisxSifPluginUiHandler( RFs& aFs ) : iFs( aFs )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::ConstructL()
    {
    OpenResourceFileL();
    iSifUi = CSifUi::NewL();
    iSelectionCache = CSisxSifUiSelectionCache::NewL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::OpenResourceFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::OpenResourceFileL()
    {
    TFileName fileName;
    fileName.Copy( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KSisxUIResourceFileName );
    BaflUtils::NearestLanguageFile( iFs, fileName );
    iResourceFile.OpenL( iFs, fileName );
    iResourceFile.ConfirmSignatureL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ReadStringResourceL()
// ---------------------------------------------------------------------------
//
HBufC* CSisxSifPluginUiHandler::ReadStringResourceL( TInt aResourceId )
    {
    HBufC8* buffer = iResourceFile.AllocReadLC( aResourceId );
    TResourceReader reader;
    reader.SetBuffer( buffer );
    const TPtrC ptr( reader.ReadTPtrC() );
    HBufC* string = ptr.AllocL();
    CleanupStack::PopAndDestroy( buffer );
    return string;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::CloseResourceFile()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::CloseResourceFile()
    {
    iResourceFile.Close();
    }

