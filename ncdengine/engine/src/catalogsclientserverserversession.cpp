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
* Description:   Implementation of CCatalogsClientServerServerSession
*
*/


#include <e32kpan.h>

#include "catalogsclientserverserversession.h"
#include "catalogsserverdefines.h"
#include "catalogscommunicable.h"
#include "catalogsbasemessageimpl.h"
#include "catalogsincompletemessage.h"
#include "catalogscontextimpl.h"
#include "catalogsdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cleanupfunction for cleanupitem usage. Closes CObject based
// object as many times as needed to destroy it when removed
// from cleanupstack.
// ---------------------------------------------------------------------------
//    
void CCatalogsClientServerServerSession::CleanupCObjectBasedClass( TAny* aCObject )
	{
	CObject* convertedCObject = ( static_cast< CObject* > ( aCObject ) );
	TInt accessCount = convertedCObject->AccessCount();
    while ( accessCount > 0 )
        {
        convertedCObject->Close();
        --accessCount;
        }
    // Now that last reference is closed, aCObject is destroyed
	}


// ======== MEMBER FUNCTIONS ========

CCatalogsClientServerServerSession* CCatalogsClientServerServerSession::NewL(
    RThread& aClient,
    CCatalogsClientServerServer& aServer )
    {
    CCatalogsClientServerServerSession* self = 
        CCatalogsClientServerServerSession::NewLC( aClient, aServer );
    CleanupStack::Pop(self) ;
    return self ;
    }

