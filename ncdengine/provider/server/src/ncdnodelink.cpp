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
* Description:   Implements CNcdNodeLink class
*
*/


#include "ncdnodelink.h"
#include "ncdnodeimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncdnodeidentifier.h"
#include "ncdproviderdefines.h"
#include "catalogsconstants.h"
#include "ncd_pp_entityref.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"


CNcdNodeLink::CNcdNodeLink( CNcdNode& aNode,
                            NcdNodeClassIds::TNcdNodeClassId aClassId )
: CCatalogsCommunicable(),
  iClassId( aClassId ),
  iNode( aNode ),
  iLinkDownloadTime( KMinTInt64 )
    {
    }

void CNcdNodeLink::ConstructL()
    {
    iParentIdentifier = CNcdNodeIdentifier::NewL();
    iRequestParentIdentifier = CNcdNodeIdentifier::NewL();
    iMetaDataIdentifier = CNcdNodeIdentifier::NewL();
    iTimeStamp = KNullDesC().AllocL();
    iCatalogsSourceName = KNullDesC().AllocL();
    iServerUri = KNullDesC().AllocL();
    iRemoteUri = KNullDesC().AllocL();
    iMetadataTimeStamp = KNullDesC().AllocL();
    }

CNcdNodeLink::~CNcdNodeLink()
    {
    DLTRACEIN((""));

    // Do not delete node here because this link does not own it.

    delete iParentIdentifier;
    iParentIdentifier = NULL;
    
    delete iRequestParentIdentifier;
    iRequestParentIdentifier = NULL;
    
    delete iMetaDataIdentifier;
    iMetaDataIdentifier = NULL;

    delete iTimeStamp;
    iTimeStamp = NULL;

    delete iCatalogsSourceName;
    iCatalogsSourceName = NULL;
        
    delete iServerUri;
    iServerUri = NULL;
    
    delete iRemoteUri;
    iRemoteUri = NULL;
    
    DeletePtr( iMetadataTimeStamp );
    iQueries.Close();
    DLTRACEOUT((""));
    }        
    

NcdNodeClassIds::TNcdNodeClassId CNcdNodeLink::ClassId() const
    {
    return iClassId;
    }    
    

const TDesC& CNcdNodeLink::Timestamp() const
    {
    return *iTimeStamp;
    }

const TDesC& CNcdNodeLink::ServerUri() const
    {
    return *iServerUri;
    }
    
void CNcdNodeLink::SetServerUriL( const TDesC& aServerUri )
    {    
    AssignDesL( iServerUri, aServerUri );
    }

const TDesC& CNcdNodeLink::RemoteUri() const
    {
    return *iRemoteUri;
    }

MNcdPreminetProtocolEntityRef::TDescription CNcdNodeLink::Description() const
    {
    return iDescription;
    }

TInt CNcdNodeLink::ValidUntilDelta() const
    {
    return iValidUntilDelta;
    }

void CNcdNodeLink::SetValidUntilDelta( TInt aValidUntilDelta )
    {
    iValidUntilDelta = aValidUntilDelta;
    iLinkDownloadTime.HomeTime();
    }

TBool CNcdNodeLink::ValidUntilAutoUpdate() const
    {
    return iValidUntilAutoUpdate;
    }


const CNcdNodeIdentifier& CNcdNodeLink::ParentIdentifier() const
    {
    // Parent identifier contains always at least the empty values.
    return *iParentIdentifier;
    }
    
void CNcdNodeLink::SetParentIdentifierL(
    const CNcdNodeIdentifier& aParentIdentifier ) 
    {
    CNcdNodeIdentifier* newParent = 
        CNcdNodeIdentifier::NewLC( aParentIdentifier );

    // Also set the request parent to correspond the same new identifier.
    // Notice that if the user wants these two differ. The request parent
    // identifier has to be reset after this function has been called.
    CNcdNodeIdentifier* newRequestParent =
        CNcdNodeIdentifier::NewLC( aParentIdentifier );

    delete iRequestParentIdentifier;
    iRequestParentIdentifier = newRequestParent;
    CleanupStack::Pop( newRequestParent );

    delete iParentIdentifier;
    iParentIdentifier = newParent;
    CleanupStack::Pop( newParent );
    }


const CNcdNodeIdentifier& CNcdNodeLink::RequestParentIdentifier() const
    {
    // Parent identifier contains always at least the empty values.
    return *iRequestParentIdentifier;
    }
    
