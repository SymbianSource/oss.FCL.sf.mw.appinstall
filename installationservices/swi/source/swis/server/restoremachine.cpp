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
*
*/


#include "restoremachine.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#else
#include "integrityservices.h"
#endif

#include "installclientserver.h"
#include "sisregistryentry.h"
#include "restorecontroller.h"
#include "restoreprocessor.h"
#include "sisregistryfiledescription.h"
#include "installationprocessor.h"
#include "sisuihandler.h"
#include "sishash.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "log.h"
#include "swispubsubdefs.h"
#include "securitycheckutil.h"
#include "secutils.h"
#include "cleanuputils.h"
#include "sisversion.h"
#include "sisregistrywritablesession.h"
#include <f32file.h>
#include "sisregistrypackage.h"

using namespace Swi;


//
// CRestoreMachine
//

CRestoreMachine* CRestoreMachine::NewL(const RMessage2& aMessage)
	{	
	CRestoreMachine* self = CRestoreMachine::NewLC(aMessage);
	CleanupStack::Pop(self);
	return self;	
	}
	
CRestoreMachine* CRestoreMachine::NewLC(const RMessage2& aMessage)
	{	
	CRestoreMachine* self = new (ELeave) CRestoreMachine;
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	return self;	
	}
	
void CRestoreMachine::ConstructL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Restore - Starting new restore session"));
	
	iSystemDriveChar = RFs::GetSystemDriveChar();

	TPckg<TUid> package(iPackageUid);
	
	TInt err = aMessage.Read(EMessageSlotRestoreUid, package, 0);
	
	if (err != KErrNone)
		{
		aMessage.Panic(KInstallServerName, KErrBadDescriptor);
		}
	
	TInt len = aMessage.GetDesLengthL(EMessageSlotRestoreController);
	iBuf = HBufC8::NewL(len);
	TPtr8 bufPtr = iBuf->Des();
	
	err = aMessage.Read(EMessageSlotRestoreController, bufPtr, 0);
	
	if (err != KErrNone)
		{
		aMessage.Panic(KInstallServerName, KErrBadDescriptor);
		}
		
	User::LeaveIfError(iFs.Connect());
  	iSecurityManager = CSecurityManager::NewL();
  	iRestoreController = CRestoreController::NewL(*iBuf, *iSecurityManager, iFs);
	iFileMan = CFileMan::NewL(iFs);
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Create an STS transaction
	iStsSession.CreateTransactionL();
	_LIT(KPathFormat, "%c:%S");	
	TPath path;	
	TUint driveCh(iSystemDriveChar); // can't pass TChar to Format

	// Connect to the registry and start a transaction
	User::LeaveIfError(iRegistrySession.Connect());
	iRegistrySession.CreateTransactionL();
#else
	// Create an integrity services object
	TTime currentTime;
	currentTime.UniversalTime();
	
	_LIT(KPathFormat, "%c:%S");	
	TUint driveCh(iSystemDriveChar); // can't pass TChar to Format
	TPath path;	
	
	path.Format(KPathFormat, driveCh, &KSysInstallPath);
	iIntegrityServices = CIntegrityServices::NewL(currentTime.Int64(), path);
	
	//Roll back partial, abortive installs before us
	iIntegrityServices->RollBackL(ETrue);
#endif

	path.Format(KPathFormat, driveCh, &KSysInstallTempPath);
	err = iFs.MkDirAll(path);
 	if (err != KErrAlreadyExists && err != KErrNone)
 		{
		User::Leave(err);
 		}
	
	// Check if it is a valid upgrade
 	if (iSecurityManager->SecurityPolicy().AllowOverwriteOnRestore())    
 		{
 		// Fix PDEF129066: Pre-installed backed up content cannot be restored 
 		PrepareUpgradesL();
 		}
 	else
 		{
 		CheckVersionUpgradeL();
 		}
		//Publish package Uid which is going to be restored.
 	err = SecUtils::PublishPackageUid(iPackageUid, iUidList);
	if (err == KErrNone)
    	{
        DEBUG_PRINTF2(_L("CRestoreMachine::ConstructL published Uid is %x."),iPackageUid);
    	}
	else if (err == KErrOverflow)
	    {
        DEBUG_PRINTF2(_L("CRestoreMachine::ConstructL Failed to publish Uid %x as the array, holding the uids, exceeded its upper limit."),iPackageUid);
	    }
    else if (err == KErrNotFound)
        {
        DEBUG_PRINTF2(_L("CRestoreMachine::ConstructL Failed to publish Uid %x as the property is not defined."),iPackageUid);
        }
	else
		{
        DEBUG_PRINTF3(_L("CRestoreMachine::ConstructL Failed to publish Uid %x with error %d."),iPackageUid, err);
        User::Leave(err);
		}

 #ifdef SYMBIAN_USER_PROMPT_SERVICE
	//connect to the SWI Observer
	User::LeaveIfError(iObserver.Connect());
	//Resgister to the SWI Observer
	TRequestStatus status;
	iObserver.Register(status);
	User::WaitForRequest(status);
	
	RBuf logFileName;
	logFileName.CreateL(KMaxFileName);
	logFileName.CleanupClosePushL();
	//Get created a log file and obtains its full name.
	iObserver.GetFileHandleL(logFileName);
	//Add the log file to the transaction
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.RegisterNewL(logFileName);
#else
	iIntegrityServices->AddL(logFileName);
