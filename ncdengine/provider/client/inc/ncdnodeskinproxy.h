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
* Description:   Contains CNcdNodeSkinProxy class
*
*/


#ifndef C_NCD_NODE_SKIN_PROXY_H
#define C_NCD_NODE_SKIN_PROXY_H


// For the streams
#include <s32mem.h>
// For TTime 
#include <e32std.h>


#include "ncdinterfacebaseproxy.h"
#include "ncdnodeskin.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeSkin interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeSkinProxy : public CNcdInterfaceBaseProxy, 
                          public MNcdNodeSkin
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * It also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeSkinProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSkinProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * It also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeSkinProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSkinProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Skin may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeSkinProxy();
     

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeSkin

    /**
     * @see MNcdNodeSkin::SkinId
     */
    virtual const TDesC& SkinId() const;


    /**
     * @see MNcdNodeSkin::SkinTimeStamp
     */
    virtual TTime SkinTimeStamp() const;


    /**
     * @see MNcdNodeSkin::LoadSkinL
     */ 
    virtual MNcdFileDownloadOperation* LoadSkinL( 
        const TDesC& aTargetFileName,
        MNcdFileDownloadOperationObserver& aObserver );


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * It also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeSkinProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeSkinProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * @return CNcdNodeMetadataProxy& The metadata that owns this object.
     * @exception KNcdErrorObsolete if the object has been set as obsolete. Then, we
     * cannot be sure if the original metadata still exists because it is not the parent
     * of this class object anymore.
     */
    CNcdNodeMetadataProxy& MetadataL() const;     


    /**
     * @return TBool ETrue if this object has been set as obsolete.
     */
    TBool IsObsolete() const;
    
    /**
     * @param aObsolete ETrue if the object should be set as obsolete else EFalse.
     */
    void SetObsolete( TBool aObsolete );
    

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
    CNcdNodeSkinProxy( const CNcdNodeSkinProxy& aObject );
    CNcdNodeSkinProxy& operator =( const CNcdNodeSkinProxy& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;
    TBool iObsolete;
    
    // Skin data
    HBufC* iSkinId;
    TTime iTimeStamp;
    
    };


#endif // C_NCD_NODE_SKIN_PROXY_H
