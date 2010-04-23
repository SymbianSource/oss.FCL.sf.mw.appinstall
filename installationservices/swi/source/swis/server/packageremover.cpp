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



#include <s32file.h>
#include "pkgremovererrors.h"
#include "siscontroller.h"
#include "sisinstallblock.h"
#include "sisinfo.h"
#include "sisstring.h"
#include "packageremover.h"
#include "sisuid.h"
#include "sisversion.h"
#include "sisfieldtypes.h"
#include "cleanuputils.h"
#include "securitycheckutil.h"
#include "secutils.h"
#include "log.h"
#include "sissupportedlanguages.h"
#include <hash.h>

using namespace Swi;
using namespace Swi::Sis;


// SIS registry main path
_LIT(KRegistryPath, "\\sys\\install\\sisregistry\\");

// SWI daemon private directory
_LIT(KSwiDaemonPrivateDirectory, "\\private\\10202dce\\");

// Controller and SIS file extensions
_LIT(KControllerFileExt, ".ctl");
_LIT(KSisFileExt, ".sis");
_LIT(KSisXFileExt, ".sisx");

// Maximum embedding depth allowed in SIS files
const TInt KMaximumEmbeddingDepth = 8;

// Length of UIDs in hex
const TInt KUidHexLength = 8;

//
// Implementation of class CUninstalledPkgEntry
//

CUninstalledPkgEntry* CUninstalledPkgEntry::NewLC(const TUid& aUid, const TDesC& aPackageName,
	const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType,
	const TDesC& aSisCtlFileName, const TDesC& aAssociatedStubSisName, const TDesC8& aControllerHash)
 	{
 	CUninstalledPkgEntry* self = new(ELeave) CUninstalledPkgEntry();
 	CleanupStack::PushL(self);
 	self->ConstructL(aUid, aPackageName, aVendorName, aVersion, aPackageType,
 					aSisCtlFileName, aAssociatedStubSisName, aControllerHash);
 	return self;
 	}

CUninstalledPkgEntry::CUninstalledPkgEntry()
	{
	}

CUninstalledPkgEntry::~CUninstalledPkgEntry()
	{
	delete iVendorName;
	delete iPackageName;
	delete iSisCtlFileName;
	delete iAssociatedStubSisName;
	delete iHashController;
	}

// Constructs the objects using the property values
void CUninstalledPkgEntry::ConstructL(const TUid& aUid, const TDesC& aPackageName,
	const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType,
	const TDesC& aSisCtlFileName, const TDesC& aAssociatedStubSisName, const TDesC8& aControllerHash)
	{
	iUid = aUid;
	iPackageName = aPackageName.AllocL();
	iVendorName = aVendorName.AllocL();
	iVersion = aVersion;
	iPackageType = aPackageType;
	iSisCtlFileName = aSisCtlFileName.AllocL();
	iAssociatedStubSisName = aAssociatedStubSisName.AllocL();	
	iHashController = aControllerHash.AllocL();
	}

void CUninstalledPkgEntry::SetAssociatedStubSisNameL(const TDesC& aAssociatedStubSisName)
	{
	delete iAssociatedStubSisName;
	iAssociatedStubSisName = NULL;
	iAssociatedStubSisName = aAssociatedStubSisName.AllocL();
	}

//
// Implementation of class CPackageRemover
//

