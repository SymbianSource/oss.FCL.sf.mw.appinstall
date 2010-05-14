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
* Description:  
*
*/


#include "catalogstransportoperationid.h"
    
#include <s32strm.h>
        
// ---------------------------------------------------------------------------
// Equals
// ---------------------------------------------------------------------------
//
inline bool TCatalogsTransportOperationId::operator==( const 
    TCatalogsTransportOperationId& aOther ) const
    {
    return ( iId == aOther.iId && iSession == aOther.iSession );
    }



// ---------------------------------------------------------------------------
// Compares this id with other
// ---------------------------------------------------------------------------
//
inline TInt TCatalogsTransportOperationId::Compare( 
    const TCatalogsTransportOperationId& aSecond ) const
    {
    if( iSession == aSecond.iSession ) 
        {        
        return iId - aSecond.iId;   
        }
    return iSession - aSecond.iSession;
    }


// ---------------------------------------------------------------------------
// Operation session id getter
// ---------------------------------------------------------------------------
//
inline TInt32 TCatalogsTransportOperationId::SessionId() const
    {
    return iSession;
    }
    

// ---------------------------------------------------------------------------
// Operation id number getter
// ---------------------------------------------------------------------------
//
inline TInt32 TCatalogsTransportOperationId::Id() const
    {
    return iId;
    }


// ---------------------------------------------------------------------------
// Secondary id
// ---------------------------------------------------------------------------
//
inline TInt32 TCatalogsTransportOperationId::SecondaryId() const
    {
    return iSecondaryId;
    }


// ---------------------------------------------------------------------------
// Secondary id setter
// ---------------------------------------------------------------------------
//
inline void TCatalogsTransportOperationId::SetSecondaryId( TInt32 aId )
    {
    iSecondaryId = aId;
    }


// ---------------------------------------------------------------------------
// Externalize id
// ---------------------------------------------------------------------------
//
inline void TCatalogsTransportOperationId::ExternalizeL( 
    RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( SessionId() );
    aStream.WriteInt32L( Id() );
    aStream.WriteInt32L( SecondaryId() );
    }


// ---------------------------------------------------------------------------
// Internalize id
// ---------------------------------------------------------------------------
//
inline void TCatalogsTransportOperationId::InternalizeL( RReadStream& aStream )
    {
    iSession = aStream.ReadInt32L();
    iId = aStream.ReadInt32L();
    iSecondaryId = aStream.ReadInt32L();
    }
