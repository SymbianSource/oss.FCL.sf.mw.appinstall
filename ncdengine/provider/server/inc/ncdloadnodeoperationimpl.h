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


#ifndef C_NCDLOADNODEOPERATIONIMPL_H
#define C_NCDLOADNODEOPERATIONIMPL_H

#include <e32base.h>

#include "ncdbaseoperation.h"
#include "ncdparserobserver.h"
#include "catalogshttpobserver.h"
#include "catalogshttptypes.h"
#include "catalogshttpsession.h"
#include "ncdoperationobserver.h"
#include "ncdchildloadmode.h"
#include "ncdnodefactory.h"
// move content source to it's own header and include it
// instead of root op's header
#include "ncdloadrootnodeoperationimpl.h"

//Constants.
const TInt KNcdStructPageSize(48);


class CDesC16Array;
class CDesC16ArrayFlat;
class CDesC8ArrayFlat;
class MNcdParser;
class CNcdNodeManager;
class MNcdProtocol;
class CNcdNodeIdentifier;
class MCatalogsContext;
class MNcdPreminetProtocolEntityRef;
class MNcdPreminetProtocolDataEntity;
class CNcdQuery;
class MNcdOperationRemoveHandler;
class MCatalogsAccessPointManager;
class MNcdOperationQueue;


class TNcdResponseFilterParams
    {
public:
    TNcdResponseFilterParams()
        {
        iPageSize = 0;
        iPageStart = 0;
        iStructureDepth = 0;
        iMetadataDepth = 0;
        iMetadataPerLevel = 0;
        }
public:
    TInt iPageSize;
    TInt iPageStart;
    TInt iStructureDepth;
    TInt iMetadataDepth;
    TInt iMetadataPerLevel;
    };

const TInt KNcdLoadNodeErrNothingToDo(-1000000);
const TInt KNcdLoadNodeErrRemoteOnly(-1000001);

/**
 *  Load node operation implementation.
 *
 *  Handles loading nodes from server.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdLoadNodeOperationImpl : public CNcdBaseOperation,
                                  public MNcdParserObserver,
                                  public MNcdParserEntityObserver,
                                  public MCatalogsHttpObserver,
                                  public MNcdParserDataBlocksObserver
    {
protected:
class CNcdNodeIconMap;    
public:

    /** Sub states of load node operation */
    enum TLoadNodeOperationState
        {
        ESendRequest,      // create and send a browse request
        EReceive,          // receive and parse node data
        //EQuery,            // a query is pending
        ERemote,           // create sub operations for remote folders
        EReceiveRemote,    // wait completion of sub operations
        EComplete,         // everything done
        EFailed            // operation failed
        };
        
    enum TLoadMode
        {
        EContentSource,
        ESingleNode,
        EChildren,
        };
    
    
    /**
     * @param aCreateParent Creates the parent node with the given purpose
     * if it does not exist yet.
     */
    static CNcdLoadNodeOperationImpl* NewL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier,
        CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
        TNcdResponseFilterParams aFilterParams,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TBool aLoadChildren = EFalse,
        TNcdChildLoadMode aMode = ELoadStructure,
        TBool aIsSubOperation = EFalse,
        TBool aCreateParent = ETrue );

    /**
     * @param aCreateParent Creates the parent node with the given purpose
     * if it does not exist yet.
     */
    static CNcdLoadNodeOperationImpl* NewLC(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier,
        CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
        TNcdResponseFilterParams aFilterParams,        
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TBool aLoadChildren = EFalse,
        TNcdChildLoadMode aMode = ELoadStructure,
        TBool aIsSubOperation = EFalse,
        TBool aCreateParent = ETrue );
        
    /**
     * Overloaded constructor for loading from a content source.
     * Sets iIsSubOperation to ETrue.
     */
    static CNcdLoadNodeOperationImpl* NewLC(
        CNcdContentSource& aContentSource,
        CNcdContentSourceMap* aContentSourceMap,
        const CNcdNodeIdentifier& aParentIdentifier,        
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,        
        MCatalogsSession& aSession );
        
    virtual ~CNcdLoadNodeOperationImpl();
    
    TLoadNodeOperationState State();
    
    /**
     * Getter for loaded nodes.
     * Intended for parent op use during a progress callback.
     * The array contents will be removed after the progress calback.
     *
     * @return An array of nodes that have been loaded by this op.
     */        
    const RPointerArray<CNcdNodeIdentifier>& LoadedNodes();
    
    /**
     * Returns the identifier of the parent node this operation is loading,
     * if available.
     *
     * @return Identifier of the parent node or NULL.
     */
    const CNcdNodeIdentifier* NodeIdentifier() const;