CPackageRemover* CPackageRemover::NewLC(const RMessage2& aMessage)
	{
	CPackageRemover* self=new(ELeave) CPackageRemover(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPackageRemover* CPackageRemover::NewL(const RMessage2& aMessage)
	{
	CPackageRemover* self=NewLC(aMessage);
	CleanupStack::Pop(self);
	return self;
	}

// Constructor
CPackageRemover::CPackageRemover(const RMessage2& aMessage) :
	iMessage(aMessage)
	{
	}

void CPackageRemover::ConstructL()
	{
	DEBUG_PRINTF(_L8("Constructing CPackageRemover"));

	// connect to SIS helper 
	User::LeaveIfError(iSisHelper.Connect());
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	// connect to SIS registry
	User::LeaveIfError(iSisRegistry.Connect());
	}

// Delete objects, close connection to the servers
CPackageRemover::~CPackageRemover()
	{	
	iFs.Close();
	iSisHelper.Close(); 
	iSisRegistry.Close();
	iUninstalledPkgEntry.ResetAndDestroy();

	iFileList.ResetAndDestroy();
	iControllerList.ResetAndDestroy();
	delete iMainController;
	delete iControllerData;
	}

// Logic for finding uninstalled packages:
// FIRST : Controller file path processed and added the entries into the array
// SECOND: Stub sis path processed and verified with already added enties, if
// not-added, then add this as new entry. 
// There are following possibilities:
// 1. Autopropagation enabled then both sis file and ctl file will exist - (ctl added and sis won't added)
// In this case ctl files added into array and also the sis file path is 
// referened from the same array entry.
// 2. Autopropagation disabled then only ctl file exist - ctl added.
// 3. For PA and PP type, only stub sis file exist - sis file added
void CPackageRemover::ListL()
	{
	TInt driveNumber = iMessage.Int0();
	TDriveInfo info;
	
	User::LeaveIfError(iFs.Drive(info, driveNumber));
	if (!(info.iDriveAtt & KDriveAttRemovable) || (info.iDriveAtt & KDriveAttSubsted))
		{
		User::Leave(KErrNotRemovable);
		}

	TPath stubSisPath;
	TPath ControllerFilePath;
	TChar drive;
	CDir* dir = NULL;
	User::LeaveIfError(RFs::DriveToChar(driveNumber, drive));
	
	// Construct the Controller file path
	ControllerFilePath.Append(drive);
	ControllerFilePath.Append(KDriveDelimiter);
	ControllerFilePath.Append(KRegistryPath);

	// Get the list of controller file
	// List the directory contents including directories
	TInt err = KErrNone;
	err = iFs.GetDir(ControllerFilePath, KEntryAttDir, ESortByName, dir);
	if (err == KErrNone)
		{
		CleanupStack::PushL(dir);

		TInt count(dir->Count());
		for (TInt i = 0; i < count; ++i)
			{
			const TDesC& directoryName = (*dir)[i].iName;
			// we expect uid directories to be 8 characters long, ignore others
			if (directoryName.Length() != KUidHexLength)
				{
				continue;
				}
			else if (!((*dir)[i].IsDir()))
				{
				continue;
				}
			else
				{
				TPath ctlPath;
				CDir* ctlFileList = NULL;
				ctlPath.Append(ControllerFilePath);
				ctlPath.Append((*dir)[i].iName);
				ctlPath.Append(KPathDelimiter);
				// List the directory contents, excluding directories, system and hidden files.
				// i.e. just normal files
				err = iFs.GetDir(ctlPath, KEntryAttMatchExclude | KEntryAttDir, ESortByName, ctlFileList);
				if (err == KErrNone)
					{
					CleanupStack::PushL(ctlFileList);
					// Add the un-installed controller file into an array
					AddIfUninstalledL(*ctlFileList, ctlPath);
					CleanupStack::PopAndDestroy(ctlFileList);
					}
				}
			}
		CleanupStack::PopAndDestroy(dir); 
		}
	if (err == KErrNone || err == KErrNotFound || err == KErrPathNotFound)
		{
		// Construct the Stub sis file path
		stubSisPath.Append(drive);
		stubSisPath.Append(KDriveDelimiter);
		stubSisPath.Append(KSwiDaemonPrivateDirectory);
	
		// Get the list of stub sis file 
		CDir* stubFileList = NULL;
		// List the directory contents, excluding directories, system and hidden files.
		// i.e. just normal files
		err = iFs.GetDir(stubSisPath, KEntryAttMatchExclude | KEntryAttDir, ESortByName, stubFileList);
		if (err == KErrNone)
			{
			CleanupStack::PushL(stubFileList);
			// Add the un-installed stub sis file into an array
			AddIfUninstalledL(*stubFileList, stubSisPath);
			CleanupStack::PopAndDestroy(stubFileList);
			}
		}
	if (err == KErrNone || err == KErrNotFound || err == KErrPathNotFound)
		{
		// write the array entries into the buffer
		HBufC8* buffer = WriteListToBufferLC();
		TPtr8 pBuffer = buffer->Des();
	
		if (iMessage.GetDesMaxLengthL(1) < pBuffer.Length())
			{
			TInt bufferSize = pBuffer.Size();
			TPckgC<TInt> bufferSizePackage(bufferSize);
			iMessage.WriteL(1, bufferSizePackage);
			err = KErrOverflow;
			}
		else
			{
			iMessage.WriteL(1, pBuffer);
			err = KErrNone;
			}
		CleanupStack::PopAndDestroy(buffer); //buffer
		}
	iMessage.Complete(err);
	}

void CPackageRemover::AddIfUninstalledL(const CDir& aDirName, const TDesC& aPathName)
	{
	TInt fileCount = aDirName.Count();
	if(fileCount)
		{
		for(TInt i = 0; i < fileCount; i++)
			{
			const TEntry &entry = (aDirName)[i];

			HBufC* fileName = HBufC::NewLC(aPathName.Length() + entry.iName.Length());
			TPtr localFilename = fileName->Des();
			localFilename.Copy(aPathName);
			localFilename.Append(entry.iName);

 			// Sanity check the filename
 			TParsePtrC fileParser(*fileName);
			TBool isControllerFile = (fileParser.Ext().CompareF(KControllerFileExt)) ? EFalse : ETrue;
			TBool isSisFile = EFalse;
			
			if ((fileParser.Ext().CompareF(KSisFileExt)) ||
				(fileParser.Ext().CompareF(KSisXFileExt)))
				{
				isSisFile = ETrue;
				}
			
			if (!isSisFile && !isControllerFile)
				{
				// ctl and sis file only processed other files skipped
				CleanupStack::PopAndDestroy(fileName);
 				continue;
				}
				
			// Get hold of the controller
			HBufC8* bufController = NULL;
			TInt err = KErrNone;
			
			if (isControllerFile)
   				{
				// Get the controller data from the file
				TRAP(err, bufController = GetControllerFromControllerFileLC(*fileName);
							CleanupStack::Pop(bufController));
   				}
   			else 
   				{
   				// Get controller data from sishelper (may need to deflate controller)
				TRAP(err, bufController = iSisHelper.GetControllerFromSisFileLC(*fileName);
							CleanupStack::Pop(bufController));
   				}
			if (err != KErrNone)
				{
				CleanupStack::PopAndDestroy(fileName);
 				continue;
				}
	
			CleanupStack::PushL(bufController);
			
			// Ensure the application is not installed
			// Discard the 4 byte type field for consistency with
			// all other registry entries
			TPtrC8 typeController(bufController->Mid(4));
			
			if (iSisRegistry.IsInstalledL(typeController))
				{
				CleanupStack::PopAndDestroy(2, fileName); //fileName, bufcontroller
				continue;
				}

			HBufC8* hashController = ComputeHashL(typeController);
			CleanupStack::PushL(hashController);
			TPtrC8 hashControllerPtr(hashController->Des());
			if (IsAlreadyAddedL(hashControllerPtr, *fileName))
				{
				CleanupStack::PopAndDestroy(3, fileName); //fileName, bufcontroller, hashController
				continue;
				}
				
			// Create controller
			TPtrProvider provider(bufController->Des());
			CController* currentController = NULL;
			TRAP(err, currentController = CController::NewL(provider));
			if (err != KErrNone)
				{
				CleanupStack::PopAndDestroy(3, fileName); // filename , bufController, hashController
 				continue;
				}
				
			CleanupStack::PushL(currentController);

			TUid pkgUid = currentController->Info().Uid().Uid();
			if (isControllerFile)
   				{
   				// Determine the uid from the directory name
	   			TLex lex(aPathName.Right(9).Left(8));
   				TUint uidValue;
   				User::LeaveIfError(lex.Val(uidValue, EHex));
   				TUid uuid = TUid::Uid(uidValue);
				if (uuid != pkgUid)
					{
					// shouldn't add this entry as it may be malware
					// where ctl file is placed under  a uid different from the controller'S UID
					CleanupStack::PopAndDestroy(4, fileName); //fileName, bufcontroller, hashController, currentController
					continue;
					}
   				}

			Sis::TInstallType installType = currentController->Info().InstallType();
			CUninstalledPkgEntry::TPackageType pkgType = CUninstalledPkgEntry::ESaPackage; // to suppress warning
			switch(installType)
				{
				case EInstInstallation:
					pkgType = CUninstalledPkgEntry::ESaPackage;
					break;
				case EInstAugmentation:
					pkgType = CUninstalledPkgEntry::ESpPackage;
					break;
				case EInstPartialUpgrade:
					pkgType = CUninstalledPkgEntry::EPuPackage;
					break;
				case EInstPreInstalledApp:
					pkgType = CUninstalledPkgEntry::EPaPackage;
					break;
				case EInstPreInstalledPatch:
					pkgType = CUninstalledPkgEntry::EPpPackage;
					break;
				default:
					// shouldn't come here
					User::Leave(KErrNotSupported);							
				}
					
			// Localise dependency name
			TLanguage systemLanguage = User::Language();    // Language of the current locale
    		// Check whether the current locale is in the list of languages in the SIS file
    		const CSupportedLanguages& languages = currentController->SupportedLanguages();
			TInt langIndex = 0;
			for (TInt i = 0; i < languages.Count(); i++)
				{
				if (systemLanguage == languages[i])
					{
					langIndex = i;
					break;
					}
				}
    	
			HBufC* pkgName = currentController->Info().Names()[langIndex]->Data().AllocLC();
			HBufC* pkgVendor = currentController->Info().VendorNames()[langIndex]->Data().AllocLC();
			TInt major = currentController->Info().Version().Major();
			TInt minor = currentController->Info().Version().Minor();
			TInt build = currentController->Info().Version().Build();
			TVersion pkgVersion(major,minor,build);
			
			HBufC* associateStubFile = KNullDesC().AllocLC(); // later this will be set
			CUninstalledPkgEntry* PkgEntry = CUninstalledPkgEntry::NewLC(pkgUid, *pkgName,
					*pkgVendor, pkgVersion, pkgType, *fileName, *associateStubFile, *hashController);
			iUninstalledPkgEntry.AppendL(PkgEntry); // ownership is transfered to array.
			CleanupStack::Pop(PkgEntry); 			// PkgEntry
			// associateStubFile, pkgVendor, pkgName ,currentController, bufController, hashController and fileName
			CleanupStack::PopAndDestroy(7, fileName); 	
			}
		}
	}

HBufC8* CPackageRemover::ComputeHashL(TDesC8& aController)
	{
	// calculate the controller hash
	CMessageDigest* digest = CMessageDigestFactory::NewDigestLC(CMessageDigest::ESHA1);
	digest->Update(aController);
	TPtrC8 hash = digest->Final();	
	HBufC8* hashBuffer = hash.AllocL();
	CleanupStack::PopAndDestroy(digest);
	return hashBuffer;
	}

TBool CPackageRemover::IsAlreadyAddedL(TDesC8& aHashController, const TDesC& aFileName)
	{
	// Ensure this entry is not already added into the array using ctl files.
	// check with existing RArray<TUninstalledAppEntry> which contains 
	// a list of uninstalled entries.
	TInt count = iUninstalledPkgEntry.Count();
	TBool same = EFalse;
	for (TInt j = 0; j < count; ++j)
		{
		const TDesC8& existingHash = iUninstalledPkgEntry[j]->HashController();
		if (aHashController.CompareF(existingHash) == 0)
			{
			TBool isSisFile = EFalse;
			if ( (aFileName.Right(4).CompareF(KSisFileExt)) || 
				 (aFileName.Right(4).CompareF(KSisXFileExt)))
				{
				isSisFile = ETrue;
				}
			if (!isSisFile)
				{
				// already a ctl file added in the array
				// it may happen as copy - paste the ctl file with different name
				// e.g copy 00000000_0000.ctl  and paste it as 00000000_1000.ctl or abc.ctl....
				same = ETrue;
				break;
				}
			if (iUninstalledPkgEntry[j]->AssociatedStubSisName() == KNullDesC)
				{
				iUninstalledPkgEntry[j]->SetAssociatedStubSisNameL(aFileName); 
				}
			same = ETrue;
			break;
			}
		}
	return same;
	}

HBufC8* CPackageRemover::WriteListToBufferLC()
	{
	TInt fileCount = iUninstalledPkgEntry.Count();
	TInt length = (fileCount + 1) * sizeof (CUninstalledPkgEntry);
	// Write the object out to a buffer, send to client
	CBufSeg* buf = CBufSeg::NewL(length);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);
	
	// write the files to the stream
	writeStream.WriteInt32L(fileCount);
	for(TInt i = 0; i < fileCount; i++)
		{
		writeStream.WriteInt32L(iUninstalledPkgEntry[i]->Uid().iUid);
		
		const TDesC& pkgName = iUninstalledPkgEntry[i]->PackageName();
		writeStream.WriteInt32L(pkgName.Length());
		writeStream.WriteL(pkgName);
		
		const TDesC& vendorName = iUninstalledPkgEntry[i]->VendorName();
		writeStream.WriteInt32L(vendorName.Length());
		writeStream.WriteL(vendorName);
		
		writeStream.WriteInt32L(iUninstalledPkgEntry[i]->Version().iMajor);
		writeStream.WriteInt32L(iUninstalledPkgEntry[i]->Version().iMinor);
		writeStream.WriteInt32L(iUninstalledPkgEntry[i]->Version().iBuild);
		writeStream.WriteInt32L(iUninstalledPkgEntry[i]->PackageType());	

		const TDesC& sisCtlFileName = iUninstalledPkgEntry[i]->SisCtlFileName();
		writeStream.WriteInt32L(sisCtlFileName.Length());
		writeStream.WriteL(sisCtlFileName);
		
		const TDesC& assocatedStubName = iUninstalledPkgEntry[i]->AssociatedStubSisName();
		writeStream.WriteInt32L(assocatedStubName.Length());
		writeStream.WriteL(assocatedStubName);
		}
	
	HBufC8* buffer = HBufC8::NewLC(buf->Size());
	TPtr8 ptr(buffer->Des());
	buf->Read(0, ptr, buf->Size());
	CleanupStack::Pop(buffer);
	CleanupStack::PopAndDestroy(2, buf); // writeStream , buf
	CleanupStack::PushL(buffer);
	return buffer;
	}

// Method to remove a specified uninstalled package
void CPackageRemover::RemoveL()
	{
	// Get the package file
	HBufC* packageFileName = HBufC::NewLC(iMessage.GetDesLengthL(0));
	TPtr ptr(packageFileName->Des());
	iMessage.ReadL(0, ptr);

	// Get the associated stub file (if any)
	HBufC* stubSisFileName = HBufC::NewLC(iMessage.GetDesLengthL(1));
	TPtr ptrStub(stubSisFileName->Des());
	iMessage.ReadL(1, ptrStub);

	// Sanity check the filename
	TBool isControllerFile = CheckFileL(*packageFileName);

	// Get hold of the controller
	if (isControllerFile)
		{
		// Get the controller data from the file
		iControllerData = GetControllerFromControllerFileLC(*packageFileName);
		CleanupStack::Pop(iControllerData);
		}
	else
		{
		// Get controller data from sishelper (may need to deflate controller)
		iControllerData = iSisHelper.GetControllerFromSisFileLC(*packageFileName);
		CleanupStack::Pop(iControllerData);
		}


	// Process the controller and compile a list of their referenced files
	ProcessControllerL();

	// Delete the files
	ValidateAndDeleteFilesL();

	// Delete the sis / controller file
	// And remove the folders if they are empty
	DeleteFile(*packageFileName);
	DeletePathIfEmpty(*packageFileName);

	// For controller files check if a corresponding stub exists
	if (isControllerFile && (*stubSisFileName != KNullDesC))
		{
		// Sanity check file and ensure its a sis file
		if (CheckFileL(*stubSisFileName) == EFalse)
			{
			DeleteFile(*stubSisFileName);
			DeletePathIfEmpty(*stubSisFileName);
			}
		}

	CleanupStack::PopAndDestroy(2, packageFileName); // stubSisFileName
	iMessage.Complete(KErrNone);
	}

// Process the given controller (including embedded controllers) to get referenced files
void CPackageRemover::ProcessControllerL()
	{
	// Create the controller
	TPtrProvider provider(iControllerData->Des());
	iMainController = Sis::CController::NewL(provider);

	// Process the controller (including the embedded ones)
	// Collect all the file descriptions from the install block
	// No need to close this array!
	Sis::CController* currentController = iMainController;
	const RPointerArray<CFileDescription>& files = currentController->InstallBlock().FileDescriptions();
	TInstallType installType = currentController->Info().InstallType();
	
	for (TInt index = 0; index < files.Count(); ++index)
		{
		iFileList.AppendL(new (ELeave) TFileDescAndInstallType(installType, files[index]));
		}

	TInt nestingLevel = 0;
	iControllerList.AppendL(new (ELeave) TControllerAndLevel(nestingLevel, currentController));
	TInt controllerIndex = -1;
	
	while (++controllerIndex < iControllerList.Count())
		{
		currentController = iControllerList[controllerIndex]->iController;
		nestingLevel = iControllerList[controllerIndex]->iLevel;
		
		if (nestingLevel++ >= KMaximumEmbeddingDepth)
			{
			User::Leave(KErrSISTooDeeplyEmbedded);
			}

		// Get a list of embedded controllers and process them
		const RPointerArray<Sis::CController>& subControllers = currentController->InstallBlock().EmbeddedControllers();
		
		for (TInt index = 0; index < subControllers.Count(); ++index)
			{
			currentController = subControllers[index];
			
			// Check if the embedded controller is installed on the device
			// If installed do not process
			TInt controllerDataOffset = (TInt)currentController->DataOffset();
			TPtrC8 typeController(iControllerData->Mid(controllerDataOffset));
			
			// Pass on the controller plus the data after DataIndex field;
			// otherwise CController::NewLC will crash when addition data not found.
			if (!iSisRegistry.IsInstalledL(typeController))
				{
				const RPointerArray<CFileDescription>& files = currentController->InstallBlock().FileDescriptions();
				TInstallType installType = currentController->Info().InstallType();
				
				for (TInt indexFiles = 0; indexFiles < files.Count(); ++indexFiles)
					{
					iFileList.AppendL(new (ELeave) TFileDescAndInstallType(installType, files[indexFiles]));
					}
				}

			// Append the controller to the list
			TControllerAndLevel* currentControllerAndLevel = new (ELeave) TControllerAndLevel(nestingLevel, currentController);
			CleanupStack::PushL(currentControllerAndLevel);
			iControllerList.AppendL(currentControllerAndLevel);
			CleanupStack::Pop(currentControllerAndLevel);
			}
		}
	}

// Read the controller file and return the controller data
HBufC8* CPackageRemover::GetControllerFromControllerFileLC(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));

	// Allocate size for controller data and controller field type (4 bytes)
	HBufC8* bufController = HBufC8::NewLC(fileSize + 4);
	TPtr8 ptrFileContents(bufController->Des());
	RDesWriteStream write(ptrFileContents);
	CleanupClosePushL(write);

	RFileReadStream read(file);
	CleanupClosePushL(read);

	// Write the controller type
	write.WriteInt32L(EFieldTypeController);

	// Stream in the rest of the controller
	write.WriteL(read);

	CleanupStack::PopAndDestroy(2, &write); // read
	CleanupStack::Pop(bufController);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PushL(bufController);

	return bufController;
	}

