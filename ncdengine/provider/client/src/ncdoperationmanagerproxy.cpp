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
* Description:   Implements CNcdOperationManagerProxy class
*
*/


#include "ncdoperationmanagerproxy.h"

#include <e32err.h>

#include "ncdbaseoperationproxy.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncddownloadoperationproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodeidentifier.h"
#include "ncdloadnodeoperation.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "ncdfiledownloadoperationproxy.h"
#include "ncdpurchaseoperationproxy.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncdpurchaseoptionproxy.h"
#include "ncdinstalloperationproxy.h"
#include "ncdsilentinstalloperationproxy.h"
#include "ncdrightsobjectoperationproxy.h"
#include "ncdsubscriptionoperationproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncddeviceinteractionfactory.h"
#include "ncdinstallationservice.h"
#include "ncdnodemetadataproxy.h"
#include "ncdcreateaccesspointoperationproxy.h"
#include "ncdsendhttprequestoperationproxy.h"
#include "ncdconnectionmethod.h"
#include "ncdserverreportoperationproxy.h"
#include "ncdproviderdefines.h"

#include "catalogsdebug.h"

CNcdOperationManagerProxy* CNcdOperationManagerProxy::NewL( MCatalogsClientServer& aSession, 
                                                            TInt aHandle )
    {
    CNcdOperationManagerProxy* self = CNcdOperationManagerProxy::NewLC( aSession, aHandle );
    CleanupStack::Pop( self );
    return self;
    }

CNcdOperationManagerProxy* CNcdOperationManagerProxy::NewLC( MCatalogsClientServer& aSession, 
                                                             TInt aHandle )
    {
    CNcdOperationManagerProxy* self = 
        new( ELeave ) CNcdOperationManagerProxy( aSession, aHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }


CNcdOperationManagerProxy::~CNcdOperationManagerProxy()
    {
    DLTRACEIN((""));
    iRemovingAllOperations = ETrue;

    DLTRACE(("All operations should have been released by now"));
    DASSERT( iOperationCache.Count() == 0 );
    iOperationCache.Close();
    delete iInstallationService;
    DLTRACEOUT((""));
    }
    
void CNcdOperationManagerProxy::SetClientLocalizer(
    MNcdClientLocalizer& aLocalizer ) 
    {
    iClientLocalizer = &aLocalizer;
    }

    
CNcdLoadNodeOperationProxy* CNcdOperationManagerProxy::CreateLoadNodeOperationL( 
    CNcdNodeProxy& aNode, TBool aLoadChildren, TInt aPageSize, TInt aPageStart, TInt aDepth,
    TNcdChildLoadMode aMode, MNcdSearchFilter* aSearchFilter )
    {
    DLTRACEIN((""));
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    aNode.NodeIdentifier().ExternalizeL( stream );
    stream.WriteInt32L( aLoadChildren );
    stream.WriteInt32L( aPageSize );
    stream.WriteInt32L( aPageStart );
    stream.WriteInt32L( aDepth );
    stream.WriteInt32L( aMode );
    stream.WriteInt32L( aSearchFilter != NULL );
    if ( aSearchFilter )
        {        
        stream.WriteUint32L( aSearchFilter->ContentPurposes() );
        stream.WriteInt32L( aSearchFilter->Keywords().MdcaCount() );
        for ( TInt i = 0 ; i < aSearchFilter->Keywords().MdcaCount() ; i++ )
            {
            ExternalizeDesL( aSearchFilter->Keywords().MdcaPoint( i ), stream );
            }
        ExternalizeEnumL( aSearchFilter->SearchMode(), stream );
        stream.WriteUint32L( aSearchFilter->RecursionDepth() );        
        }    
    
    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    DLTRACE(("Create server side operation"));
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdOperationManagerCreateLoadNodeOperation,
                          ptr,
                          operationHandle,
                          Handle() ) );
                          
    CleanupStack::PopAndDestroy( buf );
    
   
    MNcdOperationProxyRemoveHandler* removeHandler( this );

    DLINFO(("operation handle: %d", operationHandle ));
        
    // Create operation proxy
    CNcdLoadNodeOperationProxy* operation = 
        CNcdLoadNodeOperationProxy::NewLC(
            ClientServerSession(), operationHandle, removeHandler, &aNode,
            iNodeManager, aSearchFilter != NULL, iClientLocalizer );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;
    }

