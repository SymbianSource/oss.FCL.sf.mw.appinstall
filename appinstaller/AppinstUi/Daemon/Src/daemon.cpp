/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include "daemon.h"
#include "drivewatcher.h"
#include "SWInstDebug.h"
#include "securitypolicy.h"
#include "sisregistrysession.h"

namespace Swi
{
// CDaemon

/*static*/ CDaemon* CDaemon::NewL(MDaemonBehaviour& aBehaviour)
    {
    CDaemon* self=NewLC(aBehaviour);
    CleanupStack::Pop(self);
    return self;
    }

/*static*/ CDaemon* CDaemon::NewLC(MDaemonBehaviour& aBehaviour)
    {
    CDaemon* self=new(ELeave) CDaemon(aBehaviour);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
  
CDaemon::~CDaemon()
    { 
    iPolicyLockFile.Close();
  
    iWatchers.ResetAndDestroy();
    iFs.Close();
    }

CDaemon::CDaemon(MDaemonBehaviour& aBehaviour)
    : iBehaviour(aBehaviour)
    {
    }

void CDaemon::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
  
    LockSensitiveFiles();
    StartRecoveryL();
  
    iBehaviour.StartupL();
    StartWatchersL();
    }

void CDaemon::StartWatchersL()
    {
    FLOG( _L("Daemon: Start watchers") );        
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.ShareProtected());
  
    TDriveList driveList;
  
    User::LeaveIfError(fs.DriveList(driveList));

    for (TInt drive=0; drive < KMaxDrives; ++drive)
        {
        if (driveList[drive] == 0)
            {
            continue;
            }
        TDriveInfo info;
        FLOG_1( _L("Daemon: Checking drive %d"), drive );                
        // Ignore errors since the next drive might work
        if (KErrNone != fs.Drive(info, drive))
            {
            FLOG_1( _L("Daemon: drive error = %d"), fs.Drive(info, drive) );  
            /// @todo, log error
            continue;
            }
        
        FLOG_1( _L("Daemon: info.iDriveAtt %d"), info.iDriveAtt );      
        FLOG_1( _L("Daemon: iDriveAtt & KDriveAttRemovable %d"), 
            (info.iDriveAtt & KDriveAttRemovable) );        
        FLOG_1( _L("Daemon: iDriveAtt & KDriveAttLogicallyRemovable %d"), 
            info.iDriveAtt & KDriveAttLogicallyRemovable );
    
        if (info.iDriveAtt & KDriveAttRemovable || 
            info.iDriveAtt & KDriveAttLogicallyRemovable )
            {   
            FLOG( _L("Daemon: Ok Drive is Removable or LogicallyRemovable") );                
            CDriveWatcher* driveWatcher=CDriveWatcher::NewLC(iFs, drive, iBehaviour);
            User::LeaveIfError(iWatchers.Append(driveWatcher));
            CleanupStack::Pop(driveWatcher);
            }

      
#ifdef __WINS__
        // If drive is not seen as removable in wins env., check if 
        // there is Daemon folder and start watcher if needed.        
        if ( ! ( info.iDriveAtt & KDriveAttRemovable || 
                info.iDriveAtt & KDriveAttLogicallyRemovable) )
            {
            // Watcher is not needed for C.
            if ( drive == 2 )
                {
                continue;
                }
                
            FLOG( _L("Daemon: Drive found. Check private folder") );                      
            _LIT( KDaemonPrivateFolder,":\\private\\10202dce\\" );                                   
            TChar driveLetter;
            User::LeaveIfError( fs.DriveToChar( drive, driveLetter ) );
                                                          
            CDir* list = NULL;        
            HBufC* path = HBufC::NewLC( KMaxFileName );
            TPtr pathPtr = path->Des();
            pathPtr.Append( driveLetter );
            pathPtr.Append( KDaemonPrivateFolder );          
                                
            // Check if folder exists in this drive.                       
            if ( KErrNone == iFs.GetDir( pathPtr, KEntryAttNormal, ESortNone, list ) )
                {     
                FLOG_1( _L("Daemon: Private folder found from drive %d."), drive );        
                
                CDriveWatcher* driveWatcher = 
                    CDriveWatcher::NewLC( iFs, drive, iBehaviour );
                
                User::LeaveIfError( iWatchers.Append( driveWatcher ) );                
                CleanupStack::Pop( driveWatcher );                
                }

            CleanupStack::Pop( path );                                              
            }
#endif  //__WINS__  
      
        }
    CleanupStack::PopAndDestroy(&fs);
    } // StartWatchersL()


void CDaemon::LockSensitiveFiles()
    {
    TInt err = iPolicyLockFile.Open( 
        iFs, 
        KSwisPolicyFile, 
        EFileShareReadersOnly | EFileRead);
  
    FLOG_1( _L("Daemon: Policy file lock error = %d."), err );  
    }

  
void CDaemon::StartRecoveryL()
    { 
    FLOG( _L("Daemon: StartRecoveryL: Check to see if a recovery is needed.") );  
    RSisRegistrySession registrySession;
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
  
    // A recovery failure should not affect SWI Daemon start up.
    TRAP_IGNORE( registrySession.RecoverL() );
  
    CleanupStack::PopAndDestroy( &registrySession );
  
    FLOG( _L("Daemon: StartRecoveryL: Recovery check finished.") ); 
    } // StartRecoveryL()

} // namespace Swi
