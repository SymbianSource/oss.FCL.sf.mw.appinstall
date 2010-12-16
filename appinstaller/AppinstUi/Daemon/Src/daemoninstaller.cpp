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

#include <e32property.h>	//First boot issues.
#include <startupdomainpskeys.h> // Defines Statup PS keys.

#include "daemoninstaller.h"
#include "DialogWrapper.h"
#include "swispubsubdefs.h"

#include "SWInstDebug.h"
#include "SilentLauncher.h"
#include "SWInstDefs.h"

#include "sisregistrysession.h"
#include "siscontents.h"
#include "sisparser.h"
#include "filesisdataprovider.h"

#include "previouslyInstalledAppsCache.h"
#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "installationFailedAppsCache.h"
#include "sisregistryentry.h"
// For uninstaller
#include "programstatus.h"

using namespace Swi;

const static TInt KInstallRetryWaitTime = 10000000; // 10 secs
// For uninstaller
const static TInt KWaitUninstallerTime = 1000000; // 1 secs  

_LIT(KMCSisInstaller,"Daemon-Installer"); // Minor-Component name

// Two phased construction

CSisInstaller* CSisInstaller::NewL(    
   MDaemonInstallBehaviour* aDaemonBehaviour, 
   CProgramStatus& aMainStatus)
    {
    CSisInstaller* self = new (ELeave) CSisInstaller( aDaemonBehaviour );
    CleanupStack::PushL(self);
    self->ConstructL( aMainStatus ); // For uninstaller
    CleanupStack::Pop(self);
    return self;
    }
	
// Install Request constructor
	
CSisInstaller::CSisInstaller( MDaemonInstallBehaviour* aDaemonBehaviour ) 
    : CActive(CActive::EPriorityStandard),
    iDaemonBehaviour( aDaemonBehaviour ),
    iState(EDSisInstallerStateIdle),
    iFileIndex(0),
    iInstallErr( KErrNone )
    {
    CActiveScheduler::Add(this);
#ifdef RD_MULTIPLE_DRIVE      
    iInstallerState = EDSisInstallerStateIdle;
#endif       
    }
	
// Install Request destructor

CSisInstaller::~CSisInstaller()
    {
    Cancel();
    iTimer.Close();
    iFilesToInstall.ResetAndDestroy();
    iFilesToInstall.Close();
    delete iInstallLauncher;
    delete iDialogs;  
    iFs.Close();
    iApaSession.Close();    
    delete iPreviouslyInstalledAppsCache;
    delete iInstallationFailedAppsCache;  
    delete iShutdownWatcher;  
    }
	
// 2nd phase construction

void CSisInstaller::ConstructL( CProgramStatus& aMainStatus )
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iApaSession.Connect() );
    iFs.ShareProtected();
    iDialogs = CDialogWrapper::NewL( iFs );
    iPreviouslyInstalledAppsCache = CPreviouslyInstalledAppsCache::NewL();
    iInstallationFailedAppsCache = CInstallationFailedAppsCache::NewL();
    iShutdownWatcher = CShutdownWatcher::NewL( *this );
    iShutdownWatcher->Start();
    // For uninstaller
    // SisInstaller do not own this so do not delete.     
    iProgramStatus = &aMainStatus;  
    iUpdateCache = ETrue; 
    // For first boot issues. See FirstBootCheck function.
    iFirstBootDisableNotes = EFalse;    
    }
		
// Set the location of all sis files and the list of them
// also take ownership of the pointers to memory
		
void CSisInstaller::AddFileToInstallL(const TDesC& aFileName)
    {
    HBufC* fileName = aFileName.AllocLC();
    iFilesToInstall.AppendL( fileName );
    CleanupStack::Pop( fileName );
    }
	
// Start the request to process the Sisx file

