/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <usif/scr/scr.h>
#include <usif/scr/scrcommon.h>
#include <usif/scr/screntries.h>
#include <apgcli.h>    // RApaSession
#include <apgupdate.h> // TApaAppUpdateInfo

#include "daemonbehaviour.h"
#include "swispubsubdefs.h"
#include "recsisx.h"
#include "sisregistrywritablesession.h"
#include "SWInstDebug.h"
#include "sisregistryentry.h"

using namespace Swi;

// -----------------------------------------------------------------------
// CDaemonBehaviour::NewL
// -----------------------------------------------------------------------
//    
CDaemonBehaviour* CDaemonBehaviour::NewL( CProgramStatus& aMainStatus )
    {
    CDaemonBehaviour* self = NewLC( aMainStatus );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::NewLC
// -----------------------------------------------------------------------
//
CDaemonBehaviour* CDaemonBehaviour::NewLC( CProgramStatus& aMainStatus )
    {
    CDaemonBehaviour* self = new (ELeave) CDaemonBehaviour;
    CleanupStack::PushL(self);
    self->ConstructL( aMainStatus );
    return self;  
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::ConstructL
// -----------------------------------------------------------------------
//        
void CDaemonBehaviour::ConstructL( CProgramStatus& aMainStatus )
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    
    // For uninstaller
    iSisInstaller = CSisInstaller::NewL( this, aMainStatus );
    // Create plugin
    TRAP_IGNORE( iSwiDaemonPlugin = CSwiDaemonPlugin::NewL() ); 
    
    iRegSessionConnected = EFalse;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::~CDaemonBehaviour
// -----------------------------------------------------------------------
//      
CDaemonBehaviour::~CDaemonBehaviour()
    {
    if ( iSwiDaemonPlugin )
        {
        delete iSwiDaemonPlugin;
        REComSession::FinalClose();
        }   
    delete iSisInstaller;
    iSisInstaller = NULL;
    iFs.Close();                
    iDriveArray.Close(); 
    
    if ( iRegSessionConnected )
        {   
        iRegistrySession.Close();        
        }
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::StartupL
// -----------------------------------------------------------------------
//         
TBool CDaemonBehaviour::StartupL()
    {
    // Return state of Startup
    return ETrue;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::MediaChangeL
// -----------------------------------------------------------------------
//     
void CDaemonBehaviour::MediaChangeL(TInt aDrive, TChangeType aChangeType)
    {
    FLOG_1( _L("Daemon: MediaChangeL: Media change %d"), aDrive );
        
    if ( !iRegSessionConnected )
        {   
        User::LeaveIfError( iRegistrySession.Connect() );
        iRegSessionConnected = ETrue;
        }
            
    if ( aChangeType==EMediaInserted )
        {
        FLOG( _L("Daemon: MediaChangeL: Media inserted") ); 
        TInt err = KErrNone;
        // Notify SCR and AppArc for media change.
        TRAP( err, UpdateComponentStatusL( aChangeType, aDrive ) );
        FLOG_1( _L("Daemon: UpdateComponentStatusL err = %d"), err );

        // We need call sis registry since this call will
        // activate sis registry to clean uninstalled components
        // from inserted media.
        iRegistrySession.AddDriveL( aDrive );
                                                
        // Scan directory on the card and run pre-installed through SWIS
        FLOG( _L("Daemon: MediaChangeL: Process preinstalled files") );
        ProcessPreinstalledFilesL( aDrive );  
        FLOG_1( _L("Daemon: MediaChangeL: StartInstalling drive: %d"), aDrive );
        iSisInstaller->StartInstallingL();
                      
        // Add inserted media drive to drive array.                     
        if ( iDriveArray.Find(aDrive) == KErrNotFound )
            {                
            iDriveArray.AppendL(aDrive); 
            }           
        }
    else if (aChangeType==EMediaRemoved)
        {
        FLOG( _L("Daemon: MediaChangeL: Media removed") );       
        TInt err = KErrNone;           
        // Notify SCR and AppArc for media change.
        TRAP( err, UpdateComponentStatusL( aChangeType, aDrive ) );
        FLOG_1( _L("Daemon: UpdateComponentStatusL err = %d"), err );
 
        // Get Installer state.                   
        TBool installerRunning = iSisInstaller->IsInstalling();
        FLOG_1( _L("Daemon: MediaChangeL: IsInstalling = %d"), installerRunning );
        
        FLOG( _L("Daemon: MediaChangeL: Cancel install process") );
        // Cancel all requests for install
        iSisInstaller->Cancel();                       
    
        // Notify plugin
        if( iSwiDaemonPlugin )
            {
            TInt index = iDriveArray.Find(aDrive);
            iSwiDaemonPlugin->MediaRemoved(index);
            }
    
        // Get index of removed drive from array
        TInt index = iDriveArray.Find(aDrive);
    
        if ( index > KErrNotFound )
            {
            iDriveArray.Remove(index); 
            iDriveArray.Compress();   
            }
                
        // Continue installing from other drives if needed.
        if ( installerRunning )
            {
            FLOG( _L("Daemon: MediaChangeL: Continue installing other drives") );
            // Get count of inserted drives.
            TInt count = iDriveArray.Count();
            FLOG_1( _L("Daemon: Drive count = %d"), count );        
            if ( count )            
                {
                // Find packages for other drives.
                for(index = 0; index < count; index++ )
                    {                   
                    ProcessPreinstalledFilesL(iDriveArray[index]);                                        
                    }
                // Start installing.
                FLOG( _L("Daemon: MediaChangeL: StartInstallingL") );
                iSisInstaller->StartInstallingL();
                }                
            }                      
        }
        
    iRegistrySession.Close();
    iRegSessionConnected = EFalse;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::ProcessPreinstalledFilesL
// -----------------------------------------------------------------------
//          
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive)
    {
    FLOG_1( _L("Daemon: ProcessPreinstalledFilesL: Drive index: %d"), 
            aDrive );
     _LIT( KDaemonPrivatePath,":\\private\\10202dce\\" );
    
    // For uninstaller
    // Set on installing mode.
    iGeneralProcessStatus = EStateInstalling; 
    FLOG_1( _L("[CDaemonBehaviour] iGeneralProcessStatus = %d"), 
    iGeneralProcessStatus );
    
    ProcessPreinstalledFilesL(aDrive, KDaemonPrivatePath);
    iStartNotified = EFalse;
    iDrive = aDrive;    
    FLOG( _L("Daemon: ProcessPreInstalledFilesL END") );
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::ProcessPreinstalledFilesL
// -----------------------------------------------------------------------
//        
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive, const TDesC& aDirectory)
    {    
    TPath preInstalledPath;
    TChar drive;
    RFs::DriveToChar(aDrive, drive);
    preInstalledPath.Append(drive);
    preInstalledPath.Append(aDirectory);
    
    FLOG_1( _L("Daemon: ProcessPreInstalledFilesL Getting dir %S"), &preInstalledPath );
    CDir* dir = NULL;
    TInt err = iFs.GetDir( preInstalledPath, KEntryAttNormal, ESortNone, dir ); 
    if ( err != KErrNone && err != KErrPathNotFound )
        {
        FLOG_1( _L("Daemon: ProcessPreInstalledFilesL GetDir with error %d"), err );
        User::Leave(err);
        }
    if(dir)
        {
        // dir will only exist if GetDir succeeded
        CleanupStack::PushL(dir);
        for(TInt i = 0; i < dir->Count(); i++)
            {
            const TEntry &entry = (*dir)[i];
            if(!entry.IsDir())
                {
                TFileName fileName(preInstalledPath);
                fileName.Append(entry.iName);                
                // Add files to sis installer.
                iSisInstaller->AddFileToInstallL(fileName);
                }
            }
        CleanupStack::PopAndDestroy(dir);
        }
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::GetProcessStatus
// -----------------------------------------------------------------------
//     
TInt& CDaemonBehaviour::GetProcessStatus()
    {
    return iGeneralProcessStatus;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::SetProcessStatus
// -----------------------------------------------------------------------
//     
void CDaemonBehaviour::SetProcessStatus( TInt aStatus )
    {
    iGeneralProcessStatus = aStatus;
    }     

// -----------------------------------------------------------------------
// CDaemonBehaviour::DoNotifyMediaProcessingComplete
// -----------------------------------------------------------------------
//      
void CDaemonBehaviour::DoNotifyMediaProcessingComplete()
    {
    if ( iSwiDaemonPlugin && iStartNotified )
        {
        iSwiDaemonPlugin->MediaProcessingComplete();
        }
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::IsPlugin
// -----------------------------------------------------------------------
//         
// For plugin support
TBool CDaemonBehaviour::IsPlugin()
    {        
    if ( iSwiDaemonPlugin )
        {
        return ETrue;
        }        
    return EFalse;
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::NotifyPlugin
// -----------------------------------------------------------------------
//        
// For plugin support
void CDaemonBehaviour::NotifyPlugin()
    {
    FLOG( _L("Daemon: NotifyPlugin"));
    if ( !iStartNotified )
        {
        FLOG_1( _L("Daemon: MediaProcessingStart for drive = %d"), iDrive );
        iSwiDaemonPlugin->MediaProcessingStart( iDrive );
        iStartNotified = ETrue;
        }    
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::RequestPluginInstall
// -----------------------------------------------------------------------
//         
void CDaemonBehaviour::RequestPluginInstall( TDesC& aSisFile )
    {
    // Note We will open file in here but that is only because
    // plug-in interface has been designed this way. Original
    // Symbian SWI daemon did use RApaLsSession for installing
    // and thus RFile was open already. 
    // Since current SWI Daemon does use SWI API it doesn't 
    // open files and thus we need to open the file in here.        
    FLOG( _L("Daemon: RequestPluginInstall "));
            
    RFile file;
    User::LeaveIfError( file.Open( 
            iFs, aSisFile, EFileRead | EFileShareReadersOnly ) );
    CleanupClosePushL( file );
    
    iSwiDaemonPlugin->RequestInstall( file );
    
    // close file, original SWI Daemon do not leave file open.
    CleanupStack::PopAndDestroy( &file ); 
    }
 
// -----------------------------------------------------------------------
// CDaemonBehaviour::UpdateComponentStatusL
// -----------------------------------------------------------------------
//         
void CDaemonBehaviour::UpdateComponentStatusL( TChangeType aChangeType, 
                                               TInt aDrive )
    {
    FLOG( _L("Daemon: UpdateComponentStatus") ); 
            
    Usif::RSoftwareComponentRegistry scrServer;       
    User::LeaveIfError( scrServer.Connect() );
    CleanupClosePushL( scrServer );
            
    // Filter all other component types except sisx.
    Usif::CComponentFilter* filter = Usif::CComponentFilter::NewLC();        
    filter->SetSoftwareTypeL( Usif::KSoftwareTypeNative );
        
    RArray<Usif::TComponentId> componentIdList;
    CleanupClosePushL( componentIdList );
    
    // Get list of all native (sisx) installed packages. 
    scrServer.GetComponentIdsL( componentIdList, filter  );
    FLOG_1( _L("Daemon: componentIdList count: %d"),componentIdList.Count() ); 
   
    if ( componentIdList.Count() )
        {
        RArray<TApaAppUpdateInfo> appInfoArray;         
        CleanupClosePushL( appInfoArray );
        FLOG_1( _L("Daemon: target drive: %d"), aDrive );
                              
#ifdef _DEBUG
        TChar targetDrive;
        iFs.DriveToChar( aDrive, targetDrive );
        HBufC* tarceBuf = HBufC::NewL( 16 );
        TPtr bufPtr = tarceBuf->Des();
        bufPtr.Append( targetDrive );
        FLOG_1( _L("Daemon: target drive: %S"), &bufPtr ); 
        delete tarceBuf;
#endif         
    
        FLOG( _L("Daemon: Check all SCR native components") ); 
        // Check all components in SCR. If media is removed/inserted
        // change status flag in SCR and in AppArc.
        for ( TInt index=0; index < componentIdList.Count(); index++ )
            {  
            Usif::TComponentId componentId( componentIdList[index] );
            FLOG_1( _L("Daemon: componentId: %d"), componentId ); 
        
            // Check that package is not in ROM. If package is in rom
            // it can not be removed/not present.
            TBool inRom = scrServer.IsComponentOnReadOnlyDriveL( componentId );
            FLOG_1( _L("Daemon: IsComponentOnReadOnlyDriveL: %d"), inRom ); 
            
            if ( !inRom )
                {              
                UpdateStatusL( scrServer, 
                               componentId, 
                               aChangeType,
                               aDrive,
                               appInfoArray );
                }                            
            } // for loop
        
        FLOG_1( _L("Daemon: appInfoArray.Count = %d"), appInfoArray.Count() );         
        // Check do we have updated some application info.
        if ( appInfoArray.Count() )
            {             
            // Update AppArc list after we have all application 
            // uids/status in array. 
            FLOG( _L("Daemon: Update AppArc") );
            RApaLsSession appArcSession;
            TInt err = appArcSession.Connect();
            FLOG_1( _L("Daemon: appArcSession.Connect err = %d"), err ); 
            if ( !err )
                {
                CleanupClosePushL( appArcSession );            
                FLOG( _L("Daemon: UpdateAppListL") ); 
                appArcSession.UpdateAppListL( appInfoArray );            
                CleanupStack::PopAndDestroy(); // appArcSession                 
                }
            }
        CleanupStack::PopAndDestroy( &appInfoArray );
        } // if componentList.Count()
             
    CleanupStack::PopAndDestroy( &componentIdList ); //componentIDList.Close();
    CleanupStack::PopAndDestroy( filter ); 
    CleanupStack::PopAndDestroy( &scrServer );  
    FLOG( _L("Daemon: UpdateComponentStatus END") ); 
    } 

// -----------------------------------------------------------------------
// CDaemonBehaviour::UpdateStatusL
// -----------------------------------------------------------------------
//    
void CDaemonBehaviour::UpdateStatusL( 
    Usif::RSoftwareComponentRegistry& aScrServer,
    Usif::TComponentId aComponentId,
    TChangeType aChangeType,
    TInt aTargetDrive,
    RArray<TApaAppUpdateInfo>& aAppInfoArray )     
    {
    FLOG( _L("Daemon: CDaemonBehaviour::UpdateStatusL start") ); 
    
    Usif::CComponentEntry* entry = Usif::CComponentEntry::NewL();
    CleanupStack::PushL( entry );
        
    TInt err = KErrNone;
    TRAP( err, aScrServer.GetComponentL( aComponentId, 
                                         *entry, 
                                         Usif::KUnspecifiedLocale ) );    
    FLOG_1( _L("Daemon: GetComponentL TRAP err = %d"), err );
    
    // If we get error let's not stop the loop.
    if ( !err )
        { 
        TBool isInTargetDrive = EFalse;
        // Get all component drives.
        TDriveList driveList;                
        driveList = entry->InstalledDrives();           

        FLOG( _L("Daemon: Check all drives for this component") ); 
        // Go through all drives which have files for this component.
        for ( TInt i = 0; i < KMaxDrives; i++ )
            {                
            if ( driveList[i] != 0 )
                {                                           
                FLOG_1( _L("Daemon: Found drive: %d"), i ); 
                
                if ( aTargetDrive == i )
                    {
                    // Ok we have files in this target drive.
                    isInTargetDrive = ETrue;
                    FLOG( _L("Daemon: Component has files in target drive") ); 
                    }
                }
            } // for
                            
        // Check if component or part of it is in the media.
        if ( isInTargetDrive )
            {
            // Update SCR status for the component. Note SCR 
            // needs to be updated because AppArc will check 
            // status from SCR.                                             
            FLOG( _L("Daemon: Set component status to SCR") );
                        
            if ( aChangeType == EMediaInserted )
                {  
                FLOG( _L("Daemon: Set component presence = TRUE") );                                                                                              
                iRegistrySession.SetComponentPresenceL( aComponentId, ETrue );                                                            
                }
            else if ( aChangeType == EMediaRemoved )
                {
                FLOG( _L("Daemon: Set component presence = FALSE") ); 
                iRegistrySession.SetComponentPresenceL( aComponentId, EFalse );                                                                                                               
                }                        
                                                                      
            // We need to update applications status to AppArc when
            // there is some media change. AppArc needs application
            // UID (not package UID) so we need to get all app. UIDs
            // from the package and set them to present or not present.
            
            FLOG( _L("Daemon: Create application status for AppArc") );
            FLOG( _L("Daemon: Get applications UIDs from SCR") );
            RArray<TUid> appUidArray;
            CleanupClosePushL( appUidArray );
             
            TRAP( err, aScrServer.GetAppUidsForComponentL( aComponentId, appUidArray ) );
            FLOG_1( _L("Daemon: GetAppUidsForComponentL TRAP err = %d"), err );                        
            FLOG_1( _L("Daemon: UID array count = %d"), appUidArray.Count() );
            
            for (TInt index = 0; index < appUidArray.Count(); index++)
                {
                FLOG_1( _L("Daemon: Add app UID = 0x%x"), 
                 appUidArray[index].iUid );                
                
                TApaAppUpdateInfo appInfo;
                appInfo.iAppUid = appUidArray[index]; 
                
                if ( aChangeType == EMediaInserted  )
                    {
                    appInfo.iAction = TApaAppUpdateInfo::EAppPresent;
                    FLOG( _L("Daemon: Set app = EAppPresent") );
                    }
                else
                    {
                    appInfo.iAction = TApaAppUpdateInfo::EAppNotPresent;
                    FLOG( _L("Daemon: Set app = EAppNotPresent") );
                    }
                
                aAppInfoArray.Append( appInfo );   
                }                           
            CleanupStack::PopAndDestroy(&appUidArray);       
            }   // if isInTargetDrive        
        }   // if err
    
    CleanupStack::PopAndDestroy( entry );
    entry = NULL;
    
    FLOG( _L("Daemon: CDaemonBehaviour::UpdateStatusL end") ); 
    }

//EOF
