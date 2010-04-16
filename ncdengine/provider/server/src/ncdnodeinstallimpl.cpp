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
* Description:   Implements CNcdNodeInstall class
*
*/


#include "ncdnodeinstallimpl.h"

#include <f32file.h>
#include <s32mem.h>
#include <bautils.h>


#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "ncdutils.h"
#include "ncdpurchaseinstallinfo.h"
#include "ncdpurchasedetails.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdinstallinfo.h"
#include "ncdfileinfo.h"
#include "ncdextendedinstallinfo.h"
#include "ncdproviderutils.h"
#include "ncdinstallationservice.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdpanics.h"
#include "ncdpurchasehistoryutils.h"
#include "ncdproviderdefines.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdnodemanager.h"
#include "ncdnodeidentifier.h"

#include "catalogsdebug.h"


// Maximum number of content file opening attempts if the first
// one fails with KErrInUse
const TInt KMaxRetries = 3;

// Delay between content file opening attempts if the first
// one fails with KErrInUse
const TInt KOpenDelay = 500000;

static TInt CallBackOpenFile( TAny* aObject )
    {
    DLTRACEIN((""));
    static_cast<CNcdNodeInstall*>( aObject )->OpenContentFileRunner();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CNcdNodeInstall
// ---------------------------------------------------------------------------
//  


CNcdNodeInstall::CNcdNodeInstall( NcdNodeClassIds::TNcdNodeClassId aClassId,
    const CNcdNodeMetaData& aMetadata )
: CCatalogsCommunicable(),
  iClassId( aClassId ),
  iMetadata( aMetadata )
    {
    }

void CNcdNodeInstall::ConstructL()
    {
    DLTRACEIN((""));    
    iDownloadedFiles = new(ELeave) CDesCArrayFlat( KListGranularity );
    DLTRACEOUT((""));
    }


CNcdNodeInstall* CNcdNodeInstall::NewL( const CNcdNodeMetaData& aMetadata )
    {
    CNcdNodeInstall* self =   
        CNcdNodeInstall::NewLC( aMetadata );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeInstall* CNcdNodeInstall::NewLC( const CNcdNodeMetaData& aMetadata )
    {
    CNcdNodeInstall* self = 
        new( ELeave ) CNcdNodeInstall( 
            NcdNodeClassIds::ENcdNodeInstallClassId, aMetadata );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeInstall::~CNcdNodeInstall()
    {
    DLTRACEIN((""));

    // Delete member variables here
    iInstallInfos.ResetAndDestroy();
    delete iDownloadedFiles;
    
    delete iInstalledContent;
    delete iPeriodic;

    DLTRACEOUT((""));
    }        

NcdNodeClassIds::TNcdNodeClassId CNcdNodeInstall::ClassId() const
    {
    return iClassId;
    }




// ---------------------------------------------------------------------------
// Set node data from purchase details
// ---------------------------------------------------------------------------
//  
TBool CNcdNodeInstall::InternalizeL( const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN(("this-ptr: %X", this));

    if ( !aDetails.DownloadInfoL().Count() ) 
        {
        DLTRACEOUT(("No downloadinfos so there's nothing to install"));
        return EFalse;
        }

    DLTRACE(("Copying paths of downloaded files"));
    
    CDesCArray* tempDownloads = new(ELeave) CDesCArrayFlat( KListGranularity );
    CleanupStack::PushL( tempDownloads );
    
    TBool atLeastOneFile = EFalse;
    // Get names of downloaded files
    for ( TInt i = 0; i < aDetails.DownloadedFiles().MdcaCount(); ++i ) 
        {
        const TDesC& filepath( aDetails.DownloadedFiles().MdcaPoint( i ) );
        atLeastOneFile = atLeastOneFile || filepath.Length();
            
        // append even empty descriptors so that the arrays stay identical
        tempDownloads->AppendL( filepath );
        
        DLINFO(( _L("Downloaded: %S"), 
            &aDetails.DownloadedFiles().MdcaPoint( i ) ));
        }

    TArray<MNcdPurchaseInstallInfo*> info ( aDetails.InstallInfoL() );
        
    // Check download status
    if ( !atLeastOneFile && !info.Count() ) 
        {
        CleanupStack::PopAndDestroy( tempDownloads );
        DLINFO(("Nothing has been downloaded or installed"));
        return EFalse;
        }
           
    TArray<MNcdPurchaseDownloadInfo*> dlInfo ( aDetails.DownloadInfoL() );               
    

    DLTRACE(("Internalizing install infos, count: %d", info.Count() ));

    RPointerArray<CNcdExtendedInstallInfo> tempInfos;
    CleanupResetAndDestroyPushL( tempInfos );
    
    tempInfos.ReserveL( info.Count() );
    iLaunchable = EFalse;        
    
    CNcdExtendedInstallInfo* tempInstall = NULL;
    TBool launchable = EFalse;
    
    TBool atLeastOneLauncher = EFalse;
    TBool atLeastOneLaunchableLauncher = EFalse;
    TBool atLeastOneLauncherOpen = EFalse;
    
    // If there are more install infos than download infos we assume that
    // the first install infos are for rights files
    TInt dlIndex = dlInfo.Count() - info.Count();
    if ( dlIndex > 0 ) 
        {
        dlIndex = 0;
        }
    
    // Internalize infos
    for ( TInt i = 0; i < info.Count(); ++i, ++dlIndex )
        {
        const MNcdPurchaseInstallInfo& install( *info[i] );
        
        if ( dlIndex >= 0 ) 
            {            
            const MNcdPurchaseDownloadInfo& download( *dlInfo[dlIndex] );
            
            launchable = download.IsLaunchable();
            DLTRACE(("Launchable from purchase history: %d", launchable ));
            
            TBool isLauncher = IsOneOf( download.ContentUsage(), 
                       MNcdPurchaseDownloadInfo::ELauncher,
                       MNcdPurchaseDownloadInfo::ELauncherOpen );
            
            // Themes are not launchable. This also checks for reinstalled themes            
            if ( install.ThemeName() != KNullDesC || 
                 ( install.ThemeName() == KNullDesC &&
                   install.ApplicationUid() == TUid::Null() &&
                   install.Filename() == KNullDesC ) )
                {
                launchable = EFalse;
                }
            // check launcher apps
            else if ( isLauncher )
                {
                atLeastOneLauncher = ETrue;
                // Can't use IsContentInstalledL since it uses iInstallInfos-array
                // which is generated here
                if ( CNcdProviderUtils::IsApplicationInstalledL(
                        install.ApplicationUid(), 
                        install.ApplicationVersion() ) <= ENcdApplicationNotInstalled  )
                    {
                    DLTRACE(("Setting launchable=EFalse"));
                    launchable = EFalse;            
                    }
                else 
                    {
                    atLeastOneLaunchableLauncher = ETrue;
                    }
               }
             
            
            tempInstall = CNcdExtendedInstallInfo::NewLC( install,
                download.ContentMimeType(), launchable );

            tempInstall->SetUriExists( download.ContentUri().Length() != 0 );
            // Sets content type to tempInstall according to download's contentUsage
            SetContentType( *tempInstall, download );
            
            
            if ( download.ContentUsage() == 
                    MNcdPurchaseDownloadInfo::ELauncherOpen ) 
                {                
                atLeastOneLauncherOpen = ETrue;
                }
             
            // Set item as launchable if at least one file is launchable    
            if ( launchable ) 
                {
                iLaunchable = ETrue;
                }
            }
        else 
            {
            DLTRACE(("Rights file"));
            tempInstall = CNcdExtendedInstallInfo::NewLC( install,
                KNullDesC, ETrue );
            }        
        
        tempInfos.AppendL( tempInstall );
        CleanupStack::Pop( tempInstall );
        tempInstall = NULL;        
            
        DLTRACE(("Info added, iInstallInfo count: %d", iInstallInfos.Count() ));
        }
    
    if ( atLeastOneLauncher && !atLeastOneLaunchableLauncher )
        {
        DLTRACE(("No launchable launchers, setting everything unlaunchable"));
        iLaunchable = EFalse;
        TInt count = tempInfos.Count();
        while ( count-- ) 
            {
            tempInfos[ count ]->SetLaunchable( EFalse );
            }
        }

    // There was at least one launcher/open dependency so we need to go through
    // the install infos and try to find a suitable file for opening
    if ( atLeastOneLauncherOpen && 
         tempInfos.Count() <= dlInfo.Count() ) 
        {
        DLTRACE(("Setting launcher param"));
        TInt count = tempInfos.Count();
        
        for ( TInt i = 0; i < count; ++i )
            {
            const MNcdPurchaseDownloadInfo& download( *dlInfo[ i ] );
            if ( download.ContentUsage() == MNcdPurchaseDownloadInfo::ELauncherOpen ) 
                {
                SetLaunchParameterL( tempInfos, i );
                }            
            }
        }
        
    DLTRACE(("Replacing old values with new ones"));


    // Read installed status
    iInstalled = ( aDetails.State() == MNcdPurchaseDetails::EStateInstalled );
        
    DLINFO(( "Installed: %d", iInstalled ));
    
    // Get purpose
    iPurpose = aDetails.ItemPurpose();
    DLINFO(( "Purpose: %d", iPurpose ));        
    
    // Replace old values
    iInstallInfos.ResetAndDestroy();    
    iInstallInfos = tempInfos;
    CleanupStack::Pop( &tempInfos );
    
    
    delete iDownloadedFiles;
    iDownloadedFiles = tempDownloads;
    CleanupStack::Pop( tempDownloads );
    
    TInt depCount = DependencyCount( dlInfo );

    if ( !iInstalled && 
         depCount &&
         // must be more download infos than just deps
         dlInfo.Count() > depCount &&
         // install info count must be at least the amount of dlInfos 
         // since deps have equal number of install and dl infos and
         // all content must have been installed before
         info.Count() >= dlInfo.Count() )
        {
        iInstalled = ETrue;
        DLTRACE(("Dependency has already been installed, setting iInstalled to ETrue"));
        }

    
    // Identifier might be Null
    if ( iInstalledContent && iInstalledContent->ApplicationUid() != TUid::Null() ) 
        {
        
         DASSERT( iInstalledContent->ApplicationUid() != TUid::Null() );
        
        // Disable launching if protocol says so. 
        // By default apps defined in content info are launchable
        iInstalledContent->SetLaunchable( iLaunchable );
        
        // Delete iInstalledContent if it is duplicated in purchase history
        for ( TInt i = 0; i < iInstallInfos.Count(); ++i ) 
            {
            
            if ( iInstalledContent->ApplicationUid() == 
                 iInstallInfos[i]->ApplicationUid() )
                {
                DLTRACE(("App has been bought and installed"));
                delete iInstalledContent;
                iInstalledContent = NULL;
                break;
                }
            }        
        }
    
    SetContentVersionL( aDetails.Version() );
    DLTRACEOUT(("iLaunchable: %d", iLaunchable));
    return ETrue;
    }


void CNcdNodeInstall::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                      TInt aFunctionNumber )
    {
    DLTRACEIN(("this-ptr: %X", this));    

    DASSERT( aMessage );

    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;
            
        case NcdNodeFunctionIds::ENcdInstallOpenFile:
            TRAP( trapError, OpenContentFileL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdSetApplicationInstalled:
            TRAP( trapError, SetApplicationInstalledRequestL( *aMessage ) );
            break;
                    
        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    if ( !iPeriodic ) 
        {        
        iMessage = NULL;        
        }
    
    DLTRACEOUT((""));
    }


void CNcdNodeInstall::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }
                


const MDesCArray& CNcdNodeInstall::DownloadedFiles() const
    {
    DASSERT( iDownloadedFiles );
    return *iDownloadedFiles;
    }
                

TNcdApplicationStatus CNcdNodeInstall::IsContentInstalledL( 
    TInt aIndex,
    TBool aIgnoreMissingUri )
    {
    DLTRACEIN(("aIndex: %d", aIndex));
    DASSERT( aIndex >= 0 );

    if ( aIndex >= ContentCount() ) 
        {
        DLTRACEOUT(("Index out of range (count = %d), returning EFalse", 
            ContentCount() ));
        return ENcdApplicationNotInstalled;
        }

    CNcdExtendedInstallInfo& info = *iInstallInfos[aIndex];
            
    TNcdApplicationStatus status = ENcdApplicationNotInstalled;
    
    if ( info.Filename() != KNullDesC ) 
        {
        DLTRACE(("Content: file"));
        if ( BaflUtils::FileExists( 
                CNcdProviderUtils::FileSession(),
                info.Filename() ) ) 
            {
            DLTRACE(( _L("File %S exists"), &info.Filename() ));
            status = ENcdApplicationInstalled;
            }
        }
    else if ( info.ApplicationUid() != TUid::Null() )
        {
        DLTRACE(("Content: application"));
        if ( aIgnoreMissingUri && !info.UriExists() ) 
            {
            DLTRACE(("No uri and ignoring missing uris"));
            status = ENcdApplicationInstalled;
            }
        else 
            {            
            status = CNcdProviderUtils::IsApplicationInstalledL(
                info.ApplicationUid(), 
                info.ApplicationVersion() );            
            }
        }
    else if ( info.ThemeName() != KNullDesC )
        {
        DLTRACE(("Content: theme"));
        if ( CNcdProviderUtils::InstallationServiceL().IsThemeInstalledL(
            info.ThemeName() ) )
            {
            status = ENcdApplicationInstalled;
            }
        }
    info.SetInstalledStatus( status );
    
    return status;
    }


TInt CNcdNodeInstall::ContentCount() const
    {
    return iInstallInfos.Count();
    }

    
TBool CNcdNodeInstall::IsAllContentInstalledL()
    {
    DLTRACEIN((""));
        
    for ( TInt i = 0; i < iInstallInfos.Count(); ++i ) 
        {        
        // Don't care whether older or newer version is installed as long as
        // some version is installed, ignores application that don't have content URIs

        if ( IsContentInstalledL( i, ETrue ) == ENcdApplicationNotInstalled ) 
            {
            return EFalse;
            }
        }
    DLTRACEOUT(("All installed"));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CNcdNodeInstall::InternalizeContentInfoL()
    {
    DLTRACEIN((""));
    
    if ( iInstallInfos.Count() ) 
        {
        DLTRACEOUT(("Already installed something"));
        return EFalse;
        }
    
    const CNcdNodeContentInfo* info = NULL;
    TRAPD( err, info = &iMetadata.ContentInfoL() );
    
    TBool installed = EFalse;

    // Either UID or Identifier exists, we can continue here
    if ( err == KErrNone && ( info->Uid() != TUid::Null()|| info->Identifier().Length() != 0 ) ) 
        {
        TNcdApplicationStatus status = ENcdApplicationNotInstalled;
        
        if (info->MimeType().Compare( KMimeTypeMatchWidget ) == 0 && info->Identifier().Length() != 0  )
            {
            status = CNcdProviderUtils::IsWidgetInstalledL(
                    info->Identifier(), 
                    info->Version() );
            }
        else if ( info->Uid() != TUid::Null() ) 
            {
            DLINFO(("Uid: %x", info->Uid().iUid ));
            status = 
                CNcdProviderUtils::IsApplicationInstalledL(
                                   info->Uid(), 
                                   info->Version() );
            }
        
        // Application can be older version for it to be considered installed
        // Upgrade will be available for the user
        installed = status != ENcdApplicationNotInstalled;

        delete iInstalledContent;
        iInstalledContent = NULL;

        iInstalled = installed;    
        iLaunchable = installed;
                                        
        if ( installed ) 
            {
            DLTRACE(("Application installed"));
            CNcdExtendedInstallInfo* install = CNcdExtendedInstallInfo::NewLC();

            if (info->MimeType().Compare( KMimeTypeMatchWidget ) == 0 )
                {                          
                install->SetApplicationUid(CNcdProviderUtils::WidgetUidL(info->Identifier()));
                }
            else
                {
                install->SetApplicationUid( info->Uid() );
                }
            
             // This ensures that CNcdInstalledApplication actually checks the 
             // application's version number when it checks if it's installed
             // or not
             install->SetUriExists( ETrue );

             // This will be used to determine whether the app is actually installed
             // or not
             install->SetApplicationVersionL( info->Version() );
             install->SetLaunchable( ETrue );

             // don't set because it can mess upgrade handling in 
             // CNcdNodeMetadata::HandleContentUpgradeL
              iContentVersion = TCatalogsVersion();

              iInstalledContent = install;
              CleanupStack::Pop( install );
              }
        
            }
    return installed;
    }

// ---------------------------------------------------------------------------
// Content version getter
// ---------------------------------------------------------------------------
//  
const TCatalogsVersion& CNcdNodeInstall::ContentVersion() const
    {
    return iContentVersion;
    }

    

void CNcdNodeInstall::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeInstall::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN(("this-ptr: %X", this));

    // Install existed. So, insert info that meta data was found.
    aStream.WriteInt32L( ClassId() );

    // Add additional content to the stream.
    // Make sure that this matches to the order that is used in the proxy
    // side when this stream is internalized.
    // NOTE: Be careful with the 8- and 16-bit descriptors. Remember to check
    // if the proxy wants the data in 16 or 8 bits?    

    aStream.WriteInt32L( iPurpose );

    aStream.WriteInt32L( iInstalled );
    
    aStream.WriteInt32L( iLaunchable );
    
    TInt count = iInstallInfos.Count();
    
    if ( iInstalledContent ) 
        {
        DLTRACE(("App from content info is installed, adding install info"));        
        aStream.WriteInt32L( count + 1 );    
        iInstalledContent->ExternalizeL( aStream );
        }
    else
        {
        aStream.WriteInt32L( count );
        }
        
    DLTRACE(("Externalizing infos, count: %d", count ));
        
    for ( TInt i = 0; i < count; ++i )
        {
        iInstallInfos[i]->ExternalizeL( aStream );        
        }  

    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// Handle release requests
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Opens a content file
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::OpenContentFileL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    RBuf8 buf;
    buf.CreateL( aMessage.InputLength() );
    CleanupClosePushL( buf );
    User::LeaveIfError( aMessage.ReadInput( buf ) );
    
    // Read the requested file index    
    TInt fileIndex = Des8ToInt( buf );
    if ( fileIndex < 0 || fileIndex >= iInstallInfos.Count() ) 
        {
        DLERROR(( "Index: %d out of range 0-%d", 
            fileIndex, iInstallInfos.Count() ));
        User::Leave( KErrArgument );
        }
        
    DLTRACE(( _L("Opening file %S from index: %d"), 
        &iInstallInfos[fileIndex]->Filename(), fileIndex ));
    
    iRetryCount = 0;
    iFileIndex = fileIndex;        
    
    // Try to open the file
    TRAPD( err, OpenContentFileL() );
    
    // N-series hackfix: if the file is in use, 
    // retry opening it about 0.5 seconds later
    if ( err == KErrInUse ) 
        {
        DLTRACE(("File in use, retry a little bit later"));
        if ( iPeriodic ) 
            {
            iPeriodic->Cancel();
            }
        else
            {            
            iPeriodic = CPeriodic::NewL( 0 );
            }
        TTimeIntervalMicroSeconds32 delay( KOpenDelay );
        iPeriodic->Start( delay, delay, TCallBack( CallBackOpenFile, this ) );
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    CleanupStack::PopAndDestroy( &buf ); // buf    
    DLTRACEOUT((""));
    }


void CNcdNodeInstall::SetApplicationInstalledRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Note! 
    // This function is only meant for the proxy side usage and for
    // certain specific exception cases. Also, this will only allow 
    // application info updates. For example, themes are not handled here.


    // Get the data that was sent from the proxy side.
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );

    TInt errorCode( inputStream.ReadInt32L() );
    DLINFO(( "Error code from message: %d", errorCode ));

    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );    
    
    
    CNcdPurchaseHistoryDb& purchaseHistoryDb( 
        iMetadata.NodeManager().PurchaseHistory() );

    CNcdPurchaseDetails* details( 
        NcdPurchaseHistoryUtils::PurchaseDetailsLC(
            purchaseHistoryDb,
            iMetadata.Identifier().ClientUid(),
            iMetadata.Identifier(),
            EFalse ) );

    if ( details == NULL )
        {
        // Because contents have already been downloaded, there should always
        // be some details available. Because content has been installed, we
        // suppose that the content is already downloaded also.
        User::Leave( KErrNotFound );
        }
    else
        {
        // We got the newest details.
        if ( errorCode == KErrNone )
            {
            if ( details->State() != MNcdPurchaseDetails::EStateInstalled )
                {
                DLINFO(("Content not installed yet"));
                // Update the fileinfo only if the state is not already installed.
                // If the state is installed, then the fileinfo is already
                // up-to-date.
                const CNcdNodeContentInfo& contentInfo(
                    iMetadata.ContentInfoL() );
                if ( ( contentInfo.Purpose() 
                       & ENcdItemPurposeTheme ) != 0 )
                    {
                    DLINFO(("Content purpose is theme. Not allowed."));
                    // A minor sanity check because themes may have an UID given.
                    // So, if the metadata suggests that the content is theme,
                    // then leave here. Otherwise, trust the user.
                    User::Leave( KErrAbort );
                    }
                TUid contentUid( contentInfo.Uid() );
                const TDesC& version( contentInfo.Version() );
                // The item state is installed when installed info count
                // corresponds the download info count
                TInt installCount( details->InstallInfoCount() );
                TInt downloadCount( details->DownloadInfoCount() );
                for ( TInt i = installCount; i < downloadCount; ++i )
                    {
                    // Set missing install infos.
                    // Notice, that we only know the main content UID and version.
                    // So, if the content is a bundle, then we need to fake
                    // the UIDs and version here and just use the one that 
                    // metadata content info can provide us.
                    CNcdPurchaseInstallInfo* installInfo(
                        CNcdPurchaseInstallInfo::NewLC() );
                    installInfo->SetApplicationUid( contentUid );
                    installInfo->SetApplicationVersionL( version );
                    // Insert new details in the end of the array.
                    details->InsertInstallInfoL( installInfo, 
                                                 details->InstallInfoCount() );
                    CleanupStack::Pop( installInfo );
                    }
                }

            // Because error code was KErrNone, the content is thought as
            // successfully installed. So, delete unnecessary installation
            // files. Notice, that there may be installation files available
            // even if the details state is EStateInstalled because download
            // could also  be done again even though installation has been 
            // successfull already during the first time. 
            // Get details file information.
            const MDesCArray& files = details->DownloadedFiles();
            for( TInt i = 0; i < files.MdcaCount(); ++i )
                {
                const TDesC& filePath( files.MdcaPoint( i ) );
                DLINFO((_L("Delete file: %S"), &filePath));
                // Notice, that we need to do the deletion here in the server side.
                // The installation files may exist in the NCD Engine server
                // private directory and that directory is normally
                // accessable only by the NCD Engine server side.
                TInt deleteError( 
                    CNcdProviderUtils::InstallationServiceL().
                        DeleteFile( filePath ) );
                DLINFO(( "Files: %d, delete error: %d", i, deleteError ));
                }            
            }

        // Set the error code and the lates operation time
        details->SetLastOperationErrorCode( errorCode );
        details->SetLastUniversalOperationTime();
            
        // Save purhcase details into the purchase history.
        // This will replace the old detail.
        // But, do not replace old icon, because we did not load it
        // for the details above.
        purchaseHistoryDb.SavePurchaseL( *details, EFalse );
        
        CleanupStack::PopAndDestroy( details );
        details = NULL;
        }

    // Complete the message because everything went ok. 
    // If some operations above left, then ReceiveMessage will handle
    // those cases.
    aMessage.CompleteAndRelease( KErrNone );
    }

