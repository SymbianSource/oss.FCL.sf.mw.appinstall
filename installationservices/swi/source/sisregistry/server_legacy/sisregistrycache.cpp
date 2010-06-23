/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisRegistryCache class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include "log.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "swi/sisregistrylogversion.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "arrayutils.h"
#include "sisregistrycache.h"
#include "sisregistrypackage.h"
#include "sisregistryserverconst.h"
#include "sisregistryfile.h"
#include "sisregistryobject.h"

#include "integrityservices.h"
#include "installtypes.h"
#include "dessisdataprovider.h"
#include "filesisdataprovider.h"
#include "controllerinfo.h"
#include "hashcontainer.h"
#include "sisregistryfiledescription.h"
#include "siscontroller.h"
#include "sistruststatus.h"
#include "sisregistryserver.h"
#include "sisregistrytoken.h"
#include "siscertificatechain.h"
#include "cleanuputils.h"
#include "sisregistryutil.h"
#include "swispubsubdefs.h"
#include <sacls.h>
#include <connect/sbdefs.h>
#include "securitypolicy.h"
#include "sislangchangemonitor.h"

using namespace Swi;

_LIT(KTempLog,"\\sys\\install\\templog.txt");

const TInt KUidHexLength = 8;

TUint CSisRegistryCache::iSubsessionId = 0;

CSisRegistryCache* CSisRegistryCache::NewL()
	{
	CSisRegistryCache* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSisRegistryCache* CSisRegistryCache::NewLC()
	{
	CSisRegistryCache* self = new(ELeave) CSisRegistryCache;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CSisRegistryCache::~CSisRegistryCache()
	{
	TInt res;
	// General note: trap the error because this is not a leaving function
	// and generating an error will not help 

	// store the backup file out 
 	TRAP(res, StoreBackupL());
 	if (res != KErrNone)
 		{
 	DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to store backup (failure code: %d.)"), res);
 		}

	// integrity service operation committing point 
	TRAP(res, iIntegrityService->CommitL());
	if (res != KErrNone)
 		{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to commit integrity services changes (failure code %d.)"), res);
 		}

	delete iBackupFile;
	
	delete iIntegrityService;
	delete iSisRegLangMonitor;
	CleanUp();
	iFs.Close();
	
	}
	
void CSisRegistryCache::ConstructL()
	//
	// 2nd phase construct 
	//
	{	
	User::LeaveIfError(iFs.Connect());
	
	RBuf backupFile;
	backupFile.CreateL(KMaxFileName);
	backupFile.CleanupClosePushL();
	backupFile.Append(KSysBackupFile);
	backupFile[0] = iFs.GetSystemDriveChar();
	iBackupFile = backupFile.AllocL();
	
	CleanupStack::PopAndDestroy(&backupFile); //backupFile

	iSystemDrive = iFs.GetSystemDrive();
	iUseIntegServices = ETrue;
	
	// private integrity service initialized by time 
	TTime currentTime;
	currentTime.UniversalTime(); 
	iIntegrityService = CIntegrityServices::NewL(currentTime.Int64(), KIntegrityServicesPath);
	
	// if backup file exists, read it and delete file - the short cut
	// otherwise revert to reading from registry directories and building registry from scratch
	TInt res = KErrNone;
	TRAP(res , res = IsFirmwareUpdatedL());
	if ( !res )
	    {
        DEBUG_PRINTF(_L8("Sis Registry Server - No Firware upgrade detected. Trying to intialize from cache "));
	    TRAP(res, InitFromBackupL());
	    isFwUpdated = EFalse;
	    }
	else
	    {
        DEBUG_PRINTF(_L8("Sis Registry Server - Firware upgrade detected. Initializing from registry/stub entries"));
        res = KErrGeneral;
        isFwUpdated = ETrue;
	    }
	
	// if the backup file has not been found or it was garbled or else
	if (res != KErrNone) 
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Construction from backup failed (Error code %d). Proceeding with re-init."),
			res);
		
		// InitFromBackupL() failed, there might have some entries left on the cache
		CleanUp();

		InitStartUpL();
		UpdateLocalizedNamesL();
		}  
	//Start language change monitor.
	iSisRegLangMonitor = CSisLangChangeMonitor::NewL(*this);
		}
		
void CSisRegistryCache::CleanUp()
	{
	iTokens.ResetAndDestroy();
	iFiles.ResetAndDestroy();
	}
	
void CSisRegistryCache::UidListL(RArray<TUid>& aUids) const
	{
	aUids.Reset();
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		TInt err = aUids.InsertInUnsignedKeyOrder((iTokens[i])->Uid());
		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			User::Leave(err);
			}
		}
	}

void CSisRegistryCache::PackageListL(RPointerArray<CSisRegistryPackage>& aPackages) const
	{
	aPackages.ResetAndDestroy();
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		CSisRegistryPackage *package = CSisRegistryPackage::NewLC(*iTokens[i]);
		aPackages.AppendL(package);
		CleanupStack::Pop(package);
		}
	}
	
RFs& CSisRegistryCache::RFsHandle()
	{ 
	return iFs;
	}

void CSisRegistryCache::BuildRomListL()
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Registering In-Rom controllers."));
	
	TDriveUnit romDrive(SystemRomDrive());
	RBuf romRegistryPath;
	romRegistryPath.CreateL(romDrive.Name(), KMaxPath);
	CleanupClosePushL(romRegistryPath);
	romRegistryPath.Append(KPreInstalledPath);
	// open the directory
	CDir* dir;
	TInt err = iFs.GetDir(romRegistryPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
	// check that all is correct but make sure to handle missing directory	
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Reading ROM stub directory returned %d."), err);
	
	if (err == KErrNone)
		{
		CleanupStack::PushL(dir);

		TInt count(dir->Count());
		RBuf controllerFileName;
		controllerFileName.CreateL(KMaxFileName);
		CleanupClosePushL(controllerFileName);
		for (TInt index = 0; index < count; ++index)
			{
			controllerFileName = romRegistryPath;
			controllerFileName.Append((*dir)[index].iName);
			TRAPD(res, RegisterInRomControllerL(controllerFileName));
			if (res != KErrNone)
				{
				// log it only, we cannot stop as the next might be ok
				DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to register in ROM controller. Error code %d."), res);
				}
			}

		CleanupStack::PopAndDestroy(2, dir); // controllerFileName
		}
	else if(err != KErrPathNotFound)
		{
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(&romRegistryPath);
	}
	
void CSisRegistryCache::RegisterInRomControllerL(const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("Sis Registry Server - Registering In-ROM controller file '%S'."), &aFileName);
	
	RFile romController;
	CleanupClosePushL(romController);
	// open file and read the data
	User::LeaveIfError(romController.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	// read the data from the file
	TInt fileSize;
	romController.Size(fileSize);
	HBufC8* controllerData = HBufC8::NewLC(fileSize);
	TPtr8 ptr(controllerData->Des());
	romController.Read(0, ptr, fileSize);
	
	CDesDataProvider* desProvider = CDesDataProvider::NewLC(*controllerData);
	// read the controller
	Swi::Sis::CController* controller = Swi::Sis::CController::NewLC(*desProvider);

	// create an object from the controller
	CSisRegistryObject* object = CSisRegistryObject::NewLC();

	// Discard the 4 byte type field for consistency with
	// all other registry entries
	TPtrC8 assumedTypeController(controllerData->Mid(4));
	object->ProcessInRomControllerL(*controller, assumedTypeController);
	
	TBool overwriteRegEntry = EFalse;
	
    TBool isStubRegistered = IsRegistered(*object);
    if ( isFwUpdated && isStubRegistered )
	    {
	    CSisRegistryObject* tmpObj = ObjectL(object->Uid(),object->Index());
        CleanupStack::PushL(tmpObj);
	    TSisPackageTrust trustStatus = tmpObj->Trust();
	    if ( ESisPackageBuiltIntoRom == trustStatus )
	        {
            DEBUG_PRINTF2(_L8("Sis Registry Server - Attempting to delete registry entry 0x%08x as a firmware update detected"), object->Uid().iUid);
            overwriteRegEntry = ETrue;
            HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(tmpObj->Uid(), tmpObj->Index());
            //Delete in memory copy of Registry token as it is no longer valid
            // Before deleting the reg entry. Otherwise there may inconsistency
            for (TInt i = 0; i < iTokens.Count(); i++)
                {
                if (!(*iTokens[i] == *tmpObj))
                    continue;

                    // remove this token from the array
                    CleanupStack::PushL(iTokens[i]);
                    iTokens.Remove(i);
                    CleanupStack::PopAndDestroy(1); // iTokens[i]
                    break;
                }
            TEntry entry;
            TInt err = iFs.Entry(*name, entry);  
            if ( err==KErrNone )
               {
               iFs.Delete(*name);
               DEBUG_PRINTF2(_L8("Sis Registry Server - Successfully deleted registry entry 0x%08x. "), object->Uid().iUid);
	           }
			   CleanupStack::PopAndDestroy();
	        }
        CleanupStack::PopAndDestroy();
	    }
	
	
	// we cannot guess whether it is registered without opening it
    if (!isStubRegistered || overwriteRegEntry )
		{
		// update cache or just call refresh
		AddEntryL(*object, *iIntegrityService);
		
		// store a copy of the controller
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(object->Uid(), object->Index(),
				0);
		
		SisRegistryUtil::EnsureDirExistsL(iFs, *name);
		RFile newController;
		
		// If stray .ctl files exist (for example, after phone was rebooted in the middle of registration), calling RFile::Create() will result a Leave.
		// Therefore, RFile::Replace() is rather ideal.
		User::LeaveIfError(newController.Replace(iFs, *name, EFileWrite | EFileShareExclusive));
		CleanupStack::PopAndDestroy(name);
		CleanupClosePushL(newController);
		
		// Write the controller, minus the 4 byte type field for consistency with
		// all other registry entries		
		newController.Write(assumedTypeController);
		CleanupStack::PopAndDestroy(&newController);
		}
	
	CleanupStack::PopAndDestroy(5, &romController); //controller, object, desProvider, controllerData
	}
	
void CSisRegistryCache::BuildUidListL()
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Reading registry files from disk."));
	
	TDriveUnit drive(iSystemDrive);
	RBuf registryPath;
	registryPath.CreateL(drive.Name(), KMaxPath);
	CleanupClosePushL(registryPath);
	registryPath.Append(KRegistryPath);
	CDir* dir;
	TInt err = iFs.GetDir(registryPath, KEntryAttDir, ESortNone, dir);
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Reading registry directory returned %d"), err);
	if(err == KErrNone)
		{

		CleanupStack::PushL(dir);
		
		TInt count(dir->Count());
		RBuf uidPath;
		uidPath.CreateL(KMaxFileName);
		CleanupClosePushL(uidPath);
		for (TInt index = 0; index < count; ++index)
			{
	
			const TEntry& entry = (*dir)[index];
			TPtrC ptr(entry.iName);
			// we expect uid directories to be 8 characters long, ignore others
			if (ptr.Length() == KUidHexLength)
				{
				TLex lex(ptr);
				TUint32 uid;
				
				// Convert the filename to a Uint32.
				User::LeaveIfError(lex.Val(uid, EHex));	
				uidPath = registryPath;
				uidPath.Append(entry.iName);
				uidPath.Append(KPathDelimiter);
				//for this uid				
				TRAPD(res, BuildFileListL(TUid::Uid(uid), uidPath););
				if (res != KErrNone )
					{
					// log it only, we cannot stop as the next might be ok
					DEBUG_PRINTF3(_L8("Sis Registry Server - Failed to read from registry entry 0x%08x. Error code %d"),
						uid, res);
					}
				}
			}

		CleanupStack::PopAndDestroy(2, dir); // uidPath

		}
	else
		{
		if (err != KErrPathNotFound)
			{
			User::Leave(err);
			}
		}

	CleanupStack::PopAndDestroy(&registryPath);
	}

