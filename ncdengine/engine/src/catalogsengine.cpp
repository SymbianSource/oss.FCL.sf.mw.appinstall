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
* Description:   Describes a factory to return a handle to the Catalogs engine
*
*/


#include <e32base.h>
#include <e32property.h>
#include <ecom/ecom.h>

#include "catalogsengine.h"
#include "catalogsdebug.h"
#include "catalogserrors.h"
#include "catalogsconstants.h"
 
#ifndef CATALOGS_ECOM

static void CleanupMutex( TAny* aMutexPtr )
    {
    DLTRACEIN((""));
    RMutex* mutex = static_cast< RMutex* >( aMutexPtr );
    DLINFO(( "Signalling and closing engine mutex" ));
    mutex->Signal();
    mutex->Close();
    }


static void GetMutexLC( RMutex& aMutex )
    {
    DLTRACEIN((""));

    // First try to open the global mutex.
    DLINFO(( "Trying to open engine mutex" ));

    TInt mutexOpenErr = aMutex.OpenGlobal( KCatalogsEngineMutex );
    if( mutexOpenErr == KErrNotFound )
        {
        DLINFO(( "Mutex was not found, trying to create." ));
        mutexOpenErr = aMutex.CreateGlobal( KCatalogsEngineMutex );
        if( mutexOpenErr == KErrAlreadyExists )
            {
            // Someone beat us to it. Try opening again.
            DLINFO(( "Mutex was just created. Trying to open." ));
            mutexOpenErr = aMutex.OpenGlobal( KCatalogsEngineMutex );
            }
        }

    if( mutexOpenErr != KErrNone )
        {
        DLERROR(( "Mutex open failed with %d", mutexOpenErr ));
        DLTRACEOUT(( "LEAVE %d", mutexOpenErr ));
        User::Leave( mutexOpenErr );
        }

    DLINFO(( "Waiting for engine mutex..." ));
    aMutex.Wait();
    DLINFO(( "Got property mutex" ));

    // Push a cleanup item for signalling and closing the mutex.
    CleanupStack::PushL( TCleanupItem( CleanupMutex, &aMutex ) );

    DLTRACEOUT((""));
    }

static TBool IsMaintenanceLockOnL()
    {
    DLTRACEIN((""));

    // Get Catalogs Engine maintenance P&S variable
    DLINFO(( "Reading maintenance lock value" ));
    
    TInt value;
    TInt err = RProperty::Get( 
        KCatalogsEnginePropertyCategory, 
        KCatalogsEnginePropertyKeyMaintenanceLock,
        value );

    if( err == KErrNone )
        {
        DLINFO(( "Maintenance lock value %08x", value ));

        DLTRACEOUT(( "%d", value != 0 ));
        return value != 0;
        }

    else if( err != KErrNotFound )
        {
        DLERROR(( "Maintenance lock read failed with %d", err ));
        DLTRACEOUT(( "LEAVE %d", err )); 
        User::Leave( err );
        }

    // Maintenance P&S variable not set, not in maintenance mode.
    DLTRACEOUT(( "EFalse" ));
    return EFalse;
    }


static void SetMaintenanceLockL( TBool aLock )
    {
    DLTRACEIN(( "Writing %d to maintenance lock P&S", aLock ));
    
    // Write maintenance lock property.
    DLINFO(( "Writing maintenance lock property" ));
    TInt err = RProperty::Set(
        KCatalogsEnginePropertyCategory,
        KCatalogsEnginePropertyKeyMaintenanceLock,
        aLock );

    if( err == KErrNotFound )
        {
        DLINFO(( "Maintenance lock property not found, invoking server to define it" ));
        
        // P&S variable is defined in the server process. Connect to server to
        // accomplish this.
        MCatalogsEngineObserver* nullObserver = NULL;
        CCatalogsEngine* engine = CCatalogsEngine::NewLC( *nullObserver );

        err = engine->Connect( TUid::Uid( 0 ) );

        // Engine no longer needed, close, release and delete.
        DLINFO(( "Connect returned %d. Closing engine object.", err ));
        engine->Close();

        DLINFO(( "Deleting engine object" ));
        CleanupStack::PopAndDestroy( engine );

        if( err == KErrNone )
            {
            // The maintenance lock property should be defined now, try again.
            DLINFO(( "Writing maintenance lock property" ));
            err = RProperty::Set(
                KCatalogsEnginePropertyCategory,
                KCatalogsEnginePropertyKeyMaintenanceLock,
                aLock );
            }
        }

    if( err != KErrNone )
        {
        DLERROR(( "Maintenance lock set failed, error %d", err ));
        DLTRACEOUT(( "LEAVE %d", err ));
        User::Leave( err );
        }

    DLTRACEOUT((""));
    }


