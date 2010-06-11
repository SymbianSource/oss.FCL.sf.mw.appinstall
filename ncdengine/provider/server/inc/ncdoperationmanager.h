/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdOperationManager class
*
*/


#ifndef NCD_OPERATION_MANAGER_H
#define NCD_OPERATION_MANAGER_H


#include <e32base.h>
#include <e32cmn.h>

#include "catalogscommunicable.h"
#include "ncdoperationremovehandler.h"
#include "ncdoperationdatatypes.h"
#include "ncdoperationqueue.h"

class CNcdProvider;
class CNcdBaseOperation;
class MNcdStorageManager;
class MNcdProtocol;
class MCatalogsHttpSession;
class MCatalogsSmsSession;
class CNcdNodeManager;
class CNcdPurchaseHistoryDb;
class MNcdConfigurationManager;
class MCatalogsContext;
class MCatalogsAccessPointManager;
class MNcdStorage;
class CNcdSubscriptionManager;
class CNcdNodeIdentifier;
class CNcdReportManager;
class CNcdGeneralManager;

/**
 *  CNodeManager provides functions to ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdOperationManager : public CCatalogsCommunicable, 
                             public MNcdOperationRemoveHandler,
                             public MNcdOperationQueue
    {

public:

    /**
     * NewL
     *
     * @param aStorageManager Storage manager
     * @return CNcdOperationManager* Pointer to the created object 
     * of this class.
     */
    static CNcdOperationManager* NewL(
        CNcdProvider& aProvider,
        CNcdGeneralManager& aGeneralManager,        
        CNcdSubscriptionManager& aSubscriptionManager );



    /**
     * Destructor
     *
     * Deletes the nodes from the node cache.
     */
    virtual ~CNcdOperationManager();


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateLoadNodeOperationRequestL(
        MCatalogsBaseMessage& aMessage );
        
    /**
     * @leave
     * @param 
     * @return 
     */
    //virtual void CreateLoadNodeChildrenOperationRequestL(
    //    MCatalogsBaseMessage& aMessage );
    
    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateLoadRootNodeOperationRequestL(
        MCatalogsBaseMessage& aMessage );

    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateLoadBundleNodeOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateDownloadOperationRequestL(
        MCatalogsBaseMessage& aMessage );
    

    /**
     * @leave
     * @param 
     * @return 
     */
    void CreatePurchaseOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateInstallOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateSilentInstallOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateRightsObjectOperationRequestL(
        MCatalogsBaseMessage& aMessage );

    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateSubscriptionOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    
    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateCreateAccessPointOperationRequestL(
        MCatalogsBaseMessage& aMessage );
    


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateSendHttpRequestOperationRequestL(
        MCatalogsBaseMessage& aMessage );
    

    /**
     * @leave
     * @param 
     * @return 
     */
    void RestoreDownloadOperationsRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * @leave
     * @param 
     * @return 
     */
    void CreateServerReportOperationRequestL(
        MCatalogsBaseMessage& aMessage );


    CNcdReportManager& ReportManagerL( MCatalogsContext& aContext );


public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    void CounterPartLost( const MCatalogsSession& aSession );


public: // MNcdOperationRemoveHandler

    /**
     * @see MNcdOperationRemoveHandler::RemoveOperation()
     */
    void RemoveOperation( CNcdBaseOperation& aOperation );
    
public: // MNcdOperationQueue

    /**
     * @see MNcdOperationQueue
     */     
    void QueueOperationL( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationQueue
     */
    void QueuedOperationComplete( CNcdBaseOperation& aOperation );
        

protected:

    /**
     * Constructor
     */
    CNcdOperationManager(
        CNcdProvider& aProvider,
        CNcdGeneralManager& aGeneralManager,        
        CNcdSubscriptionManager& aSubscriptionManager );

    /**
     * ConstructL
     */
    void ConstructL();


private:

    // Prevent if not implemented
    CNcdOperationManager( const CNcdOperationManager& aObject );
    CNcdOperationManager& operator =( const CNcdOperationManager& aObject );


private:

    MCatalogsHttpSession& HttpSessionL( MCatalogsContext& aContext );
    MCatalogsSmsSession& SmsSessionL( MCatalogsContext& aContext );


    MNcdStorage* StorageL( const TDesC& aClientUid,
        const TDesC& aNamespace ) const;
        
    /**
     * This function is called from the proxy side when the proxy
     * is deleted.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


    /**
     * Checks if a download matching the criteria already exists
     */
    TBool DownloadExistsL( MCatalogsBaseMessage& aMessage, 
        const CNcdNodeIdentifier& aIdentifier, 
        TNcdDownloadDataType aType, 
        TInt aIndex );

    /**
     * Checks if there are load bundle node operations
     * in operation queue.
     */        
    TBool QueuedLoadBundleOperationsExists() const;
    
    /**
     * Checks that is there an operation running for the same metadata for another client.
     *
     * @note If aNodeIdentifier identifies some root node, aCompareIdsDirectly value is
     * forced to be ETrue inside this function.
     *
     * @param aNodeIdentifier Id to check for.
     * @param aContext Context of the client requesting an operation.
     * @param aCompareIdsDirectly If true, then the aNodeIdentifier is compared directly
     * with operations' identifiers. This is needed for cases where the operations' identifiers don't
     * contain a proper node identifier (generic file download).
     * @return ETrue if another op is running for the metadata.
     */    
    TBool ParallelOperationExistsForMetadataL( const CNcdNodeIdentifier& aNodeIdentifier,
        const MCatalogsContext& aContext,
        TBool aCompareIdsDirectly = EFalse ) const;

    /**
     * Retrieves node id from operation, if possible.
     *
     * @param aOperation Operation to get id from.
     * @return Id or NULL.
     */
    const CNcdNodeIdentifier* GetNodeIdFromOperation( const CNcdBaseOperation& aOperation ) const;
        
private: // data

    CNcdProvider&       iProvider;

    CNcdGeneralManager& iGeneralManager;
    
    // Storage manager
    MNcdStorageManager& iStorageManager;    
    
    // Protocol handler manages protocol sessions and provides access
    // to response parser and request processing
    MNcdProtocol& iProtocolHandler;
        
    CNcdNodeManager& iNodeManager;
    
    // Purchase history.
    CNcdPurchaseHistoryDb& iPurchaseHistory;

    MNcdConfigurationManager& iConfigurationManager;
    
    MCatalogsAccessPointManager& iAccessPointManager;

    CNcdSubscriptionManager& iSubscriptionManager;

    // This array contains all the operations that have been created.
    RPointerArray< CNcdBaseOperation > iOperationCache;
    
    // This is an array of queued operations. Only one load
    // bundle node operation can be running.
    RPointerArray< CNcdBaseOperation > iOperationQueue;
    };


#endif // NCD_OPERATION_MANAGER_H
