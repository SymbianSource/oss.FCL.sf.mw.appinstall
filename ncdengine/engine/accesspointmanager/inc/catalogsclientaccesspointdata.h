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
* Description:   Class CCatalogsClientAccessPointData declaration
*
*/


#ifndef C_CATALOGSCLIENTACCESSPOINTDATA_H
#define C_CATALOGSCLIENTACCESSPOINTDATA_H

#include "ncdstoragedataitem.h"
#include "catalogsaccesspointmanager.h"

class CCatalogsAccessPoint;
class CCatalogsAccessPointMap;

class CCatalogsClientAccessPointData : public CBase,
                                       public MNcdStorageDataItem 
{
public:
    static CCatalogsClientAccessPointData* NewL();
    static CCatalogsClientAccessPointData* NewLC();
    virtual ~CCatalogsClientAccessPointData();

    /**
     * Add the given access point to the access point list.
     *
     * @param aAccessPoint The access point.
     */    
    void AddAccessPointL(CCatalogsAccessPoint* aAccessPoint);

    /**
     * Map the access point for the given namespace and action.
     *
     * @param aNameSpace The namespace.
     * @param aType The action.
     * @param aApId Id of the access point to be mapped.
     */
    void MapAccessPointL(
        const TDesC& aNameSpace,
        const MCatalogsAccessPointManager::TAction& aType,
        const TDesC& aApId);
        
    /**
     * Map the access point for the given namespace, catalog ID
     * and action.
     *
     * @param aNameSpace The namespace.
     * @param aCatalogId ID of the catalog.
     * @param aType The action.
     * @param aApId Id of the access point to be mapped.
     */
    void MapAccessPointL(
        const TDesC& aNameSpace,
        const TDesC& aCatalogId,
        const MCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aApId);

    /**
     * Return the access point which is mapped for the given namespace
     * and action. The ownership is not returned. Returns NULL if
     * no access point is mapped.
     *
     * @param aNameSpace The namespace.
     * @param aAction The action.
     * @return The access point object or NULL.
     */
    CCatalogsAccessPoint* AccessPoint(
        const TDesC& aNameSpace,
        const MCatalogsAccessPointManager::TAction& aAction) const;
        
    /**
     * Return the access point which is mapped for the given namespace,
     * catalog ID and action. The ownership is not returned. 
     * Returns NULL if no access point is mapped.
     *
     * @param aNameSpace The namespace.
     * @param aCatalogId The catalog ID.
     * @param aAction The action.
     * @return The access point object or NULL.
     */
    CCatalogsAccessPoint* AccessPointL(
        const TDesC& aNameSpace,
        const TDesC& aCatalogId,
        const MCatalogsAccessPointManager::TAction& aAction) const;

    /**
     * Returns the number of access points in the list.
     *
     * @return The number of access points.
     */
    TInt AccessPointCount() const;

    /**
     * By the given index, returns an access point.
     *
     * @return An access point.
     */
    CCatalogsAccessPoint& AccessPoint(const TInt& aIndex);

    
public: // MNcdStorageDataItem
    void ExternalizeL(RWriteStream& aStream);
    void InternalizeL(RReadStream& aStream);
        
protected:
    CCatalogsClientAccessPointData();
    CCatalogsAccessPoint* AccessPointById(const TDesC& apId) const;
    
private:
    
    RPointerArray<CCatalogsAccessPoint> iAccessPoints;
    RPointerArray<CCatalogsAccessPointMap> iAccessPointMaps;
};

#endif