void CSisRegistryCache::BuildFileListL(TUid aUid, const TDesC& aPath)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Attempting to read registry entry 0x%08x."), aUid.iUid);

	RBuf registryFileSpec;
	registryFileSpec.CreateL(aPath, KMaxFileName);
	CleanupClosePushL(registryFileSpec);
	registryFileSpec.Append(KRegFileFilter);

	CDir* dir;
	User::LeaveIfError(iFs.GetDir(registryFileSpec, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir));
	CleanupStack::PushL(dir);

	DEBUG_PRINTF2(_L8("Sis Registry Server - Registry entry contains %d registry files."), dir->Count());

	TInt count(dir->Count());
	RBuf fileName;
	fileName.CreateL(KMaxFileName);
	CleanupClosePushL(fileName);
	for (TInt index = 0; index < count; ++index)
		{
		fileName = aPath;
		fileName.Append((*dir)[index].iName);
		RFileReadStream fileStream;
		// open the file 
		User::LeaveIfError(fileStream.Open(iFs, fileName, EFileRead));
		CleanupClosePushL(fileStream); 
		CSisRegistryToken* token = CSisRegistryToken::NewLC(fileStream);
		
		DEBUG_PRINTF5(_L("Sis Registry Server - Entry at index %d, UID: 0x%08x, Name: %S, Vendor: %S."),
			index, token->Uid().iUid, &(token->Name()), &(token->Vendor()));
			
		if (token->Uid() != aUid)
			{
			DEBUG_PRINTF(_L8("Sis Registry Server - Registry file does not match entry directory. Corrupt."));
			User::Leave(KErrCorrupt);	
			}
			
		iTokens.Append(token);
		CleanupStack::Pop(token);
		CleanupStack::PopAndDestroy(&fileStream);
				
		}

	CleanupStack::PopAndDestroy(3, &registryFileSpec); // dir, filename
	}	
	   
void CSisRegistryCache::InitFromBackupL()
	{
	// cleanup the list
	iTokens.ResetAndDestroy();

	RBuf backupFile;
	backupFile.CreateL(KMaxFileName);
	backupFile.CleanupClosePushL();
	backupFile.Append(KSysBackupFileBackup);
	backupFile[0] = iFs.GetSystemDriveChar();

	// if the backup of the cache backup file exists, the cache backup file may
	// be corrupt. Delete it and return immediately. The cache will be regenerated.
	TInt err = iFs.Delete(backupFile);
	
	if (err == KErrNone)
		{
		DEBUG_PRINTF2(_L("CSisRegistryCache::InitFromBackupL deleted %S. Corrupt file?"), &backupFile);
		User::LeaveIfError(KErrNotFound);	
		}
	else if (err != KErrNotFound)
	  	{
	  	User::LeaveIfError(err);
	  	}

	User::LeaveIfError(iFs.Rename(*iBackupFile, backupFile));
	
	// open file as a read stream and read data as a stream
	RFileReadStream stream;
	
	User::LeaveIfError(stream.Open(iFs, backupFile, EFileRead | EFileShareReadersOnly));
	
	CleanupClosePushL(stream);
	TLanguage currentLanguage = User::Language();
	TLanguage storedLanguage = static_cast<TLanguage>(stream.ReadUint32L());
	// Stored Language can't be greater than ELangMaximum
	// Hence Leave as the backupfile may be corrupted
	if ( storedLanguage > ELangMaximum)
		{
		User::Leave(KErrCorrupt);
		}
	InternalizePointerArrayL(iTokens, stream); 
	if ( currentLanguage != storedLanguage)
		{
		UpdateLocalizedNamesL();
		}
	CleanupStack::PopAndDestroy(&stream);
	
	// in the unlikely event that the CommitL() fails in ConstructL() 
	// when we remove this file, it needs to be the same as the original name.
	User::LeaveIfError(iFs.Rename(backupFile, *iBackupFile));

	CleanupStack::PopAndDestroy(&backupFile);

	// remove the file, all the information is in RAM
	// no need to maintain and syncronise the information in two places
	// so integrity services a scheduled to remove the file at next call to commit
	iIntegrityService->RemoveL(*iBackupFile); 
	
	iIntegrityService->CommitL();
			
	// only when all is processed one can check the package state		
	UpdatePackagePresentStateL();
	}
	
void CSisRegistryCache::InitStartUpL()
	{
	// else reinit lists- initial settings, esp at the first reboot
	BuildUidListL();
	iUseIntegServices = EFalse; // temporarily "turn off" integrity services. It is not needed to process ROM stubs
	BuildRomListL();
	iUseIntegServices = ETrue;
	
	// While building the UIDs and the ROM list, new files could've been generated, so we should commit the changes to the FS.
	iIntegrityService->CommitL();
			
	// only when all is processed one can check the package state		
	UpdatePackagePresentStateL();
	
    TRAPD(res, UpdateRecentFWVersionL(););
    if (res != KErrNone)
        {
        // log that
        DEBUG_PRINTF2(_L8("Updating recent Firmware Version failed with error code = %d."), res);
        }
	}

void CSisRegistryCache::StoreBackupL()
	{
	SisRegistryUtil::EnsureDirExistsL(iFs, *iBackupFile);
	iIntegrityService->AddL(*iBackupFile); 
	RFileWriteStream stream;
	CleanupClosePushL(stream);
	User::LeaveIfError(stream.Replace(iFs, *iBackupFile, EFileWrite | EFileShareExclusive));
	// Append the currently running language ID onto backup file
	// This will be used during initialization from backup to decide
	// whether to update the vendor and packages names during initialization.
	stream.WriteUint32L(User::Language());
	ExternalizePointerArrayL(iTokens, stream); 
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	}
	
TBool CSisRegistryCache::IsRegistered(const CSisRegistryPackage& aPackage) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((aPackage.Uid() == iTokens[i]->Uid()) &&
			(aPackage.Name() == iTokens[i]->Name()) &&
		    (aPackage.Index() == iTokens[i]->Index()))
		    
			{
			return ETrue; 
			}
		}
	// if we are here it is false
	return EFalse;	
	}
	
TBool CSisRegistryCache::IsRegistered(const TUid aUid) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if (iTokens[i]->Uid() == aUid)
			{
			return ETrue; 
			}
		}

	// if we are here it is not registered
	return EFalse;	
	}
	
TBool CSisRegistryCache::IsRegistered(const CHashContainer& aHashContainer) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		for (TInt j = 0; j < iTokens[i]->ControllerInfo().Count(); j++)
			{
			if (aHashContainer == iTokens[i]->ControllerInfo()[j]->HashContainer())
				{
				return ETrue; 
				}
			}
		}

	// if we are here it is not registered
	return EFalse;	
	}


CSisRegistryPackage& CSisRegistryCache::PackageL(const TUid aUid) const
	{
	// when requesting a package by UID make sure we return the base
	// package (index = 0) and not an augmentation (index > 0)
	return CSisRegistryCache::PackageL( aUid, CSisRegistryPackage::PrimaryIndex);
	}

CSisRegistryPackage& CSisRegistryCache::PackageL(const TUid aUid, TInt aIndex) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((iTokens[i]->Uid() == aUid) && (iTokens[i]->Index() == aIndex))

			{
			return *iTokens[i];
			}
		}
	User::Leave(KErrNotFound);
	// to keep compiler happy, in reality if cannot find an entry leave above 
	return *iTokens[0];
	}

CSisRegistryPackage& CSisRegistryCache::PackageL(const TDesC& aName, const TDesC& aVendor) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((iTokens[i]->Name() == aName) && 
			(iTokens[i]->Vendor() == aVendor))
			{
			return *iTokens[i];		
			}
		}
	User::Leave(KErrNotFound);
	// to keep compiler happy, in reality if cannot find an entry leave above 
	return *iTokens[0];	
	}


CSisRegistryToken& CSisRegistryCache::TokenL(const TUid aUid, TInt aIndex) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((iTokens[i]->Uid() == aUid) &&
			(iTokens[i]->Index() == aIndex))
			{
			return *iTokens[i];	
			}
		}
	
	User::Leave(KErrNotFound);
	// to keep compiler happy, in reality if cannot find an entry leave above 
	return *iTokens[0];	
	}