void CSisInstaller::StartInstallingL()
    {
    FLOG( _L("Daemon: StartInstallingL") );

    if(!iFilesToInstall.Count())
        {
        // For uninstaller
        // Check state, if installing change it to idle.
        if ( EStateInstalling == iProgramStatus->GetProgramStatus() )
            {
            FLOG( _L("Daemon: StartInstallingL: Set EStateIdle") );
            iProgramStatus->SetProgramStatusToIdle();
            }        
        
        User::Leave(KErrAbort);
        }
    
    // For uninstaller        
    // Check state. If idle chage it to installing, but if installing 
    // or uninstalling do nothing. 
    if ( EStateIdle == iProgramStatus->GetProgramStatus() )
        {
        FLOG( _L("Daemon: StartInstallingL: Set EStateInstalling") );
        iProgramStatus->SetProgramStatus( EStateInstalling );
        }         
    FLOG_1( _L("[CSisInstaller] ConstructL iGeneralProcessStatus = %d"),
            iGeneralProcessStatus );  
        
    if ( !iInstallLauncher )
        {
        iInstallLauncher = CSilentLauncher::NewL( iFs );
        }   

    if ( iState == EDSisInstallerStateIdle )
        {
        // Reset the error
        iInstallErr = KErrNone;        
        CompleteSelf();
        }
    }
    
#ifdef RD_MULTIPLE_DRIVE 
// Returns state of Installer. 

TBool CSisInstaller::IsInstalling()
    {
    if ( iInstallerState == EDSisInstallerStateCompleted )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }    
    }  
#endif //RD_MULTIPLE_DRIVE       

// Complete the request manually
		
void CSisInstaller::CompleteSelf()
    {
    if ( !IsActive() )
        {        
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status,KErrNone);
        SetActive();
        }    
    }

// Cancel the active request

void CSisInstaller::DoCancel()
    {
    iTimer.Cancel();

    iFileIndex = 0;
    iFilesToInstall.ResetAndDestroy();
    if ( iState == EDSisInstallerStateInstalling )
        {                    
        iInstallLauncher->Cancel();
        }

    // Note that this will also signal plugin that
    // Daemon part is completed. However uninstaller
    // may run since program status is set to idle.    
    InstallationCompleted( KErrNone );
    
    // For uninstaller
    // Set Installer to idle
    if ( EStateInstalling == iProgramStatus->GetProgramStatus() )
        {
        FLOG( _L("Daemon: DoCancel: Set EStateIdle") );
        iProgramStatus->SetProgramStatusToIdle();
        }    
    }
	
// When the software installer has changed state
// attemp to install a sisx file

