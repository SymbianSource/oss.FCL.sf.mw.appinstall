/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Basic console application for installation testing
*
*/


#include "sistest.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>            // Console

_LIT( KTextConsoleTitle, "Console" );
_LIT( KTextFailed, " failed, leave code = %d" );
_LIT( KTextPressAnyKey, " [press any key]\n" );

LOCAL_D CConsoleBase* console;

LOCAL_C void MainL()
    {
    console->Write(_L("Hello, world!\n"));
    }

LOCAL_C void DoStartL()
    {
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    CleanupStack::PopAndDestroy(scheduler);
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAPD( createError, console = Console::NewL( KTextConsoleTitle,
            TSize( KConsFullScreen,KConsFullScreen ) ) );
    if( createError )
        {
        return createError;
        }

    TRAPD( mainError, DoStartL() );
    if( mainError )
        {
        console->Printf(KTextFailed, mainError);
        }
    console->Printf(KTextPressAnyKey);
    console->Getch();

    delete console;
    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }

