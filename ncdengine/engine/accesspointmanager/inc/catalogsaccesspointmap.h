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
* Description:   Class CCatalogsAccessPointMap declation
*
*/


#include <e32base.h>

#ifdef _0
#include "catalogsaccesspointmanagerimpl.h"
#endif

#include <s32strm.h>

#include "catalogsaccesspointmanagerimpl.h"

class CCatalogsAccessPointMap : public CBase
{
public:
    static CCatalogsAccessPointMap* NewL(RReadStream& aStream);

    static CCatalogsAccessPointMap* NewL(
        const TDesC& aNameSpace,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);

    static CCatalogsAccessPointMap* NewL(
        const TDesC& aNameSpace,
        const TDesC& aCatalogId,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);

    static CCatalogsAccessPointMap* NewLC(RReadStream& aStream);

    static CCatalogsAccessPointMap* NewLC(
        const TDesC& aNameSpace,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);

    static CCatalogsAccessPointMap* NewLC(
        const TDesC& aNameSpace,
        const TDesC& aCatalogId,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);
               
    ~CCatalogsAccessPointMap();
        
    const TDesC& NameSpace() const;

    TBool HasCatalogId() const;
    const TDesC& CatalogId() const;

    CCatalogsAccessPointManager::TAction Action() const;

    const TDesC& AccessPointId() const;
    
    void ExternalizeL(RWriteStream& aStream);
    void InternalizeL(RReadStream& aStream);
    
protected:
    CCatalogsAccessPointMap();
    void ConstructL(
        const TDesC& aNameSpace,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);
    void ConstructL(
        const TDesC& aNameSpace,
        const TDesC& aCatalogId,
        const CCatalogsAccessPointManager::TAction& aAction,
        const TDesC& aAccessPointId);

private:
    HBufC* iNameSpace;
    TBool iHasCatalogId;
    HBufC* iCatalogId;
    CCatalogsAccessPointManager::TAction iAction;
    HBufC* iAccessPointId;
};
