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
* Description:   Contains CNcdNodeSeenFolderProxy
*
*/


#include "ncdnodeseenfolderproxy.h"
#include "catalogsdebug.h"
#include "ncdnodefolderproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "catalogsutils.h"
#include "ncdnodeidentifier.h"
#include "ncdnodemanagerproxy.h"

CNcdNodeSeenFolderProxy* CNcdNodeSeenFolderProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeFolderProxy& aNode ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenFolderProxy* self = NewLC(
        aSession, aHandle, aNode );
    CleanupStack::Pop( self );
    return self;
    }
    

CNcdNodeSeenFolderProxy* CNcdNodeSeenFolderProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeFolderProxy& aNode ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenFolderProxy* self = 
        new( ELeave ) CNcdNodeSeenFolderProxy(
            aSession, aHandle, aNode );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
    
CNcdNodeSeenFolderProxy::CNcdNodeSeenFolderProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeFolderProxy& aNode ) :
    CNcdInterfaceBaseProxy( aSession, aHandle, &aNode ),
    iFolder( aNode )
    {
    }


void CNcdNodeSeenFolderProxy::ConstructL() 
    {
    DLTRACEIN((""));
    
    // Register the interface.
    MNcdNodeSeenFolder* seen( this );
    DLINFO(("SeenFolder-ptr: %X, this: %X", seen, this ));
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL(
            seen, this, MNcdNodeSeenFolder::KInterfaceUid ) );
            
    InternalizeL();            
    }

CNcdNodeSeenFolderProxy::~CNcdNodeSeenFolderProxy() 
    {
    DLTRACEIN((""));

    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeSeenFolder::KInterfaceUid );
    }
    
    
void CNcdNodeSeenFolderProxy::InternalizeL() 
    {
    // Nothing to internalize currently.
    }
    
void CNcdNodeSeenFolderProxy::SetContentsSeenL() 
    {
    DLTRACEIN((""));
    
    // Send the message to server side.
    TInt output;
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdNodeSeenFolderSetContentsSeen,
            KNullDesC(),
            output,
            Handle() ) );
    }
    
    
TInt CNcdNodeSeenFolderProxy::NewCountL( TInt aLevels ) const
    {
    DLTRACEIN((""));
    
    if ( aLevels < 1 ) 
        {
        User::Leave( KErrArgument );
        }
    
    // Send the message to server side.
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    writer().WriteInt32L( aLevels );
    
    TInt count( 0 );
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdNodeSeenFolderNewCount,
            writer.PtrL(),
            count,
            Handle() ) );
            
    CleanupStack::PopAndDestroy( &writer );            
    
    DLINFO(( "count: %d", count ));        
    return count;
    }
    

RCatalogsArray<MNcdNode> CNcdNodeSeenFolderProxy::NewNodesL( TInt aLevels ) const 
    {
    DLTRACEIN(("levels: %d", aLevels));
    
    if ( aLevels < 1 ) 
        {
        User::Leave( KErrArgument );
        }
        
    // Send the message to server side.
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    writer().WriteInt32L( aLevels );
    
    HBufC8* receiveData( NULL );
    User::LeaveIfError(
        ClientServerSession().SendSyncAlloc(
            NcdNodeFunctionIds::ENcdNodeSeenFolderNewNodes,
            writer.PtrL(),
            receiveData,
            Handle(),
            0 ) );

    CleanupStack::PopAndDestroy( &writer );
    
    RCatalogsArray<MNcdNode> array;
    CleanupResetAndDestroyPushL( array );

    DASSERT( receiveData );
    CleanupStack::PushL( receiveData );
        
    // Read the identifiers from receive data and add the nodes to the
    // array.
    RDesReadStream desReadStream( *receiveData );
    CleanupClosePushL( desReadStream );
    
    TInt identifierCount = desReadStream.ReadInt32L();
    
    for ( TInt i = 0; i < identifierCount; i++ ) 
        {
        CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( desReadStream );
        MNcdNode* node = &iFolder.NodeManager().NodeL( *nodeId );
        CleanupStack::PopAndDestroy( nodeId );
        array.AppendL( node );
        
        // Add the reference count to make sure the nodes in the
        // array don't get deleted
        node->AddRef();
        }
        
    CleanupStack::PopAndDestroy( &desReadStream );
    CleanupStack::PopAndDestroy( receiveData );
    CleanupStack::Pop( &array );
    
    return array;
    }
