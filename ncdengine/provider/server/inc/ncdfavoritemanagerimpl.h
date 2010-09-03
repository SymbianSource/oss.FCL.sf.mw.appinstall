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


#ifndef NCDFAVORITEMANAGER_H
#define NCDFAVORITEMANAGER_H

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"

class CNcdNodeManager;
class CNcdNodeIdentifier;
class RWriteStream;
class MNcdStorageManager;
class CNcdNodeDisclaimer;
class CNcdGeneralManager;

/**
 * CNcdFavoriteManager manages the list of favorite nodes.
 */
class CNcdFavoriteManager : public CCatalogsCommunicable 
{
public:
    /**
     * NewL
     *
     */
    static CNcdFavoriteManager* NewL( CNcdGeneralManager& aGeneralManager );
    
    
    /**
     * NewLC
     *
     */
    static CNcdFavoriteManager* NewLC( CNcdGeneralManager& aGeneralManager );

    /**
     * Destructor
     */
    virtual ~CNcdFavoriteManager();
    
    /**
     * Returns the number of clients whose favorite identifiers has been loaded to
     * ram cache.
     */
    TInt ClientCount() const;
    
    /**
     * Returns a client uid by the given index.
     *
     * @param aClientIndex The index.
     * @return The client uid.
     * @leave KErrNotFound If the given index is out of range.
     */
    TUid ClientL( TInt aClientIndex ) const; 
    
    /**
     * Returns the client's favorite nodes.
     *
     * @param aClientUid The client uid.
     * @return The favorite nodes.
     * @leave System wide error code.
     */
    const RPointerArray<CNcdNodeIdentifier>& FavoriteNodesL(
        const TUid& aClientUid );
        
    /**
     * Tells whether the given node is a favorite node.
     *
     * @param aNodeIdentifier The node identifier.
     * @return ETrue if the node is a favorite node, otherwise false.
     */
    TBool IsFavorite( const CNcdNodeIdentifier& aNodeIdentifier ) const;
             

    /**
     * Removes client's favorites from database
     */
    void RemoveFavoritesL( const TUid& aClientUid );
    
public: // from CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage(
        MCatalogsBaseMessage* aMessage, 
        TInt aFunctionNumber );
        
    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost(
        const MCatalogsSession& aSession );
    
private:

    /**
     * CNcdClientFavorites object encapsulates the favorite identifiers of one
     * client.
     */
    class CNcdClientFavorites : public CBase,
                                public MNcdStorageDataItem
        {
    public:
        static CNcdClientFavorites* NewLC( const TUid& aClientUid );
        ~CNcdClientFavorites();
        
        TUid ClientUid() const;
        void AddFavoriteL( CNcdNodeIdentifier* aNodeIdentifier );
        void RemoveFavorite( const CNcdNodeIdentifier& aNodeIdentifier );
        void RemoveFavorites();
        void SetDisclaimerL(
            const CNcdNodeIdentifier& aNodeIdentifier,
            const CNcdNodeDisclaimer& aDisclaimer );
        void RemoveDisclaimer( const CNcdNodeIdentifier& aNodeIdentifier );
        CNcdNodeDisclaimer* Disclaimer(
            const CNcdNodeIdentifier& aNodeIdentifier ) const;
            
        TBool HasFavorite( const CNcdNodeIdentifier& aNodeIdentifier ) const;
        
        const RPointerArray<CNcdNodeIdentifier>& Identifiers() const;
        void ExternalizeIdentifiersL( RWriteStream& aStream ) const;
                
    public: // from MNcdStorageDataItem

        /* 
        * @see MNcdStorageDataItem::ExternalizeL
        */
        virtual void ExternalizeL( RWriteStream& aStream );

        /* 
        * @see MNcdStorageDataItem::InternalizeL
        */
        virtual void InternalizeL( RReadStream& aStream );
        
    protected:
        CNcdClientFavorites( const TUid& aClientUid );
        void CloseDisclaimers();
        
    private: 
        
        // Node identifiers.
        RPointerArray<CNcdNodeIdentifier> iIdentifiers;
        // Node disclaimers. A disclaimer at index 'i' is disclaimer of the node at
        // the same index.
        RPointerArray<CNcdNodeDisclaimer> iDisclaimers;
        
        // Client uid.
        TUid iClientUid;
        };
        
        
    class CNcdTemporaryFavorites : public CBase 
        {
    public: // public functions
        CNcdTemporaryFavorites( MCatalogsSession& aSession );
        ~CNcdTemporaryFavorites();
        
        
    public: // member variables
        MCatalogsSession& iSession;
        RPointerArray<CNcdNodeIdentifier> iFavoriteIdentifiers;
        };
        
    CNcdGeneralManager& iGeneralManager;
    
    // Node manager.        
    CNcdNodeManager& iNodeManager;
    
    // Favorite nodes of all the clients.
    RPointerArray<CNcdClientFavorites> iFavorites;
    
    // Temp favorite mappings.
    RPointerArray<CNcdTemporaryFavorites> iTempFavorites;
    
    // Storage manager.
    MNcdStorageManager& iStorageManager;       
    
