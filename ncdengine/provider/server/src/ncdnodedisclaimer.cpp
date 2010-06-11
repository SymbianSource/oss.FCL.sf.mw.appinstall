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
* Description:   Implements CNcdNodeDisclaimer class
*
*/


#include "ncdnodedisclaimer.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "ncdstring.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


CNcdNodeDisclaimer::CNcdNodeDisclaimer( NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdCommunicable(),
  iClassId( aClassId )
    {
    }

void CNcdNodeDisclaimer::ConstructL()
    {
    // These values has to be set. 
    iTitle = KNullDesC().AllocL();
    iBodyText = KNullDesC().AllocL();
    }
    
void CNcdNodeDisclaimer::ConstructL( const CNcdNodeDisclaimer& aDisclaimer ) 
    {
    iClassId = aDisclaimer.iClassId;
    iMessage = NULL;
    
    AssignDesL( iTitle, *aDisclaimer.iTitle );
    AssignDesL( iBodyText, *aDisclaimer.iBodyText );
    
    iOptional = aDisclaimer.iOptional;
    iSemantics = aDisclaimer.iSemantics;
    }


CNcdNodeDisclaimer* CNcdNodeDisclaimer::NewL()
    {
    CNcdNodeDisclaimer* self =   
        CNcdNodeDisclaimer::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeDisclaimer* CNcdNodeDisclaimer::NewLC()
    {
    CNcdNodeDisclaimer* self = 
        new( ELeave ) CNcdNodeDisclaimer( 
            NcdNodeClassIds::ENcdNodeDisclaimerClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }
    
CNcdNodeDisclaimer* CNcdNodeDisclaimer::NewL( const CNcdNodeDisclaimer& aDisclaimer ) 
    {
    CNcdNodeDisclaimer* self = NewLC( aDisclaimer );
    CleanupStack::Pop( self );
    return self;
    }


CNcdNodeDisclaimer* CNcdNodeDisclaimer::NewLC( const CNcdNodeDisclaimer& aDisclaimer ) 
    {
    CNcdNodeDisclaimer* self = new( ELeave ) CNcdNodeDisclaimer(
        NcdNodeClassIds::ENcdNodeDisclaimerClassId );
    CleanupClosePushL( *self );
    self->ConstructL( aDisclaimer );    
    return self;
    }


CNcdNodeDisclaimer::~CNcdNodeDisclaimer()
    {
    DLTRACEIN((""));

    // Delete member variables here

    delete iTitle;
    iTitle = NULL;
    
    delete iBodyText;
    iBodyText = NULL;

    DLTRACEOUT((""));
    }        
    
    
// Internalization from the protocol

void CNcdNodeDisclaimer::InternalizeL( const MNcdConfigurationProtocolQuery& aData )
    {
    DLTRACEIN((""));

    // First create the new values

    HBufC* tmpTitle = aData.Title().Data().AllocLC();
    HBufC* tmpBodyText = aData.BodyText().Data().AllocLC();

    // Now, that we are sure we have correctly created new values and the
    // code cannot leave here, set the tmp values to the member variables.

    delete iBodyText;
    iBodyText = tmpBodyText;
    CleanupStack::Pop( tmpBodyText );

    delete iTitle;
    iTitle = tmpTitle;
    CleanupStack::Pop( tmpTitle );

    iOptional = aData.Optional();
    iSemantics = aData.Semantics();

    DLTRACEOUT((""));
    }


// Internalization from and externalization to the database
    
void CNcdNodeDisclaimer::ExternalizeL( RWriteStream& aStream )
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

    ExternalizeDesL( *iTitle, aStream );
    ExternalizeDesL( *iBodyText, aStream );
    aStream.WriteInt32L( iOptional );
    aStream.WriteInt32L( iSemantics );
    
    DLTRACEOUT((""));
    }


void CNcdNodeDisclaimer::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read the class id out from the stream since it's not read 
    // anywhere else
    aStream.ReadInt32L();
    
    // Take data to the temporary variables    
    
    HBufC* tmpTitle( NULL );
    HBufC* tmpBodyText( NULL );

    InternalizeDesL( tmpTitle, aStream );
    CleanupStack::PushL( tmpTitle );
    
    InternalizeDesL( tmpBodyText, aStream );
    CleanupStack::PushL( tmpBodyText );

    TBool tmpOptional = aStream.ReadInt32L();
    
    // We can be sure that enumerations can be converted to integer values.
    MNcdQuery::TSemantics tmpSemantics = 
        static_cast<MNcdQuery::TSemantics>(aStream.ReadInt32L());

    // Because there was enough memory and this function will not
    // leave anymore, we may safely insert new values into the
    // member variables.

    delete iBodyText;
    iBodyText = tmpBodyText;
    CleanupStack::Pop( tmpBodyText );

    delete iTitle;
    iTitle = tmpTitle;
    CleanupStack::Pop( tmpTitle );

    iOptional = tmpOptional;    
    iSemantics = tmpSemantics;
    

    DLTRACEOUT((""));
    }


void CNcdNodeDisclaimer::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

void CNcdNodeDisclaimer::CounterPartLost( const MCatalogsSession& aSession )
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
                

void CNcdNodeDisclaimer::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
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


    // If this leaves, ReceiveMessge will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure taht ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeDisclaimer::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    
    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }
    
    ExternalizeL( aStream );
        
    DLTRACEOUT((""));
    }

void CNcdNodeDisclaimer::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


