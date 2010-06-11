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
* Description:   Contains CNcdNodeuriContentProxy class
*
*/


#ifndef C_NCDNODEURICONTENTPROXY_H
#define C_NCDNODEURICONTENTPROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodeuricontent.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeUriContent interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeUriContentProxy : public CNcdInterfaceBaseProxy, 
                                public MNcdNodeUriContent
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
     * @return CNcdNodeUriContentProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUriContentProxy* NewL(
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
     * @return CNcdNodeUriContentProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUriContentProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns this object may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeUriContentProxy();


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeUriContent

    /**
     * @see MNcdNodeUriContent::ContentUri
     */ 
    const TDesC& ContentUri() const;

    /**
     * @see MNcdNodeUriContent::ContentValidityDelta
     */
    TInt ContentValidityDelta() const;

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
     * @return CNcdNodeUriContentProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeUriContentProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

    // These functions are used to update the data of this class object
    virtual void InternalizeDataL( RReadStream& aStream );


private:

    // Prevent if not implemented
    CNcdNodeUriContentProxy( const CNcdNodeUriContentProxy& aObject );
    CNcdNodeUriContentProxy& operator =( const CNcdNodeUriContentProxy& aObject );
    
    
private: // data

    HBufC* iContentUri;
    TInt iValidityDelta;
    
    };


#endif // C_NCDNODEURICONTENTPROXY_H
