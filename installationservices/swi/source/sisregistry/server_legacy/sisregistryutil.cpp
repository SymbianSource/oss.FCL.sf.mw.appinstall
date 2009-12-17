/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistryUtil class - implementation
* A collection of static functions to provide useful functionality to a the sisregistryserver as a whole 	
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32file.h>
#include "log.h"
#include "sisregistryutil.h"
#include "sisregistryserverconst.h"
#include "dessisdataprovider.h"
#include "sisregistryobject.h"
#include "sisinstallblock.h"
#include "sisregistryfiledescription.h"
#include "sisinfo.h"
#include "filesisdataprovider.h"
#include "sisuid.h"

using namespace Swi;

const TInt KStubSisPUidPos = 0x18;

TInt SisRegistryUtil::NextAvailableIndexL(RFs& aFs, TUid aUid)
	{
	TInt index;
	for (index = 1; index < 0xFFFFFFFF; index++)
		{
		// going through the process of trial and error
		// check next available 
		HBufC* path = BuildEntryFileNameLC(aUid, index);
	
		if (!FileExistsL(aFs, *path))
			{
			CleanupStack::PopAndDestroy(path);
			// then we have the number
			break;
			}
			
		CleanupStack::PopAndDestroy(path);
		// otherwise go through the loop	
		}
	return index;
	}

HBufC* SisRegistryUtil::BuildEntryFileNameLC(TUid aUid, TInt aIndex)
	{
	TDriveUnit systemDrive(RFs::GetSystemDrive());
	TDriveName driveName = systemDrive.Name();
	HBufC *filename = HBufC::NewMaxLC(KMaxFileName);
	TPtr filenamePtr(filename->Des());
	filenamePtr.Format(KEntryFileNameFormat, &driveName, &KRegistryPath, aUid.iUid, aIndex);
	return filename;
	}

HBufC* SisRegistryUtil::BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController, TInt aDrive)
	{
	TDriveUnit drive(aDrive);
	TDriveName driveName = drive.Name();
	HBufC *filename = HBufC::NewMaxLC(KMaxFileName);
	TPtr filenamePtr(filename->Des());
	filenamePtr.Format(KControllerFileNameFormat, &driveName, &KRegistryPath, aUid.iUid,
					aIndex, aController);
	return filename;
	}

HBufC* SisRegistryUtil::BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController)
	{
	return BuildControllerFileNameLC(aUid, aIndex, aController, RFs::GetSystemDrive());
	}

HBufC* SisRegistryUtil::BuildUninstallLogPathLC(TUid aUid, TInt aIndex, TChar aDriveLetter)
	{
    TUint driveletter(aDriveLetter);
 	TDriveUnit ramDrive(RFs::GetSystemDrive());
 	TDriveName ramDriveName = ramDrive.Name(); 	
	HBufC *filename = HBufC::NewMaxLC(KMaxFileName);
	TPtr filenamePtr(filename->Des());
	filenamePtr.Format(KUninstallLogPathFormat, &ramDriveName, aUid.iUid, aIndex, driveletter);	
	return filename;
	}
	
TBool SisRegistryUtil::FileExistsL(RFs& aFs, const TDesC& aFullPath)
	{
	TBool found = EFalse;
	TEntry entry;
	TInt err = aFs.Entry(aFullPath, entry);
	if (err == KErrNone)
		{
		return  ETrue;
		}
	else if (err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}
	
	return found;
	}

void SisRegistryUtil::EnsureDirExistsL(RFs& aFs, const TDesC& aPath)
	{
	TInt err = aFs.MkDirAll(aPath);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	}

// Processes the ROM stub files. Retuns ETrue if the aUid or aName
// is found in any of the ROM stub files, else returns EFalse.
TBool SisRegistryUtil::RomBasedPackageL(const TUid& aUid)
	{	
	TBool isInRom = EFalse;
	
	RFs fileServer;
	User::LeaveIfError(fileServer.Connect());
	CleanupClosePushL(fileServer);
	
	CDir* stubList = NULL;
	_LIT(KRomStubPath, "z:\\system\\install\\%S");
	_LIT(KRomStubWildCard,"z:\\system\\install\\*.sis");
	
	TEntry stubEntry;
	RBuf stubFileName;
	stubFileName.CreateL(KMaxFileName);
	CleanupClosePushL(stubFileName);
	//Iterates through the stub files in the ROM calling CheckIfMatchingStubFileL() 
	//to search for the presence of the package name ignoring any SIS parsing errors.
	if (fileServer.GetDir(KRomStubWildCard,KEntryAttNormal, ESortByName, stubList)==KErrNone)
		{		
		CleanupStack::PushL(stubList);		
		for (TInt stubCount = 0; stubCount < stubList->Count(); stubCount++)
			{			
			stubEntry = (*stubList)[stubCount];
			stubFileName.Format(KRomStubPath, &(stubEntry.iName));		
			
			
			TRAPD(err, isInRom = SisRegistryUtil::CheckIfMatchingStubPUidL(aUid,  fileServer, stubFileName));
			
			if ((err >= KErrSISNotEnoughSpaceToInstall)  && (err <= KErrSISFieldIdMissing))
  				{
  				//Error reading SIS stub details, skip this one and continue searching the list.
  				DEBUG_PRINTF3(_L("ERROR: Stub file in ROM failed to parse. Filename: %S. Error code: %d."), &stubEntry.iName, err);
  				}
  			else if (err != KErrNone)
  				{
  				User::Leave(err);
  				}
  			else if (isInRom)
  				{
				break;
  				}
			}
		CleanupStack::PopAndDestroy(stubList);
		}			
	CleanupStack::PopAndDestroy(2, &fileServer); // stubFileName
	return isInRom;
	}

 HBufC* SisRegistryUtil::BuildLogFileNameLC()
 	{
 	HBufC *filename = HBufC::NewMaxLC(KMaxFileName);
 	TPtr filenamePtr(filename->Des());
 	TDriveUnit sysDrive(RFs::GetSystemDrive());
 	filenamePtr = sysDrive.Name();
 	filenamePtr.Append(KLogDir);
 	return filename;
 	}

/**
Reads the pUid of stub sis file from a given position in the file.
and match it with aUid
@param aName The file name being searched for.
@param aUid  The package uid being searched for.
@param aFileServer Handle to the file server to use.
@param aStubFileName Complete path and filename of the stub file being checked.
@return ETrue if the package uid or the pkg name is found, EFalse otherwise.
*/
TBool SisRegistryUtil::CheckIfMatchingStubPUidL(const TUid& aUid, RFs& aFileServer, const TFileName& aStubFileName)
	{
	
	RFile sisFile;
	User::LeaveIfError(sisFile.Open(aFileServer, aStubFileName, EFileRead));
	CleanupClosePushL(sisFile);
	RFileReadStream sisFileReadStream(sisFile, KStubSisPUidPos);
	CleanupClosePushL(sisFileReadStream);
	
	TUid packageUid = TUid::Uid(sisFileReadStream.ReadUint32L());
	
	CleanupStack::PopAndDestroy(2,&sisFile); //readstream, file
	return (aUid == packageUid);
	
	}

