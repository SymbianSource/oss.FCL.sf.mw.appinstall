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
* Description:   Contains CNcdNodeMetadataProxy class
*
*/


#ifndef NCD_NODE_METADATA_PROXY_H
#define NCD_NODE_METADATA_PROXY_H


#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodemetadata.h"

class CNcdNodeProxy;
class CNcdNodeDisclaimerProxy;
class CNcdNodeDownloadProxy;
class CNcdNodeIconProxy;
class CNcdNodeScreenshotProxy;
class CNcdNodePurchaseProxy;
class CNcdNodeUriContentProxy;
class CNcdNodeContentInfoProxy;
class CNcdNodeInstallProxy;
class CNcdNodePreviewProxy;
class CNcdNodeActivate;
class CNcdNodeUpgradeProxy;
class CNcdNodeDependencyProxy;
class CNcdNodeUserDataProxy;
class CNcdNodeSkinProxy;
class CNcdNodeIdentifier;
class CNcdKeyValuePair;


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeMetadataProxy: public CNcdInterfaceBaseProxy, 
                             public MNcdNodeMetadata
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNode The node that owns this metadata.
     * @return CNcdNodeMetadataProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeMetadataProxy* NewL( MCatalogsClientServer& aSession, 
                                        TInt aHandle,
                                        CNcdNodeProxy& aNode );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNode The node that owns this metadata.
     * @return CNcdNodeMetadataProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeMetadataProxy* NewLC( MCatalogsClientServer& aSession, 
                                         TInt aHandle,
                                         CNcdNodeProxy& aNode );


    /**
     * Destructor
     * The destructor is set public. So, the node that owns
     * the metadata may delete its metadata directly
     * when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeMetadataProxy();

    /**
     * @return CNcdNodeIdentifier& Identifier of this metadata.
     */
    CNcdNodeIdentifier& Identifier() const;

    /**
     * @return CNcdNodeProxy& The parent node that owns this metadata. 
     */
    CNcdNodeProxy& Node() const;

    /**
     * @return CNcdNodeDownloadProxy* Download object if it exists. 
     * NULL if the download has not been set. Ownership is not transferred.
     */
    CNcdNodeDownloadProxy* Download() const;

    
    /** 
     * @return CNcdNodeInstallProxy* Install class object pointer.
     * NULL if the object does not exist. Ownership is not transferred.
     */
    CNcdNodeInstallProxy* Install() const;


    /**
     * @return The uri content object. 
     * NULL if the uri content has not been set for this metadata.
     * Ownership is not transferred.
     */
    CNcdNodeUriContentProxy* UriContent() const;

    /**
     * @return Content info object.
     * NULL if the content info has not been set for this metadata.
     * Ownerhsip is not transferred.
     */
    CNcdNodeContentInfoProxy* ContentInfo() const;            


    /**
     * Gets the data for descriptors from the server side.
     * This function is called to update proxy data.
     * The function uses the protected virtual internalize functions. 
     * So, the child classes may provide their own implementations 
     * for internalizations of certain metadata.
     *
     * @exception KErrNotFound if the internalization could not
     * find any data for this metadata object. Then the owner
     * may delete this class object.
     */
    void InternalizeL();
    

