/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main source file for startup list updater.
*
*/

#include "startuplistupdater.h"

_LIT( KStartupListUpdaterThread, "StartupListUpdater" );


// ---------------------------------------------------------------------------
// StartupListUpdaterL()
// ---------------------------------------------------------------------------
//
void StartupListUpdaterL()
    {
    User::LeaveIfError( User::RenameThread( KStartupListUpdaterThread ) );
    RProcess::Rendezvous( KErrNone );

    CStartupListUpdater* updater = CStartupListUpdater::NewL();
    CleanupStack::PushL( updater );
    updater->UpdateStartupListL();
    CleanupStack::PopAndDestroy( updater );
}


// ---------------------------------------------------------------------------
// E32Main()
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt returnCode = KErrNone;
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    if( cleanup )
        {
        CActiveScheduler* scheduler = new CActiveScheduler();
        if( scheduler )
            {
            CActiveScheduler::Install( scheduler );

            TRAP( returnCode, StartupListUpdaterL() );

            delete scheduler;
            delete cleanup;
            }
        else
            {
            delete cleanup;
            return KErrNoMemory;
            }
        }
    else
        {
        return KErrNoMemory;
        }

    __UHEAP_MARKEND;
    return returnCode;
    }

