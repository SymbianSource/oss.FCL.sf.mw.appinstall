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


#include "ncdinstalloperationimpl.h"

#include <s32mem.h>
#include <f32file.h>
#include <bautils.h>

#include "catalogsbasemessage.h"
#include "ncdnodemanager.h"
#include "ncdnodeimpl.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncdinstallinfo.h"
#include "ncdfileinfo.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdutils.h"
#include "ncderrors.h"
#include "ncdoperationremovehandler.h"
#include "ncdnodeinstallimpl.h"
#include "ncdnodedownloadimpl.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodelink.h"
#include "ncdproviderutils.h"
#include "ncdpurchasehistoryutils.h"
#include "ncdinstallreportobserver.h"
#include "catalogsaccesspointmanager.h"
#include "catalogsconnectionmethod.h"
#include "catalogshttpconnectionmanager.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdInstallOperation* CNcdInstallOperation::NewL(
    MNcdOperationRemoveHandler& aRemoveHandler,
    const CNcdNodeIdentifier& aNodeId,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdInstallReportObserver& aReportObserver,
    MCatalogsSession& aSession )
    {
    CNcdInstallOperation* self = new( ELeave ) CNcdInstallOperation( 
        aRemoveHandler,
        aGeneralManager,
        aHttpSession,        
        aReportObserver,
        aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aNodeId );

    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstallOperation::~CNcdInstallOperation()
    {
    DLTRACEIN((""));
    
    delete iNodeId;
    
    // Deletes the JAD from disk and also deletes iJadFile
    DeleteJad();
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Node Id getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdInstallOperation::NodeIdentifier() const
    {
    return *iNodeId;
    }

// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdInstallOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    
    // Nothing to do here. Most of the install operation
    // functionality is in proxy side.
    
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// Receive message
// ---------------------------------------------------------------------------
//
void CNcdInstallOperation::ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber )
    {
    DLTRACEIN((""));

    DASSERT( aMessage );
    
    DLINFO(( "Handle: %i, aFunctionNumber=%d",
            aMessage->Handle(),
            aFunctionNumber));

    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TRAPD( err, DoReceiveMessageL( aMessage, aFunctionNumber ) ); 
    
    if ( err != KErrNone ) 
        {
        DLINFO(( ( "Error: %d" ), err ));
        CompleteMessage( iMessage, 
            ENCDOperationMessageCompletionError,
            err );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Counter part lost
// ---------------------------------------------------------------------------
//
void CNcdInstallOperation::CounterPartLost( const MCatalogsSession& aSession )
    {
    DLTRACEIN((""));

    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }    

    DLTRACEOUT((""));    
    }

// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdInstallOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));    
    
   
    DLTRACEOUT((""));
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Initializer
// ---------------------------------------------------------------------------
//
TInt CNcdInstallOperation::Initialize()
    {
    DLTRACEIN( ( "" ) );

    TRAPD( err,
        {
        DLTRACE(("Writing initialize response"));
        CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
        CleanupStack::PushL( buf );

        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
         
        // write completion code
        stream.WriteInt32L( ENCDOperationMessageCompletionInit );        

        // Get the file count from the node and return it to the proxy
        // Notice here that the node has to have some kind of metadata
        // in order the install operation to start.
        DLINFO(("Get the metadata of the node"));
        CNcdNode& node( iNodeManager->NodeL( *iNodeId ) );
        CNcdNodeMetaData& metaData( node.NodeMetaDataL() );
        DLINFO(("Get the install object of the metadata."));
        CNcdNodeInstall& install( metaData.InstallL() );        
        stream.WriteInt32L( install.DownloadedFiles().MdcaCount() );        
        
        CleanupStack::PopAndDestroy( &stream );        

        DLINFO(("Create report"));
        // Because everything has gone smoothly so far,
        // create the install report for the server reports.
        CreateReportL();        
        
        DLTRACE(("Response length: %i", buf->Size() ));

        iPendingMessage->CompleteAndReleaseL( buf->Ptr(0), KErrNone );
        iPendingMessage = NULL;
        CleanupStack::PopAndDestroy( buf );
        });
    
    if ( err != KErrNone ) 
        {
        DLTRACE( ( "Err: %d", err ) );
        iPendingMessage->CompleteAndRelease( err );
        iPendingMessage = NULL;
        }
    DLTRACEOUT(( "err: %d", err ));
    return err;    
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstallOperation::CNcdInstallOperation( 
    MNcdOperationRemoveHandler& aRemoveHandler,    
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,    
    MNcdInstallReportObserver& aReportObserver,
    MCatalogsSession& aSession ) : 
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, EInstallOperation, aSession ),
    iHttpSession( aHttpSession ),
    iAccessPointManager( aGeneralManager.AccessPointManager() ),
    iReportObserver( aReportObserver ),
    iReportId( KNcdReportNotSupported )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdInstallOperation::ConstructL( const CNcdNodeIdentifier& aNodeId )
    {
    DLTRACEIN( ( "" ) );
    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();
    
    // copy the identifier
    iNodeId = CNcdNodeIdentifier::NewL( aNodeId );
    
    iInstallService = &CNcdProviderUtils::InstallationServiceL();
    DLTRACEOUT(( "" ));
    }    
    

// ---------------------------------------------------------------------------
// Gets info for an installable file from the node
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::GetFileInfoL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    TInt nextFile = ReadFileIndexL( aMessage );
    
    CNcdNode& node( iNodeManager->NodeL( *iNodeId ) );
    // Notice that in order the node to start install, the
    // node has to have some kind of metadata
    CNcdNodeMetaData& metaData( node.NodeMetaDataL() );
    CNcdNodeDownload& nodeDl = metaData.DownloadL();
    CNcdNodeInstall& install( metaData.InstallL() );
    
    // Check that file index is in the valid range
    if ( nextFile < 0 || nextFile >= install.DownloadedFiles().MdcaCount() ) 
        {
        DLERROR(("Invalid index"));
        User::Leave( KErrArgument );
        }

    // Get item purpose        
    TUint completePurpose = 0;
    TPtrC contentMime;
    
    DeleteJad();
    
    TRAPD( error, 
        {
        const CNcdNodeContentInfo& contentInfo = 
            node.NodeMetaData()->ContentInfoL();
        
        completePurpose = contentInfo.Purpose();
        contentMime.Set( contentInfo.MimeType() );
        DLINFO(( _L("Content mime: %S"), &contentMime ));
        });
    
    LeaveIfNotErrorL( error, KErrNotFound );    
    
    // By default, use unknown purpose since installer
    // uses purpose for installing themes
    TNcdItemPurpose purpose = ENcdItemPurposeUnknown;

    // Check if the purpose includes theme or application
    if ( completePurpose & ENcdItemPurposeTheme ) 
        {
        DLTRACE(("Purpose: Theme"));
        purpose = ENcdItemPurposeTheme;
        }
    else if ( completePurpose & ENcdItemPurposeApplication 
        || completePurpose & ENcdItemPurposeGame 
        || completePurpose & ENcdItemPurposeScreensaver )
        {
        DLTRACE(("Purpose: Application"));
        purpose = ENcdItemPurposeApplication;
        }
    
    // Used to index downloadInfos.
    TInt downloadInfoIndex = nextFile;        
    
    // This check used to be for handling rights objects that were not really
    // rights objects, eg. a picture given in the rights uri but now it has
    // been removed
    if ( install.DownloadedFiles().MdcaCount() >
         nodeDl.DownloadInfo().Count() )
        {
        DLERROR(("Too many downloaded files, leaving with KErrCorrupt"));
        User::Leave( KErrCorrupt );
        }
    
    DLTRACE(("File index: %d, downloadInfoIndex: %d", nextFile, downloadInfoIndex ));
    CNcdFileInfo* fileInfo = NULL;

    // Use normal install by default
    CNcdInstallInfo::TNcdInstallType installType = 
        CNcdInstallInfo::ENcdInstallNormal;
        
            
    // Skip files that have not been downloaded or are already installed
    // Files that are downloaded are not skipped, doesn't matter 
    // if they are installed or not
    while( nextFile < install.DownloadedFiles().MdcaCount() &&
           ( !install.DownloadedFiles().MdcaPoint( nextFile ).Length() ||
           // Check if file is missing and is installed, if it's missing
           // and not installed, we give an error by failing the installation
           // on proxy side when the file is missing
            ( !BaflUtils::FileExists( CNcdProviderUtils::FileSession(), 
                install.DownloadedFiles().MdcaPoint( nextFile ) ) && (                    
            install.IsContentInstalledL( nextFile, EFalse ) >= ENcdApplicationInstalled ||
            // unless the file is launcher application which we can happily skip over
            IsOneOf( nodeDl.DownloadInfo()[ downloadInfoIndex ]->ContentUsage(),
                MNcdPurchaseDownloadInfo::ELauncher,                
                MNcdPurchaseDownloadInfo::ELauncherOpen,
                MNcdPurchaseDownloadInfo::EConsumable ) ) ) ) )
        {
        DLTRACE(("Skipping installed files or not downloaded files"));            
        nextFile++;    
        downloadInfoIndex++;
        }
   
    // We may have skipped over the last possible file so we have to 
    // complete the operation
    if ( nextFile == install.DownloadedFiles().MdcaCount() ) 
        {
        DLERROR(("Either nothing has been downloaded or everything is already installed"));
        HandleAllFilesInstalledL( nextFile, aMessage );
        DLTRACEOUT(("Operation complete because all files have been installed"));
        return;
        }
        
    // Shortcut to correct download info
    MNcdPurchaseDownloadInfo* downloadInfo( 
        nodeDl.DownloadInfo()[ downloadInfoIndex ] );       

    // Set dependency flag that we know not to update dependency/launcher
    iIsDependency = NcdPurchaseHistoryUtils::IsDependency( *downloadInfo );
        
    
    DLINFO(( _L("Descriptor type: %S"), &downloadInfo->DescriptorType() ));
    DLINFO(( _L("Descriptor name: %S"), &downloadInfo->DescriptorName() ));
    DLINFO(( _L("Descriptor URI: %S"), &downloadInfo->DescriptorUri() ));
                
    
    if ( downloadInfo->ContentMimeType().MatchF( 
            KMimeTypeMatch1JavaApplication ) != KErrNotFound ||
         downloadInfo->ContentMimeType().MatchF( 
            KMimeTypeMatch2JavaApplication ) != KErrNotFound ||
         contentMime.MatchF( 
            KMimeTypeMatch1JavaApplication ) != KErrNotFound ||
         contentMime.MatchF( 
            KMimeTypeMatch2JavaApplication ) != KErrNotFound )

        {
        if ( downloadInfo->DescriptorType().CompareF( 
            KDescriptorTypeJad ) == 0 )
            {
            DLINFO(("Java app with JAD"));
            purpose = ENcdItemPurposeApplication;
            installType = CNcdInstallInfo::ENcdInstallJad;                
            }
        else 
            {            
            DLINFO(("Java-application"));
            // Ensure that purpose is correct
            purpose = ENcdItemPurposeApplication;
            installType = CNcdInstallInfo::ENcdInstallJar;
            }
        }
    else if ( downloadInfo->ContentMimeType().MatchF(
            KMimeTypeMatchJad ) != KErrNotFound ||
        contentMime.MatchF( KMimeTypeMatchJad ) != KErrNotFound )
        {
        DLINFO(("Java app with JAD"));
        purpose = ENcdItemPurposeApplication;        
        installType = CNcdInstallInfo::ENcdInstallJad;                
        }

    // compare with widget mimetype. If matching, assign ENcdInstallWidget to install type
    // The type value will be checked later when installing starts.
    else if( downloadInfo->ContentMimeType().MatchF( KMimeTypeMatchWidget ) != KErrNotFound )
        {
        purpose = ENcdItemPurposeApplication;        
        installType = CNcdInstallInfo::ENcdInstallWidget;  
        }

    DLTRACE(("Creating fileinfo"));
    // Create file info for proxy-side installer
    fileInfo = CNcdFileInfo::NewLC( 
        install.DownloadedFiles().MdcaPoint( nextFile ), 
        nodeDl.DownloadInfo()[ downloadInfoIndex ]->ContentMimeType(), 
        purpose );

    if ( installType == CNcdInstallInfo::ENcdInstallJad ) 
        {
        DLINFO(( _L("Descriptor type: %S"), &downloadInfo->DescriptorType() ));
        DLINFO(( _L("Descriptor name: %S"), &downloadInfo->DescriptorName() ));
        DLINFO(( _L("Descriptor URI: %S"), &downloadInfo->DescriptorUri() ));
        DLINFO(( "Descriptor Data: %S", &downloadInfo->DescriptorData() ));
        if ( downloadInfo->DescriptorData().Length() ) 
            {
            // This "if" is an ugly fix for embedded DD descriptors where
            // the .dm file is given in downloaddetails 
            if ( !(downloadInfo->DescriptorType() == KDescriptorTypeOdd &&
                downloadInfo->ContentUri().Length() ) )
                {
                // Writing JAD to file on this side because proxies can't
                // write to engine's private dir
                if ( iJadFile )
                {
                    DeletePtr( iJadFile );
                }
                iJadFile = 
                    CNcdProviderUtils::InstallationServiceL().WriteJadL(
                        fileInfo->FilePath(), downloadInfo->DescriptorData() );
                }
            }
        else 
            {
            DLINFO(("No descriptor for JAD install"));                        
            }
        // Ensure that mime is JAD
        fileInfo->SetMimeTypeL( KMimeTypeMatchJad );
        }
    
        
    CNcdInstallInfo* info = CNcdInstallInfo::NewL( fileInfo,
        installType );
    CleanupStack::Pop( fileInfo );
    CleanupStack::PushL( info );
    info->SetIndex( nextFile );    
        
    MCatalogsBaseMessage* message = &aMessage;
    // Send the info back
    TInt err = CompleteMessage( 
        message, 
        ENCDOperationMessageCompletionComplete,
        *info, 
        KErrNone );
    
    CleanupStack::PopAndDestroy( info );
    DLTRACEOUT(("err: %d", err));
    }


// ---------------------------------------------------------------------------
// Completes the message correctly when all files have already been installed
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::HandleAllFilesInstalledL( 
    TInt aFinalIndex, MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    CNcdInstallInfo* info = CNcdInstallInfo::NewL( NULL,
        CNcdInstallInfo::ENcdInstallNormal );            
    CleanupStack::PushL( info );
    info->SetIndex( aFinalIndex );    
        
    MCatalogsBaseMessage* message = &aMessage;
    // Send the info back
    TInt err = CompleteMessage( 
        message, 
        ENCDOperationMessageCompletionComplete,
        *info, 
        KErrNone );
    CleanupStack::PopAndDestroy( info );
    DLTRACEOUT(("Err: %d", err));
    }


// ---------------------------------------------------------------------------
// Updates the info of an installed file
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::UpdateInstalledFileInfoL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
        
    // Read input message
    DLTRACE(("Reading file index"));
    TInt index = reader().ReadInt32L();

    // Read the info from the stream
    CNcdFileInfo* info = CNcdFileInfo::NewLC( reader() );
    
    // Read uids of installed applications
    TInt appUids = reader().ReadInt32L();
    DLTRACE(( "Reading app uids: %d", appUids ));
    RArray<TUid> uids;
    CleanupClosePushL( uids );
    uids.ReserveL( appUids );
    while( appUids ) 
        {
        uids.AppendL( TUid::Uid( reader().ReadInt32L() ) );
        DLINFO(("Read UID: %x", uids[uids.Count() - 1] ));
        --appUids;
        }
    
    
    // Installed dependencies/upgrades are not updated to purchase history
    if ( !iIsDependency ) 
        {        
        //update purchase history with the info         
        UpdatePurchaseHistoryL( *info, uids, 
            aMessage.Session().Context().FamilyId() );
        }
        
    CleanupStack::PopAndDestroy( 3, &reader );   // uids, info, reader
    
    DLTRACE(("Updating node from purchase history"));
    // Update node from the purchase history
    iNodeManager->InstallHandlerL( *iNodeId );
        
    DLTRACE(("Completing message"));
    MCatalogsBaseMessage* message = &aMessage;
    TInt err = CompleteMessage( 
        message, 
        ENCDOperationMessageCompletionComplete,
        KErrNone );
    
    DLTRACEOUT((""));
    }