void CSisInstaller::RunL()
    {
    FLOG_2( _L("Daemon: Installer RunL status:%d, state:%d"), iStatus.Int(), iState );                
    
    // For uninstaller
    // Check that uninstaller is not running.
    if ( EStateUninstalling == iProgramStatus->GetProgramStatus() )
        {
        // If uninstaller is running already let's wait.
        FLOG( _L("Daemon: RunL: EStateUninstalling -> wait") );                        
        TTimeIntervalMicroSeconds32 time( KWaitUninstallerTime );                        
        iTimer.After(iStatus,time);          
        // Set to idle, installer has not yet started.
        iState = EDSisInstallerStateIdle;
 #ifdef RD_MULTIPLE_DRIVE                
        iInstallerState = iState;
 #endif                                
        SetActive();                                
        }    
    else
        {
        // Ok uninstaller is in idle, run installing.
        iProgramStatus->SetProgramStatus( EStateInstalling );
        FLOG( _L("Daemon: RunL: Set EStateInstalling") ); 
                       
        switch (iState)
            {      
            // Reached when installation is completed
            case EDSisInstallerStateInstalling:
                FLOG( _L("Daemon: RunL: EDSisInstallerStateInstalling") );
                FLOG_1( _L("Daemon: Installation completed with %d"), iStatus.Int() );                    
                // Installation is completed, check result
                
                if ( iStatus.Int() == SwiUI::KSWInstErrBusy )
                    {
                    FLOG( _L("Daemon: RunL: iStatus: KSWInstErrBusy") );
                    // User might be installing something, wait before retrying 
                    TTimeIntervalMicroSeconds32 time( KInstallRetryWaitTime );                        
                    iTimer.After(iStatus,time);
                    iState = EDSisInstallerStateInstallerBusy;
    #ifdef RD_MULTIPLE_DRIVE                
                    iInstallerState = iState;
    #endif                                
                    SetActive();                        
                    break;                        
                    }                
                else if ( (iStatus.Int() == SwiUI::KSWInstErrSecurityFailure && iInstallErr == KErrNone) ||
                          (iStatus.Int() != KErrNone && iStatus.Int() != SwiUI::KSWInstErrSecurityFailure) )
                    {
                    FLOG( _L("Daemon: RunL: iStatus: KSWInstErrSecurityFailure or error") );
                    iInstallErr = iStatus.Int();                        
                    }
                    
                // Catch all installation error from SwiUI and update cache.              
                if ( iStatus.Int() != KErrNone )
                    {                                
                    FLOG( _L("Daemon: RunL: Installation error.") );
                    if ( iCurrentPackageId != TUid::Uid( NULL ) )
                        {
                        FLOG( _L("Daemon: RunL: Add UID to cache.") );                    
                        iInstallationFailedAppsCache->AddPackageUID( iCurrentPackageId ); 
                        // Clear current UID                    
                        iCurrentPackageId = TUid::Null();                     
                        }
                    }  
                 else
                    {
                    FLOG( _L("Daemon: RunL: Installation OK.") );
                    if ( iCurrentPackageId != TUid::Uid( NULL ) )
                        {                
                        FLOG( _L("Daemon: RunL: Add UID to cache.") );
                        iPreviouslyInstalledAppsCache->UpdateAddL( iCurrentPackageId );                        
                        // Clear current UID                    
                        iCurrentPackageId = TUid::Null(); 
                        }
                    }                                                               
    
                // Check if we have more to install
                if ( iFileIndex < iFilesToInstall.Count() )
                    {
                    // Kick of the next installation
                    iState = EDSisInstallerStateIdle;
    #ifdef RD_MULTIPLE_DRIVE                
                    iInstallerState = iState;
    #endif                
                    
                    CompleteSelf();
                    }   
                else
                    {
                    // Note that this will also signal plugin that
                    // Daemon part is completed. However uninstaller
                    // may run since program status is set to idle.
                    InstallationCompleted( iInstallErr );                     
                    }                    
                break;
    			
                // Install a file
            case EDSisInstallerStateIdle:
                {  
                FLOG( _L("Daemon: RunL: EDSisInstallerStateIdle") );    
                
                // Let's update installed apps cache so we do not give
                // installed pacakges several time to plug-in. 
                if ( iUpdateCache && iDaemonBehaviour->IsPlugin() )
                    {
                    FLOG( _L("Daemon: RunL: Update installed cache") );
                    TRAP_IGNORE( iPreviouslyInstalledAppsCache->UpdateAllL() );
                    // This is needed only once in session.
                    // InstallationCompleted will set this ETrue again.
                    iUpdateCache = EFalse;
                    }
                
                if ( iFileIndex < iFilesToInstall.Count() )
                    {             
                    iSisFile.Copy( *iFilesToInstall[iFileIndex] );
                    ++iFileIndex;
                                        
                    // Check if this is first boot and does device have eMMC.
                    // If first boot and eMMC is found disable wait dialog.
                    FirstBootCheck();            
                                                                                                                                                                                        
                    // No need to install if the package has been installed 
                    // some time in the past
                    if ( IsValidPackageL( iSisFile ) && NeedsInstallingL( iSisFile ) )
                        { 
                        // If there is plugin for SWI Daemon then let's us it.
                        // Daemon will give all files to plug-in which will 
                        // handle installation. There is not feedback so SWI 
                        // Daemon will not wait plug-in.
                        if ( iDaemonBehaviour->IsPlugin() )
                            {
                            // Notify plug-in if not yet done.                                                                 
                            iDaemonBehaviour->NotifyPlugin();
                            FLOG_1( _L("Daemon: Use plugin to install: %S"), &iSisFile );                             
                            TRAP_IGNORE( iDaemonBehaviour->RequestPluginInstall( iSisFile ) );                                                                                  
                            
                            // Let's continue to give all packages to plug-in. 
                            // Note that we do not have iStatus as this is not async. call 
                            // so we can not use EDSisInstallerStateInstalling state.                             
                            iState = EDSisInstallerStateIdle;                           
        #ifdef RD_MULTIPLE_DRIVE                
                            iInstallerState = iState;
        #endif           
                            // Plugin interface is not asyncronous. We need to 
                            // complete self to get all packages to plugin.                            
                            CompleteSelf(); 
                            }                            
                        else
                            {                                                
                            FLOG_1( _L("Daemon: Kick off the install for %S"), &iSisFile );
                            iInstallLauncher->InstallL( iSisFile, iStatus );
                            iDialogs->ShowWaitingNoteL(); 
                            iState = EDSisInstallerStateInstalling;
        #ifdef RD_MULTIPLE_DRIVE                
                            iInstallerState = iState;
        #endif                                            
                            SetActive();
                            }
                        }               
                    else
                        {                  
                        FLOG_1( _L("Daemon: No need to install %S"), &iSisFile );                                
                        iState = EDSisInstallerStateIdle;
    #ifdef RD_MULTIPLE_DRIVE                
                        iInstallerState = iState;
    #endif                    
                        // Clear current pkg UID                    
                        iCurrentPackageId = TUid::Null();                                   
                        
                        CompleteSelf();                    
                        }                        
                    }
                else
                    {
                    // Note that this will also signal plugin that
                    // Daemon part is completed. However uninstaller
                    // may run since program status is set to idle.                    
                    InstallationCompleted( iInstallErr );                                       
                    }                        
                }                        
                break;
                
            case EDSisInstallerStateInstallerBusy:
                // Try to install the file again
                FLOG( _L("Daemon: RunL: EDSisInstallerStateInstallerBusy") );  
                FLOG_1( _L("Daemon: Kick off the install for %S"), &iSisFile );
                iInstallLauncher->InstallL( iSisFile, iStatus );
                iState = EDSisInstallerStateInstalling;
    #ifdef RD_MULTIPLE_DRIVE                
                iInstallerState = iState;
    #endif                
                
                SetActive(); 
                break;                       
                
                // Active object in unknown state
            default:
                User::Panic(KMCSisInstaller,KErrNotSupported);
                break;
            }
        } // else for uninstaller
    }
	
