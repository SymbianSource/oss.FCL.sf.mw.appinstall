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
* Description:   Contains CNcdNodeUserDataProxy class implementation
*
*/


#include "ncdnodeuserdataproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdoperationimpl.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


CNcdNodeUserDataProxy::CNcdNodeUserDataProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aNodeMetadata )
    : CNcdInterfaceBaseProxy( aSession, aHandle, &aNodeMetadata ),
      iNodeMetadata( aNodeMetadata )
    {
    }


void CNcdNodeUserDataProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeUserData* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeUserData::KInterfaceUid ) );
    }


CNcdNodeUserDataProxy* CNcdNodeUserDataProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aNodeMetadata )
    {
    CNcdNodeUserDataProxy* self = 
        CNcdNodeUserDataProxy::NewLC( aSession, aHandle, aNodeMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeUserDataProxy* CNcdNodeUserDataProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aNodeMetadata )
    {
    CNcdNodeUserDataProxy* self = 
        new( ELeave ) CNcdNodeUserDataProxy( aSession, aHandle, aNodeMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeUserDataProxy::~CNcdNodeUserDataProxy()
    {
    // Delete member variables here
    // Do not delete node because this object does not own it.

    // Remove the interface that is not used anymore
    RemoveInterface( MNcdNodeUserData::KInterfaceUid );
    }


// MNcdNodeUserData functions

HBufC8* CNcdNodeUserDataProxy::UserDataL() const
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdUserData,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

    DLTRACEOUT(( "Received user data: %S", data ));    
    
    return data;
    }

void CNcdNodeUserDataProxy::SetUserDataL( const TDesC8* aData )
    {
    DLTRACEIN((""));

    // Ret data does not need to contain anything.
    TPtr8 retData( NULL, 0 );

    if( aData == NULL )
        {
        DLTRACE(("User data NULL" ));
        
        // This means that the user data should be cleared
        // from the db. No need to send any data to the server.
        // Just use the right function.
        User::LeaveIfError(
            ClientServerSession().
            SendSync( NcdNodeFunctionIds::ENcdClearUserData,
                      KNullDesC8,
                      retData,
                      Handle() ) );
        }
    else
        {
        DLTRACE(("Set user data: %S", aData ));
        
        // Send the data to the server side.
        User::LeaveIfError(
            ClientServerSession().
            SendSync( NcdNodeFunctionIds::ENcdSetUserData,
                      *aData,
                      retData,
                      Handle() ) );        
        }


    // The aData should not be deleted here, because
    // the ownership is not transferred. Even though the
    // parameter is the pointer.

    DLTRACEOUT((""));    
    }


// Other functions

CNcdNodeMetadataProxy& CNcdNodeUserDataProxy::NodeMetadata() const
    {
    return iNodeMetadata;
    }
