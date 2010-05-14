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
* Description:   Implements CNcdNodeSkin class
*
*/


#include "ncdnodeskinimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncd_pp_skin.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


CNcdNodeSkin* CNcdNodeSkin::NewL()
    {
    CNcdNodeSkin* self =   
        CNcdNodeSkin::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeSkin* CNcdNodeSkin::NewLC()
    {
    CNcdNodeSkin* self = 
        new( ELeave ) CNcdNodeSkin( 
            NcdNodeClassIds::ENcdNodeSkinClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeSkin::~CNcdNodeSkin()
    {
    DLTRACEIN((""));

    // Delete member variables here

    delete iId;
    iId = NULL;

    delete iTimeStamp;
    iTimeStamp = NULL;

    delete iUri;
    iUri = NULL;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeSkin::ClassId() const
    {
    return iClassId;
    }


// Internalization from the protocol

void CNcdNodeSkin::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    // First create the new values

    HBufC* tmpId = aData.Skin()->Id().AllocLC();
    DLTRACE((_L("id: %S"), tmpId));

    HBufC* tmpTimeStamp = aData.Skin()->Timestamp().AllocLC();
    DLTRACE((_L("timestamp: %S"), tmpTimeStamp));

    HBufC* tmpUri = aData.Skin()->Uri().AllocLC();
    DLTRACE((_L("uri: %S"), tmpUri));


    // Now, that we are sure we have correctly created new values and the
    // code cannot leave here, set the tmp values to the member variables.

    delete iUri;
    iUri = tmpUri;
    CleanupStack::Pop( tmpUri );

    delete iTimeStamp;
    iTimeStamp = tmpTimeStamp;
    CleanupStack::Pop( tmpTimeStamp );

    delete iId;
    iId = tmpId;
    CleanupStack::Pop( tmpId );

    DLTRACEOUT(( _L("SkinId: %S"), iId ));
    }


// Internalization from and externalization to the database
    
void CNcdNodeSkin::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( iClassId );
    
    
    // Write the data that will be used when internalize function
    // is called. catalogsutils.h contains good internalize and
    // externalize functions.
    // Make sure that this matches to the order that is used 
    // when this stream is internalized.

    ExternalizeDesL( *iId, aStream );
    ExternalizeDesL( *iTimeStamp, aStream );
    ExternalizeDesL( *iUri, aStream );

    DLTRACEOUT((""));
    }


void CNcdNodeSkin::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read the class id first because it is set to the stream in internalize
    // function and it is not read from the stream anywhere else.
    TInt classId( aStream.ReadInt32L() );
    if ( classId != ClassId() )
        {
        DLTRACE(("Wrong class id"));
        DASSERT( EFalse );
        // Leave because the stream does not match this class object
        User::Leave( KErrCorrupt );
        }
    
    InternalizeDesL( iId, aStream );
    InternalizeDesL( iTimeStamp, aStream );
    InternalizeDesL( iUri, aStream );

    DLTRACEOUT(( _L("SkinId: %S"), iId ));
    }



void CNcdNodeSkin::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

void CNcdNodeSkin::CounterPartLost( const MCatalogsSession& aSession )
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
                
const TDesC& CNcdNodeSkin::SkinId() const
    {
    DASSERT( iId );
    return *iId;
    }

const TDesC& CNcdNodeSkin::TimeStamp() const
    {
    DASSERT( iTimeStamp );
    return *iTimeStamp;
    }

void CNcdNodeSkin::SetTimeStampL( const TDesC& aTimeStamp )
    {
    AssignDesL( iTimeStamp, aTimeStamp );
    }

const TDesC& CNcdNodeSkin::Uri() const
    {
    DASSERT( iUri );
    return *iUri;
    }


CNcdNodeSkin::CNcdNodeSkin( NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdCommunicable(),
  iClassId( aClassId )
    {
    }

void CNcdNodeSkin::ConstructL()
    {
    iId = KNullDesC().AllocL();
    iTimeStamp = KNullDesC().AllocL();
    iUri = KNullDesC().AllocL();
    }



void CNcdNodeSkin::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary data to the stream.
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
    

void CNcdNodeSkin::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }
        
    DLINFO(( _L("SkinID: %S"), iId ));
    // Skin existed. So, insert info that meta data was found.
    aStream.WriteInt32L( ClassId() );

    // Add additional content to the stream.
    // Make sure that this matches to the order that is used in the proxy
    // side when this stream is internalized.
    // NOTE: Be careful with the 8- and 16-bit descriptors. Remember to check
    // if the proxy wants the data in 16 or 8 bits?    

    ExternalizeDesL( SkinId(), aStream );
    ExternalizeDesL( TimeStamp(), aStream );
        
    DLTRACEOUT((""));
    }

void CNcdNodeSkin::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


