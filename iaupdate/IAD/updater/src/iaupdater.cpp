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
* Description:   IAUpdater programs main.
*
*/



#include <e32base.h>
#include <f32file.h>

#include "iaupdatermngr.h"
#include "iaupdaterdefs.h"
#include "iaupdaterparams.h"
#include "iaupdatedebug.h"


// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// void MainL()
// Programs main function.
// -----------------------------------------------------------------------------
//
void MainL()
    { 
    IAUPDATE_TRACE("[IAUpdater] MainL() begin");

    // Rename thread as IAUpdater.
    User::RenameThread( IAUpdaterDefs::KIAUpdaterName );  

    TIAUpdaterStartParams startParams;       
    startParams.iInitFilePath = IAUpdaterDefs::KInitFilePath;
    startParams.iResultFilePath = IAUpdaterDefs::KResultFilePath;
    
    CIAUpdaterMngr* updater = CIAUpdaterMngr::NewLC();	
       
    // Notice, that this function will not return 
    // before all the installations are done.                
        
    updater->ProcessInstallingL( startParams );    

    CleanupStack::PopAndDestroy( updater ); 

    IAUPDATE_TRACE("[IAUpdater] MainL() end");
    }


// ======== MEMBER FUNCTIONS ========

// ======== GLOBAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// GLDEF_C TInt E32Main()
// Main function called by E32
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    // Start memory leak checkings.
    // This only works in debug builds.
    __UHEAP_MARK;

    IAUPDATE_TRACE("[IAUpdater] E32Main() begin");

    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    CActiveScheduler* scheduler = new CActiveScheduler();

     if ( ( !cleanup ) || ( !scheduler ) )
         {
         delete scheduler;
         delete cleanup;
         return KErrNoMemory;
         }
      
    CActiveScheduler::Install( scheduler );

    TRAPD( err, MainL() );
    
    IAUPDATE_TRACE_1("[IAUpdater] E32Main(): MainL TRAP err = %d", err);
     
    delete scheduler;
    delete cleanup;

    IAUPDATE_TRACE("[IAUpdater] E32Main() end");

    // End memory leak checkings.
    // This only works in debug builds.
    __UHEAP_MARKEND;

    // Notice, that if some applications are listening for rendezvous to complete
    // for this process, the rendezvous will be completed with the error code
    // returned here. So, if the IAD process is on and it is listening the rendezvous
    // of this process, then the rendezvous will be completed here.
    
    return err;	
    }


// EOF
