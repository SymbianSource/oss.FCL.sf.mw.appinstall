/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include <swi/pkgremover.h>
#include "cleanuputils.h"
//#include <swispubsubdefs.h>

using namespace Swi;

_LIT(KTxtEPOC32EX," mainL failed");

LOCAL_C void mainL();
TInt E32Main() // main function called by E32
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,mainL()); // more initialization, then do example
	if(error!=KErrPermissionDenied)
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; // destroy clean-up stack
	return 0; // and return
	}

LOCAL_C void mainL() // initialize and call example code under cleanup stack
	{
	RPointerArray<CUninstalledPackageEntry> uninstalledPkgEntry;
	CleanupResetAndDestroy<RPointerArray<CUninstalledPackageEntry> >::PushL(uninstalledPkgEntry);

	TDriveNumber drive = EDriveX;
	TRAPD(err,UninstalledSisPackages::ListL(drive, uninstalledPkgEntry));

	if(err==KErrPermissionDenied)
		{
		User::LeaveIfError(KErrPermissionDenied);
		}
	CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
	}