#endif
	CleanupStack::PopAndDestroy(&logFileName);
#endif
	aMessage.Complete(KErrNone);
	}
	
void CRestoreMachine::ServiceFileRequestL(const RMessage2& aMessage)
	{
	RFile restoreFile;
	TInt err = restoreFile.AdoptFromClient(aMessage, EMessageSlotRestoreFs, EMessageSlotRestoreFile);
	CleanupClosePushL(restoreFile);
	
	if (err != KErrNone)
		{
		aMessage.Panic(KInstallServerName, KErrBadDescriptor);
		User::Leave(KErrBadDescriptor);
		}
		
	TInt len = aMessage.GetDesLengthL(EMessageSlotRestorePath);
	HBufC* buf = HBufC::NewLC(len);
	TPtr filename = buf->Des();
	
	err = aMessage.Read(EMessageSlotRestorePath, filename, 0);
	
	if (err != KErrNone)
		{
		aMessage.Panic(KInstallServerName, KErrBadDescriptor);
		User::Leave(KErrBadDescriptor);
		}
	
	TFileName sbeFileName;
	User::LeaveIfError(restoreFile.FullName(sbeFileName));
	DEBUG_PRINTF3(_L("Restore - Servicing File Request for file '%S' from '%S'"),
		&filename, &sbeFileName);

	if (filename[0] == '!')			// install to user selected rive
		{
		filename[0] = iRestoreController->InstallDrive();
		}
	else if (filename[0] == '$')	// install to system drive
		{
		filename[0] = iFs.GetSystemDriveChar();
		}
		
	// Check if the disk is in at present
  	TInt drive;
  	User::LeaveIfError(iFs.CharToDrive(filename[0], drive));
  	TVolumeInfo volume;
  	err = iFs.Volume(volume, drive);
  	if (err != KErrNone)
  		{
  		User::Leave(KErrPathNotFound); // error code agreed with the connect team
  		}
	
	if (!SecurityCheckUtil::CheckFileName(filename,iSystemDriveChar)
		|| SecurityCheckUtil::IsSubstedDriveL(iFs, filename))
		{
		User::Leave(KErrBadName);
		}
		
	// For TCB protected directories eclipsing checks are required. This is not necessary
	// for public or private directories because SBE can restore these directly and the eclipsing
	// checks do not really improve security.
	TBool requiresHashCheck = EFalse;
	if (SecurityCheckUtil::IsProtectedDirectoryL(filename) && ! SecurityCheckUtil::IsPrivateDirectory(filename))
		{	
		if (! IsUniqueL(filename, requiresHashCheck))
			{
			User::Leave(KErrInvalidEclipsing);
			}
		}
	// Hash checks must be performed for TCB protected files
	requiresHashCheck |= SecurityCheckUtil::IsTargetTcbWriteProtected(filename);
	
	TFileName tempFileName;
	iRestoreController->AddFileDescriptionL(restoreFile, filename, tempFileName, requiresHashCheck);
	
	// move the file to it's temporary location and add it to the transaction
	// as a temporary file.
	// format of names is {drive}:\sys\install\temp\file-{uid}-{abs index}-{file index}-{hash of name in hex}
	// this format differs slightly from the install format
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.RegisterTemporaryL(tempFileName);
#else
	iIntegrityServices->TemporaryL(tempFileName);
#endif

 	err = iFs.MkDirAll(tempFileName);
 	if (err != KErrAlreadyExists && err != KErrNone)
 		{
		User::Leave(err);
 		}

 	User::LeaveIfError(iFileMan->Copy(restoreFile, tempFileName));	
	CleanupStack::PopAndDestroy(2, &restoreFile);
	aMessage.Complete(KErrNone);
	}

