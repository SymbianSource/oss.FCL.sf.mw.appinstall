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
* Description:   Implements CNcdNodePreview class
*
*/


#include "ncdnodepreviewimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeidentifier.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncd_pp_download.h"
#include "ncdnodemanager.h"
#include "ncdproviderdefines.h"
#include "ncdpreviewmanager.h"
#include "ncderrors.h"


CNcdNodePreview::CNcdNodePreview( const NcdNodeClassIds::TNcdNodeClassId aClassId,
                                  CNcdNodeMetaData& aParentMetaData,
                                  CNcdNodeManager& aNodeManager )
: CNcdCommunicable(),
  iClassId( aClassId ),
  iParentMetaData( aParentMetaData ),
  iNodeManager( aNodeManager )
    {
    }

void CNcdNodePreview::ConstructL()
    {
    }


CNcdNodePreview* CNcdNodePreview::NewL( CNcdNodeMetaData& aParentMetaData,
                                        CNcdNodeManager& aNodeManager )
    {
    CNcdNodePreview* self =   
        CNcdNodePreview::NewLC( aParentMetaData, aNodeManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodePreview* CNcdNodePreview::NewLC( CNcdNodeMetaData& aParentMetaData,
                                         CNcdNodeManager& aNodeManager )
    {
    CNcdNodePreview* self = 
        new( ELeave ) CNcdNodePreview( 
            NcdNodeClassIds::ENcdNodePreviewClassId, 
            aParentMetaData,
            aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodePreview::~CNcdNodePreview()
    {
    DLTRACEIN((""));

    iPreviewMimeTypes.ResetAndDestroy();
    iUris.ResetAndDestroy();

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodePreview::ClassId() const
    {
    return iClassId;
    }


// Internalization from the protocol

void CNcdNodePreview::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    iFileCount = aData.PreviewCount();
    
    HBufC* mimetype;
    HBufC* uri;
    iPreviewMimeTypes.ResetAndDestroy();
    iUris.ResetAndDestroy();

    for( int i = 0; i < iFileCount; i++ )
        {
        mimetype = aData.PreviewL( i ).Mime().AllocLC();
        iPreviewMimeTypes.AppendL( mimetype );
        CleanupStack::Pop( mimetype );
        mimetype = 0;

        uri = aData.PreviewL( i ).Uri().AllocLC();
        iUris.AppendL( uri );
        CleanupStack::Pop( uri );
        uri = 0;        
        }

    // Try to update mime types from already downloaded previews
    // if the server didn't send any for this
    UpdateMimesFromPreviewManagerL();
    DLTRACEOUT((""));
    }


// Internalization from and externalization to the database
    
void CNcdNodePreview::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( ClassId() );
    
    aStream.WriteInt16L( iFileCount );
    
    for( int i = 0; i < iFileCount; i++ )
        {
        ExternalizeDesL( *iPreviewMimeTypes[i], aStream );
        }
    
    for( int i = 0; i < iFileCount; i++ )
        {
        ExternalizeDesL( *iUris[i], aStream );
        }
        
    DLTRACEOUT((""));
    }


void CNcdNodePreview::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read the class id first because it is set to the stream in internalize
    // function and it is not read from the stream anywhere else.
    TInt classId( aStream.ReadInt32L() );
    if ( classId != ClassId() )
        {
        DLERROR(("Wrong class id"));
        DASSERT( EFalse );
        // Leave because the stream does not match this class object
        User::Leave( KErrCorrupt );
        }
    
    iFileCount = aStream.ReadInt16L();

    HBufC* previewMimeType = 0;
    iPreviewMimeTypes.ResetAndDestroy();
    for( int i = 0; i < iFileCount; i++ )
        {
        InternalizeDesL( previewMimeType, aStream );
        CleanupStack::PushL( previewMimeType );
        iPreviewMimeTypes.AppendL( previewMimeType );
        CleanupStack::Pop( previewMimeType );
        }
        
    HBufC* uri = 0;
    iUris.ResetAndDestroy();
    for( int i = 0; i < iFileCount; i++ )
        {
        InternalizeDesL( uri, aStream );
        CleanupStack::PushL( uri );
        iUris.AppendL( uri );
        CleanupStack::Pop( uri );
        }
    
    // Try to update MIME from downloaded previews
    UpdateMimesFromPreviewManagerL();
    DLTRACEOUT((""));
    }


void CNcdNodePreview::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                  TInt aFunctionNumber )
    {
    DLTRACEIN((""));    

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdPreviewOpenFile:
            TRAP( trapError, OpenPreviewFileL( *aMessage ) );
            break;
        
        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;
                    
        case NcdNodeFunctionIds::ENcdIsPreviewLoaded:
            TRAP( trapError, IsPreviewLoadedL( *aMessage ));
            break;
        
        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }

void CNcdNodePreview::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }
                

void CNcdNodePreview::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodePreview::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }

    aStream.WriteInt32L( ClassId() );
    
    aStream.WriteInt16L( iFileCount );

    for( int i = 0; i < iFileCount; i++ )
        {
        ExternalizeDesL( *iPreviewMimeTypes[i], aStream );
        }
    
    DLTRACEOUT((""));
    }

void CNcdNodePreview::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }

void CNcdNodePreview::OpenPreviewFileL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );
    
    // Read the requested file index    
    TInt fileIndex = Des8ToInt( buf );

    // Open a new file server session because using the same session
    // for shared and non-shared files is a security risk     
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );

    // Share the file server session
    User::LeaveIfError( fs.ShareProtected() );

    DLTRACE(("Open preview file"));
    RFile openFile = iNodeManager.PreviewManager().PreviewL( 
            fs,
            iParentMetaData.Identifier(), 
            *iUris[fileIndex] );
            
    CleanupClosePushL( openFile );    
    
    DLINFO(("File open, transferring to client"));
    aMessage.CompleteAndReleaseL( fs, 
                                  openFile );

    // a shared file must be closed on the server side too
    // also the shared file session must be closed 
    CleanupStack::PopAndDestroy( 2, &fs ); // openFile, fs
    
    CleanupStack::PopAndDestroy( &buf );                    
    
    DLTRACEOUT((""));
    }


TInt CNcdNodePreview::UriCount() const
    {
    return iUris.Count();
    }


const TDesC& CNcdNodePreview::Uri( TInt aIndex ) const
    {
    return *(iUris[aIndex]);
    }


void CNcdNodePreview::UpdateMimesFromPreviewManagerL()
    {
    DLTRACEIN((""));
    CNcdPreviewManager& previewManager( iNodeManager.PreviewManager() );
    const CNcdNodeIdentifier& metaId( iParentMetaData.Identifier() );
    
    TInt count = iFileCount; // use iFileCount since it's the "real" count
    while ( count-- ) 
        {
        if ( !iPreviewMimeTypes[ count ]->Length() ) 
            {            
            const TDesC& mime( previewManager.PreviewMimeType( 
                metaId,
                *iUris[ count ] ) );
                
            // assign only if there's something to assign
            if ( mime.Length() ) 
                {                
                AssignDesL( iPreviewMimeTypes[ count ], mime );
                }
            }        
        }
    }


void CNcdNodePreview::IsPreviewLoadedL( MCatalogsBaseMessage& aMessage )
    {
    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );

    TInt fileIndex = Des8ToInt( buf );

    TInt isLoaded = iNodeManager.PreviewManager().PreviewExists( 
        iParentMetaData.Identifier(), 
        *iUris[fileIndex] );
    
    CleanupStack::PopAndDestroy( &buf );                    
    
    aMessage.CompleteAndReleaseL( isLoaded, KErrNone );                                  
    
    }
