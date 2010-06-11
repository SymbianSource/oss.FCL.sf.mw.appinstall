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
* Description:   Contains CNcdNodeUpgradeProxy class
*
*/


#ifndef C_NCD_NODE_UPGRADE_PROXY_H
#define C_NCD_NODE_UPGRADE_PROXY_H


// For the streams
#include <s32mem.h>
// For array
#include <e32cmn.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodeupgrade.h"

class CNcdNodeMetadataProxy;
class CNcdNodeManagerProxy;
class MNcdNodeDependencyInfo;
class CNcdAttributes;

/**
 *  This class implements the functionality for the
 *  MNcdNodeUpgrade interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeUpgradeProxy : public CNcdInterfaceBaseProxy, 
                             public MNcdNodeUpgrade
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUpgradeProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUpgradeProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUpgradeProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUpgradeProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Upgrade may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeUpgradeProxy();


    /**
     * @return CNcdNodeManagerProxy& Node manager can be used to create nodes.
     */
    CNcdNodeManagerProxy& NodeManager() const;
     

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeUpgrade

    /**
     * @see MNcdNodeUpgrade::Name
     */
    virtual const TDesC& Name() const;

    /**
     * @see MNcdNodeUpgrade::Uid
     */
    virtual TUid Uid() const;

    /**
     * @see MNcdNodeUpgrade::Version
     */
    virtual const TDesC& Version() const;

    /**
     * @see MNcdNodeUpgrade::UpgradeableNodeL
     */ 
    virtual MNcdNode* UpgradeableNodeL() const;


    /**
     * @see MNcdNodeUpgrade::UpgradeType
     */ 
    virtual MNcdNodeUpgrade::TUpgradeType UpgradeType() const;

protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUpgradeProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeUpgradeProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    /**
     * @return RPointerArray Contains the info about the nodes
     * or contents that this node may upgrade.
     */
    const RPointerArray<MNcdNodeDependencyInfo>& UpgradeTargets() const;
    

    // These functions are used to update the data of this class object

    /**
     * @param aStream This stream will contain all the data content for
     * this class object. The stream is gotten from the server side. The
     * memeber variables will be updated according to the data from
     * the stream. 
     */
    virtual void InternalizeDataL( RReadStream& aStream );


private:

    // Prevent if not implemented
    CNcdNodeUpgradeProxy( const CNcdNodeUpgradeProxy& aObject );
    CNcdNodeUpgradeProxy& operator =( const CNcdNodeUpgradeProxy& aObject );

    // Internalizes the upgrade target array from given stream.
    void InternalizeUpgradeArrayL( RReadStream& aStream );  
    
    // Refreshes iUpgradeType
    void RefreshUpgradeType();
    
private: // data

    CNcdNodeManagerProxy& iNodeManager;

    RPointerArray<MNcdNodeDependencyInfo> iUpgradeTargets;
    TUpgradeType iUpgradeType;    
    CNcdAttributes* iUpgradeData;
    };


#endif // C_NCD_NODE_UPGRADE_PROXY_H
