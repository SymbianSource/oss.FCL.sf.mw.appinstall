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
* Description:   CNcdLoadNodeOperationProxy class implementation
*
*/


#include <badesca.h>
#include <s32strm.h>

#include "ncdloadnodeoperationproxy.h"
#include "ncdloadnodeoperationobserver.h"
#include "ncdoperationfunctionids.h"
#include "catalogsdebug.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationproxyremovehandler.h"
#include "ncdqueryimpl.h"
#include "ncdproviderdefines.h"
#include "ncdnodefolderproxy.h"
#include "ncdsearchrootnodeproxy.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationProxy* CNcdLoadNodeOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    TBool aSearch,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdLoadNodeOperationProxy* self = CNcdLoadNodeOperationProxy::NewLC( 
        aSession,
        aHandle,
        aRemoveHandler,
        aNode,
        aNodeManager,
        aSearch,
        aLocalizer );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationProxy* CNcdLoadNodeOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    TBool aSearch,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdLoadNodeOperationProxy* self =
        new( ELeave ) CNcdLoadNodeOperationProxy( aSearch, aLocalizer );    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( aSession, aHandle, aRemoveHandler, aNode, aNodeManager );
    return self;
    }

// ---------------------------------------------------------------------------
// From MNcdLoadNodeOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::AddObserverL(
    MNcdLoadNodeOperationObserver* aObserver )
    {
    DLTRACEIN(("adding observer: %d", aObserver ));
    iObservers.AppendL( aObserver );
    }
    
// ---------------------------------------------------------------------------
// From MNcdLoadNodeOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdLoadNodeOperationProxy::RemoveObserver(
    MNcdLoadNodeOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    TInt index = iObservers.Find( aObserver );
    if ( index != KErrNotFound )
        {
        DLINFO(("removing observer: %d", iObservers[ index ] ));
        iObservers.Remove( index );
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }
    
// ---------------------------------------------------------------------------
// From MNcdConfigurationData
// ?implementation_description
// ---------------------------------------------------------------------------
//
HBufC8* CNcdLoadNodeOperationProxy::ProtocolResponseDataL() 
    {
    HBufC8* buf(NULL);    
    // Get the configuration response data from server side operation.
    User::LeaveIfError( ClientServerSession().SendSyncAlloc( ENCDOperationFunctionGetData,
        SendBuf8L(),
        buf,
        Handle(),
        0 ) );
        
    return buf;
    }

// ---------------------------------------------------------------------------
// From MCatalogsBase
// ?implementation_description
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdLoadNodeOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(MNcdLoadNodeOperation::KInterfaceUid);
    }

MNcdNodeContainer* CNcdLoadNodeOperationProxy::SearchRootNodeL()
    {
    CNcdSearchRootNodeProxy* searchRoot = &iNodeManager->SearchRootNodeL();
    searchRoot->AddRef();
    return searchRoot;
    }

const TAny* CNcdLoadNodeOperationProxy::QueryInterfaceL( TInt aInterfaceId ) const
        {
        const TAny* result = NULL;
        switch( aInterfaceId )
            {
            case MNcdSearchOperation::KInterfaceUid:
                {
                if( iSearch )
                    {
                    result = static_cast< const MNcdSearchOperation* >( this );
                    }
                break;
                }
            case MNcdLoadNodeOperation::KInterfaceUid:
                {
                result = static_cast< const MNcdLoadNodeOperation* >( this );
                break;
                }
            case MNcdOperation::KInterfaceUid:
                {
                result = static_cast< const MNcdOperation* >( this );
                break;
                }
            case MCatalogsBase::KInterfaceUid:
                {
                result = static_cast< const MCatalogsBase* >( this );
                break;
                }
            case MNcdConfigurationData::KInterfaceUid:
                {
                if ( NodeProxy()->NodeIdentifier().NodeNameSpace() ==
                    NcdProviderDefines::KRootNodeNameSpace ) 
                    {
                    result = static_cast< const MNcdConfigurationData* >( this );
                    }
                break;
                }
            default:
                {
                break;
                }
            }

        if( result != NULL )
            {
            AddRef();
            }

        return result;
        }        


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationProxy::CNcdLoadNodeOperationProxy(
    TBool aSearch, MNcdClientLocalizer* aLocalizer )
