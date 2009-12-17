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
* Description:   Implements CNcdOperationManager class
*
*/


#include "ncdoperationmanager.h"

#include <e32err.h>
#include <s32mem.h>

#include "ncdbaseoperation.h"
#include "ncdstoragemanager.h"
#include "ncdprotocol.h"
#include "catalogshttpsession.h"
#include "ncdnodemanager.h"
#include "catalogsbasemessage.h"
#include "ncdloadnodeoperationimpl.h"
#include "ncdloadrootnodeoperationimpl.h"
#include "ncdloadbundlenodeoperationimpl.h"
#include "ncdnodefunctionids.h"
#include "ncddownloadoperationimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdproviderdefines.h"
#include "ncdstoragemanager.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdconfigurationmanager.h"
#include "catalogssmssession.h"
#include "ncdpurchaseoperationimpl.h"
#include "ncdnodeimpl.h"
#include "ncdnodelink.h"
#include "catalogscontext.h"
#include "catalogshttpconfig.h"
#include "catalogsutils.h"
#include "ncdproviderimpl.h"
#include "ncdinstalloperationimpl.h"
#include "ncdchildloadmode.h"
#include "ncdrightsobjectoperationimpl.h"
#include "ncdutils.h"
#include "ncdsearchoperationimpl.h"
#include "ncdnodefolder.h"
#include "ncdsubscriptionoperationimpl.h"
#include "ncdsubscriptionmanagerimpl.h"
#include "ncdcontentdownloadoperation.h"
#include "ncdstorageclient.h"
#include "ncdstorage.h"
#include "ncdstorageitem.h"
#include "ncddatabasestorage.h"
#include "ncdnodemetadataimpl.h"
#include "ncdreportmanager.h"
#include "ncdcreateaccesspointoperationimpl.h"
#include "ncdsendhttprequestoperationimpl.h"
#include "ncdconnectionmethod.h"
#include "ncdserverreportoperationimpl.h"
#include "ncdoperationtype.h"
#include "ncdnodeidentifiereditor.h"
#include "ncderrors.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"


// Because of security issues these capabilities are set for silent install
static _LIT_SECURITY_POLICY_C1( KSilentInstallInfoPolicy,
                                ECapabilityTrustedUI );


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//       
CNcdOperationManager* CNcdOperationManager::NewL( 
    CNcdProvider& aProvider,
    CNcdGeneralManager& aGeneralManager,    
    CNcdSubscriptionManager& aSubscriptionManager )
    {
    DLTRACEIN( ( "" ) );
    CNcdOperationManager* self = new( ELeave ) CNcdOperationManager(
        aProvider,
        aGeneralManager,          
        aSubscriptionManager );
        
    CleanupClosePushL( *self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DLTRACEOUT( ( "" ) );
    return self;        
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//       
CNcdOperationManager::~CNcdOperationManager()
    {
    DLTRACEIN((""));
    // Should we destroy the operations here if operations
    // exist? operations should delete themselves when the operation
    // is finished? This class may implement some interface that
    // will be used to inform the manager when operation is
    // finished...
    // Close the cache array.
    
    DLTRACE(("Closing operations"));
    
    
    for ( TInt i = 0; i < iOperationCache.Count(); ++i )
        {        
        iOperationCache[i]->Close();
        }
    

    DLTRACE(("Operations closed"));
    iOperationCache.Reset();
    
    iOperationQueue.Close();

    DLTRACEOUT((""));
    }

    
// ---------------------------------------------------------------------------
// CreateOperationL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateLoadNodeOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
        
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    DLTRACE(("create buffer"));

    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    DLINFO(("input length=%d",aMessage.InputLength()));
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );
    
    
    RDesReadStream stream( buf );
    CleanupReleasePushL( stream );

    DLTRACE(("get buffer data"));
    
    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        stream );
        
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context() ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }
        
    DLTRACE(("get load children flag"));
    TBool loadChildren = stream.ReadInt32L();
    DLTRACE(("get pageSize"));
    TInt pageSize = stream.ReadInt32L();
    DLTRACE(("get pageStart"));
    TInt pageStart = stream.ReadInt32L();
    DLTRACE(("get depth"));
    TInt depth = stream.ReadInt32L();
    DLTRACE(("get mode"));
    TNcdChildLoadMode mode = static_cast<TNcdChildLoadMode>(stream.ReadInt32L());
    DLTRACE(("get search flag"));
    TBool search = stream.ReadInt32L();
    CNcdSearchFilter* filter = NULL;
    if ( search )
        {
        DLTRACE(("get search filter"));
        filter = CNcdSearchFilter::NewLC();
        filter->SetContentPurposes( stream.ReadUint32L() );
        TInt keywordCount = stream.ReadInt32L();
        for ( TInt i = 0 ; i < keywordCount ; i++ )
            {
            HBufC* keyword = NULL;
            InternalizeDesL( keyword, stream );
            CleanupStack::PushL( keyword );
            filter->AddKeywordL( *keyword );
            CleanupStack::PopAndDestroy( keyword );
            }
        MNcdSearchFilter::TSearchMode mode;
        InternalizeEnumL( mode, stream );
        filter->SetSearchMode( mode );
        filter->SetRecursionDepthL( stream.ReadUint32L() );
        }
    
    DLTRACE(( _L("id, size, start, depth: %S, %d, %d, %d"),
              &identifier->NodeId(), pageSize, pageStart, depth ));
    
    
    CNcdNode& node = iNodeManager.NodeL( *identifier );

    DLTRACE(( _L("node found: %X"), &node ));
    
    TNcdResponseFilterParams filterParams;
    filterParams.iPageSize = pageSize;
    filterParams.iPageStart = pageStart;
    filterParams.iStructureDepth = depth;
    filterParams.iMetadataDepth = depth;
    filterParams.iMetadataPerLevel = pageSize;
                     
    // create the operation
    
    CNcdLoadNodeOperationImpl* operation = NULL;
    if( search )
        {
        DLTRACE(("Create search op"));
        const CNcdNodeIdentifier* parentId = NULL;
        if ( node.ClassId() == NcdNodeClassIds::ENcdSearchItemNodeClassId ||
            node.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId ||
            node.ClassId() == NcdNodeClassIds::ENcdSearchRootNodeClassId ||
            node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId )
            {
            DLTRACE(("search folder, use node's own parent"));
            parentId = &node.CreateAndSetLinkL().ParentIdentifier();
            }
        else
            {
            DLTRACE(("not a search folder, set search root as parent"));
            parentId = 
                &iNodeManager.CreateSearchRootL(aMessage.Session().Context()).Identifier();
            }
        // create op
        operation = CNcdSearchOperation::NewL(
            *identifier,
            *parentId,
            *filter,
            filterParams,
            iGeneralManager, 
            HttpSessionL( aMessage.Session().Context() ), 
            this, this, aMessage.Session(), loadChildren, mode,
            filter->RecursionDepth() );
        }
    else
        {
        DLTRACE(("Create load op"));
        CNcdNodeFactory::TNcdNodePurpose parentPurpose = CNcdNodeFactory::ENcdNormalNode;
        if ( !node.NodeLinkL().ParentIdentifier().ContainsEmptyFields() ) 
            {
            CNcdNode* parentNode = iNodeManager.NodePtrL( node.NodeLinkL().ParentIdentifier() );
            if ( parentNode )
                {                
                parentPurpose = CNcdNodeFactory::NodePurposeL( *parentNode );
                }
            }
        operation = CNcdLoadNodeOperationImpl::NewL(
            *identifier,
            node.NodeLinkL().ParentIdentifier(),
            parentPurpose,
            filterParams,
            iGeneralManager, 
            HttpSessionL( aMessage.Session().Context() ), 
            this, this, aMessage.Session(), loadChildren, mode, EFalse, EFalse );
        }

    DLTRACE(( _L("operation created: %X"), operation ));
    if( filter )
        {
        CleanupStack::PopAndDestroy( filter );
        }
    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &buf );
    
    
    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();
    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        // Is this ok?
        User::Leave( error );
        }
    }
    
