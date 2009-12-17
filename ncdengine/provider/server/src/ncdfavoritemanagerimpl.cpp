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
* Description:   Contains CNcdFavoriteManager class
*
*/


#include <s32mem.h>

#include "ncdfavoritemanagerimpl.h"
#include "ncdnodefunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogsconstants.h"
#include "catalogscontext.h"
#include "ncdnodeidentifier.h"
#include "ncdstorage.h"
#include "ncdstoragemanagerimpl.h"
#include "ncdstorageitem.h"
#include "ncdproviderdefines.h"
#include "ncddatabasestorage.h"
#include "ncdnodemanager.h"
#include "ncdnodecachecleanermanager.h"
#include "ncdnodecachecleaner.h"
#include "ncdnodedbmanager.h"
#include "ncdnodedisclaimer.h"
#include "ncdnodeimpl.h"
#include "ncdnodemetadataimpl.h"
#include "catalogsutils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

CNcdFavoriteManager* CNcdFavoriteManager::NewL(
    CNcdGeneralManager& aGeneralManager ) 
    {
    CNcdFavoriteManager* self = NewLC( aGeneralManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdFavoriteManager* CNcdFavoriteManager::NewLC(
    CNcdGeneralManager& aGeneralManager ) 
    {
    CNcdFavoriteManager* self =
        new( ELeave ) CNcdFavoriteManager( aGeneralManager );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }
    
CNcdFavoriteManager::CNcdFavoriteManager(
    CNcdGeneralManager& aGeneralManager ) 
    : iGeneralManager( aGeneralManager ),
      iNodeManager( aGeneralManager.NodeManager() ), 
      iStorageManager( aGeneralManager.StorageManager() )
    {
    }
    
void CNcdFavoriteManager::ConstructL() 
    {
    DLTRACEIN((""));
    }
    
CNcdFavoriteManager::~CNcdFavoriteManager() 
    {
    iFavorites.ResetAndDestroy();
    iTempFavorites.ResetAndDestroy();
    }
    
TInt CNcdFavoriteManager::ClientCount() const 
    {
    return iFavorites.Count();
    }
    
TUid CNcdFavoriteManager::ClientL( TInt aClientIndex ) const 
    {
    if ( aClientIndex < 0 || aClientIndex >= iFavorites.Count() ) 
        {
        User::Leave( KErrNotFound );
        }
    return iFavorites[ aClientIndex ]->ClientUid();
    }

const RPointerArray<CNcdNodeIdentifier>& CNcdFavoriteManager::FavoriteNodesL(
    const TUid& aClientUid ) 
    {
    DLTRACEIN((""));

    // Load client's favorites from DB, if they are not loaded.
    DbLoadFavoritesL( aClientUid );
    
    TInt index = FindClientFavorites( aClientUid );
    DASSERT( index != KErrNotFound );
    CNcdClientFavorites* clientFavorites = iFavorites[ index ];
    
    return clientFavorites->Identifiers();
    }


TBool CNcdFavoriteManager::IsFavorite(
    const CNcdNodeIdentifier& aNodeIdentifier ) const 
    {
    DLTRACEIN((""));
    const TUid& clientUid = aNodeIdentifier.ClientUid();
    
    TInt index = FindClientFavorites( clientUid );
    if ( index == KErrNotFound ) 
        {
        return EFalse;
        }
        
    CNcdClientFavorites* clientFavorites = iFavorites[ index ];
    return clientFavorites->HasFavorite( aNodeIdentifier );
    }
    

void CNcdFavoriteManager::RemoveFavoritesL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    TInt index = FindClientFavorites( aClientUid );
    if ( index != KErrNotFound ) 
        {
        CNcdClientFavorites& clientFavorites = *iFavorites[ index ];
        clientFavorites.RemoveFavorites();
        DbSaveFavoritesL( clientFavorites ); 
        }
    }
    
void CNcdFavoriteManager::ReceiveMessage(
    MCatalogsBaseMessage* aMessage, TInt aFunctionNumber ) 
    {
    DLTRACEIN((""));
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdFavoriteManagerAddFavorite:
            TRAP( trapError, AddFavoriteRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdFavoriteManagerRemoveFavorite:
            TRAP( trapError, RemoveFavoriteRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdFavoriteManagerSetDisclaimer:
            TRAP( trapError, SetDisclaimerRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdFavoriteManagerDisclaimerHandle:
            TRAP( trapError, DisclaimerHandleRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;            
        
        default:
            DASSERT( EFalse );
            break;
        }
    
    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }
    }
    

void CNcdFavoriteManager::CounterPartLost( const MCatalogsSession& /*aSession*/ ) 
    {
    DLTRACEIN((""));
    }
    
    
void CNcdFavoriteManager::AddFavoriteRequestL( MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));

    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );    
    
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( reader() );
    TBool removeOnDisconnect = reader().ReadInt8L();

    if ( removeOnDisconnect )
        {
        // Add the identifier with session information. Session
        // information is needed when the favorite must be removed, that is, when
        // the session is released.
        AddTemporaryFavoriteL( nodeId, aMessage.Session() );
        }
    else 
        {
        AddFavoriteL( nodeId );
        }
    
    CleanupStack::Pop( nodeId );
    CleanupStack::PopAndDestroy( &reader );
    
    aMessage.CompleteAndRelease( KErrNone );
    }