CNcdLoadNodeOperationProxy* CNcdOperationManagerProxy::CreateLoadRootNodeOperationL(
    CNcdNodeProxy& aNode )
    {
    DLTRACEIN((""));
    
    MNcdOperationProxyRemoveHandler* removeHandler( this );
    
    // Create operation on the server side
    TInt operationHandle = CreateOperationL( aNode, 
        NcdNodeFunctionIds::ENcdOperationManagerCreateLoadRootNodeOperation,
        removeHandler );
        
    // Create operation proxy
    CNcdLoadNodeOperationProxy* operation = 
        CNcdLoadNodeOperationProxy::NewLC(
            ClientServerSession(), operationHandle, removeHandler, &aNode,
                iNodeManager, EFalse, iClientLocalizer );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;
    }
    
CNcdLoadNodeOperationProxy* CNcdOperationManagerProxy::CreateLoadBundleNodeOperationL(
    CNcdNodeProxy& aNode )
    {
    DLTRACEIN((""));
    
    MNcdOperationProxyRemoveHandler* removeHandler( this );
    
    // Create operation on the server side
    TInt operationHandle = CreateOperationL(
        aNode, NcdNodeFunctionIds::ENcdOperationManagerCreateLoadBundleNodeOperation,
        removeHandler );
    
    // Create operation proxy
    CNcdLoadNodeOperationProxy* operation =
        CNcdLoadNodeOperationProxy::NewLC(
            ClientServerSession(), operationHandle, removeHandler, &aNode,
            iNodeManager, EFalse, iClientLocalizer );
            
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );
    return operation;
    }


// ---------------------------------------------------------------------------
// Creates a new download operation
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy* CNcdOperationManagerProxy::CreateDownloadOperationL( 
    TNcdDownloadDataType aType, CNcdNodeProxy& aNode, const TDesC& /* aDataId */,
    MNcdDownloadOperationObserver* aObserver,
    TInt aDownloadIndex )
    {           
    DLTRACEIN((""));     
    MNcdOperationProxyRemoveHandler* removeHandler( this );

    // Use temporary node to create a download operation
    // http://jira.bothi.fi/jira/browse/PRECLI-1229
    
    // Ownership is not transferred
    CNcdNodeMetadataProxy* metadataProxy = aNode.Metadata();
    
    if ( !metadataProxy )
        {
        DLERROR(( "CNcdNodeMetadataProxy was NULL!" ));
        User::Leave( KErrNotFound );
        }

    CNcdNodeIdentifier& nodeIdentifier = metadataProxy->Identifier();

    CNcdNodeProxy* node = NULL;
    if( aType == ENcdContentDownload )
        {
        // Content download may be started from just purchase history data
        // even when chache is completely empty.
        // Always use temporary nodes in content downloads to prevent problems.
        node = &iNodeManager->CreateTemporaryOrSupplierNodeL(
                nodeIdentifier );
        }
    else
        {
        node = &aNode;
        }

    // Create the server side download
    TInt operationHandle = CreateServerSideDownloadOperationL( aType, 
        node->NodeIdentifier(), aDownloadIndex );
  
    // Check if the download already exists, increases refcount if operation
    // is found    
    CNcdDownloadOperationProxy* operation = FindExistingDownloadOperation(
        operationHandle );
    if ( operation ) 
        {
        DLTRACE(("Found existing download"));
        if ( aObserver ) 
            {                        
            operation->AddObserverL( *aObserver );
            }        
        return operation;
        }
    
        
    // Create operation proxy
    operation = CNcdDownloadOperationProxy::NewLC(
        ClientServerSession(), aType, operationHandle, removeHandler, node,
        aObserver, iNodeManager, aDownloadIndex );
    
    iOperationCache.AppendL( operation );    
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    }
    

// ---------------------------------------------------------------------------
// Creates a new file download operation
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy* CNcdOperationManagerProxy::CreateFileDownloadOperationL( 
    TNcdDownloadDataType aType, 
    CNcdNodeProxy& aNode, 
    MNcdFileDownloadOperationObserver* aObserver,
    const TDesC& aTargetPath )
    {
    DLTRACEIN((""));
    if ( !aTargetPath.Length() )
        {
        DLERROR(( "No target path, leaving with KErrArgument" ));
        User::Leave( KErrArgument );
        }
    
    MNcdOperationProxyRemoveHandler* removeHandler( this );    

    // Create the server side download
    TInt operationHandle = CreateServerSideDownloadOperationL( aType, 
        aNode.NodeIdentifier(), 0 );
        
    // Create operation proxy
    CNcdFileDownloadOperationProxy* operation = 
        CNcdFileDownloadOperationProxy::NewLC(
            ClientServerSession(), operationHandle, removeHandler, &aNode, 
            iNodeManager, aTargetPath, aObserver, aType );
    
    iOperationCache.AppendL( operation );    
    
    CleanupStack::Pop( operation );
    
    return operation;    
    }