void CNcdNodeLink::SetRequestParentIdentifierL(
    const CNcdNodeIdentifier& aRequestParentIdentifier ) 
    {
    CNcdNodeIdentifier* newParent = 
        CNcdNodeIdentifier::NewL( aRequestParentIdentifier );
    delete iRequestParentIdentifier;
    iRequestParentIdentifier = newParent;
    }


TTime CNcdNodeLink::LinkDownloadTime() const
    {
    return iLinkDownloadTime;
    }

TTime CNcdNodeLink::ExpiredTime() const
    {
    DLTRACEIN((""));

    // The expired time is the min value as default.
    // Then if the node data has not actually been loaded
    // this value is returned.
    TTime expired( KMinTInt64 );
        
    if ( ValidUntilDelta() == NcdProviderDefines::KNeverExpires )
        {
        // Because the delta value is set to never expire
        // value, the expire time should be as large as
        // possible
        DLINFO(("Delta was set to never expire. Largest expire time."));
        expired = KMaxTInt64;
        }
    else if ( LinkDownloadTime() != KMinTInt64 )
        {
        // The expiration time can be calculated because
        // the link has been downloaded and the delta value
        // has been given and it is not never expire value.
        DLINFO(("Calculate the expire time."));

        // The valid until delta is given in minutes.
        TTimeIntervalMinutes delta( ValidUntilDelta() );
        expired = LinkDownloadTime() + delta;
        
        if ( iTimeStamp->Length() && 
             iMetadataTimeStamp->Length() &&
             *iTimeStamp != *iMetadataTimeStamp ) 
            {
            DLINFO(("Link and metadata timestamps differ, expired"));
            expired = KMinTInt64;
            }
              
        }

    DLTRACEOUT(( "Expired time: %d", expired.Int64() ));
        
    return expired;
    }
    
TBool CNcdNodeLink::IsExpired() const
    {
    DLTRACEIN((""));

    TTime currentTime;
    currentTime.HomeTime();
    
    if ( ExpiredTime() <= currentTime )
        {
        DLTRACEOUT(( "Link is expired" ));
        return ETrue;
        }
    else
        {
        DLTRACEOUT(( "Link is not expired" ));
        return EFalse;
        } 
    }

void CNcdNodeLink::SetCatalogsSourceNameL( const TDesC& aSourceName )
    {
    HBufC* tmpSourceName = aSourceName.AllocL();
    delete iCatalogsSourceName;
    iCatalogsSourceName = tmpSourceName;
    }

const TDesC& CNcdNodeLink::CatalogsSourceName() const
    {
    return *iCatalogsSourceName;
    }


void CNcdNodeLink::SetRemoteFlag( TBool aRemoteFlag )
    {
    iRemoteFlag = aRemoteFlag;
    }

TBool CNcdNodeLink::RemoteFlag() const
    {
    return iRemoteFlag;
    }


const CNcdNodeIdentifier& CNcdNodeLink::MetaDataIdentifier() const
    {    
    DLNODEID(( *iMetaDataIdentifier ));    
    return *iMetaDataIdentifier;
    }

void CNcdNodeLink::SetMetaDataIdentifierL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* newIdentifier =
        CNcdNodeIdentifier::NewL( aIdentifier );
    delete iMetaDataIdentifier;
    iMetaDataIdentifier = newIdentifier;
    DLTRACEOUT((""));
    }


void CNcdNodeLink::SetMetadataTimeStampL( const TDesC& aTimeStamp )
    {
    AssignDesL( iMetadataTimeStamp, aTimeStamp );
    }


