/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdInstallOperationProxy
*
*/


#include <s32strm.h>
#include "ncdinstalloperationproxy.h"
#include "ncdinstalloperationobserver.h"
#include "catalogsdebug.h"
#include "catalogsclientserver.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdinstallationservice.h"
#include "ncddeviceinteractionfactory.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncdfileinfo.h"
#include "ncdinstallinfo.h"
#include "ncdpanics.h"
#include "ncdoperationproxyremovehandler.h"
#include "ncderrors.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdInstallOperationProxy* CNcdInstallOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdInstallOperationObserver* aObserver,
    MNcdInstallationService& aInstallationService )
    {
    CNcdInstallOperationProxy* self =
        new( ELeave ) CNcdInstallOperationProxy( aInstallationService );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( aSession, aHandle, aRemoveHandler, aNode, aNodeManager,
        aObserver );
    return self;
    }


// ---------------------------------------------------------------------------
// From MNcdInstallOperation
// 
// ---------------------------------------------------------------------------
//
TInt CNcdInstallOperationProxy::FileCount()
    {
    return iFileCount;
    }


// ---------------------------------------------------------------------------
// From MNcdInstallOperation
// 
// ---------------------------------------------------------------------------
//
TInt CNcdInstallOperationProxy::CurrentFile()
    {
    return iCurrentFile;
    }


// ---------------------------------------------------------------------------
// From MNcdOperation
// Starts the operation
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::StartOperationL()
    {
    DLTRACEIN((""));
    if ( OperationStateL() != MNcdOperation::EStateStopped )
        {
        DLINFO(("Already running"));
        return;
        }
    else if ( !iFileCount ) 
        {
        DLERROR(( "No files to install"));
        User::Leave( KErrNotFound );
        }
    
    // Inform the install report that the install operation has
    // been started.
    StartInstallReportL();

    SetInstallationErrorCode( KErrNone );

    SetState( MNcdOperation::EStateRunning );        
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* ptr = &iStatus;
    User::RequestComplete( ptr, KErrNone );
    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// From MNcdOperation
// Cancels the operation
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::CancelOperation()
        {
        DLTRACEIN((""));

        // Set the installation error value as KErrCancel here. The child
        // classes may change this in their DoCancel function. So, the correct
        // value is available after Cancel is called below.
        SetInstallationErrorCode( KErrCancel );
        
        Cancel();
        iInstaller = NULL;

        // Use the installation error code here. It may be set by the child class
        // if installation result is checked. In some cases, the installation may 
        // have been completed even if the cancel was called.
        TRAP_IGNORE( CompleteInstallReportL( InstallationErrorCode() ) );
        
        if ( iObserver != NULL )
            {
            // According to the MNcdOperation interface
            // OperationComplete() will be called for each observer with KErrCancel
            // as the error parameter when CancelOperation is called. So, inform observer.
            iObserver->OperationComplete( *this, KErrCancel );            
            }

        DLTRACEOUT((""));        
        }


// ---------------------------------------------------------------------------
// From MNcdOperation
// Operation type getter
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdInstallOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(MNcdInstallOperation::KInterfaceUid);
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstallOperationProxy::CNcdInstallOperationProxy( 
    MNcdInstallationService& aInstallationService )
    : CNcdOperation< MNcdInstallOperation >( NULL ),
      iInstaller( &aInstallationService ), iFileCount( 1 ), iCurrentFile( 0 )
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstallOperationProxy::~CNcdInstallOperationProxy()
    {
    DLTRACEIN(( "" ));
    Cancel();     
    iInstaller = NULL;    
    iMimeTypes.ResetAndDestroy();
    iPurposes.Reset();
    
    delete iCurrentInfo;
    
    iFileHandle.Close();
    if ( iRemoveHandler )
        iRemoveHandler->RemoveOperationProxy( *this );
    DLTRACEOUT(( "" ));    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdInstallOperationObserver* aObserver )
    {
    DLTRACEIN(( "" ));
    CNcdBaseOperationProxy::ConstructL( aSession, aHandle, aRemoveHandler,
        aNode, aNodeManager );
    
    // Copy data id
    iObserver = aObserver;
            
    // Initialize the operation
    // Notice, that this will create also the install report in the server side.
    InitializeOperationL();
    DLTRACEOUT(( "" ));    
    }
    

    
// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    DASSERT( 0 );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::QueryReceivedCallback( CNcdQuery* /* aQuery */ )
    {
    DLTRACEIN( ( "" ) );
    DASSERT( 0 );
    }
    
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );
    (void) aError; // suppress warning
        
    DASSERT( 0 );

    // NOTICE:
    // If this function will be called in some future versions, then the
    // code below may be required.
    // Because the operation is completed, insert the completion time
    // and the error code into the purchase history.
    /*
    DLINFO(("Update purchase history"));
    TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( aError ) );
    TRAP_IGNORE( NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );
    */
    }
    

