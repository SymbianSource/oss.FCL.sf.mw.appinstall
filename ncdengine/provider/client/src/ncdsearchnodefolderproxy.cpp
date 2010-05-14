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
* Description:   Contains CNcdSearchNodeFolderProxy class implementation
*
*/


#include "ncdsearchnodefolderproxy.h"
#include "ncdnodemetadataproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncdnodeskinproxy.h"
#include "ncdnodesearchimpl.h"
#include "catalogsutils.h"
#include "ncdutils.h"


CNcdSearchNodeFolderProxy::CNcdSearchNodeFolderProxy( MCatalogsClientServer& aSession,
                                                      TInt aHandle,
                                                      CNcdNodeManagerProxy& aNodeManager,
                                                      CNcdOperationManagerProxy& aOperationManager,
                                                      CNcdFavoriteManagerProxy& aFavoriteManager ) 
    : CNcdParentOfTransparentNodeProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdSearchNodeFolderProxy::ConstructL()
    {
    CNcdParentOfTransparentNodeProxy::ConstructL();
    }


CNcdSearchNodeFolderProxy* CNcdSearchNodeFolderProxy::NewL( MCatalogsClientServer& aSession,
                                                TInt aHandle,
                                                CNcdNodeManagerProxy& aNodeManager,
                                                CNcdOperationManagerProxy& aOperationManager,
                                                CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchNodeFolderProxy* self = 
        CNcdSearchNodeFolderProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSearchNodeFolderProxy* CNcdSearchNodeFolderProxy::NewLC( MCatalogsClientServer& aSession,
                                                 TInt aHandle,
                                                 CNcdNodeManagerProxy& aNodeManager,
                                                 CNcdOperationManagerProxy& aOperationManager,
                                                 CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchNodeFolderProxy* self = 
        new( ELeave ) CNcdSearchNodeFolderProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdSearchNodeFolderProxy::~CNcdSearchNodeFolderProxy()
    {
    delete iSearchFilter;
    iSearchFilter = 0;
    }


const CNcdSearchFilter& CNcdSearchNodeFolderProxy::SearchFilterL() const
    {
    if ( !iSearchFilter )
        {
        User::Leave( KErrNotFound );
        }
    return *iSearchFilter;
    }

void CNcdSearchNodeFolderProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // First internalize parent data
    CNcdParentOfTransparentNodeProxy::InternalizeNodeDataL( aStream );
    
    DLTRACE((""));

    delete iSearchFilter;
    iSearchFilter = NULL;
    iSearchFilter = CNcdSearchFilter::NewL( aStream );
    
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    const MDesCArray& keywords = iSearchFilter->Keywords();
    DLINFO(("Search filter: "));
    for ( TInt i = 0; i < keywords.MdcaCount(); i++ ) 
        {
        DLINFO((_L("%S"), &keywords.MdcaPoint( i ) ));
        }
    #endif
    
    DLTRACE((""));

    DLTRACEOUT((""));
    }


MNcdLoadNodeOperation* CNcdSearchNodeFolderProxy::LoadChildrenL( TInt aIndex, 
                                                           TInt aSize,
                                                           TNcdChildLoadMode aMode,
                                                           MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN((("this: %X"), this));
    DASSERT( iSearchFilter );
    
    if( aSize < 1 || aIndex < 0 || ( aMode == ELoadMetadata && aIndex + aSize > ChildCount() ))
        {
        // Nothing to be done 
        DLERROR(( "Argument error. ChildCount: %d Given index: %d, size: %d",
                  ChildCount(), aIndex, aSize ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }

    DLTRACE(( _L("Node: %S, %S"), &Namespace(), &Id() ));

    #ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    const MDesCArray& keywords = iSearchFilter->Keywords();
    DLINFO(("Search filter: "));
    for ( TInt i = 0; i < keywords.MdcaCount(); i++ ) 
        {
        DLINFO((_L("%S"), &keywords.MdcaPoint( i ) ));
        }
    #endif
        
    CNcdLoadNodeOperationProxy* operation = 
        OperationManager().CreateLoadNodeOperationL( *this,
            ETrue, // load children
            aSize, 
            aIndex,
            1,
            aMode,
            iSearchFilter );

    if( operation == NULL )
        {
        DLTRACEOUT(("NULL"));     
        return NULL;
        }

    CleanupReleasePushL( *operation );
    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );
    CleanupStack::Pop( operation );
    
    DLTRACEOUT((""));

    return operation;
    }

MNcdNode::TState CNcdSearchNodeFolderProxy::State() const
    {
    DLTRACEIN((_L("Node namespace=%S, id=%S"), &Namespace(), &Id() ));

    // Check if the link handle has been set, which means that also
    // link data has been internalized. Also, check if the metadata 
    // exists, which means that metadata has also been internalized.
    if ( LinkHandleSet() 
         && Metadata() != NULL )
        {        
        DLTRACEOUT(("Initialized"));
        return MNcdNode::EStateInitialized;
        }
     else
        {
        // Node has not been initialized.
        DLTRACEOUT(("Not initialized"));
        return MNcdNode::EStateNotInitialized;
        }
    }
    
MNcdLoadNodeOperation* CNcdSearchNodeFolderProxy::LoadL( MNcdLoadNodeOperationObserver& aObserver )
    {
    CNcdLoadNodeOperationProxy* operation = OperationManager().CreateLoadNodeOperationL( *this,
        EFalse, // don't load children
        0, // pagesize, meaningless in this case
        0, // pagestart, meaningless in this case
        0, // depth, meaningless in this case
        ELoadStructure, // meaningless in this case
        iSearchFilter );
    CleanupReleasePushL( *operation );
    
    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );
    CleanupStack::Pop( operation );
    return operation;
    }
