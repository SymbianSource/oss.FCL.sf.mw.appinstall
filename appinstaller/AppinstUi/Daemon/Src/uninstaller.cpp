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

#include "uninstaller.h"
#include "versionRevisor.h"
#include "SWInstDebug.h"

#include "sisregistrysession.h"
#include "siscontents.h"
#include "sisparser.h"
#include "filesisdataprovider.h"

#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sisregistryentry.h"
#include "DialogWrapper.h"

using namespace Swi;

const static TInt KWaitTime = 3000000; // 2 secs
const TInt KMimeTextLength = 64;

// -----------------------------------------------------------------------
// Two phased construction
// -----------------------------------------------------------------------
//   
CSisPkgUninstaller* CSisPkgUninstaller::NewL( 
    CProgramStatus& aMainStatus, 
    CVersionRevisor& aRevisor )
    {
    CSisPkgUninstaller* self = new (ELeave) CSisPkgUninstaller();
    CleanupStack::PushL(self);
    self->ConstructL( aMainStatus, aRevisor );
    CleanupStack::Pop(self);
    return self;
    }
 

// -----------------------------------------------------------------------
// c++ constructor
// -----------------------------------------------------------------------
//   
CSisPkgUninstaller::CSisPkgUninstaller() : CActive( CActive::EPriorityStandard )      
    {  
    CActiveScheduler::Add( this );          
    }


// -----------------------------------------------------------------------
// c++ destructor
// -----------------------------------------------------------------------
//
CSisPkgUninstaller::~CSisPkgUninstaller()
    { 
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller start"));
    
    if ( EStateUninstalling == iProgramStatus->GetProgramStatus() )
        {   
        FLOG( _L("[SISUninstaller] Set status to idle"));
        iProgramStatus->SetProgramStatusToIdle();
        } 
    
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: Cancel()"));
    // Cancels the wait for completion of an outstanding request.
    Cancel();
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: Deque()"));
    // Removes the active object from the active scheduler's list.
    Deque();
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: iTimer.Close()"));
    iTimer.Close();    
    iPkgUidArray.Close();  
    delete iSilentUninstaller; 
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: delete iMime"));
    delete iMime;  
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: delete iDialogs"));
    delete iDialogs;      
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: iApaSession.Close()"));
    iApaSession.Close();            
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller: iFs.Close();"));
    iFs.Close();     
   
    FLOG( _L("[SISUninstaller] ~CSisPkgUninstaller end"));
    }
	

// -----------------------------------------------------------------------
// 2nd phase construction
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::ConstructL( 
    CProgramStatus& aMainStatus, 
    CVersionRevisor& aRevisor )
    {       
    iProgramStatus = &aMainStatus;    
    iRevisor = &aRevisor;
    User::LeaveIfError( iTimer.CreateLocal() );    
    User::LeaveIfError( iFs.Connect() );    
    User::LeaveIfError( iApaSession.Connect() );    
    iFs.ShareProtected();
    iUidArrayIndex = 0;
    iSilentUninstaller = NULL;
    iState = EUninstallerStateIdle;   
    
    // Alloc descriptor for mime type sis/sisx    
    iMime = HBufC::NewL( KMimeTextLength );
    TPtr iMimePtr = iMime->Des();
    iMimePtr.Copy( SwiUI::KSisxMimeType );
         
    // Note this will create new instance for dialog class and
    // also new dialog watcher AO for uninstaller.
    // Since uninstaller is used rarely this is not big issue.
    // In future dialog class could be added to CProgramStatus class 
    // member from where both installer and uninstaller could us it.    
    iDialogs = CDialogWrapper::NewL( iFs );      
    }


// -----------------------------------------------------------------------
// This function adds package uid to uninstall array.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::AddUidToListL( const TUid& aUID )
    {    
    iPkgUidArray.AppendL( aUID );    
    }
 		

// -----------------------------------------------------------------------
// This function starts uninstall process. Leaves if nothing to do.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::StartUninstallL()
    {    
    FLOG( _L("[SISUninstaller] StartUninstallL"));
    
    if( iPkgUidArray.Count() == 0 )
         {
         FLOG( _L("[SISUninstaller] No UIDs leave uninstaller"));
         User::Leave( KErrAbort );         
         }
    
    // If daemon installer is in idle, start uninstall and
    // set process status as uninstalling.
    if ( EStateIdle == iProgramStatus->GetProgramStatus() )
        {
        FLOG( _L("[SISUninstaller] StartUninstallL: Set EStateUninstalling") );
        iProgramStatus->SetProgramStatus( EStateUninstalling );
        }               
     
    if ( iSilentUninstaller == NULL )
        {
        FLOG( _L("[SISUninstaller] Create SilentUninstaller"));
        iSilentUninstaller = CSilentUninstaller::NewL( iFs );
        }
    
    if ( iState == EUninstallerStateIdle )
        {       
        CompleteSelf();
        }                 
    }


