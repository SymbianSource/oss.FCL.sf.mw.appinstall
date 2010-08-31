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
* Description:   Implementation of CCatalogsClientServerServer
*
*/

#include <QCoreApplication>

#include "catalogsclientserverserver.h"
#include "catalogsclientserverserversession.h"
#include "catalogsserverdefines.h"
#include "catalogsserverengine.h"
#include "catalogscontext.h"

#include "catalogsdebug.h"

CCatalogsClientServerServer* CCatalogsClientServerServer::iCatalogsServer = NULL;

// Replaced with USE_BUILD_SCRIPT when using build script
#define DUMMY_DEFINE

#ifdef CATALOGS_UNDERTAKER
#ifdef USE_BUILD_SCRIPT
_LIT( KCatalogsUndertakerFilename, "ncdundertaker_APP_NAME_POSTFIX" );
#else
_LIT( KCatalogsUndertakerFilename, "ncdundertaker_20019119" );
#endif // USE_BUILD_SCRIPT
#endif

// Capability set required from clients. Consists of CAP_APPLICATION capability set.
static _LIT_SECURITY_POLICY_C7(
    KCatalogsServerConnectPolicy1,
    ECapabilityNetworkServices,
    ECapabilityLocalServices,
    ECapabilityLocation,
    ECapabilityReadUserData,
    ECapabilityWriteUserData,
    ECapabilityReadDeviceData,
    ECapabilityWriteDeviceData );
    
static _LIT_SECURITY_POLICY_C2(
    KCatalogsServerConnectPolicy2,
    ECapabilitySwEvent,
    ECapabilityUserEnvironment );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CCatalogsClientServerServer* CCatalogsClientServerServer::NewL()
    {
    CCatalogsClientServerServer* self = CCatalogsClientServerServer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CCatalogsClientServerServer* CCatalogsClientServerServer::NewLC()
    {
    CCatalogsClientServerServer* self = new( ELeave )
        CCatalogsClientServerServer( CActive::EPriorityLow - 20 );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CCatalogsClientServerServer::CCatalogsClientServerServer( TInt aPriority ) :
    CServer2( aPriority )
    {
    iCatalogsServer = this;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CCatalogsClientServerServer::~CCatalogsClientServerServer()
    {
    DLTRACEIN((""));
    // Usually at this point all sessions are already destroyed.
    // If not, we have to go through all sessions and destroy
    // all references to the objects to which references might be lost
    // if the engine was destroyed first.
    // Also when doing it this way when the engine is destroyed,
    // provider can do the final killing of those objects
    iSessionIter.SetToFirst();
    CSession2* baseSession = NULL;
    CCatalogsClientServerServerSession* session = NULL;
    baseSession = iSessionIter++;
    while ( baseSession != NULL )
        {
        DLTRACE(("Calling ClientSideSessionDown"));
        // When going down, act as the client side would have died
        // ->call ClientSideSessionDown for each session
        session = 
            static_cast<CCatalogsClientServerServerSession*>( baseSession );
        session->ClientSideSessionDown();
        baseSession = iSessionIter++;
        DLTRACE(("ClientSideSessionDown called"));
        }
    
    DLTRACE(("Delete engine"));
    // This is destroyed before destroying container, so possible
    // resources are freed in its destructor and from the container 
    // in here.
    // Other way would end up destroying already destroyed stuff.
    delete iEngine;

    DLTRACE(("Engine deleted"));
    iShutdownOperations.ResetAndDestroy();
    
    // If container is going to be removed, it has to be done before
    // it is destroyed by destroying the container index.
    if ( iContainer != NULL )
        {
        DLTRACE(("Remove container"));
        RemoveContainer( iContainer );        
        DLTRACE(("Container removed"));
        }
    iContainer = NULL;
    
    DLTRACE(("Deleting container index"));
    delete iContainerIndex;

#ifdef CATALOGS_UNDERTAKER

    iUndertakerProcess.Close();

#endif 

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::ConstructL()
    {
    DLTRACEIN((""));
    User::LeaveIfError( User::RenameThread( KCatalogsServerName ) );
#ifdef CATALOGS_UNDERTAKER

    DLINFO(( "Creating undertaker process" ));
    TInt err = iUndertakerProcess.Create( KCatalogsUndertakerFilename, KNullDesC() );
    if( err != KErrNone )
        {
        DLERROR(( "Creating undertaker process failed with %d", err ));
        }
    iUndertakerProcess.SetPriority( (TProcessPriority)450 );  // EPriorityHigh
    
    RThread thisThread;
    DLINFO(( _L("Setting undertaker parameter 15 as %S"), &thisThread.FullName() ));
    
    err = iUndertakerProcess.SetParameter( 15, thisThread.FullName() );
    if( err != KErrNone )
        {
        DLERROR(( "Parameter setting failed with %d", err ));
        }
    thisThread.Close();
    iUndertakerProcess.Resume();

#endif

    iContainerIndex = CObjectConIx::NewL();
    iContainer = NewContainerL();

	iEngine = CCatalogsServerEngine::NewL();
    // Add the server to the active scheduler
    StartL( KCatalogsServerName );
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::IncrementSessions()
    {
    iSessionCount++;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::DecrementSessions()
    {
    iSessionCount--;

    // No more clients, shutdown
    if ( iSessionCount <= 0 )
        {
        QCoreApplication::exit(0);
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::HandleSessionRemoval( 
    MCatalogsSession& aSession )
    {
    DASSERT( iEngine );
    
    // We don't want to go further unless the context has been created
    if ( aSession.ContextPtr() )
        {
        iEngine->HandleSessionRemoval( aSession );
        // Execute shutdown operations
        OperateShutdownOperations( aSession.Context(), ETrue );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CObjectCon* CCatalogsClientServerServer::NewContainerL()
	{
    // Return a new object container
	return iContainerIndex->CreateL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::RemoveContainer( CObjectCon* aCon )
	{
	DASSERT( iContainerIndex );
	iContainerIndex->Remove( aCon );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::CreateProviderL( 
    MCatalogsSession& aSession, 
    TInt aProviderIdentifier,
    TInt& aHandle,
    TUint32 aProviderOptions )
    {
    DLTRACEIN((""));
    
    iEngine->CreateProviderL( aSession,
                              aProviderIdentifier, 
                              aHandle,
                              aProviderOptions );

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CCatalogsClientServerServer::NewInstanceId()
    {
    DLTRACEIN((""));
    return iSessionInstanceIdCounter++;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::AddObjectToContainerL( CObject* aObject )
    {    
    // If possible, change trap-implementation to something else.
    // This is just a simple way to do this.
    TRAPD( err, iContainer->AddL( aObject ) );
    DLTRACE(("err: %d", err ));
    if ( err == KErrAlreadyExists || err == KErrNone )
        {        
        return;
        }
    else
        {
        User::Leave( err );
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CCatalogsClientServerServer::RunError( TInt aError )
    {
    DLTRACEIN(( "aError=%d", aError ));    

    // Complete the message being processed, although the state of
    // the server is undetermined
    DLERROR(( "Completing last message with %d", aError ));
    Message().Complete( aError );

    DLTRACEOUT(("KErrNone"));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CSession2* CCatalogsClientServerServer::NewSessionL( 
    const TVersion& aVersion,
    const RMessage2& aMsg ) const
    {
    DLTRACEIN((""));
        
    // Check whether the server is compatible with the 
    // requested version
    if ( !User::QueryVersionSupported(
           TVersion(
               KCatalogsServerMajorVersionNumber,
               KCatalogsServerMinorVersionNumber,
               KCatalogsServerBuildVersionNumber
               ),
           aVersion))
        {        
        User::Leave( KErrNotSupported );
        }

    DLINFO(("Checking client capabilities"));
    // Check the client for required capabilities
    if (!KCatalogsServerConnectPolicy1().CheckPolicy(
        aMsg, __PLATSEC_DIAGNOSTIC_STRING("CCatalogsClientServerServer::NewSessionL, KCatalogsServerConnectPolicy1") ) ||
        !KCatalogsServerConnectPolicy2().CheckPolicy(
        aMsg, __PLATSEC_DIAGNOSTIC_STRING("CCatalogsClientServerServer::NewSessionL, KCatalogsServerConnectPolicy2" ) ) )
        {
        DLINFO(("Client capability check failed"));
        User::Leave( KErrPermissionDenied );
        }

    // Context is not created here for the session, because
    // user given uid cannot be received by this function from
    // the client-side and it is nicer to create the whole
    // context-object in one place.

    DLTRACEOUT((""));
        
    // Create a new session
    RThread client;
    return CCatalogsClientServerServerSession::NewL(
        client,
        *const_cast<CCatalogsClientServerServer*>( this ) );
    }


/**
 * Platform dependant entry points
 */

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
	{
	// WINS DLL entry-point. Just return the real thread function 
    // cast to TInt
	return 0;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
int main(int argc, char *argv[])
    {
    DLINIT;
    DCHECK_HEAP
    int err = KErrNone;
#ifdef CATALOGS_BUILD_CONFIG_HEAP_MARKS    
    __UHEAP_MARK;
#endif 
        {
        QCoreApplication app(argc, argv);
        CCatalogsClientServerServer* server = NULL;
        TRAP( err, server = CCatalogsClientServerServer::NewL() );
        if ( err == KErrNone )
            {
            DLINFO(( "Opening semaphore" ));
            RSemaphore semaphore;
            err = semaphore.OpenGlobal( KCatalogsServerSemaphoreName );
            if (err == KErrNone )
                {
                DLINFO(( "Signalling and closing semaphore" ));
                // Semaphore opened ok
                semaphore.Signal();
                semaphore.Close();
                err = app.exec();
                }
            delete server;
            }
    
        DLINFO(("Server has shut down"));
        }
#ifdef CATALOGS_BUILD_CONFIG_HEAP_MARKS    
    __UHEAP_MARKEND; 
#endif
    DLUNINIT;
    return err;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CCatalogsClientServerServer::AddShutdownOperation( 
    CCatalogsShutdownOperation* aOperation )
    {
    DLTRACEIN((""))
    DASSERT( aOperation );
    TInt err = iCatalogsServer->iShutdownOperations.Append( aOperation );

    if ( err == KErrNone )
        {
        aOperation->SetObserver( *iCatalogsServer );
        iCatalogsServer->IncrementSessions();
        }
    else
        {
        DLERROR(("Appending failed: %d", err));
        delete aOperation;
        }
    
    return err;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::ShutdownOperationComplete( 
    CCatalogsShutdownOperation* aOperation, 
    TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    (void) aError;
    iShutdownOperations.Remove( iShutdownOperations.Find( aOperation ) );
    delete aOperation;
    DecrementSessions();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerServer::OperateShutdownOperations( 
    const MCatalogsContext& aContext,
    TBool aExecute )
    {
    DLTRACEIN((""));
    TInt count = iShutdownOperations.Count();
    const TUid uid( aContext.FamilyId() );
    // Iterating backwards because an executed operation may be removed
    // from the queue while we are iterating through it
    while( count-- )
        {
        if ( iShutdownOperations[ count ]->FamilyUid() == uid ) 
            {
            if ( aExecute )
                {
                iShutdownOperations[ count ]->Execute();
                }
            else
                {
                iShutdownOperations[ count ]->Cancel();
                }
            }
        }
    }