void CNcdInstallOperation::CreateReportL()
    {
    DLTRACEIN((""));  
    CNcdNode& node( iNodeManager->NodeL( *iNodeId ) ); 
    CNcdNodeMetaData& metaData( node.NodeMetaDataL() );
    
    TNcdReportStatusInfo info( ENcdReportCreate, KErrNone );
    // Use the node identifier to identify the content in install report.
    // Node id uniquely identifies the node that contains contents
    // that will be installed. One node may contains multiple contents but
    // they are all thought as one bundle, in one operation. Also, notice that 
    // multiple nodes can contain same metadata and same content.

    /**
     * @ Get timestamp and purchase option id from purchase details and
     * set them to the report
     */
    iReportId = 
        iReportObserver.RegisterInstallL( 
            iNodeId->NodeId(),
            metaData.Identifier(),
            info,
            metaData.Identifier().ServerUri(),
            metaData.Identifier().NodeNameSpace() );

    // Access point is set when report is started.
    // Then base message is used to get required info.
    }


void CNcdInstallOperation::StartReportL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    // Set access point for report.
    UpdateReportAccessPointL( aMessage.Session().Context().FamilyId() );    

    TNcdReportStatusInfo info( ENcdReportStart, KErrNone );
    iReportObserver.ReportInstallStatusL(
            iReportId,
            info );

    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndRelease( KErrNone );
    }
    
    