EXPORT_C CCatalogsEngine::TState CCatalogsEngine::StateL()
    {
    DLTRACEIN((""));

    RMutex mutex;
    GetMutexLC( mutex );

    TState result = EStateNormal;

    if( IsMaintenanceLockOnL() )
        {
        // Either EStatePrepareMaintenance or EStateMaintenance.
        DLINFO(( "Maintenance lock is on." ));

        // Check if engine instances exist. This can be determined by mutex usage:
        // all existing instances hold a handle to the mutex. If there are handles
        // in addition to the one held here, other instances exist.
        THandleInfo mutexHandleInfo;
        mutex.HandleInfo( &mutexHandleInfo );
        
        DLINFO(( "Mutex handle in-process count %d, process count %d",
            mutexHandleInfo.iNumOpenInProcess, mutexHandleInfo.iNumProcesses ));
        
        if( mutexHandleInfo.iNumOpenInProcess > 1 || 
            mutexHandleInfo.iNumProcesses > 1 )
            {
            result = EStatePrepareMaintenance;
            }
        else
            {
            result = EStateMaintenance;
            }
        }

    CleanupStack::PopAndDestroy();  // mutex Signal() and Close()
    DLTRACEOUT(( "%d", result ));
    return result;
    }


/**
 * Instantiates the catalogs engine, using the maintenance lock to determine
 * if instantiation is allowed.
 */
EXPORT_C CCatalogsEngine* CCatalogsEngine::NewLC( MCatalogsEngineObserver& aObserver )
    {
    DLTRACEIN(( "aObserver=%08x", &aObserver ));

    // Get/create Catalogs Engine maintenance lock mutex
    RMutex mutex;
    GetMutexLC( mutex );

    if( IsMaintenanceLockOnL() )
        {
        DLWARNING(( "Catalogs Engine in maintenance mode, cannot create engine instance" ));
        DLTRACEOUT(( "LEAVE KCatalogsErrorMaintenanceMode" ));
        User::Leave( KCatalogsErrorMaintenanceMode );
        }

    // Not in maintenance mode, ok to instantiate engine class. Do this before releasing the
    // mutex, as this may define the maintenance lock P&S.
    DLINFO(( "Creating ECom object with UID %08x", KCCatalogsEngineImplUid.iUid ));
    CCatalogsEngine* object = 0;
    TRAPD( err,
        {
        object = reinterpret_cast< CCatalogsEngine* >( 
            REComSession::CreateImplementationL( 
                KCCatalogsEngineImplUid, _FOFF(CCatalogsEngine,iDtor_ID_Key), &aObserver ) );
        }); //TRAPD
    DLINFO(("ECom creation err=%d",err));
    User::LeaveIfError( err );
    
    DLINFO(( "ECom returned pointer %08x", object ));
    DASSERT( object != NULL );

    CleanupStack::PopAndDestroy(); // mutex signal & close
    CleanupStack::PushL( object );

    DLTRACEOUT(( "%08x", object ));
    return object;
    }


EXPORT_C CCatalogsEngine* CCatalogsEngine::NewL( MCatalogsEngineObserver& aObserver )
    {
    DLTRACEIN(( "aObserver=%08x", &aObserver ));
    CCatalogsEngine* engine = CCatalogsEngine::NewLC( aObserver );
    CleanupStack::Pop();
    DLTRACEOUT(( "%08x", engine ));
    return engine;
    }

EXPORT_C void CCatalogsEngine::StartMaintenanceL()
    {
    DLTRACEIN((""));

    // Get Catalogs Engine maintenance lock mutex
    RMutex mutex;
    GetMutexLC( mutex );

    if( IsMaintenanceLockOnL() )
        {
        DLERROR(( "Engine already locked for maintenance mode" ));
        DLTRACEOUT(( "LEAVE KErrAlreadyExists" ));
        User::Leave( KErrAlreadyExists );
        }

    // Set the maintenance lock. This will send a P&S event to any existing
    // engine instances and create a callback for the clients.
    SetMaintenanceLockL( ETrue );

    CleanupStack::PopAndDestroy();  // mutex signal & close
    }

EXPORT_C void CCatalogsEngine::EndMaintenanceL()
    {
    // Get Catalogs Engine maintenance lock mutex
    DLTRACEIN((""));
    
    RMutex mutex;
    GetMutexLC( mutex );

    if( !IsMaintenanceLockOnL() )
        {
        DLERROR(( "Maintenance lock not on" ));
        DLTRACEOUT(( "LEAVE KCatalogsErrorMaintenanceNotStarted" ));
        User::Leave( KCatalogsErrorMaintenanceNotStarted );
        }
    
    SetMaintenanceLockL( EFalse );

    CleanupStack::PopAndDestroy();  // mutex signal & close
    }

#endif // CATALOGS_ECOM

/**
 * Destructor
 */
CCatalogsEngine::~CCatalogsEngine()
    {
    DLTRACEIN(( "this=%08x", this ));
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    // Is it ok to say FinalClose() even if engine object instances exist? Should be, hope so.
    // FinalClose not okay, from help: "It must never be called from within a plugin class destructor."
    //DLTRACE(("FinalClose()"));
    
    //REComSession::FinalClose();
    DLTRACEOUT(( "this=%08x", this ));
    }

/**
 * Constructor
 */
CCatalogsEngine::CCatalogsEngine()
    {
    }