CCatalogsClientServerServerSession* CCatalogsClientServerServerSession::
    NewLC( RThread& aClient, CCatalogsClientServerServer& aServer )
    {
    CCatalogsClientServerServerSession* self = 
        new (ELeave) CCatalogsClientServerServerSession( aClient, aServer,
            aServer.NewInstanceId() );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsClientServerServerSession::CCatalogsClientServerServerSession(
    RThread& /*aClient*/,
    CCatalogsClientServerServer& aServer,
    TInt aInstanceId ) :
    CSession2(),
    iClientSideDownAnnounced( ENotInformed ),
    iServer( aServer ),
    iInstanceId( aInstanceId )
    {
    }

CCatalogsClientServerServerSession::~CCatalogsClientServerServerSession()
    {
    DLTRACEIN((""));
    ClientSideSessionDown();   
    DLTRACE(("Deleting context")); 
    delete iContext;
    iServer.DecrementSessions();    
    DLTRACEOUT((""));
    }

void CCatalogsClientServerServerSession::ConstructL() 
    {    
    iServer.IncrementSessions();
    iMessageHandlers = CObjectIx::NewL();
    iIncompleteMessageContainer = iServer.NewContainerL();
    iIncompleteMessages = CObjectIx::NewL();

    }

void CCatalogsClientServerServerSession::ClientSideSessionDown()
    {
    DLTRACEIN((""));
    if ( iClientSideDownAnnounced == ENotInformed )
        {
        DLTRACE(("Delete incomplete messages"));
        // Incomplete messages act as receivers of messages as well
        // as nodes etc.
        delete iIncompleteMessages;
        iIncompleteMessages = NULL;

        // In examples, container is simply deleted, but here
        // this ends up making a KERN-EXEC 3. Probably container is 
        // removed for a second time when the container-index is removed.
        if ( iIncompleteMessageContainer != NULL )
            {
            DLTRACE(("Remove container"));
            iServer.RemoveContainer( iIncompleteMessageContainer );
            DLTRACE(("Container removed"));
            }
        iIncompleteMessageContainer = NULL;
        
        
        CloseMessageHandlers();
        
        DLTRACE(("Handle session removal"));
        // Inform possibly interested objects of session removal
        // so they know that this session does not exist anymore
        iServer.HandleSessionRemoval( *this );
        
        iClientSideDownAnnounced = EInformed;        
        }
        
    DLTRACEOUT((""));
    }
    

TInt CCatalogsClientServerServerSession::WriteToLargerDesL(
    const TDesC8& aOutputData,
    TInt aStatus )
    {
    CCatalogsIncompleteMessage* incompleteMessage =
        CCatalogsIncompleteMessage::NewL();

    // NOTICE: Because a CObject-derived object cannot be deleted
    //         when its reference count is not zero, we use a
    //         cleanup-item. Another way could be to do it in the
    //         destructor of the CObject-derived object, but
    //         it could enable unintended destruction of the object.?
    TCleanupItem cleanupCObjectInfo( CleanupCObjectBasedClass,
                                 incompleteMessage );
    CleanupStack::PushL( cleanupCObjectInfo );    
    
    incompleteMessage->SetMessageL( aOutputData, aStatus );
    iIncompleteMessageContainer->AddL( incompleteMessage );
    TInt handle = iIncompleteMessages->AddL( incompleteMessage );

    CleanupStack::Pop( incompleteMessage );
    return handle;
    }

TInt CCatalogsClientServerServerSession::WriteToLargerDesL(
    const TDesC16& aOutputData,
    TInt aStatus )
    {
    CCatalogsIncompleteMessage* incompleteMessage =
        CCatalogsIncompleteMessage::NewL();

    // NOTICE: Because a CObject-derived object cannot be deleted
    //         when its reference count is not zero, we use a
    //         cleanup-item. Another way could be to do it in the
    //         destructor of the CObject-derived object, but
    //         it could enable unintended destruction of the object.?
    TCleanupItem cleanupCObjectInfo( CleanupCObjectBasedClass,
                                 incompleteMessage );
    CleanupStack::PushL( cleanupCObjectInfo );
    
    incompleteMessage->SetMessageL( aOutputData, aStatus );
    iIncompleteMessageContainer->AddL( incompleteMessage );
    TInt handle = iIncompleteMessages->AddL( incompleteMessage );

    CleanupStack::Pop( incompleteMessage );
    return handle;
    }

void CCatalogsClientServerServerSession::RemoveIncompleteMessage( TInt aHandle )
    {
    iIncompleteMessages->Remove( aHandle );
    }

void CCatalogsClientServerServerSession::ServiceL( const RMessage2& aMessage )
    {
    DLTRACEIN((""));
    
    // If ServiceL leaves, the framework calls ServiceError which by default
    // completes the message with the leave code if it has not been completed
    // yet
    HandleMessageL( aMessage );
    }

TInt CCatalogsClientServerServerSession::AddObjectL(
    CObject* aObject )
    {
    iServer.AddObjectToContainerL( aObject );
    TInt handle( iMessageHandlers->AddL( aObject ) );
    // Because add to the ix does not call Open() of the CObject,
    // we have to do it ourselves
    User::LeaveIfError( aObject->Open() );
    return handle;
    }

void CCatalogsClientServerServerSession::RemoveObject( TInt aHandle )
    {
    // This closes the CObject once.
    iMessageHandlers->Remove( aHandle );
    }

MCatalogsContext& CCatalogsClientServerServerSession::Context()
    {
    return *iContext;
    }


const MCatalogsContext* CCatalogsClientServerServerSession::ContextPtr() const
    {
    return iContext;
    }


void CCatalogsClientServerServerSession::HandleMessageL(
    const RMessage2& aMessage )
    {
    DLTRACEIN(("Function: %X", aMessage.Function() ));

    // NOTICE: If aMessage has to be stored and used later
    //         then copy it and use the copy instead.

    if ( iClientSideDownAnnounced == EInformed )
        {
        // Do not communicate to client anymore
        // client-side session is going down
        return;
        }
    else if( ( aMessage.Function() != ECatalogsCreateContext ) 
             && ( iContext == NULL ) )
        {
        DLERROR(( "Completing message with KErrNotReady" ));
        aMessage.Complete( KErrNotReady );
        }

    
    if( aMessage.Function() == ECatalogsExternalMessage 
        || aMessage.Function() == ECatalogsExternalAllocMessage )
        {
        HandleExternalMessageL( aMessage );
        }
    else
        {
        HandleInternalMessageL( aMessage );
        }
    DLTRACEOUT((""));
    }

void CCatalogsClientServerServerSession::HandleExternalMessageL(
    const RMessage2& aMessage )
    {
    DLTRACEIN((""));
    TPckgBuf<TInt> handle;
    aMessage.ReadL( 1, handle );


    CCatalogsCommunicable* receiver = ReceiverFromHandle( handle() );
    
    DLTRACE(("Receiver for message: %X", receiver ));

    if( !receiver )
        {
        PanicClient( aMessage, EBadHandle );
        return;
        }
    

    CCatalogsBaseMessageImpl* baseMessage = 
        CCatalogsBaseMessageImpl::NewLC( aMessage,
                                         *this,
                                         handle() );
    TPckgBuf<TInt> funcNumber;
    aMessage.ReadL( 0, funcNumber );
        
    CleanupStack::Pop( baseMessage );

    DLTRACE(("Passing message"));

    // Now the message is already passed to the receiver
    // and it is its duty to use it to inform of possible
    // error situations
    //      -> Nothing is done here            
    receiver->ReceiveMessage( baseMessage, funcNumber() );
    
    DLTRACEOUT(("Message passed"));    
    }

void CCatalogsClientServerServerSession::HandleInternalMessageL(
    const RMessage2& aMessage )
    {
    DLTRACEIN((""));
    switch( aMessage.Function() )
        {
        case ECatalogsCompleteMessage:
            {
            DLTRACE(("ECatalogsCompleteMessage"));
            TPckgBuf<TInt> handle;
            aMessage.ReadL( 1, handle );
            
            CCatalogsIncompleteMessage* incompleteMessage =
                IncompleteMessageFromHandle( handle() );
            if( !incompleteMessage )
                {
                PanicClient( aMessage, EBadHandle );
                return;
                }
            
            const TDesC8& message = incompleteMessage->Message();
            TInt returnValue = incompleteMessage->ReturnValue();
            
            DLINFO(( "Writing message" ));
            TRAPD( error, aMessage.WriteL( 3, message ) );                        
            if ( error != KErrNone )
                {
                RemoveIncompleteMessage( handle() );
                User::Leave( error );
                }

            DLINFO(( "Completing message with %d", returnValue ));
            aMessage.Complete( returnValue );
            
            RemoveIncompleteMessage( handle() );            
            break;
            }
            
            
        case ECatalogsCompleteMessageWide:
            {
            DLTRACE(("ECatalogsCompleteMessageWide"));
            TPckgBuf<TInt> handle;
            aMessage.ReadL( 1, handle );
            
            CCatalogsIncompleteMessage* incompleteMessage =
                IncompleteMessageFromHandle( handle() );
            if( !incompleteMessage )
                {
                PanicClient( aMessage, EBadHandle );
                return;
                }
            
            const TDesC16& message = incompleteMessage->MessageWide();
            TInt returnValue = incompleteMessage->ReturnValue();

            DLINFO(( "Writing message" ));
            TRAPD( error, aMessage.WriteL( 3, message ) );
            if ( error != KErrNone )
                {
                RemoveIncompleteMessage( handle() );
                User::Leave( error );
                }

            DLINFO(( "Completing message with %d", returnValue ));
            aMessage.Complete( returnValue );
            RemoveIncompleteMessage( handle() );
            break;
            }
                        
        case ECatalogsRemoveIncompleteMessage:
            {
            DLTRACE(("ECatalogsRemoveIncompleteMessage"));
            TPckgBuf<TInt> handle;
            aMessage.ReadL( 1, handle );
            RemoveIncompleteMessage( handle() );
            DLINFO(( "Completing message with KErrNone" ));
            aMessage.Complete( KErrNone );
            
            break;
            }
        case ECatalogsClientSideDown:
            {
            DLTRACE(("ECatalogsClientSideDown"));
            ClientSideSessionDown();
            DLINFO(( "Completing message with KErrNone" ));
            aMessage.Complete( KErrNone );
            break;
            }

        case ECatalogsCreateProvider:
            {
            DLTRACE(("Create Provider"));
            
            CreateProviderL( aMessage );
            DLTRACE(("Create Provider done"));
            break;
            }

        case ECatalogsCreateContext:
            {
            DLTRACE(("ECatalogsCreateContext"));
            CreateContextL( aMessage );
            break;
            }

        default:
            {
            }
        }    
    }

void CCatalogsClientServerServerSession::CreateProviderL(
    const RMessage2& aMessage )
    {
    DLTRACEIN((""));

    TInt providerUid = aMessage.Int0();
    TUint32 providerOptions = aMessage.Int2();
    
    // We have to have a integer which contains the handle to be
    // able to write it into client side later.
    TInt tempHandle( 0 );
    
    TRAPD( outcome,
        iServer.CreateProviderL( 
            *this, providerUid, tempHandle, providerOptions ) );
    
    // KErrNone and positive codes are a success. Cache cleaning
    // can cause positive leaves
    if ( outcome >= KErrNone ) 
        {
        DLTRACE(("Writing the provider handle to the message"));
        TPckgBuf<TInt> handleBuf( tempHandle );
    
        TRAPD( err, aMessage.WriteL( 1, handleBuf ) );
        if ( err != KErrNone )
            {
            DLTRACE(("Couldn't pass the handle back"));
            // If we can't pass the handle back, there is no use
            // to try to continue normally.
            RemoveObject( tempHandle );
            outcome = err;
            }
        }
    DLTRACE(("Completing message with: %d", outcome ));    
    aMessage.Complete( outcome );    
    }


void CCatalogsClientServerServerSession::CreateContextL(
    const RMessage2& aMessage )
    {
    DLTRACEIN((""));
    if ( iContext != NULL )
        {
        DLERROR(( "Completing message with KErrAlreadyExists" ));
        aMessage.Complete( KErrAlreadyExists );
        return;
        }
    
    TInt familyIdTint = aMessage.Int0();
    TUid familyIdUid = TUid::Uid( familyIdTint );    
    TSecureId secureId = aMessage.SecureId();

    iContext = CCatalogsContextImpl::NewL( familyIdUid, secureId, iInstanceId );

    // cancel shutdown operations for this family
    iServer.OperateShutdownOperations( Context(), EFalse );
    
    DLINFO(( "Completing message with KErrNone" ));
    aMessage.Complete( KErrNone );
    DLTRACEOUT((""));
    }

void CCatalogsClientServerServerSession::PanicClient(
    const RMessage2& aMessage, TInt aPanic ) const
    {
    DLTRACEIN((""));
    // NOTICE: KTxtCatalogsSession should not exceed 16 characters.
    //         If it does, it will result in USER 23. So it seems
    //         that RMEssage2::Panic does not truncate the characters
    //         that exceed those 16 characters.
    _LIT( KTxtCatalogsSession,"CCatalogsSession" );
    aMessage.Panic( KTxtCatalogsSession, aPanic );
    }

CCatalogsCommunicable* 
    CCatalogsClientServerServerSession::ReceiverFromHandle( TInt aHandle )
    {
    DLTRACEIN((""));
    if ( iMessageHandlers == NULL )
        {
        return NULL;
        }
    // Normal situation
    CCatalogsCommunicable* handler = 
        static_cast<CCatalogsCommunicable*>( iMessageHandlers->At( aHandle ) );
	DLTRACEOUT((""));
    return handler;
    }

CCatalogsIncompleteMessage* 
    CCatalogsClientServerServerSession::IncompleteMessageFromHandle( TInt aHandle )
    {
    DLTRACEIN((""));
    if ( iIncompleteMessages == NULL )
        {
        return NULL;
        }
    
    // Normal situation    
    CCatalogsIncompleteMessage* message =
        static_cast<CCatalogsIncompleteMessage*>( 
            iIncompleteMessages->At( aHandle ) ); 
    DLTRACEOUT((""));
    return message;   
    }
    
void CCatalogsClientServerServerSession::CloseMessageHandlers()
    {
    DLTRACEIN((""));
    TInt communicableCount( iMessageHandlers->Count() );
    for ( TInt index = 0; index < communicableCount; ++index )
        {
        CCatalogsCommunicable* handler = 
            static_cast<CCatalogsCommunicable*>( ( *iMessageHandlers )[index] );
        // If handlers have been removed, iMessageHandlers seem to contain
        // null pointers in place of those removed handlers
        if ( handler != NULL )
            {
            handler->CounterPartLost( *this );
            }
        }
        
    // Deletion closes all referenced objects once
    delete iMessageHandlers;
    iMessageHandlers = NULL;

    DLTRACEOUT((""));
    }
