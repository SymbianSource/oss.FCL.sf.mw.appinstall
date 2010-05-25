/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateStarter class 
*                member functions.
*
*/


//INCLUDES

#include <apacmdln.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apgwgnam.h>
#include <eikenv.h>

#include "iaupdatestarter.h"
#include "iaupdatestarterobserver.h"
#include "iaupdateutils.h"
#include "iaupdateuitimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateStarter::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateStarter* CIAUpdateStarter::NewL( const TDesC& aCommandLineExecutable,
                                          const TDesC8& aCommandLineArguments )
    {
    CIAUpdateStarter* self = new (ELeave) CIAUpdateStarter();
    CleanupStack::PushL( self );
    self->ConstructL( aCommandLineExecutable, aCommandLineArguments );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// void CIAUpdateStarter::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::ConstructL( const TDesC& aCommandLineExecutable,
                                   const TDesC8& aCommandLineArguments )
    {
    iCommandLineExecutable = aCommandLineExecutable.AllocL();
    iCommandLineArguments = aCommandLineArguments.AllocL();
    }    

// ---------------------------------------------------------------------------
// CIAUpdateStarter::CIAUpdateStarter
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateStarter::CIAUpdateStarter()  
    : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    iEikEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::~CIAUpdateStarter
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateStarter::~CIAUpdateStarter()
    {
    Cancel();
    delete iProcessStartTimer;
    delete iCommandLineExecutable;
    delete iCommandLineArguments;
    iThread.Close();
    iProcess.Close();
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::StartExecutableL
// Starts an executable
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::StartExecutableL( MIAUpdateStarterObserver& aStarterObserver ) 
    {
    iStarterObserver = &aStarterObserver;
    //Close all instances of executable first
    
    if ( !iStartNeeded )
        {
    	iStarterObserver->StartExecutableCompletedL( KErrNone );
    	iStarterObserver = NULL;
        }
    else
        {
        iStartNeeded = EFalse;
        iUid = TUid::Null();
        UidForExecutableL( iUid );
        if ( iUid.iUid == 0 )
            {
            LaunchExeL();
            }
        else
            {
    	    iPrevWgId = 0;  
    	    CApaWindowGroupName::FindByAppUid( iUid, iEikEnv->WsSession(), iPrevWgId );
	
	        if ( iPrevWgId == KErrNotFound )
	            {
	   	        LaunchExeL();
	            }
	        else
	            {
	            CloseExecutableL();	
	            }
    	    
            }
        }
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::CheckInstalledPackageL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateStarter::CheckInstalledPackageL( const TUid& aPUid )
    {
    if ( iCommandLineExecutable->Length() > 0 )
        {
    	if ( !iStartNeeded )
	        {
		    if ( IAUpdateUtils::IsInstalledL( aPUid, *iCommandLineExecutable ) )
		        {
		    	iStartNeeded = ETrue;
		        }
	        }
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateStarter::BringToForegroundL
// Bring started application to foreground
// ---------------------------------------------------------------------------
//    
void CIAUpdateStarter::BringToForegroundL() const
    {
    if ( iExeStarted )	
        {
        TUid uid( TUid::Null() );
        UidForExecutableL( uid );
        
        if ( uid.iUid != 0 )
            {
        	RWsSession ws;   
                                             
            if ( ws.Connect() == KErrNone )
                {
                CleanupClosePushL( ws );        
                TApaTaskList tasklist( ws );   
                 
      	        TApaTask task = tasklist.FindApp( uid );
       	        if ( task.Exists() )
       	            {
       	            task.BringToForeground();
                    }
                                                                        
                CleanupStack::PopAndDestroy( &ws );        
                }   
            }
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateStarter::DoCancel
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateStarter::DoCancel()
    {
	iProcess.LogonCancel( iStatus );
	if ( iProcessStartTimer )
	    {
	    iProcessStartTimer->Cancel();
	    }
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::RunL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::RunL()
    {
    //Check first if there are still running instances
    CApaWindowGroupName::FindByAppUid( iUid, iEikEnv->WsSession(), iPrevWgId );
	
	if ( iPrevWgId == KErrNotFound )
	    {
	    LaunchExeL();
	    }
	else
	    {
	    CloseExecutableL();	
	    }
    
	
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::RunError
// 
// ---------------------------------------------------------------------------
//    
TInt CIAUpdateStarter::RunError( TInt aError )
    {
	TRAP_IGNORE( iStarterObserver->StartExecutableCompletedL( aError ) );
    iStarterObserver = NULL;
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CIAUpdateStarter::LaunchExeL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateStarter::LaunchExeL()
    {
    // One second delay before launching because sometimes launching does not succeed immediately 
    if ( !iProcessStartTimer )
        {
        iProcessStartTimer = CIAUpdateUITimer::NewL( *this, CIAUpdateUITimer::EProcessStartDelay );
        }
    else
        {
        iProcessStartTimer->Cancel();
        }
    iProcessStartTimer->After( 1000000 );
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::CloseExecutableL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateStarter::CloseExecutableL() 
    {
	CApaWindowGroupName* wgName = CApaWindowGroupName::NewL( iEikEnv->WsSession() );	
	CleanupStack::PushL( wgName );
	wgName->ConstructFromWgIdL( iPrevWgId );
	TBool respondsToShutdownEvent = wgName->RespondsToShutdownEvent();
	CleanupStack::PopAndDestroy( wgName );
	if( respondsToShutdownEvent )
	    {
	    TApaTask task( iEikEnv->WsSession() );
	    task.SetWgId( iPrevWgId );

	    iThread.Close();
	    User::LeaveIfError(iThread.Open(task.ThreadId()));
			
	    iProcess.Close();
	    User::LeaveIfError(iThread.Process(iProcess));
	    iProcess.Logon( iStatus );
				
	    task.SendSystemEvent( EApaSystemEventShutdown );
        SetActive(); 	
        }
    else  //Just give up 
	    {
	    iStarterObserver->StartExecutableCompletedL( KErrNone );
        iStarterObserver = NULL;	
	    }
    } 



// ---------------------------------------------------------------------------
// CIAUpdateStarter::UidForExecutableL
// Find Uid for executable
// ---------------------------------------------------------------------------
//    
void CIAUpdateStarter::UidForExecutableL( TUid& aUid ) const
    {
    aUid = TUid::Null();
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession );
    lsSession.GetAllApps();
    TApaAppInfo appInfo;
    TInt err = KErrNone;
    while ( err == KErrNone )
        {
        err = lsSession.GetNextApp( appInfo );
        TFileName fullName = appInfo.iFullName;	
        TParse parse;
        parse.Set( fullName, NULL, NULL);    
        if ( parse.NameAndExt().CompareF( *iCommandLineExecutable ) == 0 )
            {
            aUid = appInfo.iUid;
          	err = RApaLsSession::ENoMoreAppsInList;
            }
        }
    CleanupStack::PopAndDestroy( &lsSession ); 
    }
 
// ---------------------------------------------------------------------------
// CIAUpdateStarter::ProcessStartDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::ProcessStartDelayComplete( TInt /*aError*/ )
    {
    delete iProcessStartTimer;
    iProcessStartTimer = NULL;
    TRAPD( err, StartProcessL() );
    TRAP_IGNORE( iStarterObserver->StartExecutableCompletedL( err ) );
    iStarterObserver = NULL;
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::BackgroundDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::BackgroundDelayComplete( TInt /*aError*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::ForegroundDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::ForegroundDelayComplete( TInt /*aError*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// CIAUpdateStarter::StartProcessL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateStarter::StartProcessL()
    {
    RProcess process;
    TInt err = process.Create( *iCommandLineExecutable, KNullDesC );
    if ( err != KErrNone )
        {
        return; // process is already running or some error, nothing to do than return 
        }
    else
        {
        CleanupClosePushL( process );
        CApaCommandLine* commandLine = CApaCommandLine::NewLC();
        commandLine->SetDocumentNameL( KNullDesC );
        commandLine->SetExecutableNameL( *iCommandLineExecutable );
        commandLine->SetCommandL( EApaCommandBackground );
        if ( iCommandLineArguments->Length() > 0 )
            {
            commandLine->SetTailEndL( *iCommandLineArguments );
            }
        commandLine->SetProcessEnvironmentL( process );
        CleanupStack::PopAndDestroy( commandLine );

        process.Resume();
        CleanupStack::PopAndDestroy( &process );
                        
        iExeStarted = ETrue;
        }
    }



// End of File  