void CNcdFavoriteManager::RemoveFavoriteRequestL( MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));
    
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( reader() );
    RemoveFavoriteL( nodeId );
    
    CleanupStack::Pop( nodeId );
    CleanupStack::PopAndDestroy( &reader );
    
    aMessage.CompleteAndRelease( KErrNone );
    }
    
    
void CNcdFavoriteManager::SetDisclaimerRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    // Get the favorite node identifier.
    CNcdNodeIdentifier* favoriteIdentifier = CNcdNodeIdentifier::NewLC( reader() );
    DLNODEID(( *favoriteIdentifier ));
    TInt clientIndex = FindClientFavorites( favoriteIdentifier->ClientUid() );
    DASSERT( clientIndex != KErrNotFound );
    CNcdClientFavorites* favorites = iFavorites[ clientIndex ];
    
    // Read whether the stream contains the disclaimer owner.
    TBool isDisclaimer = reader().ReadInt32L();
    
    if ( isDisclaimer ) 
        {
        DLINFO(("Setting disclaimer for favorite node"));
        CNcdNodeIdentifier* disclaimerOwner = CNcdNodeIdentifier::NewLC( reader() );
        CNcdNode& node = iNodeManager.NodeL( *disclaimerOwner );
        CNcdNodeMetaData& metadata = node.NodeMetaDataL();
        const CNcdNodeDisclaimer* disclaimer = NULL;
        
        // ownership is not transferred
        // DisclaimerL leaves only with KErrNotFound
        TRAP_IGNORE( disclaimer = &metadata.DisclaimerL() );
        
        if ( !disclaimer )
            {
            DLINFO(("Getting disclaimer from favorites"));
            // ownership is not transferred
            disclaimer = favorites->Disclaimer( *disclaimerOwner );
            }
        
        if ( disclaimer )
            {
            favorites->SetDisclaimerL( *favoriteIdentifier, *disclaimer );
            }
        else
            {
            DLERROR(("No disclaimer found, leaving with KErrNotFound"));
            User::Leave( KErrNotFound );
            }
        
        CleanupStack::PopAndDestroy( disclaimerOwner );
        }
    else 
        {
        DLINFO(("Removing disclaimer from favorite node"));
        favorites->RemoveDisclaimer( *favoriteIdentifier );
        }
        
    CleanupStack::PopAndDestroy( favoriteIdentifier );
    CleanupStack::PopAndDestroy( &reader );
    
    // Update the state to db.
    DbSaveFavoritesL( *favorites );
    
    aMessage.CompleteAndRelease( KErrNone );
    }   
    
    