public: // from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Start
     */
    virtual TInt Start();

    /**
     * @see CNcdBaseOperation::Cancel
     */
    virtual void Cancel();
    
    /**
     * @see CNcdBaseOperation::HandleCancelMessage
     */
    virtual void HandleCancelMessage( MCatalogsBaseMessage* aMessage );
    
    /**
     * @see CNcdBaseOperation::CompleteMessage         
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        TInt aStatus );
        
    /**
     * @see CNcdBaseOperation::CompleteMessage         
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        TInt aStatus );
        
    /**
     * @see CNcdBaseOperation::CompleteMessage         
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        RPointerArray<CNcdNodeIdentifier>& aNodes,
        TInt aStatus );
        
    /**
     * @see CNcdBaseOperation::CompleteMessage         
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes,
        TInt aStatus );
        

// from base class MCatalogsHttpObserver

    /**     
     * @see MCatalogsHttpObserver::HandleHttpEventL
     */
    virtual void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );
        
    /**
     * @see MCatalogsHttpObserver::HandleHttpError()
     */
    TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );
        
public: // from MNcdParserObserver

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseCompleteL( TInt aError );

public: // from  MNcdParserEntityObserver

    /**
     * @see MNcdParserEntityObserver
     */
    virtual void FolderRefL( MNcdPreminetProtocolFolderRef* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void FolderDataL( MNcdPreminetProtocolDataEntity* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void ItemRefL( MNcdPreminetProtocolItemRef* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void ItemDataL( MNcdPreminetProtocolDataEntity* aData );
    
public: // from MNcdOperationObserver
    
    /**
     * @see MNcdOperationObserver
     */
    virtual void Progress( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationObserver
     */
    virtual void QueryReceived( CNcdBaseOperation& aOperation,
                                CNcdQuery* aQuery );

    /**
     * @see MNcdOperationObserver
     */
    virtual void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError );

public: // from MNcdParserDataBlocksObserver

    /**
     * @see MNcdDataBlocksObserver
     */
    virtual void DataBlocksL( 
        CArrayPtr<MNcdPreminetProtocolDataBlock>* aData );

public: // from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation
     */
    virtual TBool QueryCompletedL( CNcdQuery* aQuery );
    
public: // from MNcdParserErrorObserver

    virtual void ErrorL( MNcdPreminetProtocolError* aData );

    
protected:

    CNcdLoadNodeOperationImpl( 
        CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
        TNcdResponseFilterParams aFilterParams,
        TNcdChildLoadMode aMode,
        TBool aLoadChildren,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TBool aIsSubOperation,
        TBool aCreateParent );
        
    CNcdLoadNodeOperationImpl( 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession  );
    
    void ConstructL( const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier );
        
    void ConstructL( CNcdContentSource& aContentSource,
        CNcdContentSourceMap* aContentSourceMap,
        const CNcdNodeIdentifier& aParentIdentifier );
        
    
    
    virtual HBufC8* CreateRequestLC( CNcdNodeIdentifier* aNodeIdentifier,
        TNcdResponseFilterParams aFilterParams,
        const TDesC& aUri );

    /**
     * Maps the icon ID for data block ID if the icon data is in a separate
     * data block.
     *
     * @param aIconId The icon Id.
     * @param aDataBlockId The data block Id containing the icon data.
     * @param aNodeIdenfier The node which has the icon.
     */
    void MapIconIdForDataBlockL(
        const TDesC& aIconId, const TDesC& aDataBlockId,
        const CNcdNodeIdentifier& aNodeIdentifier);
        
    /**
     * Used to request the ID of the icon of which data is in the given
     * data block.
     *
     * @note The responsibility of closing the returned array is in the caller.
     * @param aDataBlockId ID of the data block
     * @param aNodeIdentifier On return the node which has the icon is
     *                        assigned to this pointer.
     * @return The icon ID or NULL of icon id was not found.
     */
    RPointerArray<CNcdLoadNodeOperationImpl::CNcdNodeIconMap> IconsForDataBlockL(
        const TDesC& aDataBlockId);
    
    /**
     * Creates sub load-operations for remote folders.
     *
     */    
    virtual void CreateSubOperationsL();
    
    /**
     * Checks whether loading is really necessary.
     * This method is used to determine whether to complete
     * the op early on before doing anything else. 
     * There are many cases where it's impossible or unwise to complete
     * the operation normally e.g. load started for search root.
     */    
    virtual TBool IsLoadingNecessaryL();
    
    /**
     * Checks whether a folder's children should be cleared
     */
    virtual TBool IsChildClearingNecessaryL();
     

    /**
     * Determines the type of the parent node
     */
    void DetermineParentTypeL( const TUid& aUid );
    
    /**
     * Removes the session of current namespace & server uri.
     */
    void RemoveServerSessionL();
     
    /**
     * Sets the children loaded flag for the folder (or folders)
     * that were loaded (the flags intention is to differentiate first time
     * load from subsequent ones). 
     *
     * @see CNcdNodeFolder::SetChildrenPreviouslyLoaded
     */
    void SetChildrenLoadedFlagL();
    
    /**
     * Refreshesh seen status if needed.
     *
     * @see CNcdNodeSeenInfo::RefreshFolderSeenStatusL
     */
    void RefreshSeenStatusL();
    
    /**
     * Calculates the correct structure page size for the given page
     * of the given folder.
     *
     * @param aPageStart Start index of the page.
     * @param aPageSize Original page size.
     * @param aNodeFolder The folder.
     * @param aChildLoadMode Load mode.
     */
    TInt CalculateStructPageSize(
        TInt aPageStart,
        TInt aPageSize,
        CNcdNodeFolder& aNodeFolder,
        TNcdChildLoadMode aChildLoadMode );
        
    
    /**
     * Returns the amount of remote folders that need to be loaded
     * separately as sub-operations.
     *
     * @return Remote folder count.
     */
    
    virtual TInt RemoteFolderCount() const;
    
    /**
     * Removes folder's children
     */
    virtual void RemoveChildrenL( CNcdNodeFolder& aFolder );
    
    /**
     * Notifies the operation manager about completion of a queued operation,
     * if the given completion id indicates that the operation is really completed.
     *
     * @param aId The completion id.
     */
    void NotifyCompletionOfQueuedOperation( TNcdOperationMessageCompletionId aId );    
    
    
public: // from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual TInt RunOperation();
    
    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual void ChangeToPreviousStateL();
    
protected:

    void DoRunOperationL();    
    
protected: // data
    
    /**
     * A sub-state of this operation's execution.
     */
    TLoadNodeOperationState iLoadNodeState;
    
    /**
     * Nodes that have been loaded. This is used to store loaded
     * nodes identifiers before they can be sent to the proxy in a message.
     */
    RPointerArray<CNcdNodeIdentifier> iLoadedNodes;
        
    /**
     * Parent node identifier.
     */
    CNcdNodeIdentifier* iParentIdentifier;
    
    /**
     * Identifier of the node this op is loading.
     */
    CNcdNodeIdentifier* iNodeIdentifier;
    
    /**
     * Reference to access point manager.
     * Not owned by this.
     */
    MCatalogsAccessPointManager& iAccessPointManager;
    
    MNcdProtocol& iProtocol;
    
    /**
     * Http session for sending requests and receiving responses.
     * Not own.
     */
    MCatalogsHttpSession& iHttpSession;
    
    /**
     * Http operation for current transaction.
     * Not own.
     */
    MCatalogsHttpOperation* iTransaction;
        
    TNcdResponseFilterParams iFilterParams;
    
    CNcdQuery* iLoadNodeQuery;
    
    /**
     * Sub-operations for browse requests.
     */
    RPointerArray<CNcdLoadNodeOperationImpl> iSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iFailedSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iCompletedSubOps;
    
    HBufC* iServerUri;
    
    RPointerArray<CNcdQuery> iSubOpQuerys;
    
    /**
     * Remote folders encountered during the operation are stored here.
     * Load operations will be started for these nodes.
     */
    RPointerArray<CNcdNodeIdentifier> iRemoteFolders;
    
    /**
     * Child folders of transparent folders encountered during the
     * operation are stored here. Load operations will be started
     * from these nodes.
     */
    RPointerArray<CNcdNodeIdentifier> iTransparentChildFolders;
    
    /**
     * Child items of transparent folders.
     */
    RPointerArray<CNcdNodeIdentifier> iTransparentChildItems;
    
    TNcdChildLoadMode iChildLoadMode;
    TLoadMode iLoadMode;
    
    /**
     * Not own.
     */     
    CNcdContentSource* iContentSource;
    
    /**
     * Not own.
     */
    CNcdContentSourceMap* iContentSourceMap;
    
    TUid iClientUid;
    
    /**
     * Node index, this is used to put a child node to the
     * correct place in the parent node.
     */
    TInt iNodeIndex;
    
    /**
     * These are used if icon data is in separate data block.
     */
    RPointerArray<CNcdNodeIconMap> iNodeIconMaps;
    
    /**
     * Type of the parent node: root or folder
     */
    CNcdNodeFactory::TNcdNodeType iParentType;
    
    /** 
     * Purpose of the parent node.
     */
    CNcdNodeFactory::TNcdNodePurpose iParentPurpose;
    
    /**
     * Indicates whether the parent node must be created.
     */
    TBool iCreateParent;
    
    /**
     * Operation queue, not own.
     */
    MNcdOperationQueue* iOperationQueue;    
    
    /**
     * ETrue if node updates are held until the entire response
     * has been handled.
     *
     * This is used to ensure that datablocks have been received
     * before nodes are updated to the proxy-side
     */
    TBool iHoldNodeUpdates;
    
    class CNcdNodeIconMap : public CBase
        {
    public:
        static CNcdNodeIconMap* NewLC(
            const CNcdNodeIdentifier& aMetadataId,
            const TDesC& aIconId,
            const TDesC& aDataBlockId );
        ~CNcdNodeIconMap();

        HBufC* iDataBlockId;
        HBufC* iIconId;
        CNcdNodeIdentifier* iMetadataId;
        
    private:
        void ConstructL(
            const CNcdNodeIdentifier& aMetadataId,
            const TDesC& aIconId,
            const TDesC& aDataBlockId );
        CNcdNodeIconMap();                        
        };
    };

#endif // C_NCDLOADNODEOPERATIONIMPL_H
