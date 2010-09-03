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
* Description:  
*
*/


#include "catalogshttpdownloadmanagerimpl.h"

#include "catalogsconstants.h"
#include "catalogshttpoperation.h"
#include "catalogsutils.h"
#include "catalogshttpdownload.h"
#include "catalogshttpconfigimpl.h"
#include "catalogshttpsessionmanager.h"
#include "catalogshttptransactionmanager.h"
#include "catalogshttpconnectionmanager.h"
#include "catalogshttpsessionmanagerimpl.h"
#include "catalogsnetworkmanager.h"
#include "catalogshttpsessionimpl.h"
#include "catalogsconnection.h"

#include "catalogsdebug.h"
#include <QString>
#include <QVector>
#include <QList>
#include <downloadevent.h>
#include <xqconversions.h>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsHttpDownloadManager* CCatalogsHttpDownloadManager::NewL( 
    MCatalogsHttpSessionManager& aManager, 
    CCatalogsHttpSession& aSession,
    TInt32 aSessionId,
    MCatalogsHttpTransactionManager& aTransactionManager,
    CCatalogsHttpConnectionManager& aConnectionManager,
    TBool aCleanup )
    {
    DLTRACEIN(("aMan=%X aTrans=%X",&aManager,&aTransactionManager));
    
    CCatalogsHttpDownloadManager* self = new ( ELeave ) 
            CCatalogsHttpDownloadManager( 
                aManager, 
                aSession,
                aSessionId, 
                aTransactionManager, 
                aConnectionManager );        
            
    DLTRACE(("constructor ok"));
    CleanupStack::PushL( self );
   
    self->ConstructL( aCleanup );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpDownloadManager::~CCatalogsHttpDownloadManager()
    {
    DLTRACEIN( ( "" ) );

    Cancel();
    // HLa iEventQueue.Close();

    // Release downloads
    TInt count = iDownloads.Count() - 1;
    while( count > -1 )
        {
        while( iDownloads[count]->Release() )
            {
            }
        --count;
        }

    count = iRestoredDownloads.Count() - 1;
    while( count > -1 )
        {
        while( iRestoredDownloads[count]->Release() )
            {
            }
        --count;
        }

    iDownloads.Close();    
    iRestoredDownloads.Close();

    if ( iNetworkManager ) 
        {
        iNetworkManager->RemoveObserver( *this );
        }


    if ( iDmgr ) 
        {
        DLTRACE(("We were connected to DL manager"));
        DeleteHangingDownloads();
        }
      
    // Close download manager session    
    iDmgr->removeAll();
    
   
    delete iDefaultConfig;
    iManager.Release();
    
    iFs.Close();

    DLTRACEOUT( ( "" ) );
    }


void CCatalogsHttpDownloadManager::DeleteHangingDownloads() 
    {
    DLTRACEIN((""));
    // This pauses all downloads in addition to disconnecting
 	iDmgr->pauseAll(); 
   TInt count = iDownloads.Count();
    for ( TInt i = 0; i < count; ++i )
        {
         TInt32 deleteStatus = 0;
         deleteStatus = dynamic_cast<CCatalogsHttpDownload*>(iDownloads[i])->GetStatusState();
       
         if ( deleteStatus == CCatalogsHttpDownload::EDownloadCanBeDeleted ) 
            {
            DLTRACE(("Deleting download"));
            iDmgr->removeOne(dynamic_cast<CCatalogsHttpDownload*>(iDownloads[i])->GetDownload());
            }
        }        
        
    }

// ---------------------------------------------------------------------------
// Download creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpDownloadManager::CreateDownloadL( 
    const TDesC16& aUrl, TBool aStart, MCatalogsHttpObserver* aObserver )
    {
    DLTRACEIN((""));
    HBufC8* buf = ConvertUnicodeToUtf8L( aUrl );
    CleanupStack::PushL( buf );
    DLTRACE(("Creating download"));
    MCatalogsHttpOperation* operation = CreateDownloadL( *buf, 
        aStart, aObserver );
    CleanupStack::PopAndDestroy( buf );    
    DLTRACEOUT((""));
    return operation;
    }
    