void CNcdNodeLink::InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                                 const CNcdNodeIdentifier& aParentIdentifier,
                                 const CNcdNodeIdentifier& aRequestParentIdentifier,
                                 const TUid& aClientUid )
    {
    DLTRACEIN((""));

    // Notice that we do not internalize the link id and namespace here
    // because this information is contained in the node-object that
    // wraps this link.
    // Also, note that the catalogs source name is not set here. That value
    // is set separately by using the setter function.
    
    HBufC* timeStamp = aData.Timestamp().AllocLC();
    HBufC* remoteUri = aData.RemoteUri().AllocLC();
    HBufC* serverUri = aData.ServerUri().AllocLC();
    
    CNcdNodeIdentifier* parentIdentifier = 
        CNcdNodeIdentifier::NewLC( aParentIdentifier );
    CNcdNodeIdentifier* requestParentIdentifier = 
        CNcdNodeIdentifier::NewLC( aRequestParentIdentifier );
    
     CNcdNodeIdentifier* metaDataIdentifier =
        CNcdNodeIdentifier::NewLC( aData.Namespace(),
                                   aData.Id(),
                                   aData.ServerUri(),
                                   aClientUid );
    
        
    // This may be a little bit risky if the system runs out of memory.
    // Then, the array may contain only part of the actual queries.
    
    iQueries.Reset();    
    const RArray<TInt>& queries = aData.Queries();
    for ( TInt i = 0; i < queries.Count(); ++i )
        {
        DLTRACE(("Query %d: %d", i, queries[ i ]));
        iQueries.AppendL( queries[ i ] );
        }

    // The function will not leave after this. So, it is safe to replace
    // old info by new.


    delete iMetaDataIdentifier;
    iMetaDataIdentifier = metaDataIdentifier;
    CleanupStack::Pop( metaDataIdentifier );
    
    delete iRequestParentIdentifier;
    iRequestParentIdentifier = requestParentIdentifier;
    CleanupStack::Pop( requestParentIdentifier );     

    delete iParentIdentifier;
    iParentIdentifier = parentIdentifier;
    CleanupStack::Pop( parentIdentifier );
    

    if ( *serverUri != KNullDesC )
        {
        // Do not delete this value if it already exists. This is set
        // for the root children from the operation.
        delete iServerUri;
        iServerUri = serverUri;
        CleanupStack::Pop( serverUri );
        }

    delete iRemoteUri;
    iRemoteUri = remoteUri;
    CleanupStack::Pop( remoteUri );
    
    delete iTimeStamp;
    iTimeStamp = timeStamp;
    CleanupStack::Pop( timeStamp );
    DLTRACE(( _L("timestamp: %S"), iTimeStamp ));
    
    iDescription = aData.Description();
    iValidUntilDelta = aData.ValidUntilDelta();
    iValidUntilAutoUpdate = aData.ValidUntilAutoUpdate();

    // Set the download time to this exact moment.
    iLinkDownloadTime.HomeTime();
    DLTRACE(( "downloadTime: %d", iLinkDownloadTime.Int64() ));

    DLTRACEOUT(("Internalization went ok :)"));
    }