// ---------------------------------------------------------------------------
// CreateOperationL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateLoadRootNodeOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
        
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    
    RBuf8 nodeIdentifierData;
    nodeIdentifierData.CreateL( aMessage.InputLength() );
    CleanupClosePushL( nodeIdentifierData );
    User::LeaveIfError( aMessage.ReadInput( nodeIdentifierData ) );

    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        nodeIdentifierData );
            
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context(),
            ETrue ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }
        
    // create the operation
    CNcdLoadRootNodeOperation* operation = CNcdLoadRootNodeOperation::NewL(
        aMessage.Session().Context().FamilyId().iUid, 
        iGeneralManager, 
		HttpSessionL( aMessage.Session().Context() ), 
		this, 
		aMessage.Session() );
    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &nodeIdentifierData );
    
    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    
    operation->SetHandle( operationHandle );
    
    operation->Close();
    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    } 


// ---------------------------------------------------------------------------
// CreateLoadBundleNodeOperationRequestL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateLoadBundleNodeOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
        
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    RBuf8 nodeIdentifierData;
    nodeIdentifierData.CreateL( aMessage.InputLength() );
    CleanupClosePushL( nodeIdentifierData );
    User::LeaveIfError( aMessage.ReadInput( nodeIdentifierData ) );

    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        nodeIdentifierData );
        
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context() ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }
    
    // create the operation    
    CNcdLoadBundleNodeOperation* operation = CNcdLoadBundleNodeOperation::NewL(
        *identifier, 
        iGeneralManager, 
		HttpSessionL( aMessage.Session().Context() ), 
		this, 
		*this, 
		aMessage.Session() );
    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &nodeIdentifierData );
    
    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    
    operation->SetHandle( operationHandle );
    
    operation->Close();
    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    } 
    
