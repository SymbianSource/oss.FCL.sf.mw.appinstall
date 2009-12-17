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
* Description:   Class CCatalogsAccessPoint implementation
*
*/


#include "catalogsaccesspoint.h"
#include "catalogsutils.h"

CCatalogsAccessPoint* CCatalogsAccessPoint::NewL(
    const TDesC& aApNcdId, const TDesC& aName )
    {
    CCatalogsAccessPoint* self = NewLC( aApNcdId, aName );
    CleanupStack::Pop();
    return self;
    }

CCatalogsAccessPoint* CCatalogsAccessPoint::NewL( RReadStream& aStream )
    {
    CCatalogsAccessPoint* self = NewLC( aStream );
    CleanupStack::Pop();
    return self;
    }

CCatalogsAccessPoint* CCatalogsAccessPoint::NewLC(
    const TDesC& aApNcdId, const TDesC& aName )
    {
    CCatalogsAccessPoint* self = new (ELeave) CCatalogsAccessPoint();
    CleanupStack::PushL( self );
    self->ConstructL( aApNcdId, aName );    
    return self;
    }

CCatalogsAccessPoint* CCatalogsAccessPoint::NewLC( RReadStream& aStream )
    {
    CCatalogsAccessPoint* self = new (ELeave) CCatalogsAccessPoint();
    CleanupStack::PushL( self );
    self->ConstructL(aStream);
    return self;
    }

CCatalogsAccessPoint::~CCatalogsAccessPoint()
    {
    delete iName;
    delete iNcdAccessPointId;
    }

CCatalogsAccessPoint::CCatalogsAccessPoint(): 
							CCatalogsAccessPointSettings(),
							iCreatedByManager( EFalse )
    {
    }

void CCatalogsAccessPoint::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    CCatalogsAccessPointSettings::ExternalizeL(aStream);
        
    // Externalize this classes state to the stream.
    ExternalizeDesL(*iName, aStream);
    ExternalizeDesL(*iNcdAccessPointId, aStream);
    aStream.WriteUint32L( iAccessPointId );
    aStream.WriteUint32L( static_cast<TUint32>( iCreatedByManager ) );
    DLTRACEOUT((""));
    }

void CCatalogsAccessPoint::InternalizeL( RReadStream& aStream )
    {
    CCatalogsAccessPointSettings::InternalizeL(aStream);
        
    // Internalize this classes state from the stream.
    InternalizeDesL(iName, aStream);
    InternalizeDesL(iNcdAccessPointId, aStream);
    iAccessPointId = aStream.ReadUint32L();
    iCreatedByManager = static_cast<TBool>( aStream.ReadUint32L() );
    }

    
void CCatalogsAccessPoint::SetNameL(const TDesC& aName) 
    {
    HBufC* tmpName = aName.AllocL();
    delete iName;
    iName = 0;
    iName = tmpName;
    }
    
const TDesC& CCatalogsAccessPoint::Name() const 
    {
    return *iName;
    }

TBool CCatalogsAccessPoint::CreatedByManager() const 
	{
	return iCreatedByManager;
	}

void CCatalogsAccessPoint::SetCreatedByManager()
    {
	iCreatedByManager = ETrue;
    }

TUint32 CCatalogsAccessPoint::AccessPointId() const
    {
    return iAccessPointId;
    }
    
const TDesC& CCatalogsAccessPoint::NcdAccessPointId() const
    {
    return *iNcdAccessPointId;
    }

void CCatalogsAccessPoint::SetAccessPointId( TUint32 aId )
    {
    iAccessPointId = aId;
    }

void CCatalogsAccessPoint::ConstructL( const TDesC& aApNcdId, const TDesC& aName )
    {
    CCatalogsAccessPointSettings::ConstructL();
    iName = aName.AllocL();
    iNcdAccessPointId = aApNcdId.AllocL();
    }
    
void CCatalogsAccessPoint::ConstructL( RReadStream& aStream) 
    {
    CCatalogsAccessPointSettings::ConstructL();
    InternalizeL(aStream);
    }
