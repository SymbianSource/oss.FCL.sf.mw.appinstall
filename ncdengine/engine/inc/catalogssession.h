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
* Description:   Interface MCatalogsSession declaration
*
*/


#ifndef M_CATALOGS_SESSION_H
#define M_CATALOGS_SESSION_H

#include <e32base.h>

class MCatalogsContext;

/**
 *  Interface to session on server side. Used for example for 
 *  receiver object registration.
 *
 *  Message is passed to a certain object within a session
 *  based on an object handle passed along with the message.
 *  A handle is generated when an object is registered as a message
 *  receiver. This interface gives access for example to functions
 *  to register objects as message receiver and to later remove 
 *  these registered receivers. 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class  MCatalogsSession
    {

public:

    /**
     * Adds an object to the related session's receivers.
     * Calls Open() once for the given CObject.
     * NOTICE: After construction the reference count of the
     *         CObject is one, and after adding it to the
     *         receivers, with this method, it is two.
     *
     * @since S60 ?S60_version
     * @param aObject The object to be added. Notice that aObject has to
     *                be closed (destroyed) by user if a leave occurs.
     *                It is not done by this function.
     * @return a handle to refer to this object within the session
     */
    virtual TInt AddObjectL( CObject* aObject ) = 0;
    
    /**
     * Removes an object reference from the related session's
     * receivers.
     *
     * @since S60 ?S60_version
     * @param aHandle a handle to the object reference to be removed
     */
    virtual void RemoveObject( TInt aHandle ) = 0;

    /**
     * Get context information of the session. Not a const-method
     * to allow possible altering of the context through the
     * interface although it is probably never needed.
     *
     * @since S60 ?S60_version
     * @return Context information of the session.
     */
    virtual MCatalogsContext& Context() = 0;
    
    
    virtual const MCatalogsContext* ContextPtr() const = 0;

    };


#endif // M_CATALOGS_SESSION_H