// ---------------------------------------------------------------------------
// Creates a new file download operation
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy* 
    CNcdOperationManagerProxy::CreateFileDownloadOperationL( 
    TNcdDownloadDataType aType,
    const TDesC& aUri, 
    const TDesC& aTargetPath, 
    MNcdFileDownloadOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    
    if ( !aUri.Length() || !aTargetPath.Length() )
        {
        DLERROR(( "No URI or target path, leaving with KErrArgument" ));
        User::Leave( KErrArgument );
        }
        
    MNcdOperationProxyRemoveHandler* removeHandler( this );    

    // Use node id for transferring URI and targetpath to server-side
    CNcdNodeIdentifier* id = CNcdNodeIdentifier::NewLC(
        aUri, aTargetPath, TUid::Null() );

    // Create the server side download
    TInt operationHandle = CreateServerSideDownloadOperationL( 
        ENcdGenericFileDownload, 
        *id,
        0 );
    
    CleanupStack::PopAndDestroy( id );
    
    // Create operation proxy
    CNcdFileDownloadOperationProxy* operation = 
        CNcdFileDownloadOperationProxy::NewLC(
            ClientServerSession(), operationHandle, removeHandler, NULL, 
            iNodeManager, aTargetPath, aObserver, aType );
    
    iOperationCache.AppendL( operation );    
    
    CleanupStack::Pop( operation );    
    DLTRACEOUT((""));
    return operation;
    }


// ---------------------------------------------------------------------------
// Creates a new purchase operation
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationProxy* CNcdOperationManagerProxy::CreatePurchaseOperationL( 
    CNcdNodeProxy& aNode, 
    CNcdPurchaseOptionProxy& aSelectedPurchaseOption,
    MNcdPurchaseOperationObserver* aObserver )
    {
    DLTRACEIN((""));

    MNcdOperationProxyRemoveHandler* removeHandler( this );    

    // Create operation on the server side
    TInt operationHandle = CreateServerSidePurchaseOperationL( 
        aNode, 
        aSelectedPurchaseOption, 
        NcdNodeFunctionIds::ENcdOperationManagerCreatePurchaseOperation,
        removeHandler );
        
    CNcdSubscriptionManagerProxy* subscriptionManager( NULL );
    if ( iNodeManager != NULL )
        {
        subscriptionManager = &iNodeManager->SubscriptionManager();
        }
        
    // Create operation proxy
    CNcdPurchaseOperationProxy* operation = 
        CNcdPurchaseOperationProxy::NewLC( 
            ClientServerSession(), 
            operationHandle, 
            removeHandler, 
            &aNode, 
            &aSelectedPurchaseOption,
            aObserver,
            subscriptionManager,
            iNodeManager,
            iClientLocalizer );
    
    iOperationCache.AppendL( operation );    
    
    CleanupStack::Pop( operation );
    
    return operation;    
    }
         
         

// ---------------------------------------------------------------------------
// Creates a new install operation
// ---------------------------------------------------------------------------
//         
CNcdInstallOperationProxy* CNcdOperationManagerProxy::CreateInstallOperationL(
    CNcdNodeProxy& aNode,
    MNcdInstallOperationObserver* aObserver )
    {
   DLTRACEIN((""));
    // Should get the actual object for this pointer!
    MNcdOperationProxyRemoveHandler* removeHandler( this );
    
    // Create operation on the server side
    TInt operationHandle = CreateOperationL( aNode, 
        NcdNodeFunctionIds::ENcdOperationManagerCreateInstallOperation,
        removeHandler );
    
    // Create operation proxy
    CNcdInstallOperationProxy* operation = 
        CNcdInstallOperationProxy::NewLC(
            ClientServerSession(), operationHandle, 
            removeHandler, &aNode, iNodeManager, aObserver,
            InstallationServiceL() );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    }


