/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ncddescriptordownloadsuboperation.h"
#include "catalogsbasemessage.h"
#include "catalogshttpsession.h"
#include "catalogshttpconfig.h"
#include "catalogshttpoperation.h"
#include "catalogsdebug.h"
#include "ncdhttpheaders.h"
#include "catalogsutils.h"
#include "catalogshttpheaders.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncdgeneralmanager.h"


const TInt KBodyExpandSize = 4096;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdDescriptorDownloadSubOperation* CNcdDescriptorDownloadSubOperation::NewL( 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    const TDesC& aUri,
    MNcdOperationObserver& aObserver,
    MCatalogsSession& aSession )
    {
    CNcdDescriptorDownloadSubOperation* self = new( ELeave ) 
        CNcdDescriptorDownloadSubOperation( 
            aGeneralManager, aHttpSession, aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aUri, aObserver );

    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdDescriptorDownloadSubOperation::~CNcdDescriptorDownloadSubOperation()
    {
    DLTRACEIN( ( "" ) );
    if ( iDownload ) 
        {
        iDownload->Release();
        iDownload = NULL;
        }
    
    delete iBody;
    iBody = NULL;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Download config getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CNcdDescriptorDownloadSubOperation::Config()
    {
    DASSERT( iDownload );
    return iDownload->Config();
    }


// ---------------------------------------------------------------------------
// Download request header getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpHeaders& CNcdDescriptorDownloadSubOperation::RequestHeaders()
    {
    DASSERT( iDownload );
    return iDownload->Config().RequestHeaders();    
    }

// ---------------------------------------------------------------------------
// Download response header getter
// ---------------------------------------------------------------------------
//
const MCatalogsHttpHeaders& CNcdDescriptorDownloadSubOperation::ResponseHeadersL()
    {
    DASSERT( iDownload );
    return iDownload->ResponseHeadersL();
    }


// ---------------------------------------------------------------------------
// HttpOperation
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation& 
    CNcdDescriptorDownloadSubOperation::HttpOperation()
    {
    DASSERT( iDownload );
    return *iDownload;
    }

// ---------------------------------------------------------------------------
// Body
// ---------------------------------------------------------------------------
//
const TDesC8& CNcdDescriptorDownloadSubOperation::Body() const
    {
    if ( iBody && iBody->Size() ) 
        {
        iBodyPtr.Set( iBody->Ptr( 0 ) );
        return iBodyPtr;
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// Start
// ---------------------------------------------------------------------------
//
TInt CNcdDescriptorDownloadSubOperation::Start()
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
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdDescriptorDownloadSubOperation::Cancel() 
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
// HandleHttpEvent
// ---------------------------------------------------------------------------
//
void CNcdDescriptorDownloadSubOperation::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN( ( "" ) );
    (void) aOperation; // suppresses compiler warning
    DASSERT( &aOperation == iDownload );
    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );

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
            switch ( aEvent.iProgressState ) 
                {
                
                case ECatalogsHttpResponseBodyReceived:
                    {                
                    DLTRACE(("Response body received"));
                    TRAP( iError, 
                        {                        
                        // Append the body
                        iBody->InsertL( iBody->Size(), iDownload->Body() );
                        });
                    
                    if ( iError != KErrNone ) 
                        {
                        DLERROR(("Error when appending body: %d", iError ));
                        iDownload->Release();
                        iDownload = NULL;
                        iOperationState = EStateComplete;
                        iDownloadState = ENcdDownloadFailed;
                        NotifyObserversComplete( iError );
                        }
                    else 
                        {                    
                        iOperationState = EStateRunning;
                        DLTRACE(("Inform observer about progress"));
                        NotifyObserversProgress();
                        }
                    break;
                    }
                
                case ECatalogsHttpStarted:
                    {
                    DLTRACE(("Operation started"));
                    iOperationState = EStateRunning;
                    iProgress.iState = ENcdDownloadStarted;
                    DLTRACE(("Inform observer about progress"));
                    NotifyObserversProgress();
                    
                    break;
                    }
                
                default:
                    break;
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
TBool CNcdDescriptorDownloadSubOperation::HandleHttpError(
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpError aError )
    {    
    DLTRACEIN( ( "Error type: %i, id: %i", aError.iType, aError.iError ) );
    DASSERT( &aOperation == iDownload );
    
    aOperation.Release();
    iDownload = NULL;
    iError = aError.iError;
    iDownloadState = ENcdDownloadFailed;
    iOperationState = EStateComplete;   

    // notify observer
    NotifyObserversComplete( iError );
    return ETrue;
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdDescriptorDownloadSubOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    return KErrNone;
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdDescriptorDownloadSubOperation::CNcdDescriptorDownloadSubOperation(
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession ) :
        CNcdBaseOperation( aGeneralManager, NULL, EDescriptorDownloadSubOperation, aSession ),
        iHttpSession( aHttpSession ),
        iBodyPtr( NULL, 0 )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDescriptorDownloadSubOperation::ConstructL( const TDesC& aUri, 
    MNcdOperationObserver& aObserver )
    {
    DLTRACEIN(( _L("URI: %S"), &aUri ));
    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();    

    
    AddObserverL( &aObserver );

    iBody = CBufFlat::NewL( KBodyExpandSize );
    
    // Create the transaction
    iDownload = iHttpSession.CreateTransactionL( aUri, this );
    
 
    DLTRACEOUT(( "" ));
    }
    
    
// ---------------------------------------------------------------------------
// NotifyObserversComplete
// ---------------------------------------------------------------------------
//
void CNcdDescriptorDownloadSubOperation::NotifyObserversComplete( TInt aError )
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
void CNcdDescriptorDownloadSubOperation::NotifyObserversProgress()
    {
    Open(); // increase ref count in CCatalogsCommunicable
    for( TInt i = 0; i < iObservers.Count(); ++i ) 
        {
        iObservers[i]->Progress( *this );
        }
    Close();
    }
