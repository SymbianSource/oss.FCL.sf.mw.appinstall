/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?description
*
*/


#include "ncdnodeseenproxy.h"
#include "ncdnodeproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"

// ======== MEMBER FUNCTIONS ========

CNcdNodeSeenProxy* CNcdNodeSeenProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeProxy& aNode ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenProxy* self = NewLC( aSession, aHandle, aNode );
    CleanupStack::Pop( self );
    return self;
    }
    
    
CNcdNodeSeenProxy* CNcdNodeSeenProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeProxy& aNode ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenProxy* self = new( ELeave ) CNcdNodeSeenProxy(
        aSession, aHandle, aNode );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }   
    

CNcdNodeSeenProxy::CNcdNodeSeenProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeProxy& aNode ) :
    CNcdInterfaceBaseProxy( aSession, aHandle, &aNode ),
    iSeen( EFalse ) 
    {
    }
    

void CNcdNodeSeenProxy::ConstructL() 
    {
    DLTRACEIN((""));
    // Register the interface.
    MNcdNodeSeen* seen( this );
    DLINFO(("Seen-ptr: %X, this: %X", seen, this ));
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL(
            seen, this, MNcdNodeSeen::KInterfaceUid ) );
    
    // Internalize the state from server side.
    InternalizeL();
    }
    

CNcdNodeSeenProxy::~CNcdNodeSeenProxy() 
    {
    DLTRACEIN((""));
    
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeSeen::KInterfaceUid );
    }

    
void CNcdNodeSeenProxy::InternalizeL()
    {
    DLTRACEIN((""));
    
    TBool seen( EFalse );
    
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdInternalize,
            KNullDesC(),
            seen,
            Handle() ) );
            
    iSeen = seen;
    }
    

TBool CNcdNodeSeenProxy::IsSeen() const 
    {
    DLTRACEIN(("seen: %d", iSeen));
    return iSeen;
    }
    
void CNcdNodeSeenProxy::SetSeenL()
    {
    DLTRACEIN((""));
    
    // Send the message to server side.
    TInt output;
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdNodeSeenSetSeen,
            KNullDesC(),
            output,
            Handle() ) );
            
    // Do not update the internal state here, since the change
    // must take place only just when client-server session is closed.
    }
