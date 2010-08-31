/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


// -----------------------------------------------------------------------
// Two phased construction
// -----------------------------------------------------------------------
// 
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
	
// -----------------------------------------------------------------------
// Install Request constructor
// -----------------------------------------------------------------------
//	
CSisInstaller::CSisInstaller( MDaemonInstallBehaviour* aDaemonBehaviour ) 
    : CActive(CActive::EPriorityStandard),
    iDaemonBehaviour( aDaemonBehaviour ),
    iState(EDSisInstallerStateIdle),
    iFileIndex(0),
    iInstallErr( KErrNone )
    {
    CActiveScheduler::Add(this);    
    iInstallerState = EDSisInstallerStateIdle;       
    }
	
// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
//
CSisInstaller::~CSisInstaller()
    {
    FLOG( _L("Daemon: CSisInstaller::~CSisInstaller") );
    Cancel();
    iTimer.Close();
    iFilesToInstall.ResetAndDestroy();
    iFilesToInstall.Close();    
    delete iInstallLauncher;
    delete iDialogs;      
    delete iPreviouslyInstalledAppsCache;
    delete iInstallationFailedAppsCache;  
    delete iShutdownWatcher;
    
    if ( iFileOpen )
        {
        iSisFileHandle.Close();   
        }    
    
    iFs.Close();
    iApaSession.Close();        
    }
	
// -----------------------------------------------------------------------
// 2nd phase construction
// -----------------------------------------------------------------------
//
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
    iPercentValue = 0;
    
    // For uninstaller
    // SisInstaller do not own this so do not delete.     
    iProgramStatus = &aMainStatus;  
    iUpdateCache = ETrue;    
    iFileOpen = EFalse;    
    iInstallLauncher = NULL;
    }
		
// -----------------------------------------------------------------------
// CSisInstaller::AddFileToInstallL
// Set the location of all sis files and the list of them
// also take ownership of the pointers to memory
// -----------------------------------------------------------------------
//		
void CSisInstaller::AddFileToInstallL(const TDesC& aFileName)
    {
    HBufC* fileName = aFileName.AllocLC();
    iFilesToInstall.AppendL( fileName );
    CleanupStack::Pop( fileName );
    }
	

