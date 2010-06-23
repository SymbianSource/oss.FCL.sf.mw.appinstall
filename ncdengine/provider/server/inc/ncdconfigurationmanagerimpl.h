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
* Description:   CNcdConfigurationManager declaration
*
*/


#ifndef C_NCDCONFIGURATIONMANAGER_H
#define C_NCDCONFIGURATIONMANAGER_H

#include <e32base.h>
#include <badesca.h>

#include "ncdconfigurationmanager.h"
#include "ncdstoragedataitem.h"
#include "ncdproviderdefines.h"
#include "ncdserverdetails.h"

class MNcdStorageManager;
class CNcdKeyValueMap;
class CNcdServerAddress;
class CNcdServerDetails;
class CNcdGeneralManager;

/**
 * Implements configuration management for NCD provider
 */
class CNcdConfigurationManager : public CBase, 
    public MNcdConfigurationManager
    {
public:

    /**
     * Creates a new configuration manager
     *
     * @param aAppUid Application UID for the CentralRepository
     * @param aProviderUid Provider UID used for common provider data
     * @return A new configuration manager
     */
    static CNcdConfigurationManager* NewL( CNcdGeneralManager& aGeneralManager );
    
    ~CNcdConfigurationManager();
 
public: // MNcdConfigurationManager

    /**
     * @see MNcdConfigurationManager::AddObserverL()
     */
    virtual void AddObserverL(
        MNcdConfigurationObserver& aObserver,
        const MCatalogsContext& aContext );

    /**
     * @see MNcdConfigurationManager::RemoveObserver()
     */
    virtual void RemoveObserver(
        MNcdConfigurationObserver& aObserver);

    /**
     * @see MNcdConfigurationManager::MasterServerAddress()
     */
    const TDesC& MasterServerAddressL( const MCatalogsContext& aContext ) const;
       
    
    /**
     * @see MNcdConfigurationManager::IsMasterServerAddressValidL()
     */
    TBool IsMasterServerAddressValidL( const MCatalogsContext& aContext ) const;
    
    /**
     * @see MNcdConfigurationManager::SetMasterServerAddressL()
     */
    void SetMasterServerAddressL( 
        const MCatalogsContext& aContext,
        const TDesC& aAddress, 
        const TInt64& aValidity );

    /**
     * @param aAddress Server address. Ownership is transferred after a 
     * successful operation
     */
    void SetMasterServerAddressL( 
        const MCatalogsContext& aContext,
        CNcdServerAddress* aAddress );
        
    /**
     * @see MNcdConfigurationManager::ResetMasterServerAddressL()
     */
    void ResetMasterServerAddressL( const MCatalogsContext& aContext );
    
    
    /**
     * @see MNcdConfigurationManager::ClientIdL()
     */
    const TDesC& ClientIdL( const MCatalogsContext& aContext );


    /**
     * @see MNcdConfigurationManager::SetSsidL()
     */
    void SetSsidL( const MCatalogsContext& aContext,
        HBufC8* aSsid );
       
    /**
     * @see MNcdConfigurationManager::Ssid()
     */
    const TDesC8& SsidL( const MCatalogsContext& aContext );
    

    /**
     * @see MNcdConfigurationManager::ServerDetailsL()
     */
    MNcdServerDetails& ServerDetailsL( 
        const MCatalogsContext& aContext, 
        const TDesC& aServerUri,
        const TDesC& aNamespace );

    /**
     * @see MNcdConfigurationManager::ServerDetails()
     */
    const MNcdServerDetails* ServerDetails( 
        const MCatalogsContext& aContext, 
        const TDesC& aServerUri,
        const TDesC& aNamespace ) const;
        
    /**
     * @see MNcdConfigurationManager::SaveConfigurationToDbL()
     */
    void SaveConfigurationToDbL(
        const MCatalogsContext& aContext );
        
    /**
     * @see MNcdConfigurationManager::ClearServerCapabilitiesL()
     */
    virtual void ClearServerCapabilitiesL(
        const MCatalogsContext& aContext );
                    

public: // From MNcdUserConfiguration

    /**
     * @see MNcdUserConfiguration::AddConfigurationL()
     */
    void AddConfigurationL( const MCatalogsContext& aContext, 
        CNcdKeyValuePair* aConfig );

    /**
     * @see MNcdUserConfiguration::RemoveConfigurationL()
     */        
    TInt RemoveConfigurationL( const MCatalogsContext& aContext,
        const TDesC& aKey );
        
    /**
     * @see MNcdUserConfiguration::ConfigurationsL()
     */        
    CNcdKeyValueMap* ConfigurationsLC( 
        const MCatalogsContext& aContext );    


    /**
     * @see MNcdUserConfiguration::ConfigurationsL()
     */        
    CNcdKeyValueMap& ConfigurationsL(
        const MCatalogsContext& aContext, 
        TNcdConfigurationCategory aCategory );
        

    
private:    
    
    // Constructor
    CNcdConfigurationManager( CNcdGeneralManager& aGeneralManager );
        
    // 2nd phase constructor
    void ConstructL();

private:

    // Generates a new client ID
    HBufC* GenerateClientIdLC() const;


    class CContextConfiguration;

    // Compares contexts
    static TBool MatchContexts( const CContextConfiguration& aFirst,
        const CContextConfiguration& aSecond );

    TInt FindConfigurationL( const MCatalogsContext& aContext ) const;

    // Determines key's configuration category
    TNcdConfigurationCategory DetermineCategory( const TDesC& aKey ) const;


    CNcdKeyValueMap& ConfigurationByCategory( TInt aIndex, 
        const TDesC& aKey );

    CNcdKeyValueMap& ConfigurationByCategory( TInt aIndex, 
        TNcdConfigurationCategory aCategory );
        

    // Create a new context configuration for the context unless it exists
    // Returns index for the configuration in iConfigurations
    TInt CreateContextConfigurationL( 
        const MCatalogsContext& aContext );

    
    void SaveDataL( const TDesC& aId,
        NcdProviderDefines::TNcdDatabaseDataType aType, 
        MNcdStorageDataItem& aDataItem );
    
    void LoadDataL( const TDesC& aId, 
        NcdProviderDefines::TNcdDatabaseDataType aType,
        MNcdStorageDataItem& aDataItem );
    
private:


    // Cached Context-specific data
    class CContextConfiguration : public CBase, public MNcdStorageDataItem
        {
        public:
        
            static CContextConfiguration* NewLC( 
                const MCatalogsContext& aContext, 
                const TDesC& aId );

            // Only to be used with RPointerArray::Find
            static CContextConfiguration* FindNewL( 
                const MCatalogsContext& aContext );
            
            
            // Destructor
            ~CContextConfiguration();
        
            // Returns details for the server. If the details have not been
            // set, creates new empty details
            MNcdServerDetails& ServerDetailsL( const TDesC& aServerUri,
                const TDesC& aNamespace );    

            // Returns details for the server. 
            // Return NULL if details are not found
            CNcdServerDetails* ServerDetails( const TDesC& aServerUri,
                const TDesC& aNamespace );    

            
            // Clears the server details.
            void ClearServerDetails();
            
            // Notifies the observers.
            void NotifyObserversL() const;
            
            /**
             * Adds an observer.
             *
             * @param aObserver The observer.
             */
            void AddObserverL( MNcdConfigurationObserver& aObserver );
            
            /**
             * Removes an observer.
             *
             * @param aObserver The observer.
             */
            void RemoveObserver( MNcdConfigurationObserver& aObserver );
            
        
        public: // MNcdStorageDataItem
        
            void ExternalizeL( RWriteStream& aStream );
            void InternalizeL( RReadStream& aStream );
                        
        protected:

            // Constructor
            CContextConfiguration( const MCatalogsContext& aContext );            
        
            void ConstructL( const TDesC& aId );
                
        public:    
        
            // Context's family id
            TUid iFamilyId;            
            
            // Client's ID
            HBufC* iId;
            
            // General configuration keys: master server address, etc.
            CNcdKeyValueMap* iConfigurationGeneral;
            
            // Client info configuration
            CNcdKeyValueMap* iConfigurationClientInfo;            
            
            // Client's SSID
            HBufC8* iSsid;
            
            // Master server override received from the protocol
            CNcdServerAddress* iMasterServer;
            
            // Flag used to override MSA set by the client through
            // AddConfiguration
            TBool iUseHardcodedMasterServer;
        
        private:
            RPointerArray<CNcdServerDetails> iServerDetails;
            
            // Configuration observers.
            RPointerArray<MNcdConfigurationObserver> iObservers;            
        };
    
        
private: // data

    CNcdGeneralManager& iGeneralManager;
    MNcdStorageManager& iStorageManager;
    
    // Cached configurations
    RPointerArray<CContextConfiguration> iConfigurations;    
    };
    

#endif // M_NCDCONFIGURATIONMANAGER_H
