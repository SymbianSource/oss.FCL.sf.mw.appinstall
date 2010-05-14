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
* Description:   Contains CNcdNodeUserData class
*
*/


#ifndef NCD_NODE_USER_DATA_H
#define NCD_NODE_USER_DATA_H


// For streams
#include <s32mem.h>

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"


class CNcdNodeManager;
class CNcdNodeIdentifier;


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
class CNcdNodeUserData : public CCatalogsCommunicable,
                         public MNcdStorageDataItem 
    {

public:
    /**
     * NewL
     *
     * @param aIdentifier Identifies the node whose user data this class
     * object will contain.
     * @param aManager Used when the user data is read from or written 
     * into the database.
     * @return CNcdNodeUserData* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUserData* NewL( const CNcdNodeIdentifier& aIdentifier,
                                   CNcdNodeManager& aManager );

    /**
     * NewLC
     *
     * @param aIdentifier Identifies the node whose user data this class
     * object will contain.
     * @param aManager Used when the user data is read from or written 
     * into the database.
     * @return CNcdNodeUserData* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUserData* NewLC( const CNcdNodeIdentifier& aIdentifier,
                                    CNcdNodeManager& aManager );


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeUserData();


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


public: // MNcdStorageDataItem

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * Constructor
     *
     * @param aClassId Identifies this class. 
     * Is set in the NewLC function. 
     * @param aManager Used when the user data is read from or written 
     * into the database.
     */
    CNcdNodeUserData( NcdNodeClassIds::TNcdNodeClassId aClassId,
                      CNcdNodeManager& aManager );

    /**
     * ConstructL
     *
     * @param aIdentifier Identifies the node whose user data this class
     * object will contain.
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );
    

    // These functions are called from the ReceiveMessage when
    // the given function id has matched to the function.

    /**
     * This function is called when the proxy wants to get the
     * user data from the serverside.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void UserDataRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function is called when the proxy wants to set the
     * user data into the db.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void SetUserDataRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function is called when the proxy wants to clear the
     * user data of the node from the db.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void ClearUserDataRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeUserData( const CNcdNodeUserData& aObject );
    CNcdNodeUserData& operator =( const CNcdNodeUserData& aObject );


private: // data
    
    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;

    // Identifies to which metadata this user data belongs to.
    CNcdNodeIdentifier* iIdentifier;

    // The reference to the node that has this user data.
    CNcdNodeManager& iManager;

    // This contains the user data
    HBufC8* iUserData;
     
    };
    
#endif // NCD_NODE_USER_DATA_H    
