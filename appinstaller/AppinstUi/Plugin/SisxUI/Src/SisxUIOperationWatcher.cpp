/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CSisxUIOperationWatcher 
*                class member functions.
*
*/


// INCLUDE FILES

#include <swi/asynclauncher.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>
#include <pathinfo.h>
#include <zipfilemember.h>
#include <DRMLicenseManager.h>
#include <SisxUIData.rsg>
#include <SWInstTaskManager.h>
#include <SWInstTask.h>
#include <SWInstDefs.h>
#include <featmgr.h>
#include <SWInstFileTaskParam.h>
#include <e32property.h>
#include <SWInstallerInternalPSKeys.h>
#include <Oma2Agent.h>              // KOmaDrm2AgentName
#include <syslangutil.h>
#include <caf/manager.h>            // ContentAccess::CManager
#include <caf/agent.h>              // ContentAccess::TAgent

#include <DcfEntry.h> // JPAA-7GEB7P 
#include <DcfRep.h> // JPAA-7GEB7P

#include "SisxUIOperationWatcher.h"
#include "SisxUIHandler.h"
#include "SisxUIResult.h"
#include "SisxUIAppInfo.h"
#include "SisxUIStartupFile.h"
#include "SWInstDebug.h"
#include "SisxUISilentHandler.h"
#include "SisxUICleanupUtils.h" // JPAA-7GEB7P

_LIT( KTempDir, "piptemp\\" );
_LIT( KPipFileMatch, "*" );
const TUint KSisxFileUidPosition = 8;
const TUint KDot = 0x2E; // Dot
_LIT(KSisString, "sis");
_LIT( KSkinExtension, "*.skn" ); // JPAA-7GEB7P

using namespace SwiUI;
using namespace ContentAccess;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::CSisxUIOperationWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUIOperationWatcher::CSisxUIOperationWatcher() 
    : CActive( CActive::EPriorityStandard ),
      iIsSilent( EFalse ),
      iDoSilentOcsp( EFalse )
    {
    CActiveScheduler::Add( this );    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::ConstructL()
    {
    iTaskManager = CTaskManager::NewL();
    User::LeaveIfError( iRfs.Connect() );    
    iRfs.ShareProtected();
    FeatureManager::InitializeLibL(); 
    iLauncher = Swi::CAsyncLauncher::NewL();
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUIOperationWatcher* CSisxUIOperationWatcher::NewL()
    {
    CSisxUIOperationWatcher* self = new( ELeave ) CSisxUIOperationWatcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUIOperationWatcher::~CSisxUIOperationWatcher()
    {
    iDeletionOngoing = ETrue;
    if ( iCanceller )
        {
        iCanceller->Cancel();
        delete iCanceller;
        }    

    delete iLauncher;
    iLauncher = NULL;
    CActive::Cancel();

    iSupportedLanguages.Close();
    delete iUIHandler;
    delete iTaskManager;
    delete iMIME;    

    NotifyEndUninstall();

    iInstalledFiles.ResetAndDestroy();
    iAugmentations.ResetAndDestroy();    

    iTempFile.Close();
    iRfs.Close();    

    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::InstallL
// Handle install request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUIOperationWatcher::InstallL( RFile& aFile, 
                                        const TDesC8& aMIME, 
                                        TBool aIsDRM, 
                                        TRequestStatus& aRequestStatus )
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending; 

    iSourceFile = aFile;
    iMIME = aMIME.AllocL();    

    // Create the ui
    iUIHandler = CSisxUIHandler::NewL( CSisxUIHandler::EModeInstall, this );   
    if ( aIsDRM )
        {
        // We need to pass the handle to ui handler, so that it can shown
        // details of drm
        iUIHandler->SetFileForDrm( aFile );        
        }    

    iPreparing = ETrue;    
    // Kick off the RunL
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );
    SetActive();  
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::SilentInstallL
// Handle silentinstall request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUIOperationWatcher::SilentInstallL( RFile& aFile, 
                                              const TDesC8& aMIME, 
                                              TBool aIsDRM,
                                              TInstallOptions& aOptions, 
                                              TRequestStatus& aRequestStatus )
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending;

    iIsSilent = ETrue;      
    iSourceFile = aFile;
    iMIME = aMIME.AllocL();    
    if ( aOptions.iOCSP == EPolicyAllowed )
        {
        iDoSilentOcsp = ETrue;        
        }    

    // Create the ui
    iUIHandler = CSisxUISilentHandler::NewL( aOptions, this );   
    if ( aIsDRM )
        {
        // We need to pass the handle to ui handler, so that it can shown
        // details of drm
        iUIHandler->SetFileForDrm( aFile );        
        }    

    iPreparing = ETrue;    
    // Kick off the RunL
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );
    SetActive();  
    }        
      
// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::UninstallL
// Handle uninstall request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CSisxUIOperationWatcher::UninstallL( const TUid& aUid, TRequestStatus& aRequestStatus ) 
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending; 

    iUninstalledUid = aUid;    

    TRAP_IGNORE( NotifyStartUninstallL(aUid ) );
    iUIHandler = CSisxUIHandler::NewL( CSisxUIHandler::EModeUninstall, this );
    
    iLauncher->UninstallL( *iUIHandler, aUid, iStatus );
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::UninstallL
// Handle uninstall request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CSisxUIOperationWatcher::SilentUninstallL( const TUid& aUid, 
                                                TUninstallOptions& aOptions, 
                                                TRequestStatus& aRequestStatus ) 
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending; 

    iIsSilent = ETrue;    
    iUninstalledUid = aUid;    

    iUIHandler = CSisxUISilentHandler::NewL( aOptions, this );

    TRAP_IGNORE( NotifyStartUninstallL(aUid ) );
    iLauncher->UninstallL( *iUIHandler, aUid, iStatus );
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::UninstallAugmentationL
// Uninstalls only a specific augmentation (sispatch)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::UninstallAugmentationL( const TUid& aUid, 
                                                      TInt aIndex, 
                                                      TRequestStatus& aRequestStatus )
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending; 

    iUninstalledUid = aUid;    

    TRAP_IGNORE( NotifyStartUninstallL(aUid ) );
    iUIHandler = CSisxUIHandler::NewL( CSisxUIHandler::EModeUninstall, this );
   
    DoUninstallAugmentationL( aUid, aIndex );    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::SilentUninstallAugmentationL
