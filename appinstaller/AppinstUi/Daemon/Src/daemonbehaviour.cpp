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

#include "daemonbehaviour.h"
#include "swispubsubdefs.h"
#include "recsisx.h"
#include "sisregistrywritablesession.h"
#include "SWInstDebug.h"
#include "sisregistryentry.h"


namespace Swi
    {
    // For uninstaller
    CDaemonBehaviour* CDaemonBehaviour::NewL( CProgramStatus& aMainStatus )
        {
        CDaemonBehaviour* self = NewLC( aMainStatus );
        CleanupStack::Pop(self);
        return self;
        }
  
    CDaemonBehaviour* CDaemonBehaviour::NewLC( CProgramStatus& aMainStatus )
        {
        CDaemonBehaviour* self = new (ELeave) CDaemonBehaviour;
        CleanupStack::PushL(self);
        self->ConstructL( aMainStatus );
        return self;  
        }
    
    void CDaemonBehaviour::ConstructL( CProgramStatus& aMainStatus )
        {
        User::LeaveIfError(iFs.Connect());
        User::LeaveIfError(iFs.ShareProtected());
        
        // For uninstaller
        iSisInstaller = CSisInstaller::NewL( this, aMainStatus );
        // Create plugin
        TRAP_IGNORE( iSwiDaemonPlugin = CSwiDaemonPlugin::NewL() );   
        }
  
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
    
    // from MDaemonBehaviour
    TBool CDaemonBehaviour::StartupL()
        {
        // Return state of Startup
        return ETrue;
        }

    void CDaemonBehaviour::MediaChangeL(TInt aDrive, TChangeType aChangeType)
        {
        FLOG_1( _L("Daemon: Media change %d"), aDrive );
        RSisRegistryWritableSession registrySession;
        
        User::LeaveIfError(registrySession.Connect());
        CleanupClosePushL(registrySession);
        
        if (aChangeType==EMediaInserted)
            {
            FLOG( _L("Daemon: Media inserted") );  
            FLOG( _L("Daemon: Media change: Update sis registry") );            
            // notify IAR
            // Error ou1cimx1#212652
            // Note SWI Daemon needs to notify sis registry from
            // media change. Otherwice registry is not updated
            // and e.g. IsPresentL function will give false results.
            // Note also that this function should be called before
            // ProcessPreinstalledFilesL in SWI Daemon.            
            registrySession.AddDriveL(aDrive);
                                                
            // Scan directory on the card and run pre-installed through SWIS
            FLOG( _L("Daemon: Media change: Process preinstalled files") );
            ProcessPreinstalledFilesL(aDrive);            
        
#ifdef RD_MULTIPLE_DRIVE   
            // Add inserted media drive to drive array.                     
            if ( iDriveArray.Find(aDrive) == KErrNotFound )
                {                
                iDriveArray.AppendL(aDrive); 
                }      
#endif      
            }
        else if (aChangeType==EMediaRemoved)
            {
            FLOG( _L("Daemon: Media removed") ); 
        
#ifdef RD_MULTIPLE_DRIVE     
            // Get Installer state.                   
            TBool installerRunning = iSisInstaller->IsInstalling();                        
#endif 
        
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK                                           
            // notify IAR
            // Note SWI Daemon need to notify sis registry from
            // media change.            
            registrySession.RemoveDriveL(aDrive);
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
        
            // Cancel all requests for install
            iSisInstaller->Cancel();
        
#ifdef RD_MULTIPLE_DRIVE
            // Notify plugin
            if(iSwiDaemonPlugin)
                {
                TInt index = iDriveArray.Find(aDrive);
                iSwiDaemonPlugin->MediaRemoved(index);
                }
#else
            // Notify plugin
            if(iSwiDaemonPlugin)
                {
                iSwiDaemonPlugin->MediaRemoved(aDrive);
                }
#endif //RD_MULTIPLE_DRIVE            
        
#ifdef RD_MULTIPLE_DRIVE
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
#endif            
            }
        
        CleanupStack::PopAndDestroy(&registrySession);
        }
      
    void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive)
        {
        _LIT(KDaemonPrivatePath,":\\private\\10202dce\\");
        
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

    // For uninstaller
    TInt& CDaemonBehaviour::GetProcessStatus()
        {
        return iGeneralProcessStatus;
        }

    // For uninstaller
    void CDaemonBehaviour::SetProcessStatus( TInt aStatus )
        {
        iGeneralProcessStatus = aStatus;
        }     
  
    // For plugin support
    void CDaemonBehaviour::DoNotifyMediaProcessingComplete()
        {
        if ( iSwiDaemonPlugin && iStartNotified )
            {
            iSwiDaemonPlugin->MediaProcessingComplete();
            }
        }
    
    // For plugin support
    TBool CDaemonBehaviour::IsPlugin()
        {        
        if ( iSwiDaemonPlugin )
            {
            return ETrue;
            }        
        return EFalse;
        }
    
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
    
    // For plugin support
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
                                        
    } // namespace Swi
//EOF
