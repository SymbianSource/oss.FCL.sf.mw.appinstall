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
* Description:  
*
*/


#include "catalogshttpconfig.h"
#include "catalogshttpsessionimpl.h"
#include "catalogstransporttypes.h"
#include "catalogshttpdownloadmanagerimpl.h"
#include "catalogshttpoperation.h"
#include "catalogshttptransactionmanagerimpl.h"
#include "catalogsdebug.h"
#include "catalogstransportimpl.h"
#include "catalogshttpconnectionmanager.h"
#include "catalogsconnectioneventsink.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsHttpSession* CCatalogsHttpSession::NewL( 
    TInt32 aSessionId, 
    CCatalogsTransport& aOwner, 
    MCatalogsHttpSessionManager& aSessionManager,
    TBool aCleanupSession )
    {
    DLTRACEIN( ( "" ) );
    CCatalogsHttpSession* self = new ( ELeave ) CCatalogsHttpSession(
        aSessionId, aOwner, aSessionManager );
    CleanupStack::PushL( self );
    self->ConstructL( aSessionManager, aCleanupSession );
    CleanupStack::Pop( self );
    DLTRACEOUT( ( "self: %X", self ) );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpSession::~CCatalogsHttpSession()
    {
    DLTRACEIN( ( "" ) );
    // disconnect network connection if no clients use it
    TRAP_IGNORE( iSessionManager.DisconnectL() );
    delete iDownloadMgr;
    delete iTransactionMgr;
    delete iConnectionMgr;
    delete iConnectionEventSink;
    
    if ( iOwner ) 
        {        
        iOwner->RemoveSession( this );
        }
    
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSession::AddRef()
    {
    iRefCount++;
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSession::Release()
    {
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }
        

// ---------------------------------------------------------------------------
// Reference count
// ---------------------------------------------------------------------------
//        
TInt CCatalogsHttpSession::RefCount() const
    {
    return iRefCount;
    }



// ---------------------------------------------------------------------------
// Session id getter
// ---------------------------------------------------------------------------
//
TInt32 CCatalogsHttpSession::SessionId() const
    {
    return iSessionId;
    }
        
        
// ---------------------------------------------------------------------------
// Session type getter
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSession::SessionType() const
    {
    return KCatalogsTransportHttpInterface;
    }


// ---------------------------------------------------------------------------
// Download creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::CreateDownloadL( 
    const TDesC16& aUrl, TBool aStart, MCatalogsHttpObserver* aObserver)
    {        
    return iDownloadMgr->CreateDownloadL( aUrl, aStart, aObserver );    
    }

// ---------------------------------------------------------------------------
// Download creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::CreateDownloadL( 
    const TDesC8& aUrl, TBool aStart, MCatalogsHttpObserver* aObserver)
    {        
    return iDownloadMgr->CreateDownloadL( aUrl, aStart, aObserver );    
    }


// ---------------------------------------------------------------------------
// Transaction creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::CreateTransactionL( 
    const TDesC16& aUrl, MCatalogsHttpObserver* aObserver )
    {
    return iTransactionMgr->CreateTransactionL( aUrl, aObserver );
    }


// ---------------------------------------------------------------------------
// Transaction creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::CreateTransactionL( 
    const TDesC8& aUrl, MCatalogsHttpObserver* aObserver )
    {
    return iTransactionMgr->CreateTransactionL( aUrl, aObserver );
    }


// ---------------------------------------------------------------------------
// Download transaction creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::CreateDlTransactionL( 
    const TDesC8& aUrl, MCatalogsHttpObserver& aObserver,
    const CCatalogsHttpConfig& aConfig )
    {
    return iTransactionMgr->CreateDlTransactionL( aUrl, aObserver, aConfig );
    }


// ---------------------------------------------------------------------------
// Download getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::Download( 
    const TCatalogsTransportOperationId& aId ) const
    {
    return iDownloadMgr->Download( aId );
    }


// ---------------------------------------------------------------------------
// Transaction getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpSession::Transaction( 
    const TCatalogsTransportOperationId& aId ) const
    {    
    return iTransactionMgr->Transaction( aId );
    }


// ---------------------------------------------------------------------------
// Download array getter
// ---------------------------------------------------------------------------
//
const RCatalogsHttpOperationArray& 
    CCatalogsHttpSession::CurrentDownloads() const
    {
    return iDownloadMgr->CurrentDownloads();
    }
        

// ---------------------------------------------------------------------------
// Restored downloads getter
// ---------------------------------------------------------------------------
//
const RCatalogsHttpOperationArray&
    CCatalogsHttpSession::RestoredDownloads() const
    {
    return iDownloadMgr->RestoredDownloads();
    }
        

// ---------------------------------------------------------------------------
// Moves a restored download to current downloads
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSession::MoveRestoredDlToCurrentDls( 
    MCatalogsHttpOperation& aDownload )
    {
    return iDownloadMgr->MoveRestoredDlToCurrentDls( aDownload );
    }
    
