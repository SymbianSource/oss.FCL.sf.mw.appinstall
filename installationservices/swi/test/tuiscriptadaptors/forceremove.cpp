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

#include "forceremove.h"

#include "testutilclientswi.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "cleanuputils.h"
#include "swi/sisregistryentry.h"
#include "swi/sisregistrypackage.h"
#include "swi/sisregistrysession.h"
#include "sisregistryaccess_client.h"
#endif

TVerdict CForceRemove::doTestStepL()
	{

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Wait up to 30 seconds to ensure both SWIS and the sisregistry server
	// have shut down
	_LIT(KSisRegistryServerName, "!SisRegistryServer");
	_LIT(KInstallServerName, "!InstallServer");
	TInt delaytime = 30; 

	while (delaytime-- > 0)
		{
		TFullName serverName;
		TFindServer find(KInstallServerName);
		if (KErrNotFound == find.Next(serverName))
			{
			find.Find(KSisRegistryServerName);
			if (KErrNotFound == find.Next(serverName))
				{
				break;
				}
			}
		User::After(1000000); // wait a second until the next test
		}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	TPtrC name;
	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);
	
	// If file deletion fails we'll try moving the file to a temp directory
	// for another process to clean up later.
	
	TTime currentTime;
	currentTime.UniversalTime();
	
	_LIT(KTempPathFormat, "\\temp\\%Lu");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> tempPathFormat(sysDrive.Name());
	tempPathFormat.Append(KTempPathFormat);
	
	TFileName targetPath;
	targetPath.Format(tempPathFormat, currentTime.Int64());
	
	_LIT(KNumFiles, "numfiles");

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RArray<TUint> removeUids;
	CleanupClosePushL(removeUids);
#endif

	TInt num(0);
	GetIntFromConfig(ConfigSection(), KNumFiles, num);

	// Get the file names and use testutil to remove them
	for (TInt i = 0; i < num; ++i)
		{
		_LIT(KFile, "file");
		TBuf<32> key(KFile);
		key.AppendNum(i);
		
		if (!GetStringFromConfig(ConfigSection(), key, name))
			continue;
			
		INFO_PRINTF2(_L("ForceRemove - trying to delete file %S"), &name);

		TInt err = testutil.Delete(name);
		if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
			{
			INFO_PRINTF3(_L("RTestUtilSessionSwi::Delete(%S) returned %d, attempting move instead."), &name, err);
			TFileName source(name);
			TParsePtr parse(source);
			TFileName dest(targetPath);
			dest.Append(parse.Path());
			if (parse.DrivePresent())
				{
				dest[0] = source[0];
				}
			testutil.MkDirAll(dest);
			dest.Append(parse.NameAndExt());

			err = testutil.Move(source, dest);
			
			if (err != KErrNone)
				{
				INFO_PRINTF4(_L("Attempt to move from %S to %S returned %d"),
							&source, &dest, err);
				}
			}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		_LIT(KRegistryPath, "\\sys\\install\\sisregistry\\");
		if (name.FindF(KRegistryPath) == KErrNotFound)
			continue;

		// Extract the uid and add it to our list
		TInt slashPos = name.LocateReverse('\\');
		TPtrC ptrUid = name.Mid(slashPos + 1);
		if (ptrUid.Length() != 8)
			continue;

		TUint uid = 0; 
		TLex lex(ptrUid);
		if (lex.Val(uid, EHex) == KErrNone)
			{
			removeUids.InsertInOrder(uid);	// Will not allow duplicates
			}
#endif
		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Use the sisregistry folder uid to remove the package entries from SCR
	// Note that we remove packages present in ROM too due to two reasons:
	// This is exactly what ForceRemove used to do (delete entire registry folder containing ALL reg files)
	// After deleting an entry SWIRegistry re-registers ROM apps (missing ones)
	TInt count = removeUids.Count();

	RSisRegistryAccessSession regWrite;
	TInt err = regWrite.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(regWrite);

	Swi::RSisRegistrySession regRead;
	User::LeaveIfError(regRead.Connect());
	CleanupClosePushL(regRead);
	for (TInt i = 0; i < count; ++i)
		{ 
		TUid uid = TUid::Uid(removeUids[i]);
		Swi::RSisRegistryEntry entry;
		CleanupClosePushL(entry);
		if (entry.Open(regRead, uid) == KErrNone)
			{
			// Get its augmentations
			RPointerArray<Swi::CSisRegistryPackage> augmentations;
			CleanupResetAndDestroy<RPointerArray<Swi::CSisRegistryPackage> >::PushL(augmentations);
			
			entry.AugmentationsL(augmentations);
			TInt augCount = augmentations.Count();
			for (TInt j = 0; j < augCount; ++j)
				{
				// Remove it
				TInt dummyTime;
				regWrite.DeleteEntryL(*augmentations[j], dummyTime);
				}
			// Finally remove the base package entry
			Swi::CSisRegistryPackage* package = entry.PackageL();
			CleanupStack::PushL(package);
			TInt dummyTime;
			regWrite.DeleteEntryL(*package,dummyTime);
			CleanupStack::PopAndDestroy(2, &augmentations); // package
			}
		CleanupStack::PopAndDestroy(&entry);
		}
	CleanupStack::PopAndDestroy(3, &removeUids);	// regWrite, regRead
#endif
	
	CleanupStack::PopAndDestroy(&testutil);
	
	return EPass;
	}
