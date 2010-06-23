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
* Description:   Contains MNcdNodeUserData
*
*/


#ifndef M_NCD_NODE_USER_DATA_H
#define M_NCD_NODE_USER_DATA_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 *  Provides functions to handle the user data of the node.
 *  User data is meant for clients side to save some custom information
 *  relating to a node. So, client can also read its own settings from engine
 *  by using functions provided by this interface. For example, if the
 *  client wants to set the node hidden and save this value to the node it may
 *  use user data functions.
 *
 *  The format of the data is entirely up to the client. The engine saves
 *  the user data as a single binary data entry.
 *
 *  
 */
class MNcdNodeUserData : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeUserDataUid };

    /**
     * Gives the user data that has been saved in database for the node.
     *
     * @return User data.
     * Ownership of the heap descriptor is transferred to the caller. If the
     * node has no user data set, a NULL pointer is returned.
     * @exception Leave System wide error codes.
     */
    virtual HBufC8* UserDataL() const = 0;

    /**
     * Sets user data for the node.
     * 
     * @param aData is the data that is saved to the node information. If NULL,
     *  clears the previously set user data. Ownership of aData is not transferred.
     * @exception Leave System wide error codes.
     */
    virtual void SetUserDataL( const TDesC8* aData ) = 0;    

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeUserData() {}

    };


#endif // M_NCD_NODE_USER_DATA_H
