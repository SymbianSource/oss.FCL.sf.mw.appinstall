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


#ifndef C_NCDLOADBUNDLENODEOPERATIONIMPL_H
#define C_NCDLOADBUNDLENODEOPERATIONIMPL_H

#include <e32base.h>
#include <badesca.h>
#include "ncdbaseoperation.h"
#include "ncdconfigurationobserver.h"

class MCatalogsAccessPointManager;
class MNcdProtocol;
class MNcdConfigurationManager;
class CNcdContentSource;
class CNcdNodeIdentifier;
class CNcdLoadNodeOperationImpl;
class CNcdContentSourceMap;
class CNcdChildEntityMap;
class MNcdOperationQueue;
class CNcdNodeMetaData;

/**
 *  Load node operation implementation.
 *
 *  Handles the "loading of root node" i.e. creates
 *  a root node from a conf protocol response.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdLoadBundleNodeOperation : public CNcdBaseOperation,
                                    public MNcdParserObserver,
                                    public MCatalogsHttpObserver,
                                    public MNcdParserConfigurationProtocolObserver,
                                    public MNcdConfigurationObserver
    {
    
public:

    /** Sub states of bundle node operation */
    enum TBundleNodeState
        {
        EConfRequest,      // create and send a conf request
        EReceiveConf,      // receive and parse conf data
        EConfQuery,        // respond to conf query
        EBrowseRequest,    // create sub-operations 
        EReceiveBrowse,    // wait for sub-operations to complete
        EComplete,         // everything done
        EFailed            // operation failed
        };
    
    
    static CNcdLoadBundleNodeOperation* NewL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue& aOperationQueue,
        MCatalogsSession& aSession );

    static CNcdLoadBundleNodeOperation* NewLC(
        const CNcdNodeIdentifier& aNodeIdentifier,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue& aOperationQueue,
        MCatalogsSession& aSession );

    virtual ~CNcdLoadBundleNodeOperation();
    
    /**
     * Returns the identifier of the parent node this operation is loading.
     *
     * @return Identifier of the parent node.
     */
    const CNcdNodeIdentifier& NodeIdentifier() const;

    
public: //from CNcdBaseOperation

    /**
     * Starts the operation.
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
    
    
public: // from MCatalogsHttpObserver

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
        
public: //from MNcdParserObserver

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseCompleteL( TInt aError );
    
public: // from MNcdParserConfigurationProtocolObserver

    virtual void ConfigurationBeginL( const TDesC& aVersion, 
                                      TInt aExpirationDelta );
    virtual void ConfigurationQueryL( MNcdConfigurationProtocolQuery* aQuery );

    virtual void ClientConfigurationL(
        MNcdConfigurationProtocolClientConfiguration* aConfiguration );
    virtual void ConfigurationDetailsL( 
        CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails );
    virtual void ConfigurationActionRequestL(
        MNcdConfigurationProtocolActionRequest* aActionRequest );
    virtual void ConfigurationErrorL( MNcdConfigurationProtocolError* aError );
    virtual void ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails );
    virtual void ConfigurationEndL();

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
                                    
public: // from MNcdConfigurationObserver

    virtual void ConfigurationChangedL();                                   
                                    
public: // from CCatalogsCommunicable
    
    /**
     * @see CCatalogsCommunicable
     */
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
        
public: // from MNcdParserErrorObserver

    virtual void ErrorL( MNcdPreminetProtocolError* aData );
                                    

