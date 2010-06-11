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
* Description:   Implements CNcdNodeScreenshot class
*
*/


#include "ncdnodescreenshotimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodemanager.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_downloadimpl.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


CNcdNodeScreenshot::CNcdNodeScreenshot(
    NcdNodeClassIds::TNcdNodeClassId aClassId,
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData )
: CNcdCommunicable(),
  iClassId( aClassId ),
  iNodeManager( aNodeManager ),
  iParentMetaData( aParentMetaData )
    {
    }

void CNcdNodeScreenshot::ConstructL()
    {
    }


CNcdNodeScreenshot* CNcdNodeScreenshot::NewL(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeScreenshot* self =   
        CNcdNodeScreenshot::NewLC( aNodeManager, aParentMetaData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeScreenshot* CNcdNodeScreenshot::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeScreenshot* self = 
        new( ELeave ) CNcdNodeScreenshot( 
            NcdNodeClassIds::ENcdNodeScreenshotClassId,
            aNodeManager,
            aParentMetaData );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeScreenshot::~CNcdNodeScreenshot()
    {
    DLTRACEIN((""));

    // Delete member variables here

    iScreenshotDownload.ResetAndDestroy();

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeScreenshot::ClassId() const
    {
    return iClassId;
    }

TInt CNcdNodeScreenshot::ScreenshotDownloadCount() const
    {
    return iScreenshotDownload.Count();
    }

const TDesC&
    CNcdNodeScreenshot::ScreenshotDownloadUri( TInt aIndex ) const
    {
    return iScreenshotDownload[aIndex]->Key();
    }

// Internalization from the protocol

void CNcdNodeScreenshot::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    iScreenshotDownload.ResetAndDestroy();
    
    for ( TInt i = 0; i < aData.ScreenshotCount(); i++ )
        {
        CNcdKeyValuePair* download = CNcdKeyValuePair::NewLC(
            aData.ScreenshotL( i ).Uri(),
            aData.ScreenshotL( i ).Mime() );
        
        DLINFO(( _L("Screenshot URI: %S, mime: %S"), 
            &download->Key(),
            &download->Value() ));
            
        iScreenshotDownload.AppendL( download );
        CleanupStack::Pop( download );
        }

    DLTRACEOUT((""));
    }


// Internalization from and externalization to the database
    
void CNcdNodeScreenshot::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( ClassId() );
    
    
    // Write the data that will be used when internalize function
    // is called. catalogsutils.h contains good internalize and
    // externalize functions.
    // Make sure that this matches to the order that is used 
    // when this stream is internalized.

    TInt count = iScreenshotDownload.Count();
    aStream.WriteInt32L( count );

    DLTRACE(("Externalizing %d screenshot URIs and mimes", count ));
    for ( TInt i = 0; i < count; ++i )
        {
        iScreenshotDownload[i]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeScreenshot::InternalizeL( RReadStream& aStream )
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
    
    TInt count = aStream.ReadInt32L();
    DLTRACE(("Internalizing %d screenshot URIs and mimes", count ));
    
    iScreenshotDownload.ResetAndDestroy();
    iScreenshotDownload.ReserveL( count );
    
    while ( count-- )
        {
        CNcdKeyValuePair* screenshot = CNcdKeyValuePair::NewLC( aStream );
        iScreenshotDownload.Append( screenshot );
        CleanupStack::Pop( screenshot );
        }
    
    DLTRACEOUT((""));
    }



void CNcdNodeScreenshot::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

        case NcdNodeFunctionIds::ENcdScreenshotData:
            // Proxy side requested screenshot data.
            TRAP( trapError, ScreenshotDataRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
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

void CNcdNodeScreenshot::CounterPartLost( const MCatalogsSession& aSession )
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
                

void CNcdNodeScreenshot::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
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
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeScreenshot::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }

    // Make sure that the id is internalized correctly in the proxy side.
    aStream.WriteInt32L( ClassId() );

    // Add additional content to the stream.
    // Make sure that this matches to the order that is used in the proxy
    // side when this stream is internalized.
    // NOTE: Be careful with the 8- and 16-bit descriptors. Remember to check
    // if the proxy wants the data in 16 or 8 bits?    

    TInt count = iScreenshotDownload.Count();
    
    aStream.WriteInt32L( count );
    
    for ( TInt i = 0; i < count; i++ )
        {
        ExternalizeDesL( iScreenshotDownload[i]->Value(), aStream );
        }
        
    DLTRACEOUT((""));
    }


void CNcdNodeScreenshot::ScreenshotDataRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // If this leaves, ReceiveMessage will complete the message.    

    TInt inputLength = aMessage.InputLength();
    User::LeaveIfError( inputLength );
    
    HBufC8* data = HBufC8::NewLC( inputLength );
    TPtr8 ptr = data->Des();
    User::LeaveIfError( aMessage.ReadInput( ptr ) );

    TInt index = Des8ToInt( *data );

    CleanupStack::PopAndDestroy( data );
    
    CNcdNodeIdentifier* shotIdentifier = 
        CNcdNodeIdentifier::NewLC( iParentMetaData.Identifier().NodeNameSpace(),
                                   iScreenshotDownload[index]->Key(),
                                   iScreenshotDownload[index]->Key(),
                                   iParentMetaData.Identifier().ClientUid() );
    HBufC8* screenshot = iNodeManager.DbScreenshotDataLC( *shotIdentifier );
        
    aMessage.CompleteAndReleaseL( *screenshot, KErrNone );

    CleanupStack::PopAndDestroy( screenshot );
    CleanupStack::PopAndDestroy( shotIdentifier );    
    
    DLTRACEOUT((""));
    }
    

void CNcdNodeScreenshot::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


