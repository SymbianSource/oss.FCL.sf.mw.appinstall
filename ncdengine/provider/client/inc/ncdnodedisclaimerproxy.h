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
* Description:   Contains CNcdNodeDisclaimerProxy class
*
*/


#ifndef C_NCD_NODE_DISCLAIMER_PROXY_H
#define C_NCD_NODE_DISCLAIMER_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdquery.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdQuery interface. The interface is provided for
 *  API users when the disclaimer is asked from the metadata.
 *  The metadata owns this class object. Because this class object
 *  is part of the metadata, the reference counter keeps this
 *  object alive until the metadata that owns this object
 *  is deleted.
 *
 *  @since S60 v3.2
 */
class CNcdNodeDisclaimerProxy : public CNcdInterfaceBaseProxy, 
                                public MNcdQuery
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
     * count reaches zero. If the disclaimer is not part of metadata, give NULL.
     * Ownership is not transferred.
     * @return CNcdNodeDisclaimerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDisclaimerProxy* NewL( MCatalogsClientServer& aSession,
                                          TInt aHandle,
                                          CNcdNodeMetadataProxy* aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero. If the dislaimer is not part of metadata, give NULL.
     * Ownership is not transferred.
     * @return CNcdNodeDisclaimerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDisclaimerProxy* NewLC( MCatalogsClientServer& aSession, 
                                           TInt aHandle,
                                           CNcdNodeMetadataProxy* aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Disclaimer may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeDisclaimerProxy();


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdQuery

    /**
     * @see MNcdQuery::IsOptional
     */ 
    virtual TBool IsOptional() const;
    
    /**
     * @see MNcdQuery::Semantics
     */ 
    virtual MNcdQuery::TSemantics Semantics() const;
    
    /**
     * @see MNcdQuery::MessageTitle()
     */ 
    virtual const TDesC& MessageTitle() const;
    
    /**
     * @see MNcdQuery::MessageBody
     */ 
    virtual const TDesC& MessageBody() const;
    
    /**
     * Does not do anything. Only returns an empty array.
     *
     * @see MNcdQuery::QueryItemsL
     */ 
    virtual RCatalogsArray< MNcdQueryItem > QueryItemsL();

    /**
     * Can be used for UI's own purposes. Only saves
     * the response value for this object. The UI
     * may ask the reponse value by calling Response function.
     *
     * @see MNcdQuery::SetResponseL
     */ 
    virtual void SetResponseL( TResponse aResponse );

    /**
     * @see MNcdQuery::Response
     */ 
    virtual MNcdQuery::TResponse Response();
    
    /**
     * Always returns EFalse, because there is no connection
     * used with disclaimer.
     * @see MNcdQuery::IsSecureConnection
     */ 
    virtual TBool IsSecureConnection() const;


protected:

    /**
     * Constructor
     *
     * Sets this class object to be its own interface parent.
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero. If the disclaimer is not part of metadata, give NULL.
     * Ownership is not transferred.
     * @return CNcdNodeDisclaimerProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeDisclaimerProxy( MCatalogsClientServer& aSession,
                             TInt aHandle,
                             CNcdNodeMetadataProxy* aMetadata );

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
    CNcdNodeDisclaimerProxy( const CNcdNodeDisclaimerProxy& aObject );
    CNcdNodeDisclaimerProxy& operator =( const CNcdNodeDisclaimerProxy& aObject );
    
    
private: // data

    TBool iOptional;
    MNcdQuery::TSemantics iSemantics;
    HBufC* iTitle;
    HBufC* iBody;
    MNcdQuery::TResponse iResponse;

    };


#endif // C_NCD_NODE_DISCLAIMER_PROXY_H
