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
* Description:   Contains CNcdNodeIcon class
*
*/


#ifndef NCD_NODE_ICON_IMPL_H
#define NCD_NODE_ICON_IMPL_H


// For streams
#include <s32mem.h>

#include "ncdcommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"


class MNcdPreminetProtocolDataEntity;
class CNcdNodeManager;
class CNcdNodeMetaData;


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
 *  @since S60 v3.2
 */
class CNcdNodeIcon : public CNcdCommunicable,
                     public MNcdStorageDataItem
    {

public:

    /**
     * NewL
     *
     * @return CNcdNodeIcon* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIcon* NewL(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeMetaData& aParentMetaData,
        TBool aUsePurchaseDetailsIcon = EFalse );

    /**
     * NewLC
     *
     * @return CNcdNodeIcon* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIcon* NewLC(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeMetaData& aParentMetaData,
        TBool aUsePurchaseDetailsIcon = EFalse );


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeIcon();


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
     * that has been received from the parser.
     * 
     * @param aData The data is set in the protocol parser and can
     * be used to initialize this class object.
     */
    void InternalizeL( MNcdPreminetProtocolDataEntity& aData );
   
    
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


public:

    /**
     * Get icon data block.
     *
     * @return Data block.
     */
    const TDesC& DataBlock() const;
    
    /**
     * Get icon ID.
     *
     * @return Icon ID.
     */
    const TDesC& IconId() const;
    
    /**
     * Set the icon ID.
     *
     * @param aIconId The icon id.
     */
    void SetIconIdL( const TDesC& aIconId );
    
    /**
     * Get icon URI.
     *
     * @return Icon URI.
     */
    const TDesC& Uri() const;
    
    /**
     * Set icon URI.
     *
     * @param aUri The URI.
     */
    void SetUriL( const TDesC& aUri );

    /**
     * Get icon data.
     * 
     * @return Icon data.
     */
    HBufC8* IconDataL() const;
        
    /**
     * Tells whether the icon data is ready.
     *
     * @return True if ready, otherwise false.
     */
    TBool IconDataReady() const;
    
    /**
     * Set the icon data ready flag.
     *
     * @param aValue True if ready, false otherwise.
     */
    void SetIconDataReady( TBool aValue );

protected:

    /**
     * Constructor
     * @param aClassId Identifies this class. 
     * Is set in the NewLC function 
     */
    CNcdNodeIcon(
        NcdNodeClassIds::TNcdNodeClassId aClassId,
        CNcdNodeManager& aNodeManager,
        const CNcdNodeMetaData& aParentMetaData,
        TBool aUsePurchaseDetailsIcon );

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
     * child classes.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const;
        
    /**
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;

    /**
     * This function is called when the proxy wants to get the
     * icon data from the serverside.
     * @param aMessage Can be used to send data back to proxy.
     */
    void IconDataRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeIcon( const CNcdNodeIcon& aObject );
    CNcdNodeIcon& operator =( const CNcdNodeIcon& aObject );


private: // data

    // Node manager.
    CNcdNodeManager& iNodeManager;
    
    // Parent meta data.
    const CNcdNodeMetaData& iParentMetaData;

    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    // Icon data
    HBufC* iDataBlock;
    HBufC* iIconId;
    HBufC* iUri;
    
    // Tells whether the icon data is ready.
    TBool iIconDataReady;
    
    // If set then icon data is retrieved from purchase details.
    TBool iUsePurchaseDetailsIcon;
    
    };
    
#endif // NCD_NODE_ICON_IMPL_H
