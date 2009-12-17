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
* Description:   Contains CCatalogsEngineImpl class implementation
*
*/


#include <e32base.h>
#include <e32property.h>
#include <e32msgqueue.h>
#include <s32mem.h>

#include "catalogspanics.h"
#include "catalogsengineimpl.h"
#include "ncdproviderproxy.h"
#include "catalogsdebug.h"
#include "catalogsengineobserver.h"
#include "catalogsconstants.h"
#include "catalogsuids.h"
#include "catalogsconnectionobserver.h"


class CCatalogsProviderCreator : public CActive
    {
public:

    static CCatalogsProviderCreator* NewL( 
        RCatalogsClientServerClientSession& aEngine, 
        TInt aUid, 
        MCatalogsBase*& aProvider, 
        TRequestStatus& aStatus,
        TUint32 aOptions );

    MCatalogsBase* Provider() const;

    ~CCatalogsProviderCreator();

protected:

    CCatalogsProviderCreator( 
        RCatalogsClientServerClientSession& aEngine, 
        MCatalogsBase*& aProvider, 
        TRequestStatus& aStatus );
    
    void ConstructL( TInt aUid, TUint32 aOptions );

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

    /**
     * Provider handle
     */
    TInt iHandle;

    /**
     * Proxy provider.
     * Own.
     */
    CNcdProviderProxy* iProviderProxy;

    /**
     * Engine.
     */
    RCatalogsClientServerClientSession& iEngine;

    /**
     * Proxy provider result.
     */
    MCatalogsBase*& iResultProvider;

    /**
     * Proxy provider request status.
     */
    TRequestStatus* iResultStatus;

    };

CCatalogsProviderCreator* CCatalogsProviderCreator::NewL( 
    RCatalogsClientServerClientSession& aEngine, 
    TInt aUid, 
    MCatalogsBase*& aProvider, 
    TRequestStatus& aStatus,
    TUint32 aOptions )
    {
    DLTRACEIN(( "aEngine=%08x, aUid=%08x, aProvider=%08x, aStatus=%08x, aOptions=%u", 
        &aEngine, aUid, &aProvider, &aStatus, aOptions ));
    
    CCatalogsProviderCreator* self = new (ELeave) CCatalogsProviderCreator( 
        aEngine, aProvider, aStatus );
    CleanupStack::PushL( self );
    self->ConstructL( aUid, aOptions );
    CleanupStack::Pop( self );

    DLTRACEOUT(( "%08x", self ));
    return self;
    }

void CCatalogsProviderCreator::ConstructL( TInt aUid, TUint32 aOptions )
    {
    DLTRACEIN(("aUid=%08x", aUid ));

    switch( aUid )
        {
        // Instantiate a provider based on UID information
        case 0:
        default:
            {
            iEngine.CreateProvider( aUid, iStatus, iHandle, aOptions );
            SetActive();            
            DASSERT( iStatus.Int() == KRequestPending );
            }
            break;
        }

    DLTRACEOUT((""));
    }

CCatalogsProviderCreator::~CCatalogsProviderCreator()
    {
    DLTRACEIN((""));
    
    // Never hurts to call this in destructor of active objects
    Cancel();

    if( iProviderProxy )
        {
        DLTRACE(("Release providerproxy internally"));
        iProviderProxy->InternalRelease();
        }
        
    DLTRACEOUT((""));    
    }

MCatalogsBase* CCatalogsProviderCreator::Provider() const
    {
    return iProviderProxy;
    }

CCatalogsProviderCreator::CCatalogsProviderCreator(
    RCatalogsClientServerClientSession& aEngine, MCatalogsBase*& aProvider, TRequestStatus& aStatus )
    : CActive( EPriorityNormal ), iEngine( aEngine ), iResultProvider( aProvider ), iResultStatus( &aStatus )
    {
    CActiveScheduler::Add( this );
    *iResultStatus = KRequestPending;
    }

