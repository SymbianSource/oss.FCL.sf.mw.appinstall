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
* Description:   Contains CNcdNodeDownload class
*
*/


#ifndef NCD_NODE_DOWNLOAD_H
#define NCD_NODE_DOWNLOAD_H


// For streams
#include <s32mem.h>
#include <badesca.h>

#include "catalogscommunicable.h"
#include "ncdnodeclassids.h"


class MNcdPreminetProtocolDataEntity;
class MNcdPurchaseDetails;
class MNcdPurchaseDownloadInfo;


/**
 *  CNcdNodeDownload does not handle db information directly. The information
 *  is Internalized from the purchase history data and also the data is saved
 *  to the purchase history when the purchase operation progresses.
 *
 *  This server side class contains the data and the functionality
 *  that the proxy objects will use to internalize itself.
 *
 *  This object should be added to the session. So, it will be usable
 *  in the proxy side by using the handle gotten during addition. 
 *  The handle is used to identify to what object the proxy directs 
 *  the function call.  When objects are added to sessions, 
 *  multiple handles may be gotten for the same object if addition is 
 *  done multiple times.
 *
 *  @lib ?library
 *  @since S60 v3.2
 */
class CNcdNodeDownload : public CCatalogsCommunicable
    {

public:
    /**
     * NewL
     *
     * @return CNcdNodeDownload* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDownload* NewL();

    /**
     * NewLC
     *
     * @return CNcdNodeDownload* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDownload* NewLC();


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeDownload();


    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a object that creates this 
     * class object.
     *
     * @return NcdNodeClassIds::TNcdNodeClassId Describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;


    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the purchase history.
     * 
     * @param aDetails Purchase details from purchase history.
     * @return ETrue if purchase details contained download
     * data to internalize.
     */
    TBool InternalizeL( const MNcdPurchaseDetails& aDetails );
    
    /**
     * Get download information.
     *
     * @return Download information.
     */
    const RPointerArray< MNcdPurchaseDownloadInfo >& DownloadInfo() const;
          
    
public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );


protected:

    /**
     * Constructor
     * @param aClassId Identifies this class. 
     * Is set in the NewLC function 
     */
    CNcdNodeDownload( NcdNodeClassIds::TNcdNodeClassId aClassId );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

    // These functions are called from the ReceiveMessage when
    // the given function id has matched to the function.

    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * InternalizeDataForRequestL which may be overloaded in the
     * child classes
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
        
    /**
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream );

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


    void FilesExistRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Checks that all downloaded files actually exist in the file system
     */
    TBool FilesExist();

private:

    // Prevent these two if they are not implemented
    CNcdNodeDownload( const CNcdNodeDownload& aObject );
    CNcdNodeDownload& operator =( const CNcdNodeDownload& aObject );


private: // data
    
    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;
    
    // Download info array.
    RPointerArray< MNcdPurchaseDownloadInfo > iDownloadInfo;
    
    CDesCArrayFlat* iFiles;
    
    // Indicates download status.
    TBool iIsDownloaded;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    };
    
#endif // NCD_NODE_DOWNLOAD_H
