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
* Description:   Implements CNcdNodeuriContent class
*
*/


#include "ncdnodeuricontentimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_download.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdpurchasedetails.h"
#include "ncdutils.h"


CNcdNodeUriContent::CNcdNodeUriContent( NcdNodeClassIds::TNcdNodeClassId aClassId )
: CCatalogsCommunicable(),
  iClassId( aClassId ),
  iValidityDelta( -1 )
    {
    }

void CNcdNodeUriContent::ConstructL()
    {
    iUriContent = KNullDesC().AllocL();
    }


CNcdNodeUriContent* CNcdNodeUriContent::NewL()
    {
    CNcdNodeUriContent* self =   
        CNcdNodeUriContent::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeUriContent* CNcdNodeUriContent::NewLC()
    {
    CNcdNodeUriContent* self = 
        new( ELeave ) CNcdNodeUriContent( 
            NcdNodeClassIds::ENcdNodeUriContentClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeUriContent::~CNcdNodeUriContent()
    {
    DLTRACEIN((""));

    delete iUriContent;
    iUriContent = NULL;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeUriContent::ClassId() const
    {
    return iClassId;
    }


// ---------------------------------------------------------------------------
// Internalization from purchase details 
// ---------------------------------------------------------------------------
//
TBool CNcdNodeUriContent::InternalizeL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN(("this-ptr: %X", this));

    TArray<MNcdPurchaseDownloadInfo*> dlInfo ( aDetails.DownloadInfoL() );
    
    TInt index = KErrNotFound;                
    for ( TInt i = 0; i < dlInfo.Count(); ++i )
        {
        if ( dlInfo[i]->ContentUsage() == MNcdPurchaseDownloadInfo::EConsumable )
            {
            DLINFO(("Download in index %d is consumable!", i));
            index = i;
            // Only one uri is supported so break immediately as we
            // come across it
            break;
            }
        }
    
    // Leaves if index is still KErrNotFound, positive indices don't leave
    if ( index == KErrNotFound )
        {
        return EFalse;
        }

    DLTRACE(("Copying content URI"));
    HBufC* uri = dlInfo[index]->ContentUri().AllocL();
    delete iUriContent;
    iUriContent = uri;
    iValidityDelta = dlInfo[index]->ContentValidityDelta();
    DLTRACEOUT(( _L("Content URI: %S"), iUriContent ));
    
    return ETrue;
    }


// Internalization from and externalization to the database
    
void CNcdNodeUriContent::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( ClassId() );
    
    ExternalizeDesL( *iUriContent, aStream );
    
    aStream.WriteInt32L( iValidityDelta );

    DLTRACEOUT((""));
    }


void CNcdNodeUriContent::InternalizeL( RReadStream& aStream )
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

    InternalizeDesL( iUriContent, aStream );

    iValidityDelta = aStream.ReadInt32L();
    
    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// URI getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdNodeUriContent::Uri() const
    {    
    DASSERT( iUriContent );
    DLTRACEIN(( _L("%S"), iUriContent ));
    return *iUriContent;
    }


void CNcdNodeUriContent::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

void CNcdNodeUriContent::CounterPartLost( const MCatalogsSession& aSession )
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
                

void CNcdNodeUriContent::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
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
    

void CNcdNodeUriContent::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    ExternalizeL( aStream ); 
        
    DLTRACEOUT((""));
    }

void CNcdNodeUriContent::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