void CCatalogsProviderCreator::RunL()
    {
    DLTRACEIN(( "iStatus=%08x, iHandle=%08x", iStatus.Int(), iHandle ));
    // Provider creation can succeed with positive codes
    if ( iStatus.Int() >= KErrNone )
        {
        DLINFO(( "Creating provider proxy" ));
        // If the provider was successfully created, instantiate a 
        // client-side provider object
        iProviderProxy = CNcdProviderProxy::NewL( iEngine, iHandle );
        DLTRACE(("Provider proxy inc ref count"));
        // Increase both reference counters
        iProviderProxy->InternalAddRef();
        iProviderProxy->AddRef();
        DLINFO(( "Provider proxy %08x created", iProviderProxy ));
        
        // We will probably never end up here if the request has been
        // completed already, but checking just to be sure.
        if ( iResultStatus )
            {
            DLINFO(( "Completing CreateProvider request" ));            
            iResultProvider = iProviderProxy;

            // iResultStatus is nullified by the RequestComplete
            User::RequestComplete( iResultStatus, iStatus.Int() );
            }
        }
    else 
        {
        // We will probably never end up here if the request has been
        // completed already, but checking just to be sure.
        if ( iResultStatus )
            {        
            User::RequestComplete( iResultStatus, iStatus.Int() );             
            }
        }
    DLTRACEOUT((""));
    }

void CCatalogsProviderCreator::DoCancel()
    {
    DLTRACEIN((""));
    // Going down now, so pending message sent by provider creator
    // should be completed immediately by client server 
    //(or it is completed by the framework if the message is already
    // completed but creator is not yet notified of it)
    iEngine.AsyncMessageSenderDown( iStatus );
    
    // Pending request is completed.
    // We should never end up here if the request is already
    // completed, but just checking to be sure.
    if ( iResultStatus )
        {
        // iResultStatus is nullified by the RequestComplete
        User::RequestComplete( iResultStatus, KErrCancel );
        }
    
    DLTRACEOUT((""));
    }

TInt CCatalogsProviderCreator::RunError( TInt aError )
    {
    DLTRACEIN((""));

    User::RequestComplete( iResultStatus, aError );

    DLTRACEOUT((""));
    return KErrNone;
    }


/** Catalogs shutdown observer, subscribes to maintenance lock P&S */

class CCatalogsShutdownObserver : public CActive
    {
public:

    static CCatalogsShutdownObserver* NewL( MCatalogsEngineObserver& aObserver );
    virtual ~CCatalogsShutdownObserver();

protected:

    CCatalogsShutdownObserver( MCatalogsEngineObserver& aObserver );
    void ConstructL();

protected: // from CActive

    void RunL();
    void DoCancel();

private:

    RProperty                   iMaintenanceLockProperty;
    MCatalogsEngineObserver&    iObserver;
    };