// ---------------------------------------------------------------------------
// Creates a new install operation
// ---------------------------------------------------------------------------
//         
CNcdSilentInstallOperationProxy* CNcdOperationManagerProxy::CreateSilentInstallOperationL(
    CNcdNodeProxy& aNode,
    MNcdInstallOperationObserver* aObserver,
    Usif::COpaqueNamedParams* aInstallOptions )
    {
   DLTRACEIN((""));

    // Should get the actual object for this pointer!
    MNcdOperationProxyRemoveHandler* removeHandler( this );
    
    // Create operation on the server side
    // Notice, that the installation is handled in the proxy side, and only some
    // basic functionality is in the server side.
    // Notice, that this leaves with KErrPermissionDenied if UI does not have enough
    // capabilites for silent install.
    TInt operationHandle = CreateOperationL( aNode, 
        NcdNodeFunctionIds::ENcdOperationManagerCreateSilentInstallOperation,
        removeHandler );
    
    // Create operation proxy
    CNcdSilentInstallOperationProxy* operation = 
        CNcdSilentInstallOperationProxy::NewLC(
            ClientServerSession(), operationHandle, 
            removeHandler, &aNode, iNodeManager, aObserver,
            InstallationServiceL(),
            aInstallOptions );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    }


// ---------------------------------------------------------------------------
// Creates a DRM rights object download & install operation
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperationProxy* 
    CNcdOperationManagerProxy::CreateRightsObjectOperationL(
    const TDesC& aDownloadUri, 
    const TDesC& aMimeType, 
    const TNcdConnectionMethod& aConnectionMethod,
    MNcdRightsObjectOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    // Calculate size of input data buffer.
    TInt bufferLen = sizeof( TUint32 ) + aDownloadUri.Size() +  // URI length + text data
                     sizeof( TUint32 ) + aMimeType.Size() +     // mime type length + text data
                     sizeof( TNcdConnectionMethod );            // access point id

    // Allocate and build the input data buffer.
    CBufFlat* buffer = CBufFlat::NewL( bufferLen );
    CleanupStack::PushL( buffer );
    
    RBufWriteStream stream( *buffer );
    CleanupClosePushL( stream );

    ExternalizeDesL( aDownloadUri, stream );    // Don't use << (would convert to 8-bit)
    ExternalizeDesL( aMimeType, stream );
    aConnectionMethod.ExternalizeL( stream );

    // Send the command for creating the server side operation object.
    TInt operationHandle;  // communication handle to the created server side object
    User::LeaveIfError( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdOperationManagerCreateRightsObjectOperation,
        buffer->Ptr( 0 ),
        operationHandle,
        Handle() ) );
    DLINFO(( "Got server side operation object comm handle %d", operationHandle ));

    CleanupStack::PopAndDestroy( 2, buffer );   // stream-close, buffer

    // Create operation proxy

    CNcdRightsObjectOperationProxy* operation = 
        CNcdRightsObjectOperationProxy::NewLC(
            ClientServerSession(), iNodeManager, operationHandle, 
            *this, aObserver );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    }

CNcdSubscriptionOperationProxy*
    CNcdOperationManagerProxy::CreateSubscriptionRefreshOperationL(
    MNcdSubscriptionOperationObserver& aObserver )
    {
    DLTRACEIN((""));
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Subscription operation doesn't need any parameters to refresh all
    // subscriptions.
    stream.WriteInt32L( MNcdSubscriptionOperation::ERefreshSubscriptions );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    DLTRACE(("Create server side operation"));
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError(
        ClientServerSession().
            SendSync(
                NcdNodeFunctionIds::ENcdOperationManagerCreateSubscriptionOperation,
                ptr,
                operationHandle,
                Handle() ) );

    CleanupStack::PopAndDestroy( buf );
   
    MNcdOperationProxyRemoveHandler* removeHandler( this );

    DLINFO(("operation handle: %d", operationHandle ));

    CNcdSubscriptionManagerProxy* subscriptionManager( NULL );
    if ( iNodeManager != NULL )
        {
        subscriptionManager = &iNodeManager->SubscriptionManager();
        }

    CNcdSubscriptionOperationProxy* operation =
        CNcdSubscriptionOperationProxy::NewLC(
            ClientServerSession(),
            MNcdSubscriptionOperation::ERefreshSubscriptions,
            operationHandle,
            removeHandler,
            aObserver,
            subscriptionManager,
            iNodeManager,
            iClientLocalizer );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;
    }