CSisRegistryToken& CSisRegistryCache::TokenL(const TUid aUid, const TDesC& aName, const TDesC& aVendor) const
	{
	// Note - vendor not checked as the package Uid is enough to identify
	// the package.
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((iTokens[i]->Uid() == aUid) &&
			(aName == KNullDesC || (iTokens[i]->Name() == aName)) &&
			 (iTokens[i]->Vendor() == aVendor))
			{
			return *iTokens[i];	
			}
		}
	
	User::Leave(KErrNotFound);
	// to keep compiler happy, in reality if cannot find an entry leave above 
	return *iTokens[0];	
	}

CSisRegistryObject* CSisRegistryCache::ObjectL(TUid aUid, TInt  aIndex)
	{
	const CSisRegistryToken& token = TokenL(aUid, aIndex);
	return ObjectL(token);
	}
		
CSisRegistryObject* CSisRegistryCache::ObjectL(TUid aUid, const TDesC& aName, const TDesC& aVendor)
	{
	const CSisRegistryToken& token = TokenL(aUid, aName, aVendor);
	return ObjectL(token);
	}
 
CSisRegistryObject* CSisRegistryCache::ObjectL(const CSisRegistryToken& aToken)
	{
	HBufC* name= SisRegistryUtil::BuildEntryFileNameLC(aToken.Uid(), aToken.Index());
	
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(iFs, *name, EFileRead | EFileShareReadersOnly));
	CleanupStack::PopAndDestroy(name);
	CleanupClosePushL(stream);
	
	// open entry
	CSisRegistryObject *object = CSisRegistryObject::NewL(stream);
	CleanupStack::PopAndDestroy(&stream);
	return object;
	}

CSisRegistryPackage& CSisRegistryCache::SidToPackageL(const TUid aSid) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if (iTokens[i]->SidPresent(aSid))
			{
			return *iTokens[i];			
			}
		}
		
	User::Leave(KErrNotFound);
	// to keep compiler happy, in reality if cannot find an entry leave above 
	return *iTokens[0];	
	}

void CSisRegistryCache::SidToPackageL(const TUid aSid, RArray<CSisRegistryPackage>& aListMatchingPackages) const
	{
		for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if (iTokens[i]->SidPresent(aSid))
			{
			aListMatchingPackages.AppendL(*iTokens[i]); 
			}
		}
	
	DEBUG_PRINTF2(_L("SidToPackageL ListMatchingPackages->Count = %d"), aListMatchingPackages.Count());
	if(aListMatchingPackages.Count() == 0  )
		{
		User::Leave(KErrNotFound);
		}
	}
	
TBool CSisRegistryCache::ModifiableL(const TDesC& aFileName)
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		// Create the object using the token we're looking at.
		CSisRegistryObject* object = ObjectL(iTokens[i]->Uid(),
											 iTokens[i]->Name(),
											 iTokens[i]->Vendor());
		CleanupStack::PushL(object);
		
		// This (FileDescriptionL()) will confirm whether or not the object contains
		// our filename. If it doesn't, then the function will leave with KErrNotFound.
		TInt error;
		TRAP(error, object->FileDescriptionL(aFileName));
		if (error == KErrNone)
			{
			const CSisRegistryFileDescription& d = object->FileDescriptionL(aFileName);
			
			// Since we got this far, it must mean that this object contains our filename and
			// we now can safely find out if the file is modifiable, and return an appropriate
			// Boolean value. If the expression below is True, then the file is modifiable.
			TBool isModifiable = (d.OperationOptions() & Sis::EInstVerifyOnRestore) ? EFalse : ETrue;
			
			// Files under \sys or \resource are never modifiable
			_LIT(KSys, "\\sys\\"); 
			_LIT(KResource, "\\resource\\");			
			if (d.Target().FindF(KSys) == 2 || d.Target().FindF(KResource) == 2)
				{
				isModifiable = EFalse;
				}
			
			CleanupStack::PopAndDestroy(object);
			return isModifiable;
			}
		else if (error == KErrNotFound)
			{
			// This means that the object doesn't contain our filename.
			// Destroy the object so we can continue onto the next iteration.
			CleanupStack::PopAndDestroy(object);
			}
		else
			{
			// If FileDescriptionL() has returned any other error, then leave.
			User::Leave(error);
			}
		}

	User::Leave(KErrNotFound);
	
	// This is only to keep the compiler happy. In reality, if it cannot find
	// the appropriate entry, then it leaves above.
	return EFalse;
	}
	
CHashContainer* CSisRegistryCache::HashL(const TDesC& aFileName)
	{
	CHashContainer* dummyHashContainer = NULL;
	
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		// Create the object using the token we're looking at.
		CSisRegistryObject* object = ObjectL(iTokens[i]->Uid(),
											 iTokens[i]->Name(),
											 iTokens[i]->Vendor());
		CleanupStack::PushL(object);
		
		// This (FileDescriptionL()) will confirm whether or not the object contains
		// our filename. If it doesn't, then the function will leave with KErrNotFound.
		TInt error;
		TRAP(error, object->FileDescriptionL(aFileName));
		if (error == KErrNone)
			{
			const CHashContainer& hashContainer = object->FileDescriptionL(aFileName).Hash();

			// The caller (ie, Swi::CSisRegistrySession::RequestHashL()) needs to take ownership of this.
			CHashContainer* tempHashContainer = CHashContainer::NewL(hashContainer.Algorithm(), hashContainer.Data());

			CleanupStack::PopAndDestroy(object);
			
			// Since we got this far, it must mean that this object contains our filename
			// and we now can safely return its hash.
			return tempHashContainer;
			}
		else if (error == KErrNotFound)
			{
			// This means that the object doesn't contain our filename.
			// Destroy the object so we can continue onto the next iteration.
			CleanupStack::PopAndDestroy(object);
			}
		else
			{
			// If FileDescriptionL() has returned any other error, then leave.
			User::Leave(error);
			}
		}

	User::Leave(KErrNotFound);
	
	// This is only to keep the compiler happy. In reality, if
	// it cannot find the appropriate entry, then it leaves above.
	return dummyHashContainer;
	}

void CSisRegistryCache::PackageAugmentationsL(const TUid aUid, RPointerArray<CSisRegistryPackage>& aPackages) const
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if ((iTokens[i]->Uid() == aUid) && (iTokens[i]->Index() != CSisRegistryPackage::PrimaryIndex))
			{
			CSisRegistryPackage* tmp = CSisRegistryPackage::NewLC(*iTokens[i]);
			aPackages.AppendL(tmp);
			CleanupStack::Pop(tmp);
			}
		}
	}
	
TInt CSisRegistryCache::PackageAugmentationsNumber(const TUid aUid) const
	{
	TInt num = 0;
	TInt count = iTokens.Count();
	for (TInt i = 0; i < count; i++)
		{
		if ((iTokens[i]->Uid() == aUid) && (iTokens[i]->Index() != CSisRegistryPackage::PrimaryIndex))
			{
			num++;
			}
		}
	return num;
	}
	
void CSisRegistryCache::AddEntryL(CSisRegistryObject& aObject, 
									CIntegrityServices& aIntegrityService)
	{
	// set the index
	if (aObject.InstallType() == Sis::EInstAugmentation ||
		aObject.InstallType() == Sis::EInstPreInstalledPatch)
		{
		aObject.SetIndex(SisRegistryUtil::NextAvailableIndexL(iFs, aObject.Uid()));
		}
	
	// resolve the embedded packages index values
	TInt embeddedIndex = 0;
	CSisRegistryPackage* embeddedPackage = NULL;
	while ((embeddedPackage = aObject.EmbeddedPackage(embeddedIndex++)) != NULL)
		{
		ResolveEmbeddedPackage(embeddedPackage);
		}
	
	HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(aObject.Uid(), aObject.Index());
	
	DEBUG_PRINTF2(_L("Sis Registry Server - Adding registry entry at location '%S'"), name);
	
	if (aObject.InstallType() != Sis::EInstAugmentation &&
		aObject.InstallType() != Sis::EInstPreInstalledPatch)
		{
		SisRegistryUtil::EnsureDirExistsL(iFs, *name);
		}
	
	if (UseIntegrityServices())
		{
		// add registry entry to integrity services log
		aIntegrityService.AddL(*name);
		}

	// store	
	RFileWriteStream fileStream;
	User::LeaveIfError(fileStream.Create(iFs, *name, EFileWrite | EFileShareExclusive));
	CleanupStack::PopAndDestroy(name);
	CleanupClosePushL(fileStream);
	
	aObject.ExternalizeL(fileStream);
	
	// commit and close file
	CleanupStack::PopAndDestroy(&fileStream);
	
	// add to token array
	CSisRegistryToken* token = CSisRegistryToken::NewLC(aObject);
	// initialise the token to have all drives listed marked as present
	// (after all, we've just installed it so they must be)
	token->SetFixedDrives(token->Drives());
	User::LeaveIfError(iTokens.Append(token));
	CleanupStack::Pop(token);
	}

void CSisRegistryCache::ResolveEmbeddedPackage(CSisRegistryPackage* aPackage)
	{
	TInt count(iTokens.Count());
	for (TInt i = 0; i < count; ++i)
		{
		if (*aPackage == *iTokens[i])
			{
			aPackage->SetIndex(iTokens[i]->Index());
			break;
			}
		}
	}

void CSisRegistryCache::RemoveEntryL(const CSisRegistryPackage& aPackage,
										CIntegrityServices& aIntegrityService)
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		if (!(*iTokens[i] == aPackage))
			continue;

			// remove this token from the array
			CleanupStack::PushL(iTokens[i]);
			iTokens.Remove(i);
			CleanupStack::PopAndDestroy(1); // iTokens[i]
			
		// close existing file handles (if needed) before removing the file
		CloseAllHandlesForUid(aPackage.Uid());			
			// remove the corresponding registry file 
			HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(aPackage.Uid(), aPackage.Index());
			aIntegrityService.RemoveL(*name);
			CleanupStack::PopAndDestroy(name);
			
			// as there should be only one matching entry we can return
			break;
			}
		}
	
void CSisRegistryCache::OpenReadHandleL(const CSisRegistryPackage& aPackage, TUint& aId)
	{
	CSisRegistryFile* fileHandler = CSisRegistryFile::NewLC(aPackage, *this);
	iFiles.AppendL(fileHandler);
	CleanupStack::Pop(fileHandler);	
	aId = fileHandler->SubsessionId();
	}
	