// -----------------------------------------------------------------------
// CSisInstaller::StartInstallingL
// Start the request to process the Sisx file
// -----------------------------------------------------------------------
//
void CSisInstaller::StartInstallingL()
    {
    FLOG( _L("Daemon: StartInstallingL") );

    if( !iFilesToInstall.Count() )
        {
        // For uninstaller
        // Check state, if installing change it to idle.
        if ( EStateInstalling == iProgramStatus->GetProgramStatus() )
            {
            FLOG( _L("Daemon: StartInstallingL: Set EStateIdle") );
            iProgramStatus->SetProgramStatusToIdle();
            }        
        FLOG( _L("Daemon: StartInstallingL: User::Leave(KErrAbort)") );
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
    
    FLOG_1( _L("Daemon: iGeneralProcessStatus: %d"), iGeneralProcessStatus );     
    FLOG_1( _L("Daemon: iInstallLauncher: 0x%x"),iInstallLauncher );  
        
    if ( iInstallLauncher == NULL )
        {
        FLOG( _L("Daemon: Create iInstallLauncher") );
        iInstallLauncher = CSilentLauncher::NewL( iFs );   
        
        // Update cache so we do not try to start install for
        // components which are installed. This is done always
        // when new install session is started (e.g. after mmc insert).
        FLOG( _L("Daemon: StartInstallingL: Update installed cache") );
        TRAP_IGNORE( iPreviouslyInstalledAppsCache->UpdateAllL() );
        TRAP_IGNORE( iPreviouslyInstalledAppsCache->FlushToDiskL() );         
        }   

    if ( iState == EDSisInstallerStateIdle )
        {                
        // Reset the error
        iInstallErr = KErrNone;           
        CompleteSelf();
        }
    }
    
// -----------------------------------------------------------------------
// CSisInstaller::IsInstalling
// Returns state of Installer. 
// -----------------------------------------------------------------------
//
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
       
// -----------------------------------------------------------------------
// CSisInstaller::CompleteSelf
// Complete the request manually
// -----------------------------------------------------------------------
//		
void CSisInstaller::CompleteSelf()
    {
    FLOG( _L("Daemon: CSisInstaller::CompleteSelf") );
    if ( !IsActive() )
        {        
        TRequestStatus* status = &iStatus;
        FLOG( _L("Daemon: CompleteSelf: RequestComplete") );
        User::RequestComplete( status, KErrNone );
        FLOG( _L("Daemon: CompleteSelf: SetActive") );
        SetActive();
        }    
    }

// -----------------------------------------------------------------------
// CSisInstaller::DoCancel
// Cancel the active request
// -----------------------------------------------------------------------
//
void CSisInstaller::DoCancel()
    {
    FLOG( _L("Daemon: CSisInstaller::DoCancel") ); 
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
	
// -----------------------------------------------------------------------
// CSisInstaller::RunL
// When the software installer has changed state attemp to install a 
// sisx file
// -----------------------------------------------------------------------
//
void CSisInstaller::RunL()
    {    
    FLOG_1( _L("Daemon: Installer RunL status:%d"), iStatus.Int() );  
    FLOG_1( _L("Daemon: Installer RunL state:%d"), iState );                
    
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
        iInstallerState = iState;                                 
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
                FLOG_1( _L("Daemon: Installation completed with %d"), 
                        iStatus.Int() );                  
                
                // Installation is completed, check result                
                if ( iStatus.Int() == SwiUI::KSWInstErrBusy )
                    {
                    FLOG( _L("Daemon: RunL: iStatus: KSWInstErrBusy") );
                    // User might be installing something, wait before retrying 
                    TTimeIntervalMicroSeconds32 time( KInstallRetryWaitTime );                        
                    iTimer.After(iStatus,time);
                    iState = EDSisInstallerStateInstallerBusy;               
                    iInstallerState = iState;                                  
                    SetActive();                        
                    break;                        
                    }                
                else if ( (iStatus.Int() == SwiUI::KSWInstErrSecurityFailure && 
                           iInstallErr == KErrNone) ||
                          (iStatus.Int() != KErrNone && 
                           iStatus.Int() != SwiUI::KSWInstErrSecurityFailure) )
                    {
                    FLOG( _L("Daemon: RunL: iStatus: error of sec. failure") );
                    iInstallErr = iStatus.Int();                        
                    }
                
                // Close current sisx except if installer engine is busy.
                // If installer is busy we will try again later.
                if ( iStatus.Int() != SwiUI::KSWInstErrBusy )
                    {
                    if ( iFileOpen )
                         {
                         FLOG_1( _L("Daemon: RunL: Close File: %S"), &iSisFile );
                         iSisFileHandle.Close();
                         iFileOpen = EFalse;
                         }    
                    }
                    
                // Catch all installation error from SwiUI and update cache.              
                if ( iStatus.Int() != KErrNone )
                    {                                
                    FLOG( _L("Daemon: RunL: Installation error.") );
                    if ( iCurrentPackageId != TUid::Uid( NULL ) )
                        {
                        FLOG( _L("Daemon: RunL: Add UID to cache.") );                    
                        iInstallationFailedAppsCache->AddPackageUID( 
                                iCurrentPackageId ); 
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
                        iPreviouslyInstalledAppsCache->UpdateAddL( 
                                iCurrentPackageId );                        
                        // Clear current UID                    
                        iCurrentPackageId = TUid::Null(); 
                        }
                    }                                                               
    
                // Check if we have more to install
                if ( iFileIndex < iFilesToInstall.Count() )
                    {
                    // Kick of the next installation
                    iState = EDSisInstallerStateIdle;                  
                    iInstallerState = iState;                                       
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
                
                // Make sure that current file is closed before we
                // open new file handle.
                if ( iFileOpen )
                    {
                    FLOG( _L("Daemon: RunL: StateIdle: CLOSE CURRENT FILE") );
                    FLOG_1( _L("Daemon: RunL: Close File: %S"), &iSisFile );
                    iSisFileHandle.Close();
                    iFileOpen = EFalse;
                    }    
                
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
                    // Let's calc. values before index is updated.
                    CalcPercentValue();
                                                       
                    // Get next sisx package from array.                 
                    iSisFile.Copy( *iFilesToInstall[iFileIndex] );
                    ++iFileIndex;    
                    
                    FLOG_1( _L("Daemon: RunL: Open File: %S"), &iSisFile );
                    TInt err = KErrNone;
                    // Let's open the file in here bacause IsValidPackageL
                    // needs to open the file anyway.
                    err = iSisFileHandle.Open( 
                                       iFs, 
                                       iSisFile, 
                                       EFileRead | EFileShareReadersOnly );                    
                    
                    if ( err )
                        {
                        FLOG_1( _L("Daemon: File open ERROR = %d"), err );
                        iFileOpen = EFalse;                        
                        } 
                    else
                        {
                        iFileOpen = ETrue;                    
                        }
                    
                    if ( iFileOpen && IsValidPackageL() && 
                            NeedsInstallingL( iSisFile ) )
                        {                                           
                        // If there is plugin for SWI Daemon then let's us it.
                        // Daemon will give all files to plug-in which will 
                        // handle installation. There is not feedback so SWI 
                        // Daemon will not wait plug-in.
                        if ( iDaemonBehaviour->IsPlugin() )
                            {
                            // Notify plug-in if not yet done.                                                                 
                            iDaemonBehaviour->NotifyPlugin();
                            FLOG_1( _L("Daemon: Use plugin to install: %S"), 
                                    &iSisFile );                             
                            TRAP_IGNORE( iDaemonBehaviour->RequestPluginInstall( 
                                    iSisFile ) );                                                                                  
                            
                            // Let's continue to give all packages to plug-in. 
                            // Note that we do not have iStatus as this is not 
                            // async. call so we can not use 
                            // EDSisInstallerStateInstalling state.                             
                            iState = EDSisInstallerStateIdle;                                                 
                            iInstallerState = iState;
                
                            // Plugin interface is not asyncronous. We need to 
                            // complete self to get all packages to plugin.                            
                            CompleteSelf(); 
                            }                            
                        else
                            {  
                            // Start also the universal indicator.
                            TRAP_IGNORE( iDialogs->ActivateIndicatorL( iPercentValue ) );
                            // Start to show progress dialog. Dialog is shown 
                            // only 3 sec. 
                            TRAP_IGNORE( iDialogs->ShowWaitingNoteL() );    
                                                        
                            FLOG_1( _L("Daemon: Start install for %S"), &iSisFile );
                                                                                                                                                                                                                                                   
                            iInstallLauncher->InstallL( iSisFileHandle, 
                                                        iSisFile, 
                                                        iStatus );
                                                        
                            iState = EDSisInstallerStateInstalling;                       
                            iInstallerState = iState;                                                   
                            SetActive();                               
                            }
                        }               
                    else
                        {                  
                        FLOG_1( _L("Daemon: NOT INSTALLING: %S"), &iSisFile );                                                 
                        iState = EDSisInstallerStateIdle;                
                        iInstallerState = iState;                   
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
                {
                // Try to install the file again
                FLOG( _L("Daemon: RunL: EDSisInstallerStateInstallerBusy") ); 
                                
                // If file is not open, try to open it.
                if ( !iFileOpen )
                    {
                    FLOG( _L("Daemon: RunL: Error file not open !") ); 
                    FLOG_1( _L("Daemon: Open File: %S"), &iSisFile );
                
                    TInt err = iSisFileHandle.Open( 
                                    iFs, 
                                    iSisFile, 
                                    EFileRead | EFileShareReadersOnly ); 
                                                           
                    if ( err )
                         {
                         FLOG_1( _L("Daemon: File open ERROR = %d"), err );
                         iFileOpen = EFalse;
                         }
                    else
                        {
                        FLOG( _L("Daemon: RunL: File open") );                     
                        iFileOpen = ETrue;                    
                        }
                    }
                
                if ( iFileOpen )
                    {
                    FLOG_1( _L("Daemon: Try install again for: %S"), &iSisFile );
                    iInstallLauncher->InstallL( iSisFileHandle,
                                                iSisFile, 
                                                iStatus );                
                
                    iState = EDSisInstallerStateInstalling;                   
                    iInstallerState = iState;                                   
                    SetActive(); 
                    }
                else
                    {
                    // If we cannot open the sis file let's continue
                    // and install rest of the packages.
                    // We can try to install this next time in boot 
                    // or when media is mounted.
                    iState = EDSisInstallerStateIdle;                                                 
                    iInstallerState = iState;  
                    CompleteSelf(); 
                    } 
                }
                break;                       
                
                // Active object in unknown state
            default:
                User::Panic(KMCSisInstaller,KErrNotSupported);
                break;
            }
        } // else for uninstaller
    FLOG( _L("Daemon: RunL END") ); 
    }
	
// -----------------------------------------------------------------------
// CSisInstaller::RunError
// If RunL leaves then ignore errors
// -----------------------------------------------------------------------
//
TInt CSisInstaller::RunError(TInt aError)
    {
    FLOG_1( _L("Daemon: Installer Run error %d"), aError );                
    TInt err( KErrNone );
    InstallationCompleted( aError );                
    return err;
    }	

// -----------------------------------------------------------------------
// CSisInstaller::NotifyShuttingDown
// -----------------------------------------------------------------------
//    
void CSisInstaller::NotifyShuttingDown()
    {
    // System is closing down, we need to stop installations and save
    // the lists of previously installed apps and failed installations.
    // Application server receives EApaSystemEventShutdown event that
    // closes it down.
    Cancel();
    }   

// -----------------------------------------------------------------------
// CSisInstaller::InstallationCompleted
// -----------------------------------------------------------------------
//
void CSisInstaller::InstallationCompleted( TInt aResult )
    {
    // Let's update universal indicator ones more.   
    iDialogs->ActivateIndicatorL( 100 );
    
    FLOG_1( _L("Daemon: InstallationCompleted with result = %d"), aResult );  
    iState = EDSisInstallerStateIdle;    
    iInstallErr = KErrNone;     
    FLOG( _L("Daemon: InstallationCompleted: Delete iInstallLauncher") );
    delete iInstallLauncher;
    iInstallLauncher = NULL;
    // We need to update cache again in RunL if plug-in is loaded.
    // It may be that plug-in does install packages after cache is updated.
    iUpdateCache = ETrue;
    
    // If all files are installed set status to completed.
    if ( iFileIndex >= iFilesToInstall.Count() )
        {                
        iInstallerState = EDSisInstallerStateCompleted; 
        }
    
    // Make sure that current file is closed before exit.
    // File may be open if this is called from RunError/DoCancel etc.
    if ( iFileOpen )
        {
        FLOG( _L("Daemon: InstallationCompleted: File open - Close it !!!") );
        iSisFileHandle.Close();
        iFileOpen = EFalse;
        }    
    
    // Make sure that progress note is closed.
    TRAP_IGNORE( iDialogs->CancelWaitingNote() );
    // Close the universal indicator. 
    iDialogs->CancelIndicatorL();
        
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
    }  

// -----------------------------------------------------------------------
// CSisInstaller::NeedsInstallingL
// Indicates if this package is installed or not.
// -----------------------------------------------------------------------
//	
TBool CSisInstaller::NeedsInstallingL( const TDesC& aPackageName )
    {
    FLOG( _L("Daemon: CSisInstaller::NeedsInstallingL") ); 
    //TBool result( ETrue );
    TBool needsInstalling( ETrue );
    
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

	CleanupStack::PopAndDestroy( controllerObject );
	CleanupStack::PopAndDestroy( controllerProvider );

	// Check if sw is installed previously. 
	// Note if UID is found sw will not be installed even if user 
	// has uninstall it bacause cache is not updated from SCR.
	FLOG_1( _L("Daemon: Is UID installed = 0x%x"), packageId.iUid );
	needsInstalling = !iPreviouslyInstalledAppsCache->
                            HasBeenPreviouslyInstalled(packageId);
	FLOG_1( _L("Daemon: Has been installed (cache) = %d"), !needsInstalling );
	
	// No need to check rom stubs in here anymore. 
	// Note 1: SWI Daemon policy has been that RU packages are not installed 
	// from removable media.	
	// Note 2: UpdateAllL will add all pkg uids in cache (PreviouslyInstalled),
	// so rom upgrades are not installed since uid is found from the cache.
	// Note 3: Install params do not allow RU to be installed. So SWI will
	// reject RU (rom upgrade) package anyway.

	/*	
	// Note! this code has been wrong. No need to check stubs since
	// policy do not allow RU updates and SWI will reject the install.
	if ( !needsInstalling )
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
            
            needsInstalling = entry.IsInRomL();
            
            FLOG_1( _L("Daemon: NeedsInstallingL: Is in ROM = %d"), needsInstalling );              
            CleanupStack::PopAndDestroy( &entry );           
            }
        CleanupStack::PopAndDestroy( &registry ); 
        }
	*/	
	
    // Check that previous install attempt did not fail.		
	if ( needsInstalling )
	    {
	    needsInstalling = !iInstallationFailedAppsCache->
	            HasPreviousInstallationFailed( packageId );
	    FLOG_1( _L("Daemon: Has failed (cache) = %d"), !needsInstalling ); 
	    }

    // Update pkg ID. ID is added to cache after installation.
    if ( needsInstalling )
        {        
        iCurrentPackageId = packageId;
        }
    
    // fileProvider, content, controller
    CleanupStack::PopAndDestroy( 3 );
    
    FLOG_1( _L("Daemon: NeedsInstallingL = %d"), needsInstalling );    
    return needsInstalling;            
    }  

// -----------------------------------------------------------------------
// CSisInstaller::IsMediaPresent
// -----------------------------------------------------------------------
//
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

// -----------------------------------------------------------------------
// CSisInstaller::IsValidPackageL
// -----------------------------------------------------------------------
//
TBool CSisInstaller::IsValidPackageL()
    {
    TBool result( EFalse ); 
    
    if ( !iFileOpen )
        {
        FLOG( _L("Daemon: IsValidPackageL: ERROR FILE NOT OPEN") );
        return result;
        }
    
    TUid appUid;
    TDataType dataType;
        
    iApaSession.AppForDocument( iSisFileHandle, appUid, dataType );
    
    if ( dataType.Des8() == SwiUI::KSisxMimeType )
        {
        result = ETrue;
        }           
           
    FLOG_1( _L("Daemon: IsValidPackageL = %d"), result );  
    return result;    
    }
 
// -----------------------------------------------------------------------
// CSisInstaller::CalcPrecentValue
// -----------------------------------------------------------------------
//
void CSisInstaller::CalcPercentValue()
    {  
    FLOG( _L("Daemon: CSisInstaller::CalcPercentValue") );
    FLOG_1( _L("Daemon: iFileIndex = %d"), iFileIndex ); 
    // Let's calculate indicator value for UI now.
    TInt sisxFileCount = iFilesToInstall.Count();
    FLOG_1( _L("Daemon: iFilesToInstall.Count = %d"), sisxFileCount ); 
    iPercentValue = 0;

    // Note! if iFileIndex is zero, no package is installed bacause
    // installation process starts after this function.     
    if ( iFileIndex && sisxFileCount )
        {
        // Let's calculate new precent value after some
        // package is installed.     
        if ( iFileIndex <= sisxFileCount )
            {
            TReal32 realFileIndex = iFileIndex;
            TReal32 realFileCount = sisxFileCount;
            iPercentValue = (realFileIndex/realFileCount)*100;                                   
            }
        else
            {
            // Most probably all is installed if index is bigger then
            // filen count. Let's not show over 100% to user.
            // This may happend after last package is processed since 
            // index counter is updated before install starts.
            iPercentValue = 100;
            }
        }           
    FLOG_1( _L("Daemon: CalcPercentValue value = %d"), (TInt)iPercentValue );
    }

//EOF





       
