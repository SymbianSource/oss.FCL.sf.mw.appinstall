/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

	// Format for metadata ( Ensure that nothing is added in between to maintain compatability) :
	// Drive ( for Base SA ) + No of Controllers ( SA + PU + SP ) + Controllers for SA and PU +
	// Controllers for Augmentation + File Count + File Names Array ( for SA,PU and SP ) + 
	// Augmentation Drives + Matching Supported Languages ( for SA and SP with ELANGNONE as separator )
		
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

        TInt augmentationCount(packages.Count());
        TInt augCount = augmentationCount;
		
		RBufWriteStream stream(*tempBuffer);
		CleanupClosePushL(stream);
        stream.WriteUint32L(drive); 
        
		if (entry.RemovableL()) //Cannot Backup non-removable (NR) packages
	    {
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
            
			}
		else
		    {
                if(!augmentationCount)
                {
                    DEBUG_PRINTF2(_L("RBackupSession::GetMetaDataL - Cannot Backup non-removable application; Uid: '0x%08x'"),aPackageUid.iUid);
                    CleanupStack::PopAndDestroy(6, &entry); // stream, controllers, matchingSupportedLanguagesArray, tempBuffer, packages, entry
                    User::Leave(KErrNotSupported); // cannot remove non-removable (NR) packages 
                }
                else
                {
                for (TInt i = 0; i < augmentationCount; ++i)
                    {           
                    RSisRegistryEntry augmentation;
                    augmentation.OpenL(iSession, *packages[i]);
                    CleanupClosePushL(augmentation);
                    
                     if (!augmentation.RemovableL())
                        {                      
                            augCount--;
                            CleanupStack::PopAndDestroy(1, &augmentation);
                            continue;
                        }
                     CleanupStack::PopAndDestroy(1, &augmentation);
                    }
                if(!augCount)
                    {
                    DEBUG_PRINTF2(_L("RBackupSession::GetMetaDataL - Cannot Backup non-removable augmentation; Uid: '0x%08x'"),aPackageUid.iUid);
                    CleanupStack::PopAndDestroy(6, &entry); // stream, controllers, matchingSupportedLanguagesArray, tempBuffer, packages, entry
                    User::Leave(KErrNotSupported); // cannot remove non-removable (NR) packages 
                    }
                else
                    {
                    stream.WriteInt32L(augCount); // Writing removable augmentations controller count  
                    if (entry.IsInRomL())
                        {
                        matchingSupportedLanguagesArray.Close();
                        }
                    }
                }
		    }

		RArray<TChar> augmentationDrives;
		CleanupClosePushL(augmentationDrives);
		
		TBool nonRemovableAugmentation = ETrue;
		
		for (TInt i = 0; i < augmentationCount; ++i)
			{			
			RSisRegistryWritableEntry augmentation;
			augmentation.OpenL(iSession, *packages[i]);
			CleanupClosePushL(augmentation);
			
			if(augCount != augmentationCount)
			    {
                 if (!augmentation.RemovableL()) //if loops are not clubed to avoid augmentation.RemovableL() API Call
                    {		          
                        CleanupStack::PopAndDestroy(1, &augmentation);
                        continue;
                    }
			    }
		    nonRemovableAugmentation = EFalse;
		    
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
			augmentation.GetMatchingSupportedLanguagesL(matchingSupportedLanguagesArray);

			// Appending ELangNone at the end as marker for the end of the language IDs
			// as we also append the language ID's from other Augmentations
			matchingSupportedLanguagesArray.AppendL(ELangNone);

			CleanupStack::PopAndDestroy(2, &augmentation);
			}
			
		if(nonRemovableAugmentation && augmentationCount)
		    {
            DEBUG_PRINTF2(_L("RBackupSession::GetMetaDataL - Cannot Backup non-removable Augmentation; Uid: '0x%08x'"),aPackageUid.iUid);
            CleanupStack::PopAndDestroy(7, &entry); // augmentationDrives, stream, controllers, matchingSupportedLanguagesArray, tempBuffer, packages
            User::Leave(KErrNotSupported); // cannot remove non-removable (NR) packages 
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
  		for (TInt k = 0; k < augCount; ++k)
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