// -----------------------------------------------------------------------
// Complete the request manually.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::CompleteSelf()
    {  
    FLOG( _L("[SISUninstaller] CompleteSelf"));
    
    if ( !IsActive() )
        {        
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }      
    }


// -----------------------------------------------------------------------
// Cancel the active request.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::DoCancel()
    { 
    FLOG( _L("[SISUninstaller] DoCancel") ); 
    iTimer.Close();      
    if ( iState == EUninstallerStateUninstalling )
        {                    
        iSilentUninstaller->Cancel();
        }           
    }


// -----------------------------------------------------------------------
// Handles the active object's request completion. Process the uninstall
// tasks and waits install process if needed.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::RunL()
    {
    FLOG_1( _L("[SISUninstaller] RunL: ProgramStatus = %d"),
            iProgramStatus->GetProgramStatus() );    
    
    // If daemon installer is running let's wait some time and try again.
    if ( EStateInstalling == iProgramStatus->GetProgramStatus() )
        {
        FLOG( _L("[SISUninstaller] RunL: Wait daemon installer ! ! !") ); 
        TTimeIntervalMicroSeconds32 time( KWaitTime ); 
        iTimer.After( iStatus, time );
        SetActive();        
        }    
    // Ok, run uninstaller.
    else
        {
        FLOG( _L("[SISUninstaller] RunL: Start uninstalling process") );        
        iProgramStatus->SetProgramStatus( EStateUninstalling );        
        FLOG_1( _L("Daemon Uninstaller: RunL: SWI iStatus = %d"), 
                iStatus.Int()); 
        
        switch ( iState )
            {             
            case EUninstallerStateUninstalling:
                {
                FLOG( _L("[SISUninstaller] RunL: EUninstallerStateUninstal."));                                        
                // We are not checking SWI error status here because 
                // there is not need for that. If uninstall fails because 
                // some error, we can not fix it. In case of error we will 
                // start next uninstall.
                                
                // Ok, server busy, we have to wait.
                if ( iStatus.Int() == SwiUI::KSWInstErrBusy )
                    {
                    FLOG(_L("[SISUninstaller] RunL: iStatus: KSWInstErrBusy"));                                       
                    TTimeIntervalMicroSeconds32 time( KWaitTime ); 
                    iTimer.After( iStatus, time );
                    iState = EUninstallerStateBusy;
                    SetActive(); 
                    break;
                    }                                                                                                      
                // Check if we have more to uninstall
                else if ( iUidArrayIndex < iPkgUidArray.Count() )
                    {
                    FLOG(_L("[SISUninstaller] RunL: Complete self and cont."));
                    // Run again to kick of the next uninstallation.
                    iState = EUninstallerStateIdle;                 
                    CompleteSelf();
                    } 
                // Ok all is done.
                else
                    {                    
                    FLOG( _L("[SISUninstaller] RunL: Completed") );                     
                    UninstallationCompleted( KErrNone );
                    ExitUninstaller();
                    }
                }
                break;
                
            case EUninstallerStateIdle:
                 {  
                 FLOG( _L("[SISUninstaller] RunL: EDSisInstallerStateIdle") );   
                 // Check if there is pkgs to be uninstalled.
                 if ( iUidArrayIndex < iPkgUidArray.Count() )
                     { 
                     // Display installing note for user.
                     iDialogs->ShowWaitingNoteForUninstallerL(); 
                     
                     if ( iUidArrayIndex == 0 )
                         {
                         // Set uninstall mode for universal indicator and
                         // set percent value to 0%.
                         iDialogs->SetModeToIndicatorL( KSWIDaemonUninstallerMode );
                         }
                     else
                         {
                         // Calculate current percent value to iPercentValue. 
                         CalcPercentValue();
                         // Activate new value to universal indicator.
                         iDialogs->ActivateIndicatorL( iPercentValue );
                         }
                     
                     FLOG_1( _L("[SISUninstaller] Run uninstall index = %d"), 
                             iUidArrayIndex ); 
                     
                     // Get next UID. 
                     iUID = iPkgUidArray[ iUidArrayIndex ];
                     // Increas pkg counter.
                     iUidArrayIndex++;                                   
                     // Start uninstall process
                     iSilentUninstaller->UninstallL( iUID, iStatus, *iMime );
                     //iDialogs->ShowWaitingNoteL(); 
                     iState = EUninstallerStateUninstalling;       
                     SetActive();                                                                                                                                 
                     }
                 else
                     {
                     // No more pkgs to uninstall. Let's quit.                     
                     FLOG( _L("[SISUninstaller] RunL: Completed") );
                     UninstallationCompleted( KErrNone );
                     ExitUninstaller();
                     }                        
                 }                        
                 break;

            case EUninstallerStateBusy:
                {
                FLOG( _L("[SISUninstaller] RunL: EUninstallerStateBusy") );
                FLOG_1( _L("[SISUninstaller] Run uninstall again index = %d"),
                        iUidArrayIndex-1 );
                // Start uninstall process                                   
                iSilentUninstaller->UninstallL( iUID, iStatus, *iMime );                 
                iState = EUninstallerStateUninstalling;       
                SetActive();                                                              
                }
                break;
                
            default:
                 FLOG(_L("[SISUninstaller] RunL: KErrNotSupported EXIT!!!"));
                 // This state is not supported, let's exit.
                 DoCancel();
                 UninstallationCompleted( KErrNotSupported );
                 ExitUninstaller();                 
                 break;                     
            } //switch case
        } //if-else      
    }
	

