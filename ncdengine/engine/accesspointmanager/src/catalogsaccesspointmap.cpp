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
* Description:   Class CCatalogsAccessPointMap implementation
*
*/


#include "catalogsaccesspointmap.h"
#include "catalogsutils.h"

CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewL(RReadStream& aStream) 
    {
    CCatalogsAccessPointMap* self = NewLC(aStream);
    CleanupStack::Pop();
    return self;
    }

CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewL(
    const TDesC& aNameSpace,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {
    CCatalogsAccessPointMap* self = NewLC(aNameSpace, aAction, aAccessPointId);
    CleanupStack::Pop();
    return self;
    }
    
CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {
    CCatalogsAccessPointMap* self = NewLC(
        aNameSpace, aCatalogId, aAction, aAccessPointId);
    CleanupStack::Pop();
    return self;
    }

CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewLC(RReadStream& aStream) 
    {
    CCatalogsAccessPointMap* self = new (ELeave) CCatalogsAccessPointMap;
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    return self;
    }
        
CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewLC(
    const TDesC& aNameSpace,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {    
    CCatalogsAccessPointMap* self = new (ELeave) CCatalogsAccessPointMap;
    CleanupStack::PushL(self);
    self->ConstructL(aNameSpace, aAction, aAccessPointId);
    return self;
    }

CCatalogsAccessPointMap* CCatalogsAccessPointMap::NewLC(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {    
    CCatalogsAccessPointMap* self = new (ELeave) CCatalogsAccessPointMap;
    CleanupStack::PushL(self);
    self->ConstructL(aNameSpace, aCatalogId, aAction, aAccessPointId);
    return self;
    }
    
CCatalogsAccessPointMap::~CCatalogsAccessPointMap() 
    {
    delete iNameSpace;
    delete iCatalogId;
    delete iAccessPointId;
    }
    
const TDesC& CCatalogsAccessPointMap::NameSpace() const 
    {
    return *iNameSpace;
    }
    
TBool CCatalogsAccessPointMap::HasCatalogId() const 
    {
    return iHasCatalogId;
    }

const TDesC& CCatalogsAccessPointMap::CatalogId() const 
    {
    if (iCatalogId != NULL)
        {
        return *iCatalogId;
        }
    else 
        {
        return KNullDesC;
        }
    }
    
CCatalogsAccessPointManager::TAction CCatalogsAccessPointMap::Action() const 
    {
    return iAction;
    }
    
    
const TDesC& CCatalogsAccessPointMap::AccessPointId() const 
    {
    return *iAccessPointId;
    }
    
void CCatalogsAccessPointMap::ExternalizeL(RWriteStream& aStream) 
    {
    ExternalizeDesL(*iNameSpace, aStream);
    ExternalizeDesL(*iAccessPointId, aStream);
    aStream.WriteInt8L(iHasCatalogId);
    if (iHasCatalogId) 
        {
        DASSERT(iCatalogId != NULL);
        ExternalizeDesL(*iCatalogId, aStream);
        }
    aStream.WriteInt32L(iAction);
    }
    
void CCatalogsAccessPointMap::InternalizeL(RReadStream& aStream) 
    {
    InternalizeDesL(iNameSpace, aStream);
    InternalizeDesL(iAccessPointId, aStream);
    iHasCatalogId = aStream.ReadInt8L();
    if (iHasCatalogId) 
        {
        InternalizeDesL(iCatalogId, aStream);
        }
    iAction = (CCatalogsAccessPointManager::TAction)aStream.ReadInt32L();
    }

CCatalogsAccessPointMap::CCatalogsAccessPointMap() 
    {
    }
        
void CCatalogsAccessPointMap::ConstructL(
    const TDesC& aNameSpace,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {
    iNameSpace = aNameSpace.AllocL();    
    iAction = aAction;
    iAccessPointId = aAccessPointId.AllocL();
    iHasCatalogId = EFalse;
    }

void CCatalogsAccessPointMap::ConstructL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aAccessPointId) 
    {
    ConstructL(aNameSpace, aAction, aAccessPointId);
    iCatalogId = aCatalogId.AllocL();
    iHasCatalogId = ETrue;
    }
