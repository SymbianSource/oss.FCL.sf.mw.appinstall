/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of CNcdDeviceService
*
*/


#include "ncddeviceserviceimpl.h"

#include <eikenv.h>
#include <ProEngFactory.h> // Set as ringing tone
#include <MProEngEngine.h> // Set as ringing tone
#include <MProEngProfile.h> // Set as ringing tone
#include <MProEngTones.h> // Set as ringing tone
#include <AknsWallpaperUtils.h> // Set as wallpaper
#include <centralrepository.h> // Set as theme
#include <AknSkinsInternalCRKeys.h> // Set as theme
#include <etel.h> // MCC and MNC
#include <etelmm.h> // MCC and MNC
#include <sysutil.h>
#include <etel3rdparty.h>
#include <f32file.h>
#include <apgcli.h>

#if !defined( __SERIES60_31__ ) && !defined( __SERIES60_30__ )
	#include <sysversioninfo.h>
#endif

#ifdef GET_DEVICE_ID_FROM_USERAGENT
    #include <sysversioninfo.h>
#endif

#include "catalogs_device_config.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    #include "ncdtestconfig.h"
#endif    

// Make ifdeffing easier by creating a new define for product code support
#if defined( __SERIES60_31__ ) || defined( __SERIES60_30__ )
    #define NCD_PRODUCTCODE_NOT_SUPPORTED
#endif

#if !( defined( __WINS__ ) || defined( __SERIES60_31__ ) || defined( __SERIES60_30__ ) )
    #define NCD_GIVE_EXTRA_FIRMWARE_DATA
#endif

_LIT( KPhoneTsy, "PHONETSY.TSY" );

// This string is used to find Series 60 version from User Agent string
//_LIT8( KSeries60VersionFinder, "Series60/" );

// Note: This must be a request that is not used in device interaction
const CTelephony::TCancellationRequest KNoPhoneOperationRunning =
    CTelephony::EFlightModeChangeCancel;

const TInt KFirmwareLength = 64;

// Hardcoded device manufacturer string
// <hardware><manufacturer>
_LIT( KManufacturerNokia, "Nokia" );

_LIT8( KFlashMovieMime, "application/x-shockwave-flash" );

#ifdef __WINS__
_LIT( KWinsProductCode, "0000000" );
#endif