// Uninstalls only a specific augmentation (sispatch)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::SilentUninstallAugmentationL( const TUid& aUid, 
                                                            TUninstallOptions& aOptions,
                                                            TInt aIndex, 
                                                            TRequestStatus& aRequestStatus )
    {
    // Set client request to pending state
    iRequestStatus = &aRequestStatus;    
    *iRequestStatus = KRequestPending; 

    iIsSilent = ETrue;    
    iUninstalledUid = aUid;    

    TRAP_IGNORE( NotifyStartUninstallL(aUid ) );
    iUIHandler = CSisxUISilentHandler::NewL( aOptions, this );
   
    DoUninstallAugmentationL( aUid, aIndex );    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::FinalCompleteL
// Informs the UI, about final completion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::FinalCompleteL()
    {
    iTempFile.Close();
    
    if ( iUIHandler )
        {
        iUIHandler->ShowFinalNoteL();        
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::Cancel
// Override of CActive::Cancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUIOperationWatcher::Cancel()
    {
    if ( !iCancelOngoing )
        {        
        iUIHandler->ProcessResult( KSWInstErrUserCancel );
    
        if ( iPreparing )
            {
            CActive::Cancel();            
            iUIHandler->DoNotShowFinalNote();            
            // Just complete the client request
            CompleteRequest( KSWInstErrUserCancel ); 
            }        
        else
            {            
            // If we are installing/uninstalling we may have a dialog waiting
            // for user input. We need to use this special method to cancel
            // the operation.
            TRAPD( err,
                   iCanceller = CommonUI::CCUICancelTimer::NewL( this );
                   iCanceller->StartCancelling();            
                );
            if ( err != KErrNone )
                {
                // Complete the original request from client
                CompleteRequest( KSWInstErrUserCancel );           
                }            
            }        

        iCancelOngoing = ETrue;        
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::RunL
// Called by framework when request is finished.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUIOperationWatcher::RunL()
    {
    FLOG_1( _L("SisxUI: RunL() status: %d"), iStatus.Int() );

    // If we got here, we don't need the canceller anymore
    if ( iCanceller )
        {
        iCanceller->Cancel();
        delete iCanceller;
        iCanceller = NULL;        
        }    

    if ( iPreparing )
        {
        // Do all the necessary preparations
        PrepareInstallL();
        return;        
        }

    // Let the UI know about the result
    iUIHandler->ProcessResult( iStatus.Int() );    

    if ( iUIHandler->GetResult().iResult == EUiResultOk )
        {
        // Install case    
        if ( iUIHandler->InstallMode() == CSisxUIHandler::EModeInstall )
            {
            TRAP_IGNORE( CompleteInstallationL() );            
            }
        // Uninstall case
        else 
            {
            TRAP_IGNORE( CompleteUninstallationL() );
            }       
        }

    CompleteRequest( iUIHandler->GetResult().CommonResult() );          
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::DoCancelL
// Called by framework when request is cancelled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::DoCancel()
    {        
    // This function is usually not used because of overridden Cancel().
    // Usually CActive::Cancel() followed by CompleteRequest() is used to
    // cancel active CSisxUIOperationWatcher. However, when active operation
    // watcher is deleted, this function must complete the client request
    // (to avoid deadlock when client calls CActive::Cancel() later).
    if( iDeletionOngoing && iRequestStatus )
        {
        CompleteRequest( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::DoCancelL
// Handles a leave occurring in the request completion event handler RunL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CSisxUIOperationWatcher::RunError( TInt aError )
    {
    // Report the error to the UI
    iUIHandler->ProcessResult( aError );    
    // Complete client request
    CompleteRequest( aError );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::UserCancelL
// Called when user has cancelled the operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::UserCancelL( TBool aCancelImmediately )
    {
    iIsUserCancel = ETrue;
    if ( !aCancelImmediately )
        {        
        Cancel();
        }
    else
        {
        CancelEngine();
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::IsShowingDialog
// Indicates if a modal dialog is currently shown.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TBool CSisxUIOperationWatcher::IsShowingDialog()
    {
    return iUIHandler->IsShowingDialog();    
    }
    
// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::CancelEngine
// Cancels the installation engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::CancelEngine()
    {
    if ( iLauncher )
        {
        iLauncher->CancelOperation();   
        }              
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::ForceCancel
// Called when nothing else has worked.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::ForceCancel()
    {
    // We need to make a dirty cancel
    // Kill the async launcher
    delete iLauncher;
    iLauncher = NULL;
    
    // We can cancel the AO, launcher has completed the request
    CActive::Cancel();
    
    // Complete the client request
    CompleteRequest( KSWInstErrUserCancel );
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::StartedCancellingL
// Called to indicate that the cancelling progress has been started.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::StartedCancellingL()
    {
    // Show the dialog only if this is user initiated cancel.
    if ( iIsUserCancel )
        {        
        iUIHandler->ShowCancellingL();    
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::DoCancelL
// Completes the pending request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::CompleteRequest( TInt aResult )
    {
    iTempFile.Close();
    
    if ( iRequestStatus )
        {        
        FLOG_1( _L("SisxUI: Completing with: %d"), aResult );
        User::RequestComplete( iRequestStatus, aResult );
        iRequestStatus = NULL;
        }   
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::AddStartupItemsL
// Add files to startup list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::AddStartupItemsL( RArray<TStartupTaskParam>& aParams )
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {        
        if ( aParams.Count() > 0 )
            {        
            CTask* task = CTask::NewL( KSTartupTaskAddImplUid, ETrue );
            CleanupStack::PushL( task );        
            for ( TInt index = 0; index < aParams.Count(); index++ )
                {
                TStartupTaskParamPckg pckg( aParams[index] );        
                task->SetParameterL( pckg, index );        
                }
    
            iTaskManager->AddTaskL( task );
            CleanupStack::Pop( task );
            }
        }    
    }
 
// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::RemoveStartupItemsL
// Remove files from the startup list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::RemoveStartupItemsL( RArray<TStartupTaskParam>& aParams )
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {        
        if ( aParams.Count() > 0 )
            {        
            CTask* task = CTask::NewL( KSTartupTaskRemoveImplUid, ETrue );
            CleanupStack::PushL( task );        
            for ( TInt index = 0; index < aParams.Count(); index++ )
                {
                TStartupTaskParamPckg pckg( aParams[index] );        
                task->SetParameterL( pckg, index );        
                }
    
            iTaskManager->AddTaskL( task );
            CleanupStack::Pop( task );
            }    
        }
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::RemoveStartupFileL
// Checks if the given uid has installed a startup file. If it finds one
// it removes it and creates a task to remove the items as well.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::RemoveStartupFileL( TUid aUid )
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {        
        // Handle startup list
        CSisxUIStartupFile* startupFile = CSisxUIStartupFile::NewL( aUid );
        CleanupStack::PushL( startupFile );
        if ( !startupFile->NewFileExists() )
            {
            FLOG( _L("SisxUI: Removal of import file") );
        
            // First see if we have a control file for this package
            if ( startupFile->PrivateFileExists() )
                {            
                RArray<TStartupTaskParam> params;     
                startupFile->RemovePrivateFile( params );
                CleanupClosePushL( params );                        
                RemoveStartupItemsL( params ); 
                CleanupStack::PopAndDestroy(); // params
                iInstalledFiles.ResetAndDestroy();

                // Put the flag on that we did modify the startup list
                iStartupModified = ETrue;        
                }
            }
        
        CleanupStack::PopAndDestroy( startupFile );        
        }      
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::HandleLogL
// Handles the log entry addition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::HandleLogL( const CSisxUIAppInfo& aAppInfo, 
                                          const TLogTaskAction& aAction, 
                                          TBool aIsStartup )
    {
    // Create a log task object, non-persistent
    CTask* task = CTask::NewL( KLogTaskImplUid, EFalse );
    CleanupStack::PushL( task );
    
    // Construct log task parameters
    TLogTaskParam params;

    // Make sure that we don't copy over limits
    params.iName.Copy( aAppInfo.Name().Left( KMaxLogNameLength ) );
    params.iVendor.Copy( aAppInfo.Vendor().Left( KMaxLogNameLength ) );

    params.iVersion = aAppInfo.Version();
    params.iUid = aAppInfo.Uid();
    // The time must be in universal time
    TTime time;
    time.UniversalTime();    
    params.iTime = time;
    params.iAction = aAction;
    params.iIsStartup = aIsStartup;    
    
    TLogTaskParamPckg pckg( params );
    task->SetParameterL( pckg, 0 );

    // Add the log task to the task list
    iTaskManager->AddTaskL( task );

    CleanupStack::Pop( task );
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::HandlePipFileL
// Helper to handle the case of pip file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::HandlePipFileL( RFile& aPipFileHandle, RFile& aSisFileHandle )
    {
    FLOG( _L("SisxUI: Handle pip file") );

    // PIP package, we need to use license manager to extract the sis file
    // First construct the temp path
    iRfs.PrivatePath( iTempDir );
    iTempDir.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
    iTempDir.Append( KTempDir );
    iRfs.MkDir( iTempDir );    
  
    // Add task to delete the extracted sis file
    HBufC* pipFileMatch = HBufC::NewLC( KMaxFileName );
    pipFileMatch->Des().Copy( iTempDir );
    pipFileMatch->Des().Append( KPipFileMatch );

    // Create a delete file task
    CTask* task = CTask::NewL( KFileTaskDeleteImplUid, ETrue );
    CleanupStack::PushL( task );
    
    // Construct file task parameters
    TFileTaskDeleteParam params;
    params.iFile = *pipFileMatch;
    TFileTaskDeleteParamPckg pckg( params );
    task->SetParameterL( pckg, 0 );
    
    // Add the task to the task list
    iTaskManager->AddTaskL( task );
    CleanupStack::Pop( task );    
    iTaskManager->CommitL();
    
    CleanupStack::PopAndDestroy( pipFileMatch );    
    
    // Use license manager to extract files from the pip package
    CDRMLicenseManager* licenseMgr = CDRMLicenseManager::NewL();
    CleanupStack::PushL( licenseMgr );               
    User::LeaveIfError( licenseMgr->ExtractSISFileL( aPipFileHandle, iTempDir ) );
    
    // Get the sis file name and open temporary handle to it.
    // HLEI-7EGFRE
    CZipFileMember* sisMember( licenseMgr->GetSISMemberL() );
    CleanupStack::PushL( sisMember );
    iTempDir.Append( *sisMember->Name() );
    CleanupStack::PopAndDestroy( sisMember );
    sisMember = NULL;
    
    User::LeaveIfError( aSisFileHandle.Open( iRfs, iTempDir, EFileShareAny ) );        
    iIsPipFile = ETrue;
    
    CleanupStack::PopAndDestroy( licenseMgr );    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::PrepareOCSPL
// Helper to read ocsp values from central repository and prepare the ui.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::PrepareOCSPL( CSisxUIHandler* aUIHandler, 
                                            Swi::CInstallPrefs* aPrefs )
    {
    TSWInstallerOcspProcedure ocspProc( ESWInstallerOcspProcedureOff );
    
    if ( !iIsSilent || iDoSilentOcsp )
        {        
        CRepository* repository = CRepository::NewL( KCRUidSWInstallerSettings );
        CleanupStack::PushL( repository );
    
        if ( !iIsSilent )
            {            
            // In case of normal installation we check the ocsp value from central rep
            TInt value( 0 );
            repository->Get( KSWInstallerOcspProcedure, value );
            ocspProc = TSWInstallerOcspProcedure( value );    
            }
        else
            {
            // In case of silent install, the client sets the value
            ocspProc = ESWInstallerOcspProcedureOn;            
            }
    
        // Get default OCSP URL
        HBufC* ocspURL = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
        TPtr ptr( ocspURL->Des() );
        repository->Get( KSWInstallerOcspDefaultURL, ptr );

        // Convert to 8 byte
        HBufC8* ocspURL8 = HBufC8::NewLC( ocspURL->Length() );
        ocspURL8->Des().Copy( *ocspURL );
        
        aPrefs->SetRevocationServerUriL( *ocspURL8 );
        CleanupStack::PopAndDestroy( 3, repository );        
        }
    
    if ( ocspProc == ESWInstallerOcspProcedureOn ||
         ocspProc == ESWInstallerOcspProcedureMust )
        {
        if ( ocspProc == ESWInstallerOcspProcedureOn )
            {
            aUIHandler->SetOcspProcedure( ESisxUIOcspProcOn );
            }
        else
            {
            aUIHandler->SetOcspProcedure( ESisxUIOcspProcMust );
            }        
        
        aPrefs->SetPerformRevocationCheck( ETrue );        
        }
    else
        {
        aUIHandler->SetOcspProcedure( ESisxUIOcspProcOff );
        aPrefs->SetPerformRevocationCheck( EFalse );
        }   
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::GetInstalledPackagesL
// Helper to get the packages that were installed in this session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::GetInstalledPackagesL( RPointerArray<CSisxUIAppInfo>& aPackages )
    {
    ContentAccess::CContent* content = NULL;
    if ( iIsPipFile )
        {
        // The source file is a pip file, we need to use the extracted sis file instead.
        content = ContentAccess::CContent::NewLC( iTempFile );
        }  
    else
        {
        content = ContentAccess::CContent::NewLC( iSourceFile );
        }
    
    ContentAccess::CData* data = content->OpenContentLC( ContentAccess::EExecute );

    // Read the uid from the package
    TInt32 uid( 0 );    
    TPckg<TInt32> uidpkg( uid );
    TInt pos( KSisxFileUidPosition );
    data->Seek( ESeekStart, pos );    
    data->Read( uidpkg, sizeof( TInt32 ) );

    CleanupStack::PopAndDestroy( 2, content );    

    iUIHandler->ProcessedPackages( aPackages );            

    // Currently we can only get the parent package
    for ( TInt index = 0; index < 1 && index < aPackages.Count(); index++ )
        {
        aPackages[index]->SetUid( TUid::Uid( uid ) );                
        }
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::HandleStartupAdditionL
// Helper to handle startup list addition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::HandleStartupAdditionL( const CSisxUIAppInfo& aAppInfo )
    {
    FLOG( _L("SisxUI: Handle startup") );

    if ( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {        
        CSisxUIStartupFile* startupFile = CSisxUIStartupFile::NewL( aAppInfo.Uid() );
        CleanupStack::PushL( startupFile );

        // First check if there is a control file in import dir
        if ( startupFile->NewFileExists() )
            {
            FLOG( _L("SisxUI: Processing control file") );

            // Get all files installed by this package
            Swi::RSisRegistrySession regSession;
            CleanupClosePushL( regSession );
            User::LeaveIfError( regSession.Connect() );

            Swi::RSisRegistryEntry regEntry;
            CleanupClosePushL( regEntry );
            User::LeaveIfError( regEntry.Open( regSession, aAppInfo.Uid() ) );

            regEntry.FilesL( iInstalledFiles ); 

            CleanupStack::PopAndDestroy( 2 ); // regEntry, regSession
                         
            RArray<TStartupTaskParam> params;

            // See if there are some old items, if so remove them
            if ( startupFile->PrivateFileExists() )
                {
                startupFile->RemovePrivateFile( params );
                CleanupClosePushL( params );                        
                RemoveStartupItemsL( params ); 
                CleanupStack::Pop(); // params                        
                params.Reset();                               
                }                        

            startupFile->ProcessNewFile( params, iInstalledFiles );
                        
            CleanupClosePushL( params );                        
            AddStartupItemsL( params ); 
            CleanupStack::PopAndDestroy(); // params
            iInstalledFiles.ResetAndDestroy();  
            }

        iTaskManager->CommitL();

        CleanupStack::PopAndDestroy( startupFile );   
        }
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::PrepareInstallL
// Prepares the installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::PrepareInstallL()
    {    
    Swi::CInstallPrefs* prefs = Swi::CInstallPrefs::NewL();
    CleanupStack::PushL( prefs );    

    // Prepare for possible OCSP
    PrepareOCSPL( iUIHandler, prefs );

    // Check if we allow untrusted SW to be installed
    CRepository* repository = CRepository::NewL( KCRUidSWInstallerSettings );
    CleanupStack::PushL( repository );    
    TInt value( 1 );
    repository->Get( KSWInstallerAllowUntrusted, value );
    if ( !value )
        {
        iUIHandler->SetAllowUntrusted( EFalse );        
        }
    else
        {
        iUIHandler->SetAllowUntrusted( ETrue );
        }    
    CleanupStack::PopAndDestroy( repository );    

    // Get supported language ids in a RArray for Swi::CAsyncLauncher::InstallL()
    iSupportedLanguages.Reset();
    CArrayFixFlat<TInt>* installedLanguages = NULL;
    TInt err = SysLangUtil::GetInstalledLanguages( installedLanguages, &iRfs );
    CleanupStack::PushL( installedLanguages );
    User::LeaveIfError( err );
    if( installedLanguages )
        {
        for( TInt lang = 0; lang < installedLanguages->Count(); lang++ )
            {
            iSupportedLanguages.AppendL( (*installedLanguages)[ lang ] );
            }
        }
    CleanupStack::PopAndDestroy( installedLanguages );

    if ( *iMIME == KPipMimeType )
        {
        // Handle the pip file
        HandlePipFileL( iSourceFile, iTempFile );
        
        // Start installation
        iLauncher->InstallL( *iUIHandler, iTempFile, *prefs, iSupportedLanguages, iStatus );
        }   
    else
        {
        // Start installation
        iLauncher->InstallL( *iUIHandler, iSourceFile, *prefs, iSupportedLanguages, iStatus );
        }    

    CleanupStack::PopAndDestroy( prefs );
    
    // Preparing done
    iPreparing = EFalse;
    SetActive();       
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::CompleteInstallationL
// Completes the installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::CompleteInstallationL()
    {
    FLOG( _L("SisxUI: CompleteInstallationL ->") );
    // If this was a pip file, process it
    if ( iIsPipFile )
        {    
        FLOG( _L("SisxUI: PIP file") );
                    
        CDRMLicenseManager* licenseMgr = CDRMLicenseManager::NewL();
        CleanupStack::PushL( licenseMgr );     
        TInt err = licenseMgr->ProcessL( iSourceFile, KNullDesC() );
        if ( err != KErrNone )
            {
            // Reset the OK result so that we can show the error.
            iUIHandler->ResetResult();
            User::Leave( KSWInstErrFileCorrupted );
            }
                
        CleanupStack::PopAndDestroy( licenseMgr );                
        
// RVIN-6JUGWH
        HBufC* fileName = HBufC::NewLC( 100 );
        TPtr fileNamePtr( fileName->Des() );   
    	TInt err2 = iSourceFile.Name( fileNamePtr );
        
        FLOG_1( _L("Original file name: %S"), &fileNamePtr );
        
        TInt i = 0;
        for ( ; i < fileNamePtr.Length(); i++ )
            {
            if ( fileNamePtr[i] == KDot )
                {
                break;
                }                    
            } 
            
        HBufC* tmpFile = HBufC::NewLC( i + 4 );
        TPtr tmpFilePtr( tmpFile->Des() );   
        tmpFilePtr = fileNamePtr.Left( i + 1 );
        tmpFilePtr.Append( KSisString );               

        FLOG_1( _L( "Changed file name: %S" ), &tmpFilePtr );

        // Create data supplier      
        CContent* content = CContent::NewLC( iSourceFile );

        HBufC* contentID = HBufC::NewLC( KMaxCafContentName  );
        TPtr contPtr(NULL, NULL);

        // create attribute set
        ContentAccess::RStringAttributeSet stringAttSet;
        //RStringAttributeSet stringAttSet;
        CleanupClosePushL( stringAttSet );

        stringAttSet.AddL( EContentID );

        content->GetStringAttributeSet( stringAttSet );

        // pass on values of string attributes
        contPtr.Set( contentID->Des() );

        err = stringAttSet.GetValue( EContentID, contPtr );
        
        if ( err != KErrNone )
            {  
            FLOG_1( _L("SisxUI: ContentID error = %d"), err );          
            // Reset the OK result so that we can show the error.
            iUIHandler->ResetResult();
            User::Leave( KSWInstErrGeneralError );
            }

        HBufC8* contentID8 = HBufC8::NewLC( contentID->Length() );
        TPtr8 cont8Ptr = contentID8->Des();
        
        // Copy 8bit data from contentID buffer.
        // Note that AgentSpecificCommand function take's only 8bit data.
        cont8Ptr.Copy( contPtr );

        HBufC8* buffer = HBufC8::NewMaxLC( sizeof( TInt ) + 
                                           contentID8->Size() + 
                                           tmpFilePtr.Size() );                                           
        TUint8* bptr = const_cast<TUint8*>( buffer->Ptr() );

        TUint length = tmpFilePtr.Length();
        TPtr8 temp( NULL, 0, 0 );
        TPtr8 buf( buffer->Des() );
        Mem::Copy(bptr, &length, sizeof(TInt));
        Mem::Copy(bptr+sizeof( TInt ), tmpFilePtr.Ptr(), length * 2 );
        Mem::Copy(bptr+sizeof( TInt )+( length * 2 ), contentID8->Ptr(), contentID8->Size());    

        ContentAccess::CManager* manager = NULL;
        ContentAccess::TAgent agent;

        // Find the caf agent and create manager
        manager = GetCafDataL( agent );

    	// ignore any error, we couldn't do anything about it anyway
        length = manager->AgentSpecificCommand( agent, ContentAccess::ESetContentName, 
                                                buf, temp);

        FLOG( _L("SisxUI: PopAndDestroy") );
        // buffer, tmpfile, fileName, content, contentID, stringAttSet, contentID8 
        CleanupStack::PopAndDestroy( 7 ); 
        delete manager;
// RVIN-6JUGWH                       
        }            

    FLOG( _L("SisxUI: GetInstalledPackagesL") );

    // Here we get only the first package in case of embedded packages
    // as currently there is no means to get the sub packages 
    RPointerArray<CSisxUIAppInfo> packages;  
    GetInstalledPackagesL( packages );            
    CleanupClosePushL( packages );            

    // Create log entry for every installed package and handle possible 
    // startup list addition.
    for ( TInt index = 0; index < packages.Count(); index++ )
        {               
        // Handle startup list
        // Currently, if embedded sis, only the first package can contain
        // startup items
        if ( !index && packages[index]->IsTrusted() )
            {
            HandleStartupAdditionL( *packages[index] ); 
       
            // Handle log
            HandleLogL( *packages[index], ELogTaskActionInstall, ETrue );
            }
        else
            {
            // Handle log
            HandleLogL( *packages[index], ELogTaskActionInstall, EFalse );
            }                
        }            
     
// JPAA-7GEB7P
    // Register PIP file content to DRM DCF server.
    if ( iIsPipFile )     
        {               
        TInt siscount = packages.Count();
        FLOG_1( _L("SisxUI: packages.Count() = %d"), siscount ); 
    
        if ( siscount )
            {    
            TUid pkgUid( packages[0]->Uid() );
            FLOG_1( _L("SisxUI: package UID: 0x%x"), pkgUid.iUid );              
            RegisterContentToDCF( pkgUid );
            }
        }
// JPAA-7GEB7P        
        
    FLOG( _L("SisxUI: PopAndDestroy") );
    CleanupStack::PopAndDestroy(); // packages
    
    FLOG( _L("SisxUI: CompleteInstallationL <-") );
            
    // Make sure that all other control files are removed.
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::CompleteUninstallationL
// Completes the uninstallation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::CompleteUninstallationL()
    {
    NotifyEndUninstall();
    
    RemoveStartupFileL( iUninstalledUid );
    iTaskManager->CommitL();   

    RPointerArray<CSisxUIAppInfo> packages;            
    iUIHandler->ProcessedPackages( packages );
    CleanupClosePushL( packages );
        
    if ( packages.Count() > 0 )
        {            
        packages[0]->SetUid( iUninstalledUid );           
        
        HandleLogL( *packages[0], ELogTaskActionUninstall, iStartupModified );
        }        
    CleanupStack::PopAndDestroy(); // packages 
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::DoUninstallAugmentationL
// Performs the uninstallation of augmentation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::DoUninstallAugmentationL( TUid aUid, TInt aIndex )
    {
    // Find the augmentation
    Swi::RSisRegistrySession regSession;
    CleanupClosePushL( regSession );
    User::LeaveIfError( regSession.Connect() );
    
    Swi::RSisRegistryEntry regEntry;
    CleanupClosePushL( regEntry );
    User::LeaveIfError( regEntry.Open( regSession, aUid ) );
    
    regEntry.AugmentationsL( iAugmentations );
    
    for ( TInt i( 0 ); i < iAugmentations.Count(); i++ )
        {
        if ( iAugmentations[i]->Index() == aIndex )
            {
            iIsAugmentation = ETrue;            
            // We found the correct package, now uninstall it
            iLauncher->UninstallL( *iUIHandler, *iAugmentations[i], iStatus );            
            SetActive();
            break;            
            }        
        if ( i == iAugmentations.Count() + 1 )
            {
            // We have gone through the whole list and couldn't find the package
            User::Leave( KErrNotFound );            
            }        
        }  
       
    iAugmentations.ResetAndDestroy();    
    CleanupStack::PopAndDestroy( 2, &regSession );   
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::NotifyStartUninstallL
// Notifies that uninstallation is about to start.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::NotifyStartUninstallL( TUid aUid )
    {
    _LIT_SECURITY_POLICY_PASS( KPropReadPolicy );
    _LIT_SECURITY_POLICY_C1( KPropWritePolicy, ECapabilityWriteDeviceData );

    // Try to define the property if this is the first time
    TInt err = RProperty::Define( KPSUidSWInstallerUiNotification, 
                                  KSWInstallerUninstallation, 
                                  RProperty::EInt,
                                  KPropReadPolicy,
                                  KPropWritePolicy );	
    if ( err != KErrNone && err != KErrAlreadyExists )
        {		
        User::Leave( err );		
        }
    
    // Set the package uid as the value
    err = RProperty::Set( KPSUidSWInstallerUiNotification, 
                          KSWInstallerUninstallation, 
                          aUid.iUid );
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::NotifyEndUninstall
// Notifies that uninstallation has been completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIOperationWatcher::NotifyEndUninstall()
    {
    RProperty::Set( KPSUidSWInstallerUiNotification, 
                    KSWInstallerUninstallation, 
                    0 );
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::GetCafDataL
// Handles searching correct DRM agent and manager.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
ContentAccess::CManager* CSisxUIOperationWatcher::GetCafDataL( ContentAccess::TAgent& aAgent )
    {
    RArray<TAgent> agents;

    CleanupClosePushL( agents );
    CManager* manager = CManager::NewLC();

    manager->ListAgentsL( agents );
    
    for ( TInt i = 0; i < agents.Count(); i++ )
        {
        if (agents[i].Name().Compare(KOmaDrm2AgentName) == 0)
            {
            aAgent = agents[i];
            break;
            }
        }
    CleanupStack::Pop( manager );
    CleanupStack::PopAndDestroy( &agents );
    return manager;    
    }

// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::RegisterContentToDCF
// This function does register PIP packages content to DCF server.
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::RegisterContentToDCF( TUid aUid )
    {
    // Note that currently we do register only skin (*.skn) 
    // files since there is not way to SisxUI plugin to know 
    // which files should be registerd to DCF. It is not reasonable
    // to push all files to DCF and hope that some is correct one.
    
    FLOG( _L("SisxUI: RegisterContentToDCF") ); 
    Swi::RSisRegistrySession sisRegSession;                                        
    User::LeaveIfError( sisRegSession.Connect() );                
    CleanupClosePushL( sisRegSession );
                
    // Get current package UID and open the entry.
    Swi::RSisRegistryEntry entry;                                
    // Open entry
    TInt error = entry.Open( sisRegSession, aUid );
    FLOG_1( _L("SisxUI: SisEntry open error: %d"), error );  
     
    if ( error == KErrNone )
        {                       
        CleanupClosePushL( entry );   
        // Get all files to array for DCF update.
        RPointerArray<HBufC> fileArray; 
        CleanupResetAndDestroyPushL( fileArray );
        TInt index = 0;     
        // It seems that some PIP pkg do not have files in 
        // main pacakge, but let's make sure that we do not
        // miss any files.
        FLOG( _L("SisxUI: Get files from main pkg") );  
        RPointerArray<HBufC> sisFileArray;                
        entry.FilesL( sisFileArray );                    
        CleanupResetAndDestroyPushL( sisFileArray );
        
        // Add main package's file paths to file array.
        SearchFilesForDCFL( sisFileArray, fileArray );
              
        // Let's see if there is augmentations to this main pkg.                  
        RPointerArray<Swi::CSisRegistryPackage> augPackages;                      
        entry.AugmentationsL( augPackages );                                        
        CleanupResetAndDestroyPushL( augPackages );
        
        TInt augCount = augPackages.Count();
        FLOG_1( _L("SisxUI: augmentations count = %d"), augCount );
        
        if ( augCount )
            { 
            // There may be several augmentaion pkgs and we need 
            // check them all.      
            for ( TInt augInd = 0; augInd < augCount; augInd++ )
                {
                FLOG_1( _L("SisxUI: augm. index = %d"), augInd );                                           
                Swi::RSisRegistryEntry augEntry;  
                
                error = augEntry.OpenL( 
                      sisRegSession, 
                      *augPackages[augInd] );
                FLOG_1( _L("SisxUI: Open augm. error = %d"), error );  
                                                                                            
                if ( !error )
                    {
                    CleanupClosePushL( augEntry );
                    
                    RPointerArray<HBufC> augFileArray;                
                    augEntry.FilesL( augFileArray );                         
                    CleanupResetAndDestroyPushL( augFileArray );
                    
                    // Add file paths to file array.
                    SearchFilesForDCFL( augFileArray, fileArray );
        
                    // augEntry, augFileArray                         
                    CleanupStack::PopAndDestroy( 2, &augEntry );                                             
                    }
                }//for   
            }
        // sisFileArray, augPackages
        CleanupStack::PopAndDestroy( 2, &sisFileArray );  
        
        // Ok let's start to register files to DCF server.
        TInt fileCount = fileArray.Count();  
        
        if ( fileCount )
            {                                       
            FLOG( _L("SisxUI: Create DCF entry") );
            CDcfEntry* dcfEntry = CDcfEntry::NewL();    
            CleanupStack::PushL( dcfEntry );
            
            CDcfRep* dcfRep = CDcfRep::NewL();
            CleanupStack::PushL( dcfRep );    
            
            for (index = 0; index < fileCount; index++ )
                {   
                TPtr fileBufPtr = fileArray[index]->Des();
                FLOG_1( _L("SisxUI: DCF SetLocationL with file: %S"), &fileBufPtr );
                
                TRAP( error, dcfEntry->SetLocationL( fileBufPtr, 0 ) );                   
                FLOG_1( _L("SisxUI: SetLocationL err = %d"), error );                
                // Note UpdateL function needs to be TRAPed. It
                // does leave if file type is not supported.
                TRAP( error, dcfRep->UpdateL( dcfEntry ) );                            
                FLOG_1( _L("SisxUI: UpdateL err = %d"), error );                                                                      
                }                                               
            // dcfRep, dcfEntry 
            CleanupStack::PopAndDestroy( 2 );  
            }
        // fileArray
        CleanupStack::PopAndDestroy( 1, &fileArray ); 
        // sisRegSession,  sisRegEntry                                                                                           
        CleanupStack::PopAndDestroy( 2, &sisRegSession );     
        }                                                      
    }


// -----------------------------------------------------------------------------
// CSisxUIOperationWatcher::SearchFilesForDCFL
// This function finds files for DCF server.
// -----------------------------------------------------------------------------
// 
void CSisxUIOperationWatcher::SearchFilesForDCFL( 
    RPointerArray<HBufC>& aSourceArray,
    RPointerArray<HBufC>& aTargetArray )
    {
    FLOG( _L("SisxUI: SearchFilesForDCFL") );   
    // Currently we do register only skin files.       
    // Note for now there is no way to get list from file types
    // which DCF server will accecpt. PIP package contais
    // some txt file which specifies files to be registred,
    // but SisxUI do not have acceess to that file since DRM
    // has extracts the pip content.
          
    TInt fileCount = aSourceArray.Count();                                                 
    FLOG_1( _L("SisxUI: package file count = %d"), fileCount );   
            
    if ( fileCount )
        {        
        for ( TInt index = 0; index < fileCount; index++ )
            {
            HBufC* tempPath = aSourceArray[index];
            // Search extension 
            FLOG( _L("SisxUI: Search extension") ); 
            if ( tempPath->Match( KSkinExtension ) != KErrNotFound )
                {
                // Ok correct file extension found. Alloc memory
                // because source may be deleted.
                HBufC* filePath = tempPath->AllocL();
                aTargetArray.Append( filePath );
#ifdef _DEBUG
                TPtr tempPtr = filePath->Des();                                                           
                FLOG_1( _L("SisxUI: Add file to array: %S"), &tempPtr );
#endif                                
                } 
            }
        }    
    }

//  End of File  
