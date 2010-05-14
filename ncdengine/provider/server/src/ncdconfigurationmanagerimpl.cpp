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
* Description:   Implementation of CNcdConfigurationManager
*
*/


#include <e32math.h>

#include "ncderrors.h"
#include "ncdconfigurationmanagerimpl.h"
#include "catalogsdebug.h"
#include "catalogscontext.h"
#include "catalogsutils.h"
#include "ncdstoragemanager.h"
#include "ncddatabasestorage.h"
#include "ncdstorage.h"
#include "ncdkeyvaluemap.h"
#include "ncdkeyvaluepair.h"
#include "ncdproviderdefines.h"
#include "ncdstorageitem.h"
#include "ncdserveraddress.h"
#include "ncdconfigurationkeys.h"
#include "ncdutils.h"
#include "catalogsconstants.h"
#include "ncdconfigurationobserver.h"
#include "ncdserverdetailsimpl.h"
#include "ncdproviderutils.h"
#include "ncdgeneralmanager.h"


// ---------------------------------------------------------------------------
// CContextConfigurationId
// ---------------------------------------------------------------------------
//    

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//         
CNcdConfigurationManager::CContextConfiguration* 
    CNcdConfigurationManager::CContextConfiguration::NewLC( 
        const MCatalogsContext& aContext, 
        const TDesC& aId )
    {
    CContextConfiguration* self = new( ELeave ) CContextConfiguration(
        aContext );
    CleanupStack::PushL( self );
    self->ConstructL( aId );
    return self;
    }


