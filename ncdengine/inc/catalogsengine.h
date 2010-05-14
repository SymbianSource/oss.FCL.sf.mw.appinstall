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
* Description:   Declaration of MCatalogsEngine
*
*/


#ifndef CATALOGS_ENGINE_H
#define CATALOGS_ENGINE_H

#include <e32base.h>

#include "catalogsbase.h"

class MCatalogsEngineObserver;
class CCatalogsShutdown;

/**
 *  An interface providing the main handle to Catalogs engine
 *
 *  This class offers functionality for provider creation
 *
 *  
 */
class MCatalogsEngine : public virtual MCatalogsBase
    {

public:


    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ECatalogsEngineUid };


    /**
     * Connect to the Catalogs engine. Content and services provided by
     * the engine depend on the client application's UID given as a parameter.
     * 
     * @note Once opened, the connection must be closed with MCatalogsEngine::Close()
     *  before the engine object can be released. Panic
     *  ECatalogsPanicSessionNotClosed will be raised if the connection is still
     *  open when the engine object is released.
     *
     * 
     * @param aClientUid Client identifier, used for content access control.
     * @return System wide error code.
     * @see MCatalogsEngine::Close
     */
    virtual TInt Connect( TUid aClientUid ) = 0;


    /**
     * Close connection to the Catalogs engine.
     *
     * @note All references to Catalogs Engine objects must be released before closing
     *  the session.
     *
     * 
     * @see MCatalogsEngine::Connect
     * @exception ECatalogsPanicUnreleasedReferencesExist Raised if unreleased references
     *  still exist to objects within the Catalogs Engine.
     */
    virtual void Close() = 0;


    /**
     * Requests a provider from the Catalogs engine. Specific provider
     * implementations are identified by UIDs.
     * Currently only one provider is supported:
     * NCD provider (uid=0x20008013)
     *
     * 
     * @param aUid Unique identifier of the provider that should be created.
     * @param aProvider Set to point to the newly created provider object. Counted, Release()
     *  must be called after use.
     * @param aStatus Status variable used to notify the completion of provider creation.
     * @param aProviderOptions Provider spesific options
     * @see MNcdProvider
     * @see ncdprovideroptions.h
     * @exception Leave System wide error code.
     * @see ncderrors.h For possible provider creation complition codes other
     * than KErrNone
     *
     * @note NCD provider uses Download manager in master mode with the connecting client's
     * SID. This prevents the client from using Download manager with it's own SID in
     * master mode. If the client needs to use Download manager in master mode, it must
     * connect to it with some other UID than its own SID.
     * 
     * @note aProviderOptions has effect only during the first CreateProviderL call
     * so if the client calls CreateProviderL twice during its execution, the
     * provider will be unaffected by the second call.
     */    
    virtual void CreateProviderL( 
        TInt aUid, 
        MCatalogsBase*& aProvider, 
        TRequestStatus& aStatus, 
        TUint32 aProviderOptions ) = 0;

    /**
     * Cancels provider creation.
     *
     * @note Completes the ongoing provider creation immediately
     *       with KErrCancel.
     *
     * 
     */
    virtual void CancelProviderCreation() = 0;


protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MCatalogsEngine() {}

    };


/**
 *  A factory interface class for creating an instance of Catalogs Engine API.
 *
 *  @lib ncdengine_20019119.lib
 *  
 */
class CCatalogsEngine : public CBase, public MCatalogsEngine
    {

public:

#ifndef CATALOGS_ECOM

    /**
     * Catalogs Engine state.
     *
     * 
     */
    enum TState
        {
        /** Normal state, engine object instantiation allowed */
        EStateNormal,

        /** 
         * Going to maintenance mode. Engine instantiation not allowed, but
         * instances still exist.
         */
        EStatePrepareMaintenance,

        /** 
         * Maintenance mode. No Catalogs Engine instances exist, new ones
         * not allowed.
         */
        EStateMaintenance

        };

    /**
     * Returns the Catalogs Engine state.
     *
     * @return Catalogs Engine state.
     */
    IMPORT_C static TState StateL();


    /**
     * Instantiates the catalogs engine.
     *
     * @param aObserver Observer interface for the Catalogs Engine.
     * @return CCatalogsEngine* The instantiated object.
     */
    IMPORT_C static CCatalogsEngine* NewL( MCatalogsEngineObserver& aObserver );


    /**
     * Instantiates the catalogs engine, leaves pointer on cleanup stack.
     *
     * @param aObserver Observer interface for the Catalogs Engine.
     * @return CCatalogsEngine* The instantiated object.
     */
    IMPORT_C static CCatalogsEngine* NewLC( MCatalogsEngineObserver& aObserver );


    /**
     * Starts engine maintenance. All clients with open references to the engine will
     * receive a callback (MCatalogsEngineObserver::CatalogsEngineShutdown()) requesting
     * release of engine objects. Also, further engine instantiations are refused for
     * other clients until EndMaintenanceL() is called by the client that initiated the
     * update in the first place.
     *
     * @note The client calling StartMaintenanceL() may itself receive a callback for engine
     *  shutdown if it has open references to the Catalogs Engine. In this case, the client
     *  must obey the callback, like all other clients, and release the references.
     *
     * @note Reaching maintenance state can be observed by polling engine state with State().
     *
     * @exception KCatalogsErrorMaintenanceNotAuthorized Client is not authorized to
     *  start maintenance on engine.
     * @exception Leave System wide error code.
     */
    IMPORT_C static void StartMaintenanceL();


    /**
     * Ends engine maintenance. Engine usage is allowed.
     *
     * @exception KCatalogsErrorMaintenanceNotStarted Client has not started maintenance.
     * @exception Leave System wide error code.
     */
    IMPORT_C static void EndMaintenanceL();

#endif // CATALOGS_ECOM

    /**
     * Destructor.
     */
    virtual ~CCatalogsEngine();

protected:

    /**
     * Default constructor
     */
    CCatalogsEngine();

private:

    // Unique instance identifier key for ECom.                    
    TUid iDtor_ID_Key;

    };

#endif // CATALOGS_ENGINE_H
