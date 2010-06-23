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
* Description:   Contains CNcdNodeDownloadProxy class implementation
*
*/


#include "ncdnodedownloadproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodeproxy.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeDownloadProxy::CNcdNodeDownloadProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iMetadata( aMetadata )
    {
    }


void CNcdNodeDownloadProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeDownload* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeDownload::KInterfaceUid ) );
    
    InternalizeL();
    }


CNcdNodeDownloadProxy* CNcdNodeDownloadProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeDownloadProxy* self = 
        CNcdNodeDownloadProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeDownloadProxy* CNcdNodeDownloadProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeDownloadProxy* self = 
        new( ELeave ) CNcdNodeDownloadProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeDownloadProxy::~CNcdNodeDownloadProxy()
    {
    DLTRACEIN((""));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeDownload::KInterfaceUid );
    
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    }


CNcdNodeMetadataProxy& CNcdNodeDownloadProxy::Metadata() const
    {
    return iMetadata;
    }


void CNcdNodeDownloadProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
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


// MNcdNodeDownload functions

TBool CNcdNodeDownloadProxy::IsDownloadedL() const
    {
    DLTRACEIN((""));
    if ( !iIsDownloaded )
        {
        DLTRACEOUT(("iIsDownloaded == EFalse"));
        return EFalse;
        }
        
    TInt filesExist = 0;
    // Internalize so that the server checks whether the files exist or not
    User::LeaveIfError(
        ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdFilesExist,
                       KNullDesC8,
                       filesExist,
                       Handle() ) );

    DLTRACEOUT(("isDownloaded status from server: %d", filesExist ));
    return filesExist;
    }


MNcdDownloadOperation* CNcdNodeDownloadProxy::DownloadContentL( 
    MNcdDownloadOperationObserver& aObserver )
    {
    DLTRACEIN((""));    
    
    CNcdDownloadOperationProxy* operation( NULL );

    // What is the data id here? Is KNullDesC ok?
    operation = 
        Metadata().Node().
        OperationManager().
        CreateDownloadOperationL( ENcdContentDownload, 
                                  Metadata().Node(),
                                  KNullDesC, 
                                  &aObserver );

    // It is enought that the observer is informed
    // when the file operation is ready. So, no need to add
    // node or this class object to be observers.

    // No need to increase the operation ref count here because
    // it should be initially one.

    DLTRACEOUT(("Operation %x", 
        static_cast<MCatalogsBase*>( operation ) ));    
        
    return operation;
    }
    

// Other functions

void CNcdNodeDownloadProxy::InternalizeDataL( RReadStream& aStream )
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
    
    if ( classId != NcdNodeClassIds::ENcdNodeDownloadClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    iIsDownloaded = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }
