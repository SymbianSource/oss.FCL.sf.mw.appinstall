/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "usiflog.h"

namespace Usif
{

TBool IsBinary(const TEntry& aEntry)
    {
    return (aEntry[0].iUid == KExecutableImageUidValue || aEntry[0].iUid == KDynamicLibraryUidValue) ? ETrue : EFalse;
    }


void VerifyDeletionErrorL(TInt err)
	{
	if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
		User::Leave(err);
	}

void IntegrityDeleteFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, 
							   RLoader& aLoader, CFileMan& aFileMan)
	{
    _LIT(KSysBin, "\\sys\\bin");
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
			User::LeaveIfError(aFileMan.RmDir(name));
			}
		else
			{			
            if ( aLeaf->Type() == EBackupFile ) // Implies a commit operation is in progress
                {
                
                 if ( IsBinary(entry) )
                     {
                     // Forming the file name so the renamed file can be under sys/bin
					 // for special delete mechanism using RLoader::Delete
                     RBuf tmpName;
                     TParsePtrC fileName(name);
                     tmpName.CreateL(name.Length() + KSysBin.iTypeLength);
                     CleanupClosePushL(tmpName);

                     tmpName.Append(fileName.Drive());
                     tmpName.Append(KSysBin);
                     tmpName.Append(fileName.Path());
                     tmpName.Append(fileName.NameAndExt());

					 DEBUG_PRINTF3(_L("Integrity Services - Renaming %S to %S"), &name, &tmpName);
                     aFileMan.Rename(name,tmpName,CFileMan::EOverWrite);
                     User::LeaveIfError(aLoader.Delete(tmpName)); // Using RLoader delete for paged binaries
					 DEBUG_PRINTF2(_L("Integrity Services - Deleted renamed file %S"), &tmpName);

					 // prune the directory tree if possible
                     RemoveDirectoryTreeL(aFs, tmpName);
                     CleanupStack::PopAndDestroy(&tmpName);
                     }
                 else
                     {
                     User::LeaveIfError(aFileMan.Delete(name));
                     }
                }
            else
                {
				// Need to use RLoader Delete which can be used during deletion of Added files during Rollback
                User::LeaveIfError(aLoader.Delete(name));
                }
			}
			
		// prune the directory tree if possible
		RemoveDirectoryTreeL(aFs, name);
		}
	else if(err != KErrNotFound && err != KErrPathNotFound)
		{
		DEBUG_PRINTF3(_L("Integrity Services - error %d removing %S"), err, &name);
		User::Leave(err);
		}
	else
	    {

		DEBUG_PRINTF3(_L("Integrity Services - error %d removing %S"), err, &name);

	    // Check for any renamed files to move it to sys/bin for special delete mechanism
	    RBuf tmpName;
	    TParsePtrC fileName(name);
	    tmpName.CreateL(name.Length() + KSysBin.iTypeLength);
	    CleanupClosePushL(tmpName);

	    tmpName.Append(fileName.Drive());
	    tmpName.Append(KSysBin);
	    tmpName.Append(fileName.Path());
	    tmpName.Append(fileName.NameAndExt());
		DEBUG_PRINTF2(_L("Integrity Services - Removing  %S renamed binary files if any"), &tmpName);

	    aLoader.Delete(tmpName);
		// prune the directory tree if possible
	    RemoveDirectoryTreeL(aFs, tmpName);
	    CleanupStack::PopAndDestroy(&tmpName);
	    }

	CleanupStack::PopAndDestroy(&name);
	}

void IntegrityRestoreFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, 
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
		VerifyDeletionErrorL(err);
		// we may have already moved it back during a previous recovery
		// attempt, check for its presence in the original location
		TEntry restoredEntry;
		User::LeaveIfError(aFs.Entry(name, restoredEntry));
		}
	else
		{
		// prune the backup directory tree if possible.
		RemoveDirectoryTreeL(aFs, backup);
		}
	CleanupStack::PopAndDestroy(2, &name);	// backup
	}
	
void RemoveDirectoryTreeL(RFs& aFs, const TDesC& aFileName)
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
		VerifyDeletionErrorL(err);		
		// move to deleted directory's parent
		User::LeaveIfError(directory.PopDir());
		}	
	}

} // end namespace Usif