void CNcdFavoriteManager::DisclaimerHandleRequestL(
    MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));
    
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    // Get the favorite node identifier.
    CNcdNodeIdentifier* favoriteIdentifier = CNcdNodeIdentifier::NewL( reader() );
    
    CleanupStack::PopAndDestroy( &reader );
    
    TInt clientIndex = FindClientFavorites( favoriteIdentifier->ClientUid() );
    DASSERT( clientIndex != KErrNotFound );
    CNcdClientFavorites* favorites = iFavorites[ clientIndex ];
    CNcdNodeDisclaimer* disclaimer = favorites->Disclaimer( *favoriteIdentifier );
    
    delete favoriteIdentifier;
    favoriteIdentifier = NULL;
    
    if ( disclaimer == NULL ) 
        {
        // Disclaimer was not found for the node.
        User::Leave( KErrNotFound );
        }
        
    // Get the session that will contain the handle of the disclaimer.
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the icon to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( disclaimer ) );

    DLINFO(("Disclaimer handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );
    }
    

void CNcdFavoriteManager::ReleaseRequest( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    
    // Remove the temporary favorites of the session.
    TRAPD( err, RemoveTemporaryFavoritesL( requestSession ) );
    
    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( err );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );

    DLTRACEOUT((""));
    }
        
    
void CNcdFavoriteManager::AddFavoriteL( CNcdNodeIdentifier* aNodeIdentifier ) 
    {
    DLTRACEIN((""));
    DASSERT( aNodeIdentifier != NULL );
    
    TInt index = FindClientFavorites( aNodeIdentifier->ClientUid() );
    CNcdClientFavorites* favorites = iFavorites[ index ];
    
    // Add the node to node cache cleaner's block list, so it will never be removed
    // from db.    
    iNodeManager.NodeCacheCleanerManager().CacheCleanerL( 
        aNodeIdentifier->ClientUid() ).AddDoNotRemoveIdentifierL( 
            *aNodeIdentifier, ETrue );
        
    favorites->AddFavoriteL( aNodeIdentifier );    
    DbSaveFavoritesL( *favorites );
    }
    

void CNcdFavoriteManager::AddTemporaryFavoriteL(
    CNcdNodeIdentifier* aNodeIdentifier,
    MCatalogsSession& aSession )
    {
    DLTRACEIN((""));
    DASSERT( aNodeIdentifier != NULL );
     
    // The node is favorite already, do nothing.
    if ( IsFavorite( *aNodeIdentifier ) ) 
        {
        delete aNodeIdentifier;
        return;
        }

    CNcdNodeIdentifier* copy = CNcdNodeIdentifier::NewLC( *aNodeIdentifier );

    // Map the identifier and the session so that the node can be removed when the
    // session is released.
    CNcdTemporaryFavorites& tempFavorites = TemporaryFavoritesL( aSession );
    tempFavorites.iFavoriteIdentifiers.AppendL( copy );
    CleanupStack::Pop( copy );
    
    // Add the favorite normally.    
    AddFavoriteL( aNodeIdentifier );
    }

    
void CNcdFavoriteManager::RemoveFavoriteL( CNcdNodeIdentifier* aNodeIdentifier )
    {
    DLTRACEIN((""));
    DASSERT( aNodeIdentifier != NULL );
    
    TInt index = FindClientFavorites( aNodeIdentifier->ClientUid() );
    DASSERT( index != KErrNotFound );
    CNcdClientFavorites* favorites = iFavorites[ index ];

    // Remove the node from node db mananager's block list, so it can be removed from
    // db.    
    iNodeManager.NodeCacheCleanerManager().CacheCleanerL( 
        aNodeIdentifier->ClientUid() ).RemoveDoNotRemoveIdentifierL( 
            *aNodeIdentifier, ETrue );
    
    favorites->RemoveFavorite( *aNodeIdentifier );    
    DbSaveFavoritesL( *favorites );
    delete aNodeIdentifier;
    }
    
    
void CNcdFavoriteManager::RemoveTemporaryFavoritesL( MCatalogsSession& aSession ) 
    {
    DLTRACEIN((""));
    if ( HasTemporaryFavorites( aSession ) ) 
        {
        CNcdTemporaryFavorites& tempFavorites = TemporaryFavoritesL( aSession );
        for ( TInt i = tempFavorites.iFavoriteIdentifiers.Count() - 1; i >= 0; i-- ) 
            {
            // Remove the favorite, deletes also the given node identifier object.
            RemoveFavoriteL( tempFavorites.iFavoriteIdentifiers[ i ] );
            tempFavorites.iFavoriteIdentifiers.Remove( i );
            }
        }
    }
    
    