// ---------------------------------------------------------------------------
// Transaction array getter
// ---------------------------------------------------------------------------
//
const RCatalogsHttpOperationArray& 
    CCatalogsHttpSession::CurrentTransactions() const
    {
    return iTransactionMgr->CurrentTransactions();
    }


// ---------------------------------------------------------------------------
// Cancel all operations
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::CancelAllOperations()
    {
    CancelAllDownloads();
    CancelAllTransactions();    
    }



// ---------------------------------------------------------------------------
// Cancel all operations
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::NotifyCancelAllOperations()
    {
    iSessionManager.SetResumeMode( EFalse );
    iTransactionMgr->NotifyCancelAll();
    iDownloadMgr->NotifyCancelAll();    
    iSessionManager.SetResumeMode( ETrue );
    }

// ---------------------------------------------------------------------------
// Cancel all downloads
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::CancelAllDownloads()
    {
    // Disable operation resuming during cancel so that ops are not
    // unnecessarily resumed
    iSessionManager.SetResumeMode( EFalse );
    iDownloadMgr->CancelAll();    
    iSessionManager.SetResumeMode( ETrue );
    }


// ---------------------------------------------------------------------------
// Cancel all transactions
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::CancelAllTransactions()
    {
    // Disable operation resuming during cancel so that ops are not
    // unnecessarily resumed
    iSessionManager.SetResumeMode( EFalse );
    iTransactionMgr->CancelAll();
    iSessionManager.SetResumeMode( ETrue );
    }


// ---------------------------------------------------------------------------
// Deletes restored downloads
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::DeleteRestoredDownloads()
    {
    iDownloadMgr->DeleteRestoredDownloads();
    }
    
// ---------------------------------------------------------------------------
// Transaction configuration getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& 
    CCatalogsHttpSession::DefaultTransactionConfig() const
    {    
    return iTransactionMgr->DefaultConfig();
    }


// ---------------------------------------------------------------------------
// Download configuration getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CCatalogsHttpSession::DefaultDownloadConfig() const
    {
    return iDownloadMgr->DefaultConfig();
    }
  
        
// ---------------------------------------------------------------------------
// Set default accesspoint for downloads and transactions
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::SetDefaultConnectionMethod( 
    const TCatalogsConnectionMethod& aMethod )
    {
    iDownloadMgr->DefaultConfig().SetConnectionMethod( aMethod );
    iTransactionMgr->DefaultConfig().SetConnectionMethod( aMethod );

    iConnectionMgr->SetDefaultConnectionMethod( aMethod );
    }
// ---------------------------------------------------------------------------
// SetOwner
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::SetOwner( CCatalogsTransport* aOwner )
    {
    iOwner = aOwner;
    }
    
    
// ---------------------------------------------------------------------------
// Connection manager getter
// ---------------------------------------------------------------------------
//
CCatalogsHttpConnectionManager& CCatalogsHttpSession::ConnectionManager()
    {
    DASSERT( iConnectionMgr );
    return *iConnectionMgr;
    }


// ---------------------------------------------------------------------------
// Session options setter
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::SetOptions( TUint32 aOptions )
    {
    DLTRACEIN(("aOptions: %d", aOptions));
    iDownloadMgr->DefaultConfig().SetOptions( aOptions );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpSession::CCatalogsHttpSession( TInt32 aSessionId, 
    CCatalogsTransport& aOwner, 
    MCatalogsHttpSessionManager& aSessionManager ) : 
    iOwner( &aOwner ), 
    iSessionManager( aSessionManager ),
    iSessionId( aSessionId ),
    iRefCount( 1 )
    {
    }
    
    
// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSession::ConstructL( MCatalogsHttpSessionManager& aManager,
    TBool aCleanupSession )
    {
    DLTRACEIN(("this=%X",this));    
    iConnectionMgr = CCatalogsHttpConnectionManager::NewL( *this );
    
    iTransactionMgr = CCatalogsHttpTransactionManager::NewL( 
        aManager, 
        *this, 
        *iConnectionMgr, 
        iSessionId );
    DLINFO(("transaction mgr ok"));
    
    iDownloadMgr = CCatalogsHttpDownloadManager::NewL( 
        aManager, 
        *this,
        iSessionId,
        *iTransactionMgr, 
        *iConnectionMgr, 
        aCleanupSession );
    
    // This uid must be the client side process SID. We assume that
    // the session id is the same as client side SID.
    TUid uid = TUid::Uid( iSessionId );
    iConnectionEventSink = CCatalogsConnectionEventSink::NewL( uid );
    DLTRACEOUT((""));
    }


void CCatalogsHttpSession::ReportConnectionStatus( TBool aActive )
    {
    DLTRACEIN((""));
    iConnectionEventSink->ReportConnectionStatus( aActive );
    }
