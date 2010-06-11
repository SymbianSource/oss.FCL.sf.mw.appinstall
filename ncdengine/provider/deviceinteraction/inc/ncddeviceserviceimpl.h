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
* Description:   Declaration of CNcdDeviceService
*
*/


#ifndef C_NCD_DEVICE_SERVICE_IMPL_H
#define C_NCD_DEVICE_SERVICE_IMPL_H

#include <e32base.h>
#include <AknsSrvClient.h>

#include "ncddeviceservice.h"

class CTelephony;
class CNcdTestConfig;

// Uncomment this if device ID should be retrieved from 
// CUserAgent instead of CTelephony
#define GET_DEVICE_ID_FROM_USERAGENT


/**
 *
 */
class CNcdDeviceService : public CActive,
                          public MNcdDeviceService
    {
public: // Construction & destruction

    /**
     * Constructor.
     * @return CNcdDeviceService* Device service.
     */
    static CNcdDeviceService* NewL();
    
    /**
     * Constructor.
     * @return CNcdDeviceService* Device service.
     */
    static CNcdDeviceService* NewLC();
    
    /**
     * Destructor.
     */
    ~CNcdDeviceService();
    
private: // Construction

    /**
     * Constructor.
     */
    CNcdDeviceService();
    
    void ConstructL();

private: // From CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

public: // From MNcdDeviceService

    /**
     * @see MNcdDeviceService::SetAsRingingToneL
     */
    void SetAsRingingToneL( const TDesC& aFileName );

    /**
     * @see MNcdDeviceService::SetAsWallpaper
     */
    void SetAsWallpaperL( const TDesC& aFileName );

    /**
     * @see MNcdDeviceService::SetAsThemeL
     */
    void SetAsThemeL( const TDesC& aThemeName );

    /**
     * @see MNcdDeviceService::CurrentThemeNameL
     */
    const TDesC& CurrentThemeNameL();

    /**
     * @see MNcdDeviceService::HomeNetworkInfoL
     */
    void HomeNetworkInfoL( TDes& aMCC, TDes& aMNC );

   
    /**
     * @see MNcdDeviceService::CurrentMccL
     */
    const TDesC& CurrentMccL();
    
    /**
     * @see MNcdDeviceService::CurrentMncL
     */
    const TDesC& CurrentMncL();
    
    
    /**
     * @see MNcdDeviceService::ServiceProviderL
     */
    void ServiceProviderL( TDes& aServiceProvider );

    /**
     * @see MNcdDeviceService::ImsiL()
     */
    const TDesC& ImsiL();


    const TDesC& ImeiL();

    /**
     * @see MNcdDeviceService::DeviceIdentificationLC()
     */
    HBufC* DeviceIdentificationLC();


    /**
     * @see MNcdDeviceService::DeviceManufacturerL()
     */
    const TDesC& DeviceManufacturerL();


    /**
     * @see MNcdDeviceService::DeviceModelL()
     */
    const TDesC& DeviceModelL();
    
    
    /**
     * @see MNcdDeviceService::Series60VersionL()
     */
    //TVersion Series60VersionL();

    /**
     * @see MNcdDeviceService::DeviceLanguageLC()
     */
    HBufC* DeviceLanguageLC();
    
    
    /** 
     * @see MNcdDeviceService::FirmwareL()
     */
    const TDesC& FirmwareL();


    void ConnectL();
    
    TInt Close();
    

    /** 
     * @see MNcdDeviceService::AppendVariantToFirmwareL()
     */
    void AppendVariantToFirmwareL( RFs& aFs );
    
    /**
     * see MNcdDeviceService::FlashPlayerUidL()
     */
    TUid FlashPlayerUidL();
    
    
    void InitializeL();
    
    /**
     * @see MNcdDeviceService::ProductCodeLC()
     */
    HBufC* ProductCodeLC();
    
    /**
     * @see MNcdDeviceService::GetProductType()
     */
    TInt GetProductType( TDes& aType, RFs& aFs );

    /**
     * @see MNcdDeviceService::GetFirmwareId()
     */
    TInt GetFirmwareId( TDes& aId, RFs& aFs );
    
    
    void GetFirmwareVersion1( TDes& aTarget );

    void GetFirmwareVersion2( TDes& aTarget );
    
    void GetFirmwareVersion3( TDes& aTarget, RFs& aFs );
        
    
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    /**
     * Used to set a test configuration that overrides device config
     *
     * @note Ownership is transferred to the device service
     */
    void SetTestConfig( CNcdTestConfig* aTestConfig );
#endif

private: // New functions

    /**
     * Parses skin package location from path.
     * @param aPath Skin path.
     * @return TAknSkinSrvSkinPackageLocation Skin location.
     */
    TAknSkinSrvSkinPackageLocation SkinLocationFromPath( const TDesC& aPath );
    
    void GetPhoneLC( RTelServer& aServer, RPhone& aPhone );

    void GetSubscriberId();
    
    void GetNetworkInfo();
            
    void GetNetworkRegistrationStatus();

    void WaitForInitL();
    
    void DeleteWait();
    
    // Finishes the initialization 
    void FinishInitialization();

    // Reads phone variant information and returns it
    HBufC* ReadVariantInformationLC( RFs& aFs );
    
private:

    enum TDeviceServiceState
        {
        ENotInitialized = 0,
        EGetPhoneId,
        EGetSubscriberId,
        EGetNetworkInfo,        
        EInitialized
        };
    
private: // data

    HBufC* iCurrentThemeName;    

    CTelephony* iTelephony;
    CTelephony::TPhoneIdV1 iPhoneIdV1;
    CTelephony::TPhoneIdV1Pckg iPhoneIdV1Pckg;

    CTelephony::TSubscriberIdV1 iSubscriberIdV1;
    CTelephony::TSubscriberIdV1Pckg iSubscriberIdV1Pckg;
    
    CTelephony::TNetworkInfoV1 iNetworkInfoV1;
    CTelephony::TNetworkInfoV1Pckg iNetworkInfoV1Pckg;
                
    CActiveSchedulerWait* iWaiter;
    
    HBufC* iImsi;
    HBufC* iFirmware;    

    HBufC* iDeviceManufacturer;
    HBufC* iDeviceModel;

    RTelServer iServer;
    RMobilePhone iPhone;
    TBool iConnected;
    
    TDeviceServiceState iState;
    CTelephony::TCancellationRequest iCurrentCancelCode;
    
    TInt iError;
    
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    CNcdTestConfig* iTestConfig;
#endif
    };

#endif // C_NCD_DEVICE_SERVICE_IMPL_H