CNcdSubscriptionOperationProxy*
    CNcdOperationManagerProxy::CreateSubscriptionUnsubscribeOperationL(
        const TDesC& aPurchaseOptionId,
        const TDesC& aEntityId,
        const TDesC& aNamespace,
        const TDesC& aServerUri,
        MNcdSubscriptionOperationObserver& aObserver )
    {
    DLTRACEIN((""));
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Write the type of subscription operation.
    stream.WriteInt32L( MNcdSubscriptionOperation::EUnsubscribe );

    ExternalizeDesL( aPurchaseOptionId, stream );
    ExternalizeDesL( aEntityId, stream );
    ExternalizeDesL( aNamespace, stream );
    ExternalizeDesL( aServerUri, stream );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    DLTRACE(("Create server side operation"));
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError(
        ClientServerSession().
            SendSync(
                NcdNodeFunctionIds::ENcdOperationManagerCreateSubscriptionOperation,
                ptr,
                operationHandle,
                Handle() ) );

    CleanupStack::PopAndDestroy( buf );
   
    MNcdOperationProxyRemoveHandler* removeHandler( this );

    DLINFO(("operation handle: %d", operationHandle ));

    CNcdSubscriptionManagerProxy* subscriptionManager( NULL );
    if ( iNodeManager != NULL )
        {
        subscriptionManager = &iNodeManager->SubscriptionManager();
        }

    CNcdSubscriptionOperationProxy* operation =
        CNcdSubscriptionOperationProxy::NewLC(
            ClientServerSession(),
            MNcdSubscriptionOperation::EUnsubscribe,
            operationHandle,
            removeHandler,
            aObserver,
            subscriptionManager,
            iNodeManager,
            iClientLocalizer );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;
    }        


// ---------------------------------------------------------------------------
// Creates server report operation
// ---------------------------------------------------------------------------
//
CNcdServerReportOperationProxy* 
    CNcdOperationManagerProxy::CreateServerReportOperationL(
            MNcdServerReportOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    // Send the command for creating the server side operation object.
    TInt operationHandle( 0 );  // communication handle to the created server side object
    TInt error( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdOperationManagerCreateServerReportOperation,
        KNullDesC,
        operationHandle,
        Handle() ) );
    User::LeaveIfError( error );
    DLINFO(( "Got server side operation object comm handle %d", operationHandle ));

    // Create operation proxy

    CNcdServerReportOperationProxy* operation = 
        CNcdServerReportOperationProxy::NewLC(
            ClientServerSession(), iNodeManager, operationHandle, 
            *this, aObserver );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;        
    }


// ---------------------------------------------------------------------------
// Creates an accesspoint creation operation
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperationProxy* 
    CNcdOperationManagerProxy::CreateCreateAccessPointOperationL(
        const TDesC& aAccessPointData, 
        MNcdCreateAccessPointOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    if ( !aAccessPointData.Size() ) 
        {
        DLERROR(("Accesspoint data is empty"));
        User::Leave( KErrArgument );
        }
    
    RCatalogsBufferWriter buffer;
    buffer.OpenLC();
    
    ExternalizeDesL( aAccessPointData, buffer() );     
        
    // Send the command for creating the server side operation object.
    TInt operationHandle;  // communication handle to the created server side object
    User::LeaveIfError( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdOperationManagerCreateCreateAccessPointOperation,
        buffer.PtrL(),
        operationHandle,
        Handle() ) );
    DLINFO(( "Got server side operation object comm handle %d", operationHandle ));

    CleanupStack::PopAndDestroy( &buffer );   

    // Create operation proxy

    CNcdCreateAccessPointOperationProxy* operation = 
        CNcdCreateAccessPointOperationProxy::NewLC(
            ClientServerSession(), 
            iNodeManager, 
            operationHandle, 
            *this, 
            aObserver );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    
    }