void CNcdInstallOperation::CompleteReportL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    RReadStream& stream( reader() );
    TInt errorCode( stream.ReadInt32L() );
        
    CleanupStack::PopAndDestroy( &reader );        

    TNcdReportStatus status( ENcdReportSuccess );
    if ( errorCode == KErrNone )
        {
        status = ENcdReportSuccess;
        }
    else if ( errorCode == KErrCancel )
        {
        status = ENcdReportCancel;
        }
    else 
        {
        status = ENcdReportFail;
        }
    
    // Create the status info object with the given info.
    TNcdReportStatusInfo info( status, errorCode );
    
    iReportObserver.ReportInstallStatusL(
            iReportId,
            info );

    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndRelease( KErrNone );
    }


// ---------------------------------------------------------------------------
// UpdateReportAccessPointL
// ---------------------------------------------------------------------------
//
void CNcdInstallOperation::UpdateReportAccessPointL( const TUid& aClientUid )
    {
    DLTRACEIN((""));

    CNcdPurchaseHistoryDb& db = iNodeManager->PurchaseHistory();    
    CNcdNode& node( iNodeManager->NodeL( *iNodeId ) ); 
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    
    CNcdPurchaseDetails* purchase = 
        NcdPurchaseHistoryUtils::PurchaseDetailsLC(
            db,
            aClientUid,
            metadata.Identifier(),
            EFalse );
                
    // Create origin identifier
    CNcdNodeIdentifier* originIdentifier = 
        CNcdNodeIdentifier::NewL(
            node.Identifier().NodeNameSpace(), 
            purchase->OriginNodeId(), 
            node.Identifier().ClientUid() );

    CleanupStack::PopAndDestroy( purchase );
    
    CleanupStack::PushL( originIdentifier );
    
    // Get report ap    
    TUint32 apId( 0 );

    TInt error = 
        iAccessPointManager.AccessPointIdL(
            *originIdentifier, 
            MCatalogsAccessPointManager::EBrowse, 
            aClientUid, 
            apId );
        
    TCatalogsConnectionMethod reportAp;
    if ( error == KErrNone ) 
        {
        DLTRACE(( "Setting access point %d for reports", apId ))   
        reportAp = 
            TCatalogsConnectionMethod( 
                apId, 
                ECatalogsConnectionMethodTypeAccessPoint );
        }
    
    if ( reportAp.iId == 0 ) 
        {
        reportAp = iHttpSession.ConnectionManager().DefaultConnectionMethod();
        }

    CleanupStack::PopAndDestroy( originIdentifier );

    iReportObserver.SetInstallReportAccessPoint( 
        iReportId,
        reportAp );
    }


