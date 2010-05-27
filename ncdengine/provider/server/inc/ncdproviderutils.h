/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdProviderUtils implementation
*
*/


#ifndef NCD_PROVIDER_UTILS_H
#define NCD_PROVIDER_UTILS_H


#include <e32base.h>
#include <f32file.h>
#include "ncdinstallationservice.h"

class MNcdEngineConfiguration;
class CNcdEngineConfiguration;
class MNcdDeviceService;
class MNcdProtocol;
class CNcdHttpUtils;

/**
 *
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdProviderUtils : public CBase
    {

public: // 

        
    /**
     * Creates new provider utils
     *
     * @param aConfigFile Configuration file. Just filename, no path
     */
    static CNcdProviderUtils* NewL( 
        const TDesC& aConfigFile );
    
    
    virtual ~CNcdProviderUtils();
    
    
    /**
     * File session getter
     */
    static RFs& FileSession();
    
    
    /**
     * Installation service getter
     *
     * @note Creates the service if it doesn't exist yet.
     */
    static MNcdInstallationService& InstallationServiceL();


    /**
     * Engine configuration getter
     */
    static MNcdEngineConfiguration& EngineConfig();
    
    
    /**
     * Device service getter
     *
     * @return Device service
     */
    static MNcdDeviceService& DeviceService();
    
    
   
    
    /**
     * Provider temp path getter
     *
     * @return Temp path     
     */
    static HBufC* TempPathLC( const TDesC& aClientId );
    
    static void ReadDatabaseVersionsL( 
        const TDesC& aRootPath,
        TUint32& aGeneralVersion, 
        TUint32& aPurchaseHistoryVersion );
        
    static void WriteDatabaseVersionsL(
        const TDesC& aRootPath,
        TUint32 aGeneralVersion,
        TUint32 aPurchaseHistoryVersion );
    
    static TInt UpdateShutdownFileL( const TDesC& aRootPath );

    static void RemoveShutdownFileL( const TDesC& aRootPath );
    
    
    /**    
     */
    static TNcdApplicationStatus IsApplicationInstalledL(
        const TUid& aUid, const TDesC& aVersion );
    
    // check if a widget with given identifier is installed already    
    static TNcdApplicationStatus IsWidgetInstalledL(
        const TDesC& aIdentifier, const TDesC& aVersion );
    

    //  Calling widget registry API to return the Uid of the widget with given identifier
    static TUid WidgetUidL( const TDesC& aIdentifier);
    
    /**
     * Compares version number strings
     *
     * They are converted as TCatalogsVersion before conversion
     * @return 0 if versions match, negative value if left < right and
     * positive value if left > right
     * @see TCatalogsVersion::ConvertL for possible leave codes
     */
    static TInt CompareVersionsL( const TDesC& aLeft, const TDesC& aRight );

    
private:

    CNcdProviderUtils();
    void ConstructL( const TDesC& aConfigFile );
    
protected:

    static RFs iFs;
    static CNcdEngineConfiguration* iConfig;
    static MNcdDeviceService* iDeviceService;
    static MNcdInstallationService* iInstallationService;
    };
    
    
#endif // NCD_PROVIDER_UTILS_H
    
