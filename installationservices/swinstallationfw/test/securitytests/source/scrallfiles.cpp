/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @test
*/

#include "scrallfiles.h"

#include <usif/scr/scr.h>
#include <e32def.h>

_LIT(KScrAllFilesSecName, "SCR AllFiles APIs test");

using namespace Usif;

CScrAllFilesSecTest* CScrAllFilesSecTest::NewL()
	{
	CScrAllFilesSecTest* self=new(ELeave) CScrAllFilesSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrAllFilesSecTest::CScrAllFilesSecTest()
	{
	SetCapabilityRequired(ECapabilityAllFiles);
	}
	
void CScrAllFilesSecTest::ConstructL()
	{
	SetNameL(KScrAllFilesSecName);
	}

void CScrAllFilesSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	TInt err(0);
			
	TRAP(err, scrSession.SetIsComponentRemovableL(1, ETrue));
	CheckFailL(err, _L("SetIsComponentRemovableL - set as removable"));
	
	TRAP(err, scrSession.SetIsComponentRemovableL(1, EFalse));
	CheckFailL(err, _L("SetIsComponentRemovableL - set as non-removable"));
	
	CleanupStack::PopAndDestroy(&scrSession);
	}