// ---------------------------------------------------------------------------
// Download creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpDownloadManager::CreateDownloadL( 
    const TDesC8& aUrl, TBool /* aStart */, MCatalogsHttpObserver* aObserver )
    {
    DLTRACEIN((""));
    // Create a new download
    QString Url = XQConversions::s60Desc8ToQString(aUrl);  
    iDownload = iDmgr->createDownload( Url );
    
    CCatalogsHttpDownload* dl = CCatalogsHttpDownload::NewLC( 
        *this,
        //download,
        iDownload,
        *iDefaultConfig );
    
    dl->SetUriL( aUrl );
    dl->SetFileServerSession( iFs );
        // If the given observer != NULL, set it as the observer for the download
    if ( aObserver ) 
        {        
        dl->Config().SetObserver( aObserver );        
        }
    
    // Add the download to downloads
    iDownloads.AppendL( dl );
        
    CleanupStack::Pop( dl );
    
    DLTRACEOUT((""));
    return dl;        
    }
    

// ---------------------------------------------------------------------------
// Download getter
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CCatalogsHttpDownloadManager::Download( 
    const TCatalogsTransportOperationId& aId ) const
    {
    TInt count = iDownloads.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        	
        if ( iDownloads[i]->OperationId() == aId ) 
            {
            return iDownloads[i];
            }
        }
    return NULL;
    }
    
    
// ---------------------------------------------------------------------------
// Current downloads getter
// ---------------------------------------------------------------------------
//    
const RCatalogsHttpOperationArray& 
    CCatalogsHttpDownloadManager::CurrentDownloads() const
    {
    return iDownloads;
    }


// ---------------------------------------------------------------------------
// Restored downloads getter
// ---------------------------------------------------------------------------
//
const RCatalogsHttpOperationArray& 
    CCatalogsHttpDownloadManager::RestoredDownloads() const
    {
    return iRestoredDownloads;
    }


// ---------------------------------------------------------------------------
// Cancels all downloads
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::CancelAll()
    {
    TInt count = iDownloads.Count();
    while( count-- )
        {
        iDownloads[ count ]->Cancel();        
        }

    iDownloads.Reset();

    DeleteRestoredDownloads();
    iCancellingAll = EFalse;      
      
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::NotifyCancelAll()
    {
    DLTRACEIN((""));    
    
    TInt count = iDownloads.Count();
    
    while( count-- && iDownloads.Count() )
        {
        // NotifyCancel can lead to canceling other downloads too so we must
        // make sure that we don't go out of bounds
        if ( count >= iDownloads.Count() ) 
            {
            count = iDownloads.Count() - 1;
            }
            
        iDownloads[count]->NotifyCancel();        
        }
    }


// ---------------------------------------------------------------------------
// Deletes restored downloads
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::DeleteRestoredDownloads()
    {
    DLTRACEIN((""));
    TInt count = iRestoredDownloads.Count();
    while ( count-- )
        {
        iRestoredDownloads[count]->Cancel();        
        }
    iRestoredDownloads.Reset();             
    }
    

// ---------------------------------------------------------------------------
// Returns the default configuration for downloads
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CCatalogsHttpDownloadManager::DefaultConfig() const
    {
    return *iDefaultConfig;
    }


// ---------------------------------------------------------------------------
// Moves a restored download to current downloads
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpDownloadManager::MoveRestoredDlToCurrentDls( 
    MCatalogsHttpOperation& aDownload )
    {
    DLTRACEIN((""));
    DASSERT( aDownload.OperationType() == ECatalogsHttpDownload );
    TInt index = FindInDownloads( iRestoredDownloads, &aDownload );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(( "Removing from restored downloads" ));
        iRestoredDownloads.Remove( index );
        index = iDownloads.Append( &aDownload );
        
        // Update latest download id counter
        TInt opId = aDownload.OperationId().Id();
        if ( opId > iCurrentDlId ) 
            {
            iCurrentDlId = opId;
            }

        DLTRACE(( "Append err: %i", index ));                                
        }
    return index;
    }

