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
* Description:   Contains CNcdParentOfTransparentNodeProxy class implementation
*
*/


#include "ncdparentoftransparentnodeproxy.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "catalogsclientserver.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdParentOfTransparentNodeProxy::CNcdParentOfTransparentNodeProxy( MCatalogsClientServer& aSession,
                                                                    TInt aHandle,
                                                                    CNcdNodeManagerProxy& aNodeManager,
                                                                    CNcdOperationManagerProxy& aOperationManager,
                                                                    CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdNodeFolderProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdParentOfTransparentNodeProxy::ConstructL()
    {
    CNcdNodeFolderProxy::ConstructL(); 
    }


CNcdParentOfTransparentNodeProxy::~CNcdParentOfTransparentNodeProxy()
    {
    }


TInt CNcdParentOfTransparentNodeProxy::ServerChildCount() const
    {
    return iServerChildCount;
    }

    
void CNcdParentOfTransparentNodeProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // First internalize parent data
    CNcdNodeFolderProxy::InternalizeNodeDataL( aStream );
    
    // Then get the parent of transparent node specific data
    iServerChildCount = aStream.ReadInt32L();
    
    DLTRACEOUT((""));
    }
    
TBool CNcdParentOfTransparentNodeProxy::IsTransparentChildExpiredL() const
    {
    DLTRACEIN((""));
    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdIsTransparentChildExpired,
                               KNullDesC8,
                               data,
                               Handle(),
                               0 ) );

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );
    
    // Read the data from the stream
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    TBool isTransparentChildExpired = stream.ReadInt32L();
    
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    
    return isTransparentChildExpired;
    }
