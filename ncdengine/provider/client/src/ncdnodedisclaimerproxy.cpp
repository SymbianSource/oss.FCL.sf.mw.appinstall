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
* Description:   Contains CNcdNodeDisclaimerProxy class implementation
*
*/


#include "ncdnodedisclaimerproxy.h"
#include "ncdnodemetadataproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


CNcdNodeDisclaimerProxy::CNcdNodeDisclaimerProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy* aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, aMetadata ),
  iResponse( ENoResponse ) 
    {

    }


void CNcdNodeDisclaimerProxy::ConstructL()
    {
    // If the internalization fails, let this leave.    
    InternalizeL();
    }


CNcdNodeDisclaimerProxy* CNcdNodeDisclaimerProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy* aMetadata )
    {
    CNcdNodeDisclaimerProxy* self = 
        CNcdNodeDisclaimerProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeDisclaimerProxy* CNcdNodeDisclaimerProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy* aMetadata )
    {
    CNcdNodeDisclaimerProxy* self = 
        new( ELeave ) CNcdNodeDisclaimerProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeDisclaimerProxy::~CNcdNodeDisclaimerProxy()
    {
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    delete iTitle;
    iTitle = 0;
    delete iBody;
    iBody = 0;
    }


void CNcdNodeDisclaimerProxy::InternalizeL()
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
        DLINFO(("Disclaimer was obsolete"));
        // Because this object does not add any interfaces to the top parent,
        // there is no need to remove then either here.
        // Just remove object from the parent
        RemoveFromParent();        
        }

    // If error occurred during data transfer, leave here and forward the error.
    User::LeaveIfError( error );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
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


// MNcdQuery functions

TBool CNcdNodeDisclaimerProxy::IsOptional() const
    {
    return iOptional;
    }


MNcdQuery::TSemantics CNcdNodeDisclaimerProxy::Semantics() const
    {
    return iSemantics;
    }
    
const TDesC& CNcdNodeDisclaimerProxy::MessageTitle() const
    {
    if( !iTitle )
        {
        return KNullDesC;
        }
    return *iTitle;
    }
    
    
const TDesC& CNcdNodeDisclaimerProxy::MessageBody() const
    {
    if( !iBody )
        {
        return KNullDesC;
        }
    return *iBody;
    }
    
    
RCatalogsArray< MNcdQueryItem > CNcdNodeDisclaimerProxy::QueryItemsL()
    {
    RCatalogsArray<MNcdQueryItem> array;
    return array;
    }


void CNcdNodeDisclaimerProxy::SetResponseL( TResponse aResponse )
    {
    iResponse = aResponse;
    }


MNcdQuery::TResponse CNcdNodeDisclaimerProxy::Response()
    {
    return iResponse;
    }
    
    
TBool CNcdNodeDisclaimerProxy::IsSecureConnection() const
    {
    return EFalse;
    }
    

// Other functions

void CNcdNodeDisclaimerProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Use catalogsutils.h functions to internalize
    // memebervariables according to the data received
    // from the server.
    // Make sure that the variables are set here in the same
    // order as they are externalized in the server side.
    // Small mistake here messes up everything!

    // First read the class id. Because, it is the first thing in the stream.
    // Actually we do not need that information here.
    aStream.ReadInt32L();

    InternalizeDesL( iTitle, aStream );
    InternalizeDesL( iBody, aStream );
    iOptional = aStream.ReadInt32L();
    iSemantics = 
        static_cast<MNcdQuery::TSemantics>(aStream.ReadInt32L());
        
    DLTRACEOUT((""));
    }
