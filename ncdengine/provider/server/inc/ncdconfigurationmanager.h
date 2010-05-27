/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MNcdConfigurationManager declaration
*
*/


#ifndef M_NCDCONFIGURATIONMANAGER_H
#define M_NCDCONFIGURATIONMANAGER_H

#include "ncduserconfiguration.h"

class MCatalogsContext;
class CNcdServerAddress;
class MNcdServerDetails;
class MNcdConfigurationObserver;


/**
 * Interface used to manipulate provider-specific configurations
 *
 *
 * There can be three master server addresses.
 *
 * By default, hardcoded master server address is used.
 * If the client adds a KMasterServer-configuration, then that is used.
 * 
 */
class MNcdConfigurationManager : public MNcdUserConfiguration
    {
public:

    /**
     * Adds a configuration observer to a context.
     *
     * @param aObserver The observer.
     * @param aContext The context.
     */
    virtual void AddObserverL(
        MNcdConfigurationObserver& aObserver,
        const MCatalogsContext& aContext ) = 0;

    /**
     * Removes a configuration observer.
     * @note The observer is removed from all the contexts.
     *
     * @param aObserver The observer.
     */
    virtual void RemoveObserver(
        MNcdConfigurationObserver& aObserver ) = 0;

    /**
     * Gets current Master Server address from configuration manager.
     * The hardcoded address is returned if no other (valid) address is set
     *
     * @return Master server address.
     * @leave KNcdErrorNoMasterServerUri if there is no master server address at all
     */
    virtual const TDesC& MasterServerAddressL( 
        const MCatalogsContext& aContext ) const = 0;
    
    /**
     * Master server address validity getter
     *
     * @return Validity value for master server address
     */
    //virtual TInt64 MasterServerAddressValidity() = 0;
    
    
    /**
     * Checks if the current Master server address is still valid
     *
     * @return True if the address is valid
     * @throw KErrNotFound if the validity of the address has not been
     * set yet
     */
    virtual TBool IsMasterServerAddressValidL( 
        const MCatalogsContext& aContext ) const = 0;
    
    /**
     * Master server address and validity setter
     *
     * @param aAddress Master server address
     * @param aValidity Master server address validity time. Must be
     * Universal time. 
     */
    virtual void SetMasterServerAddressL( 
        const MCatalogsContext& aContext,
        const TDesC& aAddress, 
        const TInt64& aValidity ) = 0;

    /**
     * @param aAddress Server address. Ownership is transferred after a 
     * successful operation
     */
    virtual void SetMasterServerAddressL( 
        const MCatalogsContext& aContext,
        CNcdServerAddress* aAddress ) = 0;
     
    
    /**
     * Resets the Master server address to the previous address.
     */ 
    virtual void ResetMasterServerAddressL( 
        const MCatalogsContext& aContext ) = 0;


/**
 * @ Remove if not needed     
    virtual void SetClientUpdateL( const TDesC& aUri, 
        const TCatalogsVersion& aVersion, TBool aForced ) = 0;
     
    virtual HBufC* GetClientUpdateL( TCatalogsVersion& aVersion,
        TBool& aForced ) = 0; 
 */     
    /**
     * Client ID getter
     *
     * The ID is generated if it doesn't already exist
     *
     * @param aContext Client's context
     * @return Unique client ID
     */
    virtual const TDesC& ClientIdL( const MCatalogsContext& aContext ) = 0;

    
    /**
     * SSID setter
     *
     * @param aContext Client's context
     * @param aSsid SSID to set for the context. Ownership is transferred.
     */     
    virtual void SetSsidL( 
        const MCatalogsContext& aContext,
        HBufC8* aSsid ) = 0;
       
    /**
     * SSID getter
     *
     * @param aContext Context 
     * @return SSID for the context or empty descriptor if not set
     */     
    virtual const TDesC8& SsidL( const MCatalogsContext& aContext ) = 0;


    /**
     * Server details getter
     *
     * @param aContext Context
     * @param aServerUri Server URI
     * @param aNamespace Namespace
     * @return Server details
     *
     * @note If the details don't exists, they are created
     */
    virtual MNcdServerDetails& ServerDetailsL( 
        const MCatalogsContext& aContext, 
        const TDesC& aServerUri,
        const TDesC& aNamespace ) = 0;


    /**
     * Server details getter
     *
     * @param aContext Context
     * @param aServerUri Server URI
     * @param aNamespace Namespace
     * @return Server details or NULL if not found. 
     *
     * Ownership of the server details is NOT transferred.
     *
     */
    virtual const MNcdServerDetails* ServerDetails( 
        const MCatalogsContext& aContext, 
        const TDesC& aServerUri,
        const TDesC& aNamespace ) const = 0;


    /**
     * Saves the configuration of the given context to the database.
     *
     * @param aContext The context.
     */
    virtual void SaveConfigurationToDbL(
        const MCatalogsContext& aContext ) = 0;
    
    /**
     * Clears the server capabilities and cookies from the database.
     *
     * @param aContext The context of which server capabilities are cleared.
     */    
    virtual void ClearServerCapabilitiesL( const MCatalogsContext& aContext ) = 0;

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationManager()
        {
        }
    };

#endif // M_NCDCONFIGURATIONMANAGER_H
