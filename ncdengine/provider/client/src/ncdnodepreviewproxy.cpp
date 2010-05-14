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
* Description:   Contains CNcdNodePreviewProxy class implementation
*
*/


#include <f32file.h>
#include "ncdnodepreviewproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodeproxy.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdpanics.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodePreviewProxy::CNcdNodePreviewProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iMetadata( aMetadata )
    {
    }


void CNcdNodePreviewProxy::ConstructL()
    {
    // Register the interface
    MNcdNodePreview* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodePreview::KInterfaceUid ) );
    
    // Let this leave if internalization fails.
    InternalizeL();
    }


CNcdNodePreviewProxy* CNcdNodePreviewProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodePreviewProxy* self = 
        CNcdNodePreviewProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodePreviewProxy* CNcdNodePreviewProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodePreviewProxy* self = 
        new( ELeave ) CNcdNodePreviewProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodePreviewProxy::~CNcdNodePreviewProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodePreview::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    iPreviewMimeTypes.ResetAndDestroy();
    }


CNcdNodeMetadataProxy& CNcdNodePreviewProxy::MetadataL() const
    {
    DLTRACEIN((""));

   if( IsObsolete() )
        {
        DLERROR(("Preview is obsolete. Metadata may not exist."));
        User::Leave( KNcdErrorObsolete );
        }
    
    DLTRACEOUT((""));
    
    return iMetadata;
    }


void CNcdNodePreviewProxy::InternalizeL()
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
        DLINFO(("Preview was obsolete"));
        SetObsolete( ETrue );
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodePreview::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodePreview* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodePreview::KInterfaceUid ) );
        }
    else
        {
        // Just in case it was some times obsolete but not anymore
        SetObsolete( EFalse );
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


TBool CNcdNodePreviewProxy::IsObsolete() const
    {
    return iObsolete;
    }

void CNcdNodePreviewProxy::SetObsolete( TBool aObsolete )
    {
    iObsolete = aObsolete;
    }    
    

void CNcdNodePreviewProxy::InternalizeDataL( RReadStream& aStream )
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
    
    if ( classId != NcdNodeClassIds::ENcdNodePreviewClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    iPreviewCount = aStream.ReadInt16L();
    
    iPreviewMimeTypes.ResetAndDestroy();
    HBufC* previewMimeType = NULL;
    
    for( int i = 0; i < iPreviewCount; i++ )
        {
        InternalizeDesL( previewMimeType, aStream );
        CleanupStack::PushL( previewMimeType );
        iPreviewMimeTypes.Append( previewMimeType );
        CleanupStack::Pop( previewMimeType );
        previewMimeType = NULL;
        }

    DLTRACEOUT((""));
    }

TInt CNcdNodePreviewProxy::PreviewCount() const
    {
    return iPreviewCount;
    }

TBool CNcdNodePreviewProxy::IsPreviewLoadedL( TInt aIndex ) const
    {
    NCD_ASSERT_ALWAYS( 
        aIndex >= 0 && aIndex < PreviewCount(),
        ENcdPanicIndexOutOfRange );
    
    HBufC8* filenumber = IntToDes8LC( aIndex );
    TInt isLoaded;
    
    User::LeaveIfError(
        ClientServerSession().
            SendSync( NcdNodeFunctionIds::ENcdIsPreviewLoaded,
                      *filenumber,
                      isLoaded,
                      Handle()));
    CleanupStack::PopAndDestroy( filenumber );
    return isLoaded;
    }

const TDesC& CNcdNodePreviewProxy::PreviewMimeType( TInt aIndex ) const
    {
    NCD_ASSERT_ALWAYS( 
        aIndex >= 0 && aIndex < PreviewCount(),
        ENcdPanicIndexOutOfRange );
    
    return *iPreviewMimeTypes[aIndex];
    }

MNcdDownloadOperation* CNcdNodePreviewProxy::LoadPreviewL( TInt aIndex,
                                                           MNcdDownloadOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    
    NCD_ASSERT_ALWAYS( 
        aIndex >= 0 && aIndex < PreviewCount(),
        ENcdPanicIndexOutOfRange );

    CNcdDownloadOperationProxy* download =
        MetadataL().Node().OperationManager().
        CreateDownloadOperationL(
            ENcdPreviewDownload,
            MetadataL().Node(),
            KNullDesC,
            aObserver,
            aIndex );
    
    iOperationObserver = aObserver;
    DLTRACEOUT((""));
    return download;

    }

RFile CNcdNodePreviewProxy::PreviewFileL( TInt aIndex ) const
    {
    DLTRACEIN(( "aIndex: %i", aIndex ));

    NCD_ASSERT_ALWAYS( 
        aIndex >= 0 && aIndex < PreviewCount(),
        ENcdPanicIndexOutOfRange );
    
    // Convert index to a descriptor
    HBufC8* filenumber = IntToDes8LC( aIndex );
    
    /**
     * Add error handling, is it okay to leave here?
     */
    RFile fileHandle = ClientServerSession().SendSyncFileOpenL( 
        NcdNodeFunctionIds::ENcdPreviewOpenFile,
        *filenumber,        
        Handle());
    
    CleanupStack::PopAndDestroy( filenumber );
    
    DLTRACEOUT((""));
    return fileHandle;
    }

void CNcdNodePreviewProxy::DownloadProgress( MNcdDownloadOperation& /*aOperation*/,
    TNcdProgress /*aProgress*/ )
    {
    }
    
void CNcdNodePreviewProxy::QueryReceived( MNcdDownloadOperation& /*aOperation*/,
    MNcdQuery* /*aQuery*/ )
    {
    }

void CNcdNodePreviewProxy::OperationComplete( MNcdDownloadOperation& /*aOperation*/,
    TInt /*aError*/ )
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    }