CCatalogsShutdownObserver* CCatalogsShutdownObserver::NewL( MCatalogsEngineObserver& aObserver )
    {
    DLTRACEIN(( "aObserver=%08x", &aObserver ));
    CCatalogsShutdownObserver* self = new (ELeave) CCatalogsShutdownObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsShutdownObserver::~CCatalogsShutdownObserver()
    {
    DLTRACEIN(("this=%08x", this));
    Cancel();
    iMaintenanceLockProperty.Close();

    DLTRACEOUT((""));
    }

CCatalogsShutdownObserver::CCatalogsShutdownObserver( MCatalogsEngineObserver& aObserver )
    : CActive( EPriorityNormal ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

void CCatalogsShutdownObserver::ConstructL()
    {
    DLTRACEIN((""));

    // Subscribe to maintenance notify property.
    User::LeaveIfError( iMaintenanceLockProperty.Attach( 
        KCatalogsEnginePropertyCategory, 
        KCatalogsEnginePropertyKeyMaintenanceLock ) );

    iMaintenanceLockProperty.Subscribe( iStatus );
    SetActive();

    DLTRACEOUT((""));
    }

void CCatalogsShutdownObserver::RunL()
    {
    DLTRACEIN(( "iStatus=%d", iStatus.Int() ));

    if( iStatus.Int() == KErrNone )
        {
        // Read the property value.
        TInt value;
        TInt err = iMaintenanceLockProperty.Get( value );
        if( err == KErrNone && value != 0 )
            {
            DLINFO(( "Calling shutdown observer %08x", &iObserver ));
            iObserver.CatalogsEngineShutdown();
            }

        if( err != KErrNone )
            {
            DLERROR(( "Failed to read maintenance lock property value, error %d", err ));
            }
        else
            {
            DLINFO(( "Maintenance lock value was %d", value ));
            }
        }

    DLTRACEOUT((""));
    }

void CCatalogsShutdownObserver::DoCancel()
    {
    DLTRACEIN((""));
    iMaintenanceLockProperty.Cancel();
    DLTRACEOUT((""));
    }


/** Catalogs OTA update observer, subscribes to update information P&S */

class CCatalogsUpdateObserver : public CActive
    {
public:

    static CCatalogsUpdateObserver* NewL( MCatalogsEngineObserver& aObserver );
    virtual ~CCatalogsUpdateObserver();

protected:

    CCatalogsUpdateObserver( MCatalogsEngineObserver& aObserver );
    void ConstructL();

protected: // from CActive

    void RunL();
    void DoCancel();

private:

    RMsgQueueBase               iUpdateMessageQueue;
    MCatalogsEngineObserver&    iObserver;
    };


CCatalogsUpdateObserver* CCatalogsUpdateObserver::NewL( MCatalogsEngineObserver& aObserver )
    {
    DLTRACEIN(( "aObserver=%08x", &aObserver ));
    CCatalogsUpdateObserver* self = new (ELeave) CCatalogsUpdateObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsUpdateObserver::~CCatalogsUpdateObserver()
    {
    DLTRACEIN(("this=%08x", this));
    Cancel();
    iUpdateMessageQueue.Close();
    DLTRACEOUT((""));
    }

CCatalogsUpdateObserver::CCatalogsUpdateObserver( MCatalogsEngineObserver& aObserver )
    : CActive( EPriorityNormal ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

void CCatalogsUpdateObserver::ConstructL()
    {
    DLTRACEIN((""));

    // Create the update information message queue for receiving update messages.
    RProcess process;
    DLINFO(( "Client SSID %08x", process.SecureId().iId ));

	HBufC* msgQueueName = HBufC::NewLC( KCatalogsUpdateQueueNameFormat().Length() + 8 );
	msgQueueName->Des().Format( KCatalogsUpdateQueueNameFormat, process.SecureId().iId );
	
	DLINFO(( _L("Creating client's update message queue: %S"), msgQueueName ));
	TInt err = iUpdateMessageQueue.CreateGlobal( 
	    *msgQueueName, 
	    KCatalogsUpdateQueueSlotCount, 
	    KCatalogsUpdateQueueMessageSize, 
	    EOwnerProcess );
	
	if( err == KErrAlreadyExists )
	    {
        err = iUpdateMessageQueue.OpenGlobal( *msgQueueName );
	    }
	
	if( err != KErrNone )
	    {
	    DLERROR(( "Failed to create update message queue, error %d", err ));
	    }
	else
	    {
    	THandleInfo info;
    	iUpdateMessageQueue.HandleInfo( &info );

        DLINFO(( "Processes for notify data available: %d", info.iNumProcesses ));
        DLINFO(( "Threads for notify data available: %d", info.iNumThreads ));
        DLINFO(( "Open in process for notify data available: %d", info.iNumOpenInProcess ));
        DLINFO(( "Open in thread for notify data available: %d", info.iNumOpenInThread ));
        
        // Only one request allowed per messagequeue
        // This causes problem in STIF tests at least when they are executed
        // in the emulator because the handles of crashed processes are not
        // always freed so testers won't get notifications about the data
        // and engine will eventually freeze because the queue gets full
        if ( info.iNumProcesses == 1 )        
            {
            DLINFO(( "Requesting notification of update messages" ));
            // A thread can have only one data available notification request outstanding 
            // on the message queue. If a second request is made before the first request 
            // completes, then the calling thread is panicked (KERN-EXEC 47).
            iUpdateMessageQueue.NotifyDataAvailable( iStatus );
            SetActive();            
            }
	    }
	    
	CleanupStack::PopAndDestroy( msgQueueName );

    DLTRACEOUT((""));
    }

void CCatalogsUpdateObserver::RunL()
    {
    DLTRACEIN(( "iStatus=%d", iStatus.Int() ));

    if( iStatus.Int() == KErrNone )
        {
        // Renew notification request.
        iUpdateMessageQueue.NotifyDataAvailable( iStatus );
        SetActive();
        
        CBufFlat* buffer = CBufFlat::NewL( KCatalogsUpdateInformationMaxSize + KCatalogsUpdateQueueMessageSize );
        CleanupStack::PushL( buffer );
        
        buffer->ResizeL( KCatalogsUpdateInformationMaxSize + KCatalogsUpdateQueueMessageSize );
        TUint8* msgPtr = const_cast< TUint8* >( buffer->Ptr( 0 ).Ptr() );
            
        while( iUpdateMessageQueue.Receive( msgPtr, KCatalogsUpdateQueueMessageSize ) == KErrNone )
            {
            msgPtr += KCatalogsUpdateQueueMessageSize;
            
            // Make a stream for reading the buffer.
            RBufReadStream stream( *buffer );
            CleanupClosePushL( stream );

            // Read total data size.
            TInt32 total;
            stream >> total;
            DLINFO(( "%d bytes of data incoming", total ));

            for( TInt bytesLeft = total + sizeof( TInt32 ) - KCatalogsUpdateQueueMessageSize;
                 bytesLeft > 0;
                 bytesLeft -= KCatalogsUpdateQueueMessageSize )
                {
                DLINFO(( "Reading more data, %d bytes to go", bytesLeft ));
                iUpdateMessageQueue.ReceiveBlocking( msgPtr, KCatalogsUpdateQueueMessageSize );
                msgPtr += KCatalogsUpdateQueueMessageSize;
                }
                
            // Read target string.
            HBufC* target = HBufC::NewLC( stream, KCatalogsUpdateTargetMaxSize );
            DLINFO(( _L("Update target: %S"), target ));
                        
            // Read ID string.
            HBufC* id = HBufC::NewLC( stream, KCatalogsUpdateIdMaxSize );
            DLINFO(( _L("Update target ID: %S"), id ));
            
            // Read version string.
            HBufC* version = HBufC::NewLC( stream, KCatalogsUpdateVersionMaxSize );
            DLINFO(( _L("Update version: %S"), version ));
            
            // Read URI string.
            HBufC* uri = HBufC::NewLC( stream, KCatalogsUpdateUriMaxSize );
            DLINFO(( _L("Update URI: %S"), uri ));
            
            // Read force boolean
            TInt32 force;
            stream >> force;
            DLINFO(( "Update forced: %d", force ));
            
            DLINFO(( "Calling update observer %08x", &iObserver ));
            iObserver.CatalogsUpdateNotification( *target, *id, *version, *uri, force );
            
            CleanupStack::PopAndDestroy( 5 ); // uri, version, id, target, stream-close
            
            // Reset to start position for next round (if any).
            msgPtr = const_cast< TUint8* >( buffer->Ptr( 0 ).Ptr() );
            }

        CleanupStack::PopAndDestroy( buffer );
        }

    DLTRACEOUT((""));
    }

void CCatalogsUpdateObserver::DoCancel()
    {
    DLTRACEIN((""));
    iUpdateMessageQueue.CancelDataAvailable();
    DLTRACEOUT((""));
    }


// ======== MEMBER FUNCTIONS ========

TInt CCatalogsEngineImpl::AddRef() const
    {
    DLTRACEIN(( "" ));
    return ++iRefCount;
    }

TInt CCatalogsEngineImpl::Release() const
    {
    DLTRACEIN(( "" ));
    // NOTE: engine object is deleted by client separately, do not self-destruct here.
    return --iRefCount;
    }

const TAny* CCatalogsEngineImpl::QueryInterfaceL( TInt aInterfaceType ) const
    {
    DLTRACEIN(( "" ));
    const MCatalogsBase* result = NULL;
   
    switch( aInterfaceType )
        {
        case MCatalogsBase::KInterfaceUid:
            result = static_cast< const MCatalogsBase* >( this );
            result->AddRef();
            break;

        case MCatalogsEngine::KInterfaceUid:
            result = static_cast< const MCatalogsEngine* >( this );
            result->AddRef();
            break;
        }

    if( result )
        {
        iRefCount++;
        }

    return result;
    }

CCatalogsEngineImpl::CCatalogsEngineImpl( MCatalogsEngineObserver& aObserver ) 
    : iObserver( &aObserver )
    {
    AddRef(); // Reference count is set to one
    }

CCatalogsEngineImpl::~CCatalogsEngineImpl()
    {
    DLTRACEIN((""));

    DLTRACE(( "Deleting shutdown observer" ));
    delete iShutdownObserver;

    DLTRACE(( "Deleting update observer %08x", iUpdateObserver ));
    delete iUpdateObserver;

    delete iConnectionObserver;
    
    DLTRACE(( "Closing catalogs engine mutex" ));
    iCatalogsMutex.Close();

    DLTRACEOUT((""));
    }

void CCatalogsEngineImpl::ConstructL()
    {
    DLTRACEIN(( "" ));

    // Mutex open should succeed, it is already open in CCatalogsEngine::NewLC().
    User::LeaveIfError( iCatalogsMutex.OpenGlobal( KCatalogsEngineMutex ) );

    // Create observer for maintenance lock mutex changes -> engine shutdown callback
    DLINFO(( "Creating shutdown observer" ));
    iShutdownObserver = CCatalogsShutdownObserver::NewL( *iObserver );

    // For the Catalogs UI client, create update observer for receiving OTA update
    // notifications.
    DLINFO(( "Checking for Catalogs UI client SID" ));
    RProcess process;
    static _LIT_SECURITY_POLICY_PASS( catalogsUiSecurityPolicy );
    if( catalogsUiSecurityPolicy().CheckPolicy( process ) )
        {
        DLINFO(( "Passed Catalogs UI security check, SID %08x", process.SecureId().iId ));
        DLINFO(( "Creating update observer" ));
        iUpdateObserver = CCatalogsUpdateObserver::NewL( *iObserver );
        }
    process.Close();
    
    iConnectionObserver = CCatalogsConnectionObserver::NewL( *iObserver );
    DLTRACEOUT((""));
    }

CCatalogsEngineImpl* CCatalogsEngineImpl::NewL( TAny* aInitParams )
    {
    DLTRACEIN(( "aInitParams=%08x" ));

    // Engine observer was passed as init param pointer.
    MCatalogsEngineObserver* observer = static_cast< MCatalogsEngineObserver* >( aInitParams );

    CCatalogsEngineImpl* self =  new( ELeave ) CCatalogsEngineImpl( *observer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DLTRACEOUT(( "%08x", self ));
    return self;
    }

TInt CCatalogsEngineImpl::Connect( TUid aClientUid )
    {
    DLTRACEIN(("%d",aClientUid.iUid));
    iClientUid = aClientUid;
    return iCatalogsEngine.Connect( aClientUid );
    }

void CCatalogsEngineImpl::Close()
    {
    DLTRACEIN((""));    

    // Provider creator has to be deleted before disconnect is said because
    // the provider creator owns the provider (internal addref called)
    // and when the provider is deleted it connects to server.
    DLTRACE(( "Deleting provider creator %08x", iProviderCreator ));
    delete iProviderCreator;
    iProviderCreator = NULL;

    // Disconnect is enough for the iCatalogsEngine. Don't call Close for
    // the iCatalogsEngine as it does it in the Disconnect.
    DLTRACE(( "Disconnect" ));
    iCatalogsEngine.Disconnect();
    
    DLTRACEOUT((""));  
    }


void CCatalogsEngineImpl::CreateProviderL( 
    TInt aUid, 
    MCatalogsBase*& aProvider, 
    TRequestStatus& aStatus,
    TUint32 aProviderOptions )
    {
    DLTRACEIN(("%X, %u", aUid, aProviderOptions ));
    // add UID mapping

    // If a provider has already been created, return that one
    if ( iProviderCreator != NULL )
        {
        MCatalogsBase* provider = iProviderCreator->Provider();
        if( provider != NULL )
            {
            DLINFO(("Provider already created"));

            // Increase reference count and pass to provider
            provider->AddRef();
            DLINFO(( "Writing CreateProvider result" ));
            aProvider = provider;

            aStatus = KRequestPending;
            TRequestStatus* statusPtr = &aStatus;
            User::RequestComplete( statusPtr, KErrNone );
            return;
            }
        else
            {
            DLINFO(("Provider is not created"));
            delete iProviderCreator;
            iProviderCreator = NULL;
            }
        }

    DLINFO(( "Starting provider creation" ));
    iProviderCreator = CCatalogsProviderCreator::NewL( 
        iCatalogsEngine, aUid, aProvider, aStatus, aProviderOptions );

    DLTRACEOUT((""));
    }

void CCatalogsEngineImpl::CancelProviderCreation()
    {
    DLTRACEIN((""));
    
    // If iProviderCreator is active then the provider has not been properly
    // constructed.
    if ( iProviderCreator->IsActive() )
        {
        // iProviderCreator is deleted. This enables calling of
        // CreateProviderL again after this function call. In
        // that case the outcome of CreateProviderL would be as
        // if the function had been called normally before any
        // CancelProviderCreation.
        // Deletion is also a proper solution here as the creation of
        // iProviderCreator is mainly about creating the provider.
        // Construction of the iProviderCreator is simple and can be
        // done again with quite a little effort.
        delete iProviderCreator;
        iProviderCreator = NULL;
        }
    }
