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
* Description:   Contains MNcdProvider interface
*
*/


#ifndef M_NCD_PROVIDER_H
#define M_NCD_PROVIDER_H


#include <e32cmn.h>
#include "catalogsbase.h"
#include "catalogsarray.h"
#include "ncdinterfaceids.h"
#include "ncdschemenodetype.h"

class MNcdNode;
class MNcdNodeContainer;
class MNcdProviderObserver;
class MNcdOperation;
class CNcdKeyValuePair;
class MNcdPurchaseHistory;
class MNcdSubscriptionManager;
class MNcdKeyValuePair;
class MNcdClientLocalizer;
class MNcdFileDownloadOperation;
class MNcdFileDownloadOperationObserver;
class MNcdPurchaseDetails;
class TNcdConnectionMethod;

/**
 *  MNcdProvider provides functions to NCD specific functionality, such as
 *  purchase history access, subscription management, search, observing major
 *  events etc.
 *
 *  
 */
class MNcdProvider : public virtual MCatalogsBase

    {
public:


    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdProviderUid };        


    /**
     * Set the provider observer interface.
     *
     * @note NCD provider client should always set an observer to get callbacks.
     *
     * 
     * @param aObserver Observer interface to receive NCD provider callbacks. If NULL,
     *  no callbacks will be made.
     */
    virtual void SetObserver( MNcdProviderObserver* aObserver ) = 0;


    /**
     * Returns the root node for the provider. Root node provides access to
     * node hierarchy.
     *
     * 
     * @return Pointer to root node object. Counted, Release() must be
     *  called after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdNode* RootNodeL() = 0;


    /**
     * Returns a list of currently active operations for progress monitoring and
     * operation management.
     * The returned operation objects have their reference counts incremented, the
     * items must be released after use e.g. with a call to the array's ResetAndDestroy().
     *
     * @note Lifetime of the operation objects cannot exceed the lifetime of the
     *  provider object. If there are unreleased references to a provider's operation
     *  objects when the last reference to the provider object is released, a panic
     *  will be raised.
     *
     * 
     * @return List of operation object pointers.
     * @exception Leave System wide error code.
     * @see MNcdOperation
     */
    virtual RCatalogsArray< MNcdOperation > OperationsL() const = 0;


    /**
     * Returns a handle to purchase history. Purchased, downloaded and installed
     * content can be accessed via the purchase history.
     *
     * 
     * @return Pointer to purchase history object. Counted, Release() must be called
     *  after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdPurchaseHistory* PurchaseHistoryL() const = 0;


    /**
     * Returns a pointer to subscription manager object.
     *
     * 
     * @return Pointer to subscription manager. Counted, Release() must be called
     *  after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdSubscriptionManager* SubscriptionsL() const = 0;


    /**
     * Adds a configuration that will be sent to content providers.
     *
     * Only one value per key is accepted. If the key already exists, it's old value
     * is replaced with the new one. The only exception are keys that match 
     * NcdConfigurationKeys::KCapability. The number of capability keys is not limited,
     * but their (folded) values are ensured to be unique.
     *
     * 
     * @param aConfiguration Configuration to be added.
     * @see MNcdProvider::RemoveConfigurationL
     * @see NcdConfigurationKeys
     * @exception Leave System wide error code.
     */
    virtual void AddConfigurationL( const MNcdKeyValuePair& aConfiguration ) = 0;


    /**
     * Removes a configuration. This configuration will no longer be sent to 
     * content providers.
     *
     * 
     * @param aKey Key of the configuration to be removed.
     * @exception KErrNotFound The configuration is not present.
     * @exception KErrAccessDenied The configuration has been added by the root-node
     *            provider and can not be removed by the user.
     * @see MNcdProvider::AddConfigurationL
     */
    virtual void RemoveConfigurationL( const TDesC& aKey ) = 0;


    /**
     * Returns a list of configurations that are used when communicating with
     * content providers. 
     *
     * 
     * @return Current configuration as an array of key-value pairs. Ownership
     *  of the objects within the array is transferred to the caller, they must
     *  be deleted after use e.g. calling RPointerArray::ResetAndDestroy().
     * @see MNcdProvider::AddConfigurationL
     * @see MNcdProvider::RemoveConfigurationL
     * @exception KErrNotFound if no configurations have been set
     * @exception Leave System wide error code.
     */
    virtual RPointerArray< CNcdKeyValuePair > ConfigurationsL() const = 0;


    /**
     * Retrieves a node identified by a namespace Id and a node Id.
     *
     * 
     * @param aNameSpaceId  Namespace of the requested node. Node Ids are unique
     *                      inside a specific namespace.
     * @param aNodeId       Id of the requested node.
     *
     * @return Pointer to the requested node object. Note, that the node may be
     *  in an uninitialized state, or in an initialized state, depending on
     *  whether all the required node information has previously been loaded 
     *  from the server.
     *  Counted, Release() must be called after use.
     * @exception Leave KErrNotFound if the node is not found from the RAM or database
     *  cache.
     * @exception Leave System wide error code.
     */
    virtual MNcdNode* NodeL( const TDesC& aNameSpaceId, 
                             const TDesC& aNodeId ) const = 0;


    /**
     * Retrieves a node identified by purchase history details
     *
     * 
     * @param aDetails  Purchase details
     *
     * @return Pointer to the requested node object. Note, that the returned node
     *  is not located in the normal browsing hierarchy but it is just hanging by
     *  itself as a temporary node. If the metadata that corresponds the purchase details 
     *  is found from the RAM or database cache, that metadata will be used for the node. 
     *  If the metadata can not be found, then it will be initialized with the values 
     *  provided by the corresponding purchase details.
     *  Counted, Release() must be called after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdNode* NodeL( const MNcdPurchaseDetails& aDetails ) const = 0;

    /**
     * Retrieves a scheme node identified by a namespace Id, a node metadata Id and
     * a server URI. If the scheme node is created, it will not have any parent. The scheme
     * node is automatically set as favourite node.
     *
     * 
     * @param aNameSpaceId  Namespace of the requested node. Node Ids are unique
     *                      inside a specific namespace.
     * @param aEntityId     Id of the requested node metadata.
     * @param aServerUri    Server URI of the requested node.
     * @param aType         Type of the scheme node.
     * @param aRemoveOnDisconnect If ETrue, the scheme node is removed from favourites when
     *                      the client-server session is closed.
     * @param aForceCreate  ETrue means that, if necessary, a scheme node will always 
     *                      be created. Notice, that an uninitialized node object will be 
     *                      returned for an invalid name space or node metadata id.
     *                      EFalse means that the scheme node will be returned 
     *                      only if the node itself or the metadata for it already
     *                      existed in the RAM or database cache. 
     *
     * @return Pointer to the requested node object. Note, that the node may be
     *  in an uninitialized state, or in an initialized state, depending on
     *  whether the node has previously been known to the engine.
     *  Counted, Release() must be called after use.
     * @exception Leave KErrNotFound if aForceCreate parameter has been set as EFalse 
     *  and the metadata is not found from the RAM or database cache.
     * @exception Leave KErrArgument if the metadata already exists but is of different
     *  type than the given scheme node type.
     * @exception Leave System wide error code.
     */
    virtual MNcdNode* SchemeNodeL( const TDesC& aNameSpaceId,
                                   const TDesC& aEntityId,
                                   const TDesC& aServerUri,
                                   TNcdSchemeNodeType aType,
                                   TBool aRemoveOnDisconnect,
                                   TBool aForceCreate ) const = 0;

    /**
     * Sets the client string localizer interface to be used for server-initiated
     * localizable string information.
     *
     * 
     * @param aLocalizer Localizer to be used for translating localization keys
     *  into user viewable strings.
     */
    virtual void SetStringLocalizer( MNcdClientLocalizer& aLocalizer ) = 0;


    /**
     * Downloads a file from the given URI. The file is not handled by the engine, 
     * the file should be handled by the user of this interface as it sees fit.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release function of the operation should be called when the operation 
     * is not needed anymore.
     *
     * @note The file will be moved to the target location after it has been 
     * fully downloaded. Any existing file with the same name will be overwritten.
     *
     * 
     * @param aUri Source URI
     * @param aTargetFileName Full path and name for the target file.
     * @param aObserver
     */
    virtual MNcdFileDownloadOperation* DownloadFileL( const TDesC& aUri,
        const TDesC& aTargetFileName,
        MNcdFileDownloadOperationObserver& aObserver ) = 0;
        

    /**
     * Sets the default connection method used for network access.
     *
     * @note Not stored persistently. A client needs to set this again every time it
     *  creates the provider.
     *
     * 
     * @param aMethod Identifier of the connection method to use by default.
     */    
    virtual void SetDefaultConnectionMethodL( 
        const TNcdConnectionMethod& aMethod ) = 0;
    
    /**
     * Clears all search results. This method should be called before starting a new
     * search. 
     *
     * @note This method only marks the search result nodes for removal and makes them
     * unavailable, therefore it doesn't take a long time to complete. 
     * @note Previous result nodes need to be released to remove them from cache.
     */
    virtual void ClearSearchResultsL() = 0;
    
    
    /**
     * Clears the entire client cache (nodes, icons, previews)
     *
     * @note The client should release all nodes and close all open files before 
     * calling this method.
     */
    virtual void ClearCacheL( TRequestStatus& aStatus ) = 0;
    
    
    /**
     * Tells whether the SIM was changed or removed since the previous startup.
     *
     * @return ETrue If the SIM was changed or removed, otherwise EFalse.
     */
    virtual TBool IsSimChangedL() = 0;
    
    
    /**
     * Tells whether fixed access point defined in engine's configuration file is used.
     * In this case AP selection possibility may need to be disabled in UI.
     *
     * @return ETrue If fixed AP is used, otherwise EFalse.
     * @leave System wide error code.
     */
    virtual TBool IsFixedApL() = 0;

protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdProvider() {}

    };


#endif // M_NCD_PROVIDER_H