TBool CRestoreMachine::IsUniqueL(const TDesC& aFileName, TBool& aCheckFileHashOut)
	{	
	aCheckFileHashOut = EFalse;
	
	if (aFileName.Length() == 0)
		{
		return ETrue;
		}
	
	TPath path;
	path.Copy(aFileName);
	
	// Find on which disks (if any) this file exists on	
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));

	TChar disk = 'a' - 1;
	
	for (TInt i = 0; i < driveList.Length(); i++)
		{
		disk+=1;
		
		if (driveList[i] == 0)
			{
			continue;
			}
		else if (disk == 'z')
			{
			continue; // eclipsing allowed, checked later
			}
		
		path[0] = disk;
		TEntry entry;
		
		if (iFs.Entry(path, entry) == KErrNone)
			{
			if (path.CompareF(aFileName) == KErrNone)
				{
				// If there is already a file here, check its hash
				aCheckFileHashOut = ETrue;
				}
			else
				{
				// names must be unique across drives
				return EFalse;
				}
			}
		}
		
	return ETrue;	
	}



void CRestoreMachine::CheckVersionUpgradeL()
	{
	const RPointerArray<CPlan>& plans = iRestoreController->Plans();
		
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif
	
    TInt count = plans.Count(); 
    for (TInt i = 0; i < count; ++i)
		{
		// check only the first plan is a valid upgrade
        const Sis::CController& controller = plans[i]->ApplicationL().ControllerL();
		Sis::TInstallType type = controller.Info().InstallType();
		TBool isBase = ((type == EInstInstallation) || (type == EInstPreInstalledApp));
		
		// ask the registry if the UID already exists.
		RSisRegistryEntry entry;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TInt err = entry.Open(iRegistrySession, controller.Info().Uid().Uid());
#else
		TInt err = entry.Open(session, controller.Info().Uid().Uid());
#endif
		if (err == KErrNone)
			{
			CleanupClosePushL(entry);
			
			TVersion version = entry.VersionL();
			if (isBase && !entry.IsInRomL())
				{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				CleanupStack::PopAndDestroy(&entry);
#else
				CleanupStack::PopAndDestroy(2, &session); // entry
#endif
				
				const CVersion& ctrlVersion = controller.Info().Version();
				
				if (version.iMajor == ctrlVersion.Major() &&
					version.iMinor == ctrlVersion.Minor() &&
					version.iBuild == ctrlVersion.Build())
					{
					DEBUG_PRINTF(_L8("Restore - Error, attempting to restore an application that is already on device"));
					User::Leave(KErrAlreadyExists);
					}
				else
					{
					DEBUG_PRINTF(_L8("Restore - Error, attempting to restore an application that is not supported on device"));
					User::Leave(KErrNotSupported);
					}
				}
			CleanupStack::PopAndDestroy(&entry);
            // check for Augmentations as well. Need to leave with KErrAlreadyExists, if the same augmentation exists
            if (type == Sis::EInstAugmentation)
                {
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK				
				TBool foundEntryToOverwrite = FindInstalledAugmentationL(iRegistrySession, controller, entry);
#else				
                TBool foundEntryToOverwrite = FindInstalledAugmentationL(session, controller, entry);
#endif				
				entry.Close();
                if ( foundEntryToOverwrite )
                    {
                    DEBUG_PRINTF(_L8("Restore - Error, attempting to restore an augmentation of an application that is already on device"));
                    User::Leave(KErrAlreadyExists);
                    }
                }
			}
		}
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		CleanupStack::PopAndDestroy(&session);
#endif
	}

/**
 Scans the meta-data to restore and identifies any applications or upgrades that are already
 installed and should be overwritten.
 If an overwrite is required then this function removes all of the installed files 
 belonging to application using the current integrity services transaction.
 */