void CNcdFavoriteManager::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // We need session info in the functions for example to
    // check the client id (there can be several clients).
    MCatalogsSession& session = aMessage.Session();
    TUid clientUid = session.Context().FamilyId();
    
    // If the client favorites are not loaded from Db, do it now.
    DbLoadFavoritesL( clientUid );

    RCatalogsBufferWriter writer;
    writer.OpenLC();
    
    ExternalizeIdentifiersL( writer(), clientUid );
            
    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeIdentifiersL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( writer.PtrL(), KErrNone );
    
    CleanupStack::PopAndDestroy( &writer );
    }
    
void CNcdFavoriteManager::ExternalizeIdentifiersL(
    RWriteStream& aStream, const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    
    TInt index = FindClientFavorites( aClientUid );
    DASSERT( index != KErrNotFound );

    CNcdClientFavorites* favorites = iFavorites[ index ];
    favorites->ExternalizeIdentifiersL( aStream );    
    }

    
TInt CNcdFavoriteManager::FindClientFavorites( const TUid& aClientUid ) const
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iFavorites.Count(); i++ ) 
        {
        if ( iFavorites[ i ]->ClientUid() == aClientUid ) 
            {
            return i;
            }
        }
        
    return KErrNotFound;
    }
    
    
CNcdFavoriteManager::CNcdTemporaryFavorites& CNcdFavoriteManager::TemporaryFavoritesL(
    MCatalogsSession& aSession )
    {
    DLTRACEIN((""));

    // Find the correct CNcdTemporaryFavorites object from the array.        
    for ( TInt i = 0; i < iTempFavorites.Count(); i++ ) 
        {
        if ( &iTempFavorites[ i ]->iSession == &aSession ) 
            {
            return *iTempFavorites[ i ];
            }
        }
        
    // There was no existing CNcdTemporaryFavorites object, create one and return it.
    CNcdTemporaryFavorites* temp = new( ELeave ) CNcdTemporaryFavorites( aSession );
    CleanupStack::PushL( temp );
    iTempFavorites.AppendL( temp );
    CleanupStack::Pop( temp );
    return *temp;
    }
    
    
TBool CNcdFavoriteManager::HasTemporaryFavorites( MCatalogsSession& aSession ) const
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iTempFavorites.Count(); i++ ) 
        {
        if ( &iTempFavorites[ i ]->iSession == &aSession )
            {
            return iTempFavorites[ i ]->iFavoriteIdentifiers.Count() > 0;
            }
        }
    return EFalse;
    }
    
    