// ---------------------------------------------------------------------------
// CreateDownloadOperationL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateDownloadOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    
    DLTRACEIN((""));

    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
        
    // Read operation type    
    TNcdDownloadDataType type = static_cast<TNcdDownloadDataType>( 
        reader().ReadInt32L() );

    DLINFO(( "Operation type: %d", type ));
    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        reader() );
    
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context(),
        type == ENcdGenericFileDownload ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }
  
    DLINFO(( _L("NodeId: %S::%S"), &identifier->NodeNameSpace(),
        &identifier->NodeId() ));
  
    // Read download index
    TInt downloadIndex = reader().ReadInt32L();
    DLINFO(( "Download index: %d", downloadIndex ));
    
    // Check if the download already exists, completes the message
    // if it does
    if ( DownloadExistsL( aMessage, *identifier, type, downloadIndex ) )
        {
        DLTRACE(("Download already exists"));
        CleanupStack::PopAndDestroy( 2, &reader ); // identifier, reader
        return;        
        }
    
    CleanupStack::Pop( identifier );
    CleanupStack::PopAndDestroy( &reader ); // reader
    CleanupStack::PushL( identifier );
    
    // Get current context
    MCatalogsContext& context( aMessage.Session().Context() );
  
    HBufC* clientUid = context.FamilyId().Name().AllocLC();    
  
    MNcdStorageClient* storageClient = NULL;
    // Generic file download doesn't use storage
    if ( type != ENcdGenericFileDownload ) 
        {
        
        // Quick fix: ensures that StorageManager has the client
        MNcdStorage* storage = StorageL( *clientUid, 
            identifier->NodeNameSpace() );
        
        // Find the correct storage for the client
        storageClient = 
            &iStorageManager.StorageClientL( *clientUid );
        }
        
    
    DLTRACE(("Creating the download operation"));
    
    // Try to get session handler
    MNcdSessionHandler* sessionHandler = NULL;
    TRAP_IGNORE(       
        sessionHandler = &iProtocolHandler.SessionHandlerL( context ) );
    
    CNcdBaseOperation* operation = NULL;
    if ( type != ENcdContentDownload ) 
        {        
        // Create operation according to it's type
        operation = CNcdDownloadOperation::NewL(
            *this,
            type,
            *identifier, 
            iGeneralManager,         
            HttpSessionL( context ),
            sessionHandler,
            storageClient,
            context.FamilyId(),
            downloadIndex,
            aMessage.Session() );
        }
    else
        {
        MNcdStorage* dlStorage = StorageL( *clientUid, 
            NcdProviderDefines::KDownloadNamespace );
        
        // Create operation according to it's type
        operation = CNcdContentDownloadOperation::NewL(
            *this,
            *identifier, 
            iGeneralManager,         
            HttpSessionL( context ),
            ReportManagerL( context ),
            sessionHandler,
            dlStorage->DatabaseStorageL( 
                NcdProviderDefines::KDefaultDatabaseUid ),
            aMessage.Session(),
            downloadIndex );
        }
    DLTRACE(("Download operation created"));
    CleanupStack::PopAndDestroy( clientUid );
    CleanupStack::PopAndDestroy( identifier );    
    CleanupStack::PushL( operation );
    
    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );    
    
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    operation->Close();
    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    DLTRACEOUT((""));
    }
    


// ---------------------------------------------------------------------------
// CreatePurchaseOperationL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreatePurchaseOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Should use 8 bit version when client-server session supports it
    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    DLINFO(("input length=%d",aMessage.InputLength()));
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );

    RDesReadStream stream( buf );

    CleanupReleasePushL( stream );

    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        stream );
    
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context() ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }

    HBufC* purchaseOptionId = NULL;
    TInt length = InternalizeDesL( purchaseOptionId, stream );
    CleanupStack::PushL( purchaseOptionId );
   
    if( length < 1 )
        {
        User::Leave( KErrArgument );
        }

    // create the operation
    CNcdPurchaseOperationImpl* operation = CNcdPurchaseOperationImpl::NewL(
        identifier->NodeNameSpace(), 
        identifier->NodeId(),
        identifier->ClientUid(),
        *purchaseOptionId,
        iGeneralManager, 
        HttpSessionL( aMessage.Session().Context() ),
        SmsSessionL( aMessage.Session().Context() ),
        iSubscriptionManager,
        this,
        aMessage.Session() );

    
    CleanupStack::PopAndDestroy( purchaseOptionId );
    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &buf );


    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    

    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    }



