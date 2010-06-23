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
* Description:   CNcdSessionHandler implementation
*
*/


#include "ncdsessionhandler.h"
#include "ncdkeyvaluepair.h"
#include "catalogsdebug.h"
#include "ncdutils.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdSessionHandler* CNcdSessionHandler::NewL()
    {
    CNcdSessionHandler* self = new( ELeave ) CNcdSessionHandler;
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdSessionHandler::~CNcdSessionHandler()
    {
    // Delete sessions
    iSessions.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// CreateSessionL
// ---------------------------------------------------------------------------
//
void CNcdSessionHandler::CreateSessionL( const TDesC& aServerUri,
    const TDesC& aNameSpace, 
    const TDesC& aSessionId )
    {
    TInt index = FindSession( aServerUri, aNameSpace );
    if ( index != KErrNotFound ) 
        {
        User::Leave( KErrAlreadyExists );
        }
    
    // Create session info and add it to sessions
    CNcdServerSession* serverSession = CNcdSessionHandler::CNcdServerSession::NewLC( aServerUri,
        aNameSpace,
        aSessionId );
    iSessions.AppendL( serverSession );
    CleanupStack::Pop( serverSession );
    }
    
    
// ---------------------------------------------------------------------------
// RemoveSession
// ---------------------------------------------------------------------------
//    
void CNcdSessionHandler::RemoveSession( const TDesC& aServerUri,
    const TDesC& aNameSpace )
    {    
    TInt index = FindSession( aServerUri, aNameSpace );
    DLTRACE( ( "Index: %i", index ) );
    
    if ( index != KErrNotFound )
        {
        delete iSessions[index];
        iSessions.Remove( index );
        }
    }
  
// ---------------------------------------------------------------------------
// RemoveAllSessions
// ---------------------------------------------------------------------------
//    
void CNcdSessionHandler::RemoveAllSessions()
    {
    DLTRACEIN((""));
    iSessions.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// Session
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdSessionHandler::Session( const TDesC& aServerUri,
    const TDesC& aNameSpace )
    {
    TInt index = FindSession( aServerUri, aNameSpace );
    if ( index != KErrNotFound ) 
        {
        return iSessions[index]->SessionId();
        }
        
    return KNullDesC;
    }
    
    
// ---------------------------------------------------------------------------
// DoesSessionExist
// ---------------------------------------------------------------------------
//
TBool CNcdSessionHandler::DoesSessionExist( const TDesC& aServerUri,
    const TDesC& aNameSpace )
    {
    return ( FindSession( aServerUri, aNameSpace ) >= 0 );
    }


// ---------------------------------------------------------------------------
// FindSession
// ---------------------------------------------------------------------------
//
TInt CNcdSessionHandler::FindSession( const TDesC& aServerUri,
    const TDesC& aNameSpace )
    {
    for ( TInt i = 0; i < iSessions.Count(); ++i ) 
        {
        if ( iSessions[i]->ServerUri().Compare( aServerUri ) == 0 &&
             iSessions[i]->NameSpace().Compare( aNameSpace ) == 0 ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }

CNcdSessionHandler::CNcdServerSession* CNcdSessionHandler::CNcdServerSession::NewL( const TDesC& aServerUri,
    const TDesC& aNameSpace, 
    const TDesC& aSessionId )
    {
    CNcdServerSession* self = NewLC( aServerUri, aNameSpace, aSessionId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSessionHandler::CNcdServerSession* CNcdSessionHandler::CNcdServerSession::NewLC( const TDesC& aServerUri,
    const TDesC& aNameSpace, 
    const TDesC& aSessionId )
    {
    CNcdServerSession* self = new (ELeave) CNcdServerSession;
    CleanupStack::PushL( self );
    self->ConstructL( aServerUri, aNameSpace, aSessionId );
    return self;
    }
    
CNcdSessionHandler::CNcdServerSession::~CNcdServerSession()
    {
    delete iServerUri;
    delete iNameSpace;
    delete iSessionId;
    }
    
const TDesC& CNcdSessionHandler::CNcdServerSession::ServerUri()
    {
    return *iServerUri;
    }

const TDesC& CNcdSessionHandler::CNcdServerSession::NameSpace()
    {
    return *iNameSpace;
    }

const TDesC& CNcdSessionHandler::CNcdServerSession::SessionId()
    {
    return *iSessionId;
    }
    
CNcdSessionHandler::CNcdServerSession::CNcdServerSession()
    {
    }
    
void CNcdSessionHandler::CNcdServerSession::ConstructL( const TDesC& aServerUri,
    const TDesC& aNameSpace, 
    const TDesC& aSessionId )
    {
    iServerUri = aServerUri.AllocL();
    iNameSpace = aNameSpace.AllocL();
    iSessionId = aSessionId.AllocL();
    }