public: // MNcdNodeMetadata

    /**
     * @see MNcdNodeMetaData::Id
     */
    virtual const TDesC& Id() const;


    /**
     * @see MNcdNodeMetaData::Namespace
     */
    virtual const TDesC& Namespace() const;


    /**
     * @see MNcdNodeMetaData::Name
     */
    virtual const TDesC& Name() const;


    /**
     * @see MNcdNodeMetaData::Descripton
     */
    virtual const TDesC& Description() const;


    /**
     * @see MNcdNodeMetaData::Disclaimer
     */
    virtual MNcdQuery* Disclaimer() const;


    /**
     * @see MNcdNodeMetaData::ActionName
     */
    virtual const TDesC& ActionName() const;

    /**
     * @see MNcdNodeMetaData::IsAlwaysVisible
     */
    virtual TBool IsAlwaysVisible() const;
    
    /**
     * @see MNcdNodeMetaData::LayoutType
     */
    virtual const TDesC& LayoutType() const;

    /**
     * @see MNcdNodeMetaData::MoreInfo
     */
    virtual MNcdQuery* MoreInfo() const;
    
    /**
     * @see MNcdNodeMetadata::Details
     */
    virtual const RPointerArray< CNcdKeyValuePair >& Details() const;


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNode The node that owns this metadata
     */
    CNcdNodeMetadataProxy( MCatalogsClientServer& aSession,
                           TInt aHandle,
                           CNcdNodeProxy& aNode );

    /**
     * ConstructL
     */
    virtual void ConstructL();

    // These functions are used to update the data of this class object

    // Leaves with KErrNotFound if the internalization could
    // not find any data for this metadata. Then, the
    // owner of this class may delete this class object.
    virtual void InternalizeDataL( RReadStream& aStream );

    void InternalizeDownloadL();
    void InternalizeUpgradeL();
    void InternalizeDependencyL();
    void InternalizeIconL();
    void InternalizeScreenshotL();
    void InternalizePurchaseL();
    void InternalizeUriContentL();
    void InternalizeContentInfoL();
    void InternalizeInstallL();
    void InternalizePreviewL();
    void InternalizeSkinL();
    void InternalizeActivateL();         


private:

    // Prevent if not implemented
    CNcdNodeMetadataProxy( const CNcdNodeMetadataProxy& aObject );
    CNcdNodeMetadataProxy& operator =( const CNcdNodeMetadataProxy& aObject );


    
    // Internalizes the user data object if the data can be found from
    // the server side.
    void InternalizeUserDataL();

    // Internalizes the disclaimer. 
    // Also, creates the disclaimer proxy if necessary.
    void InternalizeDisclaimerL();

    // Internalizes the more info. 
    // Also, creates the more info proxy if necessary.
    void InternalizeMoreInfoL();

    
private: // data

    CNcdNodeProxy& iNode;

    // MNcdNodeMetaData data
    
    TBool iIsAlwaysVisible;
    CNcdNodeIdentifier* iIdentifier;
    HBufC* iName;
    HBufC* iDescription;
    HBufC* iLayoutType;
    HBufC* iActionName;
    
    // Disclaimer is part of the metadata information.
    // The disclaimer information of the metadata will be given to the UI
    // as a query object.
    CNcdNodeDisclaimerProxy* iDisclaimer;
    CNcdNodeDisclaimerProxy* iMoreInfo;

    // Details
    RPointerArray< CNcdKeyValuePair > iDetails;

    // These are the possible additional interface objects that this node
    // may use. If the pointer value is NULL then the node does not provide
    // functionality for that interface at that moment. Mainly these objects
    // are created after certain operations have been done for the node.

    // Node user data
    CNcdNodeUserDataProxy* iUserData;
    
    // Node icon
    CNcdNodeIconProxy* iIcon;    

    // Node screenshot
    CNcdNodeScreenshotProxy* iScreenshot;    
    
    // Preview data
    CNcdNodePreviewProxy* iPreview;
    
     // Content info
    CNcdNodeContentInfoProxy* iContentInfo;   

    // Uri content
    CNcdNodeUriContentProxy* iUriContent;
        
    // Purchase data
    CNcdNodePurchaseProxy* iPurchase;

    // Node install
    CNcdNodeInstallProxy* iInstall;

    // Node download
    CNcdNodeDownloadProxy* iDownload;

    // If this node provides upgrades to some other nodes.
    CNcdNodeUpgradeProxy* iUpgrade;

    // If this node provides upgrades to some other nodes.
    CNcdNodeDependencyProxy* iDependency;

    // Skin data
    CNcdNodeSkinProxy* iSkin;

    // Node activate
    CNcdNodeActivate* iActivate;
        
    };


#endif // NCD_NODE_METADATA_PROXY_H