// Validate the referenced files and delete them
// Also deletes the path if found empty
void CPackageRemover::ValidateAndDeleteFilesL()
	{
	// Get the filename with full path
	TInt driveNum;
	User::LeaveIfError(RFs::CharToDrive(iRemovableDriveChar, driveNum));

	// Validate and create final list of files to be removed
	RPointerArray<HBufC> filesList;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(filesList);
	TInt fileCount = iFileList.Count();
	for (TInt index = 0; index < fileCount; ++index)
		{
		Sis::CFileDescription* fileDesc = iFileList[index]->iFileDesc;
		const Sis::CString& stringTarget = iFileList[index]->iFileDesc->Target();
		HBufC* bufFileName = stringTarget.Data().AllocLC();
		TPtr ptr(bufFileName->Des());
		TBool skipFile = EFalse;

		// Handle special drive markers
		if (bufFileName->Length() == 0)
			{
			skipFile = ETrue;
			}
		else if (ptr[0] == '!')
			{
			// For PA and PP install types replace with the removable drive
			ptr[0] = iRemovableDriveChar;
			// For others we have no idea what drive the user would've chosen!
			// anyhow we will delete if the file available in removable drive and 
			// not owned by any other package.
			}
		else if (ptr[0] == '$')
			{
			// Replace with system drive
			// Doing this just for the heck of it (since system drive is not removable!)
			ptr[0] = iFs.GetSystemDriveChar();
			}

		// Ensure a valid drive and path are present
		if (!skipFile)
			{
			TParsePtr fileName(ptr);
			if (!fileName.DrivePresent() || !fileName.PathPresent() || !fileName.NamePresent())
				{
				skipFile = ETrue;
				}

			// The file being deleted should belong to the same drive as the controller / stub sis file
			TInt delFileDrive;
			User::LeaveIfError(RFs::CharToDrive(ptr[0], delFileDrive));
			if (delFileDrive != driveNum)
				{
				skipFile = ETrue;
				}

			// Ensure the file is not in a data-caged area
			TFileName directory = fileName.Path();
			if (fileDesc->Operation() == EOpNull)
				{
				// For FN ensure file is not in \sys or \resource
				TPtrC sysPath(KSysPath);
				TPtrC resourcePath(KResourcePath);
				if ((directory.Left(sysPath.Length()).CompareF(sysPath) == 0) ||
					(directory.Left(resourcePath.Length()).CompareF(resourcePath) == 0))
					{
					skipFile = ETrue;
					}
				}

			TEntry entry;
			TInt err = iFs.Entry(*bufFileName, entry);
			if ((err != KErrNone) && 
				(entry.IsTypeValid() && (!SecUtils::IsExe(entry) && !SecUtils::IsDll(entry))))
				{
				// Only EXE and DLL are allowed in \sys\bin
				TPtrC binPath(KBinPath);
				if (directory.Left(binPath.Length()).CompareF(binPath) == 0)
					{
					skipFile = ETrue;
					}
				}
			}

		if (skipFile)
			{
			DEBUG_PRINTF2(_L8("CPackageRemover: Skipping delete %S"), bufFileName);
			CleanupStack::PopAndDestroy(bufFileName);
			continue;	// Skip this file
			}
		else
			{
			// Add the file to the list of files to be removed
			filesList.AppendL(bufFileName);
			CleanupStack::Pop(bufFileName);		// Ownership transferred to array
			}
		}
 
	// Filter out unorphaned files (is not part of another valid installation)
	fileCount = filesList.Count();
	if (fileCount > 0)
		{
		SecurityCheckUtil::FilterOrphanedFilesL(filesList);
		}

	// Delete files and remove empty directories
	fileCount = filesList.Count();
	for (TInt indx = 0; indx < fileCount; ++indx)
		{
		DEBUG_PRINTF2(_L8("CPackageRemover: Deleting "), filesList[indx]);
		DeleteFile(*filesList[indx]);
		DeletePathIfEmpty(*filesList[indx]);
		}

	CleanupStack::PopAndDestroy(&filesList);
	}

