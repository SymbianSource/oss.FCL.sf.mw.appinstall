/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdNodeContentInfo class
*
*/


#include "ncdnodecontentinfoimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_dataentitycontent.h"
#include "ncd_cp_query.h"
#include "ncditempurpose.h"
#include "catalogsutils.h"
#include "ncderrors.h"
#include "ncdproviderutils.h"
#include "ncdinstallationservice.h"
#include "ncdpurchasedetails.h"

#include "catalogsdebug.h"
#include "catalogsconstants.h"


CNcdNodeContentInfo::CNcdNodeContentInfo( NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdCommunicable(),
  iClassId( aClassId )
    {
    }

void CNcdNodeContentInfo::ConstructL()
    {
    iMimeType = KNullDesC().AllocL();
    iVersion = KNullDesC().AllocL();
    iIdentifier = KNullDesC().AllocL();
    }


CNcdNodeContentInfo* CNcdNodeContentInfo::NewL()
    {
    CNcdNodeContentInfo* self =
        CNcdNodeContentInfo::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeContentInfo* CNcdNodeContentInfo::NewLC()
    {
    CNcdNodeContentInfo* self = 
        new( ELeave ) CNcdNodeContentInfo( 
            NcdNodeClassIds::ENcdNodeContentInfoClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeContentInfo::~CNcdNodeContentInfo()
    {
    DLTRACEIN((""));

    delete iMimeType;
    iMimeType = NULL;

    delete iVersion;
    iVersion = NULL;
    
    delete iIdentifier;
    iIdentifier = NULL;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeContentInfo::ClassId() const
    {
    return iClassId;
    }

TUint CNcdNodeContentInfo::Purpose() const
    {
    return iPurpose;
    }

const TDesC& CNcdNodeContentInfo::MimeType() const
    {
    return *iMimeType;
    }

const TUid& CNcdNodeContentInfo::Uid() const
    {
    return iUid;
    }

const TDesC& CNcdNodeContentInfo::Identifier() const
    {
    return *iIdentifier;
    }

const TDesC& CNcdNodeContentInfo::Version() const
    {
    return *iVersion;
    }
    
TInt CNcdNodeContentInfo::TotalContentSize() const
    {
    return iSize;
    }


// Internalization from the protocol

void CNcdNodeContentInfo::InternalizeL(
    MNcdPreminetProtocolDataEntity& aData )
    {
    DLTRACEIN((""));

    if ( ! aData.DownloadableContent() )
        {
        DLERROR(("No content data present"));
        // Content info should not be internalized if there is no content info
        // available.
        DASSERT( EFalse );
        // Content data not present.
        User::Leave( KErrNotFound );
        }

    iPurpose = 0;

    delete iMimeType;
    iMimeType = NULL;

    iUid = TUid::Null();
    
    delete iIdentifier;
    iIdentifier = NULL;
    iIdentifier = KNullDesC().AllocL();  
    
    delete iVersion;
    iVersion = NULL;

    // Construct purpose.
    TInt purposeCount = aData.DownloadableContent()->ContentPurposeCount();
    for ( TInt i = 0; i < purposeCount; i++ )
        {
        const TDesC& purpose =
            aData.DownloadableContent()->ContentPurposeL( i );

        // Notice that the content purpose does not need to be case insensitive.
        // According to the specification the purposes should be given in all
        // lower case characters. So, the Compare function should be used here, 
        // which is much faster than using of CompareF function.
        if ( purpose.Compare( KNcdContentPurposeMusic ) == 0 )
            {
            iPurpose |= ENcdItemPurposeMusic;
            }
        else if ( purpose.Compare( KNcdContentPurposeRingtone ) == 0 )
            {
            iPurpose |= ENcdItemPurposeRingtone;
            }
        else if ( purpose.Compare( KNcdContentPurposeWallpaper ) == 0 )
            {
            iPurpose |= ENcdItemPurposeWallpaper;
            }
        else if ( purpose.Compare( KNcdContentPurposeVideo ) == 0 )
            {
            iPurpose |= ENcdItemPurposeVideo;
            }
        else if ( purpose.Compare( KNcdContentPurposeTheme ) == 0 )
            {
            iPurpose |= ENcdItemPurposeTheme;
            }
        else if ( purpose.Compare( KNcdContentPurposeApplication ) == 0 )
            {
            iPurpose |= ENcdItemPurposeApplication;
            }
        else if ( purpose.Compare( KNcdContentPurposeHtmlPage ) == 0 )
            {
            iPurpose |= ENcdItemPurposeHtmlPage;
            }
        else if ( purpose.Compare( KNcdContentPurposeGame ) == 0 )
            {
            iPurpose |= ENcdItemPurposeGame;
            }
        else if ( purpose.Compare( KNcdContentPurposeScreensaver ) == 0 )
            {
            iPurpose |= ENcdItemPurposeScreensaver;
            }
        else if ( purpose.Compare( KNcdContentPurposeStream ) == 0 )
            {
            iPurpose |= ENcdItemPurposeStream;
            }
        else
            {
            iPurpose |= ENcdItemPurposeUnknown;
            }
        }
    iMimeType = aData.DownloadableContent()->Mime().AllocL();

    if ( iPurpose & ENcdItemPurposeApplication )
        {
        if ( iMimeType->Compare(KMimeTypeMatchSisx) == 0 )
            {
            // sis package, convert the string to uid
            TInt uid = 0;
            TRAPD( err, uid = DesHexToIntL( aData.DownloadableContent()->Id() ) );
            if( err != KErrNone )
                {
                DLERROR(( _L("DownloadableContent()->Id() was not valid hex, using ZERO: %S"),
                           &aData.DownloadableContent()->Id() ));
                iUid.iUid = 0;
                }
            else
                {
                iUid.iUid = uid;
                }
            }
         
        if ( iMimeType->Compare( KMimeTypeMatchWidget ) == 0 )
            {
            // widget, save identifier
            iIdentifier = aData.DownloadableContent()->Id().AllocL();
            }
        }

    iVersion = aData.DownloadableContent()->Version().AllocL();
    iSize = aData.DownloadableContent()->Size();

    DLTRACEOUT((""));
    }


// Internalization from and externalization to the database
    
void CNcdNodeContentInfo::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( ClassId() );
    
    aStream.WriteUint16L( iPurpose );
    ExternalizeDesL( *iMimeType, aStream );
    
    // The code here must be sync with the Internalization code in CNcdNodeContentInfoProxy
    if ( iMimeType->Compare( KMimeTypeMatchWidget ) == 0 )
        {
        ExternalizeDesL( *iIdentifier, aStream );
        }
    else
        {
        aStream.WriteInt32L( iUid.iUid );
        }
    
    ExternalizeDesL( *iVersion, aStream );
    aStream.WriteInt32L( iSize );

    DLTRACEOUT((""));
    }


void CNcdNodeContentInfo::InternalizeL( RReadStream& aStream )
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
    
    iPurpose = aStream.ReadUint16L();
    InternalizeDesL( iMimeType, aStream );
    
    // This Internalize function is called when the node is loaded from cache. RReadStream is a sign of that.
    // So same logic here also. Check mimetype and then decide to save uid or identifier.
    if ( iMimeType->Compare( KMimeTypeMatchWidget ) == 0  )
        {
        InternalizeDesL( iIdentifier, aStream );
        }
    else
        {
        iUid.iUid = aStream.ReadInt32L();
        }
    
    InternalizeDesL( iVersion, aStream );
    iSize = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }


void CNcdNodeContentInfo::InternalizeL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    iPurpose = aDetails.ItemPurpose();
    iSize = aDetails.TotalContentSize();
    iUid.iUid = aDetails.AttributeInt32( 
        MNcdPurchaseDetails::EPurchaseAttributeContentUid );
    AssignDesL( iVersion, aDetails.Version() );    
    AssignDesL( iMimeType, aDetails.AttributeString( 
        MNcdPurchaseDetails::EPurchaseAttributeContentMimeType ) );
    }


void CNcdNodeContentInfo::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

void CNcdNodeContentInfo::CounterPartLost( const MCatalogsSession& aSession )
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
                

void CNcdNodeContentInfo::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
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
    

void CNcdNodeContentInfo::ExternalizeDataForRequestL( RWriteStream& aStream )
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

void CNcdNodeContentInfo::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


