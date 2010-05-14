/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdNodeDependencyProxy class implementation
*
*/


#include "ncdnodedependencyproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodedependencyinfoimpl.h"
#include "ncddependencyinfo.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
#include "ncdnodemanagerproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeDependencyProxy::CNcdNodeDependencyProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iNodeManager( aMetadata.Node().NodeManager() )
    {
    }


void CNcdNodeDependencyProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeDependency* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeDependency::KInterfaceUid ) );

    // Let the construction leave if we do not get the data from the server.    
    InternalizeL();
    }


CNcdNodeDependencyProxy* CNcdNodeDependencyProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeDependencyProxy* self = 
        CNcdNodeDependencyProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeDependencyProxy* CNcdNodeDependencyProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeDependencyProxy* self = 
        new( ELeave ) CNcdNodeDependencyProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeDependencyProxy::~CNcdNodeDependencyProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeDependency::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete manager because
    // this object does not own it.

    // Casting is safe here because only this class creates the objects
    // that are inserted into the array.
    ResetAndDestroyWithCast<CNcdNodeDependencyInfo>( iDependencyTargets );
    }


CNcdNodeManagerProxy& CNcdNodeDependencyProxy::NodeManager() const
    {
    return iNodeManager;
    }


void CNcdNodeDependencyProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    TInt error(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( error == KNcdErrorObsolete )
        {
        DLINFO(("Dependency was obsolete"));
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeDependency::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeDependency* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeDependency::KInterfaceUid ) );
        }

    // If error occurred during data transfer, leave here and forward the error.
    User::LeaveIfError( error );
    
    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

     CleanupStack::PushL( data );

     // Read the data from the stream and insert it to the memeber variables
     RDesReadStream stream( *data );
     CleanupClosePushL( stream );
    
     InternalizeDataL( stream );
    
     // Closes the stream
     CleanupStack::PopAndDestroy( &stream ); 
     CleanupStack::PopAndDestroy( data );

    DLTRACEOUT((""));    
    }


// MNcdNodeDependency functions

const RPointerArray<MNcdNodeDependencyInfo>& CNcdNodeDependencyProxy::DependencyInfos() const
    {
    DLTRACEIN((""));
    return iDependencyTargets;
    }



TNcdDependencyState CNcdNodeDependencyProxy::State() const
    {
    DLTRACEIN((""));
    TNcdDependencyState totalState = ENcdDependencyInstalled;
    for ( TInt i = 0; i < iDependencyTargets.Count(); ++i ) 
        {
        if ( iDependencyTargets[i]->State() < totalState ) 
            {
            DLTRACE(("Downgrading state from %d to %d", 
                totalState, iDependencyTargets[i]->State() ));
            totalState = iDependencyTargets[i]->State();                        
            }
        }
    DLTRACEOUT(("Total state: %d", totalState ));
    return totalState;
    }
    

// Other functions

void CNcdNodeDependencyProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Use catalogsutils.h functions to internalize
    // memebervariables according to the data received
    // from the server.
    // Make sure that the variables are set here in the same
    // order as they are externalized in the server side.
    // Small mistake here messes up everything!

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeDependencyClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }

    InternalizeDependencyArrayL( aStream );

    DLTRACEOUT((""));
    }


void CNcdNodeDependencyProxy::InternalizeDependencyArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Casting is safe here because only this class creates the objects
    // that are inserted into the array.
    ResetAndDestroyWithCast<CNcdNodeDependencyInfo>( iDependencyTargets );
    
    // First get the node dependency information that is sent from the server
    // and then convert it to the proxy side dependency information object
    // that is inserted into the array.
    TInt count( aStream.ReadInt32L() );
    CNcdDependencyInfo* dependencyInfo( NULL );
    CNcdNodeDependencyInfo* info( NULL );
    for( TInt i = 0; i < count; ++i )
        {
        dependencyInfo = CNcdDependencyInfo::NewLC( aStream );
        info = CNcdNodeDependencyInfo::NewLC( *dependencyInfo, NodeManager() );
        iDependencyTargets.AppendL( info );
        // Ownership was transferred to the array
        CleanupStack::Pop( info );
        CleanupStack::PopAndDestroy( dependencyInfo );
        }

    // Second get the content dependency information
    count = aStream.ReadInt32L();
    dependencyInfo = NULL;
    info = NULL;
    for( TInt i = 0; i < count; ++i )
        {
        dependencyInfo = CNcdDependencyInfo::NewLC( aStream );
        info = CNcdNodeDependencyInfo::NewLC( *dependencyInfo, NodeManager() );
        iDependencyTargets.AppendL( info );
        CleanupStack::Pop( info );
        CleanupStack::PopAndDestroy( dependencyInfo );
        }

    DLTRACEOUT((""));
    }