// ---------------------------------------------------------------------------
// Create install operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateInstallOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {        
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    
    RBuf8 nodeIdentifierData;
    nodeIdentifierData.CreateL( aMessage.InputLength() );
    CleanupClosePushL( nodeIdentifierData );
    User::LeaveIfError( aMessage.ReadInput( nodeIdentifierData ) );

    // Create node identifier from the stream    
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( 
        nodeIdentifierData );
    
    if( ParallelOperationExistsForMetadataL( *identifier, aMessage.Session().Context() ) )
        {
        // Multiple simultaneous operations per one node are not supported, leave.
        User::Leave( KNcdErrorParallelOperationNotAllowed );
        }            
    
    // create the operation
    MCatalogsContext& context( requestSession.Context() );
    CNcdInstallOperation* operation = CNcdInstallOperation::NewL(
        *this,
        *identifier,
        iGeneralManager,
        HttpSessionL( context ),        
        ReportManagerL( context ),
        aMessage.Session() );

    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &nodeIdentifierData );
    
    
    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Create silent install operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateSilentInstallOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {        
    DLTRACEIN((""));

    if ( !aMessage.CheckSecurityPolicy( KSilentInstallInfoPolicy() ) )
        {
        DLTRACE(("Not enough capabilities for silent install"));
        User::Leave( KErrPermissionDenied );
        }

    // If there are enough capabilities, then just create a normal
    // install operation
    CreateInstallOperationRequestL( aMessage );

    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// Create rights object download and install operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateRightsObjectOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {        
    DCHECK_CSTACK;
    DLTRACEIN((""));

    // Read input data buffer
    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    DLINFO(("input length=%d",aMessage.InputLength()));
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );

    // Create a stream for reading out of the buffer
    RDesReadStream stream( buf );
    CleanupClosePushL( stream );

    // Read uri
    HBufC* downloadUri = NULL;
    InternalizeDesL( downloadUri, stream );
    CleanupStack::PushL( downloadUri );
    DLINFO((_L("Uri %S"), downloadUri ));

    // Read mime type
    HBufC* mimeType = NULL;
    InternalizeDesL( mimeType, stream );
    CleanupStack::PushL( mimeType );
    DLINFO((_L("Mime type %S"), mimeType ));

    // Read access point id
    TNcdConnectionMethod method;
    method.InternalizeL( stream );

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // create the operation
    CNcdRightsObjectOperation* operation = CNcdRightsObjectOperation::NewL(
        iGeneralManager,
        *downloadUri,
        *mimeType,
        method,
        *this,
        HttpSessionL( aMessage.Session().Context() ),
        aMessage.Session() );
    CleanupStack::PushL( operation );

    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );

    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLINFO(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );
    
    // Send the information to the client side
    TRAPD( error,
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );

    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }

    CleanupStack::PopAndDestroy( 4 ); // mimeType, downloadUri, stream-close, buf-close

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Create subscription operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateSubscriptionOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    DLTRACE(("create buffer"));

    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    DLINFO(( "input length=%d", aMessage.InputLength() ));
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );
    
    RDesReadStream stream( buf );
    CleanupReleasePushL( stream );

    DLTRACE(("get buffer data"));
    
    MNcdSubscriptionOperation::TType subscriptionOperationType =
        (MNcdSubscriptionOperation::TType)stream.ReadInt32L();

    CNcdSubscriptionOperation* operation( NULL );

    switch ( subscriptionOperationType )
        {            
        case MNcdSubscriptionOperation::EUnsubscribe:
            {
    
            DLINFO(( "-> EUnsubscribe" ));

            HBufC* subscriptionPurchaseOptionId( NULL );
            HBufC* subscriptionEntityId( NULL );
            HBufC* subscriptionNamespace( NULL );
            HBufC* subscriptionServerUri( NULL );

            InternalizeDesL( subscriptionPurchaseOptionId, stream );
            CleanupStack::PushL( subscriptionPurchaseOptionId );

            InternalizeDesL( subscriptionEntityId, stream );
            CleanupStack::PushL( subscriptionEntityId );

            InternalizeDesL( subscriptionNamespace, stream );
            CleanupStack::PushL( subscriptionNamespace );

            InternalizeDesL( subscriptionServerUri, stream );
            CleanupStack::PushL( subscriptionServerUri );
            
            operation =
                CNcdSubscriptionOperation::NewL(
                    MNcdSubscriptionOperation::EUnsubscribe,
                    *subscriptionPurchaseOptionId,
                    *subscriptionEntityId,
                    *subscriptionNamespace,
                    *subscriptionServerUri,
                    iGeneralManager,
                    iSubscriptionManager,
                    HttpSessionL( aMessage.Session().Context() ),
                    *this,
                    requestSession );
            
            CleanupStack::PopAndDestroy( subscriptionServerUri );
            CleanupStack::PopAndDestroy( subscriptionNamespace );
            CleanupStack::PopAndDestroy( subscriptionEntityId );
            CleanupStack::PopAndDestroy( subscriptionPurchaseOptionId );

            break;
            }

        case MNcdSubscriptionOperation::ERefreshSubscriptions:
            {
            DLINFO(( "-> ERefreshSubscriptions" ));

            operation =
                CNcdSubscriptionOperation::NewL(
                    MNcdSubscriptionOperation::ERefreshSubscriptions,
                    iGeneralManager,
                    iSubscriptionManager,
                    HttpSessionL( aMessage.Session().Context() ),
                    *this,
                    requestSession );

            break;
            }

        default:
            {            
            User::Leave( KErrNotSupported );

            break;
            }
        }

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &buf );

    CleanupClosePushL( *operation );

    DLTRACE(( _L("operation created: %X"), operation ));
    
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();
    DLTRACE(( "operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        // Is this ok?
        User::Leave( error );
        }
    }


// ---------------------------------------------------------------------------
// Create access point creation operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateCreateAccessPointOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {            
    DLTRACEIN((""));

    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );

    HBufC* accessPointData = NULL;
    InternalizeDesL( accessPointData, reader() );

    CleanupStack::PopAndDestroy( &reader );
    CleanupStack::PushL( accessPointData );
    
    DLINFO((_L("Data: %S"), accessPointData ));

    // create the operation
    // Note that ownership of accessPointData is transferred
    CNcdCreateAccessPointOperation* operation = CNcdCreateAccessPointOperation::NewL(
        accessPointData,
        *this,
        iGeneralManager,
        aMessage.Session() );
    CleanupStack::Pop( accessPointData );
    CleanupStack::PushL( operation );

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );

    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLINFO(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );
    
    // Send the information to the client side
    TRAPD( error,
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );

    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }

    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// Send HTTP request operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateSendHttpRequestOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );

    TNcdConnectionMethod method;
    method.InternalizeL( reader() );
    
    HBufC8* uri = NULL;
    InternalizeDesL( uri, reader() );
    CleanupStack::PushL( uri );    

    HBufC8* request = NULL;    
    InternalizeDesL( request, reader() );
    
    CleanupStack::PushL( request );
    
    DLINFO((_L("Data: %S"), request ));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // create the operation
    // Note that ownership of accessPointData is transferred
    CNcdSendHttpRequestOperation* operation = CNcdSendHttpRequestOperation::NewL(
        uri,
        request,
        method,
        iGeneralManager,
        *this,
        HttpSessionL( requestSession.Context() ),
        aMessage.Session()  );
    CleanupStack::Pop( 2, uri ); // request, uri
    CleanupStack::PopAndDestroy( &reader );
    CleanupStack::PushL( operation );


    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );

    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLINFO(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );
    
    // Send the information to the client side
    TRAPD( error,
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );

    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }


    }