void CNcdNodeLink::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    aStream.WriteInt32L( iClassId );

    // Notice that we do not externalize the link id and namespace here
    // because this information is contained in the node-object that
    // wraps this link.

    ExternalizeDesL( Timestamp(), aStream );
    ExternalizeDesL( CatalogsSourceName(), aStream );
    ExternalizeDesL( RemoteUri(), aStream );
    ExternalizeDesL( ServerUri(), aStream );
    
    aStream << LinkDownloadTime().Int64();
    
    aStream.WriteInt32L( iDescription );
    aStream.WriteInt32L( iValidUntilDelta );
    aStream.WriteInt32L( iValidUntilAutoUpdate );
    aStream.WriteInt32L( RemoteFlag() );
            
    ParentIdentifier().ExternalizeL( aStream );
    RequestParentIdentifier().ExternalizeL( aStream );
    MetaDataIdentifier().ExternalizeL( aStream );
    
    aStream.WriteInt32L( iQueries.Count() );
    for ( TInt i = 0; i < iQueries.Count(); ++i )
        {
        aStream.WriteInt32L( iQueries[ i ] );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeLink::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read all the membervariable values from the stream and
    // set the values for this object.

    // First get the data to the tmp variables. So, we can be sure
    // that there is enough memory to set the member variables.
        
    HBufC* timeStamp( NULL );
    HBufC* catalogsSourceName( NULL );
    HBufC* remoteUri( NULL );
    HBufC* serverUri( NULL );
    TInt64 linkDownloadTime( 0 );    

    TInt description( 0 );
    TInt validUntilDelta( 0 );
    TBool validUntilAutoUpdate( EFalse );
    TBool remoteFlag( RemoteFlag() );

    CNcdNodeIdentifier* parentIdentifier( NULL );
    CNcdNodeIdentifier* requestParentIdentifier( NULL );
    CNcdNodeIdentifier* metaDataIdentifier( NULL );
        
    InternalizeDesL( timeStamp, aStream );
    CleanupStack::PushL( timeStamp );

    InternalizeDesL( catalogsSourceName, aStream );
    CleanupStack::PushL( catalogsSourceName );
    
    InternalizeDesL( remoteUri, aStream );
    CleanupStack::PushL( remoteUri );
    
    InternalizeDesL( serverUri, aStream );
    CleanupStack::PushL( serverUri );

    aStream >> linkDownloadTime;
    
    description = aStream.ReadInt32L();
    validUntilDelta = aStream.ReadInt32L();
    validUntilAutoUpdate = aStream.ReadInt32L();
    remoteFlag = aStream.ReadInt32L();
    
    // The parent identifier will contain at least empty values.
    parentIdentifier = CNcdNodeIdentifier::NewLC( aStream );
    requestParentIdentifier = CNcdNodeIdentifier::NewLC( aStream );
    metaDataIdentifier = CNcdNodeIdentifier::NewLC( aStream );

    // Now that descriptors were succesfully created we
    // may delete old info and replaced it by new data.

    delete iMetaDataIdentifier;
    iMetaDataIdentifier = metaDataIdentifier;
    CleanupStack::Pop( metaDataIdentifier );
    
    delete iRequestParentIdentifier;
    iRequestParentIdentifier = requestParentIdentifier;
    CleanupStack::Pop( requestParentIdentifier );     

    delete iParentIdentifier;
    iParentIdentifier = parentIdentifier;
    CleanupStack::Pop( parentIdentifier );     

    delete iServerUri;
    iServerUri = serverUri;
    CleanupStack::Pop( serverUri );

    delete iRemoteUri;
    iRemoteUri = remoteUri;
    CleanupStack::Pop( remoteUri );

    delete iCatalogsSourceName;
    iCatalogsSourceName = catalogsSourceName;
    CleanupStack::Pop( catalogsSourceName );
        
    delete iTimeStamp;
    iTimeStamp = timeStamp;
    CleanupStack::Pop( timeStamp );

    iLinkDownloadTime = linkDownloadTime;

    // We can be sure that enumeration is same as TInt
    iDescription = 
        static_cast<MNcdPreminetProtocolEntityRef::TDescription>(description);
    iValidUntilDelta = validUntilDelta;
    iValidUntilAutoUpdate = validUntilAutoUpdate;
    iRemoteFlag = remoteFlag;

    // Handle the query data
    
    TInt queryCount = aStream.ReadInt32L();
    // Reset old array if it contains data
    iQueries.Reset();
    for ( TInt i = 0; i < queryCount; ++i )
        {
        // Because all the other info has been already set,
        // here we just skip insertion if it failed for some reason.
        // Insert the queries that you can.
        TRAP_IGNORE( iQueries.Append( aStream.ReadInt32L() ) );
        }
        
    DLTRACEOUT((""));
    }



void CNcdNodeLink::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                   TInt aFunctionNumber )
    {
    DLTRACEIN((""));

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
        
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;

        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        aMessage->CompleteAndRelease( trapError );
        }
        
    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }


void CNcdNodeLink::CounterPartLost( const MCatalogsSession& aSession )
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
                  

void CNcdNodeLink::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
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

    if ( buf->Size() > 0 ) 
        {
        DLTRACE(( "Completing the message, buf len: %d", buf->Ptr(0).Length() ));
        }
    // If this leaves, ReceiveMessage function will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }

void CNcdNodeLink::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    // Link existed. So, insert info that link was found.
    aStream.WriteInt32L( ClassId() );

    // Notice that the proxy does not require all the information contained
    // in this class. So, send only the information that proxy wants.

    ExternalizeDesL( Timestamp(), aStream );
    ExternalizeDesL( CatalogsSourceName(), aStream );
    aStream.WriteInt32L( RemoteFlag() );
 
    aStream << ExpiredTime().Int64();    
        
    ParentIdentifierForRequestL( aStream );
    
    // Externalize the real parent too.
    ParentIdentifier().ExternalizeL( aStream );
        
    DLTRACEOUT((""));
    }


void CNcdNodeLink::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }    


CNcdNode& CNcdNodeLink::Node() const
    {
    return iNode;
    }


void CNcdNodeLink::ParentIdentifierForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    // There is always at least an empty identifier value.
    RequestParentIdentifier().ExternalizeL( aStream );

    DLTRACEOUT((""));
    }
