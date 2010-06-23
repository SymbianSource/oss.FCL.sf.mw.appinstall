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
* Description:   Contains CNcdNodeContentInfo class
*
*/


#ifndef C_NCDNODECONTENTINFO_H
#define C_NCDNODECONTENTINFO_H


// For streams
#include <s32mem.h>

#include "ncdcommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"


class MNcdPreminetProtocolDataEntity;
class MNcdPurchaseDetails;

_LIT( KNcdContentPurposeMusic, "music" );
_LIT( KNcdContentPurposeRingtone, "ringtone" );
_LIT( KNcdContentPurposeWallpaper, "wallpaper" );
_LIT( KNcdContentPurposeVideo, "video" );
_LIT( KNcdContentPurposeTheme, "theme" );
_LIT( KNcdContentPurposeApplication, "application" );
_LIT( KNcdContentPurposeHtmlPage, "html-page" );
_LIT( KNcdContentPurposeGame, "game" );
_LIT( KNcdContentPurposeScreensaver, "screensaver" );
_LIT( KNcdContentPurposeStream, "stream" );

/**
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
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeContentInfo : public CNcdCommunicable,
                            public MNcdStorageDataItem
    {
public:
    /**
     * NewL
     *
     * @return CNcdNodeContentInfo* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeContentInfo* NewL();

    /**
     * NewLC
     *
     * @return CNcdNodeContentInfo* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeContentInfo* NewLC();


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeContentInfo();


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
     * The purpose of the node.
     * 
     * @return Bit field describing the purpose(s) of this node, a combination
     *         of TNcdItemPurpose flags.
     * @see TNcdItemPurpose
     */
    TUint Purpose() const;    
    

    /**
     * Gives the mime type of the content data.
     *
     * @return The mime type of the download content data. The type is
     * given as a text according to the standards that define MIME types.
     */
    const TDesC& MimeType() const;


    /**
     * This function is the indicative Symbian application UID for the
     * contents that are applications. This can be used e.g. for checking
     * if the application has already been installed to the phone.
     *
     * @return The UID of the application item.
     */
    const TUid& Uid() const;
    
    /**
     * This function is the indicative identifier for the
     * contents that are widgets. This can be used e.g. for checking
     * if the widget has already been installed to the phone.
     *
     * @return The Identifier of the application item.
     */
    const TDesC& Identifier() const;

    /**
     * Different versions of the content items may exist. Thus,
     * a version identifier may be defined for the item. When installing
     * applications the version may be required.
     *
     * @return Version string of this application item.  
     * If the protocol has not defined
     * any value, an empty string is returned.
     */
    const TDesC& Version() const;
    
    /**
     * Gives the total content size of the item in bytes.
     *
     * @return The content size or 0 if not defined.
     */
    TInt TotalContentSize() const;


    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the parser.
     * 
     * @param aData The data is set in the protocol parser and can
     * be used to initialize this class object.
     */
    void InternalizeL( MNcdPreminetProtocolDataEntity& aData );
    
    /** 
     * Internalizes content info from purchase details
     */
    void InternalizeL( const MNcdPurchaseDetails& aDetails );
    
public: // MNcdStorageDataItem 

    // These functions are used to get the data from and to insert the data
    // into the database using by the given stream.

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


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
    CNcdNodeContentInfo( NcdNodeClassIds::TNcdNodeClassId aClassId );

    /**
     * ConstructL
     */
    void ConstructL();
    

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
    void ExternalizeDataForRequestL( RWriteStream& aStream );

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeContentInfo( const CNcdNodeContentInfo& aObject );
    CNcdNodeContentInfo& operator =( const CNcdNodeContentInfo& aObject );


private: // data
    
    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;

    TUint iPurpose;
    HBufC* iMimeType;
    TUid iUid;         // UID is an identifier for sis 
    HBufC* iIdentifier; // a string identifier for widget
    HBufC* iVersion;
    TInt iSize;

    };
    
#endif // C_NCDNODECONTENTINFO_H
