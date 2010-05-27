/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:       Test app for StartupListManagement API
*
*/


#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>


_LIT( KTestMessage, "Test Application started" );

CConsoleBase* console;


void callTestL( )
    {
    console = Console::NewL( _L( "MyTestApp" ), TSize( KConsFullScreen, KConsFullScreen ) );
    CleanupStack::PushL( console );

    console->Printf( _L("\nThis is a test application lauched during startup") );

    while( 1 ) {
        User::After( 5000000 );
    }

    CleanupStack::PopAndDestroy( console );
    }

GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD( error, callTestL() );
    __ASSERT_ALWAYS( !error, User::Panic( KTestMessage, error ) );
    delete cleanup;
    return 0;
    }

