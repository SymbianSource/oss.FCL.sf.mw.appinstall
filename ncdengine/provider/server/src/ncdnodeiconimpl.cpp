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
* Description:   Implements CNcdNodeIcon class
*
*/


#include "ncdnodeiconimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodemanager.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncd_pp_icon.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdnodeidentifier.h"
#include "ncderrors.h"
#include "ncdutils.h"


_LIT( KNcdPurchaseDetailsIconIdPrefix, "*PD_1c0n*");

CNcdNodeIcon* CNcdNodeIcon::NewL(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData,
    TBool aUsePurchaseDetailsIcon )
    {
    CNcdNodeIcon* self =   
        CNcdNodeIcon::NewLC( aNodeManager, aParentMetaData,
            aUsePurchaseDetailsIcon );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIcon* CNcdNodeIcon::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData,
    TBool aUsePurchaseDetailsIcon )
    {
    CNcdNodeIcon* self = 
        new( ELeave ) CNcdNodeIcon( 
            NcdNodeClassIds::ENcdNodeIconClassId,
            aNodeManager,
            aParentMetaData,
            aUsePurchaseDetailsIcon );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeIcon::CNcdNodeIcon(
    NcdNodeClassIds::TNcdNodeClassId aClassId,
    CNcdNodeManager& aNodeManager,
    const CNcdNodeMetaData& aParentMetaData,
    TBool aUsePurchaseDetailsIcon )
        : CNcdCommunicable(),
          iNodeManager( aNodeManager ),
          iParentMetaData( aParentMetaData ),
          iClassId( aClassId ),
          iIconDataReady ( ETrue ),
          iUsePurchaseDetailsIcon( aUsePurchaseDetailsIcon )
    {
    }

void CNcdNodeIcon::ConstructL()
    {
    iDataBlock = KNullDesC().AllocL();
    if( iUsePurchaseDetailsIcon )
        {
        iIconId = HBufC::NewL( KNcdPurchaseDetailsIconIdPrefix().Length() +
            iParentMetaData.Identifier().NodeId().Length() +
            iParentMetaData.Identifier().NodeNameSpace().Length() );
        iIconId->Des().Append( KNcdPurchaseDetailsIconIdPrefix );
        iIconId->Des().Append( iParentMetaData.Identifier().NodeId() );
        iIconId->Des().Append( iParentMetaData.Identifier().NodeNameSpace() );
        }
    else
        {
        iIconId = KNullDesC().AllocL();
        }
    iUri = KNullDesC().AllocL();
    }

CNcdNodeIcon::~CNcdNodeIcon()
    {
    DLTRACEIN((""));

    // Delete member variables here

    delete iDataBlock;
    iDataBlock = NULL;

    delete iIconId;
    iIconId = NULL;

    delete iUri;
    iUri = NULL;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeIcon::ClassId() const
    {
    return iClassId;
    }


// Internalization from the protocol

void CNcdNodeIcon::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    iUsePurchaseDetailsIcon = EFalse;
    if ( ! aData.Icon() )
        {
        // Icon data not present.
        User::Leave( KErrNotFound );
        }

    // First create the new values

    HBufC* tmpDataBlock = aData.Icon()->DataBlock().AllocLC();
    DLINFO((_L("datablock: %S"), tmpDataBlock));

    HBufC* tmpId = aData.Icon()->Id().AllocLC();
    DLINFO((_L("id: %S"), tmpId));

    HBufC* tmpUri = aData.Icon()->Uri().AllocLC();
    DLINFO((_L("uri: %S"), tmpUri));


    // Now, that we are sure we have correctly created new values and the
    // code cannot leave here, set the tmp values to the member variables.

    delete iUri;
    iUri = tmpUri;
    CleanupStack::Pop( tmpUri );

    delete iIconId;
    iIconId = tmpId;
    CleanupStack::Pop( tmpId );

    delete iDataBlock;
    iDataBlock = tmpDataBlock;
    CleanupStack::Pop( tmpDataBlock );
    
    // If icon data is obtained inline, save the data to database
    if ( aData.Icon()->Data() != KNullDesC8 ) 
        {
        CNcdNodeIdentifier* iconIdentifier =
            CNcdNodeIdentifier::NewLC( iParentMetaData.Identifier().NodeNameSpace(), 
                                       *iIconId,
                                       Uri(),
                                       iParentMetaData.Identifier().ClientUid() );
                                       
        iNodeManager.DbSaveIconDataL( *iconIdentifier,
                                      aData.Icon()->Data() );
                                    
        CleanupStack::PopAndDestroy( iconIdentifier );
        }

    DLTRACEOUT((""));
    }


// Internalization from and externalization to the database
    
void CNcdNodeIcon::ExternalizeL( RWriteStream& aStream )
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

    ExternalizeDesL( *iDataBlock, aStream );
    ExternalizeDesL( *iIconId, aStream );
    ExternalizeDesL( *iUri, aStream );
    aStream.WriteInt32L( iUsePurchaseDetailsIcon );

    DLTRACEOUT((""));
    }