void CRestoreMachine::PrepareUpgradesL()
	{
	const RPointerArray<CPlan>& plans = iRestoreController->Plans();


#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif

		
	// Loop over the set of applications in the restore meta-data
	// N.B. The set of upgrades in the restore meta-data can be DIFFERENT to 
	// the set of upgrades on the device.
	TInt count = plans.Count();	
	for (TInt i = 0; i < count; ++i)
		{
		const Sis::CController& controller = plans[i]->ApplicationL().ControllerL();
		Sis::TInstallType type = controller.Info().InstallType();
		
		// Ask the registry if the application already exists.				
		RSisRegistryEntry entryToOverwrite;		
		TBool foundEntryToOverwrite = EFalse;		
		
		if (type == Sis::EInstAugmentation)
			{

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			foundEntryToOverwrite = FindInstalledAugmentationL(iRegistrySession, controller, entryToOverwrite);
#else
			foundEntryToOverwrite = FindInstalledAugmentationL(session, controller, entryToOverwrite);
#endif
         
			}
		 else 
			{
			 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			TInt err = entryToOverwrite.Open(iRegistrySession, controller.Info().Uid().Uid());
#else
			TInt err = entryToOverwrite.Open(session, controller.Info().Uid().Uid());
#endif
			 
			if (err == KErrNone)
				{
				foundEntryToOverwrite = ETrue;
				}
			else if (err != KErrNotFound)
				{
				User::Leave(err);
				}
			}
		CleanupClosePushL(entryToOverwrite);
				
		if (foundEntryToOverwrite && entryToOverwrite.IsInRomL())
			{
			// Can't overwrite entries in ROM; however, this is not a fatal error
			// because it is still valid to restore upgrades to ROM stubs. Consequently,
			// only the removal of existing files is skipped.
			DEBUG_PRINTF2(_L("Package 0x%08x is in ROM, skipping removal of files."),
					entryToOverwrite.UidL().iUid);		
			}
		else if (foundEntryToOverwrite && ! entryToOverwrite.IsPresentL())
			{
			DEBUG_PRINTF2(_L("Package 0x%08x is registered but not fully present. Skipping restore."), 
					entryToOverwrite.UidL().iUid);
			// Package exists but can't be overwritten. Leaving with KErrAlreadyExists
			// instructs SBE to skip this package but continue with the restore.
			User::Leave(KErrAlreadyExists);	
			}
		else if (foundEntryToOverwrite)
			{						
			// The application in the registry is already installed and is not in the ROM
			// This could be because the system drive has not been reformatted OR a firmware
			// upgrade also flashed the user data area.
			
			// Identify the package to be overwritten and record this in the package
			// being restored. This is needed by the SIS registry in order to overwrite
			// the controller and .reg files to overwrite. 
			CApplication* application = const_cast<CApplication*>(&plans[i]->ApplicationL());
			// controller must be controller to restore not current controller	
			application->SetUpgrade(controller);				
			CSisRegistryPackage* originalPackage=entryToOverwrite.PackageL();
			CleanupStack::PushL(originalPackage);
			application->SetPackageL(*originalPackage);	
			
			DEBUG_PRINTF5(_L("Restoring package as upgrade 0x%08x, name=\"%S\" vendor=\"%S\" index=%d"), 
					originalPackage->Uid().iUid, &originalPackage->Name(), 
					&originalPackage->Vendor(), originalPackage->Index());			
			CleanupStack::PopAndDestroy(originalPackage);

			// Remove all files registered for this package if the package is an SA or SP
			// and the version numbers mismatch.
			// If the version numbers are exactly the same then SWI avoids deleting files 
			// to work with applications that don't create full backups 
			// N.B. Files should still be overwritten if they are present in the backup.
			if (type == Sis::EInstInstallation || Sis::EInstAugmentation)
				{
				TVersion vToRestore(0,0,0);
				if (type == Sis::EInstInstallation)
					{
					// The package in the restore image could have been upgrade with a PU
					vToRestore = GetLatestVersionOfPackageToRestore();
					}
				else
					{
					// Augmentation so version number to restore is the one in the current controller
					vToRestore.iMajor = controller.Info().Version().Major();
					vToRestore.iMinor = controller.Info().Version().Minor();
					vToRestore.iBuild = controller.Info().Version().Build();
					}
				
				TVersion vCurrent(entryToOverwrite.VersionL());			
				if (vToRestore.iMajor != vCurrent.iMajor || 
					vToRestore.iMinor != vCurrent.iMinor || 
					vToRestore.iBuild != vCurrent.iBuild)
					{
					DEBUG_PRINTF2(_L("Version mismatch for package 0x%08x removing currently registered files."),
							originalPackage->Uid().iUid);
					RemovePackageFilesL(entryToOverwrite);
					}
				}			
			}
		CleanupStack::PopAndDestroy(&entryToOverwrite);
		}
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
       	CleanupStack::PopAndDestroy(&session);
#endif
	}

/**
 Gets the latest version number of the package to restore. This will either
 be the version number of the SA application or the most recent partial upgrade.
 
 @return the official version number of the package being restored.
 */
TVersion CRestoreMachine::GetLatestVersionOfPackageToRestore()
	{
	TVersion v(0,0,0);	
	
	const RPointerArray<CPlan>& plans = iRestoreController->Plans();		
	for (TInt i = plans.Count() - 1; i >= 0; --i)
		{
		const Sis::CController& controller = plans[i]->ApplicationL().ControllerL();
		Sis::TInstallType type(controller.Info().InstallType());
		if (type == Sis::EInstInstallation || type == Sis::EInstPartialUpgrade)
			{
			v.iMajor = controller.Info().Version().Major();
			v.iMinor = controller.Info().Version().Minor();
			v.iBuild = controller.Info().Version().Build();
			break;
			}
		}
	return v;
	}

