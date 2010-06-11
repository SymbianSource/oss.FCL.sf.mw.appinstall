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
* Description:   Contains CNcdNodeSkinProxy class implementation
*
*/


#include "ncdnodeskinproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdoperationimpl.h"
#include "ncdfiledownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeSkinProxy::CNcdNodeSkinProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    : CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
      iMetadata( aMetadata ),
      iTimeStamp( 0 )
    {
    }


void CNcdNodeSkinProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeSkin* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeSkin::KInterfaceUid ) );

    // Make sure that the skin id contains at least some data.
    iSkinId = KNullDesC().AllocL();

    // Constructor already initialized timestamp to zero.
    
    InternalizeL();
    }
    
    
CNcdNodeSkinProxy* CNcdNodeSkinProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeSkinProxy* self = 
        CNcdNodeSkinProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeSkinProxy* CNcdNodeSkinProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeSkinProxy* self = 
        new( ELeave ) CNcdNodeSkinProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeSkinProxy::~CNcdNodeSkinProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeSkin::KInterfaceUid );

    // Delete member variables here
    delete iSkinId;
    iSkinId = NULL;

    // Do not delete metadata because this object does not own it.
    }


void CNcdNodeSkinProxy::InternalizeL()
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
        DLINFO(("Skin was obsolete"));
        SetObsolete( ETrue );
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeSkin::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeSkin* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeSkin::KInterfaceUid ) );
        }
    else
        {
        // Just in case it was some times obsolete but not anymore
        SetObsolete( EFalse );
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


// MNcdNodeSkin functions

const TDesC& CNcdNodeSkinProxy::SkinId() const
    {
    DLTRACEIN((""));
    // SkinId is always set to contain some data. At least it is KNullDesC.
    DLINFO(( _L("SkinID: %S"), iSkinId ));
    return *iSkinId;
    }


TTime CNcdNodeSkinProxy::SkinTimeStamp() const
    {
    return iTimeStamp;
    }


MNcdFileDownloadOperation* CNcdNodeSkinProxy::LoadSkinL( 
        const TDesC& aTargetFileName,
        MNcdFileDownloadOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    CNcdFileDownloadOperationProxy* operation(   
        MetadataL().Node().OperationManager().
        CreateFileDownloadOperationL( ENcdSkinDownload, 
                                      MetadataL().Node(), 
                                      &aObserver,
                                      aTargetFileName ) );

    // It is enought that the observer is informed
    // when the file operation is ready. So, no need to add
    // node or this class object to be observers.

    // No need to increase the operation ref count here because
    // it should be initially one.

    DLTRACEOUT((""));    
        
    return operation;
    }
    

// Other functions

CNcdNodeMetadataProxy& CNcdNodeSkinProxy::MetadataL() const
    {
    DLTRACEIN((""));

    if( IsObsolete() )
        {
        DLERROR(("Skin is obsolete. Metadata may not exist."));
        User::Leave( KNcdErrorObsolete );
        }
     
    DLTRACEOUT((""));
    
    return iMetadata;
    }


TBool CNcdNodeSkinProxy::IsObsolete() const
    {
    return iObsolete;
    }

void CNcdNodeSkinProxy::SetObsolete( TBool aObsolete )
    {
    iObsolete = aObsolete;
    }    
        

void CNcdNodeSkinProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Use catalogsutils.h functions to internalize
    // memebervariables according to the data received
    // from the server.
    // Make sure that the variables are set here in the same
    // order as they are externalized in the server side.
    // Small mistake here messes up everything!

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeSkinClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    // Get the skin id from the stream
    InternalizeDesL( iSkinId, aStream );
    DLINFO(( _L("SkinID: %S"), iSkinId ));
    // Get the timestamp and change the descriptor info to be TTime type
    HBufC* timeStamp( NULL );
    InternalizeDesL( timeStamp, aStream );
    if ( timeStamp != NULL && timeStamp->Length() > 0 )
        {
        CleanupStack::PushL( timeStamp );
        iTimeStamp.Parse( *timeStamp );
        CleanupStack::PopAndDestroy( timeStamp );        
        }
    else
        {
        // Because the timestamp was not given, set the time to zero value.
        
        // In case time stamp descriptor was allocated but length was zero, delete.
        delete timeStamp;
        iTimeStamp = 0;
        }
        
    DLTRACEOUT((""));
    }
