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

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>

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
    // For uninstaller
    // SisInstaller do not own this so do not delete.     
    iProgramStatus = &aMainStatus;  
    iUpdateCache = ETrue;       
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
        // Update cache so we do not try to start install for
        // components which are installed.
        FLOG( _L("Daemon: StartInstallingL: Update installed cache") );
        TRAP_IGNORE( iPreviouslyInstalledAppsCache->UpdateAllL() );
        TRAP_IGNORE( iPreviouslyInstalledAppsCache->FlushToDiskL() );    
        
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
        User::RequestComplete(status,KErrNone);
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
                FLOG_1( _L("Daemon: Installation completed with %d"), iStatus.Int() );                    
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
                            iInstallerState = iState;
                
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
                            iInstallerState = iState;                                                   
                            SetActive();
                            }
                        }               
                    else
                        {                  
                        FLOG_1( _L("Daemon: No need to install %S"), &iSisFile );                                
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
                // Try to install the file again
                FLOG( _L("Daemon: RunL: EDSisInstallerStateInstallerBusy") );  
                FLOG_1( _L("Daemon: Kick off the install for %S"), &iSisFile );
                iInstallLauncher->InstallL( iSisFile, iStatus );
                iState = EDSisInstallerStateInstalling;                   
                iInstallerState = iState;                                   
                SetActive(); 
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
    FLOG_1( _L("Daemon: InstallationCompleted with result = %d"), aResult );  
    iState = EDSisInstallerStateIdle;    
    iInstallErr = KErrNone;    
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

//TODO: remove this update. This is done in start installing !!!!     
    //FLOG( _L("Daemon: InstallationCompleted: Update installed cache") );
    // Update cache so we do not start to install those packages 
    // which are installed by the user manyally. 
    // NOTE! plugin will install stuff after this call.
    //TRAP_IGNORE(iPreviouslyInstalledAppsCache->UpdateAllL());
    
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
	
// TODO: Onko rom stubien tarkistus tarpeen jos upgrade ei ole sallittu.
// Ei taida kannattaa tarksitaan RU ei pitäisi mennä läpi.
	FLOG( _L("Daemon: HUOM ROM STUBEJA EI TARKISTETA") ); 	
	/*	
	// Huom! edellinen koodi ollut väärin koska UID on jo cachessä !
	// Tarkistus pitää tehdä vain jos UID löytyy cachestä.
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

    FLOG_1( _L("Daemon: IsValidPackageL = %d"), result );  
    return result;    
    }
    
//EOF





       
