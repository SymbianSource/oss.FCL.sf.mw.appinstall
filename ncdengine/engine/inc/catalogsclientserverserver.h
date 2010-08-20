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
* Description:   Class CCatalogsClientServerServer declaration
*
*/


#ifndef C_CATALOGS_CLIENT_SERVER_SERVER_H
#define C_CATALOGS_CLIENT_SERVER_SERVER_H

#include <e32base.h>

#include "catalogsshutdownoperation.h"

class CCatalogsServerEngine;
class MCatalogsSession;
class MCatalogsContext;

class CCatalogsClientServerServer : 
    public CServer2, 
    public MCatalogsShutdownOperationObserver
    {
public:

    /**
    * Static constructor, leaves pointer on cleanup stack
    */
    static CCatalogsClientServerServer* NewL();    

    /**
     * Static constructor, leaves pointer on cleanup stack
     */
    static CCatalogsClientServerServer* NewLC();

    /**
     * Destructor
     */
    virtual ~CCatalogsClientServerServer();

    /**
     * Main function for the server thread.
     * 
     * @since S60 ?S60_version
     * @param aNone  Not used.
     * @return System wide error code.
     */
    static TInt ThreadFunction( TAny* aNone );

    /**
     * Increments the count of the active sessions for this server.
     *
     * @since S60 ?S60_version
     */
    void IncrementSessions();

    /**
     * Decrements the count of the active sessions for this server.
     *
     * @since S60 ?S60_version
     */
    void DecrementSessions();
 
    /**
     * Informs objects of session removal. They should not use
     * given session in anyway anymore
     *
     * @since S60 ?S60_version
     * @param aSession Session that is not in use anymore.
     */   
    void HandleSessionRemoval( MCatalogsSession& aSession );

    /**
     * Create a new object container. Called by sessions.
     *
     * @since S60 ?S60_version
     * @return Created object container.
     */
    CObjectCon* NewContainerL();

    /**
     * Removes an object container. Called by sessions.
     *
     * @since S60 ?S60_version
     * @param aCon Container that should be removed.
     */
    void RemoveContainer( CObjectCon* aCon );


    /**
     * Adds an object to container. These objects are objects
     * that communicate to client-side. Called by sessions. Leaves
     * on error, but not if the object is tried to add for a second
     * time into the container. Then simply does not do anything.
     *
     * @since S60 ?S60_version
     * @param aObject Object that is communicable.
     */    
    void AddObjectToContainerL( CObject* aObject );      
    
    /**
     * Function that conveys provider creation request to the
     * engine.
     * 
     * @since S60 ?S60_version
     * @param aSession Interface to register objects.
     * @param aProviderIdentifier Int (uid) to identify desired provider
     * @param aHandle Reference to an integer which is used to
     *                       pass the handle to the created provider back.
     * @param aProviderOptions Provider options
     */    
    void CreateProviderL( MCatalogsSession& aSession, 
                         TInt aProviderIdentifier,
                         TInt& aHandle,
                         TUint32 aProviderOptions );
                         
    TInt NewInstanceId();
    
    /**
     * Adds a shutdown operation to the server's operation queue
     * The operation is deleted if appending fails
     * 
     * @return Symbian error code
     */
    static TInt AddShutdownOperation( CCatalogsShutdownOperation* aOperation );
    
    /**
     * Executes/cancels shutdown operations for the given client
     * 
     * @param aContext
     * @param aExecute If ETrue, shutdown operations are executed, 
     * if false they are canceled
     */
    void OperateShutdownOperations( 
        const MCatalogsContext& aContext,
        TBool aExecute );

    
protected: // MCatalogsShutdownOperationObserver

    
    void ShutdownOperationComplete( 
        CCatalogsShutdownOperation* aOperation, 
        TInt aError );
    

protected:  // Methods from CActive

// from base class CActive    

    /**
     * From CActive
     * @see CActive::RunError
     *
     * @since S60 ?S60_version
     */
    TInt RunError( TInt aError );
            
private:
    
    /**
     * Constructor
     * 
     * @param aPriority Priority for this thread.
     */
    CCatalogsClientServerServer( TInt aPriority );

    /**
     * Second phase construct.
     */
    void ConstructL();

    /**
     * Second stage startup for the server thread.
     *
     * @since S60 ?S60_version
     */
    static void ThreadFunctionL();


// from base class CServer    

    /**
     * From CServer
     * @see CServer::NewSessionL
     *
     * @since S60 ?S60_version
     */
    CSession2* NewSessionL( const TVersion& aVersion, 
                            const RMessage2& aMsg ) const;

private:
    
    static CCatalogsClientServerServer* iCatalogsServer;
    
    /**
     * Amount of sessions.
     */
    TInt iSessionCount;
    
    /**
     * Server-side engine
     * Own.
     */    
    CCatalogsServerEngine* iEngine;
    
    
    /**
     * Object container index
     * Own.
     */
	CObjectConIx* iContainerIndex;
	
    /**
     * Container of objects that are registered as objects which
     * communicate to client-side.
     * Own.
     */
    CObjectCon* iContainer;

#ifdef CATALOGS_UNDERTAKER

    RProcess    iUndertakerProcess;

#endif
    
    /**
     * Counter, used for identifying session instances.
     */
    TInt iSessionInstanceIdCounter;
    
    RPointerArray<CCatalogsShutdownOperation> iShutdownOperations;
    };

#endif // C_CATALOGS_CLIENT_SERVER_SERVER_H

