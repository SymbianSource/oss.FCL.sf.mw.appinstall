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
#include <apgcli.h>    //RApaSession
//TODO enable apparc support
//#include <appinfo.h> //TAppInfo

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
            
#ifdef RD_MULTIPLE_DRIVE    
    iDriveArray.Close();
#endif        
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
    FLOG_1( _L("Daemon: Media change %d"), aDrive );
    RSisRegistryWritableSession registrySession;
    
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    
    
    if ( aChangeType==EMediaInserted )
        {
        FLOG( _L("Daemon: Media inserted") ); 

        // Notify SCR and AppArc for media change.
        UpdateComponentStatusL( aChangeType, aDrive );

        // We need call sis registry since this call will
        // activate sis registry to clean uninstalled components
        // from inserted media.
        registrySession.AddDriveL(aDrive);
                                                
        // Scan directory on the card and run pre-installed through SWIS
        FLOG( _L("Daemon: Media change: Process preinstalled files") );
        ProcessPreinstalledFilesL(aDrive);            
       
        // Add inserted media drive to drive array.                     
        if ( iDriveArray.Find(aDrive) == KErrNotFound )
            {                
            iDriveArray.AppendL(aDrive); 
            }           
        }
    else if (aChangeType==EMediaRemoved)
        {
        FLOG( _L("Daemon: Media removed") );       
                   
        // Notify SCR and AppArc for media change.
        UpdateComponentStatusL( aChangeType, aDrive );
 
        // Get Installer state.                   
        TBool installerRunning = iSisInstaller->IsInstalling();
        
        // Cancel all requests for install
        iSisInstaller->Cancel();                       
    
        // Notify plugin
        if(iSwiDaemonPlugin)
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
            // Get count of inserted drives.
            TInt count = iDriveArray.Count();
                    
            if ( count )            
                {                             
                for(index = 0; index < count; index++ )
                    {                                        
                    ProcessPreinstalledFilesL(iDriveArray[index]);
                    iSisInstaller->StartInstallingL();                
                    }
                }                
            }                      
        }
    
    CleanupStack::PopAndDestroy(&registrySession);
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::ProcessPreinstalledFilesL
// -----------------------------------------------------------------------
//          
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive)
    {
//    _LIT( KDaemonPrivatePath,":\\private\\10202dce\\" );
    
    // NOTE this is only for testing since we can not use
    // Daemon's private folder. Sifserver do not have allfiles
    // capability currently.
    _LIT( KDaemonPrivatePath,":\\installs\\swidaemon\\" );
    
#ifndef RD_MULTIPLE_DRIVE
    iSisInstaller->Cancel();
#endif      
    // For uninstaller
    // Set on installing mode.
    iGeneralProcessStatus = EStateInstalling; 
    FLOG_1( _L("[CDaemonBehaviour] iGeneralProcessStatus = %d"), 
    iGeneralProcessStatus );
    
    ProcessPreinstalledFilesL(aDrive, KDaemonPrivatePath);
    iStartNotified = EFalse;
    iDrive = aDrive;    
    iSisInstaller->StartInstallingL();
    }