// If RunL leaves then ignore errors

TInt CSisInstaller::RunError(TInt aError)
    {
    FLOG_1( _L("Daemon: Installer RunL error %d"), aError );                
    TInt err( KErrNone );
    InstallationCompleted( aError );                
    return err;
    }	
    
void CSisInstaller::NotifyShuttingDown()
    {
    // System is closing down, we need to stop installations and save
    // the lists of previously installed apps and failed installations.
    // Application server receives EApaSystemEventShutdown event that
    // closes it down.
    Cancel();
    }    	

void CSisInstaller::InstallationCompleted( TInt aResult )
    {
    FLOG_1( _L("Daemon: InstallationCompleted with result = %d"), aResult );  
    iState = EDSisInstallerStateIdle;    
    iInstallErr = KErrNone;    
    delete iInstallLauncher;
    iInstallLauncher = NULL;
    // We need to update cache again in RunL if plug-in is loaded.
    // It may be that plug-in does install packages after cache is updated.
    iUpdateCache = ETrue;
    
#ifdef RD_MULTIPLE_DRIVE 
    // If all files are installed set status to completed.
    if ( iFileIndex >= iFilesToInstall.Count() )
        {                
        iInstallerState = EDSisInstallerStateCompleted; 
        }
#endif                          
   
    TRAP_IGNORE( iDialogs->CancelWaitingNoteL() );
    
    if ( aResult != KErrNone && 
         iSisFile.Length() > 0 && 
         IsMediaPresent( TChar( iSisFile[0] ) ) )
        {        
        if ( aResult == SwiUI::KSWInstErrSecurityFailure )
            {
            TRAP_IGNORE( iDialogs->ShowUntrustedResultL() );
            }
        else 
            {
            TRAP_IGNORE( iDialogs->ShowErrorResultL() );
            }    
        }
    			    
    FLOG( _L("Daemon: InstallationCompleted: Update installed cache") );
    // Update cache so we do not start to install those packages 
    // which are installed by the user manyally. 
    // NOTE! plugin will install stuff after this call.
    TRAP_IGNORE(iPreviouslyInstalledAppsCache->UpdateAllL());
    
    TRAP_IGNORE(iPreviouslyInstalledAppsCache->FlushToDiskL(););    
    TRAP_IGNORE(iInstallationFailedAppsCache->FlushToDiskL());   
    
    // Notify plugin that daemon has complete installation.
    // Note that program status is set to idle for uninstaller.
    // Plugin needs to wait uninstaller if it starts before.
    iDaemonBehaviour->DoNotifyMediaProcessingComplete();  
        
    // For uninstaller
    // Set Installer to idle, if state is installing    
     if ( EStateInstalling == iProgramStatus->GetProgramStatus() )
          {
          FLOG( _L("Daemon: InstallationCompleted: Set EStateIdle") );
          iProgramStatus->SetProgramStatusToIdle();
          }
     
     if ( iFirstBootDisableNotes )
          {
          // All is done in first boot, set flag to false.
          iFirstBootDisableNotes = EFalse;
          // Allow to show dialogs when needed.
          iDialogs->DisableInstallNotes( EFalse );
          }
     FLOG_1( _L("Daemon: InstallationCompleted iFirstBootDisableNotes = %d"), 
             iFirstBootDisableNotes ); 
    }  

