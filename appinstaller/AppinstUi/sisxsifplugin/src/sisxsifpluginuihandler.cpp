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
#include <sifui.h>                      // CSifUi
#include <sifuiappinfo.h>               // CSifUiAppInfo
#include <sifuicertificateinfo.h>       // CSifUiCertificateInfo
#include <bautils.h>                    // BaflUtils
#include <driveinfo.h>                  // DriveInfo
#include <featmgr.h>                    // FeatureManager
//#include <csxhelp/am.hlp.hrh>           // Help IDs

using namespace Usif;

const TInt KFreeSpaceTreshold = 128*1024;   // bytes

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
    iSelectableDrives.Close();
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

    iMode = EModeInstall;
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
        TInt64 /*aSize*/, const RArray<TChar>& aDriveLetters,
        const RArray<TInt64>& /*aDriveSpaces*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayDriveL") );

    TInt err = KErrNone;
    TInt driveNumber = EDriveC;
    if( iSifUi )
        {
        err = iSifUi->SelectedDrive( driveNumber );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginUiHandler::DisplayDriveL; SelectedDrive err=%d"), err );
            }
        }

    TChar driveLetter = 'C';
    err = RFs::DriveToChar( driveNumber, driveLetter );
    if( err )
        {
        FLOG_1( _L("CSisxSifPluginUiHandler::DisplayDriveL; DriveToChar err=%d"), err );
        }

    TInt index = aDriveLetters.Find( driveLetter );
    if( index >= 0 && index < aDriveLetters.Count() )
        {
        return index;
        }
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

    if( iSifUi->IsCancelled() )
        {
        return EFalse;
        }

    TSifOperationPhase phase = ( iMode == EModeInstall ? EInstalling : EUninstalling );
    switch( aEvent )
        {
        case Swi::EEventSetProgressBarFinalValue:
            iProgressBarFinalValue = aValue;
            if( iMode == EModeInstall )
                {
                CSifUiAppInfo *appInfo = GetAppInfoLC( aAppInfo );
                iSifUi->ShowProgressL( *appInfo, aValue );
                CleanupStack::PopAndDestroy( appInfo );
                }
            break;

        case Swi::EEventUpdateProgressBar:
            if( iMode == EModeInstall )
                {
                iSifUi->IncreaseProgressBarValueL( aValue );
                }
            PublishProgressL( phase, EFileOperation, aValue, iProgressBarFinalValue );
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
        RPointerArray<CPKIXValidationResultBase>& aPkixResults,
        RPointerArray<Swi::CCertificateInfo>& aCertificates,
        TBool aInstallAnyway )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplaySecurityWarningL") );
    TBool result = EFalse;

    if( iIsDriveSelectionRequired )
        {
        AddMemorySelectionL();
        }
    AddCertificatesL( aCertificates, aPkixResults );

    switch( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:
            {
            CSifUiAppInfo *appInfo = GetAppInfoLC( aAppInfo );
            result = iSifUi->ShowConfirmationL( *appInfo );
            CleanupStack::PopAndDestroy( appInfo );
            }
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
                CSifUiAppInfo *appInfo = GetAppInfoLC( aAppInfo );
                result = iSifUi->ShowConfirmationL( *appInfo );
                CleanupStack::PopAndDestroy( appInfo );
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
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool /*aWarningOnly*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayOcspResultL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/, const TDesC& /*aFileName*/ )
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

    iMode = EModeUninstall;
    return ETrue;       // uninstall is always silent
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayPreparingInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayPreparingInstallL( const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayPreparingInstallL") );
    // TODO: display preparing install
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCompleteL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCompleteL()
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayCompleteL") );

    iSifUi->ShowCompleteL();
    iMode = EModeUndefined;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayFailedL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayFailedL( TErrorCategory /*aCategory*/,
        TInt aErrorCode, const TDesC& aErrorMessage, const TDesC& /*aErrorDetails*/ )
    {
    FLOG_1( _L("CSisxSifPluginUiHandler::DisplayFailedL, aError=%d"), aErrorCode );

    // TODO: add error details
    iSifUi->ShowFailedL( aErrorCode, aErrorMessage );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::CSisxSifPluginUiHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler::CSisxSifPluginUiHandler( RFs& aFs ) :
        CSisxSifPluginUiHandlerBase( aFs )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::ConstructL()
    {
    iSifUi = CSifUi::NewL();
    iSelectionCache = CSisxSifUiSelectionCache::NewL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::AddMemorySelectionL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::AddMemorySelectionL()
    {
    TInt64 adjustedSize = iMaxInstalledSize + KFreeSpaceTreshold;

    TDriveList driveList;
    TInt driveCount = 0;
    TInt err = DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount );
    User::LeaveIfError( err );

    iSelectableDrives.Reset();
    TInt driveListLength = driveList.Length();
    for( TInt driveNumber = 0; driveNumber < driveListLength; driveNumber++ )
        {
        if( driveList[ driveNumber ] )
            {
            TUint driveStatus = 0;
            err = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus );
            if( !err && !( driveStatus & DriveInfo::EDriveRemote ) )
                {
                TVolumeInfo volumeInfo;
                err = iFs.Volume( volumeInfo, driveNumber );
                if( !err && volumeInfo.iFree > adjustedSize )
                    {
                    iSelectableDrives.Append( driveNumber );
                    }
                }
            }
        }
    iSifUi->SetMemorySelectionL( iSelectableDrives );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::AddCertificatesL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::AddCertificatesL(
        RPointerArray<Swi::CCertificateInfo>& aCertificates,
        RPointerArray<CPKIXValidationResultBase>& aPkixResults )
    {
    ASSERT( aCertificates.Count() == aPkixResults.Count() );

    RPointerArray<CSifUiCertificateInfo> certificates;
    CleanupResetAndDestroyPushL( certificates );

    for( TInt index = 0; index < aCertificates.Count(); ++index )
        {
        TValidationStatus status = aPkixResults[ index ]->Error();
        if( status.iReason == EValidatedOK )
            {
            CSifUiCertificateInfo* cert = CSifUiCertificateInfo::NewLC( *aCertificates[ index ] );
            certificates.AppendL( cert );
            CleanupStack::Pop( cert );
            }
        }
    iSifUi->SetCertificateInfoL( certificates );
    CleanupStack::PopAndDestroy( &certificates );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::GetAppInfoLC()
// ---------------------------------------------------------------------------
//
CSifUiAppInfo* CSisxSifPluginUiHandler::GetAppInfoLC( const Swi::CAppInfo& aAppInfo )
    {
    CSifUiAppInfo *appInfo = CSifUiAppInfo::NewLC( aAppInfo.AppName(), aAppInfo.AppVendor(),
            aAppInfo.AppVersion(), iMaxInstalledSize, iLogo );
    return appInfo;
    }