// ---------------------------------------------------------------------------
// FindNewL
// ---------------------------------------------------------------------------
//         
CNcdConfigurationManager::CContextConfiguration* 
    CNcdConfigurationManager::CContextConfiguration::FindNewL( 
        const MCatalogsContext& aContext )
    {
    CContextConfiguration* self = new( ELeave ) CContextConfiguration(
        aContext );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//         
CNcdConfigurationManager::CContextConfiguration::CContextConfiguration( 
    const MCatalogsContext& aContext ) : 
    iFamilyId( aContext.FamilyId() ), 
    iId( NULL ),
    iConfigurationGeneral( NULL ), 
    iConfigurationClientInfo( NULL ),
    iSsid( NULL ),
    iMasterServer( NULL )    
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//         
void CNcdConfigurationManager::CContextConfiguration::ConstructL(
    const TDesC& aId )
    {
    DLTRACEIN((""));
    iId = aId.AllocL();
    iSsid = KNullDesC8().AllocL();
    iConfigurationGeneral = CNcdKeyValueMap::NewL();
    iConfigurationClientInfo = CNcdKeyValueMap::NewL();        
    
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//         
CNcdConfigurationManager::CContextConfiguration::~CContextConfiguration()
    {
    delete iId;
    delete iConfigurationGeneral;
    delete iConfigurationClientInfo;
    delete iSsid;
    delete iMasterServer;
    iServerDetails.ResetAndDestroy();
    iObservers.Close();
    }


// ---------------------------------------------------------------------------
// ServerDetailsL
// ---------------------------------------------------------------------------
//         
MNcdServerDetails& 
    CNcdConfigurationManager::CContextConfiguration::ServerDetailsL( 
    const TDesC& aServerUri,
    const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    CNcdServerDetails* details = ServerDetails( aServerUri, aNamespace );
    
    if ( !details ) 
        {        
        DLTRACE(("Creating new details"));
        details = CNcdServerDetails::NewLC( aServerUri,
            aNamespace );
        iServerDetails.AppendL( details );
        CleanupStack::Pop( details );
        }
    return *details;
    }

// ---------------------------------------------------------------------------
// ServerDetailsL
// ---------------------------------------------------------------------------
//         
CNcdServerDetails*
    CNcdConfigurationManager::CContextConfiguration::ServerDetails( 
    const TDesC& aServerUri,
    const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iServerDetails.Count(); ++i )
        {
        if ( iServerDetails[i]->ServerUri() == aServerUri &&
             iServerDetails[i]->Namespace() == aNamespace ) 
            {
            DLTRACEOUT(("Found details"));
            return iServerDetails[i];
            }
        }
    DLTRACEOUT(("Details not found"));
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// ClearServerDetails
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::CContextConfiguration::ClearServerDetails() 
    {
    DLTRACEIN((""));
    iServerDetails.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// Notifies the observers.
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::CContextConfiguration::NotifyObserversL() const 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iObservers.Count(); i++ ) 
        {
        iObservers[i]->ConfigurationChangedL();
        }
    }
    
// ---------------------------------------------------------------------------
// Adds an observer.
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::CContextConfiguration::AddObserverL(
    MNcdConfigurationObserver& aObserver ) 
    {
    DLTRACEIN((""));
    if ( iObservers.Find( &aObserver ) == KErrNotFound ) 
        {
        iObservers.AppendL( &aObserver );
        }
    }
    
// ---------------------------------------------------------------------------
// Removes an observer.
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::CContextConfiguration::RemoveObserver(
    MNcdConfigurationObserver& aObserver ) 
    {
    DLTRACEIN((""));
    TInt index = iObservers.Find( &aObserver );
    if ( index != KErrNotFound ) 
        {
        iObservers.Remove( index );
        }
    }


// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//         
void CNcdConfigurationManager::CContextConfiguration::ExternalizeL( 
    RWriteStream& aStream )
    {
    DLTRACEIN((""));    
    ExternalizeDesL( *iId, aStream );
    ExternalizeDesL( *iSsid, aStream );
    if ( iMasterServer ) 
        {
        DLTRACE(("Externalizing master server address"));
        aStream.WriteInt32L( 1 );
        iMasterServer->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteInt32L( 0 );
        }
    
    DLTRACE(("Externalizing %d server details", iServerDetails.Count() ));    
    // Externalize server capabilities.
    aStream.WriteInt32L( iServerDetails.Count() );
    for ( TInt i = 0; i < iServerDetails.Count(); i++ ) 
        {
        iServerDetails[i]->ExternalizeL( aStream );
        }            
            
    DLTRACEOUT((""));
    }
     
     
// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//         
void CNcdConfigurationManager::CContextConfiguration::InternalizeL( 
    RReadStream& aStream )
    {
    DLTRACEIN((""));
    InternalizeDesL( iId, aStream );
    InternalizeDesL( iSsid, aStream );
    TInt32 masterServerExists = aStream.ReadInt32L();
    if ( masterServerExists ) 
        {
        DLTRACE(("Internalizing master server address"));
        CNcdServerAddress* newServer = CNcdServerAddress::NewL( aStream );
        delete iMasterServer;
        iMasterServer = newServer;
        }
    else 
        {
        delete iMasterServer;
        iMasterServer = NULL;
        }
    
    
    // Internalize server capabilities.
    iServerDetails.ResetAndDestroy();
    TInt32 serverDetailCount = aStream.ReadInt32L();
    
    DLTRACE(("Internalizing %d server details", serverDetailCount ));
    
    iServerDetails.ReserveL( serverDetailCount );
    for ( TInt i = 0; i < serverDetailCount; i++ ) 
        {
        CNcdServerDetails* details = CNcdServerDetails::NewL( aStream );
        TInt err = iServerDetails.Append( details );
        DASSERT( err == KErrNone );
        }        
    
    DLTRACEOUT(("Internalization successful"));
    }
            

// ---------------------------------------------------------------------------
// CNcdConfigurationManager
// ---------------------------------------------------------------------------
//    


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//    
CNcdConfigurationManager* CNcdConfigurationManager::NewL( 
    CNcdGeneralManager& aGeneralManager )
    {
    CNcdConfigurationManager* self = new( ELeave ) CNcdConfigurationManager(
        aGeneralManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//        
CNcdConfigurationManager::~CNcdConfigurationManager()
    {
    DLTRACEIN((""));
    iConfigurations.ResetAndDestroy();
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Adds a new observer
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::AddObserverL(
    MNcdConfigurationObserver& aObserver,
    const MCatalogsContext& aContext ) 
    {
    DLTRACEIN((""));
    TInt index = CreateContextConfigurationL( aContext );
    CContextConfiguration* conf = iConfigurations[index];
    conf->AddObserverL( aObserver );
    }
    
// ---------------------------------------------------------------------------
// Removes a observer
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::RemoveObserver(
    MNcdConfigurationObserver& aObserver )
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iConfigurations.Count(); i++ ) 
        {
        iConfigurations[i]->RemoveObserver( aObserver );
        }
    }
 
// ---------------------------------------------------------------------------
// Master server address getter
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdConfigurationManager::MasterServerAddressL( 
    const MCatalogsContext& aContext ) const
    {
    DLTRACEIN((""));
    
    TInt index = FindConfigurationL( aContext );
    
    if ( index != KErrNotFound )
        {
        // Context has a configuration        
        const CContextConfiguration& config( *iConfigurations[index] );
    
        // See if MSA exists and is valid
        if ( config.iMasterServer && config.iMasterServer->IsValid() ) 
            {
            DLINFO(( _L("Using master server address: %S"), 
                &config.iMasterServer->Address() ));
            return config.iMasterServer->Address();
            }
        
        
        // Check if the client has overridden master server address
        CNcdKeyValueMap::KeyValueIndex keyIndex = 
            config.iConfigurationGeneral->KeyExists( 
                NcdConfigurationKeys::KMasterServer );
        
        if ( keyIndex != KErrNotFound && !config.iUseHardcodedMasterServer ) 
            {
            DLINFO(( _L("Using Master Server Address from client: %S"), 
                &config.iConfigurationGeneral->ValueByIndex( keyIndex ) ));
            return config.iConfigurationGeneral->ValueByIndex( keyIndex );
            }
        }
    
    if ( !NcdProviderDefines::KMasterServerUri().Length() ) 
        {
        DLERROR(("No master server URI. Leaving with KNcdErrorNoMasterServerUri"));
        User::Leave( KNcdErrorNoMasterServerUri );
        }
        
    // Use hardcoded MSA     
    DLTRACEOUT(( _L("Using hardcoded: %S"), 
        &NcdProviderDefines::KMasterServerUri() ));
    return NcdProviderDefines::KMasterServerUri();    
    }

    
// ---------------------------------------------------------------------------
// Master server address validity checker
// ---------------------------------------------------------------------------
//            
TBool CNcdConfigurationManager::IsMasterServerAddressValidL( 
    const MCatalogsContext& aContext ) const
    {
    DLTRACEIN((""));

    TInt index = FindConfigurationL( aContext );
    
    if ( index != KErrNotFound )
        {
        
        // Context has a configuration        
        const CContextConfiguration& config( *iConfigurations[index] );
    
        // See if MSA exists
        if ( config.iMasterServer )
            {
            DLTRACE(("Context has set a Master server address"));            
            return config.iMasterServer->IsValid();
            }
        }

    // Other MSA's are always valid
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// Master server address & validity setter
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::SetMasterServerAddressL(
    const MCatalogsContext& aContext,
    const TDesC& aAddress, 
    const TInt64& aValidity )
    {
    DLTRACEIN(( _L("address: %S, validity: %d"), &aAddress, aValidity ));
    if ( !aAddress.Length() ) 
        {
        User::Leave( KErrArgument );
        }

    
    CNcdServerAddress* address = CNcdServerAddress::NewLC(
        aAddress, aValidity );
        
    SetMasterServerAddressL( aContext, address );
    CleanupStack::Pop( address );
        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Master server address & validity setter
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::SetMasterServerAddressL(
    const MCatalogsContext& aContext,
    CNcdServerAddress* aAddress )
    {
    DLTRACEIN((""));
    TInt index = CreateContextConfigurationL( aContext );    
    CContextConfiguration& config( *iConfigurations[index] );    
    
    delete config.iMasterServer;
    config.iMasterServer = aAddress;

    config.iUseHardcodedMasterServer = EFalse;

    // Save updated master server address
    SaveDataL( aContext.FamilyId().Name(), 
        NcdProviderDefines::ENcdContextData, config );
        
    config.NotifyObserversL();    
    }


// ---------------------------------------------------------------------------
// Resets the master server address
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::ResetMasterServerAddressL( 
    const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    // Sets the current overriding master server address as invalid
    TInt index = FindConfigurationL( aContext );
    if ( index != KErrNotFound ) 
        {
        if ( iConfigurations[index]->iMasterServer ) 
            {
            DLINFO(("Reverting to client-set/hardcoded MasterServer"));
            delete iConfigurations[index]->iMasterServer;
            iConfigurations[index]->iMasterServer = NULL;
            }
        else 
            {             
            // Check if the client has overridden master server address
            CNcdKeyValueMap::KeyValueIndex keyIndex = 
                iConfigurations[index]->iConfigurationGeneral->KeyExists( 
                    NcdConfigurationKeys::KMasterServer );

            if ( keyIndex != KErrNotFound ) 
                {
                // Use hardcoded instead of the one set by the client
                iConfigurations[index]->iUseHardcodedMasterServer = ETrue;                
                }        
            }
        iConfigurations[index]->NotifyObserversL();
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Client ID getter
// ---------------------------------------------------------------------------
//        
const TDesC& CNcdConfigurationManager::ClientIdL(
    const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    
    TInt index = CreateContextConfigurationL( aContext );
    CContextConfiguration& data( *iConfigurations[index] );
    if ( !data.iId->Length() ) 
        {
        DLTRACE(("Trying to load id from db"));
        TRAPD( err,
        LoadDataL( aContext.FamilyId().Name(), 
            NcdProviderDefines::ENcdContextData,
            data ) );
        
        if ( err != KErrNone && err != KErrNotFound ) 
            {
            // serious error
            User::Leave( err );
            }
        
        if ( !data.iId->Length() )
            {
            // Generate a new client id
            HBufC* id = GenerateClientIdLC();
            delete data.iId;
            data.iId = id;
            CleanupStack::Pop( id );
            
            // Save the client id
            SaveDataL( aContext.FamilyId().Name(), 
                NcdProviderDefines::ENcdContextData, data );
            }
        }
    DLTRACEOUT(( _L("Id: %S"), data.iId ));
    return *data.iId;
    
    }


// ---------------------------------------------------------------------------
// SSID setter
// ---------------------------------------------------------------------------
//
void CNcdConfigurationManager::SetSsidL( const MCatalogsContext& aContext, 
    HBufC8* aSsid )
    {
    DLTRACEIN(( "" ));    
    TInt index = CreateContextConfigurationL( aContext );
    if ( iConfigurations[index]->iSsid ) 
        {
        DLTRACE(( "Deleting old SSID" ));
        delete iConfigurations[index]->iSsid;
        }
        
    iConfigurations[index]->iSsid = aSsid;

    // Save updated Ssid
    SaveDataL( aContext.FamilyId().Name(), 
        NcdProviderDefines::ENcdContextData, *iConfigurations[index] );

    DLTRACEOUT(( "" ));
            
    }
     
       
// ---------------------------------------------------------------------------
// SSID getter
// ---------------------------------------------------------------------------
//
const TDesC8& CNcdConfigurationManager::SsidL( 
    const MCatalogsContext& aContext )
    {
    // Try to find the configuration from both memory and db
    TInt index = CreateContextConfigurationL( aContext );
    if ( index != KErrNotFound && iConfigurations[index]->iSsid ) 
        {
        return *iConfigurations[index]->iSsid;
        }
    DLERROR(("No SSID set!"));    
    return KNullDesC8();
    }
    

// ---------------------------------------------------------------------------
// Server details getter
// ---------------------------------------------------------------------------
//
MNcdServerDetails& CNcdConfigurationManager::ServerDetailsL( 
    const MCatalogsContext& aContext, 
    const TDesC& aServerUri,
    const TDesC& aNamespace )
    {
    DLTRACEIN(( _L("ServerURI: %S, Namespace: %S"), &aServerUri, &aNamespace ));
    // Get index for the config. A new configuration is created if necessary    
    TInt index = CreateContextConfigurationL( aContext );    
    
    // Get server details. New details are created if necessary
    return iConfigurations[index]->ServerDetailsL( aServerUri, aNamespace );
    }
    

// ---------------------------------------------------------------------------
// Server details getter
// ---------------------------------------------------------------------------
//
const MNcdServerDetails* CNcdConfigurationManager::ServerDetails( 
    const MCatalogsContext& aContext, 
    const TDesC& aServerUri,
    const TDesC& aNamespace ) const
    {
    DLTRACEIN(( _L("ServerURI: %S, Namespace: %S"), &aServerUri, &aNamespace ));
    // Get index for the config. 
    TInt index = KErrNotFound;
    
    TRAPD( err, index = FindConfigurationL( aContext ) );    
    if ( err == KErrNone && index != KErrNotFound ) 
        {
        // Get server details. New details are created if necessary
        return iConfigurations[index]->ServerDetails( aServerUri, aNamespace );
        }
    DLTRACEOUT(("Details not found or error occured"));
    return NULL;
    }
    
    
// ---------------------------------------------------------------------------
// Saves context configuration details to database.
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::SaveConfigurationToDbL(
    const MCatalogsContext& aContext ) 
    {
    DLTRACEIN((""));
    
    // Get index for the config.
    TInt index = FindConfigurationL( aContext );    
    if ( index == KErrNotFound ) 
        {
        DLTRACEOUT(("Configuration not found"));
        return;
        }
    
    // Get server details.
    CContextConfiguration* config = iConfigurations[index];
    
    // Save the configuration.
    SaveDataL( aContext.FamilyId().Name(), 
        NcdProviderDefines::ENcdContextData, *config );
    }
    
// ---------------------------------------------------------------------------
// Clears the server capabilities.
// ---------------------------------------------------------------------------
void CNcdConfigurationManager::ClearServerCapabilitiesL(
    const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    
    // Get index for the config.
    TInt index = FindConfigurationL( aContext );
    if ( index == KErrNotFound ) 
        {
        return;
        }
        
    // Get the config and clear server details.
    CContextConfiguration* config = iConfigurations[index];
    config->ClearServerDetails();
    
    // Save the cleared config.
    SaveConfigurationToDbL( aContext );        
    }
    

// ---------------------------------------------------------------------------
// AddConfigurationL
// ---------------------------------------------------------------------------
//
void CNcdConfigurationManager::AddConfigurationL( 
    const MCatalogsContext& aContext, CNcdKeyValuePair* aConfig )
    {    
    DLTRACEIN(( "" ));
    
    // Ensure aConfig is deleted if a leave occurs
    CleanupStack::PushL( aConfig );

    // Get index for the config. A new configuration is created if necessary    
    TInt index = CreateContextConfigurationL( aContext );    
    
    CNcdKeyValueMap& map( ConfigurationByCategory( index, aConfig->Key() ) );
    
    if ( aConfig->Key() == NcdConfigurationKeys::KCapability ) 
        {
        DLTRACE(("Adding a capability"));
        if ( map.PairExists( *aConfig ) == KErrNotFound ) 
            {            
            map.AddL( aConfig );
            }
        else 
            {
            DLTRACE(("Capability already exists"));
            CleanupStack::PopAndDestroy( aConfig );
            return;
            }
        }
    else 
        {
        DLTRACE(("Adding something that is not a capability"));
        // Replaces the old key if necessary
        map.ReplaceL( aConfig );

        if ( aConfig->Key() == NcdConfigurationKeys::KMasterServer ) 
            {
            // Ensure that the new master server is used instead of hardcoded one
            iConfigurations[index]->iUseHardcodedMasterServer = EFalse;
            }
        }
                
    CleanupStack::Pop( aConfig );
    
    DLTRACEOUT(("" ));
    }


// ---------------------------------------------------------------------------
// RemoveConfiguration
// ---------------------------------------------------------------------------
//
TInt CNcdConfigurationManager::RemoveConfigurationL( 
    const MCatalogsContext& aContext, const TDesC& aKey )
    {
    DLTRACEIN((""));
    TInt index = FindConfigurationL( aContext );
    if ( index != KErrNotFound ) 
        {
        return ConfigurationByCategory( index, aKey ).Remove( aKey );        
        }
    return index;    
    }
    
    
// ---------------------------------------------------------------------------
// ConfigurationsL
// ---------------------------------------------------------------------------
//
CNcdKeyValueMap* CNcdConfigurationManager::ConfigurationsLC( 
    const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    TInt index = FindConfigurationL( aContext );
    if ( index == KErrNotFound ) 
        {
        User::Leave( KErrNotFound );
        }
    
    DASSERT( iConfigurations[index]->iConfigurationGeneral && 
        iConfigurations[index]->iConfigurationClientInfo );
        
    // Create a new map and append all different configuration categories to
    // it
    CNcdKeyValueMap* newMap = CNcdKeyValueMap::NewL( 
        *iConfigurations[index]->iConfigurationGeneral );
    CleanupStack::PushL( newMap );
    
    newMap->AppendL( *iConfigurations[index]->iConfigurationClientInfo );
        
    DLTRACEOUT((""));
    return newMap;
    }


// ---------------------------------------------------------------------------
// ConfigurationsL
// ---------------------------------------------------------------------------
//        
CNcdKeyValueMap& CNcdConfigurationManager::ConfigurationsL(
    const MCatalogsContext& aContext, 
    TNcdConfigurationCategory aCategory )
    {
    TInt index = FindConfigurationL( aContext );
    if ( index == KErrNotFound ) 
        {
        User::Leave( KErrNotFound );
        }
    
    return ConfigurationByCategory( index, aCategory );  
    }

    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//        
CNcdConfigurationManager::CNcdConfigurationManager( 
    CNcdGeneralManager& aGeneralManager ) : 
    iGeneralManager( aGeneralManager),
    iStorageManager( aGeneralManager.StorageManager() )
    {
    }
    
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::ConstructL()
    {
    }
    

// ---------------------------------------------------------------------------
// Generates a new client id
// ---------------------------------------------------------------------------
//        
HBufC* CNcdConfigurationManager::GenerateClientIdLC() const
    {
    DLTRACEIN((""));
#ifdef USE_OLD_CLIENT_ID_GENERATOR    
    // Enough for timestamp (64bit) + rand (32bit)
    HBufC* id = HBufC::NewLC( NcdProviderDefines::KClientIdMaxLength );  

    TTime now;
    now.HomeTime();
    TInt64 int64 = now.Int64();

    TInt rand = Math::Rand( int64 );

    TPtr ptr( id->Des() );


    ptr.NumFixedWidth( int64 >> 32, EHex, 8 );
    ptr.AppendNumFixedWidth( int64 & 0xffffffff, EHex, 8 );
    ptr.AppendNumFixedWidth( rand, EHex, 8 );
    DLTRACEOUT(( _L("Client ID: %S"), id ));
#else


    // Enough for timestamp (64bit) + rand (32bit)
    HBufC* id = HBufC::NewLC( NcdProviderDefines::KClientIdMaxLength );  

    TTime now;
    now.HomeTime();
    TInt64 int64 = now.Int64();
    

    TInt64 freeSpace = FreeDiskSpaceL( CNcdProviderUtils::FileSession(),
        EDriveC ) + int64;            

    // Use both timestamp and free disk space to get a unique seed
    TInt rand = Math::Rand( freeSpace );

    TPtr ptr( id->Des() );


    ptr.NumFixedWidth( int64 >> 32, EHex, 8 );
    ptr.AppendNumFixedWidth( int64 & 0xffffffff, EHex, 8 );
    ptr.AppendNumFixedWidth( rand, EHex, 8 );
    DLTRACEOUT(( _L("Client ID: %S"), id ));


#endif    
    return id;
    }
    
    
// ---------------------------------------------------------------------------
// MatchContexts
// ---------------------------------------------------------------------------
//    
TBool CNcdConfigurationManager::MatchContexts( 
    const CContextConfiguration& aFirst,
    const CContextConfiguration& aSecond )
    {
    return ( aFirst.iFamilyId == aSecond.iFamilyId );
    }
    

// ---------------------------------------------------------------------------
// FindConfiguration
// ---------------------------------------------------------------------------
//        
TInt CNcdConfigurationManager::FindConfigurationL( 
    const MCatalogsContext& aContext ) const
    {
    TIdentityRelation<CContextConfiguration> match( 
        CNcdConfigurationManager::MatchContexts );

    CContextConfiguration* config = CContextConfiguration::FindNewL( aContext );

    TInt index = iConfigurations.Find( config, match );
    delete config;
    return index;
    }


// ---------------------------------------------------------------------------
// DetermineCategory
// ---------------------------------------------------------------------------
//        
MNcdUserConfiguration::TNcdConfigurationCategory 
    CNcdConfigurationManager::DetermineCategory(
    const TDesC& aKey ) const
    {
    if ( aKey == NcdConfigurationKeys::KMasterServer 
         || aKey == NcdConfigurationKeys::KMaxStorageSize )
        {
        return ENcdConfigurationGeneral;
        }
    return ENcdConfigurationClientInfo;
    }


// ---------------------------------------------------------------------------
// ConfigurationByCategory
// ---------------------------------------------------------------------------
//        
CNcdKeyValueMap& CNcdConfigurationManager::ConfigurationByCategory( 
    TInt aIndex, const TDesC& aKey )
    {
    TNcdConfigurationCategory category = DetermineCategory( aKey );
    return ConfigurationByCategory( aIndex, category );
    }
    
// ---------------------------------------------------------------------------
// ConfigurationByCategory
// ---------------------------------------------------------------------------
//        
CNcdKeyValueMap& CNcdConfigurationManager::ConfigurationByCategory( TInt aIndex, 
    TNcdConfigurationCategory aCategory )
    {
    
    switch( aCategory )
        {
        case ENcdConfigurationGeneral:
            {
            return *iConfigurations[aIndex]->iConfigurationGeneral;
            }
        
        case ENcdConfigurationClientInfo:
            {
            return *iConfigurations[aIndex]->iConfigurationClientInfo;
            }
        default:
            {
            DASSERT( 0 );
            }
        }
    
    // For the compiler
    CNcdKeyValueMap* ignore = NULL;    
    return *ignore;
    }
    


// ---------------------------------------------------------------------------
// Create context configuration
// ---------------------------------------------------------------------------
//        
TInt CNcdConfigurationManager::CreateContextConfigurationL( 
    const MCatalogsContext& aContext )
    {    
    DLTRACEIN((""));
    
    TInt index = FindConfigurationL( aContext );
    if ( index == KErrNotFound ) 
        {     
                   
        DLTRACE(( "Creating a new configuration for context" ));
        
        // Create new config
        CContextConfiguration* newConfig = 
            CContextConfiguration::NewLC( aContext, KNullDesC() );
        
        iConfigurations.AppendL( newConfig );    
        CleanupStack::Pop( newConfig );        
        
        // Try to load data from database
        TRAPD( err, LoadDataL( aContext.FamilyId().Name(), 
            NcdProviderDefines::ENcdContextData, *newConfig ) );
            
        if ( err != KErrNone && err != KErrNotFound ) 
            {
            DLERROR(( "Context configuration loading failed with: %d", err ));
            User::Leave( err );
            }
        
        // update the index
        index = iConfigurations.Count() - 1;
        }
    
    DLTRACEOUT(( "index: %d", index ));    
    return index;    
    }
    
    
   
// ---------------------------------------------------------------------------
// Saves data
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::SaveDataL( const TDesC& aId,
    NcdProviderDefines::TNcdDatabaseDataType aType, 
    MNcdStorageDataItem& aDataItem )
    {
    DLTRACEIN(( _L("Id: %S, type: %d"), &aId, aType ));
    MNcdStorage& storage = iStorageManager.ProviderStorageL(
        iGeneralManager.FamilyName() );
    
    // NOTE: this creates the database if it does not already exist
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( 
            NcdProviderDefines::KDefaultDatabaseUid );
            
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item
    MNcdStorageItem* item = 
        database.StorageItemL( aId, 
            aType );    
    
    
    // Save new item to database
    item->SetDataItem( &aDataItem );
    item->OpenL();
    
    // Calls ExternalizeL for the node
    item->WriteDataL();
    
    // Save the node to the database.
    // The node object implements MNcdStorageDataItem interface.
    // So, the externalize function will insert the data to the stream
    // that the database handler will save to the database.
    item->SaveL();      
    DLTRACEOUT((""));      
    }


// ---------------------------------------------------------------------------
// Loads data
// ---------------------------------------------------------------------------
//        
void CNcdConfigurationManager::LoadDataL( const TDesC& aId,
    NcdProviderDefines::TNcdDatabaseDataType aType, 
    MNcdStorageDataItem& aDataItem )
    {
    DLTRACEIN(( _L("Id: %S, type: %d"), &aId, aType ));
    MNcdStorage& storage = iStorageManager.ProviderStorageL( 
        iGeneralManager.FamilyName() );
    
    // NOTE: this creates the database if it does not already exist
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( 
            NcdProviderDefines::KDefaultDatabaseUid );


   if( !database.ItemExistsInStorageL( aId, aType ) )
        {
        DLINFO(("Item not found"));
        User::Leave( KErrNotFound );
        }
            
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item
    MNcdStorageItem* item = 
        database.StorageItemL( aId, 
            aType );    
               
    item->SetDataItem( &aDataItem );
    item->ReadDataL();    
    
    DLTRACEOUT((""));      
    }