// ---------------------------------------------------------------------------
// Creates a HTTP request sending operation
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperationProxy* 
    CNcdOperationManagerProxy::CreateSendHttpRequestOperationL(
       const TDesC8& aUri,
       const TDesC8& aRequest, 
       const TNcdConnectionMethod& aConnectionMethod,
       MNcdSendHttpRequestOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    if ( !aRequest.Size() ) 
        {
        DLERROR(("Request is empty"));
        User::Leave( KErrArgument );
        }
    
    RCatalogsBufferWriter buffer;
    buffer.OpenLC();
    
    aConnectionMethod.ExternalizeL( buffer() );
    ExternalizeDesL( aUri, buffer() );
    ExternalizeDesL( aRequest, buffer() );         
        
    // Send the command for creating the server side operation object.
    TInt operationHandle;  // communication handle to the created server side object
    User::LeaveIfError( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdOperationManagerCreateSendHttpRequestOperation,
        buffer.PtrL(),
        operationHandle,
        Handle() ) );
    DLINFO(( "Got server side operation object comm handle %d", operationHandle ));

    CleanupStack::PopAndDestroy( &buffer );   

    // Create operation proxy

    CNcdSendHttpRequestOperationProxy* operation = 
        CNcdSendHttpRequestOperationProxy::NewLC(
            ClientServerSession(),             
            operationHandle, 
            *this, 
            *iNodeManager, 
            aObserver );
    
    iOperationCache.AppendL( operation );
    
    CleanupStack::Pop( operation );
    DLTRACEOUT((""));
    return operation;    
    
    }


// ---------------------------------------------------------------------------
// Restore content downloads
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::RestoreContentDownloadsL()
    {
    DLTRACEIN((""));
        
    // Get operation handles from the server
    HBufC8* data = NULL;
    User::LeaveIfError(
        ClientServerSession().
            SendSyncAlloc(
                NcdNodeFunctionIds::ENcdOperationManagerRestoreContentDownloads,
                KNullDesC8(),
                data,
                Handle(),
                0 ) );

    MNcdOperationProxyRemoveHandler* removeHandler( this );
    CleanupStack::PushL( data );
   
    RDesReadStream reader( *data );    
    CleanupClosePushL( reader );
    
    NcdProviderDefines::TNcdStreamDataState streamState = 
        NcdProviderDefines::ENcdStreamDataEnd;
    do
        {       
        InternalizeEnumL( streamState, reader );
        
        if ( streamState == NcdProviderDefines::ENcdStreamDataObject ) 
            {
            TInt32 operationHandle = reader.ReadInt32L();
            DLINFO(("Operation handle: %d", operationHandle ));
            
            TNcdDownloadDataType type = static_cast<TNcdDownloadDataType>(
                reader.ReadInt32L() );
            
            CNcdNodeIdentifier* metadataId = CNcdNodeIdentifier::NewLC(
                reader );
            
            TInt dlIndex = reader.ReadInt32L();
    
            // Use a temporary node        
            CNcdNodeProxy* node = &iNodeManager->CreateTemporaryOrSupplierNodeL(
                    *metadataId );
                       
            // Create operation proxy
            CNcdDownloadOperationProxy* operation = 
                CNcdDownloadOperationProxy::NewLC(
                    ClientServerSession(), type, 
                    operationHandle, removeHandler, node,
                    NULL, iNodeManager, dlIndex, ETrue );
            
            // Add operation manager as an observer so that the operation
            // is deleted correctly if it completes
            operation->AddObserverL( *this );
            iOperationCache.AppendL( operation );                
            
            CleanupStack::Pop( operation );
            CleanupStack::PopAndDestroy( metadataId );
            }
        }
    while( streamState != NcdProviderDefines::ENcdStreamDataEnd );
    
    CleanupStack::PopAndDestroy( 2, data ); // reader, data          
    DLTRACEOUT(("Downloads created"));
    }


// ---------------------------------------------------------------------------
// Operations array getter
// ---------------------------------------------------------------------------
//
const RPointerArray<MNcdOperation>& CNcdOperationManagerProxy::Operations() const
    {
    return iOperationCache;
    }


// ---------------------------------------------------------------------------
// Node manager setter
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::SetNodeManager(CNcdNodeManagerProxy* aNodeManager )
    {
    iNodeManager = aNodeManager;
    }

// ---------------------------------------------------------------------------
// Node manager getter
// ---------------------------------------------------------------------------
//
CNcdNodeManagerProxy* CNcdOperationManagerProxy::NodeManager()
    {
    return iNodeManager;
    }


// ---------------------------------------------------------------------------
// Installation service getter
// ---------------------------------------------------------------------------
//
MNcdInstallationService& CNcdOperationManagerProxy::InstallationServiceL()
    {
    DLTRACEIN((""));
    if ( !iInstallationService ) 
        {
        iInstallationService = 
            NcdDeviceInteractionFactory::CreateInstallationServiceL();
        }
    return *iInstallationService;
    }