: CNcdOperation< MNcdSearchOperation>( aLocalizer ), iSearch( aSearch )
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationProxy::~CNcdLoadNodeOperationProxy()
    {    
    DLTRACEIN((""));
    iLoadedNodes.ResetAndDestroy();
    DASSERT( iRemoveHandler );
    if ( iRemoveHandler )
        {        
        DLTRACE(("Removing proxy from remove handler"));
        iRemoveHandler->RemoveOperationProxy( *this );
        }
    iObservers.Reset();
        
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager )
    {
    // first call parent's ConstructL
    CNcdBaseOperationProxy::ConstructL( aSession,
                                        aHandle,
                                        aRemoveHandler,
                                        aNode,
                                        aNodeManager );
    iNodeManager = aNodeManager;
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::HandleCompletedMessage(
    TNcdOperationMessageCompletionId aCompletionId,
    RReadStream& aReadStream,
    TInt aDataLength )
    {
    DLTRACEIN((_L("aCompletionId =%d, aDataLength =%d"), aCompletionId,
        aDataLength));
    switch ( aCompletionId )
        {
        case ENCDOperationMessageCompletionNodesUpdated:
            {
            DLTRACE(("ENCDOperationMessageCompletionNodesUpdated"));
            TRAPD(err,
                {
                TNcdSendableProgress progress;
                progress.InternalizeL( aReadStream );
                TInt nodeIdCount = aReadStream.ReadInt32L();
                iLoadedNodes.ResetAndDestroy();
                for ( TInt i = 0 ; i < nodeIdCount ; i++ )
                    {
                    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC(
                        aReadStream );
                    DLINFO( (_L("Updated node: %S"), &nodeId->NodeId() ) );
                    iLoadedNodes.AppendL( nodeId );
                    CleanupStack::Pop( nodeId );
                    }
                SendContinueMessageL();
                }); //TRAPD
            aReadStream.Close();
            if ( err == KErrNone )
                {                
                ProgressCallback();
                }
            else
                {
                CompleteCallback( err );
                } 
            DLTRACE(("ENCDOperationMessageCompletionNodesUpdated done"));           
            break;
            }
        default:
            {
            CNcdBaseOperationProxy::HandleCompletedMessage( aCompletionId,
                aReadStream,
                aDataLength );
            break;
            }
        }
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// From CNcdBaseOperationProxy
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::ProgressCallback()
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    RCatalogsArray< MNcdNode > array;
    for ( TInt j = 0 ; j < iLoadedNodes.Count() ; j++ )
        {
        DLINFO((_L("Trying to get loaded node: namespace=%S, id=%S"),
            &iLoadedNodes[j]->NodeNameSpace(), &iLoadedNodes[j]->NodeId() ));
        TRAPD( err,
            {
            CNcdNodeProxy& node = iNodeManager->NodeL( *iLoadedNodes[j] );            
            node.OperationComplete( *this, KErrNone );
            array.AppendL( &node );
            node.InternalAddRef();
            }); // TRAPD
        if ( err != KErrNone )
            {
            DLINFO(( "error: %d", err ));            
            }
        }
    
    if ( array.Count() < 1 )
        {
        DLINFO(("error: couldn't get loaded nodes from nodemanager!"));
        //return;
        }
    
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        DASSERT( iObservers[i] );        
        iObservers[i]->NodesUpdated( *this, array );
        }
    for ( TInt i = 0 ; i < array.Count() ; i++ )
        {
        static_cast<CNcdNodeProxy*>( array[i] )->InternalRelease();
        }
    array.Close();
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// From CNcdBaseOperationProxy
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::QueryReceivedCallback( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        aQuery->AddRef();
        iObservers[i]->QueryReceived( *this, aQuery );
        }
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// From CNcdBaseOperationProxy
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN(( "err: %d", aError ));
    DPROFILING_BEGIN( x );
    AddRef();
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        DLINFO(("Calling observer: %x", iObservers[i] ));
        iObservers[i]->OperationComplete( *this, aError );
        }
    Release();
    DPROFILING_END( x );
    DLTRACEOUT((""));    
    }
