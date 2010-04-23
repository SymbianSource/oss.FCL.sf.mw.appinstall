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
* Description:   Contains CNcdNodeScreenshotProxy class
*
*/


#ifndef C_NCD_NODE_SCREENSHOT_PROXY_H
#define C_NCD_NODE_SCREENSHOT_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodescreenshot.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeScreenshot interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeScreenshotProxy : public CNcdInterfaceBaseProxy, 
                                public MNcdNodeScreenshot
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
     * @return CNcdNodeScreenshotProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeScreenshotProxy* NewL(
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
     * @return CNcdNodeScreenshotProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeScreenshotProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Screenshot may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeScreenshotProxy();


    /**
     * @return CNcdNodeMetadataProxy& Gives the proxy that owns this class object.
     * @exception KNcdErrorObsolete if the object has been set as obsolete. Then, we
     * cannot be sure if the original metadata still exists because it is not the parent
     * of this class object anymore.
     */
    CNcdNodeMetadataProxy& MetadataL() const;

     
    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain data.
     */
    void InternalizeL();
    

public: // MNcdNodeScreenshot

    /**
     * @see MNcdNodeScreenshot::ScreenshotCount
     */ 
    TInt ScreenshotCount() const;

    /**
     * @see MNcdNodeScreenshot::ScreenshotMimeType
     */ 
    const TDesC& ScreenshotMimeType( TInt aIndex ) const;

    /**
     * @see MNcdNodeScreenshot::LoadScreenshotL
     */ 
    MNcdDownloadOperation* LoadScreenshotL(
        TInt aIndex,
        MNcdDownloadOperationObserver* aObserver );

    /**
     * @see MNcdNodeScreenshot::ScreenshotDataL
     */ 
    HBufC8* ScreenshotDataL( TInt aIndex ) const;


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
     * @return CNcdNodeScreenshotProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeScreenshotProxy(
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
    CNcdNodeScreenshotProxy( const CNcdNodeScreenshotProxy& aObject );
    CNcdNodeScreenshotProxy& operator =( const CNcdNodeScreenshotProxy& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;
    TBool iObsolete;
    TInt iScreenshotCount;
    RPointerArray<HBufC> iScreenshotMimeType;

    };


#endif // C_NCD_NODE_SCREENSHOT_PROXY_H
