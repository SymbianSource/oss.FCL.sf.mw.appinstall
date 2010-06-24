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
* Description:   Contains CNcdOperationManagerProxy class
*
*/


#ifndef NCD_OPERATION_MANAGER_PROXY_H
#define NCD_OPERATION_MANAGER_PROXY_H


#include <e32base.h>
#include <e32cmn.h>

#include "ncdbaseproxy.h"
#include "ncdoperationproxyremovehandler.h"
#include "ncdnodefunctionids.h"
#include "ncdoperationdatatypes.h"
#include "ncdchildloadmode.h"
#include "ncdsearchfilter.h"
#include "ncdsubscriptionoperation.h"
#include "ncddownloadoperationobserver.h"

class MNcdOperation;
class CNcdLoadNodeOperationProxy;
class CNcdNodeProxy;
class CNcdDownloadOperationProxy;
class CNcdFileDownloadOperationProxy;
class MNcdDownloadOperationObserver;
class MNcdFileDownloadOperationObserver;
class CNcdPurchaseOperationProxy;
class MNcdPurchaseOperationObserver;
class CNcdNodeManagerProxy;
class CNcdNodeIdentifier;
class CNcdPurchaseOptionProxy;
class CNcdInstallOperationProxy;
class CNcdSilentInstallOperationProxy;
class MNcdInstallOperationObserver;
class CNcdRightsObjectOperationProxy;
class MNcdRightsObjectOperationObserver;
class CNcdSubscriptionOperationProxy;
class MNcdSubscriptionOperationObserver;
class MNcdInstallationService;
class MNcdClientLocalizer;
class MNcdCreateAccessPointOperationObserver;
class CNcdCreateAccessPointOperationProxy;
class MNcdSendHttpRequestOperationObserver;
class CNcdSendHttpRequestOperationProxy;
class TNcdConnectionMethod;
class CNcdServerReportOperationProxy;
class MNcdServerReportOperationObserver;


// Forward declaration for silent install
// Usif::TInstallOptions
namespace Usif
    {
    class COpaqueNamedParams;    
    }

