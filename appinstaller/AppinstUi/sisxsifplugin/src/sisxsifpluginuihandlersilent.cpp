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

#include "sisxsifpluginuihandlersilent.h"   // CSisxSifPluginUiHandlerSilentSilent
#include "sisxsifplugin.pan"                // Panic codes
#include "sisxsifcleanuputils.h"            // CleanupResetAndDestroyPushL

using namespace Usif;

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
// CSisxSifPluginUiHandlerSilent::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerSilent* CSisxSifPluginUiHandlerSilent::NewL( RFs& aFs )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::NewL") );
    CSisxSifPluginUiHandlerSilent *self = new( ELeave ) CSisxSifPluginUiHandlerSilent( aFs );
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
    FLOG( _L("CSisxSifPluginUiHandlerSilent::~CSisxSifPluginUiHandlerSilent") );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayTextL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayTextL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TFileTextOption /*aOption*/, const TDesC& /*aText*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayTextL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TErrorDialog /*aType*/, const TDesC& /*aParam*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayErrorL") );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aComponents*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayDependencyBreakL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayApplicationsInUseL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TQuestionDialog aQuestion, const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayQuestionL") );

    TBool result = ETrue;
    switch( aQuestion )
        {
        case Swi::EQuestionIncompatible:
            break;
        case Swi::EQuestionOverwriteFile:
        default:
            break;
        }
    return result;
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

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayLanguageL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandlerSilent::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/,
        const RArray<TLanguage>& /*aLanguages*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayLanguageL") );

    return 0;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayDriveL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandlerSilent::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/,
        TInt64 /*aSize*/, const RArray<TChar>& /*aDriveLetters*/,
        const RArray<TInt64>& /*aDriveSpaces*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayDriveL") );

    return 0;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUpgradeL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayUpgradeL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayOptionsL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aOptions*/, RArray<TBool>& /*aSelections*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayOptionsL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::HandleInstallEventL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::HandleInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TInstallEvent /*aEvent*/, TInt /*aValue*/, const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::HandleInstallEventL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL(
        const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallCancellableEvent /*aEvent*/,
        Swi::MCancelHandler& /*aCancelHandler*/, TInt /*aValue*/,const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::HandleCancellableInstallEventL") );
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
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplaySecurityWarningL") );
    TBool result = EFalse;

    switch( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:
            result = ETrue;
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
                result = ETrue;
                }
            break;

        default:
            break;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayOcspResultL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TRevocationDialogMessage /*aMessage*/, RPointerArray<TOCSPOutcome>& /*aOutcomes*/,
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,TBool /*aWarningOnly*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayOcspResultL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/,const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayCannotOverwriteFileL") );
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

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::DisplayUninstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerSilent::DisplayUninstallL( const Swi::CAppInfo& /*aAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandlerSilent::DisplayUninstallL") );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::CSisxSifPluginUiHandlerSilent()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerSilent::CSisxSifPluginUiHandlerSilent( RFs& aFs ) : iFs( aFs )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerSilent::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerSilent::ConstructL()
    {
    }

