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

/**
 * @file
 * @internalComponent
 */
#include <e32base.h>
#include <f32file.h>

#include "daemon.h"
#include "daemonbehaviour.h"
#include "versionRevisor.h"  // For uninstaller
#include "SWInstDebug.h"     // For uninstaller
#include "programstatus.h"   // For uninstaller
#include "uninstallerdefs.h" // For uninstaller

_LIT(KSWIDaemonInstaller,"SWI Daemon"); // Thread name

namespace Swi
{

/**
 * Daemon main function
 */
void MainL()
    {
    FLOG( _L("Daemon: MainL start") );  
    FLOG( _L("Daemon: Rename thread to SWI Daemon") );
    // Rename thread as SWI Daemon.
    User::RenameThread( KSWIDaemonInstaller );  

    // For uninstaller
    // Set status to installing just to be sure that uninstaller 
    // do not run before installing in complete.
    CProgramStatus* mainStatus = CProgramStatus::NewLC( EStateInstalling ); 
  
    CDaemonBehaviour* daemonBehaviour=CDaemonBehaviour::NewLC( *mainStatus );
    CDaemon* daemon=CDaemon::NewLC(*daemonBehaviour);

#ifdef __SWIDAEMON_ENABLE_UNINSTALLER 
    // For uninstaller
    // Note do not push revisor to CleanupStack. It will exit itself when needed.
    CVersionRevisor* revisor = CVersionRevisor::NewL( *mainStatus );
    TInt err;
    TRAP( err, revisor->StartProcessL() );
    if ( err )
        {
        FLOG( _L("Daemon: StartProcessL leaves, delete revisor") );
        delete revisor;
        }
#endif //__SWIDAEMON_ENABLE_UNINSTALLER    
  
    FLOG( _L("Daemon: MainL: RProcess::Rendezvous") );
    RProcess::Rendezvous( KErrNone );
    FLOG( _L("Daemon: MainL: CActiveScheduler::Start") );
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 3, mainStatus );//mainStatus,daemonBehav.,daemon 
    }

} // namespace Swi

/**
 * Entry Point, sets up the cleanup stack and calls the main function.
 * @return Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* scheduler=new CActiveScheduler();

    if(cleanup == NULL || scheduler==NULL)
        {
        delete scheduler;
        delete cleanup;
        return KErrNoMemory;
        }
        
    CActiveScheduler::Install(scheduler);
    
    TRAP_IGNORE(Swi::MainL());

    delete scheduler;
    delete cleanup;
    
    return KErrNone;
    }

