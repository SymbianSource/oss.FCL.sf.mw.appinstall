/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <e32base.h>
#include <e32uid.h>
#include <f32file.h>

#include "MEclipsingTest.h"

static void MainL()
	{
	
	RLibrary lib;
	
	TUid libraryType = {0x10272FB3};
	TUidType libType(KDynamicLibraryUid, libraryType);
	_LIT(KEclipsingDllName, "eclipsetest.dll");
	
	User::LeaveIfError(lib.Load(KEclipsingDllName, libType));
	CleanupClosePushL(lib);
	
	TLibraryFunction funct = lib.Lookup(1);
	
	if (funct == NULL)
		{
		User::Leave(KErrGeneral);
		}
	
	MEclipsingTest* dllClass = (MEclipsingTest*)(*funct)();
	
	if (dllClass == NULL)
		{
		User::Leave(KErrGeneral);
		}
		
	TInt ver = dllClass->DllVersionQuery();
	delete dllClass;

	CleanupStack::PopAndDestroy(&lib);
	

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	_LIT(KVersionFileNameFormat, "\\eclipsev%d.out");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<32> fileName (sysDrive.Name());
	fileName.Append(KVersionFileNameFormat);
	
	TBuf<64> name;
	name.Format(fileName, ver);
	User::LeaveIfError(file.Replace(fs, name, EFileWrite));
	file.Close();
	
	CleanupStack::PopAndDestroy(&fs);
	
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err=KErrNoMemory;
	if (cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}