/**
 *  CNodeManagerProxy provides functions to ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdOperationManagerProxy : public CNcdBaseProxy,
                                  public MNcdOperationProxyRemoveHandler,
                                  public MNcdDownloadOperationObserver
    {

public:

    /**
     * NewL
     *
     * @return CNcdOperationManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdOperationManagerProxy* NewL( MCatalogsClientServer& aSession, 
                                            TInt aHandle );

    /**
     * NewLC
     *
     * @return CNcdOperationManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdOperationManagerProxy* NewLC( MCatalogsClientServer& aSession, 
                                             TInt aHandle );


    /**
     * Destructor
     *
     * Deletes the nodes from the node cache.
     */
    virtual ~CNcdOperationManagerProxy();

    /**
     * Sets the localizer object.
     *
     * @param aLocalizer The localizer.
     */   
    void SetClientLocalizer( MNcdClientLocalizer& aLocalizer );

    
    /**
     * Creates a load node children operation.
     *
     * @leave 
     * @param aNode The node to be loaded.
     * @return Load node operation.
     */
    virtual CNcdLoadNodeOperationProxy* CreateLoadNodeOperationL(
        CNcdNodeProxy& aNode, TBool aLoadChildren = EFalse, TInt aPageSize = 0,
        TInt aPageStart = 0, TInt aDepth = 0,
        TNcdChildLoadMode aMode = ELoadStructure,
        MNcdSearchFilter* aSearchFilter = NULL );
        
    /**
     * Creates a load root node operation.
     *
     * @leave 
     * @param aNode The node to be loaded.
     * @return Load node operation.
     */
    virtual CNcdLoadNodeOperationProxy* CreateLoadRootNodeOperationL(
        CNcdNodeProxy& aNode );
    
    /**
     * Creates a load bundle node operation.
     *
     * @leave
     * @param aNode The node to be loaded.
     * @return Load node operation.
     */
    virtual CNcdLoadNodeOperationProxy* CreateLoadBundleNodeOperationL(
        CNcdNodeProxy& aNode );
        
    /**
     * Creates download operation
     *
     * @param aType Type of the download
     * @param aNode Proxy of the node that provides information for the operation
     * @param aDataId Identifier for the data the download is downloading.
     * @param aObserver Observer
     * @param aDownloadIndex Index of the item that will be downloaded
     * @return A download proxy. Returns an existing download if type and
     * data ID match with an existing download,
     *
     * Data ID is used only on the proxyside for comparing downloads. 
     */
    virtual CNcdDownloadOperationProxy* CreateDownloadOperationL( 
        TNcdDownloadDataType aType, 
        CNcdNodeProxy& aNode, 
        const TDesC& aDataId,
        MNcdDownloadOperationObserver* aObserver,
        TInt aDownloadIndex = 0 );
        
    /**
     * Creates a file download operation
     *
     * @param aType Type of the download
     * @param aNode Node
     * @param aTargetPath Download target
     * @return a new file download
     */
    virtual CNcdFileDownloadOperationProxy* CreateFileDownloadOperationL( 
        TNcdDownloadDataType aType, 
        CNcdNodeProxy& aNode, 
        MNcdFileDownloadOperationObserver* aObserver,
        const TDesC& aTargetPath );


    /**
     * Creates a generic file download operation
     *
     * @param aUri Source URI
     * @param aTargetPath Download target
     * @return a new file download
     */
    virtual CNcdFileDownloadOperationProxy* CreateFileDownloadOperationL( 
        TNcdDownloadDataType aType, 
        const TDesC& aUri, 
        const TDesC& aTargetPath, 
        MNcdFileDownloadOperationObserver* aObserver );


    /**
     * Creates a purchase operation
     * 
     * @param 
     * @return
     */
    virtual CNcdPurchaseOperationProxy* CreatePurchaseOperationL(
        CNcdNodeProxy& aNode,
        CNcdPurchaseOptionProxy& aSelectedPurchaseOption,
        MNcdPurchaseOperationObserver* aObserver );
    
    

    /**
     * Creates an install operation
     * 
     * @param 
     * @return
     */
    virtual CNcdInstallOperationProxy* CreateInstallOperationL(
        CNcdNodeProxy& aNode, 
        MNcdInstallOperationObserver* aObserver );    


    /**
     * Creates an install operation
     * 
     * @param 
     * @return
     */
    virtual CNcdSilentInstallOperationProxy* CreateSilentInstallOperationL(
        CNcdNodeProxy& aNode, 
        MNcdInstallOperationObserver* aObserverconst,
        Usif::COpaqueNamedParams* aInstallOptions );
    
    
    /**
     * Creates a DRM rights object download & install operation.
     * 
     * @param 
     * @return
     */
    virtual CNcdRightsObjectOperationProxy* CreateRightsObjectOperationL(
           const TDesC& aDownloadUri, 
           const TDesC& aMimeType, 
           const TNcdConnectionMethod& aConnectionMethod,
           MNcdRightsObjectOperationObserver& aObserver );

    /**
     * Creates a subscription refresh operation.
     * 
     * @param aObserver 
     * @return Subscription operation.
     */
    virtual CNcdSubscriptionOperationProxy*
        CreateSubscriptionRefreshOperationL(
            MNcdSubscriptionOperationObserver& aObserver );
            

    /**
     * Creates a subscription refresh operation.
     * 
     * @param aObserver 
     * @return Subscription operation.
     */
    virtual CNcdSubscriptionOperationProxy*
        CreateSubscriptionUnsubscribeOperationL(
            const TDesC& aPurchaseOptionId,
            const TDesC& aEntityId,
            const TDesC& aNamespace,
            const TDesC& aServerUri,
            MNcdSubscriptionOperationObserver& aObserver );


    /**
     * Creates a server report operation.
     * 
     * @param 
     * @return
     */
    virtual CNcdServerReportOperationProxy* 
        CreateServerReportOperationL(
            MNcdServerReportOperationObserver& aObserver );


    /**
     * Creates an access point creation operation
     * 
     * @param 
     * @return
     */
    CNcdCreateAccessPointOperationProxy* CreateCreateAccessPointOperationL(
           const TDesC& aAccessPointData, 
           MNcdCreateAccessPointOperationObserver& aObserver );


    /**
     * Creates a HTTP request sending operation
     * 
     * @param 
     * @return
     */
    CNcdSendHttpRequestOperationProxy* CreateSendHttpRequestOperationL(
       const TDesC8& aUri,
       const TDesC8& aRequest, 
       const TNcdConnectionMethod& aConnectionMethod,
       MNcdSendHttpRequestOperationObserver& aObserver );


    /**
     * Restores serialized content downloads
     *
     * The operations can be retrieved through Operations()
     */
    void RestoreContentDownloadsL();
    
    
    /**
     * Returns the ongoing operations.
     */
    const RPointerArray<MNcdOperation>& Operations() const;


    /**
     * @param aNodeManager Ownership is not transferred.
     */ 
    void SetNodeManager( CNcdNodeManagerProxy* aNodeManager );

    /**
     * @return CNcdNodeManagerProxy* Ownership is not transferred.
     */ 
    CNcdNodeManagerProxy* NodeManager();
    

    /**
     * Installation service getter
     *
     * @return Installation service
     */
    MNcdInstallationService& InstallationServiceL();


    /**
     * Cancels all operations
     *
     * @note Releases operations with 0 refcount
     */
    void CancelAllOperations();
    

    /**
     * Releases operations that had not been used by any
     * client.     
     */        
    void ReleaseUnusedOperations();
    
    
