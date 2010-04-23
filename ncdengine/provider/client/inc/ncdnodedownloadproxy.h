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
* Description:   Contains CNcdNodeDownloadProxy class
*
*/


#ifndef C_NCD_NODE_DOWNLOAD_PROXY_H
#define C_NCD_NODE_DOWNLOAD_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodedownload.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeDownload interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeDownloadProxy : public CNcdInterfaceBaseProxy, 
                              public MNcdNodeDownload
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The proxy also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeDownloadProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDownloadProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The proxy also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeDownloadProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDownloadProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Download may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeDownloadProxy();


    /**
     * @return CNcdNodeMetadataProxy& Gives the proxy that owns this class object.
     */
    CNcdNodeMetadataProxy& Metadata() const;


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

public: // MNcdNodeDownload

    /**
     * @see MNcdNodeDownload::IsDownloadedL
     */
    virtual TBool IsDownloadedL() const;


    /**
     * @see MNcdNodeDownload::DownloadContentL
     */
    virtual MNcdDownloadOperation* DownloadContentL( 
        MNcdDownloadOperationObserver& aObserver );


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The proxy also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeDownloadProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeDownloadProxy(
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
    CNcdNodeDownloadProxy( const CNcdNodeDownloadProxy& aObject );
    CNcdNodeDownloadProxy& operator =( const CNcdNodeDownloadProxy& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;

    TBool iIsDownloaded;    
    };


#endif // C_NCD_NODE_DOWNLOAD_PROXY_H