/**
 Scans the set of installed augmentations to find one matching the supplied controller.
 This is necessary because SWI does not have a non-localised application name.
 Also, in order to overwrite a registry entry via UpdateEntryL the CSisRegistryPackage
 must contain the correct Index; otherwise, the SIS registry server will not be able
 to locate the correct .reg file.
 
 @param aSession			Shared connection to the SIS registry server.
 @param aControlller		The controller to restore.
 @param aEntryToOverwrite	If a match is found then this parameter will be 
							set to the registry entry to overwrite.
 @return ETrue if a matching augmentation is found; otherwise EFalse is returned.
 */
TBool CRestoreMachine::FindInstalledAugmentationL(RSisRegistrySession& aSession, const Sis::CController& aController, RSisRegistryEntry& aEntryToOverwrite)
	{
	TBool foundEntryToOverwrite = EFalse;
	
	TUid puid = aController.Info().Uid().Uid();
	const RPointerArray<CString>& names = aController.Info().Names();			
	TInt nameCount = names.Count();	
	
	// Item being restored is an SP upgrade so try and find the base
	// on the device. 
	RSisRegistryEntry augBase;
    TInt err = augBase.Open(aSession, puid);

	
	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	else if (err == KErrNone)
		{
		CleanupClosePushL(augBase);
		RPointerArray<CSisRegistryPackage> augmentations;															
		augBase.AugmentationsL(augmentations);
		CleanupStack::PopAndDestroy(); // augBase
		CleanupResetAndDestroyPushL(augmentations);

		// Check each installed augmentation against the list of names for
		// the augmentation to be restored in-case the language has changed
		TInt augCount = augmentations.Count();
		TInt i = 0;
		while (i < augCount && ! foundEntryToOverwrite)
			{				 					
			for (TInt j = 0; j < nameCount; ++j)
				{						
				if (names[j]->Data().CompareF(augmentations[i]->Name()) == 0)
					{
					// This is the SP to overwrite
                    TRAP(err, aEntryToOverwrite.OpenL(aSession, *augmentations[i]));
					
					if (err == KErrNone)
						{
						foundEntryToOverwrite = ETrue;
						break;
						}
					else if (err != KErrNotFound)
						{
						User::Leave(err);
						}					
					}
				}
			++i;	// check next installed augmenation
			}
		CleanupStack::PopAndDestroy(); // augmentations
		}
		
	return foundEntryToOverwrite;
	}

/**
 Deletes all files belonging to a registry entry using the current integrity
 services transaction. Private directories are NOT deleted because an restore
 operation that overwrites an existing application should allow additional 
 user-data that is not in the backup image to be preserved.
 
 @param aEntry 	the SIS registry entry to process.
 */
void CRestoreMachine::RemovePackageFilesL(RSisRegistryEntry& aEntry)
	{
	RPointerArray<HBufC> files;
	aEntry.FilesL(files);
	CleanupResetAndDestroyPushL(files);
	for (TInt i = files.Count() - 1; i >= 0; --i)
		{
		// Make sure drive characters have been resolved correctly
		TPtr fileName = files[i]->Des();
		if (fileName[0] == '$')
			{
			fileName[0] = RFs::GetSystemDriveChar();
			}
		else if (fileName[0] == '!')
			{
			fileName[0] = aEntry.SelectedDriveL();
			}

		if (iFs.IsValidName(fileName)) // Ensure wildcarded files aren't processed
			{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			TRAPD(err, iStsSession.RemoveL(fileName));
#else
			TRAPD(err, iIntegrityServices->RemoveL(fileName));
#endif
			if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
				{
				DEBUG_PRINTF3(_L("Error %d failed to delete file %S"), err, &fileName);
				User::LeaveIfError(err);
				}

			// If the file is in an exe or library then the hash must also be removed
			TParsePtrC parse(fileName);
			_LIT(KSysBin, "\\sys\\bin\\");
			if (parse.Path().CompareF(KSysBin) == 0)
				{
				RBuf hashFileName;
				hashFileName.CreateL(KMaxFileName);
				CleanupClosePushL(hashFileName);
				_LIT(KSysHash, "$:\\sys\\hash\\");
				hashFileName.Append(KSysHash);
				hashFileName[0] = iFs.GetSystemDriveChar();
				hashFileName.Append(parse.NameAndExt());
				DEBUG_PRINTF2(_L("Removing hash %S"), &hashFileName);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			TRAP(err, iStsSession.RemoveL(hashFileName));
#else
			TRAP(err, iIntegrityServices->RemoveL(hashFileName));
#endif
			
				if (err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
					{
					DEBUG_PRINTF3(_L("Error %d failed to delete file %S"), err, &fileName);
					User::LeaveIfError(err);
					}
				CleanupStack::PopAndDestroy(&hashFileName);
				}
			}
		}
	CleanupStack::PopAndDestroy(&files);
	}
	
void CRestoreMachine::ServiceCommitRequestL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L8("Restore - Commiting restore session"));
	
	// Finally, add remaining dummy stuff to the plan
	iRestoreController->AddEmbeddedAppsAndFilesL();

	// Dummy plan for use with the restore processor, we use
	// CFileCopyDescriptions directly when doing a restore
	const RPointerArray<CPlan>& plans = iRestoreController->Plans(); 

	iCommitWatcher = CCommitWatcher::NewL(plans, *iSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iStsSession, iRegistrySession,
#else
		*iIntegrityServices, 
#endif
		*iRestoreController, aMessage, iFs, *this);

	iCommitWatcher->StartL();
	}
	