void CNcdFavoriteManager::DbLoadFavoritesL( const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    
    if ( FindClientFavorites( aClientUid ) != KErrNotFound ) 
        {
        // Client favorites already loaded, return
        return;
        }
    
    MNcdStorage& providerStorage = iStorageManager.ProviderStorageL(
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database =
        providerStorage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
    
    if( !database.ItemExistsInStorageL( aClientUid.Name(), 
        NcdProviderDefines::ENcdFavoriteManager ) ) 
        {
        // Create empty list of favorites
        CNcdClientFavorites* list = CNcdClientFavorites::NewLC( aClientUid );
        iFavorites.AppendL( list );
        CleanupStack::Pop( list );
        }
    else 
        {
        // Get the storage item from which the data is loaded
        // Note: database has the ownership of the item
        MNcdStorageItem* item = database.StorageItemL(
            aClientUid.Name(), NcdProviderDefines::ENcdFavoriteManager );    
        
        // Get data from database by using CNcdClientFavorites as the target so that 
        // internalize will be called for it
        CNcdClientFavorites* data = CNcdClientFavorites::NewLC( aClientUid );
        item->SetDataItem( data );
    
        // Read data -> calls CNcdClientFavorites::InternalizeL
        item->ReadDataL();
        
        iFavorites.AppendL( data );
        CleanupStack::Pop( data );
       
        // Add the favorites to node cache cleaner's list of nodes that must never be
        // removed from db.         
        iNodeManager.NodeCacheCleanerManager().CacheCleanerL( 
            aClientUid ).AddDoNotRemoveIdentifiersL( data->Identifiers(), ETrue );
        }
    }
    

void CNcdFavoriteManager::DbSaveFavoritesL( CNcdClientFavorites& aFavorites )
    {
    DLTRACEIN((""));
    
    MNcdStorage& providerStorage = iStorageManager.ProviderStorageL(
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database =
        providerStorage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
        
    // Get the storage item to which the access point manager is stored
    // Note: database has the ownership of the item
    MNcdStorageItem* item = database.StorageItemL(
        aFavorites.ClientUid().Name(), NcdProviderDefines::ENcdFavoriteManager);
    item->SetDataItem( &aFavorites );
    item->OpenL();
        
    // Calls ExternalizeL for this
    item->WriteDataL();
    item->SaveL();
    }
    
    
CNcdFavoriteManager::CNcdClientFavorites* CNcdFavoriteManager::CNcdClientFavorites::NewLC(
    const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    CNcdClientFavorites* self = new( ELeave ) CNcdClientFavorites( aClientUid );
    CleanupStack::PushL( self );
    return self;
    }
    
    
CNcdFavoriteManager::CNcdClientFavorites::CNcdClientFavorites( const TUid& aClientUid )
    : iClientUid( aClientUid ) 
    {
    }
    
    
CNcdFavoriteManager::CNcdClientFavorites::~CNcdClientFavorites() 
    {
    RemoveFavorites();
    }
    
TUid CNcdFavoriteManager::CNcdClientFavorites::ClientUid() const 
    {
    return iClientUid;
    }
    
void CNcdFavoriteManager::CNcdClientFavorites::AddFavoriteL(
    CNcdNodeIdentifier* aNodeIdentifier )  
    {
    DLTRACEIN((""));
    DASSERT( aNodeIdentifier != NULL );
    DASSERT( iIdentifiers.Count() == iDisclaimers.Count() );
    
    iIdentifiers.ReserveL( iIdentifiers.Count() + 1 );
    iDisclaimers.ReserveL( iDisclaimers.Count() + 1 );
    
    TInt err1 = iIdentifiers.Append( aNodeIdentifier );
    
    // Add NULL disclaimer for the node.
    TInt err2 = iDisclaimers.Append( NULL );
    
    DASSERT( err1 == KErrNone && err2 == KErrNone );
    }
    
void CNcdFavoriteManager::CNcdClientFavorites::RemoveFavorite(
    const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    TInt count = iIdentifiers.Count();
    DASSERT( iDisclaimers.Count() == count );
    
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( iIdentifiers[ i ]->Equals( aNodeIdentifier ) ) 
            {
            delete iIdentifiers[ i ];
            iIdentifiers.Remove( i );
            if ( iDisclaimers[ i ] ) 
                {
                iDisclaimers[ i ]->Close();
                }
            iDisclaimers.Remove( i );
            break;
            }
        }
    }


void CNcdFavoriteManager::CNcdClientFavorites::RemoveFavorites() 
    {
    CloseDisclaimers();
    iIdentifiers.ResetAndDestroy();    
    }
    
    
void CNcdFavoriteManager::CNcdClientFavorites::SetDisclaimerL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeDisclaimer& aDisclaimer ) 
    {
    DLTRACEIN((""));
    DLNODEID( aNodeIdentifier );
    DASSERT( iIdentifiers.Count() == iDisclaimers.Count() );
    TInt count = iIdentifiers.Count();
    
    for ( TInt i = 0; i < count; i++ ) 
        {
        CNcdNodeIdentifier* identifier = iIdentifiers[ i ];
        if ( identifier->Equals( aNodeIdentifier ) ) 
            {
            // Correct index found, copy the disclaimer.
            CNcdNodeDisclaimer* copy = CNcdNodeDisclaimer::NewL( aDisclaimer );
            if ( iDisclaimers[ i ] ) 
                {
                iDisclaimers[ i ]->Close();
                }
            iDisclaimers[ i ] = copy;
            break;
            }
        }
    }
    
    
