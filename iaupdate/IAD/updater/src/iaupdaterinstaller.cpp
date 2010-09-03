/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Installs self update. 
*
*/


#include "iaupdaterinstaller.h"
#include "iaupdatermngr.h"
#include "iaupdatersilentlauncher.h"
#include "iaupdaterdefs.h"
#include "iaupdatedebug.h"

#include <usif/usiferror.h>


// ======== LOCAL FUNCTIONS =========

// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::NewL
// Two phased construction 
// -----------------------------------------------------------------------------
//
CIAUpdaterInstaller* CIAUpdaterInstaller::NewL( 
    RFs& aFs,
    CIAUpdaterMngr& aIAUpdater )
    {
    CIAUpdaterInstaller* self = 
        new (ELeave) CIAUpdaterInstaller( aFs, aIAUpdater );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
	
 
// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::CIAUpdaterInstaller() 
// C++ constructor
// -----------------------------------------------------------------------------
//	
CIAUpdaterInstaller::CIAUpdaterInstaller( 
    RFs& aFs,
    CIAUpdaterMngr& aIAUpdater )
    : CActive( CActive::EPriorityStandard ),
    iFs( aFs ),
    iIAUpdater( aIAUpdater ),
    iState( EDSisInstallerStateIdle ),
    iInstallErr( KErrNone ),
    iFileIndex( 0 )       
    {
    CActiveScheduler::Add( this );
    }
	

// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::~CIAUpdaterInstaller()
// Install Request destructor
// -----------------------------------------------------------------------------
//
CIAUpdaterInstaller::~CIAUpdaterInstaller()
    {    
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::~CIAUpdaterInstaller begin");
    
    CancelOperation();
    
    iTimer.Close();
            
    iFilesToInstall.ResetAndDestroy();
    delete iInstallLauncher;
    
    delete iResults;

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::~CIAUpdaterInstaller end");
    }
	

// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::ConstructL()
// Constructior
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::ConstructL begin");       
    User::LeaveIfError( iTimer.CreateLocal() );
    iResults = Usif::COpaqueNamedParams::NewL();
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::ConstructL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::AddFileToInstallL()
// Set the path of all sis files. 
// -----------------------------------------------------------------------------
//		
void CIAUpdaterInstaller::AddFileToInstallL( const TDesC& aFileName )
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::AddFileToInstallL begin"); 
    HBufC* fileName = aFileName.AllocLC();
    IAUPDATE_TRACE_1("[IAUpdater] fileName: %S", fileName); 
    iFilesToInstall.AppendL( fileName );
    CleanupStack::Pop( fileName );
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::AddFileToInstallL end"); 
    }
	
	
// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::StartInstallingL()
// Start installing
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::StartInstallingL()
    {
    IAUPDATE_TRACE("[IAUpdater] Installer::StartInstallingL begin"); 
        
    if ( !iFilesToInstall.Count() )
        {
        IAUPDATE_TRACE_1("[IAUpdater] Error No file to install: %d", KErrAbort );
        User::Leave( KErrAbort );
        }

    if ( iState != EDSisInstallerStateIdle )
        {
        // Leave, because installation operation is already on.
        User::Leave( KErrInUse );
        }
        
    if ( !iInstallLauncher )
        {
        IAUPDATE_TRACE("[IAUpdater] Create install launcher");
        iInstallLauncher = CIAUpdaterSilentLauncher::NewL( iFs );
        }   


    // Reset the error
    IAUPDATE_TRACE("[IAUpdater] Reset error and complete self");
    iInstallErr = KErrNone;        
    CompleteSelf();


    // Notice, that this will not return before 
    // CActiveSchedulerWait::AsyncStop() is called.
    // So, active object may do its job above 
    // and the code will continue after they
    // are finished and AsyncStop is called.
    IAUPDATE_TRACE("[IAUpdater] Active scheduler start waiting begins");
    iWaiter.Start();
    IAUPDATE_TRACE("[IAUpdater] Active scheduler start waiting ends");

    IAUPDATE_TRACE("[IAUpdater] Installer::StartInstallingL end"); 
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::CompleteSelf()
// Complete the request manually 
// -----------------------------------------------------------------------------
//		
void CIAUpdaterInstaller::CompleteSelf()
    {
    IAUPDATE_TRACE("[IAUpdater] Installer::CompleteSelf begin");
    
    if ( !IsActive() )
        {        
        IAUPDATE_TRACE("[IAUpdater] SetActive");
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }    

    IAUPDATE_TRACE("[IAUpdater] Installer::CompleteSelf end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::CancelOperation()
// Cancel the active request and stop waiter.
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::CancelOperation()
    {
    IAUPDATE_TRACE("[IAUpdater] Installer::CancelOperation begin");

    // Use normal cancel to cancel possible active operation.
    Cancel();

	// Stop active scheduler 
    if ( iWaiter.IsStarted() )
        {
        IAUPDATE_TRACE("[IAUpdater] Stop waiter.");
        iWaiter.AsyncStop();            
        }

    IAUPDATE_TRACE("[IAUpdater] Installer::CancelOperation end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::DoCancel()
// Cancel the active request.
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::DoCancel()
    {    
    IAUPDATE_TRACE("[IAUpdater] Installer::DoCancel begin");
    
    iTimer.Cancel();

    iFileIndex = 0;
    iFilesToInstall.ResetAndDestroy();
    
    if ( iState == EDSisInstallerStateInstalling )
        {                    
        iInstallLauncher->Cancel();
        }
    
    TRAP_IGNORE( iIAUpdater.SetProcessParametersL( KErrCancel ) );
    
    IAUPDATE_TRACE("[IAUpdater] Installer::DoCancel end");
    }
	

// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::RunL()
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::RunL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::RunL() begin");
    IAUPDATE_TRACE_1("[IAUpdater] RunL(): iStatus = %d", iStatus.Int() );     
    
    switch ( iState )
        {      
        // Reached when installation is completed. Check result.
        case EDSisInstallerStateInstalling:
            { 
            IAUPDATE_TRACE("[IAUpdater] RunL() Sis Inst. state INSTALLING");                    
            
            // Get error category
            TInt errCategory = iResults->GetIntByNameL
                    (Usif::KSifOutParam_ErrCategory, errCategory);
            
            if ( errCategory == Usif::EInstallerBusy )
                {
                IAUPDATE_TRACE("[IAUpdater] RunL() SWInstaller Busy"); 
                
                // User might be installing something, wait before retrying 
                TTimeIntervalMicroSeconds32 time( 
                    IAUpdaterDefs::KInstallRetryWaitTime );                        
                iTimer.After( iStatus, time );                
                iState = EDSisInstallerStateInstallerBusy;
                SetActive();                        
                break;                        
                }                
            else if ( (errCategory == Usif::ESecurityError && 
                iInstallErr == KErrNone) ||
                (iStatus.Int() != KErrNone && 
                 iStatus.Int() != errCategory == Usif::ESecurityError) )
                {
                IAUPDATE_TRACE("[IAUpdater] RunL() Ins. err or sec. failure"); 
                
                iInstallErr = iStatus.Int();                                      
                }
                                                 
            // Check if we have more to install
            if ( iFileIndex < iFilesToInstall.Count() )
                {
                IAUPDATE_TRACE("[IAUpdater] RunL() Start next installation"); 
                                              
                iState = EDSisInstallerStateIdle;               
                CompleteSelf();
                }   
            else
                {                               
                InstallationCompleted( iInstallErr );
                } 
            }
            break;
			
        // Install a file
        case EDSisInstallerStateIdle:
            { 
            IAUPDATE_TRACE("[IAUpdater] RunL() Sis Inst. state IDLE");  
            
            if ( iFileIndex < iFilesToInstall.Count() )
                {                               
                iSisFile.Copy( *iFilesToInstall[iFileIndex] );
                ++iFileIndex;

                IAUPDATE_TRACE_1("[IAUpdater] RunL() Start install %S", &iSisFile ); 
                iInstallLauncher->InstallL( iSisFile, iStatus, iResults );                
                iState = EDSisInstallerStateInstalling;                    
                SetActive();
                }
            else
                {
                InstallationCompleted( iInstallErr );                            
                } 
            }                        
            break;
            
        case EDSisInstallerStateInstallerBusy:
            {
            IAUPDATE_TRACE("[IAUpdater] RunL() Sis Inst. state INSTALLER BUSY");  
            IAUPDATE_TRACE_1("[IAUpdater] RunL() Start install for %S", &iSisFile ); 
                        
            iInstallLauncher->InstallL( iSisFile, iStatus, iResults );
            iState = EDSisInstallerStateInstalling;

            SetActive();                 
            }
            break;                       
            
        // Active object in unknown state
        default:
            IAUPDATE_TRACE("[IAUpdater] RunL() Active object in unknown state"); 
            User::Panic( IAUpdaterDefs::KIAUpdaterInstaller, KErrNotSupported );
            break;
        }

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::RunL() end");
    }
	
 
// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::RunError()
// If RunL leaves then ignore errors
// -----------------------------------------------------------------------------
//
TInt CIAUpdaterInstaller::RunError( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterInstaller::RunError() error = %d", aError );            
    TInt err( KErrNone );
    InstallationCompleted( aError );                
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::RunError() end");
    return err;
    }
    
    		
// -----------------------------------------------------------------------------
// CIAUpdaterInstaller::InstallationCompleted()
// Stop active scheduler and stop installation.
// -----------------------------------------------------------------------------
//
void CIAUpdaterInstaller::InstallationCompleted( TInt aResult )
    {
    IAUPDATE_TRACE_1("[IAUpdater] Installation is completed. result = %d", aResult);        
    
    TRAP_IGNORE( iIAUpdater.SetProcessParametersL( aResult ) );    
        
    iState = EDSisInstallerStateIdle;    
    iInstallErr = KErrNone;    
    delete iInstallLauncher;
    iInstallLauncher = NULL;
    
    IAUPDATE_TRACE("[IAUpdater] Stop active scheduler"); 

    // Cancel outstanding requests.    
    CancelOperation();

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterInstaller::InstallationCompleted() end");
    } 

// ======== GLOBAL FUNCTIONS ========
    
//EOF




       
