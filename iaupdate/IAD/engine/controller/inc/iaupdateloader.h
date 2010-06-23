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
* Description:   This file contains the header file of CIAUpdateLoader class 
*
*/



#ifndef IA_UPDATE_LOADER_H
#define IA_UPDATE_LOADER_H


// INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include <ncdloadnodeoperationobserver.h>

#include "iaupdateoperationinfo.h"


// FORWARD DECLARATIONS
class MNcdProvider;
class MNcdNode;
class MNcdNodeContainer;
class MNcdLoadNodeOperation;
class MNcdQuery;
class MIAUpdateLoaderObserver;


/**
 * CIAUpdateLoader provides functions to load node hierarchy from 
 * the internet server.
 */
class CIAUpdateLoader : public CBase,
                        public MNcdLoadNodeOperationObserver
    {

public:

    /**
     * @param aProvider NCD Engine provider
     * @param aObserver Observer whose callbacks are called when operations
     * progress.
     * @return CIAUpdateLoader* Newly created object. 
     * Ownership is transferred.
     */
    static CIAUpdateLoader* NewLC( MNcdProvider& aProvider, 
                                   MIAUpdateLoaderObserver& aObserver );

    /**
     * @see CIAUpdateLoader::NewLC
     */
    static CIAUpdateLoader* NewL( MNcdProvider& aProvider,
                                  MIAUpdateLoaderObserver& aObserver );

    
    /**
     * Destructor
     */
    virtual ~CIAUpdateLoader();


    /**
     * This function can be used to check if the root node is expired.
     * If ETrue is returned, then CDB connection is also required to
     * update the root when nodes are loaded.
     *
     * @return TBool ETrue, if root is expired. Else, EFalse.
     */
    TBool RootExpiredL() const;
    

    /**
     * LoadNodesL()
     *
     * Loads the whole node hierarchy.
     *
     * @note Load for the root is done only if the root is not 
     * in initilized state. This way CDB connections, that can be 
     * a bottle neck with massive amount of connections, are minimized 
     * and the server side has power to define CDB connection rate.
     *
     * @note Even if root would not be refreshed because it is already
     * initialized, the hierarchy below it is always refreshed when 
     * this function is called -- even if the nodes were in initialized
     * state.
     */
    void LoadNodesL();


    /**
     * Cancels the possible on going operations.
     */
    void Cancel();


    /**
     * @note If this flag is set to ETrue, it means that a correct child count
     * is not requested from the server for the parent folder. Therefore, in every 
     * request all the children are requested. This way parent folder does not need 
     * to be refreshed to get a correct child count before downloading its children.
     *
     * @note If a correct child count is required by NCD Engine, then
     * this value is resetted back to EFalse when operation flow progresses. 
     * EFalse is a default value for this.
     *
     * @param aOptimize ETrue means that skip child count requests.
     * EFalse means that requests are sent in a normal way.
     */
    void SetSkipChildCountRefresh( TBool aSkip );


public: // MNcdLoadNodeOperationObserver

    /**
     * @see MNcdLoadNodeOperationObserver::NodesUpdated
     */
    virtual void NodesUpdated( MNcdLoadNodeOperation& aOperation,
                               RCatalogsArray< MNcdNode >& aNodes );

    /**
     * @see MNcdLoadNodeOperationObserver::QueryReceived
     */
    virtual void QueryReceived( MNcdLoadNodeOperation& aOperation,
                                MNcdQuery* aQuery );
                                
    /**
     * @see MNcdLoadNodeOperationObserver::OperationComplete
     */
    virtual void OperationComplete( MNcdLoadNodeOperation& aOperation,
                                    TInt aError );                               


private: // methods

    // Prevent these if not implemented
    CIAUpdateLoader( const CIAUpdateLoader& aObject );
    CIAUpdateLoader& operator =( const CIAUpdateLoader& aObject );

    
    /**
     * C++ constructor
     */
    CIAUpdateLoader( MNcdProvider& aProvider,
                     MIAUpdateLoaderObserver& aObserver);
    
    /**
     * 2nd phase contructorC++ constructor
     */
    void ConstructL();


    /**
     * LoadRootL
     * 
     * Loads the root node if it is not in initialized state.
     * Also, loads the required node hierarchy below root in
     * all the cases -- even if those nodes were in initialized
     * state.
     */
    void LoadRootL();


    /**
     * Loads children of the parent container one by one.
     * This is used if the child containers should be first
     * updated before loading their children.
     *
     * @param aParentContainer Container whose child containers will
     * be loaded.
     */
    void LoadChildContainersL( MNcdNodeContainer& aParentContainer );


    /**
     * Load container and the required hierarchy below it.
     *
     * @note This function should not be used for the root node.
     * Instead, use LoadRootL if the refresh should be started
     * from the root.
     * 
     * @note When this function is called, first load is made
     * for the container itself and then load children function
     * is used fo its children. By calling load first, we can be
     * sure that the container itself has the latest data before
     * updating its children.
     *
     * @param aContainer Parent container.
     **/
    void LoadContainerL( MNcdNodeContainer& aContainer );


    /**
     * Load children of the given container, but not the container itself.
     *
     * @note This function should not be used to load children of the root.
     * Instead, use LoadRootL or LoadChildContainersL for that. Root is an 
     * exception and should be handled as such.
     * 
     * @param aContainer Parent container.
     */
    void LoadChildrenL( MNcdNodeContainer& aContainer );


    /**
     * @note This function is provided for special optimization
     * of the amount of server requests. This can be used if all
     * the children of the container should be downloaded and
     * a specially defined value is used for child counts instead
     * of requesting the correct count value firts from the server.
     * 
     * @see CIAUpdateLoader::LoadAllChildrenL
     */
    void LoadAllChildrenL( MNcdNodeContainer& aContainer );


    /**
     * This is used to load grand children of the container.
     * Dependening on the aPreviousOperationType, children 
     * are loaded as a group or one by one.
     *
     * @param aContainer Container whose children of children will be
     * loaded.
     * @param aPreviousOperationType Previously completed operation.
     * Describes what kind of operation for the child container should 
     * be created.
     */
    void LoadChildrenOfChildrenL( 
        MNcdNodeContainer& aContainer,
        TIAUpdateOperationInfo::TOperationType aPreviousOperationType );


    /**
     * @param aContainer Container to which the load operation is directed.
     * @param aOperationType Describes what kind of operation for the
     * container should be created.
     */
    void StartLoadOperationL( 
        MNcdNodeContainer& aContainer,
        TIAUpdateOperationInfo::TOperationType aOperationType );


    /**
     * Starts next level of node loading for the container if necessary.
     *
     * @param aNode Node that should be a container. Its children will be
     * loaded in next round if necessary.
     * @param  aPrviousOperationType Operation type of the previous round.
     */
    void NextLoadLoopL( 
        MNcdNode& aNode,
        TIAUpdateOperationInfo::TOperationType aPreviousOperationType );
    
    
private: // data

    // Not owned by this class object.
    MNcdProvider& iProvider;
    MIAUpdateLoaderObserver& iObserver;

    // Information about started operations.
    RArray< TIAUpdateOperationInfo > iOperations;
    
    // ETrue if cancellation of operations is going on.
    TBool iCancelling;
    
    // This will contain the error code that is
    // given for the observer when operation completes.
    TInt iErrorCode;

    // This flag informs if child counts should be refreshed
    // from server before children are downloaded. This can be
    // used to skip unwanted refresh requests to the server.
    TBool iSkipChildCountRefresh;
    
    };

#endif // IA_UPDATE_LOADER_H