public: // MNcdOperationProxyRemoveHandler

    /**
     * @see MNcdOperationProxyRemoveHandler::RemoveOperationProxy
     */
    virtual void RemoveOperationProxy(
        CNcdBaseOperationProxy& aOperationProxy );

public: // MNcdDownloadOperationObserver


    /**
     * @see MNcdDownloadOperationObserver::DownloadProgress()
     */    
    virtual void DownloadProgress( MNcdDownloadOperation& aOperation,
        TNcdProgress aProgress );
        
    /**
     * @see MNcdDownloadOperationObserver::QueryReceived()
     */
    virtual void QueryReceived( MNcdDownloadOperation& aOperation,
        MNcdQuery* aQuery );

    /**
     * @see MNcdDownloadOperationObserver::OperationComplete()
     */
    virtual void OperationComplete( MNcdDownloadOperation& aOperation,
        TInt aError );


protected:

    /**
     * Constructor
     */
    CNcdOperationManagerProxy( MCatalogsClientServer& aSession, 
                               TInt aHandle );

    /**
     * ConstructL
     */
    virtual void ConstructL();


private:

    // Prevent if not implemented
    CNcdOperationManagerProxy( const CNcdOperationManagerProxy& aObject );
    CNcdOperationManagerProxy& operator =( const CNcdOperationManagerProxy& aObject );

private: // New methods

    /**
     * Creates an identification stream for the node
     *
     * @param aNode
     * @return Id stream
     */
    HBufC8* CreateNodeIdentifierDataLC( CNcdNodeProxy& aNode );

    

    /**
     * Creates an operation on the server side
     *
     * @param aNode Node used by the operation
     * @param aFunction Operation manager function used to create the operation
     * @param aRemoveHandler Remove handler is assigned to this
     * @return Operation handle
     */
    TInt CreateOperationL( CNcdNodeProxy& aNode,
        NcdNodeFunctionIds::TNcdOperationManagerFunctionNumber aFunction, 
        MNcdOperationProxyRemoveHandler*& aRemoveHandler );


    /**
     * Creates an operation on the server side
     *
     * @param aNode Node used by the operation
     * @param aSelectedPurchaseOption Selected purchase option
     * @param aFunction Operation manager function used to create the operation
     * @param aRemoveHandler Remove handler is assigned to this
     * @return Operation handle
     */
    TInt CreateServerSidePurchaseOperationL( CNcdNodeProxy& aNode,
        const CNcdPurchaseOptionProxy& aSelectedPurchaseOption,
        NcdNodeFunctionIds::TNcdOperationManagerFunctionNumber aFunction, 
        MNcdOperationProxyRemoveHandler*& aRemoveHandler );
    


    // Create a download operation on the server side
    TInt CreateServerSideDownloadOperationL( 
        TNcdDownloadDataType aType, const CNcdNodeIdentifier& aNodeId,
        TInt aDownloadIndex );

    // Searches for an existing download operation
    CNcdDownloadOperationProxy* FindExistingDownloadOperation(
        TInt aHandle ) const;

    
private: // data

    // This array contains all the operations that have been created.
    // When the operation is finished its job it should inform
    // this manager that it is going to delete itself and delete itself.
    RPointerArray<MNcdOperation> iOperationCache;
    
    TBool iRemovingAllOperations;
    
    CNcdNodeManagerProxy* iNodeManager;
    
    /**
     * Client localizer, not own.
     */
    MNcdClientLocalizer* iClientLocalizer;
    
    /**
     * Installation service, own
     */
    MNcdInstallationService* iInstallationService;
    
    };


#endif // NCD_OPERATION_MANAGER_PROXY_H
