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
* Description:  
*
*/


#ifndef C_CATALOGS_COMMUNICABLE_H
#define C_CATALOGS_COMMUNICABLE_H

#include <e32base.h>

class MCatalogsBaseMessage;
class MCatalogsSession;

/**
 *  A class that should be inherited by an object
 *  receiving messages
 *
 *  A server side object that is assigned to receive messages
 *  should derive from this class. ReceiveMessageL method should
 *  be implemented to define message handling.
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsCommunicable : public CObject
    {

public:

    /**
     * A function that is called when a message is passed
     * to an instantiation of an implementing class. The implementation
     * should define message handling.
     * NOTICE: This function should return as soon as possible
     *         so execution of ClientServer can continue freely and new 
     *         messages are forwarded to receivers using this function.
     * NOTICE: This function receives ownership of the message and it
     *         should be released using completion-functions or release-
     *         function. (Also in error situations)
     *
     * @since S60 ?S60_version
     * @param aMessage message sent
     * @param aFunctionNumber Number for receiver to use as desired. Usually
     *                        used to pass the message to correct function in
     *                        the receiver.
     */
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber ) = 0;

    /**
     * When session has died and a object of this class is still
     * registered as a message receiver to that session, then this
     * function is used to inform the object that the session
     * should not be used anymore.
     * This means that MCatalogsBaseMessage-typed messages should
     * not be completed anymore. (Use Release instead.)
     *
     * @since S60 ?S60_version
     * @param aSession Reference to the session that has died.
     *                 This parameter is given only to identify
     *                 which session has died. Do not use it.
     */
    virtual void CounterPartLost(
        const MCatalogsSession& aSession ) = 0;

    };

#endif // C_CATALOGS_COMMUNICABLE_H