// ---------------------------------------------------------------------------
// Restore serialized downloads
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::RestoreDownloadOperationsRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // Get current context
    MCatalogsContext& context( aMessage.Session().Context() );
  
    HBufC* clientUid = context.FamilyId().Name().AllocLC();    
      

    // Get client's download storage
    MNcdStorage* storage = StorageL( *clientUid, 
        NcdProviderDefines::KDownloadNamespace );
    
        
    CleanupStack::PopAndDestroy( clientUid );
    DLTRACE(("Creating the download operation"));

    // Get download db
    MNcdDatabaseStorage& db( storage->DatabaseStorageL( 
        NcdProviderDefines::KDefaultDatabaseUid ) );

    // Write the handles of resumed operations and return 
    // them to the client-side
    RCatalogsBufferWriter writer;
    writer.OpenLC();

    
    // Get all items from the db
    RPointerArray<MNcdStorageItem> items;
    db.StorageItemsL( items );
    CleanupClosePushL( items );
    
    DLTRACE(("Internalizing %d downloads", items.Count() ));

    MCatalogsHttpSession& httpSession( HttpSessionL( context ) );

    CNcdReportManager& reportManager( ReportManagerL( context ) );

    for ( TInt i = 0; i < items.Count(); ++i )
        {
        // Create operation
        CNcdContentDownloadOperation* operation = 
            CNcdContentDownloadOperation::NewLC( 
                *this, 
                iGeneralManager,
                httpSession,
                reportManager,
                db,
                aMessage.Session() );
        
        // Internalize from db
        items[i]->SetDataItem( operation );
        items[i]->ReadDataL();
        
        if ( operation->IsOk() && 
             !ParallelOperationExistsForMetadataL( operation->NodeId(),
                aMessage.Session().Context() ) )
            {
            TInt32 operationHandle( aMessage.Session().AddObjectL( operation ) );
            DLTRACE(("operation handle: %d", operationHandle ));
            operation->SetHandle( operationHandle );
            operation->Close();
            
            // Add to operation cache
            iOperationCache.AppendL( operation );

            CleanupStack::Pop( operation );         
            
            // add object data
            ExternalizeEnumL(
                NcdProviderDefines::ENcdStreamDataObject, writer() );            
            
            // Write handle
            writer().WriteInt32L( operationHandle );        
            
            // Write type
            writer().WriteInt32L( ENcdContentDownload );
            
            // Write metadata id
            
            operation->MetadataId().ExternalizeL( writer() );                
            
            writer().WriteInt32L( operation->CurrentDownload() );            
            
            reportManager.SetReportsAsUsedL( operation->MetadataId() );
            DLTRACE(("Download operation created"));            
            }    
        else
            {            
            DLTRACE(("Removing failed download"));    
            TRAP_IGNORE( items[i]->RemoveFromStorageL() );
            DLTRACE(("Pop&Destroy"));
            CleanupStack::PopAndDestroy( operation );
            }        
        }
    
    if ( items.Count() )
        {
        DLTRACE(("Commit"));
        db.CommitL();
        }
    
    // mark stream end
    ExternalizeEnumL( NcdProviderDefines::ENcdStreamDataEnd, writer() );
    CleanupStack::PopAndDestroy( &items );
    
    // Delete downloads that were not restored
    httpSession.DeleteRestoredDownloads();

    reportManager.RemoveUnusedReportsL();
        
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( writer.PtrL(), KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }

    // writer, operationHandles
    CleanupStack::PopAndDestroy( &writer ); 
       
    DLTRACEOUT((""));

    }

// ---------------------------------------------------------------------------
// Create server report operation
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CreateServerReportOperationRequestL(
    MCatalogsBaseMessage& aMessage )
    {        
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );
    
    // create the operation
    // Get current context
    MCatalogsContext& context( aMessage.Session().Context() );
    CNcdServerReportOperation* operation = CNcdServerReportOperation::NewL(
        iGeneralManager,
        *this,
        ReportManagerL( context ),
        aMessage.Session() );
    
    CleanupStack::PushL( operation );
    // Add the operation to the session and get the handle.
    // If the operation already existed in the session we will still
    // get a new handle to the same object.
    TInt32 operationHandle( requestSession.AddObjectL( operation ) );
    operation->SetHandle( operationHandle );
    
    // AddObjectL adds reference count by 1
    operation->Close();

    DLTRACE(("operation handle: %d", operationHandle ));

    // Because we created a new operation, it should be added to the
    // cache. 
    iOperationCache.AppendL( operation );
    CleanupStack::Pop( operation );    
    
    // Send the information to the client side
    TRAPD( error, 
           aMessage.CompleteAndReleaseL( operationHandle, KErrNone ) );
    if( error != KErrNone )
        {        
        iOperationCache.Remove( iOperationCache.Count() - 1 );
        }
    DLTRACEOUT((""));
    }


MCatalogsHttpSession& CNcdOperationManager::HttpSessionL( MCatalogsContext& aContext )
    {
    TNcdProviderContext providerContext;
    iProvider.GetProviderContextL( aContext, providerContext );
    return *providerContext.iHttpSession;
    }

MCatalogsSmsSession& CNcdOperationManager::SmsSessionL( MCatalogsContext& aContext )
    {
    TNcdProviderContext providerContext;
    iProvider.GetProviderContextL( aContext, providerContext );
    return *providerContext.iSmsSession;
    }

