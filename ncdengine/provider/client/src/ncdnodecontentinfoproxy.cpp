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
* Description:   Contains CNcdNodeContentInfoProxy class implementation
*
*/


#include "ncdnodecontentinfoproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdoperationimpl.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"
#include "catalogsconstants.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeContentInfoProxy::CNcdNodeContentInfoProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata )
    {
    }


void CNcdNodeContentInfoProxy::ConstructL()
    {
    DLTRACEIN((""));
    // Register the interface
    MNcdNodeContentInfo* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeContentInfo::KInterfaceUid ) );
    
    InternalizeL();
    }


CNcdNodeContentInfoProxy* CNcdNodeContentInfoProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeContentInfoProxy* self = 
        CNcdNodeContentInfoProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeContentInfoProxy* CNcdNodeContentInfoProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeContentInfoProxy* self = 
        new( ELeave ) CNcdNodeContentInfoProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeContentInfoProxy::~CNcdNodeContentInfoProxy()
    {
    DLTRACEIN((""));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeContentInfo::KInterfaceUid );

    delete iMimeType;
    iMimeType = NULL;
    
    delete iIdentifier;
    iIdentifier = NULL;
    
    delete iVersion;
    iVersion = NULL;
    }


void CNcdNodeContentInfoProxy::InternalizeL()
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
        DLINFO(("Content info was obsolete"));
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeContentInfo::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeContentInfo* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeContentInfo::KInterfaceUid ) );
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


// MNcdNodeContentInfo functions
    
TUint CNcdNodeContentInfoProxy::Purpose() const
    {
    DLTRACEIN(( "iPurpose: %u", iPurpose ));
    return iPurpose;
    }

const TDesC& CNcdNodeContentInfoProxy::MimeType() const
    {
    DASSERT( iMimeType );
    return *iMimeType;
    }

const TDesC& CNcdNodeContentInfoProxy::Identifier() const
    {
    return *iIdentifier;
    }

TUid CNcdNodeContentInfoProxy::Uid() const
    {
    return iUid;
    }

const TDesC& CNcdNodeContentInfoProxy::Version() const
    {
    DASSERT( iVersion );
    return *iVersion;
    }

TInt CNcdNodeContentInfoProxy::Size() const
    {
    return iSize;
    }


// Other functions

void CNcdNodeContentInfoProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeContentInfoClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }

    iPurpose = aStream.ReadUint16L();
    DLINFO(( "Purpose: %u", iPurpose ));
    
    InternalizeDesL( iMimeType, aStream );    
    DLINFO(( _L("Mime: %S"), iMimeType ));
    
    //The Internalization here must be consistant with the externalization in CNcdNodeContentInfo
    if ( iMimeType->Compare( KMimeTypeMatchWidget ) == 0 )
        {
        //widget
        InternalizeDesL( iIdentifier, aStream );
        }
    else
        {
        //sis
        iUid.iUid = aStream.ReadInt32L();   
        DLINFO(( "Uid: %x", iUid.iUid )); 
        }
   
    InternalizeDesL( iVersion, aStream );    
    DLINFO(( _L("Version: %S"), iVersion ));

    iSize = aStream.ReadInt32L();
    DLINFO(( _L("Size: %d"), iSize ));
    
    DLTRACEOUT((""));
    }