// ---------------------------------------------------------------------------
// Cancel all operations
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::CancelAllOperations()
    {
    DLTRACEIN((""));    
    
    for ( TInt i = iOperationCache.Count() - 1; i >= 0; --i )
        {
        // Add reference so that the operation is not deleted in
        // CancelOperation's callbacks
        iOperationCache[i]->AddRef();
        iOperationCache[i]->CancelOperation();
        
        // This releases those operations that have 1 refcount such as
        // restored content downloads that the client hasn't obtained
        // from the manager
        while( iOperationCache[i]->Release() )
            {
            // empty on purpose
            }
        }    
    }


// ---------------------------------------------------------------------------
// Cancel all operations
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::ReleaseUnusedOperations()
    {
    DLTRACEIN((""));    
    
    for ( TInt i = iOperationCache.Count() - 1; i >= 0; --i )
        {
        // This releases those operations that have 1 refcount such as
        // restored content downloads that the client hasn't obtained
        // from the manager        
        iOperationCache[i]->Release();
        }    
    }


// ---------------------------------------------------------------------------
// Operation proxy remover
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::RemoveOperationProxy( 
                                    CNcdBaseOperationProxy& aOperationProxy )
    {
    DLTRACEIN((""));
    
    // Releasing all operations so we don't want to mess with the operation cache
    if ( iRemovingAllOperations ) 
        {
        DLTRACE(("Removing all operations"));
        return;
        }
        
    MNcdOperation* operation = NULL;
    TRAP_IGNORE( 
    operation = aOperationProxy.QueryInterfaceLC< MNcdOperation >();
        CleanupStack::Pop( operation );
        ); // TRAP_IGNORE
    if( ! operation )
        {
        DLINFO( ("!!!QueryInterfaceLC failed for operation! FATAL ERROR!"));
        DASSERT( 0 );
        }    
    
    // Remove operation from the cache because it will be deleted.
    for ( TInt i = 0; i < iOperationCache.Count(); ++i )
        {
        if ( operation == iOperationCache[ i ] )
            {
            DLTRACE(("Removed the proxy"));
            // Remove node from the array and delete it.
            iOperationCache.Remove( i );
            break;
            }
        } 
    
    // RemoveOperationProxy MUST only be called from proxy's destructor
    // and therefore the operation MUST NOT be released again    
    DLTRACEOUT((""));
    }    
    
    
// ---------------------------------------------------------------------------
// Download operation observer interface implementation
// ---------------------------------------------------------------------------
//
void CNcdOperationManagerProxy::DownloadProgress( 
    MNcdDownloadOperation& /* aOperation */,
    TNcdProgress /* aProgress */ )
    {
    // Do nothing
    }
    
    
void CNcdOperationManagerProxy::QueryReceived( 
    MNcdDownloadOperation& /* aOperation */,
    MNcdQuery* /* aQuery */)
    {
    // Do nothing
    }


void CNcdOperationManagerProxy::OperationComplete( 
    MNcdDownloadOperation& aOperation,
    TInt aError )
    {
    DLTRACEIN(("aOperation: %x, error: %d", &aOperation, aError ));
    (void)aError; // suppresses compiler warning
    // Just release the operation because operation manager had a 
    // reference to it
    aOperation.Release();
    }

    
    
CNcdOperationManagerProxy::CNcdOperationManagerProxy( MCatalogsClientServer& aSession, 
                                                      TInt aHandle )
: CNcdBaseProxy( aSession, aHandle )
    {
    }

void CNcdOperationManagerProxy::ConstructL()
    {
    // Nothing to be done here    
    }


// ---------------------------------------------------------------------------
// Creates the identification stream for the given node
// ---------------------------------------------------------------------------
//
HBufC8* CNcdOperationManagerProxy::CreateNodeIdentifierDataLC( CNcdNodeProxy& aNode )
    {
    DLTRACEIN((""));
    HBufC8* nodeIdentifierData( aNode.NodeIdentifier().NodeIdentifierDataL() );
    if( nodeIdentifierData == NULL )
        {
        DLTRACEIN(("Null identifier data"));
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( nodeIdentifierData );    
    DLTRACEOUT((""));
    return nodeIdentifierData;
    }



// ---------------------------------------------------------------------------
// Creates an operation on the server side
// ---------------------------------------------------------------------------
//
TInt CNcdOperationManagerProxy::CreateOperationL( CNcdNodeProxy& aNode,
    NcdNodeFunctionIds::TNcdOperationManagerFunctionNumber aFunction, 
    MNcdOperationProxyRemoveHandler*& aRemoveHandler )
    {
    DLTRACEIN(( _L("Node: %S::%S"), &aNode.NodeIdentifier().NodeNameSpace(), 
        &aNode.NodeIdentifier().NodeId() ));
    // Create id stream
    HBufC8* nodeIdentifierData = CreateNodeIdentifierDataLC( aNode );
                
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( aFunction,
                          *nodeIdentifierData,
                          operationHandle,
                          Handle() ) );

    DLINFO(("operation handle: %d", operationHandle ));
    CleanupStack::PopAndDestroy( nodeIdentifierData );
        
    aRemoveHandler = this;
    DLTRACEOUT((""));
    return operationHandle;
    }


