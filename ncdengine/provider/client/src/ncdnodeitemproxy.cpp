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
* Description:   Contains CNcdNodeItemProxy class implementation
*
*/


#include "ncdnodeitemproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "catalogsdebug.h"


CNcdNodeItemProxy::CNcdNodeItemProxy( MCatalogsClientServer& aSession,
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdNodeProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdNodeItemProxy::ConstructL()
    {
    CNcdNodeProxy::ConstructL();
    }


CNcdNodeItemProxy::~CNcdNodeItemProxy()
    {
    }


CNcdNodeItemProxy* CNcdNodeItemProxy::NewL( MCatalogsClientServer& aSession,
                                            TInt aHandle,
                                            CNcdNodeManagerProxy& aNodeManager,
                                            CNcdOperationManagerProxy& aOperationManager,
                                            CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdNodeItemProxy* self = 
        CNcdNodeItemProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeItemProxy* CNcdNodeItemProxy::NewLC( MCatalogsClientServer& aSession,
                                             TInt aHandle,
                                             CNcdNodeManagerProxy& aNodeManager,
                                             CNcdOperationManagerProxy& aOperationManager,
                                             CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdNodeItemProxy* self = 
        new( ELeave ) CNcdNodeItemProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CNcdNodeItemProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    // First internalize parent data
    CNcdNodeProxy::InternalizeNodeDataL( aStream );

    // Nothing else to do here
    }

void CNcdNodeItemProxy::InternalizeNodeLinkDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    // First internalize parent data
    CNcdNodeProxy::InternalizeNodeLinkDataL( aStream );

    // Nothing else to do here
    }