// ---------------------------------------------------------------------------
// Updates purchase history
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::UpdatePurchaseHistoryL( const CNcdFileInfo& aInfo,
    const RArray<TUid>& aAppUids, const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    CNcdPurchaseHistoryDb& db = iNodeManager->PurchaseHistory();    
    CNcdNode& node( iNodeManager->NodeL( *iNodeId ) ); 
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    
    CNcdPurchaseDetails* purchase = NcdPurchaseHistoryUtils::PurchaseDetailsLC(
        db,
        aClientUid,
        metadata.Identifier(),
        EFalse );
                
    
    CNcdPurchaseInstallInfo* installInfo = CNcdPurchaseInstallInfo::NewLC();    
       
    const CNcdNodeContentInfo* contentInfo = NULL;                
    TRAPD( err, contentInfo = &metadata.ContentInfoL() );            

    LeaveIfNotErrorL( err, KErrNotFound );
    
    if ( aAppUids.Count() ) 
        {
        DLTRACE(("Updating installed application info"));
        // Use application UID from the protocol if it exists
        if ( contentInfo && contentInfo->Uid() != TUid::Null() ) 
            {
            DLINFO(( "Using UID from Content info: %x", 
                contentInfo->Uid().iUid ));
            installInfo->SetApplicationUid( contentInfo->Uid() );
            }
        else
            {                
            DLINFO(( "Using UID from installer: %x", aAppUids[0].iUid ));
            installInfo->SetApplicationUid( aAppUids[0] );
            }
        
        // Theme version number's are not updated from sis registry
        if ( aInfo.Purpose() != ENcdItemPurposeTheme &&
             aInfo.FilePath().Length() ) 
            {
            DLTRACE(( _L("Got version number from sis registry: %S"), 
                &aInfo.FilePath() ));
                
            installInfo->SetApplicationVersionL(
                aInfo.FilePath() );
            }
        else if ( contentInfo ) 
            {        
            DLTRACE(("Content info exists"));        
            // Use application version from the protocol
            DLINFO(( _L("Setting app version: %S"), 
                &contentInfo->Version() ));
            installInfo->SetApplicationVersionL( 
                contentInfo->Version() );
            }
            
        }
    else if ( aInfo.Purpose() == ENcdItemPurposeTheme )
        {
        DLTRACE(("Updating installed theme info"));
        installInfo->SetThemeNameL( aInfo.FilePath() );
        }
    else
        {
        DLTRACE(("Updating installed content info"));
        DLINFO(( _L("Installed file: %S"), &aInfo.FilePath() ));
        
        // Save the filename 
        installInfo->SetFilenameL( aInfo.FilePath() );
        }
    
    // update purpose to purchase history
    
    // if purpose in content info was unknown, we use the one gotten from installer
    // since it may have been updated to something more specific
    if ( contentInfo && contentInfo->Purpose() != ENcdItemPurposeUnknown )     
        {        
        TUint newPurpose = contentInfo->Purpose();
        if ( aInfo.Purpose() == ENcdItemPurposeApplication ) 
            {
            // This ensures that games etc. stuff that were installed like 
            // applications, are also handled like applications 
            newPurpose |= ENcdItemPurposeApplication;
            }
        DLTRACE(("Setting purpose as %d", newPurpose));
        purchase->SetItemPurpose( newPurpose );
        }
    else
        {        
        // backup in case we didn't get content info for some reason
        DLINFO(("Didn't get ContentInfo, updating purpose from FileInfo"));
        purchase->SetItemPurpose( purchase->ItemPurpose() | aInfo.Purpose() );
        }
    
    DLINFO(("Item purpose: %d", purchase->ItemPurpose() ));
    // Add install info to purchase details. Ownership is transferred
    
    if ( ReplaceInstallInfoL( *purchase, *installInfo ) )
        {
        CleanupStack::PopAndDestroy( installInfo );
        }
    else 
        {
        DLTRACE(("Adding install info to purchase details"));    
        purchase->AddInstallInfoL( installInfo );        
        CleanupStack::Pop( installInfo );
        }

    DLTRACE(("Saving purchase details"));                
    db.SavePurchaseL( *purchase, EFalse );    
    CleanupStack::PopAndDestroy( purchase );
    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Checks if the details already contain a matching info
// ---------------------------------------------------------------------------
//        
TBool CNcdInstallOperation::ReplaceInstallInfoL( 
    MNcdPurchaseDetails& aDetails, 
    const MNcdPurchaseInstallInfo& aInfo )
    {
    DLTRACEIN((""));
    TArray< MNcdPurchaseInstallInfo* > installInfos( 
        aDetails.InstallInfoL() );
        
    TParsePtrC path( aInfo.Filename() );
    TPtrC installedFile( path.NameAndExt() );
    DLTRACE(("Going through %d install infos", installInfos.Count() ));
    for ( TInt i = 0; i < installInfos.Count(); ++i )
        {
        MNcdPurchaseInstallInfo& oldInfo( *installInfos[ i ] );
        
        if ( ( oldInfo.ApplicationUid() == aInfo.ApplicationUid() &&
               oldInfo.ApplicationVersion() == aInfo.ApplicationVersion() &&
               oldInfo.ThemeName() == aInfo.ThemeName() ) ||
               // also replace empty infos, hopefully this doesn't break anything
               // because this is needed in order to correctly handle cancelled
               // theme installations or more spefically successful theme installations
               // after a cancelled theme installation
             ( oldInfo.ApplicationUid() == KNullUid && 
               oldInfo.ApplicationVersion() == KNullDesC &&
               // replace the old info if theme name is empty or if the old theme name
               // is not empty but the new info is entirely empty == reinstalling a theme
               ( oldInfo.ThemeName() == KNullDesC || 
                ( aInfo.ApplicationUid() == KNullUid &&
                  aInfo.ApplicationVersion() == KNullDesC &&
                  aInfo.ThemeName() == KNullDesC ) ) ) )
             
            {
            // Parse the filename from the file path
            TParsePtrC oldPath( oldInfo.Filename() );
            if ( oldPath.NameAndExt() == installedFile ) 
                {                
                DLTRACEOUT(("Info already exists"));
                
                // Update the filename in case the file has been installed
                // to a different drive than before
                CNcdPurchaseInstallInfo* modInfo = 
                    static_cast<CNcdPurchaseInstallInfo*>( installInfos[i] );
                
                // This updates the theme name in the following case:
                // 1. theme was already installed when bought and installed with the client
                // 2. user uninstalled the theme
                // 3. user downloaded and installed the theme again
                if ( oldInfo.ThemeName() == KNullDesC ) 
                    {
                    modInfo->SetThemeNameL( aInfo.ThemeName() );
                    }
                modInfo->SetFilenameL( aInfo.Filename() );
                return ETrue;
                }
            }                    
        }
    
    // This tries to update an existing java app uid when we are reinstalling
    // java apps    
    TArray< MNcdPurchaseDownloadInfo* > downloadInfos(
        aDetails.DownloadInfoL() );        
        
    if ( installInfos.Count() &&
         aInfo.ApplicationUid() != KNullUid && 
         downloadInfos.Count() >= installInfos.Count() )        
        {   
        TBool isJava = EFalse;
        for ( TInt i = 0; i < downloadInfos.Count(); ++i )
            {
            if ( IsJava( downloadInfos[i]->ContentMimeType(), ETrue ) )
                {
                isJava = ETrue;
                break;
                }
            }
        
        if ( !isJava ) 
            {
            DLTRACEOUT(("No java app"));
            return EFalse;
            }
            
        DLTRACE(("Update the uid of the first existing app"));
        for ( TInt i = 0; i < installInfos.Count(); ++i ) 
            {
            MNcdPurchaseInstallInfo& oldInfo( *installInfos[ i ] );
            if ( oldInfo.ApplicationUid() != KNullUid ) 
                {
                DLTRACE(("Updating application uid from %x to %x",
                    oldInfo.ApplicationUid(), aInfo.ApplicationUid() ));
                // Update the app uid for java app                
                CNcdPurchaseInstallInfo* modInfo = 
                    static_cast<CNcdPurchaseInstallInfo*>( installInfos[i] );
                
                modInfo->SetApplicationUid( aInfo.ApplicationUid() );
                modInfo->SetApplicationVersionL( aInfo.ApplicationVersion() );
                return ETrue;
                }                
            }
        }
        
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Checks if the given mime type matches a java app
// ---------------------------------------------------------------------------
//        
TBool CNcdInstallOperation::IsJava( 
    const TDesC& aMimeType, 
    TBool aAcceptJad ) const
    {
    DLTRACEIN(( _L("aMimeType: %S"), &aMimeType ));
    
    TBool matches = aMimeType.MatchF( KMimeTypeMatch1JavaApplication ) != KErrNotFound ||
                  aMimeType.MatchF( KMimeTypeMatch2JavaApplication ) != KErrNotFound;

    if ( !matches && aAcceptJad ) 
        {
        matches = aMimeType.MatchF( KMimeTypeMatchJad ) != KErrNotFound;
        }
    DLTRACEOUT(("Matched: %d", matches));
    return matches;    
    }


// ---------------------------------------------------------------------------
// Gets the path to the file indexed by the message
// ---------------------------------------------------------------------------
//        
HBufC* CNcdInstallOperation::FilePathLC( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    TInt fileIndex = ReadFileIndexL( aMessage );
    
    CNcdNode& node( iNodeManager->NodeL( *iNodeId ) );
    // Notice that in order the node to start install, the
    // node has to have some kind of metadata
    CNcdNodeMetaData& metaData( node.NodeMetaDataL() );    
    CNcdNodeInstall& install( metaData.InstallL() );
    
    // Check that file index is in the valid range
    if ( fileIndex < 0 || fileIndex >= install.DownloadedFiles().MdcaCount() ) 
        {
        DLERROR(("Invalid index: %d", fileIndex ));
        User::Leave( KErrArgument );
        }
                    
    DLTRACEOUT(( _L("Filepath %S from index: %d"), 
        &install.DownloadedFiles().MdcaPoint( fileIndex ), fileIndex ));

    return install.DownloadedFiles().MdcaPoint( fileIndex ).AllocLC();
    }


// ---------------------------------------------------------------------------
// Opens a file
// ---------------------------------------------------------------------------
//  
void CNcdInstallOperation::OpenFileL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    DLTRACE(("Sharing the file server"));
    User::LeaveIfError( fs.ShareProtected() );
    
    DLTRACE(("Trying to open the file"));
    RFile file;
    CleanupClosePushL( file );
    
    // Ugly hackfix for handling JADs
    if ( iJadFile ) 
        {
        User::LeaveIfError( file.Open( fs, 
            *iJadFile,
            NcdProviderDefines::KNcdSharableFileOpenFlags ) );                        
        }
    else
        {
        HBufC* path = FilePathLC( aMessage ); 
        User::LeaveIfError( file.Open( fs, 
            *path,
            NcdProviderDefines::KNcdSharableFileOpenFlags ) );                
        CleanupStack::PopAndDestroy( path );
        }
    DLTRACE(("File open, transferring to client"));
    
    aMessage.CompleteAndReleaseL( fs, file );
    
    DLTRACE(("File transferred"));
    CleanupStack::PopAndDestroy( 2, &fs ); // file, fs    
    }


// ---------------------------------------------------------------------------
// Does the actual message handling
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::DoReceiveMessageL( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber )
    {
    DLTRACEIN((""));
    switch ( aFunctionNumber )
        {
        case ENCDOperationFunctionGetData:
            {
            GetFileInfoL( *aMessage );
            break;
            }
            
        case ENCDOperationFunctionSetData:
            {
            UpdateInstalledFileInfoL( *aMessage );
            break;
            }

        case ENcdOperationFunctionOpenFile:
            {
            OpenFileL( *aMessage );
            break;
            }

        case ENcdOperationFunctionDeleteFile:
            {
            DeleteFileL( *aMessage );
            break;
            }

        case ENCDOperationFunctionReportStart:
            {
            StartReportL( *aMessage );
            break;
            }

        case ENCDOperationFunctionReportComplete:
            {
            CompleteReportL( *aMessage );
            break;
            }

        default:
            {
            DLTRACE(("Calling baseclass"));
            // Call implementation in the base class
            CNcdBaseOperation::ReceiveMessage( aMessage, aFunctionNumber );            
            break;
            }            
        } 
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Deletes a file index by the message. Also JAD is deleted if necessary
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::DeleteFileL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    HBufC* path = FilePathLC( aMessage );
    // Delete installed SIS/JAR/content file
    User::LeaveIfError( iInstallService->DeleteFile( *path ) );      
    
    CleanupStack::PopAndDestroy( path );
    // If we installed JAD+JAR we must delete JAD separately.
    DeleteJad();
    
    aMessage.CompleteAndReleaseL( KErrNone, KErrNone );    
    }


// ---------------------------------------------------------------------------
// Reads a file index from the message
// ---------------------------------------------------------------------------
//        
TInt CNcdInstallOperation::ReadFileIndexL( MCatalogsBaseMessage& aMessage )
    {
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    // Read the requested file index
    TInt fileIndex = reader().ReadInt32L();
    CleanupStack::PopAndDestroy( &reader );
    return fileIndex;
    }


// ---------------------------------------------------------------------------
// Deletes JAD from disk and the filename
// ---------------------------------------------------------------------------
//        
void CNcdInstallOperation::DeleteJad()
    {
    DLTRACEIN((""));
    
    if ( iJadFile && iInstallService ) 
        {        
        // Error is ignored because we can't handle disk errors in 
        // any special way and other errors will make themselves known
        // anyway
        iInstallService->DeleteFile( *iJadFile );              
        }
    
    DeletePtr( iJadFile );
    }