protected:

    CNcdLoadBundleNodeOperation(
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue& aOperationQueue,
        MCatalogsSession& aSession );
    
    void ConstructL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    HBufC8* CreateConfRequestLC( CNcdQuery* aQuery = NULL );

    /**
     * Reverts the nodes which could not be retrieved due to broken content
     * source from NodeManager's temp cache to main RAM cache.
     */
    void RevertNodesOfBrokenSourcesToCacheL();
    
    /**
     * Reverts the given node belonging to given content source from
     * NodeManager's temp cache to the main cache.
     */
    void RevertNodeL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdContentSource& aContentSource );
    /**
     * Removes the nodes that were removed from content sources.
     */
    void RemoveOldNodesL();
    
    /**
     * Adds the bundle node identifier to iLoadedNodes.
     */
    void AddBundleToLoadedNodesL();
    
    /**
     * Updates the root node's content source map according to the new
     * content sources of the loaded bundle.
     */
    void UpdateCsMapToRootNodeL();
    
    /**
     * Sets always visible flags to the metadata of the nodes that belong to
     * content sources defined as always visible in client configuration.
     */
    void SetAlwaysVisibleFlagsL();
    
    void ParseCatalogBundleL( const MNcdConfigurationProtocolDetail& aDetail );
    
    /**
     * Notifies the operation manager about completion of a queued operation,
     * if the given completion id indicates that the operation is really completed.
     *
     * @param aId The completion id.
     */
    void NotifyCompletionOfQueuedOperation( TNcdOperationMessageCompletionId aId );    
    
    static TBool ContainsNode(
        const RPointerArray<CNcdNodeIdentifier>& aNodes,
        const CNcdNodeIdentifier& aNode);
    
    void HandleBundleDisclaimerL(
        CNcdNodeMetaData& aMetadata, 
        const TDesC& aDisclaimer );
    

protected:

    void HandleConfigurationDataRequestMessage( MCatalogsBaseMessage& aMessage );
    
    
protected: // from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual TInt RunOperation();
    
    void DoRunOperationL();
    
    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual void ChangeToPreviousStateL();
    
    virtual TBool QueryCompletedL( CNcdQuery* aQuery );
    
private: // type declarations
    
    /** Sub states of master server redirection process */
    enum TMasterServerRedirectionState 
        {
        EBegin, // Start state.
        ERedirecting, // Redirecting.
        EReverted // Returned to use previous master server address.
        };

private: // data
    /**
     * A sub-state of this operation's execution.
     */
    TBundleNodeState iBundleNodeState;
    
    /**
     * A sub-state of master server redirection.
     */
    TMasterServerRedirectionState iMasterServerRedirectionState;
    
    /**
     * Nodes that have been loaded. This is used to store loaded
     * nodes identifiers before they can be sent to the proxy in a message.
     */
    RPointerArray<CNcdNodeIdentifier> iLoadedNodes;
        
    /**
     * This flag is set to indicate that progress info should be sent
     * when the next message is received.
     */
    TBool iSendProgress;    
        
    /**
     * Access point manager for creating access points.
     */
    MCatalogsAccessPointManager& iAccessPointManager;
    

    /**
     * Http session for sending requests and receiving responses.
     * Not own.
     */
    MCatalogsHttpSession& iHttpSession;

    MNcdProtocol& iProtocol;
    
    /**
     * Configuration manager for getting the Master Server Address
     */
    MNcdConfigurationManager& iConfigManager;
        
    /**
     * Http operation for current transaction.
     * Not own.
     */
    MCatalogsHttpOperation* iTransaction;
        
    /**
     * Sub-operations for browse requests.
     */
    RPointerArray<CNcdLoadNodeOperationImpl> iSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iFailedSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iCompletedSubOps;
        
    /**
     * Content sources are stored here.
     */
    CNcdContentSourceMap* iContentSourceMap;
    
    /**
     * Identifier of the bundle node.
     */
    CNcdNodeIdentifier* iNodeIdentifier;
    
    CNcdQuery* iConfQuery;
    RPointerArray<CNcdQuery> iSubOpQuerys;
       
    MNcdParserConfigurationProtocolObserver*	iDefaultConfigurationProtocolObserver;
    
    HBufC* iServerUri;
    
    CBufFlat* iConfigResponseBuf;
        
    TBool iNodeDbLocked;
        
    /**
     * Maps of current children and their children for new checking.
     */
    RPointerArray<CNcdChildEntityMap> iChildEntityMaps;
    
    MNcdOperationQueue& iOperationQueue;
    
    TBool iFirstConfRequest;

    };

#endif // C_NCDLOADBUNDLENODEOPERATIONIMPL_H