void CSisRegistryCache::CloseReadHandleL(TUint aId)
	{
	// find the entry
	TInt i = FindReadHandleIndx(aId);
	User::LeaveIfError(i);

	CSisRegistryFile* fileHandler = iFiles[i];
	CleanupStack::PushL(fileHandler);
	iFiles.Remove(i);
	CleanupStack::PopAndDestroy(fileHandler);
	}

CSisRegistryObject& CSisRegistryCache::EntryObjectL(TUint aId)
	{
	TInt i = FindReadHandleIndx(aId);
	
	User::LeaveIfError(i);
	return iFiles[i]->RegistryObject();
	}

CSisRegistryFile& CSisRegistryCache::HandleEntryL(TUint aId)
	{
	TInt i = FindReadHandleIndx(aId);
	
	User::LeaveIfError(i);
	return *iFiles[i];
	}
	

TInt CSisRegistryCache::FindReadHandleIndx(TUint aId)
	{
	for (TInt i=0; i<iFiles.Count(); i++)
		{
		if (iFiles[i]->SubsessionId() == aId)
			{
			return i;
			}	
		}
	return KErrNotFound;
	}

void CSisRegistryCache::DependentsPackagesL(
							   const CSisRegistryObject& aObject,
							   RPointerArray<CSisRegistryPackage>& aDependents
							   )
	{
	aDependents.ResetAndDestroy();
	// if it is an augmentation - nothing depends on it
	if (aObject.InstallType() == Sis::EInstAugmentation || 
		aObject.InstallType() == Sis::EInstPreInstalledPatch)
		{
		return;
		}	
		
	TUid matchingUid = aObject.Uid();
	
	// for all entries in the list
	for (TInt i = 0; i < iTokens.Count(); i++) 
		{
		// add all augmentations
		if (iTokens[i]->Uid() == matchingUid)
			{
			if (iTokens[i]->Index() != CSisRegistryPackage::PrimaryIndex)
				{
				CSisRegistryPackage* tmp = CSisRegistryPackage::NewLC(*iTokens[i]); 
				aDependents.AppendL(tmp);
				CleanupStack::Pop(tmp);
				}
			}
		else
			{
			// have to open to open the file
			HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(iTokens[i]->Uid(), iTokens[i]->Index());
			
			RFileReadStream stream;
			TInt retVal = stream.Open(iFs, *name, EFileRead | EFileShareReadersOnly);
			CleanupStack::PopAndDestroy(name);
			
			if(retVal != KErrNone)
				{//don't halt the process if a registry file is missing or corrupted
				if(retVal == KErrNotFound)
					{
					continue;
					}
				else
					{//Leave if the error is different
					User::Leave(retVal);
					}
				}
			CleanupClosePushL(stream);
			// open entry
			CSisRegistryObject *tmpObject = CSisRegistryObject::NewLC(stream);
			if (tmpObject->DependsOnObject(aObject))
				{
				CSisRegistryPackage* tmp = CSisRegistryPackage::NewLC(*tmpObject);
				aDependents.AppendL(tmp);
				CleanupStack::Pop(tmp);
				}
			// delete entry & stream 
			CleanupStack::PopAndDestroy(2, &stream);// &stream, tmpObject
			}
		}
	}

void CSisRegistryCache::EmbeddingPackagesL(const CSisRegistryObject& aObject,
									   RPointerArray<CSisRegistryPackage>& aEmbeddingPackages)
	{
	aEmbeddingPackages.ResetAndDestroy();
	
	TUid matchingUid = aObject.Uid();
	
	// for all entries in the list
	for (TInt i = 0; i < iTokens.Count(); i++) 
		{
		HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(iTokens[i]->Uid(), iTokens[i]->Index());
		
		RFileReadStream stream;
		User::LeaveIfError(stream.Open(iFs, *name, EFileRead | EFileShareReadersOnly));
		CleanupStack::PopAndDestroy(name);
		CleanupClosePushL(stream);
		
		// open entry
		CSisRegistryObject *tmpObject = CSisRegistryObject::NewLC(stream);
		if (tmpObject->EmbedsPackage(aObject)) 
			{
			CSisRegistryPackage* tmpPackage = CSisRegistryPackage::NewLC(*iTokens[i]) ;
			aEmbeddingPackages.AppendL(tmpPackage);
			CleanupStack::Pop(tmpPackage);	
			}
		// delete entry & stream 
		CleanupStack::PopAndDestroy(2, &stream);// &stream, tmpObject
		}	
	}
	
void CSisRegistryCache::GenerateChainListL(const CSisRegistryObject& aObject, 
											 RPointerArray<HBufC8>& aChainList)	
	{
	aChainList.ResetAndDestroy();
	// read the controller for every member of the list	
	for (TInt i = 0; i < aObject.ControllerInfo().Count(); i++)
		{
		// open every controller to extract the information
		// construct the name using the package and offset
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo()[i]->Offset());
		// open the file
		CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(iFs, *name);
		// read the controller
		Swi::Sis::CController* controller = Swi::Sis::CController::NewLC(*fileProvider, Swi::Sis::EAssumeType);
		// add the data to the list of certificate Chains
		for (TInt j = 0; j < controller->SignatureCertificateChains().Count(); j++)
			{
			HBufC8* tmp= controller->SignatureCertificateChains()[j]->CertificateChain().Data().AllocLC();
			aChainList.AppendL(tmp);
			CleanupStack::Pop(tmp);
			}
		// release the data	
		CleanupStack::PopAndDestroy(3, name); // fileProvider, controller
		}	
	}

HBufC8* CSisRegistryCache::LoadControllerLC(const CSisRegistryObject& aObject, TUint aIndex)
	{
	// construct the name using the package and offset
	HBufC* fileName = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo()[aIndex]->Offset());
	// open the file
	RFile file;
	// open file and read the data
	User::LeaveIfError(file.Open(iFs, *fileName, EFileRead | EFileShareReadersOnly));
	CleanupStack::PopAndDestroy(fileName);
	CleanupClosePushL(file);
	// read the data from the file
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	// create a buffer according to file size and load it with data
	HBufC8* buffer = HBufC8::NewLC(fileSize);
	TPtr8 ptr(buffer->Des());
	User::LeaveIfError(file.Read(0, ptr, fileSize));
	CleanupStack::Pop(buffer); 
	// cleanup
	CleanupStack::PopAndDestroy(&file);	
	CleanupStack::PushL(buffer);
	return buffer;
	}

void CSisRegistryCache::GenerateControllersArrayL(const CSisRegistryObject& aObject, 
												RPointerArray<HBufC8>& aControllers)
	{
	CleanupResetAndDestroyPushL(aControllers);
	aControllers.ResetAndDestroy();
	
	// read the controller for every member of the list	
	TInt count = aObject.ControllerInfo().Count();
	for (TInt i = 0; i < count; ++i)
		{
		HBufC8* buffer = LoadControllerLC(aObject, i);
		// add it to the list 
		aControllers.AppendL(buffer);
		CleanupStack::Pop(buffer);
		}
	CleanupStack::Pop(&aControllers);
	}	
		
void CSisRegistryCache::AddControllerL(const CSisRegistryObject& aObject, 
									CIntegrityServices& aIntegrityService,
									const TDesC8& aBuffer, const TInt aDrive)		
	{
	HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo().Count()-1, aDrive);
	DEBUG_PRINTF2(_L("Sis Registry Server - Installing controller file to '%S'."), name);

	TEntry entry;
	if (KErrNone == iFs.Entry(*name, entry))
		{
		// Remove any existing file with the same name
		aIntegrityService.RemoveL(*name);
		}

	// add controller to integrity services log
	aIntegrityService.AddL(*name);

	// store
	SisRegistryUtil::EnsureDirExistsL(iFs, *name);
		
	RFileWriteStream fileStream;
	User::LeaveIfError(fileStream.Create(iFs, *name, EFileWrite|EFileShareExclusive));
	CleanupStack::PopAndDestroy(name);
	CleanupClosePushL(fileStream);
	fileStream.WriteL(aBuffer);
 
	// commit and close file
	CleanupStack::PopAndDestroy(&fileStream);
 
	}
										
