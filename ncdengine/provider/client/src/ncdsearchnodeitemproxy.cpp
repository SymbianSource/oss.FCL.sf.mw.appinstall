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
* Description:   Contains CNcdSearchNodeItemProxy class implementation
*
*/


#include "ncdsearchnodeitemproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "catalogsdebug.h"
#include "ncdutils.h"
#include "ncdloadnodeoperationproxy.h"


CNcdSearchNodeItemProxy::CNcdSearchNodeItemProxy( MCatalogsClientServer& aSession,
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdNodeItemProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdSearchNodeItemProxy::ConstructL()
    {
    CNcdNodeItemProxy::ConstructL();
    }


CNcdSearchNodeItemProxy::~CNcdSearchNodeItemProxy()
    {
    }


CNcdSearchNodeItemProxy* CNcdSearchNodeItemProxy::NewL( MCatalogsClientServer& aSession,
                                            TInt aHandle,
                                            CNcdNodeManagerProxy& aNodeManager,
                                            CNcdOperationManagerProxy& aOperationManager,
                                            CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchNodeItemProxy* self = 
        CNcdSearchNodeItemProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSearchNodeItemProxy* CNcdSearchNodeItemProxy::NewLC( MCatalogsClientServer& aSession,
                                             TInt aHandle,
                                             CNcdNodeManagerProxy& aNodeManager,
                                             CNcdOperationManagerProxy& aOperationManager,
                                             CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchNodeItemProxy* self = 
        new( ELeave ) CNcdSearchNodeItemProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
MNcdLoadNodeOperation* CNcdSearchNodeItemProxy::LoadL( MNcdLoadNodeOperationObserver& aObserver )
    {
    CNcdSearchFilter* emptyFilter = CNcdSearchFilter::NewLC();    
    CNcdLoadNodeOperationProxy* operation = OperationManager().CreateLoadNodeOperationL( *this,
        EFalse, // don't load children
        0, // pagesize, meaningless in this case
        0, // pagestart, meaningless in this case
        0, // depth, meaningless in this case
        ELoadStructure, // meaningless in this case
        emptyFilter );
    CleanupStack::PopAndDestroy( emptyFilter );
    CleanupReleasePushL( *operation );
    
    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );
    CleanupStack::Pop( operation );
    return operation;
    }