// -----------------------------------------------------------------------
// This function is called when RunL leaves. 
// -----------------------------------------------------------------------
//
TInt CSisPkgUninstaller::RunError( TInt aError )
    { 
    FLOG_1( _L("[SISUninstaller] RunError: error = %d"), aError );
    UninstallationCompleted( aError );
    return aError;    
    }		


// -----------------------------------------------------------------------
// This function is called after uninstall tasks are done.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::UninstallationCompleted( TInt /*aResult*/ )
    {
    FLOG( _L("[SISUninstaller] UninstallationCompleted") );
    // Update indicator value last time and show 100% to user.
    iDialogs->ActivateIndicatorL( 100 );
    
    if ( EStateUninstalling == iProgramStatus->GetProgramStatus() )
         {   
         FLOG( _L("[SISUninstaller] Set status to idle"));
         iProgramStatus->SetProgramStatusToIdle();
         }     
 
    iUidArrayIndex = 0;    
    iState = EUninstallerStateIdle; 
          
    // Cancel waiting note.
    TRAP_IGNORE( iDialogs->CancelWaitingNoteForUninstaller());
    // Close indicator.
    iDialogs->CancelIndicatorL();
    }  


// -----------------------------------------------------------------------
// This is for self exit.
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::ExitUninstaller()
    {
    FLOG( _L("[SISUninstaller] ExitUninstaller") );            
    // Uninstall is completed and we do not need uninstaller anymore.
    // Call revisor's destructor.
    iRevisor->Exit();
    }

// -----------------------------------------------------------------------
// CSisInstaller::CalcPrecentValue
// -----------------------------------------------------------------------
//
void CSisPkgUninstaller::CalcPercentValue()
    {     
    FLOG( _L("[SISUninstaller] CalcPercentValue") );
    FLOG_1( _L("[SISUninstaller] iUidArrayIndex = %d"), iUidArrayIndex ); 
    // Let's calculate indicator value for UI now.
    TInt packageUidCount = iPkgUidArray.Count();
    FLOG_1( _L("[SISUninstaller] pkg UID count = %d"), packageUidCount ); 
    
    iPercentValue = 0;

    if ( iUidArrayIndex && packageUidCount )
        {
        // Let's calculate new precent value.     
        if ( iUidArrayIndex <= packageUidCount )
            {
            TReal32 realArrayIndex = iUidArrayIndex;
            TReal32 realPkgCount = packageUidCount;
            iPercentValue = (realArrayIndex/realPkgCount)*100;                                   
            }
        else
            {
            // Most probably all is uninstalled if index is bigger then
            // filen count. Let's not show over 100% to user.           
            iPercentValue = 100;
            }
        }
            
    FLOG_1( _L("[SISUninstaller] percent value = %d"), (TInt)iPercentValue );
    }

//EOF
