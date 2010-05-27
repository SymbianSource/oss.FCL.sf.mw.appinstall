/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* uuser.cpp
*
*/


#include <e32std.h>
#include <e32cons.h>
#include <e32base.h>

#include "../../inc/swi/msisuihandlers.h"
#include "tuiadaptors.h"
#include "consoleclientserver.h"

using namespace Swi;
using namespace Swi::Test;

LOCAL_D void executeL() // initialize and call example code under cleanup stack
    {
	Swi::Test::RConsoleServerSession consoleSession;
	CleanupClosePushL(consoleSession);

	User::LeaveIfError(consoleSession.Connect());

//	CConsoleBase* console = Console::NewL(_L("SWI Install Test"), TSize(KDefaultConsWidth, KDefaultConsHeight));
	
//	CleanupStack::PushL(console);
	MUiHandler* handler = SisUiAdapter::NewLC(consoleSession);
	CleanupStack::PopAndDestroy(handler);
	CleanupStack::PopAndDestroy(&consoleSession);
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(executeL());
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