void CSisRegistryCache::AddCleanupInfrastructureL(CSisRegistryObject& aObject, CIntegrityServices& aIntegrityServices, const TDesC8& aControllerBuffer)
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Adding Cleanup Infrastructure."));
	
	RArray<TInt> driveArray;
	CleanupClosePushL(driveArray);
	ControllerDriveListL(aObject, driveArray);
	TInt driveCount(driveArray.Count());
	for (TInt i = 0; i < driveCount; ++i)
		{
		TInt drive = driveArray[i];
		// Skip drives which are not present.  Note that this will only be
		// valid for filenull operations, but we will otherwise have failed
		// earlier in installation planning so we don't need to check
		// the operation again.
		TVolumeInfo volInfo;
		if (KErrNone != iFs.Volume(volInfo, drive))
			{
			continue;
			}

		// Create a flagging controller on the drive in question
		AddControllerL(aObject, aIntegrityServices, aControllerBuffer, drive);

		// There is no need to create uninstall logs on the system drive; all
		// other drives in the list should be removable.
		// We can also skip writing uninstall logs if the object is in ROM,
		// since uninstallation should never be possible.
		if (drive == iSystemDrive || aObject.InRom())
			{
			continue;
			}

		TChar driveLetter;
		User::LeaveIfError(iFs.DriveToChar(driveArray[i], driveLetter));
		driveLetter.Fold();

		HBufC* name = SisRegistryUtil::BuildUninstallLogPathLC(aObject.Uid(), aObject.Index(), driveLetter);
		DEBUG_PRINTF2(_L("Sis Registry Server - Adding cleanup infrastructure uninstall log '%S'."), name);

		TEntry entry;
		if (KErrNone == iFs.Entry(*name, entry))
			{
			// Remove any uninstall logs with this path
			aIntegrityServices.RemoveL(*name);
			}

		// Make sure the journal is cleaned up if we abort this install
		aIntegrityServices.AddL(*name);

		// Also cover the uninstall log path on the target drive with
		// integrityservices.
		TPtr namePtr(name->Des());
		namePtr[0] = driveLetter;
		if (KErrNone == iFs.Entry(*name, entry))
			{
			aIntegrityServices.RemoveL(*name);
			}

		aIntegrityServices.AddL(*name);

		// Create the journal
		TTime currentTime;
		currentTime.UniversalTime();
		CIntegrityServices* uninstallLog = CIntegrityServices::NewLC(currentTime.Int64(), *name);

		// Register the flagging controller and any files on this drive for cleanup
		HBufC* controller = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo().Count()-1, drive);

		uninstallLog->AddL(*controller);
		TInt filesCount(aObject.FileDescriptions().Count());
		
		TInt j;
		for (j = 0; j < filesCount; ++j)
			{
			const TDesC& target = aObject.FileDescriptions()[j]->Target();
			TChar targetDrive = target[0];
			targetDrive.Fold();
			
			if (driveLetter == targetDrive)
				{
				DEBUG_PRINTF2(_L("Sis Registry Server - Adding file '%S' cleanup uninstall log"), &target);
				uninstallLog->AddL(target);
				}
			}
		
		// Add any potential private paths as targets for cleanup operations
		
		_LIT(KPrivatePath, "?:\\private\\");
		TInt sidCount(aObject.Sids().Count());
		RBuf privatePath;
		privatePath.CreateL(KMaxFileName);
		CleanupClosePushL(privatePath);
		for (j = 0; j < sidCount; ++j)
			{
			privatePath = KPrivatePath;
			privatePath[0] = driveLetter;
			privatePath.AppendNumFixedWidthUC(aObject.Sids()[j].iUid, EHex, KUidHexLength);
			privatePath.Append(KPathDelimiter);
			
			DEBUG_PRINTF2(_L("Sis Registry Server - Adding (potential) private path '%S' to cleanup uninstall log"), &privatePath);
			uninstallLog->AddL(privatePath);
			}

		CleanupStack::PopAndDestroy(4, name); //uninstallLog, controller, privatePath
		}

	CleanupStack::PopAndDestroy(&driveArray); 
	
	}									
		
void CSisRegistryCache::RemoveCleanupInfrastructureL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityServices)
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Removing cleanup infrastructure."));
	
	RArray<TInt> driveArray;
	CleanupClosePushL(driveArray);
	ControllerDriveListL(aObject, driveArray);
	TInt count(driveArray.Count());
	for (TInt i = 0; i < count; ++i)
		{
		
		TChar drive;
		User::LeaveIfError(RFs::DriveToChar(driveArray[i], drive)); 
		
		// Check if the drive exists before trying to do anything with it,
		// otherwise ignore it and continue to the next drive.
		// Also skip write-protected media for pre-installed packages.
		// An entry registered as a preinstalled package should only have a
		// drive in this array when it is a package installed from a removable
		// media stub (also known as a propagation) and the swipolicy indicates
		// that such packages are deletable.
		TVolumeInfo volInfo;
		if (KErrNone == iFs.Volume(volInfo, driveArray[i]) &&
			(!aObject.PreInstalled() ||
			!(volInfo.iDrive.iMediaAtt & KMediaAttWriteProtected)))
			{
			
			// remove the controller(s) for this registry entry and drive
			if (RemoveControllerL(aObject, aIntegrityServices, driveArray[i]))
				{
				// if the controller was sucessfully removed
				// remove the uninstall logs both for this drive, and the system drive
				HBufC* name = SisRegistryUtil::BuildUninstallLogPathLC(aObject.Uid(), aObject.Index(), drive);

				DEBUG_PRINTF2(_L("Sis Registry Server - Attempting to remove cleanup infrastructure uninstall log '%S'."),
					name);

				TRAPD(err, aIntegrityServices.RemoveL(*name));
				if (err != KErrNotFound && err != KErrNone && err != KErrPathNotFound && err != KErrCorrupt)
					{
					DEBUG_PRINTF2(_L("Sis Registry Server - Failed to remove uninstall log (unexpected error: %d.)"), err);
					User::Leave(err);
					}
			
				TPtr namePtr(name->Des());
				namePtr[0] = drive;
				TRAP(err, aIntegrityServices.RemoveL(*name));
				if (err != KErrNotFound && err != KErrNone && err != KErrPathNotFound && err != KErrCorrupt)
					{
					User::Leave(err);
					}
				CleanupStack::PopAndDestroy(name);
				}
			}
		
		}
	
	CleanupStack::PopAndDestroy(&driveArray);
	
	}

TBool CSisRegistryCache::RemoveControllerL(const CSisRegistryObject& aObject,
	CIntegrityServices& aIntegrityServices, TInt aDrive)
	{
	TInt count(aObject.ControllerInfo().Count());
	TBool removed = ETrue;
	for (TInt i = 0; i < count; i++)
		{
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo()[i]->Offset(), aDrive);
		DEBUG_PRINTF2(_L("Sis Registry Server - Removing flagging controller '%S'."), name);

		TRAPD(err, aIntegrityServices.RemoveL(*name));
		if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound && err != KErrCorrupt)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Removal of flagging controller failed (Unexpected error %d.)"), err);
			User::Leave(err);
			}
		else if (err != KErrNone)
			{
			removed = EFalse;
			}
		CleanupStack::PopAndDestroy(name);
		}
	return removed;
	}
	
// creates a compacted array using a token
void CSisRegistryCache::ControllerDriveListL(const CSisRegistryObject& aObject,
										 RArray<TInt>& aDriveList)
	{
	aDriveList.Reset();
	// a copy of the controller is always kept on drive C
	aDriveList.Append(iSystemDrive);
	
	// only controllers will be written to removable media and 
	// we have now to check for those 
	TUint installationDrives  = aObject.Drives();
	TUint fixedDrives = FixedDrives();
	TUint remainingDrives = installationDrives & ~fixedDrives;

		if (remainingDrives)
			{
			TInt index = 0;
			// reuse the path but change drive letter
			while (remainingDrives)
				{
				// compare a single drive digit
				if (remainingDrives & 0x00000001)
					{
					User::LeaveIfError(aDriveList.Append(index)); 
					}
				remainingDrives>>=1;
				index++;
				}
			}
	}

	
void CSisRegistryCache::InitDrivesL()
	{
	// reset the permenant drive bitmap
	iPermDrives = 0;
	
	TDriveList driveList; 
	if(iFs.DriveList(driveList) != KErrNone)
		{
		return; // Can not get drive list so abort drive scan.
		}
	
	TInt err;
	for (TInt drive = 0; drive < KMaxDrives; drive++)
		{
		if (driveList[drive] == 0)
			{
			continue;
			}
			
		TDriveInfo info;
		
		// Ignore errors since the next drive might work
		if (KErrNone != (err = iFs.Drive(info, drive)))
			{
			continue;
			}

		if (info.iDriveAtt & KDriveAttInternal)
			{
			// add it to our store of known fixed drives
			iPermDrives |= (1 << drive);	
			DEBUG_PRINTF2(_L8("Sis Registry Server - Fixed drive %d detected."), drive);
			}
		else if (info.iDriveAtt & KDriveAttRemovable)
			{
			// check if volume is present
			TVolumeInfo volumeInfo;
			err = iFs.Volume(volumeInfo, drive);
			if (err == KErrNone)
				{
				DEBUG_PRINTF2(_L8("Sis Registry Server - Removable drive %d detected."), drive);
				// Find flagging controllers for non-preinstalled packages on
				// this drive.
				DiscoverUidsL(drive);
				// Find stub sis files on this drive - needed for preinstalled
				// packages which won't have flagging controllers.
				DiscoverStubsL(drive);
				}
			}	
		}

	TInt tokenCount(iTokens.Count());
	for (TInt i = 0; i < tokenCount; i++)
		{
		iTokens[i]->SetFixedDrives(iPermDrives);
		}
			
	
	}

void CSisRegistryCache::DiscoverUidsL(TInt aDrive)
	{
	TDriveUnit drive(aDrive);
	// create directory path
	RBuf regPath;
	regPath.CreateL(drive.Name(), KMaxFileName);
	CleanupClosePushL(regPath);
	regPath.Append(KRegistryPath);
	
	CDir* dir;
	TInt err = iFs.GetDir(regPath, KEntryAttDir, ESortNone, dir);
	CleanupStack::PushL(dir);
	if(err == KErrNone)
		{
		TInt count(dir->Count());
		for (TInt i = 0; i < count; ++i)
			{
			// want to be able to trap the failure, log it and process the next controller
			// in this way a corrupt controler would not hog the processing
			TRAPD(res, DiscoverControllersL(regPath, (*dir)[i].iName));
			if (res != KErrNone)
				{
				// log that
				DEBUG_PRINTF2(_L8("Sis Registry Server - DiscoverController failed with error %d."), res);
				}
			}
		}

	CleanupStack::PopAndDestroy(2, &regPath); // dir
	}

TUid CSisRegistryCache::IdentifyUidFromSisFileL(TDesC& aFilename)
	{
	// Returns the package UID read from the symbian header in the specified
	// file.  Note that there is no check to ensure that it is a valid sisx
	// file, but it will leave if the file is too short.
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFilename, EFileRead | EFileStream));
	CleanupClosePushL(file);
	TUid packageUid;
	TUid uid1;
	TUid uid2;
	ReadSymbianHeaderL(file, uid1, uid2, packageUid);
	CleanupStack::PopAndDestroy(&file);
	return packageUid;
	}

