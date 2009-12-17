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
* Description:   Declaration of CCatalogsEngineImpl
*
*/


#ifndef C_CATALOGS_ENGINE_IMPL_H
#define C_CATALOGS_ENGINE_IMPL_H

#include <e32base.h>

#include "catalogsengine.h"
#include "catalogsclientserverclientsession.h"
#include "catalogsbaseimpl.h"

// Forward declaration                        
class CNcdProviderProxy;
class MCatalogsEngineObserver;
class CCatalogsProviderCreator;
class CCatalogsShutdownObserver;
class CCatalogsUpdateObserver;
class CCatalogsConnectionObserver;

/**
 *  An interface providing the main handle to Catalogs engine
 *
 *  This class offers functionality for provider creation
 *
 *  @lib ncdproxy_20019119.dll
 *  @since S60 v3.2
 */

// Cannot use CCatalogsBase<> base class because class declaration must match CCatalogsEngine
// ECom interface.
class CCatalogsEngineImpl : public CCatalogsEngine
    {

public:

    /**
     * Instantiates the catalogs engine.
     *
     * @param aObserver Observer for engine callbacks.
     * @return CCatalogsEngineImpl* The instantiated object.
     */
    static CCatalogsEngineImpl* NewL( TAny* aInitParams );


    /**
     * Destructor
     */
    virtual ~CCatalogsEngineImpl();

  
    /**
     * Connect to the Catalogs engine
     * 
     * @since S60 v3.2
     * @return System wide error code.
     */
    TInt Connect( TUid aClientUid );

    /**
     * Close connection to the Catalogs engine
     *
     * @since S60 v3.2
     */
    void Close();

    /**
     * Requests a provider from the Catalogs engine. Spesific provider types are
     * identified by UIDs.
     *
     * @since S60 v3.2
     * @param aUid Type of the provider that should be created.
     *             (integer containing a hexadecimal number)
     * @param aStatus 
     * @param aProviderOptions Provider spesific options
     * @return Pointer to the created provider. The pointer should be cast by
     *         the user of the interface to match the requested provider type.
     * @exception System wide error code.
     * 
     */    
    void CreateProviderL( 
        TInt aUid, 
        MCatalogsBase*& aProvider, 
        TRequestStatus& aStatus, 
        TUint32 aProviderOptions );

    /**
     * Cancels provider creation.
     *
     * @note Completes the ongoing provider creation immediately
     *       with KErrCancel.
     *
     * @since S60 v3.2
     */
    void CancelProviderCreation();

    /**
     * Starts engine maintenance. All clients with open sessions to the engine will
     * receive a callback requesting closing of engine sessions. Also, further
     * connections are refused until EndMaintenanceL() is called.
     *
     * A state change callback is received to engine observer when the server is ready for
     * maintenance (no open connections).
     *
     * @note After calling StartMaintenanceL(), the client may itself receive a
     *  CatalogsEngineShutdown() callback to its observer, in case it has open sessions
     *  to the engine.
     *
     * @exception KErrAccessDenied Client is not authorized to start maintenance
     *  on engine.
     * @exception Leave System wide error code.
     */
//    virtual void StartMaintenanceL();


    /**
     * Ends engine maintenance. Connections to engine are allowed.
     *
     * @exception KErrAccessDenied Client is not authorized to end maintenance
     *  on engine.
     * @exception KErrNotFound Client has not started maintenance.
     * @exception Leave System wide error code.
     */
//    virtual void EndMaintenanceL();

/*
public: // from CActive

    void RunL();
    void DoCancel();
*/

public: // from MCatalogsBase

    TInt AddRef() const;
    TInt Release() const;
    
protected: // from MCatalogsBase
    const TAny* QueryInterfaceL( TInt aInterfaceType ) const;

private:
                                          
    /**
     * Constructor, private
     * @param aObserver Observer for engine callbacks.
     */
    CCatalogsEngineImpl( MCatalogsEngineObserver& aObserver );

    /**
     * 2nd phase constructor, private
     */
    void ConstructL();


private: // data

    mutable TInt iRefCount;

    /**
     * Uid of the client.
     */
    TUid iClientUid;

    /**
     * Handle to the Catalogs engine.
     */
    RCatalogsClientServerClientSession iCatalogsEngine;

    /**
     * Engine observer. 
     * Not own.
     */
    MCatalogsEngineObserver* iObserver;

    /**
     * Provider creator active object. Pwned.
     */
    CCatalogsProviderCreator* iProviderCreator;
    
    /**
     * Catalogs engine mutex.
     */
    RMutex iCatalogsMutex;

    /** Catalogs maintenance lock observer. */
    CCatalogsShutdownObserver* iShutdownObserver;
    
    /** Catalogs OTA update observer */
    CCatalogsUpdateObserver* iUpdateObserver;

    CCatalogsConnectionObserver* iConnectionObserver;
    };

#endif // C_CATALOGS_ENGINE_IMPL_H
