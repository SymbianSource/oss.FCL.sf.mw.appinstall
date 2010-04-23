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
* Description:   Contains CNcdNodeDependencyProxy class
*
*/


#ifndef C_NCD_NODE_DEPENDENCY_PROXY_H
#define C_NCD_NODE_DEPENDENCY_PROXY_H


// For the streams
#include <s32mem.h>
// For array
#include <e32cmn.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodedependency.h"

class CNcdNodeMetadataProxy;
class CNcdNodeManagerProxy;
class MNcdNodeDependencyInfo;

/**
 *  This class implements the functionality for the
 *  MNcdNodeDependency interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeDependencyProxy : public CNcdInterfaceBaseProxy, 
                                public MNcdNodeDependency
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
     * @return CNcdNodeDependencyProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDependencyProxy* NewL(
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
     * @return CNcdNodeDependencyProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDependencyProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Dependency may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeDependencyProxy();


    /**
     * @return CNcdNodeManageraProxy& Can be used to create nodes.
     */
    CNcdNodeManagerProxy& NodeManager() const;
     

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeDependency

    /**
     * @see MNcdNodeDependency::DependencyInfos()
     */ 
    virtual const RPointerArray< MNcdNodeDependencyInfo >& DependencyInfos() const;


    /**
     * @see MNcdNodeDependency::State()
     */ 
    virtual TNcdDependencyState State() const;
    
    
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
     * @return CNcdNodeDependencyProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeDependencyProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
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
    CNcdNodeDependencyProxy( const CNcdNodeDependencyProxy& aObject );
    CNcdNodeDependencyProxy& operator =( const CNcdNodeDependencyProxy& aObject );

    // Internalizes the Dependency target array from given stream.
    void InternalizeDependencyArrayL( RReadStream& aStream );  

    
private: // data

    CNcdNodeManagerProxy& iNodeManager;

    RPointerArray<MNcdNodeDependencyInfo> iDependencyTargets;
    
    };


#endif // C_NCD_NODE_DEPENDENCY_PROXY_H
