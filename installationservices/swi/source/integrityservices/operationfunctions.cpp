/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
 @internalTechnology
*/

#include "operationfunctions.h"

#include "integritytree.h"
#include "log.h"

using namespace Swi;

void Swi::IntegrityDeleteFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, 
							   RLoader& aLoader, CFileMan& aFileMan)
	{
	RBuf name;
	name.CreateL(aPath, KMaxFileName);
	CleanupClosePushL(name);
	name.Append(aLeaf->Name());

	TEntry entry;
	TInt err = aFs.Entry(name, entry);
	if (err == KErrNone)
		{
		aFs.SetAtt(name, 0, KEntryAttReadOnly);
		if(entry.IsDir())
			{
			// Make sure to append slash before calling RmDir - otherwise it deletes the parent directory			
			if (name[name.Length()-1] != KPathDelimiter) 
	  			{
  				name.Append(KPathDelimiter);
  				}
			
			TInt err = aFileMan.RmDir(name);
			DEBUG_PRINTF3(_L("Integrity Services - Remove Directory %S err = %d"), &name, err);
			User::LeaveIfError(err);
			}
		else
			{			
			TInt err = aLoader.Delete(name);
			DEBUG_PRINTF3(_L("Integrity Services - Delete File %S err = %d"), &name, err);
			User::LeaveIfError(err);
			}
			
		// prune the directory tree if possible
		RemoveDirectoryTreeL(aFs, name);
		}
	else if(err != KErrNotFound && err != KErrPathNotFound)
		{
		DEBUG_PRINTF3(_L("Integrity Services - error %d removing %S"), err, &name);
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&name);
	}

void Swi::IntegrityRestoreFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, 
								RLoader& /*aLoader*/, CFileMan& /*aFileMan*/)
	{
	RBuf name;
	name.CreateL(aPath, KMaxFileName);
	CleanupClosePushL(name);
	name.Append(aLeaf->Name());

	// find the peer file, and check it's a backup.
	CIntegrityTreeLeaf* peer = aLeaf->Peer();
	if (peer->Type() != EBackupFile)
		{
		User::Leave(KErrCorrupt);
		}
	
	TParsePtrC parse(peer->Journal());
	RBuf backup;
	backup.CreateL(parse.DriveAndPath(), KMaxFileName);
	CleanupClosePushL(backup);
	backup.Append(parse.Name());
	backup.Append(KPathDelimiter);
	backup.Append(peer->Name());

	TInt err = aFs.MkDirAll(name);
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
			
	err = aFs.Rename(backup, name);
	if (err != KErrNone)
		{
		// we may have already moved it back during a previous recovery
		// attempt, check for its presence in the original location
		if(err == KErrNotFound || err == KErrPathNotFound)
			{
			TEntry restoredEntry;
			err = aFs.Entry(name, restoredEntry);
			}
		User::LeaveIfError(err);
		}
	else
		{
		// prune the backup directory tree if possible.
		RemoveDirectoryTreeL(aFs, backup);
		}
	CleanupStack::PopAndDestroy(2, &name);	// backup
	}
	
void Swi::RemoveDirectoryTreeL(RFs& aFs, const TDesC& aFileName)
	{
	TParse directory;
	User::LeaveIfError(directory.SetNoWild(aFileName, NULL, NULL));
	while(!directory.IsRoot())
		{
		// try to remove this directory
		TInt err = aFs.RmDir(directory.DriveAndPath());
		if(err == KErrInUse || err == KErrAccessDenied)
			{
			break;
			}
		if(err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
			{
			User::Leave(err);
			}
		// move to deleted directory's parent
		User::LeaveIfError(directory.PopDir());
		}	
	}
