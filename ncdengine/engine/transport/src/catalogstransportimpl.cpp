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


#include "catalogstransportimpl.h"

#include "catalogstransporttypes.h"
#include "catalogssmssessionimpl.h"
#include "catalogshttpsessionimpl.h"
#include "catalogshttpsessionmanagerimpl.h"

#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCatalogsTransport* CCatalogsTransport::NewL()
    {
    CCatalogsTransport* self = new( ELeave ) CCatalogsTransport();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsTransport::~CCatalogsTransport()
    {
    iRemovingAll = ETrue;
    
    DLTRACEIN(( KNullDesC() ));
    // Release SMS sessions if the clients haven't done it yet
    for ( TInt i = 0; i < iSmsSessions.Count(); ++i )
        {
        while ( iSmsSessions[i]->Release() )
            {
            }
        }


    DLTRACE( ( _L("Releasing HTTP sessions") ) );
    // Release HTTP sessions if the clients haven't done it yet
    for ( TInt i = 0; i < iHttpSessions.Count(); ++i )
        {
        /*
        while ( iHttpSessions[i]->Release() ) 
            {
            }
            */
        // Ensure sessions don't try to access Transport anymore
        static_cast<CCatalogsHttpSession*>(iHttpSessions[i])->SetOwner( NULL );
        }    
    DLTRACE( ( _L("HTTP sessions released") ) );
    
    // Should the sessions be deleted?
    iSmsSessions.Close();
    iHttpSessions.Close();
    //delete iHttpSessionManager;
    if ( iHttpSessionManager ) 
        {        
        iHttpSessionManager->Release();
        }
    DLTRACEOUT(( KNullDesC() ));
    }


// ---------------------------------------------------------------------------
// Queries an interface from Transport
// ---------------------------------------------------------------------------
//
MCatalogsTransportSession* CCatalogsTransport::QueryInterfaceL( 
    TInt32 aSessionId, 
    TInt aInterfaceId,
    TBool aCleanupSession )
    {
    DLTRACEIN(("this=%X,session=%d,interface=%d",this, aSessionId,aInterfaceId));
    MCatalogsTransportSession* interface = NULL;
	
    // Instantiate the correct interface
    switch( aInterfaceId ) 
        {
        case KCatalogsTransportSmsInterface: 
            {
            // Check if the interface has been already instantiated with 
            // the same session id
            interface = FindSession( aSessionId, iSmsSessions );
			
            if ( !interface ) 
                {			    
                interface = CCatalogsSmsSession::NewL( aSessionId, *this );
    			
                TInt err = iSmsSessions.Append( interface );			
                if ( err != KErrNone ) 
                    {
                    interface->Release();
                    User::Leave( err );
                    }    			
                }
            else 
                {
                // Increase reference count 
                interface->AddRef();
                }
            break;
            }
		
		
        case KCatalogsTransportHttpInterface:
            {
            // Check if the interface has been already instantiated with 
            // the same session id
            interface = FindSession( aSessionId, iHttpSessions );
			
            if( !interface ) 
                {			    
                interface = CCatalogsHttpSession::NewL( 
                    aSessionId, 
                    *this, 
                    *iHttpSessionManager,
                    aCleanupSession );
    			
                TInt err = iHttpSessions.Append( interface );
                if ( err != KErrNone )
                    {
                    interface->Release();
                    User::Leave( err );
                    }    			
                }
            else
                {
                interface->AddRef();
                }
            break;
            }
			
			
        default: 
            {
            // Invalid interface ID
            User::Leave( KErrArgument );
            }
        }

    DLTRACEOUT((""));
    return interface;
    }


// ---------------------------------------------------------------------------
// Searches for the session
// ---------------------------------------------------------------------------
//		
MCatalogsTransportSession* CCatalogsTransport::FindSession( 
    TInt32 aSessionId, RCatalogsSessionArray& aSessions ) const
    {
    for( TInt i = 0; i < aSessions.Count(); ++i )
        {
        if( aSessions[i]->SessionId() == aSessionId ) 
            {
            return aSessions[i];
            }
        }
    return NULL;
    }


// ---------------------------------------------------------------------------
// Removes the session from Transport
// ---------------------------------------------------------------------------
//			
void CCatalogsTransport::RemoveSession( MCatalogsTransportSession* aSession )
    {
    DLTRACEIN( ( "aSession: %x", reinterpret_cast<TInt>( aSession ) ) );
    
    if( iRemovingAll ) 
        {
        return;
        }
        
    DASSERT( aSession );
    RCatalogsSessionArray* sessions = NULL;
    
    // Choose the correct array according to the session type
    switch( aSession->SessionType() )
        {
        case KCatalogsTransportSmsInterface:  
            {
            sessions = &iSmsSessions;
            break;
            }

        case KCatalogsTransportHttpInterface:  
            {
            sessions = &iHttpSessions;
            break;
            }
        
        default:
            {
            DASSERT( 0 );
            }
        }
            
    TInt index = sessions->Find( aSession );

    // There must not be a situation where a session is not found from
    // Transport
    //DASSERT( index != KErrNotFound );

    if ( index != KErrNotFound )
        {        
        // Remove the session
        sessions->Remove( index );
        }
    else
        {
        DLERROR(("Session was not found"));
        }
    }
    

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsTransport::ConstructL()
    {
    DLTRACEIN( ( "" ) );
    iHttpSessionManager = CCatalogsHttpSessionManager::NewL();
    DLTRACEOUT( ( "" ) );
    }
