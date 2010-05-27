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
* Description:   Contains CNcdNodeIconProxy class
*
*/


#ifndef C_NCDNODEICONPROXY_H
#define C_NCDNODEICONPROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodeicon.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeIcon interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeIconProxy : public CNcdInterfaceBaseProxy, 
                          public MNcdNodeIcon
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
     * @return CNcdNodeIconProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIconProxy* NewL(
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
     * @return CNcdNodeIconProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIconProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the icon may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeIconProxy();
    

    /**
     * @return CNcdNodeMetadataProxy& The metadata that owns this object.
     * @exception KNcdErrorObsolete if the object has been set as obsolete. Then, we
     * cannot be sure if the original metadata still exists because it is not the parent
     * of this class object anymore.
     */
    CNcdNodeMetadataProxy& MetadataL() const;     


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeIcon

    /**
     * @see MNcdNodeIcon::IconId
     */ 
    const TDesC& IconId() const;

    /**
     * NOTE: Currently protocol does not offer mime type!
     *       This function returns KNullDesC!
     * @see MNcdNodeIcon::IconMimeType
     */
    const TDesC& IconMimeType() const;

    /**
     * @see MNcdNodeIcon::LoadIconL
     */ 
    MNcdDownloadOperation* LoadIconL(
        MNcdDownloadOperationObserver* aObserver );

    /**
     * @see MNcdNodeIcon::IconDataL
     */
    HBufC8* IconDataL();


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
     * @return CNcdNodeIconProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeIconProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * @return TBool ETrue if this object has been set as obsolete.
     */
    TBool IsObsolete() const;
    
    /**
     * @param aObsolete ETrue if the object should be set as obsolete else EFalse.
     */
    void SetObsolete( TBool aObsolete );
    
    
    void InternalizeDataL( RReadStream& aStream );


private:

    // Prevent if not implemented
    CNcdNodeIconProxy( const CNcdNodeIconProxy& aObject );
    CNcdNodeIconProxy& operator =( const CNcdNodeIconProxy& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;
    
    HBufC* iIconId;

    TBool iObsolete;
    };


#endif // C_NCDNODEICONPROXY_H