// -----------------------------------------------------------------------
// CDaemonBehaviour::ProcessPreinstalledFilesL
// -----------------------------------------------------------------------
//        
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive, const TDesC& aDirectory)
    {
    FLOG( _L("Daemon: ProcessPreInstalledFilesL") );
    TPath preInstalledPath;
    TChar drive;
    RFs::DriveToChar(aDrive, drive);
    preInstalledPath.Append(drive);
    preInstalledPath.Append(aDirectory);
    
    FLOG_1( _L("Daemon: ProcessPreInstalledFilesL Getting dir %S"), &preInstalledPath );
    CDir* dir = NULL;
    TInt err = iFs.GetDir(preInstalledPath, KEntryAttNormal, ESortNone, dir); 
    if (err != KErrNone && err != KErrPathNotFound)
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
void CDaemonBehaviour::UpdateComponentStatusL( 
   TChangeType aChangeType, 
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
// TODO Enabloi AppArcin päivitys. 
// TODO Hae TAppInfon headeri includeen.        
//            RArray<TAppInfo> appinfoArray;
//            CleanupClosePushL( appinfoArray );
    
        // Convert the given target drive number to drive letter.
        TChar targetDrive;
        iFs.DriveToChar( aDrive, targetDrive );
        FLOG_1( _L("Daemon: targetDrive: 0x%x"), TUint( targetDrive ) ); 
    
        FLOG( _L("Daemon: Check all SCR native components") ); 
        // Check all components in SCR. If media is removed/inserted
        // change status flag in SCR and in AppArc.
        for ( TInt index=0; index < componentIdList.Count(); index++ )
            {                       
            Usif::CComponentEntry* entry = Usif::CComponentEntry::NewL();
            CleanupStack::PushL( entry );
            
            Usif::TComponentId componentId( componentIdList[index] );
            FLOG_1( _L("Daemon: componentId: %d"), componentId ); 
            
            scrServer.GetComponentL( componentId, 
                                     *entry, 
                                     Usif::KUnspecifiedLocale ); 
            
// TODO poista ei tarvetta jos filter toimii.
            //if ( entry->SoftwareType() == Usif::KSoftwareTypeNative  )
            
            // Get all component drives.
            TDriveList driveList;                
            driveList = entry->InstalledDrives();
               
            TBool isInTargetDrive = EFalse;

            FLOG( _L("Daemon: Check all drives for this component") ); 
            // Go through all drives which have files for 
            // this component.
            for ( TInt i = 0; i < KMaxDrives; i++ )
                {                
                if ( driveList[i] != 0 )
                    {
                    TChar installDrive = 
                            static_cast<TChar>( driveList[i] );
                
                    FLOG_1( _L("Daemon: driveList index: %d"), i ); 
                    FLOG_1( _L("Daemon: installDrive: 0x%x"), TUint( installDrive ) ); 
                    
                    if ( targetDrive == installDrive )
                        {
                        // Ok we have files in this target drive.
                        isInTargetDrive = ETrue;
                        FLOG( _L("Daemon: SW in target drive.") ); 
                        }
                    }
                }
                                
            // Check if component or part of it is in the media.
            if ( isInTargetDrive )
                {
                FLOG( _L("Daemon: Set component status to SCR") ); 
//                    TAppInfo appInfo;                     
//                    appInfo.iAppUid = componentId;
                                    
                if ( aChangeType == EMediaInserted )
                    {  
                    // Update component flag to SCR.
                    scrServer.SetIsComponentPresentL( componentId, 
                                                      ETrue );
                    FLOG( _L("Daemon: Set component present = TRUE") );                             
                    // Set app status for AppArc. AppArc sees this 
                    // as new component.
//                        appInfo.iAppUid = TAppInfo::ENewApp;
                    }
                else if ( aChangeType==EMediaRemoved )
                    {
                   // Update component flag to SCR.
                    scrServer.SetIsComponentPresentL( componentId, 
                                                      EFalse );
                    FLOG( _L("Daemon: Set component present = FALSE") );                            
                    // Set app status for AppArc. AppArc sees this 
                    // component as removed.
//                        appInfo.iAppUid = TAppInfo::ERemoveApp;
                    }
                
                // Add component info to array.
//                    appinfoArray.Append( appInfo );                    
                }                     
            
            CleanupStack::PopAndDestroy( entry );
            entry = NULL;
            } // for
//TODO enable AppArc support when TAppInfo is available.            
/*       
         FLOG( _L("Daemon: Set component status to AppArc") ); 
         // Update AppArc list after we have all components in array.               
         RApaLsSession appArcSession;
         TInt err = appArcSession.Connect();
         if ( !err )
             {
             CleanupClosePushL(appArcSession);
             appArcSession.UpdateAppListL( appinfoArray );
             CleanupStack::PopAndDestroy();                   
             }
        
        CleanupStack::PopAndDestroy( &appinfoArray );
*/ 
        } // if componentList.Count()
             
    CleanupStack::PopAndDestroy( &componentIdList ); //componentIDList.Close();
    CleanupStack::PopAndDestroy( filter ); 
    CleanupStack::PopAndDestroy( &scrServer );  
    FLOG( _L("Daemon: UpdateComponentStatus END") ); 
    }       
    
//EOF