// ---------------------------------------------------------------------------
// Creates a purchase operation on the server side
// ---------------------------------------------------------------------------
//
TInt CNcdOperationManagerProxy::CreateServerSidePurchaseOperationL( 
    CNcdNodeProxy& aNode,
    const CNcdPurchaseOptionProxy& aSelectedPurchaseOption,
    NcdNodeFunctionIds::TNcdOperationManagerFunctionNumber aFunction, 
    MNcdOperationProxyRemoveHandler*& /*aRemoveHandler*/ )
    {
    DLTRACEIN(( _L("Node: %S::%S"), &aNode.NodeIdentifier().NodeNameSpace(), 
        &aNode.NodeIdentifier().NodeId() ));

    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    aNode.NodeIdentifier().ExternalizeL( stream );

    ExternalizeDesL( aSelectedPurchaseOption.Id(), stream );

    CleanupStack::PopAndDestroy( &stream );

    TPtrC8 ptr = buf->Ptr( 0 );
    
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( aFunction,
                          ptr,
                          operationHandle,
                          Handle() ) );

    CleanupStack::PopAndDestroy( buf );

    DLINFO(("operation handle: %d", operationHandle ));
        
    DLTRACEOUT((""));
    return operationHandle;
    }

// ---------------------------------------------------------------------------
// Creates an download operation on the server side
// ---------------------------------------------------------------------------
//
TInt CNcdOperationManagerProxy::CreateServerSideDownloadOperationL( 
    TNcdDownloadDataType aType, 
    const CNcdNodeIdentifier& aNodeId,
    TInt aDownloadIndex )
    {
    DLTRACEIN((""));
    // Create operation on the server side

    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Write type
    stream.WriteInt32L( aType );
    
    // Write node ID
    aNodeId.ExternalizeL( stream );
    
    // Write download index
    stream.WriteInt32L( aDownloadIndex );

    CleanupStack::PopAndDestroy( &stream );    
                
    // Send message to server side operation manager to create the server side 
    // operation and to get a handle to the operation.
    TInt operationHandle( 0 );
    User::LeaveIfError( ClientServerSession().
        SendSync( 
        NcdNodeFunctionIds::ENcdOperationManagerCreateDownloadOperation,
        buf->Ptr( 0 ), operationHandle, Handle() ) );

    DLINFO(("operation handle: %d", operationHandle ));
    CleanupStack::PopAndDestroy( buf );
    DLTRACEOUT((""));
    return operationHandle;
    }
    
    

// ---------------------------------------------------------------------------
// Searches operations for a matching download operation
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy* 
    CNcdOperationManagerProxy::FindExistingDownloadOperation(
        TInt aHandle ) const
    {
    DLTRACEIN((""));    
    
    for ( TInt i = 0; i < iOperationCache.Count(); ++i )
        {
        if ( iOperationCache[i]->OperationType() == ENcdDownloadOperationUid )
            {
            DLTRACE(("Querying for download interface"));
            MNcdDownloadOperation* dl = 
                iOperationCache[i]->QueryInterfaceLC<MNcdDownloadOperation>();
            DASSERT( dl );
                        
            
            CNcdDownloadOperationProxy* operation = 
                static_cast<CNcdDownloadOperationProxy*>( dl );
            
            DLTRACE(("Checking download handle"));
            if ( operation->Handle() == aHandle ) 
                {
                DLTRACE(("Found a match"));
                CleanupStack::Pop( dl );
                return operation;    
                }
                
            // QueryInterfaceLC increases the ref count
            CleanupStack::PopAndDestroy( dl );
            }

        }
    DLTRACEOUT((""));
    return NULL;            
    }
        