void CNcdNodeIcon::InternalizeL( RReadStream& aStream )
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
    
    InternalizeDesL( iDataBlock, aStream );
    InternalizeDesL( iIconId, aStream );
    InternalizeDesL( iUri, aStream );
    iUsePurchaseDetailsIcon = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }



void CNcdNodeIcon::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

        case NcdNodeFunctionIds::ENcdIconData:
            // Get icon data.
            TRAP( trapError, IconDataRequestL( *aMessage ) );
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

void CNcdNodeIcon::CounterPartLost( const MCatalogsSession& aSession )
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
                
const TDesC& CNcdNodeIcon::DataBlock() const
    {
    DASSERT( iDataBlock );
    return *iDataBlock;
    }

const TDesC& CNcdNodeIcon::IconId() const
    {
    DASSERT( iIconId );
    return *iIconId;
    }
    
void CNcdNodeIcon::SetIconIdL( const TDesC& aIconId ) 
    {
    HBufC* newId = aIconId.AllocL();
    delete iIconId;
    iIconId = newId;
    }
    
const TDesC& CNcdNodeIcon::Uri() const
    {
    DASSERT( iUri );
    return *iUri;
    }
    
void CNcdNodeIcon::SetUriL( const TDesC& aUri ) 
    {
    HBufC* newUri = aUri.AllocL();
    delete iUri;
    iUri = newUri;
    }
    
TBool CNcdNodeIcon::IconDataReady() const 
    {
    return iIconDataReady;
    }
    
void CNcdNodeIcon::SetIconDataReady( TBool aValue ) 
    {
    iIconDataReady = aValue;
    }


void CNcdNodeIcon::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
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
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeIcon::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }
    
    // Icon existed. So, insert info that meta data was found.
    aStream.WriteInt32L( ClassId() );

    // Add additional content to the stream.
    // Make sure that this matches to the order that is used in the proxy
    // side when this stream is internalized.
    // NOTE: Be careful with the 8- and 16-bit descriptors. Remember to check
    // if the proxy wants the data in 16 or 8 bits?    

    ExternalizeDesL( *iIconId, aStream );
        
    DLTRACEOUT((""));
    }

void CNcdNodeIcon::IconDataRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // If this leaves, ReceiveMessage will complete the message.    

    if ( ! iIconId )
        {
        User::Leave( KErrNotFound );
        }

    HBufC8* icon = IconDataL();
    CleanupStack::PushL( icon );
        
    aMessage.CompleteAndReleaseL( *icon, KErrNone );
    
    CleanupStack::PopAndDestroy( icon );
    
    DLTRACEOUT((""));
    }

void CNcdNodeIcon::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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

HBufC8* CNcdNodeIcon::IconDataL() const
    {
    if( iUsePurchaseDetailsIcon )
        {
        // Get purchase details with icon.
        CNcdPurchaseDetails* purchaseDetails =
            iParentMetaData.PurchaseDetailsLC( ETrue );
        HBufC8* iconData = purchaseDetails->GetIconData();
        CleanupStack::PopAndDestroy( purchaseDetails );
        return iconData;
        }
    else
        {
        CNcdNodeIdentifier* iconIdentifier =
            CNcdNodeIdentifier::NewLC( iParentMetaData.Identifier().NodeNameSpace(), 
                                       *iIconId,
                                       Uri(),
                                       iParentMetaData.Identifier().ClientUid() );                 
        HBufC8* icon = iNodeManager.DbIconDataLC( *iconIdentifier );
        CleanupStack::Pop( icon );
        CleanupStack::PopAndDestroy( iconIdentifier );
        return icon;
        }
    }
