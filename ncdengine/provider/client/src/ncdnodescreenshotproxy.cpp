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
* Description:   Contains CNcdNodeScreenshotProxy class implementation
*
*/


#include "ncdnodescreenshotproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodeproxy.h"
#include "ncddownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncdpanics.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeScreenshotProxy::CNcdNodeScreenshotProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    : CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
      iMetadata( aMetadata )
    {
    }


void CNcdNodeScreenshotProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeScreenshot* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeScreenshot::KInterfaceUid ) );

    // Let this leave if the internalization fails.    
    InternalizeL();
    }


CNcdNodeScreenshotProxy* CNcdNodeScreenshotProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeScreenshotProxy* self = 
        CNcdNodeScreenshotProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeScreenshotProxy* CNcdNodeScreenshotProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeScreenshotProxy* self = 
        new( ELeave ) CNcdNodeScreenshotProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeScreenshotProxy::~CNcdNodeScreenshotProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeScreenshot::KInterfaceUid );
    
    // Delete member variables here.
    // Do not delete node and operation manager because
    // this object does not own them.

    iScreenshotMimeType.ResetAndDestroy();
    }


CNcdNodeMetadataProxy& CNcdNodeScreenshotProxy::MetadataL() const
    {
    DLTRACEIN((""));

    if( IsObsolete() )
        {
        DLERROR(("Screenshot is obsolete. Metadata may not exist."));
        User::Leave( KNcdErrorObsolete );
        }
    
    DLTRACEOUT((""));
    
    return iMetadata;
    }


void CNcdNodeScreenshotProxy::InternalizeL()
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
        DLINFO(("Screenshot was obsolete"));
        SetObsolete( ETrue );
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeScreenshot::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeScreenshot* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeScreenshot::KInterfaceUid ) );
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
        User::Leave( KErrNotFound );
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


// MNcdNodeScreenshot functions

TInt CNcdNodeScreenshotProxy::ScreenshotCount() const
    {
    return iScreenshotCount;
    }

const TDesC& CNcdNodeScreenshotProxy::ScreenshotMimeType( TInt aIndex ) const
    {
    NCD_ASSERT_ALWAYS( aIndex >= 0 && aIndex < ScreenshotCount(),
        ENcdPanicIndexOutOfRange );
    return *iScreenshotMimeType[aIndex];
    }

MNcdDownloadOperation* CNcdNodeScreenshotProxy::LoadScreenshotL(
    TInt aIndex,
    MNcdDownloadOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    NCD_ASSERT_ALWAYS( aIndex >= 0 && aIndex < ScreenshotCount(),
        ENcdPanicIndexOutOfRange );

    CNcdDownloadOperationProxy* download =
        MetadataL().Node().OperationManager().
        CreateDownloadOperationL(
            ENcdScreenshotDownload,
            MetadataL().Node(),
            KNullDesC,
            aObserver,
            aIndex );

    DLTRACEOUT((""));

    return download;
    }

HBufC8* CNcdNodeScreenshotProxy::ScreenshotDataL( TInt aIndex ) const
    {
    NCD_ASSERT_ALWAYS( aIndex >= 0 && aIndex < ScreenshotCount(),
        ENcdPanicIndexOutOfRange );

    HBufC8* data( NULL );

    HBufC8* index = IntToDes8LC( aIndex );

    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdScreenshotData,
                       *index,
                       data,
                       Handle(),
                       0 ) );

    CleanupStack::PopAndDestroy( index );

    if ( data && data->Length() == 0 )
        {
        // If data does not contain any data, return NULL.
        delete data;
        return NULL;
        }

    return data;
    }


// Other functions


TBool CNcdNodeScreenshotProxy::IsObsolete() const
    {
    return iObsolete;
    }

void CNcdNodeScreenshotProxy::SetObsolete( TBool aObsolete )
    {
    iObsolete = aObsolete;
    }    
    
    
void CNcdNodeScreenshotProxy::InternalizeDataL( RReadStream& aStream )
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
    
    if ( classId != NcdNodeClassIds::ENcdNodeScreenshotClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    iScreenshotCount = aStream.ReadInt32L();

    iScreenshotMimeType.ResetAndDestroy();

    // Set new values if screenshots exist.
    for ( TInt i = 0; i < iScreenshotCount; i++ )
        {
        HBufC* mimeType = NULL;
        InternalizeDesL( mimeType, aStream );
        CleanupStack::PushL( mimeType );
        iScreenshotMimeType.AppendL( mimeType );
        CleanupStack::Pop( mimeType );
        }

    DLTRACEOUT((""));
    }
