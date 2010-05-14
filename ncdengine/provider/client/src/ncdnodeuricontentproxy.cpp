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
* Description:   Contains CNcdNodeuriContentProxy class implementation
*
*/


#include "ncdnodeuricontentproxy.h"
#include "ncdnodemetadataproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeUriContentProxy* CNcdNodeUriContentProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeUriContentProxy* self = 
        CNcdNodeUriContentProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeUriContentProxy* CNcdNodeUriContentProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeUriContentProxy* self = 
        new( ELeave ) CNcdNodeUriContentProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeUriContentProxy::CNcdNodeUriContentProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iValidityDelta( -1 )
    {
    }


void CNcdNodeUriContentProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeUriContent* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
            MNcdNodeUriContent::KInterfaceUid ) );
    
    // Let this leave if the internalization fails.
    InternalizeL();
    }

CNcdNodeUriContentProxy::~CNcdNodeUriContentProxy()
    {
    RemoveInterface( MNcdNodeUriContent::KInterfaceUid );
    delete iContentUri;
    }


void CNcdNodeUriContentProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );

    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

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


// CNcdNodeUriContentProxy functions
    


// Other functions

void CNcdNodeUriContentProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeUriContentClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    InternalizeDesL( iContentUri, aStream );
    
    iValidityDelta = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }

const TDesC& CNcdNodeUriContentProxy::ContentUri() const
    {
    return *iContentUri;
    }

TInt CNcdNodeUriContentProxy::ContentValidityDelta() const
    {
    return iValidityDelta;
    }