CNcdReportManager& CNcdOperationManager::ReportManagerL( MCatalogsContext& aContext )
    {
    TNcdProviderContext providerContext;
    iProvider.GetProviderContextL( aContext, providerContext );
    return *providerContext.iReportManager;
    }


// ---------------------------------------------------------------------------
// ReceiveMessage
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                           TInt aFunctionNumber )
    {
    DLTRACEIN((""));

    // The message is always required.
    if( aMessage == NULL )
        {
        // Do not do anything here because we can not give any
        // feedback
        return;
        }

    TInt trapError( KErrNone );
        
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdOperationManagerCreateLoadNodeOperation:
            DLTRACE(("Creating load node operation"));

            TRAP( trapError, CreateLoadNodeOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateDownloadOperation:
            DLTRACE(("Creating download operation"));

            TRAP( trapError, CreateDownloadOperationRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdOperationManagerCreateLoadRootNodeOperation:
            DLTRACE(("Creating load root node operation"));
            
            TRAP( trapError, CreateLoadRootNodeOperationRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdOperationManagerCreateLoadBundleNodeOperation:
            DLTRACE(("Creating load bundle node operation"));
            
            TRAP( trapError, CreateLoadBundleNodeOperationRequestL( *aMessage ) );
            break;
        
        case NcdNodeFunctionIds::ENcdOperationManagerCreatePurchaseOperation:
            DLTRACE(("Creating purchase operation"));
            
            TRAP( trapError, CreatePurchaseOperationRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdOperationManagerCreateRightsObjectOperation:
            DLTRACE(("Creating rights object download and install operation"));
            
            TRAP( trapError, CreateRightsObjectOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateInstallOperation:
            DLTRACE(("Creating install operation"));
            
            TRAP( trapError, CreateInstallOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateSilentInstallOperation:
            DLTRACE(("Creating install operation"));
            
            TRAP( trapError, CreateSilentInstallOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateSubscriptionOperation:
            DLTRACE(("Creating subscription operation"));
            
            TRAP( trapError, CreateSubscriptionOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerRestoreContentDownloads:
            DLTRACE(("Restoring downloads"));
            
            TRAP( trapError, RestoreDownloadOperationsRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateCreateAccessPointOperation:
            DLTRACE(("Creating create accesspoint operation"));
            
            TRAP( trapError, CreateCreateAccessPointOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateSendHttpRequestOperation:
            DLTRACE(("Creating HTTP request sending operation"));
            
            TRAP( trapError, CreateSendHttpRequestOperationRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdOperationManagerCreateServerReportOperation:
            DLTRACE(("Creating server report operation"));
            
            TRAP( trapError, CreateServerReportOperationRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;

        default:
            DASSERT( 0 );
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        DLTRACE(("ERROR, Complete and release %d", trapError));
        
        aMessage->CompleteAndRelease( trapError );
        }

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// CounterPartLost
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::CounterPartLost( const MCatalogsSession& /*aSession*/ )
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    
    }
    

// ---------------------------------------------------------------------------
// RemoveOperation
// ---------------------------------------------------------------------------
//           
void CNcdOperationManager::RemoveOperation( CNcdBaseOperation& aOperation )
    {
    DLTRACEIN((""));
    TInt index = iOperationCache.Find( &aOperation );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(("Removing operation"));
        iOperationCache.Remove( index );
        }
        
    index = iOperationQueue.Find( &aOperation );
    if ( index != KErrNotFound )
        {
        // Should never come here.
        DASSERT( EFalse );
        iOperationQueue.Remove( index );
        }
    }
    
    
// ---------------------------------------------------------------------------
// QueueOperation
// ---------------------------------------------------------------------------
//           
void CNcdOperationManager::QueueOperationL( CNcdBaseOperation& aOperation )
    {
    DLTRACEIN(("queue size: %d", iOperationQueue.Count()));
    iOperationQueue.AppendL( &aOperation );
    
    // Start the operation if it is the only one in queue, or there are no load bundle
    // node operations which may lock the database. If the operation is a load bundle operation
    // it should not be started if the queue is not empty since there are other operations
    // running.
    if ( aOperation.Type() == ELoadBundleNodeOperation )
        {
        if ( iOperationQueue.Count() == 1 )
            {
            aOperation.RunOperation();
            }
        }
    else
        {
        if ( !QueuedLoadBundleOperationsExists() )
            {
            aOperation.RunOperation();
            }
        }
    }
    
// ---------------------------------------------------------------------------
// QueueOperation
// ---------------------------------------------------------------------------
//           
void CNcdOperationManager::QueuedOperationComplete( CNcdBaseOperation& aOperation )
    {
    DLTRACEIN(("operation-ptr: %x", &aOperation));
    
    TInt index = iOperationQueue.Find( &aOperation );
    
    if ( index != KErrNotFound )
        {        
        iOperationQueue.Remove( index );

        // Start new operations only if the completed one was the first in queue since
        // the first one is otherwise running still.
        if ( index == 0 && iOperationQueue.Count() ) 
            {
            if ( iOperationQueue[ 0 ]->Type() == ELoadBundleNodeOperation )
                {
                // If the next in queue is load bundle node, it can be started since
                // it is not started yet.
                iOperationQueue[ 0 ]->ContinueOperationL();
                }
                                
            else if ( aOperation.Type() == ELoadBundleNodeOperation ) 
                {
                // Completed operation was a bundle load op, so the next in queue are not
                // started yet. Start the next operations in queue before the first load
                // bundle node operation.
                for ( TInt i = 0; i < iOperationQueue.Count(); i++ )
                    {
                    if ( iOperationQueue[ i ]->Type() != ELoadBundleNodeOperation )
                        {
                        iOperationQueue[ i ]->ContinueOperationL();
                        }
                    else
                        {
                        break;
                        }
                    }
                }
            }
        }
    }
                    
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//           
CNcdOperationManager::CNcdOperationManager( 
    CNcdProvider& aProvider,
    CNcdGeneralManager& aGeneralManager,      
    CNcdSubscriptionManager& aSubscriptionManager )
	: 
    CCatalogsCommunicable(),
    iProvider( aProvider ),
    iGeneralManager( aGeneralManager ),
    iStorageManager( aGeneralManager.StorageManager() ),
    iProtocolHandler( aGeneralManager.ProtocolManager() ),
    iNodeManager( aGeneralManager.NodeManager() ),
    iPurchaseHistory( aGeneralManager.PurchaseHistory() ),
    iConfigurationManager( aGeneralManager.ConfigurationManager() ),
    iAccessPointManager( aGeneralManager.AccessPointManager() ),
    iSubscriptionManager( aSubscriptionManager )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//       
void CNcdOperationManager::ConstructL()
    {
    DLTRACEIN( ( "this: %X", this ) );
    
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// StorageL
// ---------------------------------------------------------------------------
//    
MNcdStorage* CNcdOperationManager::StorageL( const TDesC& aClientUid,
    const TDesC& aNamespace ) const
    {    
    DLTRACEIN((""));
    MNcdStorage* storage = NULL;
        
    
    DLTRACE(( _L("Namespace: %S"), &aNamespace ));
    TRAPD( err, storage = &iStorageManager.StorageL( aClientUid, aNamespace ) );
    
    if ( err == KErrNotFound ) 
        {
        DLTRACE(("Creating storage for the client"));
        err = KErrNone;
        TRAP( err, storage = &iStorageManager.CreateStorageL( aClientUid, aNamespace ) );
        
        if ( err == KErrAlreadyExists )
            {
            err = KErrNone;
            storage = &iStorageManager.StorageL( aClientUid, aNamespace );
            }
        }

    if ( err != KErrNone )
        {
        DLTRACE(("Leaving: %i", err));
        User::Leave( err );   
        }
        
    DLTRACEOUT((""));
    return storage;
    }
    
    
// ---------------------------------------------------------------------------
// Release request
// ---------------------------------------------------------------------------
//    
void CNcdOperationManager::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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
    

// ---------------------------------------------------------------------------
// Checks if a download already exists
// ---------------------------------------------------------------------------
//    
TBool CNcdOperationManager::DownloadExistsL( MCatalogsBaseMessage& aMessage, 
    const CNcdNodeIdentifier& aIdentifier, 
    TNcdDownloadDataType aType, 
    TInt aIndex )
    {
    DLTRACEIN(("Op cache count: %d", iOperationCache.Count()));
    TInt32 handle = 0;
    
    const CNcdNodeIdentifier* metaId = &aIdentifier;

    // Use metadata id's for comparison since temp nodes have different
    // node id's but same metadata id's as normal nodes
    //
    // Generic file downloads don't have node nor metadata. The given node id
    // contains the URL and target filename
    if ( aType != ENcdGenericFileDownload )
        {
        DLTRACE(("Not skin nor generic DL, getting metadata id"));        
        
        metaId = &iNodeManager.NodeL( aIdentifier ).NodeMetaDataL().Identifier();
        }        
    
    for ( TInt i = 0; i < iOperationCache.Count(); ++i )
        {
        // Check other downloads but content
        if ( aType != ENcdContentDownload && 
             iOperationCache[i]->Type() == 
                EDownloadOperation )
            {
            CNcdDownloadOperation* op = static_cast<CNcdDownloadOperation*>(
                iOperationCache[i] );
            if ( op->MatchDownload( *metaId, aType, aIndex ) )
                {
                DLTRACE(("Found a matching download"));
                handle = op->Handle();
                break;
                }
            }
        // check content downloads
        else if ( aType == ENcdContentDownload &&
            iOperationCache[i]->Type() ==
                EContentDownloadOperation )
            {
            CNcdContentDownloadOperation* op = static_cast<
                CNcdContentDownloadOperation*>(
                iOperationCache[i] );
            if ( op->MatchDownload( *metaId, aType, aIndex ) )
                {
                DLTRACE(("Found a matching content download"));
                handle = op->Handle();
                break;
                }
            }            
        }
    if ( handle ) 
        {
        // Send the information to the client side
        aMessage.CompleteAndReleaseL( handle, KErrNone );
        return ETrue;
        }
    DLTRACEOUT(("No matching download"));
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Checks if there are load bundle node operations in operation queue.
// ---------------------------------------------------------------------------
//        
TBool CNcdOperationManager::QueuedLoadBundleOperationsExists() const
    {
    DLTRACEIN((""));
    TInt queueSize = iOperationQueue.Count();
    for ( TInt i = 0; i < queueSize; i++ )
        {
        if ( iOperationQueue[ i ]->Type() == ELoadBundleNodeOperation )
            {
            DLINFO(("queued load bundle op exists"));
            return ETrue;
            }
        }
        
    DLINFO(("no queued load bundle op"));
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Checks if there are on-going operations for the metadata for another client
// ---------------------------------------------------------------------------
// 
TBool CNcdOperationManager::ParallelOperationExistsForMetadataL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const MCatalogsContext& aContext,
    TBool aCompareIdsDirectly ) const
    {
    DLTRACEIN((""));
    const CNcdNodeIdentifier* metaId = NULL;

    // If parameter suggests that ids should not be compared directly,
    // make a safety check.
    if ( !aCompareIdsDirectly
         && NcdNodeIdentifierEditor::IdentifiesSomeRoot( aNodeIdentifier ) )
        {
        // In the case of root nodes, the metadata does not
        // exist. So, force direct comparing of node ids.
        DLINFO(("Force direct comparing of node ids"));
        aCompareIdsDirectly = ETrue;
        }

    if( !aCompareIdsDirectly )
        {
        // The node should always exist.
        CNcdNode& node( iNodeManager.NodeL( aNodeIdentifier ) );

        // Above, a root node check was made to force direct comparing 
        // of ids for root nodes because they do not have metadata. So,
        // in root node cases we do not come here.
        // In normal cases, the metadata should always exist. 
        // But, to be safe in all cases, check if metadata is NULL here.  
        // Notice, ownership is not transferred here.
        CNcdNodeMetaData* metadata( node.NodeMetaData() );

        if ( metadata )
            {
            DLINFO(("Metadata existed for paralled operations checking"));
            // Ownership is not transferred. Metadata owns its own identifier.
            metaId = &metadata->Identifier();            
            }
        else
            {
            // Because metadata is NULL, 
            // no operation should exist for it either.
            DLTRACEOUT(("No metadata to compare parallel operations"));
            return EFalse;
            }
        }
        
    // Check operation cache
    for ( TInt i = 0; i < iOperationCache.Count(); i++ )
        {
        // Check is only done if the client is not the same
        if( iOperationCache[i]->Session().Context().FamilyId() != aContext.FamilyId() ||
            ( iOperationCache[i]->Session().Context().SecureId() == aContext.SecureId() && 
              iOperationCache[i]->Session().Context().InstanceId() == aContext.InstanceId() ) )
            {
            continue;
            }
        // Try to get id from operation, not all operations are node specific.
        const CNcdNodeIdentifier* id = GetNodeIdFromOperation( *iOperationCache[i] );
        if( !id )
            {
            continue;
            }
        if( !aCompareIdsDirectly &&
            NcdNodeIdentifierEditor::DoesMetaDataIdentifierMatchL( *metaId, *id ) )
            {
            return ETrue;
            }
        else if( aNodeIdentifier.Equals( *id ) )
            {
            return ETrue;
            }
        }
    
    // Check operation queue
    for ( TInt i = 0; i < iOperationQueue.Count(); i++ )
        {
        // Check is only done if the client is not the same
        if( iOperationQueue[i]->Session().Context().FamilyId() != aContext.FamilyId() ||
            ( iOperationQueue[i]->Session().Context().SecureId() == aContext.SecureId() && 
              iOperationQueue[i]->Session().Context().InstanceId() == aContext.InstanceId() ) )
            {
            continue;
            }
        // Try to get id from operation, not all operations are node specific.
        const CNcdNodeIdentifier* id = GetNodeIdFromOperation( *iOperationQueue[i] );
        if( !id )
            {
            continue;
            }
        if( !aCompareIdsDirectly &&
            NcdNodeIdentifierEditor::DoesMetaDataIdentifierMatchL( *metaId, *id ) )
            {
            return ETrue;
            }
        else if( aNodeIdentifier.Equals( *id ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
const CNcdNodeIdentifier* CNcdOperationManager::GetNodeIdFromOperation(
    const CNcdBaseOperation& aOperation ) const
    {
    DLTRACEIN((""));
    switch( aOperation.Type() )
        {
        case ELoadNodeOperation:
        case ESearchOperation:
            {
            const CNcdLoadNodeOperationImpl& op =
                static_cast<const CNcdLoadNodeOperationImpl&>( aOperation );
            return op.NodeIdentifier();
            }
        case ELoadRootNodeOperation:
            {
            const CNcdLoadRootNodeOperation& op =
                static_cast<const CNcdLoadRootNodeOperation&>( aOperation );
            return &op.NodeIdentifier();
            }
        case ELoadBundleNodeOperation:
            {
            const CNcdLoadBundleNodeOperation& op =
                static_cast<const CNcdLoadBundleNodeOperation&>( aOperation );
            return &op.NodeIdentifier();
            }
        case EDownloadOperation:
            {
            const CNcdDownloadOperation& op =
                static_cast<const CNcdDownloadOperation&>( aOperation );
            return &op.NodeIdentifier();
            }
        case EInstallOperation:
            {
            const CNcdInstallOperation& op =
                static_cast<const CNcdInstallOperation&>( aOperation );
            return &op.NodeIdentifier();
            }
        case EPurchaseOperation:
            {
            const CNcdPurchaseOperationImpl& op =
                static_cast<const CNcdPurchaseOperationImpl&>( aOperation );
            return &op.NodeIdentifier();
            }
        case EContentDownloadOperation:
            {
            const CNcdContentDownloadOperation& op =
                static_cast<const CNcdContentDownloadOperation&>( aOperation );
            return &op.NodeId();
            }
        default:
            {
            return NULL;
            }
        // These operations are not node-specific, hence no id information can be
        // retrieved from them:
        /*EDownloadSubOperation,
        EDescriptorDownloadSubOperation,
        ESendNotificationSubOperation,
        ERightsObjectOperation,
        ECreateAccessPointOperation,
        ESendHttpRequestOperation,
        EServerReportOperation,
        ESubscriptionOperation*/
        }
    }