protected:
    
    CNcdFavoriteManager( CNcdGeneralManager& aGeneralManager );
    
    void ConstructL();
    
    /**
     * Called from ReceiveMessage if favorite is added.
     */
    void AddFavoriteRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * Called from ReceiveMessage if favorite is removed.
     */
    void RemoveFavoriteRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Called from ReceiveMessage if disclaimer need to be added or removed.
     */
    void SetDisclaimerRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Called from ReceiveMessage if disclaimer handle is requested.
     */
    void DisclaimerHandleRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Called from ReceiveMessage if proxy object wants to release the server side
     * object.
     */    
    void ReleaseRequest( MCatalogsBaseMessage& aMessage );
    
    /**
     * Called from ReceiveMessage if proxy object wants to internalize its state
     * from server side.
     */    
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Adds the given favorite node.
     *
     * @param aNodeIdentifier The identifier.
     */
    void AddFavoriteL( CNcdNodeIdentifier* aNodeIdentifier );
    
    /**
     * Adds the given favorite node temporarily. The node is removed from
     * favorites when the given session is closed.
     *
     * If the node is favorite node already, nothing is done.
     *
     * @param aNodeIdentifier The identifier.
     * @param aSession The session.
     */
    void AddTemporaryFavoriteL(
        CNcdNodeIdentifier* aNodeIdentifier,
        MCatalogsSession& aSession );
    
    /**
     * Removes the given favorite node.
     *
     * @param aNodeIdentifier The identifier.
     */
    void RemoveFavoriteL( CNcdNodeIdentifier* aNodeIdentifier );
    
    /**
     * Removes the temporary favorites of the given session.
     *
     * @param aSession The session.
     */
    void RemoveTemporaryFavoritesL( MCatalogsSession& aSession );
    
    /**
     * Externalizes the favorite identifiers of the given client to the given stream.
     *
     * @param aStream The stream.
     * @param aClientUid The client uid.
     */
    void ExternalizeIdentifiersL( RWriteStream& aStream, const TUid& aClientUid );
    
    /**
     * Finds the favorite node set of the given client.
     *
     * @param aClientUid The client uid.
     * @return Index of the set in iFavorites member variable or KErrNone if the
     *         client does not exist.
     */
    TInt FindClientFavorites( const TUid& aClientUid ) const;
    
    /**
     * Returns the temporary favorites of the given session. The given
     * object is created if there is no existing CNcdTemporaryFavorites object
     * for the given session.
     *
     * @param aSession The session.
     * @return The temporary favorites of the session.
     */
    CNcdTemporaryFavorites& TemporaryFavoritesL( MCatalogsSession& aSession );
    
    /**
     * Tells whether the given session has temporary favorites.
     *
     * @param aSession The session.
     * @return ETrue if there are temporary favorites, otherwise false.
     */
    TBool HasTemporaryFavorites( MCatalogsSession& aSession ) const;
    
    /**
     * Loads the client's favorite identifiers from database. If the database
     * does not contain any favorite identifiers, creates an empty identifier set
     * for the client to iFavorites member variable.
     *
     * @param aClientUid The client uid.
     */
    void DbLoadFavoritesL( const TUid& aClientUid );
    
    /**
     * Saves the given client's favorite identifiers to the database.
     *
     * @param aFavorites The favorites to save.
     */ 
    void DbSaveFavoritesL( CNcdClientFavorites& aFavorites );    
    
    
};

#endif
