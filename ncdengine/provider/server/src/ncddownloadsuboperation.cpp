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


#include <s32mem.h>
#include <f32file.h>

#include "ncddownloadsuboperation.h"
#include "catalogsbasemessage.h"
#include "catalogshttpsession.h"
#include "catalogshttpconfig.h"
#include "catalogshttpoperation.h"
#include "ncdhttpheaders.h"
#include "catalogsutils.h"
#include "catalogshttpheaders.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdDownloadSubOperation* CNcdDownloadSubOperation::NewL( 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    const TDesC& aUri,
    const TDesC& aDestination,
    MNcdOperationObserver& aObserver,
    MCatalogsSession& aSession )
    {
    CNcdDownloadSubOperation* self = new( ELeave ) CNcdDownloadSubOperation( 
        aGeneralManager, 
        aHttpSession, 
        aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aUri, aDestination, aObserver );

    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdDownloadSubOperation* CNcdDownloadSubOperation::NewL( 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationObserver& aObserver,
    RReadStream& aStream,
    MCatalogsSession& aSession )
    {
    CNcdDownloadSubOperation* self = new( ELeave ) CNcdDownloadSubOperation( 
        aGeneralManager,
        aHttpSession, 
        aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aObserver );
    self->InternalizeL( aStream );

    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdDownloadSubOperation::~CNcdDownloadSubOperation()
    {
    DLTRACEIN( ( "" ) );
    if ( iDownload ) 
        {
        iDownload->Release();
        iDownload = NULL;
        }
        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Download config getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CNcdDownloadSubOperation::Config()
    {
    DASSERT( iDownload );
    return iDownload->Config();
    }


// ---------------------------------------------------------------------------
// Download request header getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpHeaders& CNcdDownloadSubOperation::RequestHeaders()
    {
    DASSERT( iDownload );
    return iDownload->Config().RequestHeaders();    
    }

// ---------------------------------------------------------------------------
// Download response header getter
// ---------------------------------------------------------------------------
//
const MCatalogsHttpHeaders& CNcdDownloadSubOperation::ResponseHeadersL()
    {
    DASSERT( iDownload );
    return iDownload->ResponseHeadersL();
    }


// ---------------------------------------------------------------------------
// HttpOperation
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation& CNcdDownloadSubOperation::HttpOperation()
    {
    DASSERT( iDownload );
    return *iDownload;
    }


// ---------------------------------------------------------------------------
// Start
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadSubOperation::Start()
    {
    DLTRACEIN((""));
    TInt ret = iDownload->Start();
        
    if ( ret >= KErrNone ) 
        {
        DLTRACEOUT((""));
        return KErrNone;
        }
    DLTRACEOUT((""));
    return ret;
    }


// ---------------------------------------------------------------------------
// Pause
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadSubOperation::Pause()
    {
    DLTRACEIN((""));
    TInt err = iDownload->Pause();              
    if ( err == KErrNone ) 
        {
        iDownloadState = ENcdDownloadPaused;
        }
    
    DLTRACEOUT((""));
    return err;
    }
    
    
// ---------------------------------------------------------------------------
// Resume
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadSubOperation::Resume()
    {
    DLTRACEIN((""));
    TInt err = KErrNone; 
    
    if ( iDownload->State() == ECatalogsHttpOpPaused ) 
        {
        err = iDownload->Start();                
        if ( err == KErrNone ) 
            {
            iDownloadState = ENcdDownloadInProgress;
            }
    
        }    
    DLTRACEOUT((""));
    return err;
    }


// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    if ( iDownload ) 
        {
        // Cancel also releases the dl
        iDownload->Cancel();
        iDownload = NULL;
        }
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// HandleHttpEventL
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((( "Event op state: %d, progress state: %d" ), aEvent.iOperationState, aEvent.iProgressState ));
    (void) aOperation; // suppresses compiler warning
    DASSERT( &aOperation == iDownload );
    DASSERT( aOperation.OperationType() == ECatalogsHttpDownload );

    TCatalogsTransportProgress progress( iDownload->Progress() );
    
    iProgress = TNcdSendableProgress( iDownloadState,
        iDownload->OperationId().Id(), progress.iProgress,
        progress.iMaxProgress );

    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            DLTRACE(("ECatalogsHttpOpCompleted"));
            iDownloadState = ENcdDownloadComplete;
            NotifyObserversComplete( KErrNone );            
            break;
            }
        
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            iDownloadState = ENcdDownloadInProgress;
            switch( aEvent.iProgressState ) 
                {
                case ECatalogsHttpResponseBodyReceived:
                    {            
                    iOperationState = EStateRunning;
                    DLTRACE(("Inform observer about progress"));
                    NotifyObserversProgress();
                    break;
                    }
                case ECatalogsHttpDisconnected: 
                    {
                    DLTRACE(("ECatalogsHttpDisconnected"));
                    
                    iError = KErrDisconnected;
                    iDownloadState = ENcdDownloadFailed;
                    iOperationState = EStateComplete;   
                    NotifyObserversComplete( iError );
                    break;
                    }
                
                case ECatalogsHttpStarted:
                    {
                    DLTRACE(("ECatatalogsHttpStarted"));
                    iOperationState = EStateRunning;
                    iProgress.iState = ENcdDownloadStarted;
                    NotifyObserversProgress();
                    break;
                    }
                    
                default:
                    {
                    break;
                    }
                }
            break;
            }
                
        default:
            {
            break;
            }
        }
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// HandleHttpError
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadSubOperation::HandleHttpError(
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpError aError )
    {    
    DLTRACEIN( ( "Error type: %i, id: %i", aError.iType, aError.iError ) );
    DASSERT( &aOperation == iDownload );
    
    if ( aError.iError == KErrCancel ) 
        {
        aOperation.Cancel();
        }
    else
        {        
        aOperation.Release();
        }
    iDownload = NULL;
    iError = aError.iError;
    iDownloadState = ENcdDownloadFailed;
    iOperationState = EStateComplete;   

    // notify observer
    NotifyObserversComplete( iError );
    return ETrue;
    }