CRestoreMachine::~CRestoreMachine()
	{
	delete iRestoreController;
	delete iSecurityManager;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.Close();
	iRegistrySession.Close();
#else
	delete iIntegrityServices;
#endif
	delete iBuf;
	if (iCommitWatcher)
		{
		iCommitWatcher->Cancel();
		}
	delete iCommitWatcher;
	delete iFileMan;
	iFs.Close();

	// close SWI Observer session
	iObserver.Close();
	}

/**
	@return SWI Observer session handle.
 */
RSwiObserverSession& CRestoreMachine::Observer()
	{
	return iObserver;
	}
		
//
// CCommitWatcher
//
	
CRestoreMachine::CCommitWatcher* CRestoreMachine::CCommitWatcher::NewL(const RPointerArray<CPlan>& aPlans, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	CRestoreController& aController, const RMessage2& aMessage, RFs& aFs, CRestoreMachine& aMachine)
	{
	CCommitWatcher* self = new (ELeave) CCommitWatcher(aPlans, aSecurityManager, 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		aStsSession, aRegistrySession,
#else
		aIntegrityServices,
#endif
		aController, aMessage, aFs, aMachine);
	return self;
	
	}

CRestoreMachine::CCommitWatcher::CCommitWatcher(const RPointerArray<CPlan>& aPlans, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	CRestoreController& aController, const RMessage2& aMessage, RFs& aFs, CRestoreMachine& aMachine)
	: CActive(CActive::EPriorityStandard),
	  iClientMessage(aMessage), 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	  iStsSession(aStsSession),
	  iRegistrySession(aRegistrySession),
#else
	  iIntegrityServices(aIntegrityServices),
#endif
	  iRestoreController(aController), iSecurityManager(aSecurityManager),
	  iPlans(aPlans), iFs(aFs), iMachine(aMachine)
	{
	CActiveScheduler::Add(this);
	iPlanIndex = -1;
	iVerifierIndex = -1;	
	}
	
void CRestoreMachine::CCommitWatcher::RunL()
	{
	SwitchStateL();
	}
	 
void CRestoreMachine::CCommitWatcher::DoCancel()
	{
	if (iProcessor)
		{
		iProcessor->Cancel();
		}
	}	

CRestoreMachine::CCommitWatcher::~CCommitWatcher()
	{
	Cancel();
	delete iProcessor;
	}

void CRestoreMachine::CCommitWatcher::StartL()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}	
	
void CRestoreMachine::CCommitWatcher::SwitchStateL()
	{
	if (++iVerifierIndex < iRestoreController.Verifiers().Count() && iStatus.Int() == KErrNone)
		{
		
		iRestoreController.Verifiers()[iVerifierIndex]->StartL(iStatus);
		SetActive();
		}
	else if (++iPlanIndex < iPlans.Count() && iStatus.Int() == KErrNone)
		{
		
		CApplication& app = const_cast <CApplication&> (iPlans[iPlanIndex]->ApplicationL());
		// Check the application is installed based on su cert rules
		Sis::TInstallType type = app.ControllerL().Info().InstallType();
		Sis::TInstallFlags flag = app.ControllerL().Info().InstallFlags();
		TBool allowedType = (type == EInstInstallation || (type == EInstAugmentation) || type == EInstPartialUpgrade );
		TBool allowedFlag = (flag & EInstFlagROMUpgrade);
	
		if (app.ControllerL().IsSignedBySuCert() && allowedType && allowedFlag && 
			!app.IsStub() && app.ControllerL().TrustStatus().ValidationStatus()
			>= EValidatedToAnchor)
			{
			app.SetInstallSuCertBased();
			}

		RArray<TUid> baseSids;
		CleanupClosePushL(baseSids);
		
		// check to see if the plan we are about to process is a valid upgrade
		if (!IsValidUpgradeL(app.ControllerL(), baseSids))
			{
			User::Leave(KErrInvalidUpgrade);
			}
		
		if (!ValidEclipsingL(app))
			{
			User::Leave(KErrInvalidEclipsing);
			}
		
		delete iProcessor;

		const RPointerArray<TPtrC8>& controllerBinaries = iRestoreController.ControllerBinaries();
		ASSERT(controllerBinaries.Count() == iPlans.Count());
		DEBUG_PRINTF3(_L8("Restoring plan %d out of %d"), iPlanIndex, iPlans.Count());
		const TDesC8& controllerBinary = *controllerBinaries[iPlanIndex];
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		/*During restore the controllers are processed sequentialy one by one, ecah controller has one CPlan object (which internally has
		CApplication object), we maintain the set of affected apps for each controller within its CPlan object(done while doning processing)
		and we use the same set of affected apps for the subsequent controllers. For example In case of SP over SA ,while processing the SA controller we will not have
		any affected apps as it is the first controller but for the second SP controller we will tahke the affected apps from the previously 
		processed controller ie SA.
		*/
		RArray<TAppUpdateInfo> appInfo;
		CleanupClosePushL(appInfo);		
		if(iPlanIndex > 0)
		    {
		    iPlans[iPlanIndex-1]->GetAffectedApps(appInfo);
		    }
#endif
		
		iProcessor = CRestoreProcessor::NewL(*(iPlans[iPlanIndex]), controllerBinary, iSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			iStsSession, iRegistrySession, appInfo,
#else
			iIntegrityServices,
#endif
			iRestoreController.Verifiers(), baseSids, iMachine.Observer()); 

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		CleanupStack::Pop(&appInfo);
#endif
		iProcessor->ProcessPlanL(iStatus);
		CleanupStack::PopAndDestroy(&baseSids);
		
		SetActive();
		}
	else
		{
		CompleteL();
		}
	}
	