// Indicates if this package is installed or not.
	
TBool CSisInstaller::NeedsInstallingL( const TDesC& aPackageName )
    {
    FLOG( _L("Daemon: NeedsInstallingL") ); 
    TBool result( ETrue );
    
    // Read the controller data from the package
    CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC( iFs, aPackageName ); 
    TInt64 pos( 0 );                            
    fileProvider->Seek( ESeekStart, pos );                            
    Swi::Sis::CContents* content = Swi::Sis::Parser::ContentsL( *fileProvider );
    CleanupStack::PushL( content );                           

    HBufC8* controller = content->ReadControllerL();                                          
    CleanupStack::PushL( controller );                            

    // Code to read UID

	CDesDataProvider* controllerProvider= CDesDataProvider::NewLC(*controller);
	CController* controllerObject = NULL;
	controllerObject = CController::NewL(*controllerProvider);
	CleanupStack::PushL(controllerObject);

	TUid packageId = controllerObject->Info().Uid().Uid();

	CleanupStack::PopAndDestroy(controllerObject);
	CleanupStack::PopAndDestroy(controllerProvider);

	FLOG_1( _L("Daemon: NeedsInstallingL: Is UID installed = 0x%x"), packageId.iUid );
	result = !iPreviouslyInstalledAppsCache->HasBeenPreviouslyInstalled(packageId);
	FLOG_1( _L("Daemon: NeedsInstallingL: Has been installed (cache) = %d"), !result ); 
	
	if (result)
	    {
        RSisRegistrySession registry;
        User::LeaveIfError( registry.Connect() );  
        CleanupClosePushL( registry );  
        result = !registry.IsInstalledL( packageId );      
          
        FLOG_1( _L("Daemon: NeedsInstallingL: Is installed = %d"), !result );         
        // Check if binary is installed in ROM. If pkg is in ROM, it is stub sis. 
        // Continue installation.
        if ( !result )
            {
            RSisRegistryEntry entry;
            User::LeaveIfError( entry.Open( registry, packageId ) );
            CleanupClosePushL( entry ); 
            
            result = entry.IsInRomL();
            
            FLOG_1( _L("Daemon: NeedsInstallingL: Is in ROM = %d"), result );              
            CleanupStack::PopAndDestroy( &entry );           
            }
        CleanupStack::PopAndDestroy( &registry ); 
		}
		
    // Check that previous install attempt did not fail.		
	if (result)
	    {
	    result = !iInstallationFailedAppsCache->HasPreviousInstallationFailed( packageId );
	    FLOG_1( _L("Daemon: NeedsInstallingL: Has failed (cache) = %d"), !result ); 
	    }

    // Update pkg ID. ID is added to cache after installation.
    if (result)
        {        
        iCurrentPackageId = packageId;
        }
    
    CleanupStack::PopAndDestroy( 3 );
        
    return result;            
    }  