// ---------------------------------------------------------------------------
// Externalize
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iDownloadState );
    if ( iDownload && 
         // don't save the download if there's no platform download 
         // because we can't restore it from download manager anyway
         iDownload->OperationId().SecondaryId() != KErrNotFound )
        {
        // Download exists
        aStream.WriteInt8L( 1 );

        // Write operation id so that it can be retrieved from Transport
        iDownload->OperationId().ExternalizeL( aStream );
        
        iDownload->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt8L( 0 );
        }
            
    }


// ---------------------------------------------------------------------------
// Internalize
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    iDownloadState = static_cast<TNcdDownloadState>( aStream.ReadInt32L() );
    if ( iDownloadState == ENcdDownloadInProgress ) 
        {
        DLTRACE(("State was set to in progress, setting as paused"));
        iDownloadState = ENcdDownloadPaused;
        }
        
    TInt8 downloadExists = aStream.ReadInt8L();
    if ( downloadExists )
        {        
        TCatalogsTransportOperationId opId;
        opId.InternalizeL( aStream );
        
        // Search through restored downloads
        const RCatalogsHttpOperationArray& downloads = 
            iHttpSession.RestoredDownloads();
            
        for ( TInt i = 0; i < downloads.Count(); ++i )
            {
            TCatalogsTransportOperationId compId( downloads[i]->OperationId() );
            if ( compId.SecondaryId() == opId.SecondaryId() && 
                 compId.SessionId() == opId.SessionId() ) 
                {
                DLINFO(("Found a matching download"));
                iDownload = downloads[i];
                iDownload->InternalizeL( aStream );
                iDownload->Config().SetObserver( this );                
                                
                // Move dl from restored to current dls so that the restored
                // dl list can be purged from unnecessary dls afterwards
                User::LeaveIfError( iHttpSession.MoveRestoredDlToCurrentDls( 
                    *iDownload ) );
                return;
                }
            }                
        }        
    DLERROR(("Download was not found!"));
    // Download was not found though it should have existed
    User::Leave( KErrNotFound );
    
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadSubOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    return KErrNone;
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdDownloadSubOperation::CNcdDownloadSubOperation(
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession ) :
        CNcdBaseOperation( aGeneralManager, NULL, EDownloadSubOperation, aSession ),
        iHttpSession( aHttpSession )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::ConstructL( const TDesC& aUri, 
    const TDesC& aDestination, MNcdOperationObserver& aObserver )
    {
    DLTRACEIN(( _L("URI: %S, Destination: %S"), &aUri, &aDestination ));
    
    AddObserverL( &aObserver );
    
    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();    
    
    // Create the download
    iDownload = iHttpSession.CreateDownloadL( aUri, EFalse, this );
    
    TParsePtrC parse( aDestination );
        
    iDownload->Config().SetDirectoryL( parse.DriveAndPath() );
    
    TPtrC filename( parse.NameAndExt() );
    if ( filename.Length() ) 
        {
        DLTRACE(( _L("Destination filename: %S"), &filename ));
        iDownload->Config().SetFilenameL( filename );
        }
 
    iProgress.iProgress = -1;
    iProgress.iMaxProgress = -1;
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::ConstructL( MNcdOperationObserver& aObserver )
    {
    DLTRACEIN(( "" ));
    
    AddObserverL( &aObserver );
    
    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();    
     
    iProgress.iProgress = -1;
    iProgress.iMaxProgress = -1;
    DLTRACEOUT(( "" ));
    }
    
    
// ---------------------------------------------------------------------------
// NotifyObserversComplete
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::NotifyObserversComplete( TInt aError )
    {
    Open(); // increase ref count in CCatalogsCommunicable
    for( TInt i = 0; i < iObservers.Count(); ++i ) 
        {
        iObservers[i]->OperationComplete( this, aError );
        }
    Close();
    }


// ---------------------------------------------------------------------------
// NotifyObserversProgress
// ---------------------------------------------------------------------------
//
void CNcdDownloadSubOperation::NotifyObserversProgress()
    {
    Open(); // increase ref count in CCatalogsCommunicable
    for( TInt i = 0; i < iObservers.Count(); ++i ) 
        {
        iObservers[i]->Progress( *this );
        }
    Close();
    }
