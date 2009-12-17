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


#include "backuprestore.h"

#include <s32mem.h>

#include "sisregistryentry.h"
#include "sisregistrywritableentry.h"
#include "siscontroller.h"
#include "cleanuputils.h"
#include "sisregistrypackage.h"
#include "log.h"
 

namespace Swi
	{
	
	EXPORT_C TInt RBackupSession::Connect()
		{
		DEBUG_PRINTF(_L8("RBackupSession::Connect"));
		return iSession.Connect();		
		}
		
	EXPORT_C void RBackupSession::Close()
		{
		DEBUG_PRINTF(_L8("RBackupSession::Close"));
		iSession.Close();		
		}
		
	EXPORT_C HBufC8* RBackupSession::GetMetaDataL(TUid aPackageUid, RPointerArray<HBufC>& aFilesArray)
		{
		DEBUG_PRINTF2(_L8("Getting backup meta data for puid=0x%08x"), aPackageUid.iUid);
		RSisRegistryWritableEntry entry;
		User::LeaveIfError(entry.Open(iSession, aPackageUid));
		CleanupClosePushL(entry);
		
		RPointerArray<HBufC8> controllers;
		entry.ControllersL(controllers);
		CleanupResetAndDestroyPushL(controllers);
		
		const TInt KDefaultBufferSize = 2048;
		
		CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
		CleanupStack::PushL(tempBuffer);
		
		// Include the drive letter at the head of the metadata
		TChar drive = entry.SelectedDriveL();
		RArray<TLanguage> matchingSupportedLanguagesArray;
		CleanupClosePushL(matchingSupportedLanguagesArray);
		entry.GetMatchingSupportedLanguagesL(matchingSupportedLanguagesArray);
		// Appending ELangNone at the end as marker for the end of the language IDs
		// as we also append the language ID's from Augmentations
		matchingSupportedLanguagesArray.AppendL(ELangNone);

		RPointerArray<CSisRegistryPackage> packages;
		CleanupResetAndDestroyPushL(packages);
		entry.AugmentationsL(packages);

		RBufWriteStream stream(*tempBuffer);
		CleanupClosePushL(stream);
		stream.WriteUint32L(drive);		
		
		// backup the base package (except for PA and ROM stubs) plus partial upgrades and augmentations.		
		entry.FilesL(aFilesArray);
		
		
		// Filter out any files on the Z drive
		TInt fileIndex = aFilesArray.Count() - 1;
		while (fileIndex >= 0)
			{
			TChar driveCh = (*aFilesArray[fileIndex])[0];
			if (driveCh == 'z' || driveCh == 'Z')
				{
				delete aFilesArray[fileIndex];
				aFilesArray.Remove(fileIndex);
				}
			--fileIndex;
			}

		TInt controllersCount = controllers.Count();
		TInt firstController = 0;
		if (entry.PreInstalledL() || entry.IsInRomL())
			{
			firstController = 1;	// Don't backup the controller for the ROM stub 
			if (controllersCount <= 1)
				{				
				DEBUG_PRINTF(_L8("Base package is pre-installed or in ROM and has not been upgraded"));				
				stream.WriteInt32L(packages.Count());
				if (entry.IsInRomL())
                    {
                    matchingSupportedLanguagesArray.Close();
                    }
				}			
			else 
				{
				// Backup any PUs for the ROM stub.
				DEBUG_PRINTF2(_L8("Found %d partial upgrades"), controllersCount - 1);				
				stream.WriteInt32L(controllersCount - 1 + packages.Count());
				}
			}
		else
			{
			// Standard SA package
			DEBUG_PRINTF2(_L8("Found %d controllers"), controllersCount);		
			stream.WriteInt32L(controllers.Count() + packages.Count());			
			}
		
		for (TInt i = firstController; i < controllersCount; ++i)
			{
			stream << *controllers[i];
			}
		
		TInt augmentationCount(packages.Count());
		RArray<TChar> augmentationDrives;
		CleanupClosePushL(augmentationDrives);
		
		for (TInt i = 0; i < augmentationCount; ++i)
			{			
			RSisRegistryEntry augmentation;
			augmentation.OpenL(iSession, *packages[i]);
			CleanupClosePushL(augmentation);
			
			augmentation.FilesL(aFilesArray);
			
			RPointerArray<HBufC8> augmentationControllers;
			CleanupResetAndDestroyPushL(augmentationControllers);
			augmentation.ControllersL(augmentationControllers);

			stream << *augmentationControllers[0];
			
			DEBUG_PRINTF3(_L("Augmentation %d installed to drive %d"), i, augmentation.SelectedDriveL().GetNumericValue());
			// The drives for all SPs need to be appended after the file to
			// list to preserve BC with backup data created before SP drives
			// were backed up.
			augmentationDrives.AppendL(augmentation.SelectedDriveL());
			// Get the matching supported languages
			entry.GetMatchingSupportedLanguagesL(matchingSupportedLanguagesArray);

			// Appending ELangNone at the end as marker for the end of the language IDs
			// as we also append the language ID's from other Augmentations
			matchingSupportedLanguagesArray.Append(ELangNone);

			CleanupStack::PopAndDestroy(2, &augmentation);
			}
			
		// Write the number of files originally installed,
  		// and their names to the backup metadata		
  		TInt filesCount(aFilesArray.Count());
		DEBUG_PRINTF2(_L8("Files installed originally %d"), filesCount);
  		stream.WriteInt32L(filesCount);
  		for (TInt i = 0; i < filesCount; ++i)
  			{
  			stream << *aFilesArray[i];
  			}

		// Add the selected drive for each augmentation in the same order as
		// as the list of controllers.  The augmentation controllers will
		// be counted on restore to be sure to correct number of drives.
  		for (TInt k = 0; k < augmentationCount; ++k)
  			{
			stream.WriteUint32L(augmentationDrives[k]);
  			}

		//Add the array of matching device supported languages
  		TInt supportedLanguageCount = matchingSupportedLanguagesArray.Count();
  		
		stream.WriteInt32L(supportedLanguageCount);
  		for (TInt k = 0; k < supportedLanguageCount; ++k)
  			{
			stream.WriteInt32L(matchingSupportedLanguagesArray[k]);
  			}
        
		stream.CommitL();
		
		HBufC8* result = tempBuffer->Ptr(0).AllocL();
		CleanupStack::PopAndDestroy(7, &entry); // augmentationDrives, stream, controllers, matchingSupportedLanguagesArray, tempBuffer, packages
		return result; 
		
		}	
	
	}