#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA
_LIT( KFirmwareEol, "\n" );
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdDeviceService* CNcdDeviceService::NewL()
    {
    CNcdDeviceService* self = NewLC();
    CleanupStack::Pop();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdDeviceService* CNcdDeviceService::NewLC()
    {
    CNcdDeviceService* self = new (ELeave) CNcdDeviceService();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CNcdDeviceService::~CNcdDeviceService()
    {
    DLTRACEIN((""));
    delete iCurrentThemeName;
    Cancel();
    delete iImsi;
    delete iFirmware;
    delete iDeviceManufacturer;
    delete iDeviceModel;
    Close();
    delete iTelephony;
    
#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    delete iTestConfig;
#endif     
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdDeviceService::CNcdDeviceService() : CActive( EPriorityStandard ),
    iPhoneIdV1Pckg( iPhoneIdV1 ), 
    iSubscriberIdV1Pckg( iSubscriberIdV1 ),
    iNetworkInfoV1Pckg( iNetworkInfoV1 ), 
    iConnected( EFalse ),
    iCurrentCancelCode( KNoPhoneOperationRunning ),
    iError( KErrNone )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::ConstructL()
    {
    DLTRACEIN((""));
    iTelephony = CTelephony::NewL();
    InitializeL();
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//    
void CNcdDeviceService::RunL()
    {
    DLTRACEIN(( "iStatus: %d", iStatus.Int() ));
    DLINFO(( "iState: %d", iState ));
    
    // GetSubscriberId fails with KErrNotFound if there's no SIM in the device
    switch ( iState ) 
        {
        case EGetPhoneId:
            {            
            DLTRACE((_L("manufacturer: %S, model: %S, IMEI: %S"),
                &iPhoneIdV1.iManufacturer, &iPhoneIdV1.iModel, 
                &iPhoneIdV1.iSerialNumber ));
            GetSubscriberId();    
            break;
            }
        
        case EGetSubscriberId:
            {
            if ( iStatus.Int() == KErrNone ||
                 iStatus.Int() == KErrNotFound ||
                 iStatus.Int() == KErrNotReady ) 
                {
                
                DLTRACE(("Got subscriber ID"));
                DLINFO(( _L("IMSI: %S"), &iSubscriberIdV1.iSubscriberId ))                
                GetNetworkInfo();
                }
            else 
                {
                DLTRACE(("Unrecoverable error"));
                iError = iStatus.Int();
                }
            break;                        
            }
            
        case EGetNetworkInfo:
            {
            DLTRACE(("EGetNetworkInfo"));
            if ( iStatus.Int() == KErrAccessDenied ) 
                {
                DLTRACE(("Couldn't access network information, probably SIM is not present"));
                FinishInitialization();
                break;
                }
            else if ( iStatus.Int() != KErrNone )
                {
                DLTRACE(("Unrecoverable error"));
                iError = iStatus.Int();
                break;
                }
                
            DLTRACE(("Got network info"));
            // All is well
            DLINFO(( _L("Mode: %d, Status: %d, MCC: %S, MNC: %S, DisplayTag: %S"),
                iNetworkInfoV1.iMode, iNetworkInfoV1.iStatus, 
                &iNetworkInfoV1.iCountryCode,
                &iNetworkInfoV1.iNetworkId,
                &iNetworkInfoV1.iDisplayTag ));
            DLINFO(( _L("Short: %S, Long: %S, Access: %d, AreaKnown: %d"),
                &iNetworkInfoV1.iShortName, &iNetworkInfoV1.iLongName,
                iNetworkInfoV1.iAccess, iNetworkInfoV1.iAreaKnown ));
            
            if ( iNetworkInfoV1.iAreaKnown ) 
                {
                DLINFO(( "LocationAreaCode: %u, CellId: %u", 
                    iNetworkInfoV1.iLocationAreaCode,
                    iNetworkInfoV1.iCellId ));                        
                }
            else 
                {
                DLINFO(( "Area not known" ));
                }
                
            FinishInitialization();
            break;
            }
                        
        case EInitialized:
            {
            DLTRACE(("Initialization done"));
            FinishInitialization();
            break;
            }
        
        }


    if ( iError != KErrNone )
        {
        DLTRACE(("Error: %d", iError));        
        DeleteWait();
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::DoCancel()
    {
    DLTRACEIN((""));
    DeleteWait();
    if ( iTelephony && iCurrentCancelCode != KNoPhoneOperationRunning ) 
        {     
        
        iTelephony->CancelAsync( iCurrentCancelCode ) ;
        }
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CNcdDeviceService::RunError( TInt aError )
    {    
    DLERROR((" Silently ignoring an error: %d", aError ));
    iError = aError;
    DeleteWait();
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// Set sound file as ringing tone to the currently active profile.
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::SetAsRingingToneL( const TDesC& aFileName )
    {
    DLTRACEIN((""));
    
    MProEngEngine* engine = ProEngFactory::NewEngineLC();
    MProEngProfile* profile = engine->ActiveProfileLC();
    MProEngTones& tones = profile->ProfileTones();
    
    User::LeaveIfError( tones.SetRingingTone1L( aFileName ) );
    profile->CommitChangeL();
    
    DLINFO(("Ringing tone set"));
    
    // Can't use PopAndDestroy( 2, MProEngEngine ) etc. because
    // ProEngFactory puts a C-class in the cleanupstack but returns an M-class
    // so the pointer comparison done by PopAndDestroy will fail and cause
    // a panic with E32User-CBase 90
    CleanupStack::PopAndDestroy( 2 ); // MProEngProfile, MProEngEngine    
        
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Set graphics file as wallpaper.
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::SetAsWallpaperL( const TDesC& aFileName )
    {
    DLTRACEIN((""));
    User::LeaveIfError(
        AknsWallpaperUtils::SetIdleWallpaper( aFileName,
                                              CCoeEnv::Static() ) );
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Set theme by using its name.
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::SetAsThemeL( const TDesC& aThemeName )
    {
    DLTRACEIN((""));
    RAknsSrvSession aknsSrv;
    CleanupClosePushL( aknsSrv );
    User::LeaveIfError( aknsSrv.Connect() );
    
    TAknsPkgID pid;
    TAknSkinSrvSkinPackageLocation location = EAknsSrvPhone;

    CArrayPtr<CAknsSrvSkinInformationPkg>* skins;
    skins = aknsSrv.EnumerateSkinPackagesL( EAknsSrvAll );

    TInt i = 0;
    TInt skinCount = skins->Count();
    for ( i = 0; i < skinCount; i++ )
        {
        if ( skins->At( i )->Name().CompareF( aThemeName ) == 0 )
            {
            // Name found from theme list
            pid = skins->At( i )->PID();
            location = SkinLocationFromPath(
                skins->At( i )->IniFileDirectory() );
            break;
            }
        }

    skins->ResetAndDestroy();
    delete skins;
    skins = NULL;

    if ( i == skinCount )
        {
        User::Leave( KErrNotFound );
        }

    // Set all active item definition sets
    User::LeaveIfError( aknsSrv.SetAllDefinitionSets( pid ) );

    TAknsPkgIDBuf pidBuf;
    pid.CopyToDes( pidBuf );

    // Save settings to repository
    CRepository* skinsRepository =
        CRepository::NewLC( KCRUidPersonalisation );
    User::LeaveIfError( skinsRepository->Set( KPslnActiveSkinUid, pidBuf ) );
    User::LeaveIfError( skinsRepository->Set( KPslnActiveSkinLocation,
                                              location ) );

    CleanupStack::PopAndDestroy( skinsRepository );
    CleanupStack::PopAndDestroy( &aknsSrv );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Get name of the currently active theme.
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::CurrentThemeNameL()
    {
    DLTRACEIN((""));
    TAknsPkgIDBuf pidBuf;

    CRepository* skinsRepository =
        CRepository::NewLC( KCRUidPersonalisation );

    User::LeaveIfError( skinsRepository->Get( KPslnActiveSkinUid, pidBuf ) );

    CleanupStack::PopAndDestroy( skinsRepository );

    RAknsSrvSession aknsSrv;
    CleanupClosePushL( aknsSrv );
    User::LeaveIfError( aknsSrv.Connect() );
    
    TAknsPkgID pid;
    pid.SetFromDesL( pidBuf );

    DLTRACE((""));
    CArrayPtr<CAknsSrvSkinInformationPkg>* skins;
    skins = aknsSrv.EnumerateSkinPackagesL( EAknsSrvAll );
    DLTRACE((""));

    delete iCurrentThemeName;
    iCurrentThemeName = NULL;

    TInt skinCount = skins->Count();
    TInt i = 0;
    for ( ; i < skinCount; i++ )
        {
        if ( skins->At( i )->PID().iNumber == pid.iNumber &&
             skins->At( i )->PID().iTimestamp == pid.iTimestamp )
            {
            // Package ID found from theme list.
            iCurrentThemeName = skins->At( i )->Name().Alloc();
            break;
            }
        }

    skins->ResetAndDestroy();
    delete skins;
    skins = NULL;

    CleanupStack::PopAndDestroy( &aknsSrv );

    if ( iCurrentThemeName )
        {
        DLTRACEOUT(( _L("Theme name %S"), iCurrentThemeName ));
        return *iCurrentThemeName;
        }
    else
        {
        DLTRACEOUT(("No theme name"));
        return KNullDesC;
        }
    }


// ---------------------------------------------------------------------------
// Retrieves home network information.
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::HomeNetworkInfoL( TDes& aMCC, TDes& aMNC )
    {
    DLTRACEIN((""));

#ifdef CATALOGS_OVERRIDE_NETWORK
    aMCC.Copy( KCatalogsOverrideHomeMcc );
    aMNC.Copy( KCatalogsOverrideHomeMnc );
    
#else // CATALOGS_OVERRIDE_NETWORK

    TBool neededConnect = !iConnected;
    if ( !iConnected ) 
        {
        DLTRACE(("Was not connected"));
        ConnectL();
        }

    RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg info2( networkInfo );

    TRequestStatus status;

    iPhone.GetHomeNetwork( status, info2 );
    User::WaitForRequest( status );

    if ( networkInfo.iCountryCode.Length() > KMccLength )
        {
        aMCC.Copy( networkInfo.iCountryCode.Left( KMccLength ) );
        }
    else
        {
        aMCC.Copy( networkInfo.iCountryCode );
        }

    if ( networkInfo.iNetworkId.Length() > KMncLength )
        {
        aMNC.Copy( networkInfo.iNetworkId.Left( KMncLength ) );
        }
    else
        {
        aMNC.Copy( networkInfo.iNetworkId );
        }

    if ( neededConnect ) 
        {
        Close();
        }
        

    // Test config is handled afterwards since it's just easier to
    // handle mixed cases where one param is taken from the file
    // and another from the device
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG

    if ( iTestConfig ) 
        {
        if ( iTestConfig->IsSet( CNcdTestConfig::EConfigHomeMcc ) ) 
            {            
            aMCC.Copy( iTestConfig->Value( CNcdTestConfig::EConfigHomeMcc ) );
            }

        if ( iTestConfig->IsSet( CNcdTestConfig::EConfigHomeMnc ) ) 
            {            
            aMNC.Copy( iTestConfig->Value( CNcdTestConfig::EConfigHomeMnc ) );
            }
                
        DLTRACE(( _L("test MCC: %S, MNC: %S"), &aMCC, &aMNC ));        
        }
    
    #endif  // CATALOGS_BUILD_CONFIG_DEBUG
        
#endif // CATALOGS_OVERRIDE_NETWORK       
    DLTRACEOUT(( _L("MCC: %S, MNC: %S"), &aMCC, &aMNC ));
    }

    
// ---------------------------------------------------------------------------
// MCC getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::CurrentMccL()
    {
    DLTRACEIN((""));
#ifndef CATALOGS_OVERRIDE_NETWORK  

    #ifdef CATALOGS_BUILD_CONFIG_DEBUG

    if ( iTestConfig && 
         iTestConfig->IsSet( CNcdTestConfig::EConfigCurrentMcc ) ) 
        {
        DLTRACEOUT(( _L("test MCC: %S"), 
            &iTestConfig->Value( CNcdTestConfig::EConfigCurrentMcc ) ));    
        return iTestConfig->Value( CNcdTestConfig::EConfigCurrentMcc );
        }
    
    #endif  // CATALOGS_BUILD_CONFIG_DEBUG
    
    WaitForInitL();
    DLTRACEOUT(( _L("MCC"), &iNetworkInfoV1.iCountryCode ));
    return iNetworkInfoV1.iCountryCode;
#else // CATALOGS_OVERRIDE_NETWORK    
    return KCatalogsOverrideCurrentMcc();    
#endif    
    }
    
// ---------------------------------------------------------------------------
// MNC getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::CurrentMncL()
    {
    DLTRACEIN((""));
#ifndef CATALOGS_OVERRIDE_NETWORK      

    #ifdef CATALOGS_BUILD_CONFIG_DEBUG

    if ( iTestConfig && 
         iTestConfig->IsSet( CNcdTestConfig::EConfigCurrentMnc ) ) 
        {
        DLTRACEOUT(( _L("test MNC: %S"), 
            &iTestConfig->Value( CNcdTestConfig::EConfigCurrentMnc ) ));    
        return iTestConfig->Value( CNcdTestConfig::EConfigCurrentMnc );
        }
    
    #endif  // CATALOGS_BUILD_CONFIG_DEBUG
  
    WaitForInitL();
    DLTRACEOUT(( _L("MNC"), &iNetworkInfoV1.iNetworkId ));
    return iNetworkInfoV1.iNetworkId;
#else // CATALOGS_OVERRIDE_NETWORK        
    return KCatalogsOverrideCurrentMnc();    
#endif // CATALOGS_OVERRIDE_NETWORK        
    }


// ---------------------------------------------------------------------------
// Retrieves service provider information.
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::ServiceProviderL( TDes& aServiceProvider )
    {
    DLTRACEIN((""));

#ifndef CATALOGS_OVERRIDE_NETWORK
    TBool neededConnect = !iConnected;
    if ( !iConnected ) 
        {
        ConnectL();
        }

    RMobilePhone::TMobilePhoneServiceProviderNameV2 serviceProviderName;
    RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg
        serviceProviderNamePckg( serviceProviderName );

    TRequestStatus status;

    iPhone.GetServiceProviderName( status, serviceProviderNamePckg );
    User::WaitForRequest( status );

    // Operator name, service provider
    if ( serviceProviderName.iSPName.Length() > 0 )
        {
        aServiceProvider.Copy( serviceProviderName.iSPName );
        }

    if ( neededConnect ) 
        {
        Close();
        }
#else // CATALOGS_OVERRIDE_NETWORK

    aServiceProvider.Copy( KCatalogsOverrideServiceProvider );

#endif // CATALOGS_OVERRIDE_NETWORK        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// IMSI getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::ImsiL()
    {
    DLTRACEIN(( "" ));
    
    // Handle possible IMSI override from config file
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG

    if ( iTestConfig && 
         iTestConfig->IsSet( CNcdTestConfig::EConfigImsi ) ) 
        {
        DLTRACEOUT(( _L("test IMSI: %S"), 
            &iTestConfig->Value( CNcdTestConfig::EConfigImsi ) ));    
        return iTestConfig->Value( CNcdTestConfig::EConfigImsi );
        }
    
    #endif  // CATALOGS_BUILD_CONFIG_DEBUG
    
    WaitForInitL();
    DLTRACEOUT(( _L("IMSI: %S"), &iSubscriberIdV1.iSubscriberId ));    
    return iSubscriberIdV1.iSubscriberId;
    }
    
    
// ---------------------------------------------------------------------------
// IMEI getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::ImeiL()
    {
    DLTRACEIN(( "" ));
    WaitForInitL();
    return iPhoneIdV1.iSerialNumber;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CNcdDeviceService::DeviceIdentificationLC()
    {
    DLTRACEIN((""));
#ifdef CATALOGS_OVERRIDE_MODEL
    return KCatalogsOverrideDeviceId().AllocLC();
#endif    

#ifdef GET_DEVICE_ID_FROM_USERAGENT  
    
    // Get model version.
    TBuf< KSysVersionInfoTextLength > modelVersion;
    User::LeaveIfError( SysVersionInfo::GetVersionInfo(
        SysVersionInfo::EModelVersion, modelVersion ) );
    
    // Create buffer.
    HBufC* devId = HBufC::NewLC( modelVersion.Length() );
    devId->Des().Append( modelVersion );
    DLTRACEOUT(( _L("devId: %S"), devId ));
    return devId;
    
#else // Get devId from CTelephony

#error User agent must not be fetched from CTelephony!

    WaitForInitL();
    
    HBufC* devId = HBufC::NewLC( iPhoneIdV1.iManufacturer.Length() + 
        iPhoneIdV1.iModel.Length() );
    devId->Des().Copy( iPhoneIdV1.iManufacturer );
    devId->Des().Append( iPhoneIdV1.iModel );
    DLTRACEOUT(( _L("devId: %S"), devId ));
    return devId;
#endif    
    }


// ---------------------------------------------------------------------------
// Device manufacturer getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::DeviceManufacturerL()
    {
    DLTRACEIN((""));
    //WaitForInitL();    
    //return iPhoneIdV1.iManufacturer;
    return KManufacturerNokia;
    }


// ---------------------------------------------------------------------------
// Device model getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDeviceService::DeviceModelL()
    {
    DLTRACEIN((""));

#ifdef CATALOGS_OVERRIDE_MODEL
    return KCatalogsOverrideDeviceModel();
#else // CATALOGS_OVERRIDE_MODEL
    
    if ( !iDeviceModel )
        {
        // Get model version.
        TBuf< KSysVersionInfoTextLength > modelVersion;
        User::LeaveIfError( SysVersionInfo::GetVersionInfo(
            SysVersionInfo::EModelVersion, modelVersion ) );
    
        // Create buffer.
        iDeviceModel = HBufC::NewL( modelVersion.Length() );
        iDeviceModel->Des().Append( modelVersion );
        }
        
    DLTRACEOUT(( _L("device model: %S"), iDeviceModel ));
    return *iDeviceModel;
#endif // CATALOGS_OVERRIDE_MODEL   
    }

// ---------------------------------------------------------------------------
// Returns device language
// ---------------------------------------------------------------------------
//    
HBufC* CNcdDeviceService::DeviceLanguageLC()
    {
    DLTRACEIN((""));
#ifdef CATALOGS_OVERRIDE_LANGUAGE
    return KCatalogsOverrideDeviceLanguage().AllocLC();
#else
    TLanguage code = User::Language();
    return LangCodeToDescLC( code);
#endif    
    }


// ---------------------------------------------------------------------------
// Returns device firmware
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdDeviceService::FirmwareL()
    {
    DLTRACEIN((""));
#ifndef CATALOGS_OVERRIDE_FIRMWARE
    if ( iFirmware ) 
        {
        return *iFirmware;
        }
        
    iFirmware = HBufC::NewL( KFirmwareLength );
    TPtr ptr = iFirmware->Des();
#ifndef __WINS__
    User::LeaveIfError( SysUtil::GetSWVersion( ptr ) );
#else
    ptr.Copy( _L( "emulator" ));
#endif
    return *iFirmware;
    
#else // CATALOGS_OVERRIDE_FIRMWARE
    if ( iFirmware ) 
        {
        return *iFirmware;
        }
        
    iFirmware = KCatalogsOverrideFirmware().AllocL();

    return *iFirmware;
#endif    
    }


// ---------------------------------------------------------------------------
// Parses skin package location from path.
// ---------------------------------------------------------------------------
//
TAknSkinSrvSkinPackageLocation
    CNcdDeviceService::SkinLocationFromPath( const TDesC& aPath )
    {
    DLTRACEIN((""));
    TBuf<1> driveLetterBuf;
    driveLetterBuf.CopyUC( aPath.Left( 1 ) );
    if ( driveLetterBuf.CompareF( _L( "e" ) ) == 0 )
        {
        return EAknsSrvMMC;
        }
    return EAknsSrvPhone;
    }

// ---------------------------------------------------------------------------
// GetPhoneLC
// ---------------------------------------------------------------------------
void CNcdDeviceService::GetPhoneLC( RTelServer& aServer, RPhone& aPhone )
    {    
    DLTRACEIN((""));
    CleanupClosePushL( aServer );

    User::LeaveIfError( aServer.Connect() );
    User::LeaveIfError( aServer.LoadPhoneModule( KPhoneTsy ) );

    RTelServer::TPhoneInfo info;

    // Find the number of phones available from the tel server
    TInt numberPhones;
    User::LeaveIfError( aServer.EnumeratePhones( numberPhones ) );

    // Check there are available phones
    if ( numberPhones < 1 )
        {
        User::Leave( KErrNotFound );
        }

    // Get the details for the first (and only) phone
    User::LeaveIfError( aServer.GetPhoneInfo( 0, info ) );   
    
    User::LeaveIfError( aPhone.Open( aServer, info.iName ) );
    CleanupClosePushL( aPhone );    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// ConnectL
// ---------------------------------------------------------------------------    
void CNcdDeviceService::ConnectL()
    {
    DLTRACEIN((""));
    if ( !iConnected ) 
        {        
        GetPhoneLC( iServer, iPhone );
        CleanupStack::Pop( 2 );
        iConnected = ETrue;
        }
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// Close
// ---------------------------------------------------------------------------    
TInt CNcdDeviceService::Close()
    {
    DLTRACEIN((""));
    TInt err = KErrNone;
    if ( iConnected ) 
        {
        iPhone.Close();
        err = iServer.UnloadPhoneModule( KPhoneTsy );
        iServer.Close();
        iConnected = EFalse;
        }
    DLTRACEOUT(("err: %d", err));
    return err;
    }
    
    

// ---------------------------------------------------------------------------
// InitializeL
// ---------------------------------------------------------------------------    
void CNcdDeviceService::InitializeL()
    {
    DLTRACEIN((""));
    iTelephony->GetPhoneId( iStatus, iPhoneIdV1Pckg );
    iState = EGetPhoneId;
    iCurrentCancelCode = CTelephony::EGetPhoneIdCancel;
    SetActive();    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------        
#ifdef CATALOGS_BUILD_CONFIG_DEBUG

void CNcdDeviceService::SetTestConfig( CNcdTestConfig* aTestConfig )
    {
    DLTRACEIN((""));
    delete iTestConfig;
    iTestConfig = aTestConfig;
    }
    
#endif // CATALOGS_BUILD_CONFIG_DEBUG    
    
// ---------------------------------------------------------------------------
// GetSubscriberId
// ---------------------------------------------------------------------------        
void CNcdDeviceService::GetSubscriberId()
    {
    DLTRACEIN((""));
    iTelephony->GetSubscriberId( iStatus, iSubscriberIdV1Pckg );

#ifndef __WINS__
    iState = EGetSubscriberId;
#else // Skip other phases since 3.2 week 50 emulator doesn't support GetNetworkInfo
    iState = EInitialized;
#endif    
    iCurrentCancelCode = CTelephony::EGetSubscriberIdCancel;
    SetActive();
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// GetNetworkInfo
// ---------------------------------------------------------------------------        
void CNcdDeviceService::GetNetworkInfo()
    {
    DLTRACEIN((""));
    iTelephony->GetCurrentNetworkInfo( iStatus, iNetworkInfoV1Pckg );
    iState = EGetNetworkInfo;
    iCurrentCancelCode = CTelephony::EGetCurrentNetworkInfoCancel;
    SetActive();
    DLTRACEOUT((""));
    }    


// ---------------------------------------------------------------------------
// WaitForInitL
// ---------------------------------------------------------------------------        
void CNcdDeviceService::WaitForInitL()
    {    
    if ( iState != EInitialized && iError == KErrNone ) 
        {
        DLTRACE(("Start waiter"));
        DASSERT( !iWaiter );
        iWaiter = new(ELeave) CActiveSchedulerWait;
        iWaiter->Start();
        }
    User::LeaveIfError( iError );
    }


// ---------------------------------------------------------------------------
// DeleteWait
// ---------------------------------------------------------------------------    
void CNcdDeviceService::DeleteWait()
    {
    if ( iWaiter ) 
        {
        DLTRACE(("Stopping and deleting wait-object"));
        iWaiter->AsyncStop();
        delete iWaiter;
        iWaiter = NULL;
        }
    }


// ---------------------------------------------------------------------------
// FinishInitialization
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::FinishInitialization()
    {
    DLTRACEIN((""));
        
    DeleteWait();
    iCurrentCancelCode = CTelephony::ECurrentNetworkInfoChangeCancel;
    iState = EInitialized;

    // Get notifications for changes in network info
    iTelephony->NotifyChange( iStatus, 
        CTelephony::ECurrentNetworkInfoChange,
        iNetworkInfoV1Pckg );                                           
                                  
    SetActive();
    DLTRACEOUT(("Observing for changes in network"));
    }



// ---------------------------------------------------------------------------
// Appends variant information to firmware string
// ---------------------------------------------------------------------------
//
void CNcdDeviceService::AppendVariantToFirmwareL( RFs& aFs )
    {
    DLTRACEIN((""));

// Don't try to append any variant information if the firmware string is
// overridden in catalogs_device_config.h 
#ifndef CATALOGS_OVERRIDE_FIRMWARE    
    // Ensure that variant is not appended multiple times
    delete iFirmware;
    iFirmware = NULL;
    
    // Update firmware string
    FirmwareL();
    _LIT( KLineSeparator, "\n" );
    HBufC* variant = ReadVariantInformationLC( aFs );
    iFirmware = iFirmware->ReAllocL( 
        iFirmware->Length() + 
        variant->Length() + 
        KLineSeparator().Length() );
        
    DLTRACE(("Appending variant to firmware"));
    iFirmware->Des().Append( KLineSeparator );
    iFirmware->Des().Append( *variant );
    DLINFO(( _L("Firmware now: %S"), iFirmware ));
    CleanupStack::PopAndDestroy( variant );
#endif    
    }

TUid CNcdDeviceService::FlashPlayerUidL()
    {
    DLTRACEIN((""));
    
    RApaLsSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );
    session.GetAllApps();
    
    // Get the uid of the flash player
    TDataType flashType( KFlashMovieMime() );
    TUid flashUid;
    
    User::LeaveIfError( session.AppForDataType( flashType, flashUid ) );
    
    // Calls close
    CleanupStack::PopAndDestroy( &session );    
    
    DLTRACEOUT(("Flash player Uid: %d", flashUid.iUid));
    
    return flashUid;
    }


// ---------------------------------------------------------------------------
// Reads the variant information from the platform's depths
// ---------------------------------------------------------------------------
//
HBufC* CNcdDeviceService::ReadVariantInformationLC( RFs& aFs )
    {    
    DLTRACEIN((""));
#ifndef __WINS__

#ifdef __SERIES60_31__

    _LIT(KSalesModelFileName, "Z:\\resource\\versions\\model.txt");
    
    // TFileText::Read Reads a single line text record into the specified descriptor.
    // Maximum record length is described as 256 characters in the class' header.
    const TInt KMaxRecordLength = 256;

    HBufC* phoneModel = HBufC::NewLC( KMaxRecordLength );
    
    RFile file;
    User::LeaveIfError( 
        file.Open( aFs, KSalesModelFileName, 
                   EFileShareReadersOnly | EFileRead ) );
                   
    CleanupClosePushL( file );
         
    TPtr ptr = phoneModel->Des();

    // Read the data from file.
    TFileText reader;
    reader.Set( file );
    User::LeaveIfError( reader.Read( ptr ) );

    CleanupStack::PopAndDestroy( &file ); // file
    
    // Append lang version
    _LIT( KSpace, " " );
    
    HBufC* variant = HBufC::NewLC( KSysUtilVersionTextLength );
    TPtr variantPtr = variant->Des();
    User::LeaveIfError( SysUtil::GetLangVersion( variantPtr ) );
    
    phoneModel = phoneModel->ReAllocL( 
        phoneModel->Length() + 
        variant->Length() + 
        KSpace().Length() );
    
    phoneModel->Des().Append( KSpace );
    phoneModel->Des().Append( *variant );
    
    CleanupStack::PopAndDestroy( variant );

    // ReAllocL may change the location of the descriptor so 
    // we have to pop the old phoneModel pointer
    CleanupStack::Pop(); // phoneModel 
    CleanupStack::PushL( phoneModel );    
    
    DLTRACEOUT(( _L("Variant: %S"), phoneModel));    
    return phoneModel;
    
#else // __SERIES60_31__   
    _LIT( KSpace, " " );
       
    // Get model version.
    TBuf< KSysVersionInfoTextLength > modelVersion;
    User::LeaveIfError( SysVersionInfo::GetVersionInfo(
        SysVersionInfo::EModelVersion, modelVersion ) );

    // Get lang version.
    TBuf<KSysUtilVersionTextLength> langVersion;
    User::LeaveIfError( SysUtil::GetLangVersion( langVersion ) );
    
    // Create buffer.
    HBufC* variantInfo = HBufC::NewLC( modelVersion.Length() + 
        langVersion.Length() + 
        KSpace().Length() );
    variantInfo->Des().Append( modelVersion );
    variantInfo->Des().Append( KSpace );
    variantInfo->Des().Append( langVersion );
    DLTRACEOUT(( _L("Variant info: %S"), variantInfo ));
    return variantInfo;
#endif // __SERIES60_31__
    
#else
    (void) aFs;
    return KNullDesC().AllocLC();
#endif
    }

// -----------------------------------------------------------------------------
// Returns device product code
// -----------------------------------------------------------------------------
//
HBufC* CNcdDeviceService::ProductCodeLC()
    {
    DLTRACEIN((""));

#ifdef NCD_PRODUCTCODE_NOT_SUPPORTED
    // Product code information is not provided from the
    // platform for the emulator. So, use a hard coded 
    // product code value for the emulator.    
    DLTRACEOUT(("Product code not supported"));
    return NULL;

#elif defined(__WINS__)

    return KWinsProductCode().AllocLC();

#else

    // Buffer for the product code.
    HBufC* productCode(
        HBufC::NewLC( KSysVersionInfoTextLength ) );
    TPtr productCodeDes( productCode->Des() );

    // Get productCode to the buffer.
    TInt errorCode( 
        SysVersionInfo::GetVersionInfo(
//             SysVersionInfo::EProductCode, productCodeDes ) );
// This is just temporary line to use before NCD gets SDK that
// has the EProductCode defined in the sysversioninfo.h. When correct
// enum is available, remove this line and use the commented line above
// instead.
             static_cast<SysVersionInfo::TVersionInfoType>(SysVersionInfo::EModelVersion + 1), productCodeDes ) );

    if ( errorCode == KErrNotSupported )
        {
        // Because product code getter is not supported in the platform,
        // return NULL.
        DLTRACEOUT(("Product code not supported, KErrNotSupported."));
        CleanupStack::PopAndDestroy( productCode );
        return NULL;
        }
    else
        {
        // Leave if error occurred. 
        // Above, KErrNotSupported was handled as
        // a special case.
        User::LeaveIfError( errorCode );
        }

    DLTRACEOUT(( _L("Product code: %S"), productCode ));

    return productCode;
#endif // NCD_PRODUCTCODE_NOT_SUPPORTED
    }


// -----------------------------------------------------------------------------
// Returns device product type
// -----------------------------------------------------------------------------
//
TInt CNcdDeviceService::GetProductType( TDes& aType, RFs& aFs )
    {
    DLTRACEIN((""));
#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA    

    SysVersionInfo::TProductVersion productVersion;
    TInt err = SysVersionInfo::GetVersionInfo( productVersion, aFs );
    aType.Copy( productVersion.iProduct );
    return err;
    
#else
    
    (void) aType;
    (void) aFs;
    
    return KErrNotSupported;
    
#endif    
    }


// -----------------------------------------------------------------------------
// Gets firmware id 
// -----------------------------------------------------------------------------
//
TInt CNcdDeviceService::GetFirmwareId( TDes& aId, RFs& aFs )
    {
    DLTRACEIN((""));    
#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA    

    return SysVersionInfo::GetVersionInfo(
        SysVersionInfo::EFWVersion, 
        aId,
        aFs );
    
#else
    
    (void) aId;
    (void) aFs;
    
    return KErrNotSupported;
    
#endif    
    }



void CNcdDeviceService::GetFirmwareVersion1( TDes& aTarget )
    {
#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA    

    DLTRACEIN((""));
    TBuf<KSysUtilVersionTextLength> version;    

    if ( SysUtil::GetSWVersion( version ) == KErrNone )
        {         
        TInt len = version.Length();
        TInt pos1 = version.Find( KFirmwareEol );
        if( pos1 != KErrNotFound && len > pos1 )
            {
            aTarget.Append( version.Left( pos1 ) );
            }
        }
    DLTRACE(( _L("aTarget: %S"), &aTarget ));

#else

    (void) aTarget;

#endif        
    }


void CNcdDeviceService::GetFirmwareVersion2( TDes& aTarget )
    {
    DLTRACEIN((""));
#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA
    
    TBuf<KSysUtilVersionTextLength> version;    

    if ( SysUtil::GetLangSWVersion( version ) == KErrNone )
        {         
        TInt len = version.Length();
        TInt pos1 = version.Find( KFirmwareEol );
        if( pos1 != KErrNotFound && len > pos1 )
            {
            aTarget.Append( version.Left( pos1 ) );
            }
        }

    DLTRACE(( _L("aTarget: %S"), &aTarget ));

#else

    (void) aTarget;

#endif            
    }


void CNcdDeviceService::GetFirmwareVersion3( TDes& aTarget, RFs& aFs )
    {
    DLTRACEIN((""));
#ifdef NCD_GIVE_EXTRA_FIRMWARE_DATA
    
    TBuf<KSysUtilVersionTextLength> version;            

    if ( SysVersionInfo::GetVersionInfo( 
            SysVersionInfo::EOPVersion, 
            version,
            aFs ) == KErrNone )
        {
        TInt len = version.Length();
        TInt pos1 = version.Find( KFirmwareEol );
        if( pos1 != KErrNotFound && len > pos1 )
            {        
            aTarget.Append( version.Left( pos1 ) );        
            }
        }  

    DLTRACE(( _L("aTarget: %S"), &aTarget ));

#else

    (void) aTarget;
    (void) aFs;
    
#endif            
    }



// For testing purposes only, avoiding several mmp changes by including
// the cpp
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    #include "ncdtestconfig.cpp"
#endif