// ---------------------------------------------------------------------------
// Create initialization data sent to the server-side
// ---------------------------------------------------------------------------
//
HBufC8* CNcdInstallOperationProxy::CreateInitializationBufferL()
    {
    DLTRACEIN((""));
    // The default implementation: return an empty buffer to be sent
    // if there's no input value to send in the message
    return HBufC8::NewL( 0 );
    }


// ---------------------------------------------------------------------------
// Handle initialization callback
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::InitializationCallback( 
    RReadStream& aReadStream, TInt /* aDataLength */ )
    {
    DLTRACEIN( ( "" ) );
    TRAPD( ignore, 
        {
        // Read file count from the stream
        iFileCount = aReadStream.ReadInt32L();
        
        DLTRACE(("done"));
        });
    DLTRACEOUT( ( "Ignored error: %d", ignore ) );
    ignore = ignore; // to suppress compiler warning
    }



// ---------------------------------------------------------------------------
// Handles callbacks from the installation service
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::InstallationCompleteL( const TDesC& aFileName,
                                                       const TUid& aAppUid,
                                                       TInt aError )
    {
    DLTRACEIN(( _L("File: %S, uid: %X, error: %d"),
                &aFileName, aAppUid.iUid, aError ));

    // Because the operation is completed. Insert the completion time
    // and the error code into the purchase history.
    DLINFO(("Update purchase history"));
    iFileHandle.Close();
    
    // Ignore possible errors here because this just updates the purchase history info
    // and there is not anything that we can do later if this fails. If this fails,
    // then some information in the purchase history will not be entirely up to date.
    TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( aError ) );
    
    SetInstallationErrorCode( aError );
    if ( aError != KErrNone && 
         aError != KNcdThemeReinstalled &&
         aError != KNcdThemePossiblyReinstalled ) 
        {
        DLERROR(( "Installation failed with: %d", aError ));
        // Complete the request
        TRequestStatus* statusPtr = &iStatus;
        User::RequestComplete( statusPtr, aError );
        return;        
        }        
        
    // Update file's info to the server.
    // If something went wrong here, 
    // then inform the observer with that error code in the end of this function.
    TRAPD( trapError, UpdateInfoToServerL( aFileName, aAppUid, aError ) );
    DLINFO(("UpdateInfoToServerL errorCode: %d", trapError));
    
    DLINFO(("Internalizing related node"));
    // Get the possible error from here because the internalization may have gone wrong
    // for some nodes.
    TRAPD( trapErrorNodes, NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );
    DLINFO(("Node internalized; %d", trapErrorNodes));

    // Check what error code should be returned.
    // If the update info to server failed, then return that error code.
    // If update info to server was successfull, then use the node internalization
    // error code.
    TInt retError( trapError );
    if ( retError == KErrNone )
        {
        retError = trapErrorNodes;
        }
    DLINFO(("retError: %d", retError));
        
    // Complete the request
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete( statusPtr, retError );

    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::RunL()
    {
    DLTRACEIN(("current file: %d, file count: %d", iCurrentFile, iFileCount ));
    if ( iCurrentFile != iFileCount && iStatus.Int() == KErrNone )
        {
        TNcdProgress progress( iCurrentFile, 
                               iFileCount );

        DLTRACE(("Calling observer for progress"));            
        iObserver->InstallProgress( *this, progress );
        
        DLINFO(("Still %d more file(s) to install", iFileCount - iCurrentFile ));
        DASSERT( iInstaller );
        
        // Start installing the next file
        TRAPD( err, InstallNextFileL() );        
        
        // Handle case where everything has been installed but some
        // files were not reinstalled, eg. dependencies that were not
        // downloaded
        if ( err == KNcdErrorNothingToInstall ) 
            {
            DLTRACE(("Complete operation because everything is installed"));
            CompleteInstallReportL( KErrNone );
            iObserver->OperationComplete( *this, KErrNone );
            }
        else if ( err != KErrNone )
            {
            User::Leave( err );
            }        
        }
    else 
        {
        iFileHandle.Close();
        if ( iStatus.Int() != KErrNone ) 
            {
            SetInstallationErrorCode( iStatus.Int() );
            }
        DLTRACE(("Call OperationComplete for the observer"));
        CompleteInstallReportL( InstallationErrorCode() );
        iObserver->OperationComplete( *this, InstallationErrorCode() );
        }
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//

void CNcdInstallOperationProxy::DoCancel()
    {
    DLTRACEIN((""));
    // Cannot actually cancel anything.
    // Because the operation is pending now in Cancel function, we have to set
    // iStatus here to make sure that the Cancel operation may continue and
    // it does not stuck when waiting for the request complete from the service
    // provider.
    if ( iStatus == KRequestPending ) 
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    }

    
// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdInstallOperationProxy::RunError( TInt aError )
    {
    DLTRACEIN(("error: %d", aError));
    
    // Leave has occurred in RunL.
    // If active object is set active, then complete request.
    // And return KErrNone in the end to avoid panic.

    SetInstallationErrorCode( aError );
    iObserver->OperationComplete( *this, iInstallationError );    
    return KErrNone;
    }
    

// ---------------------------------------------------------------------------
// Installs the next file
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::InstallNextFileL()
    {
    DLTRACEIN((""));
    if ( iCurrentFile >= iFileCount ) 
        {
        DLTRACE(("no more files to install"));
        User::Leave( KNcdErrorNothingToInstall );
        }

    // set observer
    iInstaller->SetObserver( *this );
        
    iCurrentInfo = InstallInfoL( iCurrentFile );

    if ( iCurrentFile >= iFileCount ) 
        {
        DLTRACE(("Everything has been installed"));
        User::Leave( KNcdErrorNothingToInstall );
        }
    
    NCD_ASSERT_ALWAYS( iCurrentInfo->FileInfoCount(), ENcdPanicNoData ); 
    
    CNcdFileInfo& file( iCurrentInfo->FileInfo( 0 ) );

    // Open the source file. Using member variable so that the file can
    // be closed after installation
    iFileHandle.Close();
    iFileHandle = OpenFileL( iCurrentFile );
    iStatus = KRequestPending;
    SetActive();

    UseInstallServiceL( file );
        
    DLTRACE(("Setting active"));
    }
    
    
// ---------------------------------------------------------------------------
// Calls the functions of the installer. Child classes may call different functions.
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::UseInstallServiceL( 
    const CNcdFileInfo& aFile )
    {
    DLTRACEIN((""));
    // Choose correct installation type
    switch( CurrentInfo().InstallType() )
        {
        case CNcdInstallInfo::ENcdInstallNormal:
            {            
            Installer().InstallL( 
                iFileHandle, 
                aFile.MimeType(), 
                aFile.Purpose() );
            break;
            }
        
        case CNcdInstallInfo::ENcdInstallJar: // flow through
        case CNcdInstallInfo::ENcdInstallJad:
            {
            Installer().InstallJavaL( 
                iFileHandle, 
                aFile.MimeType(), 
                aFile.Data() );
             
            break;
            }            
        
        
        default:
            {
            // All cases should be handled
            DASSERT( 0 );
            break;
            }
        }
    DLTRACEOUT((""));   
    }

// ---------------------------------------------------------------------------
// Gives the installer that handles the installation.
// ---------------------------------------------------------------------------
//
MNcdInstallationService& CNcdInstallOperationProxy::Installer()
    {
    DLTRACEIN((""));
    DASSERT( iInstaller != NULL );
    return *iInstaller;
    }


CNcdInstallInfo& CNcdInstallOperationProxy::CurrentInfo()
    {
    DLTRACEIN((""));
    DASSERT( iCurrentInfo != NULL );
    return *iCurrentInfo;
    }


// ---------------------------------------------------------------------------
// Gets info for a file from the server
// ---------------------------------------------------------------------------
//
CNcdInstallInfo* CNcdInstallOperationProxy::InstallInfoL( TInt /*aIndex*/ )
    {
    DLTRACEIN((""));
    InitBuffersL( 0, 0 );
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    // write file index
    stream.WriteInt32L( iCurrentFile );    
    
    CleanupStack::PopAndDestroy( &stream );
    
    HBufC8* data = NULL;

    
    // Get file info for the file
    User::LeaveIfError( ClientServerSession().SendSyncAlloc( 
        ENCDOperationFunctionGetData,
        buf->Ptr( 0 ),        
        data,
        Handle(),
        0 ) );
    
    CleanupStack::PopAndDestroy( buf );
    
    if ( !data ) 
        {
        DLERROR(("No data"));
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PushL( data );
    
    DLTRACE(("Data length: %d", data->Length() ));
    
    // Open a stream to the data
    RDesReadStream readStream( *data );
    CleanupClosePushL( readStream );
    
    TInt32 completionId = readStream.ReadInt32L();
    
    if ( completionId != ENCDOperationMessageCompletionComplete ) 
        {
        DLERROR(("Message failed! CompletionId: %d", completionId ));
        User::Leave( KErrGeneral );
        }
    
    // internalize file info from the stream
    CNcdInstallInfo* info = CNcdInstallInfo::NewLC( readStream );
    
    // update current file index in case some files were skipped on 
    // server-side
    iCurrentFile = info->Index();
        
    CleanupStack::Pop( info );
    CleanupStack::PopAndDestroy( 2, data ); // readStream, data
        
    DLTRACEOUT((""));
    return info;
    }


// ---------------------------------------------------------------------------
// Sets info for a file in the server
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::UpdateInfoToServerL( const TDesC& aFileName,
                                                     const TUid& aAppUid,
                                                     TInt aError )
    {
    DLTRACEIN((""));

    // Update file's info to the server
    
    InitBuffersL( 0, 0 );
        
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // write file index
    stream.WriteInt32L( iCurrentFile );    
    
    // get the purpose info from the original file    
    TNcdItemPurpose oldPurpose = iCurrentInfo->FileInfo( 0 ).Purpose();    
    
    // delete current info
    delete iCurrentInfo;
    iCurrentInfo = NULL;

    TUid uid( aAppUid );
    if ( uid.iUid == KNcdThemeSisUid ) 
        {
        // ensure that purpose for themes is theme
        oldPurpose = ENcdItemPurposeTheme;
        
        // nullify appuid so it won't be added as an application
        uid = TUid::Null();
        }
    
    // Update purpose if we didn't get it from ContentInfo
    if ( oldPurpose == ENcdItemPurposeUnknown && uid != TUid::Null() ) 
        {
        oldPurpose = ENcdItemPurposeApplication;
        }
        
    // Write updated file info
    CNcdFileInfo* info = NULL;
    
    if ( aError == KNcdThemeReinstalled || 
         aError == KNcdThemePossiblyReinstalled ) 
        {
        info = 
            CNcdFileInfo::NewLC( aFileName, 
                                 KNullDesC,
                                 ENcdItemPurposeTheme );

        }
    else 
        {
        info = 
            CNcdFileInfo::NewLC( aFileName, 
                                 KNullDesC,
                                 oldPurpose );
        }
    
    info->ExternalizeL( stream );
    CleanupStack::PopAndDestroy( info );
            
    // write app uid
    if ( uid != TUid::Null() ) 
        {
        DLTRACE(( _L("Writing UID: %S"), &uid.Name() ));
        stream.WriteInt32L( 1 );
        stream.WriteInt32L( uid.iUid );
        }
    else
        {        
        stream.WriteInt32L( 0 ); // 0 uids 
        }    
        
    CleanupStack::PopAndDestroy( &stream );
    
    HBufC8* data = NULL;

    TInt err = ClientServerSession().SendSyncAlloc(
        ENCDOperationFunctionSetData,
        buf->Ptr( 0 ),        
        data, 
        Handle(),
        0 );
    
    // We could check the completion code but why bother...
    delete data;
    data = NULL;
    
    DLTRACE(( "err: %d", err));
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( buf );    
    
    // Ensure that file is closed before trying to delete
    iFileHandle.Close();
    
    DLTRACE(("Installation successful, delete original file"));
    DeleteCurrentFileL();
    
    // handle the next file
    iCurrentFile++;
    }

    
void CNcdInstallOperationProxy::SetInstallationErrorCode( const TInt& aErrorCode )
    {
    DLTRACEIN((""));
    iInstallationError = aErrorCode;
    }


TInt CNcdInstallOperationProxy::InstallationErrorCode() const
    {
    DLTRACEIN((""));
    return iInstallationError;
    }


void CNcdInstallOperationProxy::StartInstallReportL()
    {
    DLTRACEIN((""));
    
    // Inform the install report that the install operation has
    // been started. The install report is handled in the server side.
    // So, send the information to the server side operation that will
    // handle the reports.
    
    TInt tmp( 0 );
    TInt error( 
        ClientServerSession().
            SendSync( ENCDOperationFunctionReportStart,
                      KNullDesC,
                      tmp,
                      Handle() ) );  
    User::LeaveIfError( error );
    }


void CNcdInstallOperationProxy::CompleteInstallReportL( TInt aErrorCode )
    {
    DLTRACEIN((""));
    
    // Inform the install report that the install operation has
    // been completed. The install report is handled in the server side.
    // So, send the information to the server side operation that will
    // handle the reports.
    
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    RWriteStream& stream( writer() );

    stream.WriteInt32L( aErrorCode );

    TInt tmp( 0 );
    TInt error =
    ClientServerSession().
        SendSync( ENCDOperationFunctionReportComplete,
                  writer.PtrL(),
                  tmp,
                  Handle() );
    User::LeaveIfError( error ); 

    CleanupStack::PopAndDestroy( &writer );
    }


// ---------------------------------------------------------------------------
// Deletes the current content file
// ---------------------------------------------------------------------------
//
void CNcdInstallOperationProxy::DeleteCurrentFileL()
    {
    TRAPD( err, DeleteFileL( iCurrentFile ) );
    LeaveIfNotErrorL( err, KErrNotFound );        
    }


// ---------------------------------------------------------------------------
// File opener
// ---------------------------------------------------------------------------
//
RFile CNcdInstallOperationProxy::OpenFileL( TInt aFileIndex )
    {
    DLTRACEIN((""));

    RCatalogsBufferWriter buffer;
    buffer.OpenLC();
    
    buffer().WriteInt32L( aFileIndex );
    
    RFile fileHandle = ClientServerSession().SendSyncFileOpenL( 
        ENcdOperationFunctionOpenFile,
        buffer.PtrL(),        
        Handle() );
        
    CleanupStack::PopAndDestroy( &buffer );
        
    DLTRACEOUT((""));
    return fileHandle;
    }


// ---------------------------------------------------------------------------
// File deleter
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperationProxy::DeleteFileL( TInt aFileIndex )
    {
    DLTRACEIN((""));

    RCatalogsBufferWriter buffer;
    buffer.OpenLC();
    
    buffer().WriteInt32L( aFileIndex );
    
    TInt output = 0;
    User::LeaveIfError( ClientServerSession().SendSync( 
        ENcdOperationFunctionDeleteFile,
        buffer.PtrL(),
        output,
        Handle() ) );
        
    CleanupStack::PopAndDestroy( &buffer );
        
    DLTRACEOUT((""));
    }