TBool CRestoreMachine::CCommitWatcher::ValidEclipsingL(const CApplication& aApplication)
	{
	if (aApplication.IsInstallSuCertBased())
		{
		return ETrue;
		}

	RPointerArray<HBufC> eclipsableFiles;
	CleanupResetAndDestroyPushL(eclipsableFiles);
	
	// check which (if any) files we're allowed to eclipse
	Sis::TInstallType type = aApplication.ControllerL().Info().InstallType();
	if (type != EInstPreInstalledApp &&
		aApplication.ControllerL().TrustStatus().ValidationStatus() >= EValidatedToAnchor)
		{
		// we're an upgrade (includes SA with RU flag upgrade over ROM) and trusted sufficiently. 
		// Check if we're a suitable candidate.

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		RSisRegistrySession session;
		User::LeaveIfError(session.Connect());
		CleanupClosePushL(session);
#endif
		
		RSisRegistryEntry entry;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TInt err = entry.Open(iRegistrySession, aApplication.ControllerL().Info().Uid().Uid());
#else
		TInt err = entry.Open(session, aApplication.ControllerL().Info().Uid().Uid());
#endif
		if (err == KErrNone)
			{
			CleanupClosePushL(entry);

			if (entry.IsInRomL())
				{
				// we can eclipse files from this package
				entry.FilesL(eclipsableFiles);
				}
				
			CleanupStack::PopAndDestroy(&entry);
			}
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		CleanupStack::PopAndDestroy(&session);
#endif
		}
		
	// check for files that eclipse Z:
	
	TBool ret = ETrue;
	const RPointerArray<CSisRegistryFileDescription>& files = aApplication.FilesToAdd();
	TInt count(files.Count());

	TFileName file;
	for (TInt i = 0; i < count; ++i)
		{
		// find file descriptions that eclipse Z:
		file.Zero();
		file.Copy(files[i]->Target());
		file[0] = 'z';
		
		TEntry entry;
		if (KErrNone == iFs.Entry(file, entry))
			{
			TBool found = EFalse;
			// we've got an eclipse, check it against the list of allowed eclipses
			for (TInt j = 0; j < eclipsableFiles.Count(); ++j)
				{
				if (0 == file.CompareF(*eclipsableFiles[j]))
					{
					found = ETrue;
					break;
					}
				}
			
			if (!found)
				{
				ret = EFalse;
				break; // return immediately.
				}	
			}
		}

	CleanupStack::PopAndDestroy(&eclipsableFiles);
	return ret;
	}