void CSisRegistryCache::DiscoverStubsL(TInt aDrive)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Attempting to discover stub sis files on drive %d"), aDrive);
	
	// Find stub sis files on this drive, read package UIDs from files and
	// tag this drive as present for any pre-installed packages which match
	// the UIDs.
	TDriveUnit drive(aDrive);
	// create path for swidaemon's private directory.
	RBuf stubPath;
	stubPath.CreateL(drive.Name(), KMaxPath);
	CleanupClosePushL(stubPath);
	stubPath.Append(KSwiDaemonPrivateDirectory);
	
	CDir* dir;
	TInt err = iFs.GetDir(stubPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
	if(err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Reading stub directory failed (Error code: %d.)"), err);
		CleanupStack::PopAndDestroy(&stubPath);
		return;
		}
	CleanupStack::PushL(dir);

	TInt count(dir->Count());
	RBuf filename;
	filename.CreateL(KMaxFileName);
	CleanupClosePushL(filename);
	for (TInt i = 0; i < count; ++i)
		{
		filename = stubPath;
		filename.Append((*dir)[i].iName);
		TUid packageUid;
		// Trap so that invalid sisx files don't cause a problem.
		TRAP(err, packageUid = IdentifyUidFromSisFileL(filename));
		
		if (err != KErrNone)
			{
			continue;
			}

		DEBUG_PRINTF2(_L("Sis Registry Server - Identified stub '%S'."), &filename);
		TInt tokenCount = iTokens.Count();
		for (TInt i = 0; i < tokenCount; i++)
			{
			if (iTokens[i]->Uid() == packageUid)
				{
				// Create the object using the token we're looking at.
				// We only want to add this drive for preinstalled
				// apps/patches, since non-preinstalled ones should
				// be detected by controller discovery, and this
				// limits false positives.
				CSisRegistryObject* object = 0;
				TRAP(err, object = ObjectL(iTokens[i]->Uid(),
										   iTokens[i]->Name(),
										   iTokens[i]->Vendor()));
				if(err != KErrNone)
					{
					continue;
					}
				if (object->PreInstalled())
					{
					iTokens[i]->AddRemovableDrive(aDrive);
					}
				delete object;
				}
			}
		}
	CleanupStack::PopAndDestroy(3, &stubPath); // dir, filename
	}

void CSisRegistryCache::ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3)
	{
	// Read the first 3 32-bit values from the file as UIDs 1 to 3, leave with
	// KErrUnderflow if the file is too short.
	TInt uidLen =  sizeof(TInt32);

	TPckg<TInt32> uid1(aUid1.iUid);
	User::LeaveIfError(aFile.Read(uid1, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}

	TPckg<TInt32> uid2(aUid2.iUid);
	User::LeaveIfError(aFile.Read(uid2, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}

	TPckg<TInt32> uid3(aUid3.iUid);
	User::LeaveIfError(aFile.Read(uid3, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}

	}
	
void CSisRegistryCache::DiscoverControllersL(const TDesC& aRegistryPath, const TDesC& aDirectoryName)
	{
	DEBUG_PRINTF2(_L("Sis Registry Server - Attempting to discover controllers we can clean up (Directory '%S')"),
		&aDirectoryName);
	
	// we expect uid directories to be 8 characters long, ignore others
	if (aDirectoryName.Length() == KUidHexLength)
		{
		// Determine the uid from the directory name
		TLex lex(aDirectoryName);
		TUint uidValue;
		User::LeaveIfError(lex.Val(uidValue, EHex));	
		
		TUid uid = TUid::Uid(uidValue);
		RBuf controllerPath;
		controllerPath.CreateL(aRegistryPath, KMaxFileName);
		CleanupClosePushL(controllerPath);
		controllerPath.Append(aDirectoryName);
		controllerPath.Append(KPathDelimiter);
		controllerPath.Append(KControllerFileFilter);
		
		CDir* dir;
		// List the directory contents, excluding directories, system and hidden files.
		// i.e. just normal files
		TInt err = iFs.GetDir(controllerPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
		DEBUG_PRINTF2(_L8("Sis Registry Server - Flagging controller directory, read returned code %d."), err);	

		if (err == KErrNone)
			{
			CleanupStack::PushL(dir);
			
			TParsePtrC parse(controllerPath);
			RBuf filename;
			filename.CreateL(KMaxFileName);
			CleanupClosePushL(filename);
			
			TInt count(dir->Count());
			for (TInt i = 0; i < count; ++i)
				{
				const TEntry& entry = (*dir)[i];

				// Determine the augmentation index
				TLex parser(entry.iName);
				TUint augmentationIndex = 0;
				if (parser.Val(augmentationIndex, EHex) != KErrNone)
					{
					continue;
					}

				filename = parse.DriveAndPath();
				filename.Append(entry.iName);
				
				// controller present, see if is still installed on this device
				if(IdentifyControllerL(uid, augmentationIndex, filename))
					{
					continue;	// Controller identified, move on to next one
					}
				// execute the uninstall log (if present)
				// Note: will only do nothing unless we have a corresponding
				// uninstall log on the device indicating it was originally
				// installed on this device.
				HBufC* name = SisRegistryUtil::BuildUninstallLogPathLC(uid, augmentationIndex, aRegistryPath[0]);
				TParsePtrC pathPtr(*name);
				DEBUG_PRINTF2(_L("Sis Registry Server - App no longer on device, executing uninstall log '%S'."), name);

				// use a transactionid of 0 since actual value unknown
				CIntegrityServices* uninstallLog = CIntegrityServices::NewLC(0, pathPtr.Path());
				uninstallLog->RollBackL(ETrue); // roll back all transactions
				CleanupStack::PopAndDestroy(2, name); // uninstallLog
				}
			CleanupStack::PopAndDestroy(2, dir);	// filename
			}
		CleanupStack::PopAndDestroy(&controllerPath);
		}
	}

TBool CSisRegistryCache::IdentifyControllerL(TUid aUid, TInt aAugmentationIndex, const TDesC& aFileName)
	{
	TBool found = EFalse;
	
	for (TInt i = 0; i < iTokens.Count() && !found; i++)
		{
		if (iTokens[i]->Uid() == aUid &&
			iTokens[i]->Index() == aAugmentationIndex)
			{
			RFile file;
			User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
			CleanupClosePushL(file);
			TInt fileSize;
			User::LeaveIfError(file.Size(fileSize));
			HBufC8* buffer = HBufC8::NewLC(fileSize);
			TPtr8 ptr(buffer->Des());
			User::LeaveIfError(file.Read(0, ptr, fileSize));
		
			// Read the controller
			CDesDataProvider* desProvider = CDesDataProvider::NewLC(*buffer);
			Sis::CController* controller = Sis::CController::NewLC(*desProvider, Sis::EAssumeType);
		 	CMessageDigest* msgDigest = controller->GenerateControllerHashLC(*buffer);
			TPtrC8 hash = msgDigest->Final();

			for (TInt j = 0; j < iTokens[i]->ControllerInfo().Count(); j++)
				{
				if (iTokens[i]->ControllerInfo()[j]->HashContainer().Data() == hash)
					{
					found = ETrue;
					// if fine just update the relevant token 
					TDriveUnit drive(aFileName);
					iTokens[i]->AddRemovableDrive(drive);
					}
				}
			CleanupStack::PopAndDestroy(5, &file); // file, buffer, desProvider, controller, msgDigest
			}
		}
	return found;
	}

void CSisRegistryCache::AddDriveAndRefreshL(TInt aDrive)
	{
	// Find flagging controllers for non-preinstalled packages on
	// this drive.
	DiscoverUidsL(aDrive);
	// Find stub sis files on this drive - needed for preinstalled
	// packages which won't have flagging controllers.
	DiscoverStubsL(aDrive);
	}

void CSisRegistryCache::RemoveDriveAndRefresh(TInt aDrive)
	{
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		iTokens[i]->RemoveRemovableDrive(aDrive);
		}
	}

void CSisRegistryCache::UpdatePackagePresentStateL()
	{
	InitDrivesL();
	}

void CSisRegistryCache::RemoveRegistryEntryL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService)
	{
	RemoveEntryL(aObject, aIntegrityService);
	RemoveCleanupInfrastructureL(aObject, aIntegrityService);	
	}
	
void CSisRegistryCache::RemoveRegistryFilesL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService)
	{
	RemoveEntryL(aObject, aIntegrityService);
	}
	
void CSisRegistryCache::AddRegistryEntryL(CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService, const TDesC8& aController)
	{
	AddEntryL(aObject, aIntegrityService);

	// If swipolicy.ini sets false to DeletePreinstalledFilesOnUninstall  
	// we don't need to create an uninstall log because we don't want cleanup activity to happen.
	// Just add the controller for preinstalled app in system drive.
	if(aObject.PreInstalled() && !aObject.DeletablePreInstalled())
		{
		// Create a controller on system drive for preinstalled application
		AddControllerL(aObject, aIntegrityService, aController, iSystemDrive);
		}
	else
		{
		// Add cleanup infrastructure if required
		AddCleanupInfrastructureL(aObject, aIntegrityService, aController);
		}
	}	

void CSisRegistryCache::RegenerateCacheL()
	{
	CleanUp();

	InitStartUpL();
	}

TUint CSisRegistryCache::UpdateTrustStatusL(const TUid& uid, const TSisTrustStatus& status )
	{
	CSisRegistryObject* obj = 0;
	
	// find all file entries that hold the desired file and close them
	CloseAllHandlesForUid(uid);
	
	TUint id;
	
	// load the object, doing this separately protects open sessions from intermediate changes.
	OpenReadHandleL(PackageL(uid),id);
	obj = &EntryObjectL(id);
	HandleEntryL(id).ReadStream().Close();
	
	// update the object	 
	obj->SetTrustStatus(status);
	
	// and write it to file using the rollback facilities. 
	HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(uid, obj->Index());

	TTime currentTime;
	currentTime.UniversalTime();

	CIntegrityServices* integrityServices = 
		CIntegrityServices::NewLC(currentTime.Int64(), 
								  KIntegrityServicesPath);
	
	integrityServices->RemoveL(*name);
	integrityServices->AddL(*name);

	TRAPD(err, UpdateRegistryFileL(*name, *obj));
	
	
	if (err == KErrNone)
		{
		// commit file changes...
		integrityServices->CommitL();
		}
	else
		{
		// rollback file changes and return error; 
		integrityServices->RollBackL(EFalse);
		}
		
	// ...and reload any open subsessions
	ReloadAllHandlesForUidL(uid, name);
		
	// can close the temporary file handle now the object is no longer required	   
	CloseReadHandleL(id);
	
	CleanupStack::PopAndDestroy(2, name); // integrityServices
	
	User::LeaveIfError(err);
	
	return KErrNone;
	}
	
	
void CSisRegistryCache::UpdateRegistryFileL(const TFileName& filename,
											const CSisRegistryObject& obj)
	{
	RFileWriteStream writeStream;
	CleanupClosePushL(writeStream);
	User::LeaveIfError(writeStream.Create(iFs, filename, EFileWrite | EFileShareExclusive));

	obj.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();
	
	CleanupStack::Pop(&writeStream);

	}

TBool CSisRegistryCache::IsSidPresent(const TUid aSid) const
/**
	Used by RSisRegistrySession::IsSidPresentL to determine
	if the supplied SID is present in any registered package.
	
	@param	aSid			Executable SID to search for.
	@return					Whether the executable with the supplied
							SID is present in any registered package.
 */
	{
	for (TInt i = iTokens.Count() - 1; i >= 0; --i)
		{
		if (iTokens[i]->SidPresent(aSid))
			return ETrue;
		}
	
	return EFalse;
	}

TUint CSisRegistryCache::SetRemoveWithLastDependentL(const TUid& uid)
	{
	CSisRegistryObject* obj = 0;
	
	// find all file entries that hold the desired file and close them
	CloseAllHandlesForUid(uid);	
	
	TUint id;
	
	// load the object, doing this separately protects open sessions from intermediate changes.
	OpenReadHandleL(PackageL(uid),id);
	obj = &EntryObjectL(id);
	HandleEntryL(id).ReadStream().Close();
	
	// update the object	 
	obj->SetRemoveWithLastDependent();
	
	// and write it to file using the rollback facilities. 
	HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(uid, obj->Index());

	TTime currentTime;
	currentTime.UniversalTime();

	CIntegrityServices* integrityServices = 
		CIntegrityServices::NewLC(currentTime.Int64(), 
								  KIntegrityServicesPath);
	
	integrityServices->RemoveL(*name);
	integrityServices->AddL(*name);

	TRAPD(err, UpdateRegistryFileL(*name, *obj));
	
	
	if (err == KErrNone)
		{
		// commit file changes...
		integrityServices->CommitL();
		}
	else
		{
		// rollback file changes and return error; 
		integrityServices->RollBackL(EFalse);
		}
		

	// ...and reload any open subsessions
	ReloadAllHandlesForUidL(uid, name);
		
	// can close the temporary file handle now the object is no longer required	   
	CloseReadHandleL(id);
	
	CleanupStack::PopAndDestroy(2, name); // integrityServices
	
	User::LeaveIfError(err);
	
	return KErrNone;
	}

void CSisRegistryCache::UpdateLocalizedNamesL()
	{
	TInt count = iTokens.Count();
	for (TInt i = 0; i < count; i++) 
		{
		HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(iTokens[i]->Uid(), iTokens[i]->Index());
			
		RFileReadStream stream;
		User::LeaveIfError(stream.Open(iFs, *name, EFileRead | EFileShareReadersOnly));
		CleanupStack::PopAndDestroy(name);
		CleanupClosePushL(stream);
			
		// open entry
		CSisRegistryObject *sisRegObject = CSisRegistryObject::NewLC(stream);
		// update the localized pkg name to CSisRegistryPackage object
		iTokens[i]->SetNameL(sisRegObject->Name());
						
		// delete entry & stream 
		CleanupStack::PopAndDestroy(2, &stream);// &stream, sisRegObject
		}	
	}
//
// Class CSisLangChangeMonitor
//

CSisRegistryCache::CSisLangChangeMonitor::~CSisLangChangeMonitor()
	{	
	Cancel();
	iLangNotifier.Close();
	}

CSisRegistryCache::CSisLangChangeMonitor* CSisRegistryCache::CSisLangChangeMonitor::NewL(CSisRegistryCache& aSisRegCache)
	{ // static	
	CSisLangChangeMonitor* self=new(ELeave) CSisLangChangeMonitor(aSisRegCache);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSisRegistryCache::CSisLangChangeMonitor::CSisLangChangeMonitor(CSisRegistryCache& aSisRegCache)
	: CActive(EPriorityLess),
	iSisRegistryCache(aSisRegCache)
	{	
	iPrevLanguage = User::Language();
	CActiveScheduler::Add(this);
	}

void CSisRegistryCache::CSisLangChangeMonitor::ConstructL()
	{
	User::LeaveIfError(iLangNotifier.Create());
	Start();
	}
 
void CSisRegistryCache::CSisLangChangeMonitor::Start()
	{
	iLangNotifier.Logon(iStatus);
	SetActive();
 	}
 
void CSisRegistryCache::CSisLangChangeMonitor::DoCancel()
	{
 	iLangNotifier.LogonCancel();
	}
 
void CSisRegistryCache::CSisLangChangeMonitor::RunL()
	{
	// Logon to get further events before handling current request.
	TRequestStatus status = iStatus;
	Start();
	
	// if it is a language change event, start a rescan on app-list.
 	if (status.Int() == EChangesLocale && iPrevLanguage != User::Language())
		{		
		iPrevLanguage = User::Language();
		// Invoke API on SISRegistry to updated the information
		// based on the current locale
		iSisRegistryCache.UpdateLocalizedNamesL();
		}
	}

TInt CSisRegistryCache::CSisLangChangeMonitor::RunError(TInt /*aError*/)
	{
	iPrevLanguage = ELangNone;
	return KErrNone;
	}

void CSisRegistryCache::RemovablePackageListL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	CSisRegistryObject* obj = 0;
	TUint id = 0;
	
	aPackages.ResetAndDestroy();
	for (TInt i = 0; i < iTokens.Count(); i++)
		{
		// load the object.
		OpenReadHandleL(PackageL(iTokens[i]->Uid() ,iTokens[i]->Index()), id); 
		obj = &EntryObjectL(id);
		HandleEntryL(id).ReadStream().Close();
		if (obj->IsRemovable())
			{
			CSisRegistryPackage *package = CSisRegistryPackage::NewLC(*iTokens[i]);
			aPackages.AppendL(package);
			CleanupStack::Pop(package);			
			}

		CloseReadHandleL(id);
		}	
	}
	
void CSisRegistryCache::RecoverL()
	{
	// Check to see if SWI is in use (install/uninstall/restore)
	TInt swisState = 0;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,KUidSoftwareInstallKey,swisState));
	
	// Check to see if a backup is in progress...
	TInt backup = 0;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,conn::KUidBackupRestoreKey,backup));
	
	if (swisState == ESwisNone && (backup == conn::EBURUnset || backup & (conn::EBURNormal | conn::ENoBackup)))
		{
		// Attempt to recover all previously failed installations
		TRAP_IGNORE(iIntegrityService->RollBackL(ETrue));
	
		if (iIntegrityService->StartedJournalRollback())
			{
			// At least one journal file started being rolled back. In case any
			// registry entries were removed and then re-added, the cache must be regenerated.
			RegenerateCacheL();
			}
		//Start SWI Observer so that it can process the log files left from the previous session.
		RSwiObserverSession swiObserver;
		swiObserver.ProcessLogsL(RFsHandle());
		swiObserver.Close();
		}
	}

