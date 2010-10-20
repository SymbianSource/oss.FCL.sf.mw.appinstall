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
* Description:  Installation parameters class.
*
*/

#include "sisxsifplugininstallparams.h" // CSisxSifPluginInstallParams
#include <f32file.h>                    // EDriveC

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


// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginInstallParams* CSisxSifPluginInstallParams::NewL(
        const COpaqueNamedParams& aParams )
    {
    CSisxSifPluginInstallParams* self = new ( ELeave ) CSisxSifPluginInstallParams;
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginInstallParams* CSisxSifPluginInstallParams::NewL(
        const CSisxSifPluginInstallParams& aParams )
    {
    CSisxSifPluginInstallParams* self = new ( ELeave ) CSisxSifPluginInstallParams;
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::~CSisxSifPluginInstallParams()
// ---------------------------------------------------------------------------
//
CSisxSifPluginInstallParams::~CSisxSifPluginInstallParams()
    {
    iDrives.Close();
    iLanguages.Close();
    delete iOCSPUrl;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::IsSilentMode()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginInstallParams::IsSilentMode() const
    {
    return iUseSilentMode;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::IsInstallInactive()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginInstallParams::IsInstallInactive() const
    {
    return iIsInstallInactive;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::Drives()
// ---------------------------------------------------------------------------
//
const RArray<TUint>& CSisxSifPluginInstallParams::Drives() const
    {
    return iDrives;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::Languages()
// ---------------------------------------------------------------------------
//
const RArray<TLanguage>& CSisxSifPluginInstallParams::Languages() const
    {
    return iLanguages;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::OCSPUrl()
// ---------------------------------------------------------------------------
//
const TDesC& CSisxSifPluginInstallParams::OCSPUrl() const
    {
    if( iOCSPUrl )
        {
        return *iOCSPUrl;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::PerformOCSP()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::PerformOCSP() const
    {
    return iPerformOCSP;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::IgnoreOCSPWarnings()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::IgnoreOCSPWarnings() const
    {
    return iIgnoreOCSPWarnings;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowUpgrade()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowUpgrade() const
    {
    return iAllowUpgrade;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::InstallOptionalItems()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::InstallOptionalItems() const
    {
    return iInstallOptionalItems;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowUntrusted()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowUntrusted() const
    {
    return iAllowUntrusted;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::GrantCapabilities()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::GrantCapabilities() const
    {
    return iGrantCapabilities;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowAppShutdown()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowAppShutdown() const
    {
    return iAllowAppShutdown;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowAppBreakDependency()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowAppBreakDependency() const
    {
    return iAllowAppBreakDependency;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowOverwrite()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowOverwrite() const
    {
    return iAllowOverwrite;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::PackageInfo()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::PackageInfo() const
    {
    return iPackageInfo;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::AllowIncompatible()
// ---------------------------------------------------------------------------
//
TSifPolicy CSisxSifPluginInstallParams::AllowIncompatible() const
    {
    return iAllowIncompatible;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::CSisxSifPluginInstallParams()
// ---------------------------------------------------------------------------
//
CSisxSifPluginInstallParams::CSisxSifPluginInstallParams() : iAllowUntrusted( ENotAllowed )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::ConstructL( const COpaqueNamedParams& aParams )
    {
    FLOG( _L("CSisxSifPluginInstallParams::ConstructL, begin") );
    iUseSilentMode = GetIntParam( aParams, KSifInParam_InstallSilently, EFalse );
    iIsInstallInactive = GetIntParam( aParams, KSifInParam_InstallInactive, EFalse );
    TRAPD( err, DoProcessDriveParamL( aParams ) );
    if( err )
        {
        TInt defaultDrive = GetIntParam( aParams, KSifInParam_Drive, EDriveC );
        iDrives.AppendL( defaultDrive );
        }
    TRAP_IGNORE( DoProcessLangParamL( aParams ) );  // no default language
    GetStringParamL( aParams, KSifInParam_OCSPUrl, iOCSPUrl );
    GetPolicyParam( aParams, KSifInParam_PerformOCSP, iPerformOCSP, EAllowed );
    GetPolicyParam( aParams, KSifInParam_IgnoreOCSPWarnings, iIgnoreOCSPWarnings, EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowUpgrade, iAllowUpgrade, EAllowed );
    GetPolicyParam( aParams, KSifInParam_InstallOptionalItems, iInstallOptionalItems, EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowUntrusted, iAllowUntrusted, ENotAllowed );
    GetPolicyParam( aParams, KSifInParam_GrantCapabilities, iGrantCapabilities, EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowAppShutdown, iAllowAppShutdown, EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowAppBreakDependency, iAllowAppBreakDependency,
            EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowOverwrite, iAllowOverwrite, EAllowed );
    GetPolicyParam( aParams, KSifInParam_PackageInfo, iPackageInfo, EAllowed );
    GetPolicyParam( aParams, KSifInParam_AllowIncompatible, iAllowIncompatible, EAllowed );
    FLOG( _L("CSisxSifPluginInstallParams::ConstructL, end") );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::ConstructL( const CSisxSifPluginInstallParams& aParams )
    {
    FLOG( _L("CSisxSifPluginInstallParams::ConstructL") );
	iUseSilentMode = aParams.iUseSilentMode;
	FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iUseSilentMode=%d"),
	    iUseSilentMode );
	iIsInstallInactive = aParams.iIsInstallInactive;
	FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iIsInstallInactive=%d"),
	    iIsInstallInactive );
	for( TInt index = 0; index < aParams.iDrives.Count(); index++ )
	    {
	    iDrives.AppendL( aParams.iDrives[ index ] );
        FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iDrives %d"),
            aParams.iDrives[ index ] );
	    }
	for( TInt index = 0; index < aParams.iLanguages.Count(); index++ )
	    {
	    iLanguages.AppendL( aParams.iLanguages[ index ] );
        FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iLanguages %d"),
            aParams.iLanguages[ index ] );
	    }
    if( aParams.iOCSPUrl )
        {
        iOCSPUrl = aParams.iOCSPUrl->AllocL();
        FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iOCSPUrl '%S'"), iOCSPUrl );
        }
    iPerformOCSP = aParams.iPerformOCSP;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iPerformOCSP=%d"),
        iPerformOCSP );
    iIgnoreOCSPWarnings = aParams.iIgnoreOCSPWarnings;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iIgnoreOCSPWarnings=%d"),
        iIgnoreOCSPWarnings );
    iAllowUpgrade = aParams.iAllowUpgrade;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowUpgrade=%d"),
        iAllowUpgrade );
    iInstallOptionalItems = aParams.iInstallOptionalItems;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iInstallOptionalItems=%d"),
        iInstallOptionalItems );
    iAllowUntrusted = aParams.iAllowUntrusted;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowUntrusted=%d"),
        iAllowUntrusted );
    iGrantCapabilities = aParams.iGrantCapabilities;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iGrantCapabilities=%d"),
        iGrantCapabilities );
    iAllowAppShutdown = aParams.iAllowAppShutdown;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowAppShutdown=%d"),
        iAllowAppShutdown );
    iAllowAppBreakDependency = aParams.iAllowAppBreakDependency;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowAppBreakDependency=%d"),
        iAllowAppBreakDependency );
    iAllowOverwrite = aParams.iAllowOverwrite;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowOverwrite=%d"),
        iAllowOverwrite );
    iPackageInfo = aParams.iPackageInfo;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iPackageInfo=%d"),
        iPackageInfo );
    iAllowIncompatible = aParams.iAllowIncompatible;
    FLOG_1( _L("CSisxSifPluginInstallParams::ConstructL, iAllowIncompatible=%d"),
        iAllowIncompatible );
    FLOG( _L("CSisxSifPluginInstallParams::ConstructL, end") );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::GetIntParam()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginInstallParams::GetIntParam( const COpaqueNamedParams& aParams,
        const TDesC& aParamName, TInt aDefaultValue )
    {
    TInt value = aDefaultValue;
    TBool paramFound = EFalse;
    TRAPD( err, paramFound = aParams.GetIntByNameL( aParamName, value ) );
    if( !err && paramFound )
        {
        FLOG_2( _L("CSisxSifPluginInstallParams::GetIntParam, %S=%d"),
            &aParamName, value );
        return value;
        }
    return aDefaultValue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::GetPolicyParam()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::GetPolicyParam( const COpaqueNamedParams& aParams,
        const TDesC& aParamName, TSifPolicy& aPolicy, const TSifPolicy& aDefault )
    {
    TInt value = aDefault;
    TBool paramFound = EFalse;
    TRAPD( err, paramFound = aParams.GetIntByNameL( aParamName, value ) );
    if( !err && paramFound )
        {
        FLOG_2( _L("CSisxSifPluginInstallParams::GetPolicyParam, %S=%d"),
            &aParamName, aPolicy );
        aPolicy = static_cast<TSifPolicy>( value );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::GetStringParamL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::GetStringParamL( const COpaqueNamedParams& aParams,
        const TDesC& aParamName, HBufC*& aBuf )
    {
    TPtrC value( aParams.StringByNameL( aParamName ) );
    if( value.Length() )
        {
        if( aBuf )
            {
            delete aBuf;
            aBuf = NULL;
            }
        aBuf = value.AllocL();
        FLOG_2( _L("CSisxSifPluginInstallParams::GetStringParamL, %S='%S'"),
            &aParamName, aBuf );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::DoProcessDriveParamL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::DoProcessDriveParamL( const COpaqueNamedParams& aParams )
    {
    const RArray<TInt>& driveArray = aParams.IntArrayByNameL( KSifInParam_Drive );
    iDrives.Reset();
    for( TInt index = 0; index < driveArray.Count(); index++ )
        {
        FLOG_1( _L("CSisxSifPluginInstallParams::DoProcessDriveParamL, drive %d"),
            driveArray[ index ] );
        iDrives.AppendL( driveArray[ index ] );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginInstallParams::DoProcessLangParamL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginInstallParams::DoProcessLangParamL( const COpaqueNamedParams& aParams )
    {
    const RArray<TInt>& langArray = aParams.IntArrayByNameL( KSifInParam_Languages );
    iLanguages.Reset();
    for( TInt index = 0; index < langArray.Count(); index++ )
        {
        FLOG_1( _L("CSisxSifPluginInstallParams::DoProcessLangParamL, lang %d"),
            langArray[ index ] );
        iLanguages.AppendL( static_cast<TLanguage>( langArray[ index ] ) );
        }
    }

