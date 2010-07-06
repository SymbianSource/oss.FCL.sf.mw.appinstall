/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handels IAD self update and starting the IAD process 
*                after self update.
*
*/



#include <apgcli.h>     // RApaLsSession
#include <apacmdln.h>   // CApaCommandLine
#include <apgtask.h>
#include <apgwgnam.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <catalogsuids.h>

#include "iaupdatermngr.h"
#include "iaupdaterinstaller.h"
#include "iaupdaterdialog.h"
#include "iaupdaterdefs.h"

#include "iaupdaterfilelistfile.h"
#include "iaupdaterfilelist.h"
#include "iaupdaterfileinfo.h"
#include "iaupdaterresultsfile.h"
#include "iaupdaterresult.h"
#include "iaupdateridentifier.h"

#include "iaupdatedebug.h"


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// Maximum time (in microseconds) that ECOM is waited.
const TInt KMaxEcomWait( 3000000 );


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::NewL() 
// 
// -----------------------------------------------------------------------------
//
CIAUpdaterMngr* CIAUpdaterMngr::NewL() 
    {
    CIAUpdaterMngr* self = CIAUpdaterMngr::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::NewLC() 
// 
// -----------------------------------------------------------------------------
//
CIAUpdaterMngr* CIAUpdaterMngr::NewLC() 
    {
    CIAUpdaterMngr* self = new (ELeave) CIAUpdaterMngr;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;	
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::ConstructL()
// 
// -----------------------------------------------------------------------------
//
void CIAUpdaterMngr::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::ConstructL start");
    iResultsFile = CIAUpdaterResultsFile::NewL();
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::ConstructL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::~CIAUpdater()
//
// -----------------------------------------------------------------------------
//
CIAUpdaterMngr::~CIAUpdaterMngr()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::~CIAUpdaterMngr start");
    delete iIAInstaller;        
    delete iResultsFile;
    iWsSession.Close();
    iFs.Close();
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::~CIAUpdaterMngr end");
    }
  
  
// -----------------------------------------------------------------------------
// CIAUpdaterMngr::ProcessInstallingL()
// 
// -----------------------------------------------------------------------------
//
void CIAUpdaterMngr::ProcessInstallingL( TIAUpdaterStartParams& aStartParams )
    {
    IAUPDATE_TRACE("[IAUpdater] ProcessInstallingL");

    // Reset member variables
    iInstallError = KErrNone;
    iEcomPluginInstalled = EFalse;


    // Initialize the results file for operations
    
    // Just in case there would be some old information left.
    iResultsFile->Reset();    
    // Set the results file path.
    iResultsFile->SetFilePathL( aStartParams.iResultFilePath );


    // Start actual operations.

    // Get the file info data from the file.
    CIAUpdaterFileListFile* listFile( CIAUpdaterFileListFile::NewLC() );

    // The correct path was given in the starting parameters.
    // Set this path. So, the object can read its data from the correct file.
    IAUPDATE_TRACE_1("[IAUpdater] ProcessInstallingL set path: %S", 
                     &aStartParams.iInitFilePath);
    listFile->SetFilePathL( aStartParams.iInitFilePath );

    // Read data from the defined file.
    listFile->ReadDataL();

    // Remove the file, because all the information from the list file
    // is now available in the lisFile object. Then, it will not be reused 
    // if this operation leaves and iaupdate is restarted later.
    IAUPDATE_TRACE_1("[IAUpdater] ProcessInstallingL fileRemove: %S", 
                     &listFile->FilePath());
    User::LeaveIfError( listFile->RemoveFile() );
    IAUPDATE_TRACE("[IAUpdater] File removed.");
    
    // Now, get the info that is required for the dialog.
    // Notice, that we keep the index value to constant. 
    // This application supposes that all the installable
    // bundles are part of the one same packet. So, indexing
    // is not changed.
    TInt KIndex( listFile->PckgStartIndex() );
    const TInt KTotalCount( listFile->TotalPckgCount() );
    // Check if the dialogs should be shown
    const TBool KSilent( listFile->Silent() );

    TInt count( listFile->FileList().Count() );
    TBool showing( EFalse );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iWsSession.Connect( iFs ) );
    //CIAUpdaterDialog* dialogs( CIAUpdaterDialog::NewLC( iFs, *this ) );
    CIAUpdaterDialog* dialogs( CIAUpdaterDialog::NewLC( *this ) );
    if ( count > 0 )
        {
        TInt nameIndex( 0 );
        for ( TInt i = 0; i < count; ++i )
            {
            IAUPDATE_TRACE_1("[IAUpdater] FileList: %d", i);
            iCurrentFileList = listFile->FileList()[ i ];

            if ( iInstallError == KErrNone )
                {
                IAUPDATE_TRACE("[IAUpdater] No errors. Continue install.");
                if ( !KSilent && i == nameIndex )
                    {
                    IAUPDATE_TRACE("[IAUpdater] Show dialog");
                    // If the dialog should be shown, then use the constant values for the
                    // indexing and total count.
                    // Search for the bundle name. Hidden names are not shown, so the first
                    // not hidden name will be shown. Notice, that here we think that the name
                    // that describes also the hidden items is non hidden item after those hidden
                    // items.
                    for ( TInt j = nameIndex; j < count; ++j )
                        {
                        IAUPDATE_TRACE_1("[IAUpdater] nameIndex: %d", j);
                        CIAUpdaterFileList* list( listFile->FileList()[ j ] );
                        if ( !list->Hidden() 
                             || j + 1 == count && !showing )
                            {
                            if ( !KSilent && showing )
                                {
                                IAUPDATE_TRACE("[IAUpdater] Cancel waiting note");
                                // Stop showing wait note before starting new note.       
                                dialogs->CancelWaitingNoteL();
                                showing = EFalse;
                                }

                            // Show dialog if the item is not hidden or 
                            // if all the items are hidden and no dialog is showing, then show
                            // the last one.
                            // The last item is always thought as non hidden because it is the main item.
                            // All other items before it are items that the main item depends on.
                            // Show waiting note because we are starting the install operation
                            IAUPDATE_TRACE_1("[IAUpdater] Show new waiting note: %S", 
                                             &list->BundleName());
                            dialogs->
                                ShowWaitingNoteL( list->BundleName(), 
                                                  KIndex, KTotalCount );                    
                            nameIndex = j + 1;
                            showing = ETrue;
                            break;
                            }
                        }
                    }

                // Delete old installer. So, the new installer will be configured 
                // with correct values.
                delete iIAInstaller; 
                iIAInstaller = NULL;            
                iIAInstaller = CIAUpdaterInstaller::NewL( iFs, *this );
                ProcessSISFilesL( *iCurrentFileList );
                    
                // Notice, that this is synchronous installation that will wait until the
                // active install operation has finished its job. 
                // Uses, CActiveSchedulerWait.
                // So, be cautious about nested loops. 
                iIAInstaller->StartInstallingL();                     
                }
            else if ( iInstallError == KErrCancel )
                {
                // Because cancel occurred during the last install,
                // cancel installation of other items. 
                // Set error code as KErrCancel for remaining items.
                // Setting of these parameters is so quick operation,
                // that do not show separate dialogs for this after
                // the possible shown dialog is closed below.
                IAUPDATE_TRACE("[IAUpdater] Cancel occurred. So, do not try to install.");
                SetProcessParametersL( KErrCancel );
                }
            else
                {
                // Because an error occurred during the last install,
                // abort installation of other items. 
                // Set error code as KErrAbort for remaining items.
                // Setting of these parameters is so quick operation,
                // that do not show separate dialogs for this after
                // the possible shown dialog is closed below.
                IAUPDATE_TRACE("[IAUpdater] Error occurred. So, do not try to install.");
                SetProcessParametersL( KErrAbort );
                }
            }

        IAUPDATE_TRACE("[IAUpdater] Finished installation.");

        if ( iEcomPluginInstalled )
            {
            IAUPDATE_TRACE("[IAUpdater] Wait for the ECOM.");
            // Ecom plugin was installed during the update flow.
            // Wait for the ECOM to finish indexing of its plugins.
            EcomWaiterL();
            }

        }

    
    // Increase the index by one for the result. 
    // Notice, that here the index may become as great as KTotalCount but
    // it is up to the user of the results to check this later.
    iResultsFile->SetPckgStartIndex( KIndex + 1 );
    iResultsFile->SetTotalPckgCount( KTotalCount );
    
    // Look at is there iaupdate instance running
    //
    TApaTaskList tasklist( iWsSession );   
    TApaTask task = tasklist.FindApp( TUid::Uid( IAUpdaterDefs::KIADUpdateUid3 ) );
  
    
    
    IAUPDATE_TRACE("[IAUpdater] Final process checks");
      
    // Result data is not written if user exit (end key pressed) and  
    // there is not iaupdate running
    if ( !iUserExit || task.Exists() )
        {
        IAUPDATE_TRACE("[IAUpdater] Write results file data");
    	iResultsFile->WriteDataL();
        }
        
    // Iaupdate is restarted only when there is not running instance.  
    // When user exit (end key pressed) new iaupdate is not launched     
    if ( !iUserExit && !task.Exists() )
        {
        IAUPDATE_TRACE("[IAUpdater] Start IAUpdate UI process");
    	StartIADProcessesL();
        }
    
    if ( !KSilent && showing )
        {
        IAUPDATE_TRACE("[IAUpdater] Stop showing dialog.");
        // Stop showing wait note because we have finished this installation.       
        TRAP_IGNORE( dialogs->CancelWaitingNoteL() );
        CleanupStack::PopAndDestroy( dialogs );
        }
    
    CleanupStack::PopAndDestroy( listFile );
    listFile = NULL;
    
    if ( iUserExit )
        {
        IAUPDATE_TRACE("[IAUpdater] User exit");
        User::Leave( IAUpdaterDefs::KIAUpdaterShutdownRequest );
        }   
  

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::ProcessInstallingL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::UserCancel
//
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::UserCancel()
    {
    IAUPDATE_TRACE("[IAUpdater] UserCancel() start");
    if ( iIAInstaller )
        {
    	iIAInstaller->CancelOperation();
        }
    IAUPDATE_TRACE("[IAUpdater] UserCancel() end");    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::UserExit()
//
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::UserExit()
    {
    IAUPDATE_TRACE("[IAUpdater] UserExit() start");
    if ( iIAInstaller )
        {
    	iIAInstaller->CancelOperation();
    	iUserExit = ETrue;
        }
    IAUPDATE_TRACE("[IAUpdater] UserExit() end"); 
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::ProcessSISFilesL()
//
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::ProcessSISFilesL( CIAUpdaterFileList& aList )
    { 
    IAUPDATE_TRACE("[IAUpdater] ProcessSISFilesL start");       

    TInt count( aList.FileInfos().Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CIAUpdaterFileInfo* info( aList.FileInfos()[ i ] );
        const TDesC& sisFileName( info->FilePath() );
        iIAInstaller->AddFileToInstallL( sisFileName );
        }

    IAUPDATE_TRACE("[IAUpdater] ProcessSISFilesL end");        
    }



// -----------------------------------------------------------------------------
// CIAUpdaterMngr::SetProcessParametersL
// Set installation return parameters for IAD process.
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::SetProcessParametersL( TInt aErr )
    {    
    IAUPDATE_TRACE_1("[IAUpdater] SetProcessParametersL start: %d", aErr);

    if ( aErr == KErrNone )
        {
        IAUPDATE_TRACE("[IAUpdater] Installation was success");
        // Because self update related item was successfully installed,
        // make sure that the IAD is closed. In certain cases, the
        // installer may not close the IAD. For example, if only
        // NCD Engine is updated but IAD is not, or, if original
        // IAD was in ROM but new update is installed into another
        // drive.
        CloseIADProcessesL();

        // Check if the installed package should be thought as
        // an ecom plugin. The UID value is used for the checking.
        if ( IsEcomPlugin( iCurrentFileList->Identifier().Uid() ) )
            {
            IAUPDATE_TRACE("[IAUpdater] ECOM plugin was installed.");
            iEcomPluginInstalled = ETrue;
            }
        }
    else
        {
        IAUPDATE_TRACE_1("[IAUpdater] SetProcessParametersL error: %d", aErr);
        // Set the error value to the member variable.
        // So, the manager will know, that it will not try to install
        // any other items after this.
        iInstallError = aErr;
        }
    
    CIAUpdaterResult* result( CIAUpdaterResult::NewLC() );
    CIAUpdaterIdentifier& resultIdentifier( result->Identifier() );
    CIAUpdaterIdentifier& currentIdentifier( iCurrentFileList->Identifier() );
    resultIdentifier.SetIdL( currentIdentifier.Id() );
    resultIdentifier.SetNamespaceL( currentIdentifier.Namespace() );
    resultIdentifier.SetUid( currentIdentifier.Uid() );
    result->SetErrorCode( aErr );
    result->SetHidden( iCurrentFileList->Hidden() );
    iResultsFile->Results().AppendL( result );
    // Ownership of the result was transferred to the array.
    CleanupStack::Pop( result );

    IAUPDATE_TRACE("[IAUpdater] SetProcessParametersL end");
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterMngr::StartIADProcessesL()
// Start IAD process after installation.
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::StartIADProcessesL()
    {    
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::StartIADProcessesL start"); 
    // Apparc is not always up-to-date just after installation
    // To guarantee that just installed iaupdate.exe will be launched
    // it's checked that RApaLsSession is up-to-date before launching iupdatelancher.exe
    
    
    TFileName fullNameOfIaupdate; 
    InstalledIAUpdateL( fullNameOfIaupdate );
    
    RApaLsSession apa;
    User::LeaveIfError( apa.Connect() );
    CleanupClosePushL(apa);
    
    TBool foundInApparc = EFalse;
    TApaAppInfo info;
    // trying with 0.5 seconds intervals  max. 10 seconds. 
    for( TInt i = 0; !foundInApparc && i < 21 ; i++)  
        {
        if ( i > 0 )
            {
            User::After( 500000 );
            }
        User::LeaveIfError( apa.GetAppInfo( info, TUid::Uid( IAUpdaterDefs::KIADUpdateUid3 ) ) );
        if ( info.iFullName.CompareF ( fullNameOfIaupdate ) == 0 )
            {
            foundInApparc = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( &apa );   
    
    RProcess process;
    User::LeaveIfError( 
        process.Create( IAUpdaterDefs::KIAUpdateLauncherExe, KNullDesC ) );
    process.Resume();
    process.Close();
    
    User::LeaveIfError( 
        process.Create( IAUpdaterDefs::KIAUpdateBgCheckerExe, KNullDesC ) );
    process.Resume();
    process.Close();    

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::StartIADProcessesL end"); 
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::CloseIADProcessesL()
// Close IAD process after one successfull installation.
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::CloseIADProcessesL()
    {
    IAUPDATE_TRACE("[IAUpdater] CloseIADProcessesL start");
    
    //close background checker 
    
    TBool continueScanning = ETrue;
    TFindProcess findProcess;
    // : add search pattern to optimizing
    TFullName fullName;
    while( continueScanning )
		{
		if ( findProcess.Next( fullName ) == KErrNone ) 
		    {
		    RProcess process;
	        User::LeaveIfError( process.Open( findProcess ) );
	        TUid sid( process.SecureId() );
	        if ( sid == IAUpdaterDefs::KUidIABgChecker && process.ExitType() == EExitPending )
		        {
		        process.Kill( KErrNone );
		        continueScanning = EFalse;
		        }
		    process.Close(); 	
		    }
		else
		    {
		    continueScanning = EFalse;	
		    }   
		}
    
    //close IAD launcher
    TInt wgId=0;
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewL( iWsSession );
    CleanupStack::PushL( wgName );
    TUid launcherUid( TUid::Uid( IAUpdaterDefs::KIADUpdateLauncherUid3 ) );
    CApaWindowGroupName::FindByAppUid( launcherUid, iWsSession, wgId );
    if ( wgId != KErrNotFound )
        {
        IAUPDATE_TRACE_1("[IAUpdater] IAD process found, wgId = %d", wgId ); 
        wgName->ConstructFromWgIdL( wgId );
        TApaTask task( iWsSession );
        task.SetWgId( wgId );
        RThread thread;
        User::LeaveIfError( thread.Open( task.ThreadId() ) );
        CleanupClosePushL(thread);
                        
        RProcess process;
        User::LeaveIfError( thread.Process( process ) );
        CleanupClosePushL( process );

        TRequestStatus processStatus;
        process.Logon( processStatus );
                
        // IAD launcher application should response to shutdown event, so just wait for it       
        task.SendSystemEvent( EApaSystemEventShutdown );
        IAUPDATE_TRACE("[IAUpdater] Closing IAD launcher");
        User::WaitForRequest( processStatus );
        IAUPDATE_TRACE("[IAUpdater] IAD launcher closed");
        CleanupStack::PopAndDestroy( &process );
        CleanupStack::PopAndDestroy( &thread );
        }
    CleanupStack::PopAndDestroy(wgName);
    wgName = NULL;
    
    //close all processes of IAD 
    wgId=0;
    wgName = CApaWindowGroupName::NewL( iWsSession );
	CleanupStack::PushL( wgName );
	TUid iadUid( TUid::Uid( IAUpdaterDefs::KIADUpdateUid3 ) );
	CApaWindowGroupName::FindByAppUid( iadUid, iWsSession, wgId );
	while (wgId != KErrNotFound )
		{
		IAUPDATE_TRACE_1("[IAUpdater] IAD process found, wgId = %d", wgId ); 	
		wgName->ConstructFromWgIdL( wgId );
		TApaTask task( iWsSession );
		task.SetWgId( wgId );
		RThread thread;
		User::LeaveIfError( thread.Open( task.ThreadId() ) );
		CleanupClosePushL(thread);
				
		RProcess process;
		User::LeaveIfError( thread.Process( process ) );
		CleanupClosePushL( process );

		TRequestStatus processStatus;
		process.Logon( processStatus );
		
		// IAD application should response to shutdown event, so just wait for it		
		task.SendSystemEvent( EApaSystemEventShutdown );
        IAUPDATE_TRACE("[IAUpdater] Closing IAD");
		User::WaitForRequest( processStatus );
		IAUPDATE_TRACE("[IAUpdater] IAD closed");
		CleanupStack::PopAndDestroy( &process );
		CleanupStack::PopAndDestroy( &thread );
		
		// See if there's still an instance of IAD running.
		CApaWindowGroupName::FindByAppUid( iadUid, iWsSession, wgId );
		}
	CleanupStack::PopAndDestroy(wgName);	
  
    IAUPDATE_TRACE("[IAUpdater] CloseIADProcessesL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::EcomWaiter()
// Wait until the ECOM has updated its index list.
// -----------------------------------------------------------------------------
//	
void CIAUpdaterMngr::EcomWaiterL() const
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::EcomWaiter() begin");

    const TTimeIntervalMicroSeconds32 KWaitTime( KMaxEcomWait );

    REComSession& session( REComSession::OpenL() );
    CleanupClosePushL( session );

    IAUPDATE_TRACE("[IAUpdater] Create timer"); 
    RTimer timer;
    CleanupClosePushL( timer );
    timer.CreateLocal();

    IAUPDATE_TRACE("[IAUpdater] Start timer");
    TRequestStatus timerStatus( KErrNone );
    timer.After( timerStatus, KWaitTime ); 

    IAUPDATE_TRACE("[IAUpdater] Call notifier"); 
    TRequestStatus ecomStatus( KErrNone );
    session.NotifyOnChange( ecomStatus );

    // Wait for the first request to complete
    IAUPDATE_TRACE("[IAUpdater] Wait first");
    User::WaitForRequest( ecomStatus, timerStatus );

    IAUPDATE_TRACE_2("[IAUpdater] Continue after waitforrequest: %d, %d",
                     ecomStatus.Int(), timerStatus.Int());

    // These are synchronous operations.
    // So, no need to use WaitForRequest after these.
    // It does not hurt to call cancel to completed operation. 
    // So, no need to check which one was completed.
    session.CancelNotifyOnChange( ecomStatus );
    timer.Cancel(); 

    // Notice, that in this debug text another status should always imply
    // that cancel operation is done. And, another will give the result of the
    // completed operation.
    IAUPDATE_TRACE_2("[IAUpdater] Continue after second waitforrequest: %d, %d",
                     ecomStatus.Int(), timerStatus.Int());

    CleanupStack::PopAndDestroy( &timer );
    CleanupStack::PopAndDestroy( &session );
    
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::EcomWaiter() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdaterMngr::IsEcomPlugin()
// Compares the given UID to the known ECOM plug-in installation package UIDs.
// -----------------------------------------------------------------------------
//	
TBool CIAUpdaterMngr::IsEcomPlugin( const TUid& aUid ) const
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterMngr::IsEcomPlugin() begin: %d",
                     aUid.iUid);

    TBool isEcomPlugin( EFalse );

    switch ( aUid.iUid )
        {
        // Part of these NCD UIDs are not exactly ECOM plugins themselves
        // but they are most likely part of the package that contains
        // a plugin. So, if any of these matches. Then just think
        // the package as ECOM plugin.
        case KCatalogsServerUid:
        case KNcdProviderUid:
        case KCatalogsEngineUid:
        case KCatalogsEngineInterfaceUid:
        case KCatalogsEngineImplementationUid:
            IAUPDATE_TRACE("[IAUpdater] ECOM plugin.");
            isEcomPlugin = ETrue;
            break;

        default:
            IAUPDATE_TRACE("[IAUpdater] Not ECOM plugin.");
            break;
        }

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterMngr::IsEcomPlugin() end");

    return isEcomPlugin;
    }

// -----------------------------------------------------------------------------
// CIAUpdaterMngr::InstalledIAUpdateL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdaterMngr::InstalledIAUpdateL( TFileName& aFullName ) const
    {
    Swi::RSisRegistrySession registrySession;
    Swi::RSisRegistryEntry entry;
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    TInt exeFound = EFalse;
    if ( entry.Open( registrySession, TUid::Uid( IAUpdaterDefs::KIADUpdateUid3 ) ) == KErrNone )
        {
        CleanupClosePushL( entry );
        RPointerArray<HBufC> files; 
        entry.FilesL( files );
        for( TInt i = 0; !exeFound && i < files.Count(); i++)
            {
            TFileName fullName = *files[i]; 
            TParse parse;
            parse.Set( fullName, NULL, NULL);    
            if ( parse.NameAndExt().CompareF( IAUpdaterDefs::KIAUpdateExe() ) == 0 )
                {
                aFullName = fullName;
                exeFound = ETrue;
                }
            }
        files.ResetAndDestroy();    
        CleanupStack::PopAndDestroy( &entry );
        }
    CleanupStack::PopAndDestroy( &registrySession );
    }

//EOF
