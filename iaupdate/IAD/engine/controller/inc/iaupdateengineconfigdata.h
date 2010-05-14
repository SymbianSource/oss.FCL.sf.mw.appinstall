/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_ENGINE_CONFIG_DATA_H
#define IA_UPDATE_ENGINE_CONFIG_DATA_H


#include <e32base.h>


/**
 * CIAUpdateEngineConfigData provides configuration data 
 * for the engine.
 *
 * @since S60 v3.2
 */
class CIAUpdateEngineConfigData : public CBase
    {

public:

    /**
     * @return CIAUpdateEngineConfigData*
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineConfigData* NewL();

    /**
     * @return CIAUpdateEngineConfigData*
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineConfigData* NewLC();


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateEngineConfigData();  


    /**
     * Resets config data to its default values.
     *
     * @since S60 v3.2
     */
    void ResetL();


    /**
     * @return const TDesC& Software type.
     *
     * @since S60 v3.2
     */
    const TDesC& SoftwareType() const;
    
    /**
     * @param aType Software type.
     *
     * @since S60 v3.2
     */
    void SetSoftwareTypeL( const TDesC& aType );


    /**
     * @return const TDesC& Software version.
     *
     * @since S60 v3.2
     */
    const TDesC& SoftwareVersion() const;

    /**
     * @param aVersion Software version.
     *
     * @since S60 v3.2
     */
    void SetSoftwareVersionL( const TDesC& aVersion );


    /**
     * @return const TDesC& Storagame maximum size 
     * in kilo bytes. This should be an integer value 
     * in a text format.
     *
     * @since S60 v3.2
     */
    const TDesC& StorageMaxSize() const;

    /**
     * @param aSize Storagame maximum size 
     * in kilo bytes. This should be an integer value 
     * in a text format.
     *
     * @since S60 v3.2
     */
    void SetStorageMaxSizeL( const TDesC& aSize );


    /**
     * @return const TDesC& Master server uri. This is
     * the uri to the CDB server.
     *
     * @since S60 v3.2
     */
    const TDesC& MasterServerUri() const;

    /**
     * @param aUri Master server uri. This is
     * the uri to the CDB server.
     *
     * @since S60 v3.2
     */
    void SetMasterServerUriL( const TDesC& aUri );
    
    
    /**
     * @return const TDesC& Provisioning.
     *
     * @since S60 v3.2
     */
    const TDesC& Provisioning() const;

    /**
     * @param aProvisioning Provisioning.
     *
     * @since S60 v3.2
     */
    void SetProvisioningL( const TDesC& aProvisioning );


    /**
     * @return const TDesC& Client role.
     *
     * @since S60 v3.2
     */
    const TDesC& ClientRole() const;

    /**
     * @param aClientRole Client role.
     *
     * @since S60 v3.2
     */
    void SetClientRoleL( const TDesC& aClientRole );


private:

    // Prevent these if not implemented
    CIAUpdateEngineConfigData( const CIAUpdateEngineConfigData& aObject );    
    CIAUpdateEngineConfigData& operator =( const CIAUpdateEngineConfigData& aObject );  


    // Constructor
    CIAUpdateEngineConfigData();

    // 2nd. phase constructor
    void ConstructL();


private: // data

    HBufC* iSoftwareType;
    HBufC* iSoftwareVersion;
    HBufC* iStorageMaxSize;
    HBufC* iMasterServerUri;
    HBufC* iProvisioning;  
    HBufC* iClientRole;
    
    };
    
#endif // IA_UPDATE_ENGINE_CONFIG_DATA_H

