/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of 
*                CIAUpdateLoader class member functions.
*
*/




#include <ncdprovider.h>
#include <ncdnode.h>
#include <ncdnodecontainer.h>
#include <ncdloadnodeoperation.h>
#include <ncdquery.h>
#include <ncdnodechildoftransparent.h>

#include "iaupdateloader.h"
#include "iaupdateloaderobserver.h"
#include "iaupdatectrlconsts.h"
#include "iaupdateversion.h"
#include "iaupdateutils.h"
#include "iaupdatedebug.h"


// Use KMaxTInt value for the child count.
// Then, all the children will be downloaded 
// even if the parent does not have the correct
// information about its child count.
const TInt KChildCountLoadAll( KMaxTInt );


// -----------------------------------------------------------------------------
// CIAUpdateLoader::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateLoader* CIAUpdateLoader::NewLC( 
    MNcdProvider& aProvider, 
    MIAUpdateLoaderObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::NewLC() begin");
    
    CIAUpdateLoader* self = 
        new( ELeave ) CIAUpdateLoader( aProvider, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::NewLC() end");

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//    
CIAUpdateLoader* CIAUpdateLoader::NewL(
    MNcdProvider& aProvider, 
    MIAUpdateLoaderObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::NewL() begin");

    CIAUpdateLoader* self = 
        CIAUpdateLoader::NewLC( aProvider, aObserver );
    CleanupStack::Pop( self );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::NewL() end");

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::CIAUpdateLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//        
CIAUpdateLoader::CIAUpdateLoader(
    MNcdProvider& aProvider, 
    MIAUpdateLoaderObserver& aObserver ) 
: CBase(),
  iProvider( aProvider ), 
  iObserver( aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::CIAUpdateLoader()");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::ConstructL() begin");
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::ConstructL() end");
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateLoader::~CIAUpdateLoader
// Destructor
// -----------------------------------------------------------------------------
//     
CIAUpdateLoader::~CIAUpdateLoader()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::~CIAUpdateLoader() begin");

    // Call just to be sure.
    Cancel();

    iOperations.Close();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::~CIAUpdateLoader() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::RootExpiredL
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateLoader::RootExpiredL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::RootExpiredL() begin");
    
    TBool rootExpired( EFalse );

    // Remember to release this node when it is not needed.
    MNcdNode* rootNode( iProvider.RootNodeL() );

    // Get the state of the root.
    MNcdNode::TState rootState( rootNode->State() );

    IAUPDATE_TRACE_1("[IAUPDATE] Root state: %d",
                     rootState);

    // Check if the root is expired or not.
    if ( rootState == MNcdNode::EStateExpired )
        {
        IAUPDATE_TRACE("[IAUPDATE] Root is expired");
        rootExpired = ETrue;
        }

    TInt childCount( 0 );
    MNcdNodeContainer* container( rootNode->QueryInterfaceLC< MNcdNodeContainer >() );
    if ( container )
        {
        childCount = container->ChildCount();
        CleanupStack::PopAndDestroy( container );
        }
       
    IAUPDATE_TRACE_1("[IAUPDATE] count of children: %d", childCount ); 
    
    //2 here means arrow CGW and firmware CGW. If one of them failed last time, 
    //refresh from CDB again
    if ( childCount != 2 && rootState == MNcdNode::EStateInitialized )
        {
        IAUPDATE_TRACE("[IAUPDATE] Root is expired. update from phase 1 to phase 2 or previous CGW load failed");
        rootExpired = ETrue;
        }
    // Release root because it is not needed any more.
    rootNode->Release();
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::RootExpiredL() end");

    return rootExpired;
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadNodesL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::LoadNodesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadNodesL() begin");
    
    if ( iOperations.Count() > 0 )
        {
        // 
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Refresh already going on.");
        User::Leave( KErrInUse );
        }

    // Reset this value for new round.
    iErrorCode = KErrNone;
        
    // Root refresh will handle everything.
    LoadRootL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadNodesL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::Cancel
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateLoader::Cancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::Cancel() begin");

    // Cancel asynchronous operations.
    
    // Use this flag to inform that operation complete callback function
    // that cancellation is going on. Cancel related things are handled
    // here.
    iCancelling = ETrue;

    // Operations will use callback functions to inform 
    // when the operation is completed.
    TInt count( iOperations.Count() );
    IAUPDATE_TRACE_1("[IAUPDATE] Cancel array count: %d", count);

    for ( TInt i = count - 1; i >= 0; --i )    
        {
        TIAUpdateOperationInfo info( iOperations[ i ] );
        MNcdOperation* operation( info.iOperation );

        // Notice, that OperationCompleteL callback is called when
        // cancellation finishes
        operation->CancelOperation();
      
        }
        
    // Now, that the array elements were released, reset the array.
    iOperations.Reset();

    // Release the flag now, that all the cancellations have been handled.
    iCancelling = EFalse;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::CancelOperation() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::SetSkipChildCountRefresh
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateLoader::SetSkipChildCountRefresh( TBool aSkip )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateLoader::SetSkipChildCountRefresh(): %d",
                     aSkip);
    iSkipChildCountRefresh = aSkip;
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::NodesUpdated
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateLoader::NodesUpdated( 
    MNcdLoadNodeOperation& /*aOperation*/,
    RCatalogsArray< MNcdNode >& /*aNodes*/ )
    {
    // No need to inform the observer about single updates.
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::QueryReceived
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateLoader::QueryReceived( 
    MNcdLoadNodeOperation& aOperation,
    MNcdQuery* aQuery )  
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::QueryReceived() begin");

    // Operation query received. Always accept queries.
    // Queries should not be requested from this client.
    TInt trapError( KErrNone );
    if ( aQuery )
        {
        TRAP ( trapError, 
               aQuery->SetResponseL( MNcdQuery::EAccepted );
               aOperation.CompleteQueryL( *aQuery ); );
        // Release needs to be called to the query after it is not used.
        aQuery->Release();        
        }

    if ( ( trapError != KErrNone ) || ( !aQuery ) )
        {
        // Error occurred when query was handled.
        // So, operation can not continue.
        // Cancel operation. Notice, that OperationComplete will be called
        // by the operation when cancel is called.
        aOperation.CancelOperation();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::QueryReceived() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::OperationComplete
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateLoader::OperationComplete( 
    MNcdLoadNodeOperation& aOperation,
    TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateLoader::OperationComplete() begin: %d",
                     aError);

    MNcdNode* operationNode( NULL );
    TIAUpdateOperationInfo::TOperationType operationType( 
                    TIAUpdateOperationInfo::EIdle );

    // Remember to release the operation, 
    // because its reference count was increased
    // when the operation was inserted into the array.
    for ( TInt i = 0; i < iOperations.Count(); ++i )
        {
        TIAUpdateOperationInfo tmpInfo( iOperations[ i ] );
        MNcdOperation* operation( tmpInfo.iOperation );
        MNcdOperation* paramOperation( &aOperation );
        if ( paramOperation == operation )
            {
            IAUPDATE_TRACE_1("[IAUPDATE] Completed operation was found from the list %d",
                                 i);

            // Get the operation type info. 
            // Then, we can later decide how to continue.
            operationType = tmpInfo.iOperationType;

            // Get the node of the operation.
            // Notice, this needs to be released.
            operationNode = operation->Node();

            // Release the operation here because its reference count
            // was increased before it was added into the array.
            operation->Release();
                
            // Remove completed operation from the array.
            iOperations.Remove( i );

            // No need to continue the for loop anymore.
            break;
            }
        }

    if ( operationNode )
        {
        if ( aError != KErrNone )
            {
            // Get the latest error code, if error has occurred.
            // We handle here only error codes that have been
            // directed to the operations that we recognized from the
            // operation array.
            
            //Continue next round to load the children.
            //This is to make sure when one CGW loading failed, the updates from the other CGW will
            //still be shown in mainview.
            TRAP_IGNORE( NextLoadLoopL( *operationNode, operationType ) );
            IAUPDATE_TRACE_1("[IAUPDATE] New iErrorCode value: %d", aError);
            
            if ( iErrorCode == KErrNone )
                {
                iErrorCode = aError;
                }
            }
        else
            {
            // Continue to the next loop only if an error did not occur 
            // during this round.
            TRAPD( trapError,
                   NextLoadLoopL( *operationNode, operationType ) );
            if ( trapError != KErrNone )
                {
                IAUPDATE_TRACE_1("[IAUPDATE] ERROR: Next round error: %d", trapError);
                // Something went wrong when children load was started.
                // Update error code.
                iErrorCode = trapError;
                }
            }

        // No need for the node anymore.
        operationNode->Release();
        operationNode = NULL;
        }

    if ( iOperations.Count() == 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] The operation as whole completed");
        // No more operations left. So, inform observer.
        if ( iErrorCode == KErrNone )
            {
            // Because of workaround needed for handling firmware updates.
            // Refresh from network was completed succesfully.
            // Current firmware version is stored to private folder. 
            TRAP_IGNORE( IAUpdateUtils::SaveCurrentFwVersionIfNeededL() );
            }
        iObserver.LoadComplete( iErrorCode );
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::OperationComplete() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadRootL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::LoadRootL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadRootL() begin");
    
    MNcdNode* rootNode( iProvider.RootNodeL() );
    CleanupReleasePushL( *rootNode );

    MNcdNodeContainer* rootContainer(
        rootNode->QueryInterfaceLC< MNcdNodeContainer >() );

    if ( !rootContainer )
        {
        User::Leave( KErrNotFound );            
        }

    MNcdNode::TState rootState( rootNode->State() );
    if ( rootState == MNcdNode::EStateInitialized )
        {
        IAUPDATE_TRACE("[IAUPDATE] Root already initialized.");

        // CDB connections should be avoided if possible. So, if the
        // root node is initialized and not expired, then do not update it
        // from the net. Just continue directly to its children.

        if ( iSkipChildCountRefresh )
            {
            IAUPDATE_TRACE("[IAUPDATE] Root, load all children and skip child count refresh.");
            // Because child count refresh should be skipped. Try to load children
            // of the children directly without requesting the child count first.
            LoadChildrenOfChildrenL( 
                *rootContainer, TIAUpdateOperationInfo::ELoadAllChildren );
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Root, load child containers.");
            // At the moment, NCD Engine does not load root children nodes 
            // when MNcdContainer::LoadChildrenL is called. So, we have to
            // load the children one by one here.
            LoadChildContainersL( *rootContainer );            
            }
        }
    else
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Refresh root. State: %d", rootState);

        // Start loading root.
        StartLoadOperationL( *rootContainer, TIAUpdateOperationInfo::ELoadRoot );
        }

    // No use for the node objects any more. Release them.
    CleanupStack::PopAndDestroy( rootContainer );
    CleanupStack::PopAndDestroy( rootNode );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadRootL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadChildContainersL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateLoader::LoadChildContainersL( MNcdNodeContainer& aParentContainer )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildContainersL() begin");
    
    // LoadL is used to update the children of the parent container here
    // instead of updating them by calling LoadChildrenL function.

    TInt childCount( aParentContainer.ChildCount() );
    IAUPDATE_TRACE_1("[IAUPDATE] Parent container child count: %d", childCount);
 
    for ( TInt i = 0; i < childCount; ++i )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Container child: %d", i);

        MNcdNode* node( aParentContainer.ChildL( i ) );
        if ( node )
            {
            CleanupReleasePushL( *node );

            MNcdNodeContainer* container(
            node->QueryInterfaceLC< MNcdNodeContainer >() );
            if ( container )
                {
                IAUPDATE_TRACE("[IAUPDATE] Container child had container interface");
                LoadContainerL( *container );
                CleanupStack::PopAndDestroy( container );                
                }
            CleanupStack::PopAndDestroy( node );
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildContainersL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadContainerL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::LoadContainerL( MNcdNodeContainer& aContainer )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadContainerL() begin");

    StartLoadOperationL( 
        aContainer, TIAUpdateOperationInfo::ELoadContainer );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadContainerL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadChildrenL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateLoader::LoadChildrenL( MNcdNodeContainer& aContainer )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildrenL() begin");

    if ( aContainer.ChildCount() > 0 )
        {
        StartLoadOperationL( 
            aContainer, TIAUpdateOperationInfo::ELoadChildren );        
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildrenL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadAllChildrenL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateLoader::LoadAllChildrenL( MNcdNodeContainer& aContainer )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadAllChildrenL() begin");

    if ( !iSkipChildCountRefresh )
        {
        // Child count skip flag has been set to EFalse in previous round.
        // Most likely NCD Engine does not support load all children functionality. 
        // So, just load children using normal flow from now on.
        // Notice, that now we need to first load the child count
        // for the child containers. So, even if children were 
        // already updated by LoadChildrenL, a new request for
        // containers needs . Otherwise, the child count 
        // will not be up-to-date. When the child containers are 
        // loaded, the flow will continue to load the children.
        IAUPDATE_TRACE("[IAUPDATE] Child count should not be skipped");
        LoadContainerL( aContainer );
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadAllChildrenL() end");
        return;
        }

    TRAPD ( trapError, 
            StartLoadOperationL( 
                aContainer, TIAUpdateOperationInfo::ELoadAllChildren ); );

    IAUPDATE_TRACE_1("[IAUPDATE] trapError: %d", trapError);

    // Only acceptable error is KErrArgument.
    // Then it is possible that an old NCD Engine is used
    // and it does not allow too big page size. So, in that
    // case try one more time below by not skipping child count
    // refresh. Else, leave if error occurred.

    if ( trapError == KErrArgument )
        {
        IAUPDATE_TRACE("[IAUPDATE] Try to change child load method");
        // Notice, that now we need to first load the child count
        // for the child containers. So, even if children were 
        // already updated by LoadChildrenL, a new request for
        // containers needs . Otherwise, the child count 
        // will not be up-to-date. When the child containers are 
        // loaded, the flow will continue to load the children.
        LoadContainerL( aContainer );
        IAUPDATE_TRACE("[IAUPDATE] Child load method changed and container load started");
        // If we come here, it means that support for all children is
        // not available in NCD Engine. So, do not try it after this.
        SetSkipChildCountRefresh( EFalse );
        }
    else if ( trapError != KErrNone )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Error: %d", trapError);
        User::Leave( trapError );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadAllChildrenL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLoader::LoadChildrenOfChildrenL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::LoadChildrenOfChildrenL( 
    MNcdNodeContainer& aContainer,
    TIAUpdateOperationInfo::TOperationType aPreviousOperationType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildrenOfChildrenL() begin");
    
    TInt childCount( aContainer.ChildCount() );
    IAUPDATE_TRACE_1("[IAUPDATE] Container child count: %d", childCount);
 
    for ( TInt i = 0; i < childCount; ++i )
        {
        MNcdNode* node( aContainer.ChildL( i ) );
        if ( node )
            {
            CleanupReleasePushL( *node );

            MNcdNodeContainer* container( 
               node->QueryInterfaceLC< MNcdNodeContainer >() );
            if ( container )
                {
                IAUPDATE_TRACE_1("[IAUPDATE] Container child: %d had container interface", i);
                switch ( aPreviousOperationType )
                    {
                    case TIAUpdateOperationInfo::ELoadRoot:
                        IAUPDATE_TRACE("[IAUPDATE] Load children of the root child");
                        // When root is loaded, also its children are loaded
                        // and they have now their child count. So, no need to
                        // reload children of the root. Load their children
                        // directly now.
                        LoadChildrenL( *container );
                        break;
                            
                    case TIAUpdateOperationInfo::ELoadChildren:
                        IAUPDATE_TRACE("[IAUPDATE] Load children of a container");
                        // Notice, that now we need to first load the child count
                        // for the child containers. So, even if children were 
                        // already updated by LoadChildrenL, a new request for
                        // containers needs . Otherwise, the child count 
                        // will not be up-to-date. When the child containers are 
                        // loaded, the flow will continue to load the children.
                        LoadContainerL( *container );
                        break;

                    case TIAUpdateOperationInfo::ELoadAllChildren:
                        IAUPDATE_TRACE("[IAUPDATE] Load all children of a container");
                        // Notice, here we will try to skip the loading of the child
                        // count of the container. So, all the children are tried to
                        // be loaded directly even if the child count may not be
                        // up-to-date.
                        LoadAllChildrenL( *container );
                        break;
                            
                    default:
                        IAUPDATE_TRACE("[IAUPDATE] ERROR: Wrong operation type");
                        User::Leave( KErrArgument );
                        break;
                    }
                CleanupStack::PopAndDestroy( container );                
                }
            CleanupStack::PopAndDestroy( node );
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::LoadChildrenOfChildrenL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::StartLoadOperationL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::StartLoadOperationL( 
    MNcdNodeContainer& aContainer,
    TIAUpdateOperationInfo::TOperationType aOperationType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::StartLoadOperation() begin");

    MNcdOperation* operation( NULL );

    // Create a new operation based on the operation type information.
    switch ( aOperationType )
        {
        case TIAUpdateOperationInfo::ELoadRoot:
        case TIAUpdateOperationInfo::ELoadContainer:
            {
            // Container itself should be loaded.
            IAUPDATE_TRACE("[IAUPDATE] LoadL requested as operation");
            MNcdNode* node( aContainer.QueryInterfaceLC< MNcdNode >() );
            if ( !node )
                {
                IAUPDATE_TRACE("[IAUPDATE] Container did not have node interface.");
                User::Leave( KErrNotFound );
                }
            operation = node->LoadL( *this );
            // No need for the node anymore
            CleanupStack::PopAndDestroy( node );
            }
            break;
 
        case TIAUpdateOperationInfo::ELoadChildren:
            {
            // Children of the container should be loaded.
            IAUPDATE_TRACE("[IAUPDATE] LoadChildrenL requested as operation");
            TInt childCount( aContainer.ChildCount() );
            IAUPDATE_TRACE_1("[IAUPDATE] Child count: %d", childCount);
            operation = 
                aContainer.LoadChildrenL( 
                    0, childCount, ELoadMetadata, *this );
            }
            break;

        case TIAUpdateOperationInfo::ELoadAllChildren:
            {
            // All children of the container should be loaded.
            IAUPDATE_TRACE("[IAUPDATE] LoadAllChildren requested as operation");
            // Because we can not be sure about the container child count,
            // use the really big value. Then we will surely get all the
            // children.            
            operation = 
                aContainer.LoadChildrenL( 
                    0, KChildCountLoadAll, ELoadMetadata, *this );
            }
            break;
        
        default:
            IAUPDATE_TRACE_1("[IAUPDATE] ERROR: Operation type: %d", aOperationType);
            User::Leave( KErrArgument );
            break;
        }

    // Insert the operation to the cleanupstack. So, it will be released
    // if leave occurs.
    CleanupReleasePushL( *operation );

    // Insert the operation into the array first. By doing this before starting
    // the operation, we do not need to Cancel just started operation if leave
    // occurs.
    TIAUpdateOperationInfo info( aOperationType, operation );
    iOperations.AppendL( info );

    TRAPD( trapError, operation->StartOperationL() );
    if ( trapError != KErrNone )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] ERROR: Operation Start failed: %d", trapError);
        // Operation start failed.
        // Remove operation from the array because it was appended
        // above.
        iOperations.Remove( iOperations.Count() - 1 );

        // Now, we can leave.
        // Notice, this leave will pop and destroy the operation. So,
        // it will be released then.
        User::Leave( trapError );
        }

    // Operation was started.
    // Do not release it now but remove only from the stack.
    // Operation will be released when it completes.
    CleanupStack::Pop( operation );           
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::StartLoadOperation() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateLoader::NextLoadLoopL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLoader::NextLoadLoopL( 
    MNcdNode& aNode,
    TIAUpdateOperationInfo::TOperationType aPreviousOperationType )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateLoader::NextLoadLoopL() begin: %d",
                     aPreviousOperationType);
    
    MNcdNodeContainer* container( 
        aNode.QueryInterfaceLC< MNcdNodeContainer >() );
    if ( !container )
        {
        IAUPDATE_TRACE("[IAUPDATE] Container interface was not found");
        // Container interface should always exist in these cases.
        User::Leave( KErrNotFound );
        }

    // Check if the operation should be continued to its children.
    switch ( aPreviousOperationType )
        {
        case TIAUpdateOperationInfo::ELoadRoot:
            IAUPDATE_TRACE("[IAUPDATE] Previously loaded root");
            // When root is loaded, also its children are loaded
            // and they have now their child count. So, no need to
            // reload children of the root. Load their children
            // directly now.
            LoadChildrenOfChildrenL( 
                *container,
                TIAUpdateOperationInfo::ELoadRoot );
            break;

        case TIAUpdateOperationInfo::ELoadContainer:
            IAUPDATE_TRACE("[IAUPDATE] Previously loaded container");
            // After normal container has been loaded, 
            // its children can be loaded.
            LoadChildrenL( *container );
            break;

        case TIAUpdateOperationInfo::ELoadChildren:
            IAUPDATE_TRACE("[IAUPDATE] Previously loaded children");
            // Because children of the container have been loaded here,
            // maybe we need to start a new loop for their children.
            // If this container does not contain any new containers,
            // then there is nothing to do anymore.
            if ( iSkipChildCountRefresh )
                {
                // We should come here only after root has been loaded
                // and its children of children have been loaded after that.
                // After that, skip child count refreshes in the
                // next levels.
                IAUPDATE_TRACE("[IAUPDATE] Start to load all children");
                LoadChildrenOfChildrenL( 
                    *container,
                    TIAUpdateOperationInfo::ELoadAllChildren );                
                }
            else
                {
                IAUPDATE_TRACE("[IAUPDATE] Continue load children of children");
                LoadChildrenOfChildrenL( 
                    *container,
                    TIAUpdateOperationInfo::ELoadChildren );
                }                
            break;

        case TIAUpdateOperationInfo::ELoadAllChildren:
            IAUPDATE_TRACE("[IAUPDATE] Previously loaded all children");
            // Notice, here we will try to skip the loading of the child
            // count of the container. So, all the children are tried to
            // be loaded directly even if the child count may not be
            // up-to-date.
            LoadChildrenOfChildrenL( 
                *container,
                TIAUpdateOperationInfo::ELoadAllChildren );
            break;
                         
        default:
            IAUPDATE_TRACE("[IAUPDATE] No need for next loop");
            // No need to load anything else anymore.
            break;
        }    

    // No need for the interface anymore. Release it.
    CleanupStack::PopAndDestroy( container );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLoader::NextLoadLoopL() end");
    }