void CSisRegistryCache::CloseAllHandlesForUid(const TUid& aUId)
	{
	for (TUint i = 0 ; i < iFiles.Count() ; ++i)
		{
			if (iFiles[i]->RegistryObject().Uid() == aUId)
				{
					iFiles[i]->ReadStream().Close();
				}
		}	
	}

void CSisRegistryCache::ReloadAllHandlesForUidL(const TUid& aUId, const HBufC *aRegistryFilename)
	{
	for (TUint i = 0 ; i < iFiles.Count() ; ++i)
		{
		if (iFiles[i]->RegistryObject().Uid() == aUId)
			{
			iFiles[i]->ReloadL(*aRegistryFilename, *this);
			}
		}			
	}

	
void CSisRegistryCache::AddLogEntryL(const CSisRegistryObject& aObject, TSwiLogTypes aInstallInfo)
 	{
 	CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
	TInt numOfEntries = securityPolicy->MaxNumOfLogEntries();
	
	if(numOfEntries)
		{		
	 	CLogEntry* logObject = CLogEntry::NewL(aObject,aInstallInfo);
	    CleanupStack::PushL(logObject);
	  	
	  	RFile file;
	  	RFileWriteStream stream;
	  	CleanupClosePushL(file);
	  	CleanupClosePushL(stream);
	  	
	  	HBufC* logName = SisRegistryUtil::BuildLogFileNameLC();
		
	   	TInt err = file.Open(iFs,*logName,EFileRead|EFileWrite| EFileShareExclusive| EFileStream);
	 	// If it's the first entry in file then append FileMajorVersion FileMinorVersion 
	 	// followed by package entries else log the package entries only
	  	if(err == KErrNotFound)
	 		{
	 		User::LeaveIfError(file.Create(iFs,*logName, EFileWrite | EFileShareExclusive| EFileStream));
	 		stream.Attach(file);
	 		CLogFileVersion lofFileVersionObject;
	 		lofFileVersionObject.ExternalizeL(stream);
	 		}
	 	else if( err == KErrNone )
			{
		 	
			RFileWriteStream writeStream;
			CleanupClosePushL(writeStream);
					 	
			TInt fileCount = 0;
			
			RFileReadStream readStream(file);		 
			CleanupClosePushL(readStream);
			CLogEntry* log = NULL;
			TInt err;
			file.Close();
					 	
			CLogFileVersion* logVer = CLogFileVersion::NewL(readStream);
			CleanupStack::PushL(logVer);
	  	
			do
				{
				TRAP(err,log = CLogEntry::NewL(readStream));
				if(err == KErrEof)
					{
					break;
					}
				else if(err != KErrNone)
					{
					User::Leave(err);	
					}				
				delete log;
				++fileCount;
				}
			while(err == KErrNone);
			
			CleanupStack::PopAndDestroy(logVer);
			readStream.Release();
	
			//Check If the entries in log file is equal to maximum
			//number of entries
			if (fileCount >= numOfEntries)
			 	{
			 	TTime time;
			 	time.HomeTime();
			 	TInt64 transactionID = time.Int64();
				 
				// coverity[leave_without_push]
			  	CIntegrityServices* integrityService = CIntegrityServices::NewLC(transactionID, KIntegrityServicesPath);
					
				//Create a temporary file and read the log file in that
				//oldest entry will be removed in the temporary log file
				RFile tempLogFile;
				
				TDriveUnit sysDrive(iFs.GetSystemDrive());
				TBuf<128> tempLogName = sysDrive.Name();
				tempLogName.Append(KTempLog);
			 		
			 	User::LeaveIfError(tempLogFile.Replace(iFs,tempLogName,EFileWrite| EFileShareExclusive |EFileStream));
						 		
			 	writeStream.Attach(tempLogFile);
				TInt err = file.Open(iFs,*logName,EFileRead| EFileShareExclusive| EFileStream);
				CleanupClosePushL(file);
				readStream.Attach(file);
				CLogFileVersion* logFileVersion = CLogFileVersion::NewL(readStream);
				CleanupStack::PushL(logFileVersion);		 	
						 		
			 	writeStream << *logFileVersion;
			 	err = 0;
			 		
			 	for (TInt i = 0; i < fileCount; ++i)
					{
					TRAP(err, log = CLogEntry::NewL(readStream));
					if(err == KErrEof)
						{
						break;
						}
					else if(err != KErrNone)
						{
						User::Leave(err);	
						}
					if(i)
						{
						CleanupStack::PushL(log);
						writeStream << *log;
						CleanupStack::PopAndDestroy(log); 	
						}
					else
						{
						delete log;
						}
					}
				writeStream.CommitL();
				tempLogFile.Close();
				writeStream.Release();
				readStream.Release();
				CleanupStack::PopAndDestroy(2,&file);//file,logFileVersion
			
				
			 	if( SisRegistryUtil::FileExistsL(iFs,*logName))
			 		{
			 		integrityService->RemoveL(*logName);
					}
				integrityService->AddL(*logName);
				
				TInt renameErr =iFs.Rename(tempLogName,*logName);			
					
				if (renameErr == KErrNone)
					{
					// commit file changes...
					integrityService->CommitL();
					}
				else
					{
					// rollback file changes and return error;
					iFs.Delete(tempLogName); 
					integrityService->RollBackL(EFalse);
					}
				CleanupStack::PopAndDestroy(integrityService); //integrityService
										
				logVer = NULL;
				log = NULL;
			 	}
			CleanupStack::PopAndDestroy(2,&writeStream); //writeStream,readStream
 			User::LeaveIfError(file.Open(iFs,*logName,EFileRead|EFileWrite| EFileShareExclusive| EFileStream)); 
			TInt pos = 0;
	  		User::LeaveIfError(file.Seek(ESeekEnd, pos));	
	  	
	  		stream.Attach(file,pos);
	 		}
		else 
	 		{
		 	User::Leave(err);	
		 	}
	  	
	  	logObject->ExternalizeL(stream);
	 	stream.CommitL();
	 	
	 	CleanupStack::PopAndDestroy(4,logObject);//logObject,file,stream,logName
		}
	}
	

