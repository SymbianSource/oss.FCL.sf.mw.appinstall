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
* Description:   Class CCatalogsClientAccessPointData declation
*
*/


#include "catalogsclientaccesspointdata.h"
#include "catalogsaccesspoint.h"
#include "catalogsaccesspointmap.h"
#include "catalogsdebug.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeidentifiereditor.h"

CCatalogsClientAccessPointData* CCatalogsClientAccessPointData::NewL() 
    {
    CCatalogsClientAccessPointData* self = NewLC();
    CleanupStack::Pop();
    return self;
    }
    
CCatalogsClientAccessPointData* CCatalogsClientAccessPointData::NewLC()
    {
    CCatalogsClientAccessPointData* self = 
        new (ELeave) CCatalogsClientAccessPointData();
    CleanupStack::PushL(self);
    return self;
    }
    
CCatalogsClientAccessPointData::~CCatalogsClientAccessPointData() 
    {
    iAccessPoints.ResetAndDestroy();
    iAccessPointMaps.ResetAndDestroy();
    }
    
void CCatalogsClientAccessPointData::AddAccessPointL(CCatalogsAccessPoint* aAccessPoint) 
    {
    iAccessPoints.AppendL(aAccessPoint);
    }
    
void CCatalogsClientAccessPointData::MapAccessPointL(
    const TDesC& aNameSpace,
    const MCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aApId) 
    {
    // check if some access point is mapped already
    TInt mapCount = iAccessPointMaps.Count();
    for (TInt i = 0; i < mapCount; i++) 
        {
        CCatalogsAccessPointMap* map = iAccessPointMaps[i];
        if (map->NameSpace() == aNameSpace && map->Action() == aAction) 
            {
            // access point map found, delete it
            iAccessPointMaps.Remove(i);
            break;
            }
        }
     
    // create new map
    CCatalogsAccessPointMap* map = CCatalogsAccessPointMap::NewLC(
        aNameSpace, aAction, aApId);
    iAccessPointMaps.AppendL(map);
    CleanupStack::Pop(map);
    iAccessPointMaps.Compress();
    }

void CCatalogsClientAccessPointData::MapAccessPointL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const MCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aApId) 
    {
    // check if some access point is mapped already
    TInt mapCount = iAccessPointMaps.Count();
    for (TInt i = 0; i < mapCount; i++) 
        {
        CCatalogsAccessPointMap* map = iAccessPointMaps[i];
        if (map->NameSpace() == aNameSpace && map->HasCatalogId() &&
            map->CatalogId() == aCatalogId && map->Action() == aAction) 
            {
            // access point map found, delete it
            iAccessPointMaps.Remove(i);
            break;
            }
        }
     
    // create new map
    CCatalogsAccessPointMap* map = CCatalogsAccessPointMap::NewLC(
        aNameSpace, aCatalogId, aAction, aApId);
    iAccessPointMaps.AppendL(map);
    CleanupStack::Pop(map);
    iAccessPointMaps.Compress();
    }
    
CCatalogsAccessPoint* CCatalogsClientAccessPointData::AccessPoint(
    const TDesC& aNameSpace,
    const MCatalogsAccessPointManager::TAction& aAction) const
    {
    const TDesC* apId = NULL;        
    
    // find the access point id
    // find the map
    TInt mapCount = iAccessPointMaps.Count();
    for (TInt i = 0; i < mapCount; i++) 
        {
        CCatalogsAccessPointMap* map = iAccessPointMaps[i];
        if (map->NameSpace() == aNameSpace && map->Action() == aAction && !map->HasCatalogId()) 
            {
            // id found
            apId = &map->AccessPointId();
            break;
            }
        }

    if (!apId) 
        {        
        return NULL;
        }
    
    return AccessPointById(*apId);
    }

CCatalogsAccessPoint* CCatalogsClientAccessPointData::AccessPointL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const MCatalogsAccessPointManager::TAction& aAction) const
    {
    const TDesC* apId = NULL;        
    
    // Create metadata identifier and search the access point using it.
    // Fake clientUid can be used, we are only interested in namespace and id
    // at this point.
    CNcdNodeIdentifier* nodeId =
        CNcdNodeIdentifier::NewLC( aNameSpace, aCatalogId, TUid::Uid( 1 ) );         
    CNcdNodeIdentifier* metaDataId = 
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( *nodeId );
    CleanupStack::PopAndDestroy( nodeId );
    
    // find the access point id
    // find the map
    TInt mapCount = iAccessPointMaps.Count();
    for (TInt i = 0; i < mapCount; i++) 
        {
        CCatalogsAccessPointMap* map = iAccessPointMaps[i];
        if (map->NameSpace() == metaDataId->NodeNameSpace() && map->HasCatalogId() &&
            map->CatalogId() == metaDataId->NodeId() && map->Action() == aAction) 
            {
            // id found
            apId = &map->AccessPointId();
            break;
            }
        }

    delete metaDataId;
    metaDataId = NULL;
    
    if (!apId) 
        {        
        return NULL;
        }
    
    return AccessPointById(*apId);
    }
    
TInt CCatalogsClientAccessPointData::AccessPointCount() const 
    {
    return iAccessPoints.Count();
    }
    
CCatalogsAccessPoint& CCatalogsClientAccessPointData::AccessPoint(const TInt& aIndex)
    {
    return *iAccessPoints[aIndex];
    }

    
void CCatalogsClientAccessPointData::ExternalizeL(RWriteStream& aStream) 
    {
    DLTRACEIN((""));
    
    TInt apCount = iAccessPoints.Count();
    
    // write access points
    aStream.WriteInt32L(apCount);
    for (TInt i = 0; i < apCount; i++) 
        {
        iAccessPoints[i]->ExternalizeL(aStream);
        }
        
    TInt mapCount = iAccessPointMaps.Count();
    aStream.WriteInt32L(mapCount);
    
    // write access point maps
    for (TInt i = 0; i < mapCount; i++) 
        {
        iAccessPointMaps[i]->ExternalizeL(aStream);
        }
    DLTRACEOUT((""));
    }
    
void CCatalogsClientAccessPointData::InternalizeL(RReadStream& aStream) 
    {    
    DLTRACEIN((""));
    TInt apCount = aStream.ReadInt32L();
    for (TInt i = 0; i < apCount; i++) 
        {
        CCatalogsAccessPoint* ap = CCatalogsAccessPoint::NewLC(aStream);
        iAccessPoints.AppendL(ap);
        CleanupStack::Pop(); // ap
        }
        
    TInt mapCount = aStream.ReadInt32L();
    for (TInt i = 0; i < mapCount; i++) 
        {
        CCatalogsAccessPointMap* apMap = CCatalogsAccessPointMap::NewLC(aStream);
        iAccessPointMaps.AppendL(apMap);
        CleanupStack::Pop(); // apMap
        }
    }
     
    
CCatalogsClientAccessPointData::CCatalogsClientAccessPointData()
    {
    }
    
CCatalogsAccessPoint* CCatalogsClientAccessPointData::AccessPointById(
    const TDesC& apId) const 
    {
    TInt apCount = iAccessPoints.Count();
    for (TInt i = 0; i < apCount; i++) 
        {
        if (iAccessPoints[i]->NcdAccessPointId() == apId) 
            {
            return iAccessPoints[i];
            }
        }
    
    return NULL;
    }
    
