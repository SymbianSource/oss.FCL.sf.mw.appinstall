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
* Description:   Contains CNcdNodeIconProxy class implementation
*
*/


#include "ncdnodeiconproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
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

CNcdNodeIconProxy::CNcdNodeIconProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iMetadata( aMetadata )
    {
    }


void CNcdNodeIconProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeIcon* icon( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( icon, this, MNcdNodeIcon::KInterfaceUid ) );
    // Let this leave if internalization fails.
    InternalizeL();
    }


CNcdNodeIconProxy* CNcdNodeIconProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeIconProxy* self = 
        CNcdNodeIconProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeIconProxy* CNcdNodeIconProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeIconProxy* self = 
        new( ELeave ) CNcdNodeIconProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeIconProxy::~CNcdNodeIconProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeIcon::KInterfaceUid );

    delete iIconId;
    iIconId = NULL;
    }


CNcdNodeMetadataProxy& CNcdNodeIconProxy::MetadataL() const
    {
    DLTRACEIN((""));

    if( IsObsolete() )
        {
        DLERROR(("Icon is obsolete. Metadata may not exist."));
        User::Leave( KNcdErrorObsolete );
        }        

    DLTRACEOUT((""));
    
    return iMetadata;
    }


void CNcdNodeIconProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    TInt error(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( error == KNcdErrorObsolete )
        {
        DLINFO(("Icon was obsolete"));
        SetObsolete( ETrue );
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeIcon::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeIcon* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeIcon::KInterfaceUid ) );
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
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    // Read the data from the stream and insert it to the member variables.
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );

    InternalizeDataL( stream );

    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );

    DLTRACEOUT((""));    
    }


// MNcdNodeIcon functions
    
const TDesC& CNcdNodeIconProxy::IconId() const
    {
    DLTRACEIN(("iIconId=%X",iIconId));
    if ( iIconId ) 
        {        
        return *iIconId;        
        }
    return KNullDesC();
    }

const TDesC& CNcdNodeIconProxy::IconMimeType() const
    {
    DLTRACEIN((""));
    // NOTE: Currently protocol does not offer mime type!
    return KNullDesC;
    }

MNcdDownloadOperation* CNcdNodeIconProxy::LoadIconL(
    MNcdDownloadOperationObserver* aObserver )
    {
    DLTRACEIN((""));

    CNcdDownloadOperationProxy* download =
        MetadataL().Node().OperationManager().
        CreateDownloadOperationL(
            ENcdIconDownload,
            MetadataL().Node(),
            *iIconId,
            aObserver );
            
    DLTRACEOUT((""));
    
    return download;
    }

HBufC8* CNcdNodeIconProxy::IconDataL()
    {
    HBufC8* iconData( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdIconData,
                               KNullDesC8,
                               iconData,
                               Handle(),
                               0 ) );
    
    if ( iconData && iconData->Length() == 0 )
        {
        // If icon data does not contain any data, return NULL.
        delete iconData;
        return NULL;
        }
    DLTRACE(( _L("Icon data length: %d for id: %S"), iconData->Length(),
        &IconId() ));
    return iconData;
    }


// Other functions

TBool CNcdNodeIconProxy::IsObsolete() const
    {
    return iObsolete;
    }

void CNcdNodeIconProxy::SetObsolete( TBool aObsolete )
    {
    iObsolete = aObsolete;
    }


void CNcdNodeIconProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeIconClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }


    // Get the icon id from the stream.
    InternalizeDesL( iIconId, aStream );
    DLINFO(( _L("iIconId=%S"),iIconId));

    DLTRACEOUT((""));
    }