TBool CRestoreMachine::CCommitWatcher::IsValidUpgradeL(const Sis::CController& aController, RArray<TUid>& aSids)
	{
	
	Sis::TInstallType type = aController.Info().InstallType();
	TBool isBase = ((type == EInstInstallation) || (type == EInstPreInstalledApp));
	TBool inROM(EFalse);
	
	// ask the registry if the UID already exists.
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif

	RSisRegistryEntry installedBase;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    TInt err = installedBase.Open(iRegistrySession, aController.Info().Uid().Uid());
#else
	TInt err = installedBase.Open(session, aController.Info().Uid().Uid());
#endif

	if (err == KErrNone)
		{
		CleanupClosePushL(installedBase);
		
		// Found the package entry. Check status
		TSisTrustStatus status = installedBase.TrustStatusL();
			
		// Add the SIDs belonging to the base package
		installedBase.SidsL(aSids);
		
        


		// Check weather the package is ROM based or not.
		inROM = installedBase.IsInRomL();
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		CleanupStack::PopAndDestroy(&installedBase);
#else
		CleanupStack::PopAndDestroy(2, &session); //session, entry
#endif

		if (inROM)
			{
			// return true if we're sufficiently trusted to upgrade this package
			return SecurityCheckUtil::IsSufficientlyTrusted(aController, status.ValidationStatus());
			}		
		else
			{
			// SP or PU upgrade where the base package has been overwritten
			return ETrue;
			}
		}
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CleanupStack::PopAndDestroy(&session);
#endif

	// No match found. If this is an upgrade, the we'd return false 
	// (missing base package)
	if (!isBase)
		{
		return EFalse;
		}
			
	// Otherwise, if this is just a regular install, just return true.
	return ETrue;
	}
	
	
void CRestoreMachine::CCommitWatcher::CompleteL()
	{
	DEBUG_PRINTF2(_L8("Restore - Restore completed with status %d"), iStatus.Int());
	
	TUid pubsubCategory;
	pubsubCategory.iUid = KUidSystemCategoryValue;
	
	if (iStatus.Int() == KErrNone)
		{
		
		TInt err = RProperty::Set(pubsubCategory, KUidSoftwareInstallKey, ESwisStatusSuccess | ESwisRestore);
		User::LeaveIfError(err);
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// NOTE: There is a caveat here as we need to commit two independant transactions and there 
		// exists a possibility that the second commit fails and this would leave the device in an 
		// inconsistent state - there is no known solution for this
		// Due to the fact that SCR allows only one transaction at a time (one writer, many readers), 
		// the possibility of the registry commit (SCR commit) failing is less likely than an STS commit 
		// failure - therefore we commit STS first and then the registry 
		iStsSession.CommitL();
		iRegistrySession.CommitTransactionL();
   
		CPlan* lastPlan = iPlans[iPlans.Count()-1];
		if (lastPlan)
            {
            RSisLauncherSession launcher;  
            CleanupClosePushL(launcher);
            if (launcher.Connect() != KErrNone)
                {
                iMachine.Observer().CommitL();
                User::LeaveIfError(RProperty::Set(pubsubCategory, KUidSoftwareInstallKey, ESwisNone));
                iClientMessage.Complete(iStatus.Int());
                DEBUG_PRINTF(_L8("Install Machine - Failed to connect to SisLauncher"));
                CleanupStack::Pop(&launcher);
                launcher.Close();
                return;
                }             
             //Notify apparc for the the change in the Applications
             RArray<TAppUpdateInfo> affectedApps;
             CleanupClosePushL(affectedApps);
             lastPlan->GetAffectedApps(affectedApps);
#ifdef _DEBUG             
             for(TInt i = 0; i < affectedApps.Count(); i++)
                  {
                  DEBUG_PRINTF2(_L("Affected AppUid during restore is 0x%x"), affectedApps[i].iAppUid);
                  DEBUG_PRINTF2(_L("Action to be performed is %d"), affectedApps[i].iAction);
                  }  
#endif             
             if (affectedApps.Count() > 0)
                  {
                  launcher.NotifyNewAppsL(affectedApps);
                  }          
             CleanupStack::PopAndDestroy(2, &launcher);             
             }        
       
		
#else
		iIntegrityServices.CommitL();
#endif
		iMachine.Observer().CommitL();
		}
	else 
		{
		
		TInt err = RProperty::Set(pubsubCategory, KUidSoftwareInstallKey, ESwisStatusAborted | ESwisRestore);
		User::LeaveIfError(err);
		iMachine.Observer().Close();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iStsSession.RollBackL();
		iRegistrySession.RollbackTransactionL();
#else
		iIntegrityServices.RollBackL(EFalse);
#endif
		}
	
	User::LeaveIfError(RProperty::Set(pubsubCategory, KUidSoftwareInstallKey, ESwisNone));
	iClientMessage.Complete(iStatus.Int());
	}
	
TInt CRestoreMachine::CCommitWatcher::RunError(TInt aError)
	{
	//Close the swi observer session handle to allow transaction service
	//to delete the observation log file.
	iMachine.Observer().Close();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TRAP_IGNORE(iStsSession.RollBackL());
	TRAP_IGNORE(iRegistrySession.RollbackTransactionL());
#else
	TRAP_IGNORE(iIntegrityServices.RollBackL(EFalse));
#endif
	iClientMessage.Complete(aError);
	return KErrNone;
	}
