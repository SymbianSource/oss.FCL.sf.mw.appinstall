/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdNodeContentInfoProxy class
*
*/


#ifndef C_NCDNODECONTENTINFOPROXY_H
#define C_NCDNODECONTENTINFOPROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodecontentinfo.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeContentInfo interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeContentInfoProxy : public CNcdInterfaceBaseProxy, 
                                 public MNcdNodeContentInfo
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
     * @return CNcdNodeContentInfoProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeContentInfoProxy* NewL(
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
     * @return CNcdNodeContentInfoProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeContentInfoProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns this object may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeContentInfoProxy();


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeContent

    /**
     * @see MNcdNodeContent::Purpose
     */ 
    TUint Purpose() const;    
    
    /**
     * @see MNcdNodeContent::MimeType
     */ 
    const TDesC& MimeType() const;
    
    /**
     * @see MNcdNodeContent::Uid
     */ 
    TUid Uid() const;
    
    /**
     * @see MNcdNodeContent::Version
     */ 
    const TDesC& Version() const;

    /**
     * @see MNcdNodeContent::Size
     */     
    virtual TInt Size() const;

    /**
     * @see MNcdNodeContent::Identifier
     */ 
    const TDesC& Identifier() const;
    
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
     * @return CNcdNodeContentInfoProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeContentInfoProxy(
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
    CNcdNodeContentInfoProxy( const CNcdNodeContentInfoProxy& aObject );
    CNcdNodeContentInfoProxy& operator =( const CNcdNodeContentInfoProxy& aObject );
    
    
private: // data

    TUint iPurpose;
    HBufC* iMimeType;
    HBufC* iIdentifier;
    TUid iUid;
    HBufC* iVersion;
    TInt iSize;
    
    };


#endif // C_NCDNODECONTENTINFOPROXY_H
