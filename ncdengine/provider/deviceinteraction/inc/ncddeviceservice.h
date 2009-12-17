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
* Description:   Declaration of MNcdDeviceService
*
*/


#ifndef M_NCD_DEVICE_SERVICE_H
#define M_NCD_DEVICE_SERVICE_H

#include <etel3rdparty.h>
#include <etelmm.h>

class CNcdTestConfig;
class RFs;

/**
 *
 */
class MNcdDeviceService
    {
public:
    
    /**
     * MCC string length
     */
    //static const TInt KMccLength = CTelephony::KNetworkCountryCodeSize;
    static const TInt KMccLength = 3;
    
    /**
     * MNC string length
     */
    //static const TInt KMncLength = CTelephony::KNetworkIdentitySize;
    static const TInt KMncLength = 3;
    
    /**
     * Maximum length of the service provider name
     */
    static const TInt KServiceProviderMaxLength = RMobilePhone::KMaxSPNameSize;
    
    /**
     * Maximum length of firmware details
     */
    static const TInt KFirmwareDetailMaxLength = 256;
    
    
public: // Destruction

    /**
     * Destructor
     */
    virtual ~MNcdDeviceService() {}
    
public: // New functions

    /**
     * Set sound file as ringing tone to the currently active profile.
     * @param aFileName Full path and name of the sound file.
     */
    virtual void SetAsRingingToneL( const TDesC& aFileName ) = 0;

    /**
     * Set graphics file as wallpaper.
     * @param aFileName Full path and name of the graphics file.
     */
    virtual void SetAsWallpaperL( const TDesC& aFileName ) = 0;

    /**
     * Set theme by using its name.
     * @param aThemeName Name of the theme.
     */
    virtual void SetAsThemeL( const TDesC& aThemeName ) = 0;
    
    /**
     * Get name of the currently active theme.
     *
     * @return Name of the theme.
     */
    virtual const TDesC& CurrentThemeNameL() = 0;

    /**
     * Retrieves home network information.
     * @param aMCC On return contains MCC. Length must
     * @param aMNC On return contains MNC.
     */
    virtual void HomeNetworkInfoL( TDes& aMCC, TDes& aMNC ) = 0;


    /**
     * Returns current MCC
     * @return MCC
     */
    virtual const TDesC& CurrentMccL() = 0;
    
    /**
     * Returns current MNC
     * @return MNC
     */
    virtual const TDesC& CurrentMncL() = 0;
            
    
    /**
     * Retrieves service provider information.
     * @param aServiceProvider On return contains service provider info.
     */
    virtual void ServiceProviderL( TDes& aServiceProvider ) = 0;
    

    /**
     * IMSI getter
     *
     * @return IMSI in a descriptor
     */
    virtual const TDesC& ImsiL() = 0;
    
    
    /**
     * IMEI getter
     *
     * @return IMEI in a descriptor
     */
    virtual const TDesC& ImeiL() = 0;


    /**
     * Returns device identification (e.g. "Nokia6620")
     * 
     * @return HBufC* Dynamically allocated descriptor containing the device id.
     */
    virtual HBufC* DeviceIdentificationLC() = 0;


    /**
     * Returns device manufacturer (e.g. "Nokia" )
     *
     * @return Device manufacturer
     */
    virtual const TDesC& DeviceManufacturerL() = 0;


    /**
     * Returns device model (e.g. "N70" )
     *
     * @return Device model
     */
    virtual const TDesC& DeviceModelL() = 0;


    /**
     * Returns device language (e.g. "en-EN")
     * 
     * @return HBufC* Dynamically allocated descriptor containing the device 
     *                language string.
     */
    virtual HBufC* DeviceLanguageLC() = 0;


    /** 
     * Returs the firmware version
     * 
     * @return Firmware version.
     */    
    virtual const TDesC& FirmwareL() = 0;


    virtual void ConnectL() = 0;
    
    virtual TInt Close() = 0;

    /**
     * Appends variant information to the string returned by 
     * FirmwareL().
     *
     * @param aFs File server session. Not used in 3.2 and newer platforms so an uninitialized handle can be given. 
     */     
    virtual void AppendVariantToFirmwareL( RFs& aFs ) = 0;
    
    /**
     * Gets flash player UID.
     *
     * @return ETrue On return, contains the UID of flash player installed
     *               on the device. If flash player was not found, returns NULL Uid
     */
    virtual TUid FlashPlayerUidL() = 0;
    
    
    /**
     * Gets device product code
     * 
     * @note Returns 0000000 in emulator builds. 
     * @note If platform does not provide support for the product code, 
     * NULL is returned.
     * @return Product code
     */
    virtual HBufC* ProductCodeLC() = 0;
    

    /**
     * Gets device product type, eg. "RM-1"
     * 
     * @param aType Target descriptor, should be KFirmwareDetailMaxLength long 
     * @param aFs File server session
     */
    virtual TInt GetProductType( TDes& aType, RFs& aFs ) = 0;

    /**
     * Gets device firmware ID
     * 
     * @param aId Target descriptor, should be KFirmwareDetailMaxLength long 
     * @param aFs File server session
     */    
    virtual TInt GetFirmwareId( TDes& aId, RFs& aFs ) = 0;
    
    
    virtual void GetFirmwareVersion1( TDes& aTarget ) = 0;
    virtual void GetFirmwareVersion2( TDes& aTarget ) = 0;
    virtual void GetFirmwareVersion3( TDes& aTarget, RFs& aFs ) = 0;
    
    
#ifdef CATALOGS_BUILD_CONFIG_DEBUG

    virtual void SetTestConfig( CNcdTestConfig* aTestConfig ) = 0;
    
#endif        

    };

#endif // M_NCD_DEVICE_SERVICE_H