void CNcdFavoriteManager::CNcdClientFavorites::RemoveDisclaimer(
    const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    DASSERT( iIdentifiers.Count() == iDisclaimers.Count() );
    TInt count = iIdentifiers.Count();
    
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( iIdentifiers[ i ]->Equals( aNodeIdentifier ) ) 
            {
            // Correct index found, delete the disclaimer.
            if ( iDisclaimers[ i ] ) 
                {                
                iDisclaimers[ i ]->Close();
                }
            iDisclaimers[ i ] = NULL;
            break;
            }
        }
    }
    
    
CNcdNodeDisclaimer* CNcdFavoriteManager::CNcdClientFavorites::Disclaimer(
    const CNcdNodeIdentifier& aNodeIdentifier ) const 
    {
    DASSERT( iIdentifiers.Count() == iDisclaimers.Count() );
    TInt count = iIdentifiers.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( iIdentifiers[ i ]->Equals( aNodeIdentifier ) ) 
            {
            return iDisclaimers[ i ];
            }
        }
        
    return NULL;
    }
    

TBool CNcdFavoriteManager::CNcdClientFavorites::HasFavorite(
    const CNcdNodeIdentifier& aNodeIdentifier ) const 
    {
    TInt count = iIdentifiers.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( iIdentifiers[ i ]->Equals( aNodeIdentifier ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
    
const RPointerArray<CNcdNodeIdentifier>& CNcdFavoriteManager::CNcdClientFavorites::Identifiers() const 
    {
    return iIdentifiers;
    }
    
    
void CNcdFavoriteManager::CNcdClientFavorites::ExternalizeIdentifiersL(
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    
    TInt count = iIdentifiers.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; i++ ) 
        {
        iIdentifiers[ i ]->ExternalizeL( aStream );
        }
    }
    
    
void CNcdFavoriteManager::CNcdClientFavorites::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    DASSERT( iIdentifiers.Count() == iDisclaimers.Count() );
    
    TInt count = iIdentifiers.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; i++ ) 
        {
        iIdentifiers[ i ]->ExternalizeL( aStream );
        if ( iDisclaimers[ i ] ) 
            {
            aStream.WriteInt8L( ETrue );
            iDisclaimers[ i ]->ExternalizeL( aStream );
            }
        else 
            {
            aStream.WriteInt8L( EFalse );
            }
        }    
    }
    
void CNcdFavoriteManager::CNcdClientFavorites::InternalizeL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    iIdentifiers.ResetAndDestroy();
  
    // Close all the disclaimer objects.
    CloseDisclaimers();
  
    // Reserve memory for the arrays.
    TInt count = aStream.ReadInt32L();
    
    iIdentifiers.ReserveL( count );
    iDisclaimers.ReserveL( count );
    
    // Read the stream.
    for ( TInt i = 0; i < count; i++ ) 
        {
        CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( aStream );
        CNcdNodeDisclaimer* disclaimer( NULL );
        TBool isDisclaimer = aStream.ReadInt8L();
        if ( isDisclaimer ) 
            {
            disclaimer = CNcdNodeDisclaimer::NewLC();
            disclaimer->InternalizeL( aStream );
            CleanupStack::Pop( disclaimer );
            }
        
        CleanupStack::Pop( identifier);
        
        TInt err1 = iIdentifiers.Append( identifier );
        TInt err2 = iDisclaimers.Append( disclaimer );
        
        // Memory was allocated earlier, so error should never occur.
        DASSERT( err1 == KErrNone && err2 == KErrNone );
        }
    }

void CNcdFavoriteManager::CNcdClientFavorites::CloseDisclaimers() 
    {
    TInt disclaimerCount = iDisclaimers.Count();
    for ( TInt i = 0; i < disclaimerCount; i++ ) 
        {
        if ( iDisclaimers[ i ] ) 
            {            
            iDisclaimers[ i ]->Close();
            }
        }
    iDisclaimers.Reset();
    }
    

CNcdFavoriteManager::CNcdTemporaryFavorites::CNcdTemporaryFavorites(
    MCatalogsSession& aSession ) 
    : iSession( aSession )
    {
    }
    
    
CNcdFavoriteManager::CNcdTemporaryFavorites::~CNcdTemporaryFavorites() 
    {
    DLTRACEIN((""));
    iFavoriteIdentifiers.ResetAndDestroy();
    }