_LIT( KSWVersionFileName, "Z:\\resource\\versions\\sw.txt" );
const TInt KSysUtilVersionTextLength = 64;
_LIT( KNewLinePattern, "\\n" );
_LIT( KNewline, "\n" );
/**
Fetch text from specified files.
Copied from /common/generic/syslibs/bafl/sysutil/src/sysutil.cpp 
*/
static TInt GetTextFromFile(
    const TDesC& aFilename,
    TDes& aValue,
    TBool aRemoveNewLines )
    {
    RFs fs;
    TInt err;
    err = fs.Connect();
    if (err != KErrNone)
        return err;

    RFile file;
    err = file.Open( fs, aFilename,
                     EFileRead | EFileStreamText | EFileShareReadersOnly );
    if (err != KErrNone)
        {
        fs.Close();
        return err;
        }

    TBuf8<2> characters;    
    err =  file.Read(characters);
    
    if (err == KErrNone || err == KErrTooBig)
        {
        // This means that we have an ANSI file (without the header bytes)
        if( characters.Length() == 0 || characters.Length() == 1 )
            {
            file.Close();
            fs.Close();
            return KErrCorrupt;     
            }
        else 
            {
            TUint8 firstByte = characters[0];
            TUint8 secondByte = characters[1];
            
            // Heading byte values for unicode files
            const TInt KFFByte = 255;
            const TInt KFEByte = 254;
            
            // If file isn't unicode KErrCorrupt is returned
            if( (firstByte!=KFFByte && secondByte!=KFEByte) && (secondByte!=KFFByte && firstByte!=KFEByte) )
                {
                file.Close();
                fs.Close();
                return KErrCorrupt;
                } 
            } 
        }
    
    TFileText tf;
    tf.Set(file);
    err = tf.Read(aValue);
    // If the maximum length of the descriptor is insufficient to hold the record,
    // the Read() function returns KErrTooBig and the descriptor is filled to its maximum length.
    //
    // If Read() is called when the current position is the end of the file (that is, after
    // the last line delimiter in the file), KErrEof is returned, and the length of the buffer
    // is set to zero. In this case, this would mean an empty file, as this code always reads
    // from the beginning of the file.
    
    if (err == KErrNone || err == KErrTooBig)
        {
        if (aValue.Length() > KSysUtilVersionTextLength)
            {
            // File content is larger than 64 characters. Truncate to 64 characters.
            aValue.Delete(KSysUtilVersionTextLength,aValue.Length() - KSysUtilVersionTextLength);
            err = KErrTooBig;
            }
        
        if (aRemoveNewLines)
            {
            // Replace new-line patterns with real ones
            TInt error = aValue.Find(KNewLinePattern);
            while (error != KErrNotFound)
                {
                // error is a position
                aValue.Replace(error, KNewLinePattern().Length(), KNewline );
                error = aValue.Find(KNewLinePattern);
                }
            }
        }

    file.Close();
    fs.Close();
    
    return err;
    }
// GetSWVersion.  Read sw.txt and return concatenated to single line
// Copied from /common/generic/syslibs/bafl/sysutil/src/sysutil.cpp but without caching functionality
static TInt GetSWVersion(TDes& aValue)
    {
    TInt err = GetTextFromFile( KSWVersionFileName, aValue, ETrue );


    if ( err != KErrNone )
        {
        DEBUG_PRINTF3(_L8("Error: %d, while processing: %S"),err, &KSWVersionFileName);
        }

    return err;
    }

const TInt KInfoBufLength=KSysUtilVersionTextLength;
_LIT(KROMVersionStringCacheDir, ":\\sys\\install\\sisregistry\\");
_LIT(KROMVersionStringCacheFileName, "ROMVersionStringCache_localSysUtil.bin");


TBool CSisRegistryCache::IsFirmwareUpdatedL()
    {
    TChar sysDrive = RFs::GetSystemDriveChar();
    TInt maxSizeofFileName = KROMVersionStringCacheDir().Length() + KROMVersionStringCacheFileName().Length() + 1;
    RBuf romVersionCacheFileName;
    romVersionCacheFileName.CreateL(maxSizeofFileName);
    romVersionCacheFileName.CleanupClosePushL();
    romVersionCacheFileName.Append(sysDrive);
    romVersionCacheFileName.Append(KROMVersionStringCacheDir());
    romVersionCacheFileName.Append(KROMVersionStringCacheFileName());
    
    //Read the length & value from it, if any.
    RFileReadStream romVerStream;
    TInt err = romVerStream.Open(iFs,romVersionCacheFileName,EFileRead);
    if ( err != KErrNone )
        {
        CleanupStack::PopAndDestroy(1);
        return ETrue;
        }
    CleanupClosePushL(romVerStream);
    TBuf<KInfoBufLength> version;
    TUint32 length = romVerStream.ReadUint32L();
    if (length>KInfoBufLength)
        {
        //File must be corrupt, an attempt to read will panic
        //User::Leave(KErrCorrupt);
        CleanupStack::PopAndDestroy(2);
        return ETrue;
        }   
    romVerStream.ReadL(version, length);

    //the persisted version has been successfully read
    //read the actual current version string
    TBuf<KInfoBufLength> actualVersion;

    err = GetSWVersion(actualVersion); //use the local implementation of the SysUtil::GetSWVersion function

    if(err == KErrNone)
        {
        if (version.Compare(actualVersion) != 0)
            {
            //Leave if the current version is different from the previous stored version and recreate applist.
            DEBUG_PRINTF(_L8("!!Firmware update detected!! Rebuilding AppList"));
            CleanupStack::PopAndDestroy(2);
            return ETrue;
            }
        }
    else
        {
        //Leave if any error reading the version information, except if file is not present
        if (err != KErrPathNotFound && err != KErrNotFound)
            {
            DEBUG_PRINTF2(_L8("!!Error %d reading Firmware version.  Rebuilding AppList"),err);
            CleanupStack::PopAndDestroy(2);
            return ETrue;
            }
        }
    CleanupStack::PopAndDestroy(2); //romVerStream, romVersionCacheFileName
    return EFalse;
    }

void CSisRegistryCache::UpdateRecentFWVersionL()
    {
        //Write a cache of the ROM version to a separate stream
        //Build the filename for the cache file
        TChar sysDrive = RFs::GetSystemDriveChar();
        TInt maxSizeofFileName = KROMVersionStringCacheDir().Length() + KROMVersionStringCacheFileName().Length() + 1;
        RBuf romVersionCacheFileName;
        romVersionCacheFileName.CreateL(maxSizeofFileName);
        romVersionCacheFileName.CleanupClosePushL();
        romVersionCacheFileName.Append(sysDrive);
        romVersionCacheFileName.Append(KROMVersionStringCacheDir());
        romVersionCacheFileName.Append(KROMVersionStringCacheFileName());
        
        //Read the length & value from it, if any.
        RFileWriteStream romVerStream;
        User::LeaveIfError(romVerStream.Replace(iFs,romVersionCacheFileName,EFileWrite));
        CleanupClosePushL(romVerStream);
        TBuf<KInfoBufLength> version;
        GetSWVersion(version);

        // Write even if SysUtil returns err since all conditions are taken care during restore.
        romVerStream.WriteUint32L(version.Length());
        romVerStream.WriteL(version);
        CleanupStack::PopAndDestroy(2); //romVerStream, romVersionCacheFileName
    }

