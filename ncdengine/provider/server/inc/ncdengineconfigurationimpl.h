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
* Description:   CNcdEngineConfiguration declaration
*
*/


#ifndef C_NCDENGINECONFIGURATION_H
#define C_NCDENGINECONFIGURATION_H


#include <e32base.h>
#include <f32file.h>
#include "ncdengineconfiguration.h"
#include "ncdconfigurationparser.h"

class CNcdKeyValuePair;
class MNcdDeviceService;

/**
 * Engine configuration handling
 *
 * This class is used to read the engine configuration from a configuration
 * file and access the read configuration during runtime
 */
class CNcdEngineConfiguration : 
    public CBase, 
    public MNcdEngineConfiguration,
    public MNcdConfigurationParserObserver
    {    
public:

    /**
     * NewL
     *
     * @param aDeviceService Device service needed to ask MCC/MNC.
     */
    static CNcdEngineConfiguration* NewL( MNcdDeviceService& aDeviceService );
    
    /**
     * Destructor
     */
    ~CNcdEngineConfiguration();
    
    
    /**
     * Reads the configuration from the given file. 
     *
     * Engine's private path is automatically used as the root for
     * the filename. The config file is first searched from C: or E:
     * and then from Z: 
     *
     * @param aFilename Name of the file without path
     */     
    void ReadConfigurationL( const TDesC& aFilename );

public: // MNcdEngineConfiguration

    /**
     * @see MNcdEngineConfiguration::EngineType()
     */
    const TDesC& EngineType() const;
    
    /**
     * @see MNcdEngineConfiguration::EngineVersion()
     */    
    const TDesC& EngineVersion() const;

    /**
     * @see MNcdEngineConfiguration::EngineUid()
     */    
    const TDesC& EngineUid() const;


    /**
     * @see MNcdEngineConfiguration::EngineProvisiong()
     */
    const TDesC& EngineProvisioning() const;


    /**
     * @see MNcdEngineConfiguration::EngineInstallDrive()
     */
    const TDesC& EngineInstallDrive() const;
             
    
    /**
     * @see MNcdEngineConfiguration::EngineTempDrive()
     */
    TInt EngineTempDrive() const;   
    
    
    /**
     * @see MNcdEngineConfiguration::ClientDataPathLC()
     */
    HBufC* ClientDataPathLC(
        const TDesC& aClientId, TBool aTemp );
       
           
    /**
     * @see MNcdEngineConfiguration::UseFixedAp()
     */
    virtual TBool UseFixedAp() const;
         

    /**
     * @see MNcdEngineConfiguration::FixedApDetailsL()
     */
    virtual const RPointerArray<CNcdKeyValuePair>& FixedApDetails() const;


    /**
     * @see MNcdEngineConfiguration::ClearClientDataL()
     */
    void ClearClientDataL( const TDesC& aClientId, TBool aTemp );
    
    
protected: // MNcdConfigurationParserObserver

    /**
     * @see MNcdConfigurationParserObserver::ConfigurationElementEndL()
     */
    void ConfigurationElementEndL( 
        const TDesC8& aElement, 
        const TDesC8& aData );


    /**
     * @see MNcdConfigurationParserObserver::ConfigurationAttributeL()
     */
    void ConfigurationAttributeL( 
        const TDesC8& aElement, 
        const TDesC8& aAttribute, 
        const TDesC8& aValue );
        

    /**
     * @see MNcdConfigurationParserObserver::ConfigurationError()
     */        
    void ConfigurationError( TInt aError );
    
protected:

    /**
     * Constructor.
     *
     * @param aDeviceService Device service needed to ask MCC/MNC.
     */
    CNcdEngineConfiguration( MNcdDeviceService& aDeviceService );

    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Determines the drive with most free space
     *
     * @return Drive number
     */
    TDriveNumber DetermineDataDriveL() const;
    
private:
    
    HBufC* iType;
    HBufC* iVersion;
    HBufC* iProvisioning;    
    HBufC* iUid;
    TBuf<2> iInstallationDrive;
    
    TDriveUnit iDataDrive; // Drive that is used for storing (temp) data

    // Temporary variables used in AP parsing.
        
    TBool iParseApDetails; // AP details should be parsed.
    TBool iApIdFound; // Correct AP id found.
    
    HBufC* iMcc; // MCC of the AP.
    HBufC* iMnc; // MNC of the AP.
    HBufC8* iApId; // Id of the correct AP.
    HBufC* iApDetailId; // Access point detail id.
    HBufC* iApDetailValue; // Access point detail value.
    
    HBufC8* iCorrectApId; // The correct AP id, when it is found.
    
    RBuf iHomeMcc;
    RBuf iHomeMnc;
    
    // Access point details are stored here.
    RPointerArray<CNcdKeyValuePair> iApDetails;
    
    MNcdDeviceService& iDeviceService;
    
    };

#endif // C_NCDENGINECONFIGURATION_H