// ---------------------------------------------------------------------------
// Removes the download from the manager
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::RemoveDownload( 
    MCatalogsHttpOperation* aDownload )
    {    
    DLTRACEIN( ( "Total DLs before: %i", iDownloads.Count() ) );
    TInt index = KErrNotFound;
    // If cancelling all, we don't want to remove them separately 
    // from the list
    if ( !iCancellingAll ) 
        {        
        // Search from downloads
        index = FindInDownloads( iDownloads, aDownload );
        
        if ( index != KErrNotFound ) 
            {
            DLTRACE( ( "Removing from downloads" ) );
            iDownloads.Remove( index );
            }
        else 
            {
            // Search from restored
            index = FindInDownloads( iRestoredDownloads, aDownload );
                
            if ( index != KErrNotFound ) 
                {
                DLTRACE( ("Removing from restored downloads") );
                iRestoredDownloads.Remove( index );
                }
            }
        
        }
    DLTRACEOUT( ( "Total DLs after: %i", iDownloads.Count() ) );
    }


// ---------------------------------------------------------------------------
// Create download transaction
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpDownloadManager::CreateDlTransactionL( 
    const TDesC8& aUrl, MCatalogsHttpObserver& aObserver,
    const CCatalogsHttpConfig& aConfig )
    {
    return iTrManager.CreateDlTransactionL( aUrl, aObserver, aConfig );
    }


// ---------------------------------------------------------------------------
// Sets the access point for the next download
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::SetConnectionMethodL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));
    if ( aMethod != iCurrentAp )
        {
		try
		{
        DLTRACE( ( "Set AP, type: %d, id: %u, apn: %d", 
            aMethod.iType, aMethod.iId, aMethod.iApnId ));
		 iDmgr->pauseAll(); 
		 
        iCurrentAp = aMethod;
        DLTRACE(("AP set"));
		}
		 catch(const std::exception& exception)
		 {
		 qt_symbian_exception2LeaveL(exception);
		 }
        }    
    }


// ---------------------------------------------------------------------------
// Connection setter
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::SetConnectionL(
    CCatalogsConnection& aConnection )
    {
    DLTRACEIN((""));
    // No need to do any comparisons, SetConnectionMethodL ensures that
    // we don't update the APN unnecessarily to download manager
    //
    // There used to be a pointer comparison here but it didn't always work
    // correctly because the new CCatalogsConnection object was often
    // created to the same address as the old one so the APN was not
    // updated to the download manager    
    
    SetConnectionMethodL( aConnection.ConnectionMethod() );            
    }

// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::AddRef()
    {
    iRefCount++;    
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpDownloadManager::Release()
    {
    DLTRACEIN( ( "" ) );
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Returns the session id
// ---------------------------------------------------------------------------
//
TInt32 CCatalogsHttpDownloadManager::SessionId() const
    {
    return iSessionId;
    }

// ---------------------------------------------------------------------------
// Handles download manager events
// ---------------------------------------------------------------------------
//    
 void CCatalogsHttpDownloadManager::downloadMgrEventRecieved(WRT::DownloadManagerEvent* dlmEvent)
  {
  	
  	switch(dlmEvent ->type())
		{
 
    default:
    	break;
		};

  	
  }  


// ---------------------------------------------------------------------------
// Accesspoint observer
// ---------------------------------------------------------------------------
//
void CCatalogsHttpDownloadManager::HandleAccessPointEventL( 
    const TCatalogsConnectionMethod& /* aAp */,
    const TCatalogsAccessPointEvent& aEvent )
    {
    DLTRACEIN((""));
    if ( aEvent == ECatalogsAccessPointClosed ) 
        {
		try
		{
        DLTRACE(("Disconnecting download manager"));

		iDmgr->pauseAll();
	}

 		catch(const std::exception& exception) 
               { 
               qt_symbian_exception2LeaveL(exception); 
               } 
            

        }
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpDownloadManager::CCatalogsHttpDownloadManager( 
    MCatalogsHttpSessionManager& aManager, 
    CCatalogsHttpSession& aSession,
    TInt32 aSessionId,
    MCatalogsHttpTransactionManager& aTransactionManager,
    CCatalogsHttpConnectionManager& aConnectionManager ) 
    :    
#ifdef __SERIES60_31__    
    // LOL, it works
    CActive( CActive::EPriorityHigh ),
#else    
    CActive( CActive::EPriorityLow - 21 ),
    
#endif
    iManager( aManager ), 
    iSession( aSession ),
    iTrManager( aTransactionManager ),
    iConnectionManager( aConnectionManager ), 
    iSessionId( aSessionId ),
    iCancellingAll( EFalse ), 
    iRefCount( 1 )    
    {
    DLTRACEIN((""));
    iManager.AddRef();
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpDownloadManager::ConstructL( TBool aCleanup )
    {        
    DLTRACEIN((""));
    User::LeaveIfError( iFs.Connect() );
    
    // shared so that RFiles can be given to Download manager
    User::LeaveIfError( iFs.ShareProtected() ); 
   	
   	QString DmgrUid(QString::number(KNCDEngineAppID));
    iDmgr =  new DownloadManager(DmgrUid);
    iDmgr->initialize();
    iQTmgr = new CCatalogsHttpQTDownloadManager(this,iDmgr);
    
    TUid sessionId( TUid::Uid( iSessionId ) );
    if ( aCleanup ) 
        {
        DLTRACE(("Cleaning download manager before connecting"));        
        TUidName client( CleanUidName( sessionId ) );
        TPath path;        
        path.Format( KCatalogsDownloadMgrPath, &client );

        CFileMan* fileman = CFileMan::NewL( iFs );
        CleanupStack::PushL( fileman );
        DLINFO(( _L("Clearing directory: %S"), &path ));
        // Ignoring errors
        fileman->Delete( path );
        CleanupStack::PopAndDestroy( fileman );
        }
        
    // Connect with the download manager
	// If the 3rd parameter == ETrue, this session inherits downloads 
	// from other sessions that have the same UID.    
    TInt err = KErrNone;
    TInt retries = KCatalogsDlMgrConnectRetryAttempts;
    
    do    
        {
        if ( err != KErrNone ) 
            {
            DLERROR(("DL manager connection failed with err: %d, retry attempts left", 
                err, retries ));
            // This halts the whole thread which is not nice but shouldn't
            // be a problem since this should occur only when the download
            // manager is being shutdown when we try to connect to it
            User::After( KCatalogsDlMgrConnectRetryInterval ); 
            }
        }
    while ( err != KErrNone && retries-- );
    
    if( err != KErrNone )
        {
        DLINFO(("Leaving.. DL manager connection failed with: %d", err ));
        User::Leave( err );
        }

    DLTRACE(("ConnectL ok"));
        
    iDefaultConfig = CCatalogsHttpConfig::NewL();
    
    iNetworkManager = &CCatalogsHttpSessionManager::NetworkManagerL();
    iNetworkManager->AddObserverL( *this );    
    
    // Restore downloads from previous sessions
//    RestoreDownloadsL();
 
    DLTRACEOUT((""));

    }
    
DownloadManager* CCatalogsHttpDownloadManager::GetDownloadManager()
{
	return iDmgr;
}
// ---------------------------------------------------------------------------
// Restore downloads from previous session
// ---------------------------------------------------------------------------
//        
void CCatalogsHttpDownloadManager::RestoreDownloadsL()
    {
    DLTRACEIN((""));
    
    const TInt KMaxUrlLength = 2048; // HLa
    TBuf8<KMaxUrlLength> url;
    
    
    QList<WRT::Download*> downloads = iDmgr->currentDownloads();


    // Go through downloads, create wrappers for DL manager downloads and
    // add them to HttpDownloadManager
    for ( TInt i = 0; i < downloads.size(); ++i ) 
        {                 
        DLTRACE(( "Restoring dl %i", i ));
        WRT::Download *dlPtr = downloads[i];

        // Create the download and push it to cleanup stack
        CCatalogsHttpDownload* dl = CCatalogsHttpDownload::NewLC( *this,
            dlPtr, *iDefaultConfig );

        // set file server session
        dl->SetFileServerSession( iFs );
       
         QString name   =  dlPtr->attribute(FileName).toString();
         TBuf<KMaxPath> filename(name.utf16());

        // Separate the filename from the path
        TParsePtrC parser( filename );
        
        if ( parser.PathPresent() ) 
            {
            
            dl->Config().SetDirectoryL( parser.DriveAndPath() );
            DLTRACE( ( dl->Config().Directory() ) );    
            }
            
        if ( parser.NameOrExtPresent() ) 
            {
            dl->Config().SetFilenameL( parser.NameAndExt() );
            DLTRACE( ( dl->Config().Filename() ) );    
            }
            
        // Update the content type from download manager       
        dl->UpdateContentType();
        
        // Update URI from download manager
        dl->UpdateUriL();               
        
        // Append the download to the list of downloads
        iRestoredDownloads.AppendL( dl );
                
        CleanupStack::Pop( dl );
        }
    }
    

// ---------------------------------------------------------------------------
// Finds the index for the given download in iDownloads
// ---------------------------------------------------------------------------
//   
/* HLa
TInt CCatalogsHttpDownloadManager::FindInDownloads( 
    const RCatalogsHttpOperationArray& aArray,
    RHttpDownload* aDownload ) const
    {    
    DASSERT( aDownload );
    
    TInt32 id = 0;
    //aDownload->GetIntAttribute( EDlAttrId, id );    
            
    TInt count = aArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aArray[ i ]->OperationId().SecondaryId() == id ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }
*/

// ---------------------------------------------------------------------------
// Finds the index for the given download in the given array
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpDownloadManager::FindInDownloads( 
    const RCatalogsHttpOperationArray& aArray,
    MCatalogsHttpOperation* aDownload ) const
    {
    DASSERT( aDownload );
        
    return aArray.Find( aDownload );    
    }


// ---------------------------------------------------------------------------
// Decides whether the download can start or not
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpDownloadManager::StartOperation( 
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN(( "" ));
    DASSERT( aOperation );
    TInt err = iManager.StartOperation( aOperation, EFalse );
    if ( err == KErrNone || err == KCatalogsHttpOperationQueued ) 
        {
        TInt err2 = MoveRestoredDlToCurrentDls( *aOperation );
        // Check for unwanted errors
        if ( err2 != KErrNone && err2 != KErrNotFound ) 
            {
            return err2;
            }
        }
    return err;
    }


// ---------------------------------------------------------------------------
// Moves the download to the queue if it isn't there already
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpDownloadManager::PauseOperation( 
    MCatalogsHttpOperation* aOperation )
    {    
    DLTRACEIN(( "" ));    
    return iManager.PauseOperation( aOperation );
    }


// ---------------------------------------------------------------------------
// Complete the download and resume a download from the queue if any
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpDownloadManager::CompleteOperation( 
    MCatalogsHttpOperation* aOperation )
    {    
    DLTRACE(( "Total DLs: %i", iDownloads.Count() ));
    return iManager.CompleteOperation( aOperation );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//            
void CCatalogsHttpDownloadManager::ReportConnectionStatus( TBool aActive )
    {
    iSession.ReportConnectionStatus( aActive );
    }
    

// ---------------------------------------------------------------------------
// Handles priority change
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpDownloadManager::OperationPriorityChanged( 
    MCatalogsHttpOperation* aOperation )
    {
    return iManager.OperationPriorityChanged( aOperation );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//            
CCatalogsHttpConnectionCreator& CCatalogsHttpDownloadManager::ConnectionCreatorL()
    {
    return iManager.ConnectionCreatorL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//            
CCatalogsHttpConnectionManager& CCatalogsHttpDownloadManager::ConnectionManager()
    {
    return iConnectionManager;
    }

// ---------------------------------------------------------------------------
// Event handling
// ---------------------------------------------------------------------------
//            
void CCatalogsHttpDownloadManager::RunL()
    {
    DLTRACEIN(( "Event queue count: %d", iEventQueue.Count() ));
   
    // Handle the event queue 
    /* HLa
    while ( iEventQueue.Count() ) 
        {
        
        TDownloadEvent dlEvent( iEventQueue[0] );
        iEventQueue.Remove( 0 );
        THttpDownloadEvent& event( dlEvent.iEvent );

        if ( event.iDownloadState == EHttpDlDeleted )
            {
            DLTRACEOUT(("Download was deleted"));
            RemoveUnhandledEvents( dlEvent );
            continue;
            }        
        
        // Find the correct download
        TInt index = FindInDownloads( iDownloads, dlEvent.iDownload );
        if ( index == KErrNotFound ) 
            {
            DLTRACE(("Download was not found, dl-ptr: %x", dlEvent.iDownload ));            
            continue;
            }
            
        CCatalogsHttpDownload* dl = 
            static_cast<CCatalogsHttpDownload*>( iDownloads[index] );

        DLINFO(("Found the download, index: %d", index));
        DLTRACE(( "DL id: %i", iDownloads[index]->OperationId().Id() ));
        // Use the download object to handle the event

#ifdef __SERIES60_31__
        if ( dl->IsCancelled() && 
            IsOneOf( 
                event.iDownloadState,             
                EHttpDlPaused, 
                EHttpDlCompleted, 
                EHttpDlFailed ) )
            {
            RemoveUnhandledEvents( dlEvent );
            }
#endif        
        
        TRAPD( eventErr, dl->HandleEventL( event ) );
        
        if ( eventErr != KErrNone )
            {
            DLERROR(( "" ));
            MCatalogsHttpConfig& config = dl->Config();
            MCatalogsHttpObserver* observer = config.Observer();
            
            if ( observer )
                {
                DLINFO(( "Calling HandleHttpError" ));
                observer->HandleHttpError( *dl, TCatalogsHttpError( ECatalogsHttpErrorGeneral, eventErr ) );
                }
            else
                {
                DLERROR(( "Observer was NULL" ));
                User::Leave( eventErr );
                }
            }
        }
        */
    }
    
    
void CCatalogsHttpDownloadManager::DoCancel()
    {
    DLTRACEIN((""));
    // Nothing to be done
    }
    
    
TInt CCatalogsHttpDownloadManager::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    (void) aError;
    
    DLERROR(( "Trying to cancel all downloads" ));
    NotifyCancelAll();
    
    return KErrNone;
    }    
    
    
void CCatalogsHttpDownloadManager::ContinueEventHandling()
    {
    DLTRACEIN((""));
    /* HLa
    if( iEventQueue.Count() && !IsActive() ) 
        {
        DLTRACE(("More events to handle"));
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
        */
    }

    
    /* HLa
void CCatalogsHttpDownloadManager::RemoveUnhandledEvents( 
    const TDownloadEvent& aEvent )
    {
    DLTRACEIN((""));
    TInt index = iEventQueue.Count();
    while( index-- ) 
        {
        if ( iEventQueue[ index ].iDownload == aEvent.iDownload ) 
            {
            DLTRACE(("Removing unhandled event"));
            iEventQueue.Remove( index );
            }        
        }    
    }
*/

/* HLa
void CCatalogsHttpDownloadManager::RemoveUnhandledProgressEvents( 
    const TDownloadEvent& aEvent )
    {
    DLTRACEIN((""));
    TInt index = iEventQueue.Count();
    while( index-- ) 
        {
        if ( iEventQueue[ index ].iDownload == aEvent.iDownload &&
             iEventQueue[ index ].iEvent.iProgressState == EHttpProgResponseBodyReceived ) 
            {
            DLTRACE(("Removing unhandled progress event"));
            iEventQueue.Remove( index );
            break;
            }        
        }    
    }
*/

TInt CCatalogsHttpDownloadManager::NewDownloadId()
    {
    return ++iCurrentDlId;
    }


Download& CCatalogsHttpDownloadManager::CreatePlatformDownloadL( 
    const TDesC8& aUrl )
    {
    
   	QString myString=	QString::fromRawData( reinterpret_cast<const QChar*>(aUrl.Ptr()),aUrl.Length());

    return *(iDmgr->createDownload( myString ));
    }
    
CCatalogsHttpQTDownloadManager::CCatalogsHttpQTDownloadManager(CCatalogsHttpDownloadManager* aDownloadManager,DownloadManager* aDmgr)
	{
		iDownloadManager = aDownloadManager;
		iDmgr = aDmgr;
		connect(iDmgr, SIGNAL(downloadManagerEvent(DownloadManagerEvent*)), this,SLOT(downloadMgrEventRecieved(DownloadManagerEvent*)));
	}
	
void CCatalogsHttpQTDownloadManager::downloadMgrEventRecieved(DownloadManagerEvent* aEvent)
	{
		iDownloadManager->downloadMgrEventRecieved(aEvent);
	}