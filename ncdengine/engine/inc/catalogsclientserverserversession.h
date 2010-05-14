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
* Description:   Class CCatalogsClientServerServerSession declaration
*
*/


#ifndef C_CATALOGS_CLIENT_SERVER_SERVER_SESSION_H
#define C_CATALOGS_CLIENT_SERVER_SERVER_SESSION_H

#include <e32base.h>

#include "catalogsclientserverserver.h"
#include "catalogssession.h"

class CCatalogsCommunicable;
class CCatalogsIncompleteMessage;
class CCatalogsContextImpl;

/**
 *  Describes a server side session.
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsClientServerServerSession : 
    public CSession2,
    public MCatalogsSession
    {
public:

    /**
     * Used to push CObjects to Cleanup stack
     */
    static void CleanupCObjectBasedClass( TAny* aCObject );
        
public:

    /**
     * Static constructor.
     * 
     * @since S60 ?S60_version
     * @param aClient Client's thread.
     * @param aServer The server.
     * 
     * @return Pointer to the created session.
     */
    static CCatalogsClientServerServerSession* NewL(
        RThread& aClient,
        CCatalogsClientServerServer& aServer );

    /**
     * Static constructor, leaves pointer on cleanup stack.
     * 
     * @since S60 ?S60_version
     * @param aClient Client's thread.
     * @param aServer The server.
     * 
     * @return Pointer to the created session.
     */
    static CCatalogsClientServerServerSession* NewLC( 
        RThread& aClient, 
        CCatalogsClientServerServer& aServer );

    /**
     * Destructor.
     */
    virtual ~CCatalogsClientServerServerSession();

    /**
     * Function that informs possible message receivers
     * that client-side has died and deletes references
     * to possible message receivers also deleting ixes.
     * Deletes also incomplete messages. Informs server
     * that client-side session is down.
     * 
     * @since S60 ?S60_version
     */     
    void ClientSideSessionDown();

    /**
     * Function that creates incomplete message object that is
     * used to complete the message send later.
     * 
     * @since S60 ?S60_version
     * @param aOutputData Return descriptor that should be written
     *                    to client side.
     * @param aStatus Complete value for the message that should be
     *                used when completing the message later.
     * @return Handle to the incomplete message.
     */    
    TInt WriteToLargerDesL( const TDesC8& aOutputData, TInt aStatus );

    /**
     * Function that creates incomplete message object that is
     * used to complete the message send later.
     * 
     * @since S60 ?S60_version
     * @param aOutputData Return descriptor that should be written
     *                    to client side.
     * @param aStatus Complete value for the message that should be
     *                used when completing the message later.
     * @return Handle to the incomplete message.
     */    
    TInt WriteToLargerDesL( const TDesC16& aOutputData, TInt aStatus );

    /**
     * Uses handle to remove an incomplete message
     *
     * @since S60 ?S60_version
     * @param aHandle Handle to the message
     */
    void RemoveIncompleteMessage( TInt aHandle );
  

public:
    
// from base class CSession
    
    /**
     * From CSession.
     * Service request from client.
     * 
     * @since S60 ?S60_version
     * @param aMessage Message from client.
     */
    void ServiceL( const RMessage2& aMessage );

    
// from base class MCatalogsSession


    /**
     * From MCatalogsSession.
     * Used to add object to possible receivers of messages in this
     * session. Calls Open() once for the given CObject.
     * NOTICE: After construction the reference count of the
     *         CObject is one, and after adding it to the
     *         receivers, it is two.
     *
     * @since S60 ?S60_version
     * @param aObject Pointer to a object that should be added to
     *                receiver-list.
     * @return Handle to the added object.
     */
    TInt AddObjectL( CObject* aObject );

    /**
     * From MCatalogsSession.
     * Used to remove object from possible receivers of messages
     * in this session. Closes once the CObject into which the
     * handle points.
     *
     * @since S60 ?S60_version
     * @param aHandle Handle to the object that should be removed.
     */    
    void RemoveObject( TInt aHandle );

    /**
     * From MCatalogsSession.
     * Get context information of the session.
     *
     * @since S60 ?S60_version
     * @return Context information of the session.
     */
    MCatalogsContext& Context();


    const MCatalogsContext* ContextPtr() const;
    
    
private:
    
    /**
     * Constructor.
     * 
     * @param aClient Client's thread.
     * @param aServer The server.
     * @param aInstanceId Instance id of the session.
     */
    CCatalogsClientServerServerSession( 
        RThread& aClient,
        CCatalogsClientServerServer& aServer,
        TInt aInstanceId );

    /**
     * Second phase construct.
     */
    void ConstructL() ;


    /**
     * Function that handles and forwards received message.
     * 
     * @since S60 ?S60_version
     * @param aMessage Message from client.
     */
    void HandleMessageL( const RMessage2& aMessage );

    /**
     * Function that handles and forwards received message that
     * is pointed to a external receiver (not a internal message).
     * 
     * @since S60 ?S60_version
     * @param aMessage Message from client.
     */
    void HandleExternalMessageL( const RMessage2& aMessage );

    /**
     * Function that handles received internal message. 
     * @since S60 ?S60_version
     * @param aMessage Message from client.
     */
    void HandleInternalMessageL( const RMessage2& aMessage );

    /**
     * Function used to create a provider.
     *
     * @since S60 ?S60_version
     * @param aMessage Message containing information needed in the creation
     */
    void CreateProviderL( const RMessage2& aMessage );

    /**
     * Function used to create context-object for this session.
     *
     * @since S60 ?S60_version
     * @param aMessage Message containing information needed in the creation
     */
    void CreateContextL( const RMessage2& aMessage );

    /**
     * Panics client.
     *
     * @since S60 ?S60_version
     * @param aMessage Message that caused the panic
     * @param aPanic Panic that was caused
     */    
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

    /**
     * Uses handle to get a pointer to the receiver of a message
     *
     * @since S60 ?S60_version
     * @param aHandle Handle to the receiver
     * @return Pointer to the receiver
     */
    CCatalogsCommunicable* ReceiverFromHandle( TInt aHandle );

    /**
     * Uses handle to get a pointer to an incomplete message
     *
     * @since S60 ?S60_version
     * @param aHandle Handle to the message
     * @return Pointer to the message
     */
    CCatalogsIncompleteMessage* IncompleteMessageFromHandle( TInt aHandle );

    /**
     * Function that goes through message receiver ix and 
     * informs of the still registered objects that they
     * should not try to send messages trough this session
     * anymore.
     * 
     * @since S60 ?S60_version
     */ 
    void CloseMessageHandlers();

private:


    /**
     * Variable to keep track whether we already have
     * announced that client-side session is closing
     * down or not.
     */
    enum
        {
        ENotInformed,
        EInformed
        } iClientSideDownAnnounced;

    /**
     * Object that contains context-information of the client and
     * needed functionality related to it.
     * Own.
     */
    CCatalogsContextImpl* iContext;

    /**
     * Reference to the server that owns this session
     */
    CCatalogsClientServerServer& iServer;
    
    /**
     * Object index used to store references to the receiver objects
     * Own.
     */
    CObjectIx* iMessageHandlers;
    
    /**
     * Object index used to store references to the incomplete messages
     * Own.
     */    
    CObjectIx* iIncompleteMessages;
    /**
     * Container of incompletemessage-objects.
     * Own.
     */    
    CObjectCon* iIncompleteMessageContainer;
    
    /**
     * Instance id.
     */
    TInt iInstanceId;
    };

#endif // C_CATALOGS_CLIENT_SERVER_SERVER_SESSION_H

