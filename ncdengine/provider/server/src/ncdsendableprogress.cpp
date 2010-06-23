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


#include <s32strm.h>

#include "ncdsendableprogress.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TNcdSendableProgress::TNcdSendableProgress() :
    iState( 0 ),
    iOperationId( 0 ),
    iProgress( 0 ),
    iMaxProgress( 0 )
    {    
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TNcdSendableProgress::TNcdSendableProgress( TUint aState, TInt32 aOperationId,
    TInt32 aProgress, 
    TInt32 aMaxProgress ) :
    iState( aState ),
    iOperationId( aOperationId ),
    iProgress( aProgress ), 
    iMaxProgress( aMaxProgress )
    {
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void TNcdSendableProgress::InternalizeL( RReadStream& aStream )
    {
    iState = aStream.ReadUint32L();
    iOperationId = aStream.ReadInt32L();
    iProgress = aStream.ReadInt32L();
    iMaxProgress = aStream.ReadInt32L();
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void TNcdSendableProgress::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iState );
    aStream.WriteInt32L( iOperationId );
    aStream.WriteInt32L( iProgress );
    aStream.WriteInt32L( iMaxProgress );
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt TNcdSendableProgress::Size() const
    {
    TInt size(0);
    size += sizeof( iState );
    size += sizeof( iOperationId );
    size += sizeof( iProgress );
    size += sizeof( iMaxProgress );
    return size;
    } 
