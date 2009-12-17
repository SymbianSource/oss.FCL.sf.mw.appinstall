/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateContentOperationManager 
*
*/



#include <ncdpurchasedetails.h>

#include "iaupdatecontentoperationmanager.h"
#include "iaupdatenodeimpl.h"
#include "iaupdateoperation.h"
#include "iaupdatepurchaseoperation.h"
#include "iaupdatedownloadoperation.h"
#include "iaupdateinstalloperation.h"
#include "iaupdateselfupdateinitoperation.h"
#include "iaupdatectrlnodeconsts.h"
#include "iaupdatenodefactory.h"

#include "iaupdatedebug.h"


// Const value for the progress init value.
const TInt KInitProgress( 0 );


// This is a static function that is used with RPointerArray::Sort to sort
// the nodes according to their node depths.
TInt NodeArraySorter( const CIAUpdateNode& aNode1, const CIAUpdateNode& aNode2 )
    {
    // The leaf should be in the end of the array and the root in the beginning.
    // The depth value informs how deep in the hierarchy the node is. A depth
    // value zero means that the node is a root. If multiple branches lead to
    // a same node, then the greatest depth value is used for the node.
    return ( aNode1.Depth() - aNode2.Depth() );
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::::NewL
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperationManager* CIAUpdateContentOperationManager::NewL()
    {
    CIAUpdateContentOperationManager* self = 
        CIAUpdateContentOperationManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::::NewLC
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperationManager* CIAUpdateContentOperationManager::NewLC()
    {
    CIAUpdateContentOperationManager* self =
        new( ELeave ) CIAUpdateContentOperationManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CIAUpdateContentOperationManager
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperationManager::CIAUpdateContentOperationManager()
: CActive( CActive::EPriorityStandard )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CIAUpdateContentOperationManager");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ConstructL begin");

    CActiveScheduler::Add( this );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ConstructL end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::~CIAUpdateContentOperationManager
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperationManager::~CIAUpdateContentOperationManager()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::~CIAUpdateContentOperationManager begin");

    // It is always good to call this in active object destructor 
    // to be sure that no operations are left hanging. 
    // This will also reset and delete all the necessary data if needed.
    Cancel();

    // Reset the array to be sure.
    // If the operation has left during startup, the array may not be reseted.
    iNodes.Reset();

    // Delete operation to make sure that it is also deleted.
    delete iOperation;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::~CIAUpdateContentOperationManager end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CheckErrorCode
// 
// ---------------------------------------------------------------------------
// 
TInt CIAUpdateContentOperationManager::CheckErrorCode( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::CheckErrorCode begin: %d",
                     aError);
    
    if ( aError > ( IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall 
                    - IAUpdateCtrlNodeConsts::KErrBaseRange ) 
         && aError <= IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall )
        {
        IAUPDATE_TRACE("[IAUPDATE] Service pack install");
        aError -= IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall;
        }
    else if ( aError > IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall 
              && aError <= IAUpdateCtrlNodeConsts::KErrBaseServicePackDownload )
        {
        IAUPDATE_TRACE("[IAUPDATE] Service pack download");
        aError -= IAUpdateCtrlNodeConsts::KErrBaseServicePackDownload;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::CheckErrorCode end: %d",
                     aError);

    return aError;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CheckErrorCode
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperationManager::TContentOperationType 
    CIAUpdateContentOperationManager::ServicePackOperationType( 
        MNcdPurchaseDetails& aNodeDetails )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CheckErrorCode begin");

    if ( !IAUpdateNodeFactory::IsServicePack( 
            aNodeDetails.AttributeString( 
                MNcdPurchaseDetails::EPurchaseAttributeContentMimeType ) ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CheckErrorCode end: ENoContentOperation");
        return ENoContentOperation;
        }
    
    TContentOperationType type( EPurchaseOperation );
    
    TInt errorCode( aNodeDetails.LastOperationErrorCode() );

    if ( errorCode > ( IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall 
                       - IAUpdateCtrlNodeConsts::KErrBaseRange ) 
         && errorCode <= IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall )
        {
        IAUPDATE_TRACE("[IAUPDATE] Service pack install");
        type = EInstallOperation;
        }
    else if ( errorCode > IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall 
              && errorCode <= IAUpdateCtrlNodeConsts::KErrBaseServicePackDownload )
        {
        IAUPDATE_TRACE("[IAUPDATE] Service pack download");
        type = EDownloadOperation;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::CheckErrorCode end: %d",
                     type);

    return type;     
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::SortNodeArray
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::SortNodeArray( 
    RPointerArray< CIAUpdateNode >& aNodes )
    {
    // Use that static function to sort the array.
    aNodes.Sort( TLinearOrder< CIAUpdateNode >( &NodeArraySorter ) );
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::GetOperationNodesL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::GetOperationNodesL( 
    const CIAUpdateNode& aNode, RPointerArray< CIAUpdateNode >& aNodes,
    TBool aAcceptHiddenDependencyNodes, TBool aAcceptVisibleDependencyNodes )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CreateOperationNodeArrayL() begin");
    
    // Reset the array.
    // So, new nodes can be inserted into it.
    aNodes.Reset();

    // We need to get the array of the nodes that this node depends on.
    // Notice, that here we just want the specific type of nodes that belong 
    // to this node. If another dependency node does not match the criteria then 
    // ignore that branch starting from that node.
    HandleDependenciesL( aNode, aNodes,
                         aAcceptHiddenDependencyNodes, 
                         aAcceptVisibleDependencyNodes );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CreateOperationNodeArrayL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::TotalContentSizeL
// 
// ---------------------------------------------------------------------------
//     
TInt CIAUpdateContentOperationManager::TotalContentSizeL( 
    const CIAUpdateNode& aNode,
    TBool aIncludeDownloaded,
    TBool aIncludeInstalled,
    TBool aAcceptHiddenDependencyNodes,
    TBool aAcceptVisibleDependencyNodes )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::TotalContentSizeL() begin");

    RPointerArray< CIAUpdateNode > nodes;
    CleanupClosePushL( nodes );
    GetOperationNodesL( 
        aNode, nodes,
        aAcceptHiddenDependencyNodes, 
        aAcceptVisibleDependencyNodes );
    TInt totalContentSize( 
        ArrayTotalContentSizeL( 
            nodes, aIncludeDownloaded, aIncludeInstalled ) );
    CleanupStack::PopAndDestroy( &nodes );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::TotalContentSizeL() end: %d",
                     totalContentSize);

    return totalContentSize;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::StartL
// 
// ---------------------------------------------------------------------------
//     
void CIAUpdateContentOperationManager::StartL( 
    MIAUpdateContentOperationObserver& aObserver,
    const CIAUpdateContentOperationManager::TContentOperationType& aOperationType,
    CIAUpdateNode& aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::StartL begin");

    if ( iOperation || IsActive() )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR Operation already existed");
        User::Leave( KErrInUse );
        }
    else if ( aNode.Hidden()  )
        {
        // The main node is not allowed to be hidden.
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Hidden node given");
        User::Leave( KErrArgument );
        }
    else if ( aOperationType != EPurchaseOperation
              && aOperationType != EDownloadOperation
              && aOperationType != EInstallOperation )
        {
        // The main node is not allowed to be hidden.
        IAUPDATE_TRACE("[IAUPDATE] ERROR Operation type not supported");
        User::Leave( KErrNotSupported );
        }

    // Reset member variables
    iSuccessCount = 0;

    // Set progress values to defaults.
    ResetProgress();
    
    // Notice, that this will also set the main node.
    SetNodeArrayL( aNode );

    // Progress values were resetted above.
    // Now, set the expected total maximum value for the progress.
    InitTotalMaxProgressL( aOperationType );

    // Set the observer
    iObserver = &aObserver;
    
    // Set the operation type
    iOperationType = aOperationType;

    // Start the operation by calling the first round for active loop.
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, KErrNone );                

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::StartL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::OperationType
// 
// ---------------------------------------------------------------------------
// 
const CIAUpdateContentOperationManager::TContentOperationType& 
    CIAUpdateContentOperationManager::OperationType() const
    {
    return iOperationType;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ContentOperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateContentOperationManager::ContentOperationComplete( 
    CIAUpdateBaseNode& /*aNode*/,
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ContentOperationComplete() begin");

    // Content operation has completed.
    // In normal cases, start new active loop. So, RunL will be called.
    // Notice, that if Cancel was called, this callback function is called from the
    // operation when its cancel is synchronously completed. Because Cancel is waiting 
    // for DoCancel function to complete the request, let it complete here. So, 
    // actual cancel can finish its job. We do not want to call callbacks of the observer 
    // of this operation manager after Cancel.

    if ( aError == KErrNone )
        {
        ++iSuccessCount;
        IAUPDATE_TRACE_1("[IAUPDATE] Successfull operation: %d",
                         iSuccessCount); 

        // Check that the node MIME is set correctly.
        // Then, the history will be shown also correctly for the nodes.
        // By updating the purchase history, also the correct MIME type
        // is set, for example, if node is hidden.
        IAUPDATE_TRACE("[IAUPDATE] Update purchase history for current node.");
        UpdatePurchaseHistory( *iCurrentNode, aError );
        }

    TRequestStatus* ptrStatus = &iStatus;
    User::RequestComplete( ptrStatus, aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ContentOperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ContentOperationProgress
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateContentOperationManager::ContentOperationProgress( 
    CIAUpdateBaseNode& /*aNode*/, 
    TInt aProgress, 
    TInt aMaxProgress )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ContentOperationProgress() begin");

    UpdateProgress( aProgress, aMaxProgress );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::ContentOperationProgress() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::DoCancel
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::DoCancel() begin");
    
    // If we are here, then there should always be an operation going on.

    // Notice, that if disclaimers are shown, for example,
    // when installing, then the KErrCancel can be gotten from
    // the operation even if the cancellation was not started from here.
    // But, then the RunL is called and the operation is finished correctly.
    // But, if cancellation is started here, the RunL is not called and the 
    // cancellation proceedes correctly. Instead then, operations call directly
    // ContentOperationComplete callback function that completes the request that
    // active object Cancel is listening to complete its actions.
    
    // Cancel the current operation by deleting it.
    delete iOperation;
    iOperation = NULL;

    // No need for the observer anymore.
    iObserver = NULL;


    // Because, the main node is the one that is visible, its error code
    // needs to be updated here separately. Also, the current node needs
    // to be updated. So, set them by hand. If other nodes were not operated,
    // no need to update their info to purchase history because they should
    // not be shown in the history view anyways.
    // Notice, that even if the operations may have updated the purchase history
    // by themselves in NCD Engine side, the purchase history MIME type may
    // need to be updated according to the IAD node values. So, that is why
    // we update the purchase history here one more time.
    // Notice, that in case of service packs and other service packs 
    // inside them, also then just set the purchase history for the head
    // service pack.

    if ( iNode != iCurrentNode )
        {
        IAUPDATE_TRACE("[IAUPDATE] Update current node purchase history with KErrCancel");
        UpdatePurchaseHistory( *iCurrentNode, KErrCancel );
        }

    IAUPDATE_TRACE("[IAUPDATE] Update head node purchase history with KErrCancel");
    UpdatePurchaseHistory( *iNode, KErrCancel );


    // No need for nodes
    iNodes.Reset();
    iNode = NULL;
    iCurrentNode = NULL;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::DoCancel() end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::RunL
// 
// ---------------------------------------------------------------------------
//     
void CIAUpdateContentOperationManager::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::RunL() begin");

    // Reset progress for current operation.
    ResetCurrentProgress();

    // Delete old operation.
    // Later a new and correct type of operation will be created for
    // the new node if one is found.
    delete iOperation;
    iOperation = NULL;

    // When the operation flow is started, the error code is always KErrNone.    
    // We only get errors to status if content operation is going on.
    User::LeaveIfError( iStatus.Int() );
    
    // Handle the next content in the array.
    if ( iNodes.Count() > 0 )
        {
        // Handle next node.
        SetCurrentNodeL();

        if ( IsServicePack( *iNode ) 
             && iCurrentNode->IsSelfUpdate() )
            {
            // Because operation was created, it means that the self update inside
            // the service pack is not installed yet. Because we do not support
            // self updates inside service packs, leave from here.
            
            // Note for future improvements: 
            // If service packs support for self update is added, remove
            // the if clause and always append nodes.
            // Also, remember to update CIAUpdateServicePackNode::IsSelfUpdate 
            // function if UI side should know about the self updates inside
            // the service pack. Then, most likely service pack should be thought 
            // as self update if any of the items gotten in the dependency hierarchy
            // is a self update.

            IAUPDATE_TRACE("[IAUPDATE] Service pack does not support self updates");
            User::Leave( KErrNotSupported );
            }


        // Notice, that in case of service packs, the operation is not created.
        // Skip, service pack items themself and only handle operations for 
        // their dependencies.

        TBool operationStarted( EFalse );      

        iOperation = CreateOperationL( iOperationType, *iCurrentNode );

        if ( iOperation )
            {
            IAUPDATE_TRACE("[IAUPDATE] Operation existed");
            operationStarted = iOperation->StartOperationL();
            if ( !operationStarted )
                {
                IAUPDATE_TRACE("[IAUPDATE] Operation already completed successfully");
                // Notice, we increase iSuccessCount value here
                // even if the operation has already been finished.
                // This is because in CompleteOperation function success count is
                // checked to give a correct error code to observer. If we do not
                // count already completed successes and for some reason a next type of
                // operation in the flow is stopped (for example cancelled), then
                // the continuation of the flow in a new try of the flow may be prevented.
                // For example, some of the items are downloaded and some had errors. But,
                // flow continues to install which is then cancelled. So, not all items
                // are installed. Then flow is started again, but no new items can be
                // downloaded because of errors. Then we need to count also previous
                // successess to make the flow to continue to the install operations of
                // previously cancelled items. 
                // So, by also counting previous successes we ensure that the flow 
                // will continue in all the cases. The downside of this is that in 
                // some cases the flow will continue even if there is nothing to do
                // in next type of operation, but it is better than let the flow 
                // been locked.
                ++iSuccessCount;
                }
            }

        // Make the active object to listen operation completion.
        iStatus = KRequestPending;
        SetActive();            

        if ( !operationStarted )
            {
            IAUPDATE_TRACE("[IAUPDATE] Operation was not started");
            // Complete the active request now. 
            // This way a new active loop will continue.
            // We come hear when a service pack has been already fully handled,
            // or if the item operation was already finished before.
            // There should be no need to update the progress here because
            // the node operation were already done. When the total progress
            // variables were initialized during the start of the operation, 
            // already handled items were skipped also then.
            // So, skip progress handling for them also now. 
            TRequestStatus* ptrStatus = &iStatus;
            User::RequestComplete( ptrStatus, KErrNone );                        
            }
        }
    else
        {
        // No other node content to handle.
        // So, complete the whole operation.
        CompleteOperation( KErrNone );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::RunL() end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::RunError
// 
// ---------------------------------------------------------------------------
// 
TInt CIAUpdateContentOperationManager::RunError( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::RunError() begin: %d",
                     aError);

    // By updating the purchase history here, the correct MIME type
    // is set, for example, if node is hidden. NCD Engine sets the
    // purchase history details already in its own operations but
    // we need to be sure that MIME type is set correctly according
    // to the IAD node values. Update current notde only if it was
    // not skipped.
    if ( iCurrentNode != iNode
         && aError != IAUpdateCtrlNodeConsts::KErrSkipNode )
        {
        IAUPDATE_TRACE("[IAUPDATE] Update purchase history for current node.");
        UpdatePurchaseHistory( *iCurrentNode, aError );
        }

    // Because, the main node is the one that is visible, its error code
    // needs to be updated here separately. So, set it by hand. If other
    // nodes were not handled, no need to update their info to purchase
    // history because they should not be shown in the history view
    // anyways.
    // Notice, that in case of service packs and other service packs 
    // inside them, also then just set the purchase history for the head
    // service pack.
    // Do not update the purchase history if the node has been skipped
    // inside a service pack. If node has been skipped during normal
    // dependency chain, then most likely UI tries to operate normal 
    // visible node whose dependency has not been completed before in
    // the update flow. Notice, that also in this case there may be some
    // hidden nodes that are installed before the main node. So, skipping
    // may also start from those hidden nodes.
    if ( aError != IAUpdateCtrlNodeConsts::KErrSkipNode
         || !IsServicePack( *iNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] Update purchase history for main node.");
        UpdatePurchaseHistory( *iNode, aError );
        }

    // If the head node is service pack,
    // one error does not prevent the whole head 
    // service pack operation.
    // If the head node is not a service pack, then the operation
    // will end here. 
    // Also, do not continue with service packs either if no node
    // is left in the node array.

    if ( iNodes.Count() > 0 && IsServicePack( *iNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] Service pack error accepted");

        // Because we are still going to continue the operation,
        // update the progrees for the error node.
        if ( !IsServicePack( *iCurrentNode ) )
            {
            // Skip service packs because their own size is zero.
            // In the progress bar point of view, just increase the
            // progress in a same way as if the operation had been completed
            // correctly.
            TInt newProgress( iCurrentMaxProgress );
            if ( newProgress == KInitProgress )
                {
                if ( iOperationType == EDownloadOperation )
                    {
                    TRAP_IGNORE ( newProgress += iCurrentNode->OwnContentSizeL() );
                    }
                else
                    {
                    ++newProgress;
                    }
                }
            // Update progress with new max values.
            // Notice, that if newProgress was left to KInitProgress,
            // the this function call will immediately return.
            UpdateProgress( newProgress, newProgress );  
            }

        // Continue to the next branch now that error is handled.
        IAUPDATE_TRACE("[IAUPDATE] Start new loop");
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* ptrStatus = &iStatus;
        User::RequestComplete( ptrStatus, KErrNone );
        }
    else
        {
        CompleteOperation( aError );       
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::RunError() end");
    
    // Return KErrNone. So, the application will not panic.
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::UpdateProgress
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::UpdateProgress( 
    TInt aProgress, TInt aMaxProgress )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateContentOperationManager::UpdateProgress() begin: %d, %d",
                     aProgress, aMaxProgress);

    if ( aProgress < 0 
         || aMaxProgress <= 0
         || ( aProgress == iCurrentProgress && aMaxProgress == iCurrentMaxProgress ) )
        {
        // Do not proceed if progress is negative or max progress is zero or less.
        // Also, do not proceed if nothing has changed.
        IAUPDATE_TRACE("[IAUPDATE] Progress values negative or zero or no changes. Return.");
        return;
        }

    TInt currentDif( aProgress - iCurrentProgress );
    if ( currentDif != 0 )
        {
        // Progress has occurred.
        IAUPDATE_TRACE_2("[IAUPDATE] New value for current: %d . Dif: %d", 
                         aProgress, currentDif);
        iTotalProgress += currentDif;
        IAUPDATE_TRACE_1("[IAUPDATE] New total current. Dif: %d", iTotalProgress);
        }

    iCurrentProgress = aProgress;
    iCurrentMaxProgress = aMaxProgress;
    if ( iCurrentProgress > iCurrentMaxProgress )
        {
        // A sanity check to be sure that max progress is at least as great as the
        // current progress.
        IAUPDATE_TRACE("[IAUPDATE] Progress was greater than max progress.");
        iCurrentMaxProgress = iCurrentProgress;
        }

    if ( iCurrentMaxProgress > iTotalMaxProgress )
        {
        // It seems that the total max progress has been set to too little value.
        // Update it to match the given data.
        iTotalMaxProgress = iCurrentMaxProgress;
        IAUPDATE_TRACE_1("[IAUPDATE] Current max greater than total max: %d", 
                         iTotalMaxProgress);
        }

    if ( iTotalProgress > iTotalMaxProgress )
        {
        // It seems that the total max progress has been set to too little value.
        // Update it to match the possible pending progress.
        iTotalMaxProgress = iTotalProgress + iCurrentMaxProgress - iCurrentProgress; 
        IAUPDATE_TRACE_1("[IAUPDATE] New total max: %d", iTotalMaxProgress);
        }

    IAUPDATE_TRACE("[IAUPDATE] Progress updated. Call observer.");
    iObserver->ContentOperationProgress( 
        *iCurrentNode, iTotalProgress, iTotalMaxProgress );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::UpdateProgress() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CompleteOperation
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::CompleteOperation( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::CompleteOperation() begin: %d",
                     aError);

    // First do service pack related checks and settings.
    if ( IsServicePack( *iNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] Head node is service pack");
        // Because node is a service pack, the error code that will be 
        // returned to observer needs to be checked. Notice, that if
        // an error occurred, then purchase history already contains
        // correct error code for the service pack. If everything was
        // a success, then update purchase history for the main service
        // pack. UI may use that function to get the last error value if
        // necessary. But, use KErrNone for callback parameter in all the 
        // cases. Then, the value suggests that UI may try to continue
        // the update flow.
        // Notice, here we also check self updates because they should be
        // handled from the UI before other items, such as service packs
        // are handled.
        if ( CheckNode( *iNode, ETrue ) )
            {
            IAUPDATE_TRACE("[IAUPDATE] Service pack thought as success");
            // Because no error occurred, the service pack error code
            // has not been yet set to the purchase history. So, it
            // needs to be set now.
            aError = KErrNone;
            UpdatePurchaseHistory( *iNode, aError );
            }
        else if ( iSuccessCount > 0 )
            {
            IAUPDATE_TRACE("[IAUPDATE] Some of operations were success");
            // If at least one item was successfully handled,
            // then return KErrNone in the callback function.
            // So, observer knows that update flow may continue
            // at least for some items.
            aError = KErrNone;
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] All item operations failed.");
            // All operations that were tried at this operation flow were failures.
            // Notice, that in preivous flows something may have been completed
            // but not now. So, set the current error code for the callback.
            // Here, use the function that gives the decoded error value
            // of the error that has been saved to purchase history for the
            // service pack.
            // The parameter value of aError is not accepted
            // because it may not give the service pack specific error
            // but, for example, the skip error of dependency chain skips.
            TRAPD ( trapError, 
                    aError = iNode->LastUpdateErrorCodeL() );
            if ( trapError != KErrNone )
                {
                IAUPDATE_TRACE_1("[IAUPDATE] Trap error: %d", trapError);
                // Something failed with the error code. Maybe out of memory.
                // So, use the trapError after this.
                aError = trapError;
                }
            else if ( aError == KErrNone )
                {
                IAUPDATE_TRACE("[IAUPDATE] Change error to KErrGeneral");
                // Use KErrGeneral as default if the correct
                // error code can not be gotten from the history.
                // Also, we do not return the possible trapError.
                // Instead, in this case, just give a vague error.
                aError = KErrGeneral;

                // Because error code was read from the history successfully
                // to aError but it was KErrNone, try to update the history
                // also here with the KErrGeneral.
                UpdatePurchaseHistory( *iNode, aError );
                }
            }
        }

    // Reset and set everything before calling observer complete because
    // the observer may delete this object immediately. So, take temporary
    // information for the function call.
    MIAUpdateContentOperationObserver* tmpObserver( iObserver );
    CIAUpdateNode* tmpNode( iNode );    
    
    // Observer is not needed anymore
    iObserver = NULL;    

    // No need for the operation anymore.
    delete iOperation;
    iOperation = NULL;

    // Reset the array because operations will not continue.
    iNodes.Reset();

    // Node not needed anymore.
    iNode = NULL;
    iCurrentNode = NULL;

    // Inform the observer about the completion of the operation. 
    // Give the main node as a parameter.
    tmpObserver->ContentOperationComplete( *tmpNode, aError );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CompleteOperation() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ResetProgress
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::ResetProgress()
    {
    ResetCurrentProgress();
    iTotalMaxProgress = KInitProgress;
    iTotalProgress = KInitProgress;
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ResetCurrentProgress
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::ResetCurrentProgress()
    {
    iCurrentMaxProgress = KInitProgress;
    iCurrentProgress = KInitProgress;    
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::InitTotalMaxProgressL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::InitTotalMaxProgressL( 
    const CIAUpdateContentOperationManager::TContentOperationType& aOperationType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::InitTotalMaxProgressL() begin");
                     
    // For download operation use the given content size for expected progress values.
    // For other operations just use the node count.
    if ( aOperationType == CIAUpdateContentOperationManager::EDownloadOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Download operation");
        // Notice, that here we do not want to include already downloaded 
        // or installed nodes into the total contant size.
        iTotalMaxProgress = 
            ArrayTotalContentSizeL( iNodes, EFalse, EFalse );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Not download operation");
        iTotalMaxProgress = iNodes.Count();        
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::InitTotalMaxProgressL() end: %d",
                     iTotalMaxProgress);
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::ArrayTotalContentSizeL
// 
// ---------------------------------------------------------------------------
//     
TInt CIAUpdateContentOperationManager::ArrayTotalContentSizeL( 
    const RPointerArray< CIAUpdateNode >& aNodes,
    TBool aIncludeDownloaded,
    TBool aIncludeInstalled )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ArrayTotalContentSizeL() begin");

    TInt totalContentSize( 0 );

    TInt count( aNodes.Count() );
    IAUPDATE_TRACE_1("[IAUPDATE] Node count: %d", count);
    for ( TInt i = 0; i < count; ++i )
        {
        CIAUpdateNode& node( *aNodes[ i ] );
        TBool skipNode( !aIncludeDownloaded && node.IsDownloaded()
                        || !aIncludeInstalled && node.IsInstalled()
                        || IsServicePack( node ) );
        if ( !skipNode )
            {
            IAUPDATE_TRACE_1("[IAUPDATE] Node accepted: %d", i);
            totalContentSize += node.OwnContentSizeL();           
            }
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::ArrayTotalContentSizeL() end: %d",
                     totalContentSize);

    return totalContentSize;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::SetNodeArrayL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::SetNodeArrayL( CIAUpdateNode& aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::SetNodeArrayL() begin");
    
    // Reset and put required nodes into the node array.
    // Only accept hidden dependency nodes. Skip visible nodes.
    GetOperationNodesL( aNode, iNodes, ETrue, EFalse );

    // Sort the created node array according to the
    // node depths. This way the operation can just use the
    // last node and there is no need to check dependencies
    // because the leaf nodes are in the end of the array after sorting.
    // This provides some what optimized way to handle things later.
    // Also, note that here trust that no loops will be in the dependency
    // chains. If loops may occur, then the depths of the nodes may not be
    // correct because in a loop there is now way to decide what is first and
    // what is last.
    SortNodeArray( iNodes );

    // Ownership is not taken.
    iNode = &aNode;
    iCurrentNode = NULL;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::SetNodeArrayL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::HandleDependenciesL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::HandleDependenciesL( 
    const CIAUpdateNode& aNode,
    RPointerArray< CIAUpdateNode >& aNodes,
    TBool aAcceptHiddenDependencyNodes,
    TBool aAcceptVisibleDependencyNodes )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::HandleDependenciesL() begin");
    
    if ( !aAcceptHiddenDependencyNodes && !aAcceptVisibleDependencyNodes
         || aNodes.Find( &aNode ) != KErrNotFound )
        {
        // Nothing is accepted. So, no need to do anything.
        // Or the node has already been inserted into the array, which means that
        // branch has already been handled.
        IAUPDATE_TRACE("[IAUPDATE] Nothing to do for this branch");
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::HandleDependenciesL() end");
        return;
        }

    // Insert all the required nodes into the array.
    // Here we know, that the given node should be inserted into the array.
    aNodes.AppendL( &aNode );

    // Create temporary array for the nodes that current node depends on.
    RPointerArray< CIAUpdateNode > dependencyNodes;
    CleanupClosePushL( dependencyNodes );

    // Get nodes that this node depends on into the temporary list.
    aNode.GetDependencyNodesL( dependencyNodes, ETrue );
    for ( TInt i = 0; i < dependencyNodes.Count(); ++i )
        {
        CIAUpdateNode& node( *dependencyNodes[ i ] );
        if ( aAcceptHiddenDependencyNodes && node.Hidden() 
             || aAcceptVisibleDependencyNodes && !node.Hidden() )
            {
            // Because node fills the criteria, handle the tree below it
            // to get all the nodes of the dependencies.
            HandleDependenciesL( node, aNodes, 
                                 aAcceptHiddenDependencyNodes, 
                                 aAcceptVisibleDependencyNodes );
            }
        }

    CleanupStack::PopAndDestroy( &dependencyNodes );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::HandleDependenciesL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CreateOperationL
// 
// ---------------------------------------------------------------------------
// 
MIAUpdateOperation* CIAUpdateContentOperationManager::CreateOperationL(
    const CIAUpdateContentOperationManager::TContentOperationType& aOperationType,
    CIAUpdateNode& aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CreateOperationL() begin");
    
    if ( IsServicePack( aNode ) )
        {
        // Because node is a service pack, no operations are
        // created for it. So, just return the NULL value.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CreateOperationL() end: NULL");
        return NULL;
        }

    MIAUpdateOperation* operation( NULL );

    switch ( aOperationType )
        {
        case EPurchaseOperation:
            operation = 
                CIAUpdatePurchaseOperation::NewL( aNode, *this );
            break;

        case EDownloadOperation:
            if ( IsServicePack( *iNode ) && !aNode.IsPurchased() )
                {
                // Already previous operation has failed.
                // So, skip the operation for this node.
                IAUPDATE_TRACE("[IAUPDATE] Skip download because not purhcased, KErrSkipNode"); 
                User::Leave( IAUpdateCtrlNodeConsts::KErrSkipNode );
                }
            operation = 
                CIAUpdateDownloadOperation::NewL( aNode, *this );
            break;

        case EInstallOperation:
            if ( IsServicePack( *iNode ) && !aNode.IsDownloaded() )
                {
                // Already previous operation has failed.
                // So, skip the operation for this node.
                IAUPDATE_TRACE("[IAUPDATE] Skip install because not downloaded, KErrSkipNode"); 
                User::Leave( IAUpdateCtrlNodeConsts::KErrSkipNode );
                } 
            if ( aNode.IsSelfUpdate() )
                {
                operation = 
                    CIAUpdateSelfUpdateInitOperation::NewL( aNode, *this );
                }
            else
                {
                operation = 
                    CIAUpdateInstallOperation::NewL( aNode, *this );
                }
            break;

        case ENoContentOperation:
            // Let it be NULL.
            // Should not ever come here.
            break;

        default:
            // Let it be NULL.
            // Should not ever come here.
            User::Leave( KErrNotSupported );
            break;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CreateOperationL() end");

    return operation;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::UpdatePurchaseHistory
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperationManager::UpdatePurchaseHistory( 
    CIAUpdateNode& aNode, TInt aError ) const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::UpdatePurchaseHistory() begin: %d",
                     aError);

    if ( IsServicePack( aNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] Is service pack");
        // Because NCD API does not provide means to update the purchase details
        // type, it is always EStatePurchased. So, here we edit the error code.
        // Later, by checking the base code of the error, the correct operation
        // can be concluded.
        switch ( iOperationType )
            {
            case EDownloadOperation:
                IAUPDATE_TRACE("[IAUPDATE] Download operation");    
                aError += IAUpdateCtrlNodeConsts::KErrBaseServicePackDownload;
                break;

            case EInstallOperation:
                IAUPDATE_TRACE("[IAUPDATE] Install operation");
                aError += IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall;
                break;

            default:
                // For purchase operations just use the default value.
                IAUPDATE_TRACE("[IAUPDATE] Default");
                break;
            }
        }

    // In normal cases, just update the history error value.
    // If service pack is handled, then the error code is edited above
    // to make it a correct value.
    IAUPDATE_TRACE_1("[IAUPDATE] Set error to history: %d", aError);
    TRAP_IGNORE ( aNode.SetIdleErrorToPurchaseHistoryL( aError, EFalse ) );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::UpdatePurchaseHistory() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::IsServicePack
// 
// ---------------------------------------------------------------------------
// 
TBool CIAUpdateContentOperationManager::IsServicePack( 
    const CIAUpdateNode& aNode )
    {
    return ( aNode.Type() == MIAUpdateNode::EPackageTypeServicePack );
    }



// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::SetCurrentNodeL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateContentOperationManager::SetCurrentNodeL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager:SetCurrentNodeL() begin");
    
    // Take the node from the end of the array because root of the dependency
    // hierarchy starts from the beginning of the array and leaves are in the end.
    TInt index( iNodes.Count() - 1 );
    iCurrentNode = iNodes[ index ];
    iNodes.Remove( index );

    // In case of service packs an error does not complete the whole operation chain. 
    // So, check here if some nodes need to be skipped because their dependencies are 
    // not operated correctly.
    // Also, this check is needed if UI tries to operate nodes whose dependencies
    // were not handled successfully before.

    IAUPDATE_TRACE("[IAUPDATE] Current node is not service pack");

    RPointerArray< CIAUpdateNode > dependencies;
    CleanupClosePushL( dependencies );

    iCurrentNode->GetDependencyNodesL( dependencies, ETrue );

    // Check all the dependencies.
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNode& dependency( *dependencies[ i ] );
        if ( !CheckNode( dependency, EFalse ) )
            {
            // In normal cases, the flow will be immediately completed 
            // if error occurs. If we come here it means that the operation
            // has still been continued and we are handling a service pack.
            // Because the dependency node is reason of the error for this leaf,
            // leave here with the specified skip error. So, later
            // we know that the reason of the leave is this skip.
            IAUPDATE_TRACE("[IAUPDATE] Error: KErrSkipNode");
            User::Leave( IAUpdateCtrlNodeConsts::KErrSkipNode );
            }
        }

    CleanupStack::PopAndDestroy( &dependencies );        

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager:SetCurrentNodeL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperationManager::CheckNode
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateContentOperationManager::CheckNode( 
    const CIAUpdateNode& aNode,
    TBool aCheckSelfUpdate ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperationManager::CheckNode() begin");

    TBool checkPassed( EFalse );

    if ( aNode.Hidden() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency node is hidden");
        // In service packs and in hidden dependency chains, update flow goes 
        // so that same operation is done to all of the items in that dependency
        // chain before continuing to another type of operation.
        // So, check accordingly.
        switch ( iOperationType )
            {
            case EPurchaseOperation:
                if ( aNode.IsPurchased()
                     || aNode.IsDownloaded() 
                     || aNode.IsInstalled() )
                    {
                    // Notice, item may be in more advanced state
                    // than is required at the moment. If operations
                    // have been done from somewhere else. For example,
                    // content may have been installed outside of IAD.
                    checkPassed = ETrue;
                    }
                break;
            
            case EDownloadOperation:
                if ( aNode.IsDownloaded() 
                     || aNode.IsInstalled() )
                    {
                    // Notice, item may be in more advanced state
                    // than is required at the moment. If operations
                    // have been done from somewhere else. For example,
                    // content may have been installed outside of IAD.
                    checkPassed = ETrue;
                    }
                break;
                
            case EInstallOperation:
                if ( !aCheckSelfUpdate && aNode.IsSelfUpdate() )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Always accept self update during install");
                    checkPassed = ETrue;
                    }
                else
                    {
                    checkPassed = aNode.IsInstalled();
                    }
                break;

            case ENoContentOperation:
                break;
                
            default:
                break;
            }        
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency node is visible");
        // Because dependency node is some visible node, it should have been
        // installed before the update flow of this node has been started.
        // So, if dependency node is not installed, then check will fail.
        if ( !aCheckSelfUpdate && aNode.IsSelfUpdate() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Always accept self update during install");
            checkPassed = ETrue;
            }
        else
            {
            checkPassed = aNode.IsInstalled();
            }
        }


    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateContentOperationManager::CheckNode() end: %d",
                     checkPassed);

    return checkPassed;
    }