// ---------------------------------------------------------------------------
// Opens a content file, called by CallbackOpenFile
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::OpenContentFileRunner()
    {
    DLTRACEIN((""));
    TRAPD( err, OpenContentFileL() );
    iRetryCount++;
    if ( err != KErrInUse || iRetryCount >= KMaxRetries ) 
        {
        DASSERT( iPeriodic ) 
        DLTRACE(("Deleting periodic timer"));
        delete iPeriodic;
        iPeriodic = NULL;

        if ( err != KErrNone )
            {
            DLTRACE(("Handling other errors"));
            iMessage->CompleteAndRelease( err );
            iMessage = NULL;
            }
        
        }
    }

// ---------------------------------------------------------------------------
// Opens a content file
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::OpenContentFileL()
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
    User::LeaveIfError( file.Open( fs, 
        iInstallInfos[iFileIndex]->Filename(),
        NcdProviderDefines::KNcdSharableFileOpenFlags ) );
        
        
    DLTRACE(("File open, transferring to client"));
    DASSERT( iMessage );
    iMessage->CompleteAndReleaseL( fs, file );
    iMessage = NULL;
    DLTRACE(("File transferred"));
    CleanupStack::PopAndDestroy( 2, &fs ); // file, fs    
    }


// ---------------------------------------------------------------------------
// Calculate the number of dependencies
// ---------------------------------------------------------------------------
//  
TInt CNcdNodeInstall::DependencyCount( 
    const TArray<MNcdPurchaseDownloadInfo*>& aInfos ) const
    {
    DLTRACEIN((""));
    TInt depCount = 0;
    for ( TInt i = 0; i < aInfos.Count(); ++i )    
        {
        if ( NcdPurchaseHistoryUtils::IsDependency( *aInfos[i] ) )
            {
            depCount++;
            }
        }
    return depCount;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::SetContentType( 
    CNcdExtendedInstallInfo& aInstall,
    const MNcdPurchaseDownloadInfo& aDownload ) const
    {
    DLTRACEIN((""));
    switch( aDownload.ContentUsage() ) 
        {
        case MNcdPurchaseDownloadInfo::EDownloadable:
            {
            aInstall.SetContentType( MNcdInstalledContent::EInstalledContent );
            break;
            }

        case MNcdPurchaseDownloadInfo::EDependency:
            {
            aInstall.SetContentType( MNcdInstalledContent::EInstalledDependency );
            break;
            }
        
        case MNcdPurchaseDownloadInfo::ELauncher:  // flowthrough      
        case MNcdPurchaseDownloadInfo::ELauncherOpen:
            {
            aInstall.SetContentType( MNcdInstalledContent::EInstalledLauncher );
            break;
            }
                
        default: 
            {
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
            }
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::SetLaunchParameterL( 
    RPointerArray<CNcdExtendedInstallInfo>& aInstalls, 
    TInt aIndex ) const
    {
    DLTRACEIN((""));
    TInt count = aInstalls.Count();
    CNcdExtendedInstallInfo& launcher( *aInstalls[ aIndex ] );
    
    // aIndex is the index of aInstall so increase it
    aIndex++;
    while ( aIndex < count ) 
        {        
        if ( aInstalls[ aIndex ]->Filename().Length() && 
             aInstalls[ aIndex ]->IsLaunchable() ) 
            {
            DLTRACE(( _L("Found \"%S\", setting as parameter"), 
                &aInstalls[ aIndex ]->Filename() ));
            launcher.SetParameterL( aInstalls[ aIndex ]->Filename() );
            break;
            }
        aIndex++;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CNcdNodeInstall::SetContentVersionL( const TDesC& aVersion ) 
    {
    DLTRACEIN((""));
    iContentVersion = TCatalogsVersion();    
    
    // Convert the version number in purchase history to TCatalogsVersion    
    TRAPD( err, TCatalogsVersion::ConvertL( 
        iContentVersion, aVersion ) );
        
    LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
    
    }
