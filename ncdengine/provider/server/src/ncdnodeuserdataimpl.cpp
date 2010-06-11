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
* Description:   Implements CNcdNodeUserData class
*
*/


#include "ncdnodeuserdataimpl.h"
#include "ncdnodemanager.h"
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


CNcdNodeUserData::CNcdNodeUserData( NcdNodeClassIds::TNcdNodeClassId aClassId,
                                    CNcdNodeManager& aManager )
: CCatalogsCommunicable(),
  iClassId( aClassId ),
  iManager( aManager )
    {
    }

void CNcdNodeUserData::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));

    // These values have to be set.

    iIdentifier = 
        CNcdNodeIdentifier::NewL( aIdentifier );


    // Notice that the user data is left here to be NULL.
    // The value is set from db during the first query for the user data.
    // This is checked by comparing the user data value to NULL.
    // Also, notice that the Internalization can not be done here because
    // this class object may be created from the node when it is been internalized.
    // Multiple internalizations from the same db at the same time are not allowed.

    DLTRACEOUT((""));
    }


CNcdNodeUserData* CNcdNodeUserData::NewL( const CNcdNodeIdentifier& aIdentifier,
                                          CNcdNodeManager& aManager )
    {
    CNcdNodeUserData* self =   
        CNcdNodeUserData::NewLC( aIdentifier, aManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeUserData* CNcdNodeUserData::NewLC( const CNcdNodeIdentifier& aIdentifier,
                                           CNcdNodeManager& aManager )
    {
    CNcdNodeUserData* self = 
        new( ELeave ) CNcdNodeUserData( 
            NcdNodeClassIds::ENcdNodeUserDataClassId, aManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }


CNcdNodeUserData::~CNcdNodeUserData()
    {
    DLTRACEIN((""));

    // Delete member variables here
    delete iIdentifier;
    iIdentifier = NULL;
    
    delete iUserData;
    iUserData = NULL;

    // Do not delete manager because this class object does not own it.
    
    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeUserData::ClassId() const
    {
    return iClassId;
    }


void CNcdNodeUserData::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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
        case NcdNodeFunctionIds::ENcdUserData:
            TRAP( trapError, UserDataRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdSetUserData:
            TRAP( trapError, SetUserDataRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdClearUserData:
            TRAP( trapError, ClearUserDataRequestL( *aMessage ) );
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

void CNcdNodeUserData::CounterPartLost( const MCatalogsSession& aSession )
    {
    DLTRACEIN((""));

    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }

    DLTRACEOUT((""));    
    }


void CNcdNodeUserData::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN(("Data: %S", iUserData));

    if ( iUserData != NULL )
        {
        // Write the class id to the stream in case it should be checked sometime.
        aStream.WriteInt32L( iClassId );

        // Just insert the user data into the stream
        ExternalizeDesL( *iUserData, aStream );        
        }

    DLTRACEOUT((""));    
    }


void CNcdNodeUserData::InternalizeL( RReadStream& aStream )
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
    
    // Only the user data is part of the stream
    // If the length was set to zero, then this function will set the value
    // to KNullDesC8. So, after internalization the iUserData will not be NULL.
    InternalizeDesL( iUserData, aStream );        

    DLTRACEOUT(("Data: %S", iUserData));
    
    }


void CNcdNodeUserData::UserDataRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    if ( iUserData == NULL )
        {
        TInt trapError( KErrNone );
        
        // This function will read data from the database and call
        // InternalizeL which sets the data to the user data variable.    
        TRAP( trapError,
              iManager.DbLoadUserDataL( *iIdentifier,
                                        *this ) );

        if ( trapError == KErrNotFound )
            {
            // User data was not found from the db.
            // So, set it to KNullDesC8
            iUserData = KNullDesC8().AllocL();
            }
        else if ( trapError != KErrNone )
            {        
            // Only KErrNotFound is allowed error.
            // Otherwise leave if error occurred.
            User::Leave( trapError );
            }
        }
                                          
    // iUserData is always up to date because the data is read from the db
    // when this object is created. Also, when the data is updated the latest
    // info will be set into the iUserData same time when it is updated to the
    // db.

    // Send complete information back to proxy.
    aMessage.CompleteAndReleaseL( *iUserData, KErrNone );
        
    DLTRACEOUT((""));
    }

void CNcdNodeUserData::SetUserDataRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Get the session that will contain the user data
    MCatalogsSession& requestSession( aMessage.Session() );

    // Get the user data
    HBufC8* data = HBufC8::NewLC( aMessage.InputLength() );
    
    // Read data that proxy sent into the created buffer.
    TPtr8 dataPtr( data->Des() );
    TInt err = aMessage.ReadInput( dataPtr );

    User::LeaveIfError( err );

    DLTRACE(("Replace old information"));
    
    // Replace old information by the new.
    delete iUserData;
    iUserData = data;
    CleanupStack::Pop( data );
        
    // ExternalizeL of this class object will be called 
    // and the iUserData is saved to the stream there.
    iManager.DbSaveUserDataL( *iIdentifier, *this );        

    DLTRACE(("Complete message"));
    
    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
    
    DLTRACEOUT((""));
    }

void CNcdNodeUserData::ClearUserDataRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Clear the db data using the manager.
    iManager.DbRemoveUserDataL( *iIdentifier ); 

    // Also, delete the old info from here and the user data empty.
    HBufC8* tmpUserData = KNullDesC8().AllocL();
    delete iUserData;
    iUserData = tmpUserData;

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
            
    DLTRACEOUT((""));
    }
                
void CNcdNodeUserData::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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