TInt CPackageRemover::DeleteFile(const TDesC& aName)
	{
	TInt err = iFs.Delete(aName);
	if (err == KErrAccessDenied)
		{
		// File may be read-only. Clear the attribute and try again
		iFs.SetAtt(aName, 0, KEntryAttReadOnly);
		err = iFs.Delete(aName);
		}

	return err;
	}

// Recursively deletes all folders in the path (as long as they are empty)
void CPackageRemover::DeletePathIfEmpty(const TDesC& aPath)
	{
	TParse path;
	path.Set(aPath, NULL, NULL);

	if (path.PathPresent())
		{
		while ((iFs.RmDir(path.DriveAndPath()) == KErrNone) && (path.PopDir() == KErrNone))
			;
		}
	}

// Sanity checks the filename and path
// Returns ETrue if the file is a controller file
TBool CPackageRemover::CheckFileL(TDesC& aFile)
	{
	// Sanity check the filename
	HBufC* bufFileName = aFile.AllocLC();
	TPtr ptrFileName(bufFileName->Des());
	TParsePtr fileName(ptrFileName);
	if (!fileName.DrivePresent() || !fileName.PathPresent() || !iFs.IsValidName(aFile))
		{
		User::Leave(KErrArgument);
		}

	// Ensure the drive is a removable one
	TInt driveNum;
	iRemovableDriveChar = fileName.Drive()[0];
	User::LeaveIfError(RFs::CharToDrive(iRemovableDriveChar, driveNum));

	TDriveInfo info;
	User::LeaveIfError(iFs.Drive(info, driveNum));
	if (!(info.iDriveAtt & KDriveAttRemovable) || (info.iDriveAtt & KDriveAttSubsted))
		{
		User::Leave(KErrNotRemovable);
		}

	// Check if volume is present
	TVolumeInfo volumeInfo;
	User::LeaveIfError(iFs.Volume(volumeInfo, driveNum));

	// Ensure the file still exists
	RFile file;
	TInt err = file.Open(iFs, aFile, EFileShareAny | EFileRead);
	if (err != KErrNone)
		{
		User::Leave(KErrNotFound);
		}
	file.Close();

	// Get the file type and sanity check its location and contents
	TBool isControllerFile = EFalse;
	isControllerFile = (fileName.Ext().Compare(KControllerFileExt)) ? EFalse : ETrue;
	if (isControllerFile)
		{
		// Should be present in sys registry folder
		User::LeaveIfError(fileName.PopDir());
		if (fileName.Path().Compare(KRegistryPath) != 0)
			{
			User::Leave(KErrNotSupported);
			}
		}
	else
		{
		// Should be present in the SWI Daemon private folder
		if (fileName.Path().Compare(KSwiDaemonPrivateDirectory) != 0)
			{
			User::Leave(KErrNotSupported);
			}
		}

	CleanupStack::PopAndDestroy(bufFileName);
	return isControllerFile;
	}

