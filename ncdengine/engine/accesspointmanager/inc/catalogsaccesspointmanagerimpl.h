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
* Description:   Class CCatalogsAccessPointManager declation
*
*/


#ifndef C_CATALOGSACCESSPOINTMANAGERIMPL_H
#define C_CATALOGSACCESSPOINTMANAGERIMPL_H

#include <e32def.h>
#include <e32base.h> // HLa: DLMgr remove

#ifdef _0
#include <ApAccessPointItem.h>
#endif

#include <e32cmn.h>

#include "catalogsaccesspointmanager.h"
#include "catalogsaccesspointobserver.h"

class CCatalogsAccessPointFilterSettings;
class CCatalogsAccessPointSettings;
class CCatalogsAccessPoint;
class CCatalogsClientAccessPointData;
class MNcdStorageManager;
class CNcdNodeManager;
class CNcdGeneralManager;

class CCatalogsAccessPointManager: public CBase,
                                   public MCatalogsAccessPointManager,
                                   public MCatalogsAccessPointObserver
{
public:
    static CCatalogsAccessPointManager* NewL(
        CNcdGeneralManager& aGeneralManager );
        
    ~CCatalogsAccessPointManager();
    

public: // from MCatalogsAccessPointManager

    /**
     * @see MCatalogsAccessPointManager::SetFixedApL.
     */
    virtual void SetFixedApL( const RPointerArray<CNcdKeyValuePair>& aApDetails ); 
    
    /**
     * @see MCatalogsAccessPointManager::GetFixedApL
     */
    virtual void GetFixedApL( TUint32& aAccessPointId );

    /**
     * @see MCatalogsAccessPointManager::ParseAccessPointDataFromClientConfL.
     */
    virtual void ParseAccessPointDataFromClientConfL(
        const MNcdConfigurationProtocolClientConfiguration& aConfiguration,
        const TUid& aClientUid, 
        TBool aIgnoreFixedAp, 
        RArray<TUint32>* aCreatedAps );
        
        
    /**
     * @see MCatalogsAccessPointManager::AccessPointIdL.
     */
    TInt AccessPointIdL(
        const TDesC& aNameSpace,
        const MCatalogsAccessPointManager::TAction& aAction, const TUid& aClientUid,
        TUint32& aAccessPointId);
        
    /**
     * @see MCatalogsAccessPointManager::AccessPointIdL.
     */
    TInt AccessPointIdL(
        const TDesC& aNameSpace, const TDesC& aCatalogId,
        const MCatalogsAccessPointManager::TAction& aAction, const TUid& aClientUid,
        TUint32& aAccessPointId);
        
    /**
     * @see MCatalogsAccessPointManager
     */
    TInt AccessPointIdL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const MCatalogsAccessPointManager::TAction& aAction, const TUid& aClientUid,
        TUint32& aAccessPointId);

    static void RemoveApFromCommsDatabaseL( const TUint32& aId );
    
public: // From MCatalogsAccessPointObserver

    void HandleAccessPointEventL( 
        const TCatalogsConnectionMethod& aAp,
        const TCatalogsAccessPointEvent& aEvent );


private:

    CCatalogsAccessPointManager(
        CNcdGeneralManager& aGeneralManager );
    
    void ConstructL();

    /**
     * Finds the access point settings for the given node from the
     * given access point data. NOTE! The node represented by the given
     * identifier must exist, otherwise the function leaves.
     *
     * @param aNodeIdentifier The node identifier.
     * @param aAction The action type.
     * @param aAccessPointData The data to search from.
     * @return The access point or NULL if not found.
     */    
    CCatalogsAccessPoint* FindAccessPointL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const MCatalogsAccessPointManager::TAction& aAction,
        const CCatalogsClientAccessPointData& aAccessPointData );

    
    /**
     * Maps the access point for given namespace and action.
     *
     * @param aNameSpace Namespace of the content source.
     * @param aAction The action.
     * @param aApId The access point id obtained from client configuration.
     * @param aClientUid Uid of the client.
     */
    void MapAccessPointL(
        const TDesC& aNameSpace, const TAction& aAction,
        const TDesC& aApId );
        
    /**
     * Maps an access point for the given namespace, ID and action.
     *
     * @param aNameSpace Namespace of the catalog.
     * @param aCatalogId ID of the catalog.
     * @param aAction The action.
     * @param aApId The access point ID obtained from client configuration.
     * @param aClientUid Uid of the client.
     */
    void MapAccessPointL(
        const TDesC& aNameSpace, const TDesC& aCatalogId, const TAction& aAction,
        const TDesC& aApId );
        
    /**
     * Create accesspoint to commsDB if corresponding accesspoint does not exist.
     * Add the given access point to access point datas of the given client.
     * 
     * @param aSettings The access point settings.
     * @param aClientUid Uid of the client.
     */
    void CreateAccessPointL( CCatalogsAccessPoint* aSettings );
    
    /**
     * Creates the access point to commsDB if corresponding access point does not exist.
     * Sets the id of the access point in commsDB to aSettings object.
     *
     * @param aSettings The access point settings.
     */
    void CreateAccessPointL( CCatalogsAccessPoint& aSettings );
    
    /**
     * Removes the client's accesspoint settings from the comms database.
     * 
     * @param aClientUid UID of the client.
     * @param aAddShutdownOperation If true, a shutdown operation is
     * created if removing the accesspoint fails that will try to remove it
     * later
     */
    void RemoveAccessPointsFromCommsDbL(
        TBool aAddShutdownOperation );
    
    /**
     * Creates the access points from the client configuration.
     *
     * @param aDetail The detail of which id is "accessPoints".
     * @param aClientUid The Uid of the owner of the access points.
     */
    void CreateAccessPointsFromClientConfL(
        const MNcdConfigurationProtocolDetail& aDetail, 
        RArray<TUint32>* aCreatedAps );
    /**
     * Removes client's access point datas from the internal data structures and
     * from the comms database.
     *
     */
    void RemoveAccessPointsL();
        
#ifdef _0
    HBufC16* ReadText16L( CApAccessPointItem* aItem, const TApMember& aApMember );

    HBufC8* ReadText8L( CApAccessPointItem* aItem, const TApMember& aApMember );
#endif 
    
    TBool MatchInCommsDbL( const CCatalogsAccessPointSettings& aSettings, TUint32& aId );

    void CreateApToCommsDBL( const CCatalogsAccessPoint& aSettings, TUint32& aId );

    TBool ValidateAccessPointL( CCatalogsAccessPoint* aAccessPoint ); 

    void LoadFromStorageL();

    void SaveToStorageL();
    
        
#ifdef _0
    TBool MatchingSettingsL(const CCatalogsAccessPointSettings& aSettings, CApAccessPointItem& aItem);
#endif
    
    void SetApDetailL( CCatalogsAccessPoint& aAp, const TDesC& aKey, const TDesC& aValue );

    TInt GetAccessPoint( 
        CCatalogsAccessPoint*& aAccessPoint,
        CCatalogsClientAccessPointData*& aApData );

    TInt ValidateOrCreateAccessPointL(
        CCatalogsAccessPoint* aAccessPoint,
        TUint32& aAccessPointId );

    void DestructL();

private:
    
    CNcdGeneralManager& iGeneralManager;
    // Storage manager is used to load and to save data from
    // databases.
    MNcdStorageManager& iStorageManager;
    
    CNcdNodeManager& iNodeManager;

    CCatalogsClientAccessPointData* iClientAccessPointData;
    
    CCatalogsAccessPoint* iFixedAp;
    TBool iCheckAp;
};

#endif
