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
* Description:   Contains CNcdNodeSearch class implementation
*
*/


#include "ncdnodesearchimpl.h"
#include "ncdnodeproxy.h"
#include "ncdoperationimpl.h"
#include "ncdnodeidentifier.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdsearchrootnodeproxy.h"


CNcdNodeSearch::CNcdNodeSearch(
    CNcdNodeProxy& aNode,
    CNcdOperationManagerProxy& aOperationManager )
    : CCatalogsInterfaceBase( &aNode ),
      iNode( aNode ),
      iOperationManager( aOperationManager )
    {
    }


void CNcdNodeSearch::ConstructL()
    {
    DLTRACEIN((""));
    // Register the interface
    MNcdNodeSearch* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeSearch::KInterfaceUid ) );
    }


CNcdNodeSearch* CNcdNodeSearch::NewL(
    CNcdNodeProxy& aNode,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdNodeSearch* self = 
        CNcdNodeSearch::NewLC( aNode, aOperationManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeSearch* CNcdNodeSearch::NewLC(
    CNcdNodeProxy& aNode,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdNodeSearch* self = 
        new( ELeave ) CNcdNodeSearch( aNode, aOperationManager );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeSearch::~CNcdNodeSearch()
    {
    DLTRACEIN((""))
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeSearch::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    }


// MNcdNodeSearch functions

MNcdSearchOperation* CNcdNodeSearch::SearchL( MNcdLoadNodeOperationObserver& aObserver,
                                              MNcdSearchFilter& aSearchFilter )
    {
    DLTRACEIN((""));
    
    if ( aSearchFilter.Keywords().MdcaCount() < 1 )
        {
        DLERROR(("Error, no keywords in search!"));
        DASSERT( 0 );
        User::Leave( KErrArgument );
        }
    // make sure that search root is created
    CNcdSearchRootNodeProxy* searchRoot = &iNode.NodeManager().SearchRootNodeL();    
    CNcdLoadNodeOperationProxy* operation = iOperationManager.CreateLoadNodeOperationL( iNode,
        EFalse, // don't load children
        0, // pagesize, meaningless in this case
        0, // pagestart, meaningless in this case
        0, // depth, meaningless in this case
        ELoadStructure, // meaningless in this case
        &aSearchFilter );
    CleanupReleasePushL( *operation );
    // Search root is added as an observer here to make it
    // update once the op is complete. 
    operation->AddObserverL( searchRoot );
    operation->AddObserverL( &iNode );
    operation->AddObserverL( &aObserver );
    CleanupStack::Pop( operation );
    return operation;
    }
    


// Other functions

CNcdOperationManagerProxy& CNcdNodeSearch::OperationManager() const
    {
    return iOperationManager;
    }


CNcdNodeProxy& CNcdNodeSearch::Node() const
    {
    return iNode;
    }