TBool CSisInstaller::IsMediaPresent( TChar aDrive )
    {
    TInt drive( 0 );
    TInt err = iFs.CharToDrive( aDrive, drive );
    if ( err == KErrNone )
        {     
        TVolumeInfo volumeInfo;
        err = iFs.Volume( volumeInfo, drive );
        }
    
    if ( err == KErrNone )
        {
        return ETrue;
        }
    else
        {
        FLOG( _L("Daemon: Drive no present") );
        return EFalse;        
        }
    }

TBool CSisInstaller::IsValidPackageL( const TDesC& aPackageName )
    {
    TBool result( EFalse );
  
    RFile file;
    TUid appUid;
    TDataType dataType;
    User::LeaveIfError( file.Open( iFs, aPackageName, EFileRead ) );        
    iApaSession.AppForDocument( file, appUid, dataType );
    file.Close();
    
    if ( dataType.Des8() == SwiUI::KSisxMimeType )
        {
        result = ETrue;
        }

    return result;    
    }

void CSisInstaller::FirstBootCheck()
    { 
    FLOG( _L("Daemon: FirstBootCheck") );
    // TB9.2 project (PR1) has request that SWI Daemon should not show
    // dialogs in first boot if device has eMMC. Reason is that wait 
    // dialog is shown too long time top of UI, since eMMC contains
    // several pre-installed applications.
    // Note Unintall dialogs are shown in boot.
            
    iFirstBootDisableNotes = EFalse;    
    TInt iFirstBoot = 0;
    
    // Check first, that drive is E. 
    // If drive is e.g. F no need to check eMMC issue.
    
    if ( TChar( iSisFile[0] ) == 'e' || TChar( iSisFile[0] ) == 'E' )
        {   
        FLOG( _L("Daemon: FirstBootCheck: Drive is E - check boot reason") );
        FLOG( _L("Daemon: FirstBootCheck: Get startup reason") );
        // Get startup key.
        RProperty::Get( KPSUidStartup, KPSStartupFirstBoot, iFirstBoot );   
        
        FLOG_1( _L("Daemon: FirstBootCheck: iFirstBoot = %d"), iFirstBoot );
        
        if ( iFirstBoot == EPSStartupFirstBoot )
            { 
            FLOG( _L("Daemon: FirstBootCheck: EPSStartupFirstBoot") );
            TDriveInfo iDriveInfo;  
            if( iFs.Drive( iDriveInfo, EDriveE ) == KErrNone )
                {
                // If E drive is internal we have eMMC.
                if ( iDriveInfo.iDriveAtt & KDriveAttInternal )
                    {
                    iFirstBootDisableNotes = ETrue;
                    iDialogs->DisableInstallNotes( ETrue ); 
                    FLOG( _L("Daemon: FirstBootCheck: eMMC found, disable notes") );                
                    }
                }        
            }
        }
    else
        {
        iDialogs->DisableInstallNotes( EFalse ); 
        FLOG( _L("Daemon: FirstBootCheck: Enable UI notes") ); 
        }
       
    FLOG_1( _L("Daemon: FirstBootCheck: iFirstBootDisableNotes = %d"), 
            iFirstBootDisableNotes );      
    }
    
//EOF





       
