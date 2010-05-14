/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdnodeseenimpl.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "catalogsbasemessage.h"
#include "ncdnodeimpl.h"
#include "ncdnodelink.h"

CNcdNodeSeen* CNcdNodeSeen::NewL( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode )
    {
    CNcdNodeSeen* self = NewLC( aSeenInfo, aNode );
    CleanupStack::Pop( self );
    return self;
    }


CNcdNodeSeen* CNcdNodeSeen::NewLC( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode )
    {
    CNcdNodeSeen* self = new ( ELeave ) CNcdNodeSeen( aSeenInfo, aNode );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }

CNcdNodeSeen::CNcdNodeSeen( CNcdNodeSeenInfo& aSeenInfo, CNcdNode& aNode ) :
    iSeenInfo( aSeenInfo ),
    iOwnerNode( aNode )
    {	
    }    

void CNcdNodeSeen::ConstructL()
    {
    }    

CNcdNodeSeen::~CNcdNodeSeen()
    {
    DLTRACEIN((""));
    }

    
TBool CNcdNodeSeen::IsSeenL()
    {
    DLTRACEIN((""));
    return iSeenInfo.IsSeenL( iOwnerNode.Identifier() );
    }
    

void CNcdNodeSeen::SetSeenL()
    {
    DLTRACEIN((""));
    iSeenInfo.SetSeenL( iOwnerNode.Identifier() );
    }

void CNcdNodeSeen::ReceiveMessage(
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN((""))
    
    DASSERT( aMessage );
                
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdNodeSeenSetSeen:
            TRAP( trapError, SetSeenRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;
            
        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }
       
    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        aMessage->CompleteAndRelease( trapError );
        }
    }
    
    
void CNcdNodeSeen::CounterPartLost( const MCatalogsSession& /*aSession*/ )
    {
    DLTRACEIN((""));
    }
    

void CNcdNodeSeen::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    TBool seen = IsSeenL();
    aMessage.CompleteAndReleaseL( seen, KErrNone );
    }
    

void CNcdNodeSeen::SetSeenRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    SetSeenL();
    aMessage.CompleteAndRelease( KErrNone );
    }


void CNcdNodeSeen::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );
                
    DLTRACEOUT((""));
    }
