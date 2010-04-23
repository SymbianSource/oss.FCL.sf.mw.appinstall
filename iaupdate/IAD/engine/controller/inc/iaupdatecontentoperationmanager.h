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


#ifndef IA_UPDATE_CONTENT_OPERATION_MANAGER_H
#define IA_UPDATE_CONTENT_OPERATION_MANAGER_H


#include <e32base.h>
#include <e32cmn.h>

#include "iaupdatecontentoperationobserver.h"

class CIAUpdateBaseNode;
class CIAUpdateNode;
class MIAUpdateOperation;
class MNcdPurchaseDetails;


/**
 * CIAUpdateContentOperationManager
 * This will provide base functionality that can handle installing 
 * of hidden dependency nodes when one operation is requested
 * for one visible node.
 *
 * @note Functions of this class trust that node dependencies do not loop.
 * If dependency loops are set into nodes, then the operations may go
 * into forever loop. So, when dependency links between nodes are created,
 * the loops need to be handled already then.
 */
class CIAUpdateContentOperationManager : public CActive,
                                         public MIAUpdateContentOperationObserver
    {

public:

    /**
     * This defines the possible types of operations.
     */
    enum TContentOperationType
        {
        /**
         * No content operation is created for this.
         */
        ENoContentOperation,

        /**
         * Purchase operation should be created for this.
         */
        EPurchaseOperation,

        /**
         * Download operation should be created for this.
         */
        EDownloadOperation,

        /**
         * Install operation should be created for this.
         */
        EInstallOperation
        };


    /**
     * @return CIAUpdateContentOperationManager* Created object.
     * Ownership is transferred.
     */
    static CIAUpdateContentOperationManager* NewL();
    
    /**
     * @see CIAUpdateContentOperationManager:: NewL
     */ 
    static CIAUpdateContentOperationManager* NewLC();


    /**
     * Destructor
     */
    virtual ~CIAUpdateContentOperationManager();


    /**
     * If aError contains some base error value
     * this function will remove the base value 
     * and returns the actual error code.
     *
     * @param aError Error code that needs to be checked.
     * @return TInt Correct error code.
     */
    static TInt CheckErrorCode( TInt aError );

    /**
     * This function can be used to check the last operation type of the
     * service pack.
     *
     * @param aNodeDetails Purchase history details of a node.
     * @return TContentOperationType Operation type which describes the last
     * operation that was done for the node. As a default this is EPurchaseOperation,
     * but if download or install is tried then result is according to that.
     * If the given node is not a service pack, then ENoContentOperation
     * is returned.
     */
    static TContentOperationType ServicePackOperationType( 
        MNcdPurchaseDetails& aNodeDetails );


    /**
     * This function will sort the given array according to node depths.
     * Then, all leaf nodes will be after their dependants in the
     * array and root nodes are always in the beginning. This function
     * can be used, for example, if recursion hierarchy is not what is wanted.
     *
     * @param aNodes Node array that will be sorted.
     */
    static void SortNodeArray( RPointerArray< CIAUpdateNode >& aNodes );


    /**
     * Uses HandleDependenciesL to create the node list.
     *
     * @param aNode Node whose dependencies are gone through.
     * @param aNodes Array that will contain all the nodes that are part of 
     * the operation directed for this node.
     * This means that all the wanted node branches that this node depends on
     * are included into the array. The given node is set as first in the array.
     * The tree hierarchy will be created recursive and leaves are towards the end 
     * of the array.
     * @param aAcceptHiddenDependencyNodes If ETrue, then all the hidden 
     * dependency nodes and their branches are included. If EFalse the hidden
     * dependency nodes are skipped.
     * @param aAcceptVisibleDependencyNodes If ETrue, then all the non-hidden 
     * dependency nodes and their branches are included. If EFalse the non-hidden
     * dependency nodes are skipped.
     */
    static void GetOperationNodesL( const CIAUpdateNode& aNode, 
                                    RPointerArray< CIAUpdateNode >& aNodes,
                                    TBool aAcceptHiddenDependencyNodes,
                                    TBool aAcceptDependencyVisibleNodes );


    /**
     * @param aNode The head node of the hierarchy whose total content
     * size is counted.
     * @param aIncludeDownloaded ETrue if already downloaded node content
     * should be included into the total size. Else EFalse.
     * @param aIncludeInstalled ETrue if already installed node content
     * should be included into the total size. Else EFalse.
     * @param aAcceptHiddenDependencyNodes If ETrue, then all the hidden 
     * dependency nodes and their branches are included. If EFalse the hidden
     * dependency nodes are skipped.
     * @param aAcceptVisibleDependencyNodes If ETrue, then all the non-hidden 
     * dependency nodes and their branches are included. If EFalse the non-hidden
     * dependency nodes are skipped.
     * @return TInt The content size of this node and contents it depends on.
     */
    static TInt TotalContentSizeL( const CIAUpdateNode& aNode,
                                   TBool aIncludeDownloaded,
                                   TBool aIncludeInstalled,
                                   TBool aAcceptHiddenDependencyNodes,
                                   TBool aAcceptDependencyVisibleNodes );


    /**
     * Starts the operation.
     *
     * @param aObserver This observer will be informed when
     * operation is completed.
     * @param aOperationType Type of the operation for node content.
     * @param aNode The main node that will be inserted to the node list.
     * Hidden nodes that belong to the dependency tree under this node
     * are also inserted to the list.
     * @exception Leaves with KErrInUser if the operation has
     * already been started and not completed. Leaves with
     * KErrArgument if the node is set as hidden. In other cases,
     * leaves with system wide error codes.
     */
    void StartL( MIAUpdateContentOperationObserver& aObserver,
                 const TContentOperationType& aOperationType,
                 CIAUpdateNode& aNode );


    /**
     * @return const TContentOperationType& Operation type of the current or last
     * operation. This value is set when StartL is called.
     */
    const TContentOperationType& OperationType() const;
    

public: // MIAUpdateContentOperationObserver

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationComplete
     *
     * This is called when one of the nodes in the node chain has completed.
     * This does not mean that the operation as a whole has finished yet.
     *
     */
    virtual void ContentOperationComplete( CIAUpdateBaseNode& aNode, 
                                           TInt aError );

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationProgress
     */
    virtual void ContentOperationProgress( CIAUpdateBaseNode& aNode, 
                                           TInt aProgress, 
                                           TInt aMaxProgress );


protected: // CActive

    /**
     * @see CActive::DoCancel
     *
     * Cancels the operation.
     */
    virtual void DoCancel();

    /**
     * @see CActive::RunL
     *
     * Calls HandleContentL for the current node from the node list.
     * If the previous operation has failed, then other operations
     * will not be continued. Observer is informed about the completion
     * of the jobs with error code or KErrNone if operation was successfull
     * as a whole.
     */
    virtual void RunL();    

    /**
     * @see CActive::RunError
     */
    virtual TInt RunError( TInt aError );
 
 
private:
    
    // Prevent these if not implemented
    CIAUpdateContentOperationManager( 
        const CIAUpdateContentOperationManager& aObject );
    CIAUpdateContentOperationManager& operator =( 
        const CIAUpdateContentOperationManager& aObject );


    /**
     * Constructor
     */ 
    CIAUpdateContentOperationManager();

    /**
     * 2nd. phase constructor
     */ 
    void ConstructL();


    /**
     * If a progress value was changed. Then, observer is informed.
     *
     * @param aProgress Progress value for current operation.
     * @param aMaxProgress Progress value for current operation.
     */
    void UpdateProgress( TInt aProgress, TInt aMaxProgress );
 
    // Called when all the nodes have been handled or if error has occurred
    // during operation chain.
    void CompleteOperation( TInt aError );


    // Resets progress values to their defaults.
    void ResetProgress();

    // Resets only current progress values to their defaults.
    void ResetCurrentProgress();


    // Initialize total maximum progress value to according to
    // the type of the operation.
    void InitTotalMaxProgressL(
        const TContentOperationType& aOperationType );


    // This function calculates the whole content size of the nodes 
    // in the array.
    static TInt ArrayTotalContentSizeL( 
        const RPointerArray< CIAUpdateNode >& aNodes,
        TBool aIncludeDownloaded,
        TBool aIncludeInstalled );
    

    // Insert this node and all the hidden dependencies under it
    // into the member node array.
    // Also, reset all the variables related to this.
    void SetNodeArrayL( CIAUpdateNode& aNodes );


    /**
     * Handle node dependencies and correct nodes into the array.
     *
     * @param aNode Node that is handled.
     * @param aNodes Node array that will contain the whole dependency hierarchy.
     * @param aAcceptHiddenDependencyNodes If ETrue, then all the hidden 
     * dependency nodes and their branches are included. If EFalse the hidden
     * dependency nodes are skipped.
     * @param aAcceptVisibleDependencyNodes If ETrue, then all the non-hidden 
     * dependency nodes and their branches are included. If EFalse the non-hidden
     * dependency nodes are skipped.
     */
    static void HandleDependenciesL( 
        const CIAUpdateNode& aNode,
        RPointerArray< CIAUpdateNode >& aNodes,
        TBool aAcceptHiddenDependencyNodes,
        TBool aAcceptVisibleDependencyNodes );


    // Creates the operation the corresponds the given operation type and node.
    // Ownership of the operation is transferred.
    MIAUpdateOperation* CreateOperationL( 
        const TContentOperationType& aOperationType,
        CIAUpdateNode& aNode );


    // Updates the purchase history of the node with the given 
    // error code.
    void UpdatePurchaseHistory( 
        CIAUpdateNode& aNode, TInt aError ) const;


    // Checks if the node is a service pack.
    static TBool IsServicePack( const CIAUpdateNode& aNode );


    // Set the current node for the operation flow
    void SetCurrentNodeL();


    // Check if the operation has been successfully done for the given node.
    // If aCheckSelfUpdate is ETrue, then also self updates are checked
    // when install operation is going on. If aCheckSelfUpdate is EFlase, 
    // then check is always passed for self update installs. This is because 
    // self updates will be installed separately later. So, self updates can
    // not be checked here in the middle of the install flow.
    TBool CheckNode( const CIAUpdateNode& aNode,
                     TBool aCheckSelfUpdate ) const;


private: // data

    // All the nodes that will be handled by HandleContentL function
    // during active loops. Does not own nodes.
    RPointerArray< CIAUpdateNode > iNodes;

    // The main node that is given when the operation is requested and started.
    // Not owned.
    CIAUpdateNode* iNode;

    // The node that is currently handled with the operation.
    // Not owned.
    CIAUpdateNode* iCurrentNode;

    // When operation is started, the observer is set.
    // When operation finishes observer is informed and this variable
    // is set to NULL.
    // Not owned.
    MIAUpdateContentOperationObserver* iObserver;

    // Operation type informs what kind of operations should be created
    // for the nodes when flow proceeds.
    TContentOperationType iOperationType;

    // Current operation.
    // Owned.
    MIAUpdateOperation* iOperation;

    // Defines total maximum progress value for the operation.    
    TInt iTotalMaxProgress;

    // Defines total progress value for the operation.
    TInt iTotalProgress;

    // Defines current maximum progress value for the operation.    
    TInt iCurrentMaxProgress;

    // Defines progress value for the current operation.
    TInt iCurrentProgress;

    // This variable is used to count how many operations
    // were successfull in the flow.
    TInt iSuccessCount;

    };

#endif // IA_UPDATE_CONTENT_OPERATION_MANAGER_H

