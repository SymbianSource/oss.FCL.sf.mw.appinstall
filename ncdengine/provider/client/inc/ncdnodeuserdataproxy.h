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
* Description:   Contains CNcdNodeUserDataProxy class
*
*/


#ifndef C_NCD_NODE_USER_DATA_PROXY_H
#define C_NCD_NODE_USER_DATA_PROXY_H


#include "ncdinterfacebaseproxy.h"
#include "ncdnodeuserdata.h"

class CNcdNodeMetadataProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeUserData interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeUserDataProxy : public CNcdInterfaceBaseProxy, 
                              public MNcdNodeUserData
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUserDataProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUserDataProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aNodeMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeMetadata Owns and uses the information of this proxy.
     * The node also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUserDataProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUserDataProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aNodeMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the UserData may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeUserDataProxy();
     

public: // MNcdNodeUserData

    /**
     * @see MNcdNodeUserData::UserDataL
     */
    virtual HBufC8* UserDataL() const;

    /**
     * @see MNcdNodeUserData::SetUserDataL
     */
    virtual void SetUserDataL( const TDesC8* aData );    


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeMetadata Owns and uses the information of this proxy.
     * The node also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeUserDataProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeUserDataProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aNodeMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    /**
     * @return CNcdNodeProxy& Gives the node proxy that owns this class object.
     * This node may be given for the operations. So, the operations may
     * inform the node about the progress of or about the completion of the
     * operation.
     */
    CNcdNodeMetadataProxy& NodeMetadata() const;


private:

    // Prevent if not implemented
    CNcdNodeUserDataProxy( const CNcdNodeUserDataProxy& aObject );
    CNcdNodeUserDataProxy& operator =( const CNcdNodeUserDataProxy& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iNodeMetadata;

    };


#endif // C_NCD_NODE_USER_DATA_PROXY_H
