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


#include <s32mem.h>


#include "restorecontroller.h"

#include "arrayutils.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sisversion.h"
#include "sissupportedlanguages.h"
#include "sisinstallblock.h"
#include "sishash.h"
#include "userselections.h"
#include "siselseif.h"
#include "log.h"
#include "securitycheckutil.h"
#include "siscapabilities.h"
#include "certchainconstraints.h"
#include "sisregistryfiledescription.h"
#include "sistruststatus.h"
#include "secutils.h"

#include <f32file.h>

using namespace Swi;
using namespace Swi::Sis;

//
// TRestoreFileInformation
//

TRestoreFileInformation::TRestoreFileInformation(CFileDescription& aFileDescription, CApplication& aParentApplication)
	: iFileDescription(aFileDescription), 
	iParentApplication(aParentApplication)	
	{
	}
  

//
// CRestoreController
// 
   
void CRestoreController::ConstructL(TDesC8& aController)
	{				
	iSystemDriveChar = RFs::GetSystemDriveChar();
	
	// Read the controllers from the buffer		
	InternalizeControllersL(aController);
	
	//We also create a dummy plan for use when writing the 
	//registry entries
	TInt augmentationIndex(0);
	TInt noOfMatchingLanguages = iMatchingSupportedLanguagesArray.Count();
	TInt matchingLanguageCount = 0;
	for (TInt i = 0; i < iControllers.Count(); i++)
		{
		
		CPlan* plan = CPlan::NewL();
		CleanupStack::PushL(plan);
		User::LeaveIfError(iInstallPlans.Append(plan));
		CleanupStack::Pop(plan);
		
		CApplication* application = CApplication::NewLC();
		
		CController* controller = iControllers[i];
		
		DEBUG_PRINTF3(_L8("Restore - Metadata controller %d is of type %d"),
			i, controller->Info().InstallType());
		
 		// Set the install type 		
 		switch (controller->Info().InstallType())
 			{
 		case EInstInstallation:
 			application->SetInstall(*controller);
			application->UserSelections().SetDrive(iInstallDrive);		
			
			//if there are macthing languages then populate iMatchingDeviceLanguages array of application object
			if (noOfMatchingLanguages)
				{				
				while( iMatchingSupportedLanguagesArray[matchingLanguageCount] != ELangNone)
					{		
					DEBUG_PRINTF2(_L8("Device matching language for Restore - %d"),iMatchingSupportedLanguagesArray[matchingLanguageCount]);
					application->PopulateMatchingDeviceLanguagesL(iMatchingSupportedLanguagesArray[matchingLanguageCount++]);					
					}				
				}
			++matchingLanguageCount;
 			break;
 		
 		case EInstPartialUpgrade:
 			application->SetPartialUpgrade(*controller);
			application->UserSelections().SetDrive(iInstallDrive);	
			// Carry forwards the in-rom flag
			if (i == 0 || iTopLevelApplications[0]->IsInROM()) 				
				{
				// If the first controller is a parial upgrade then 
				// it must be a partial upgrade to a ROM stub.
				// If there are multiple PUs then they must also be in-rom
				application->SetInROM();
				}
	         if ( noOfMatchingLanguages )
	                {
	                TInt i = 0;
	                while( iMatchingSupportedLanguagesArray[i] != ELangNone)
	                    {
	                    application->PopulateMatchingDeviceLanguagesL(iMatchingSupportedLanguagesArray[i++]);
	                    }
	                }
 			break;
 			
		case EInstAugmentation:	
			application->SetAugmentation(*controller);
			application->UserSelections().SetDrive(iAugmentationDrives[augmentationIndex++]);
			
			//if there are macthing languages then populate iMatchingDeviceLanguages array of application object
			if ( noOfMatchingLanguages)
				{				
				while( iMatchingSupportedLanguagesArray[matchingLanguageCount] != ELangNone)
					  {
					  DEBUG_PRINTF2(_L8("Device matching language for Restore - %d"),iMatchingSupportedLanguagesArray[matchingLanguageCount]);
					  application->PopulateMatchingDeviceLanguagesL(iMatchingSupportedLanguagesArray[matchingLanguageCount++]);
					   
					  }				
				}
			++matchingLanguageCount;
			break;
 			
 		default:
 			User::Leave(KErrNotSupported);
 			break;
 			
 			}
 					
		TPtrC8* buf = iControllerBinaries[i];
		
		application->SetController(*controller);
		
		CSisCertificateVerifier* verifier = CSisCertificateVerifier::NewLC(controller, *buf, iSecurityManager, application);
		User::LeaveIfError(iVerifiers.Append(verifier));
		CleanupStack::Pop(verifier);
		
		TInt languageIndex = SetApplicationLanguageL(*application);
	
		//This is the first controller, add it to the plan
		User::LeaveIfError(iTopLevelApplications.Append(application));
		plan->SetApplication(application); // ownership transferred.
			
		const CVersion& version = controller->Info().Version();
		TVersion appVersion(version.Major(), version.Minor(), version.Build());
			
		plan->SetApplicationInformationL(controller->Info().Names()[languageIndex]->Data(),
			controller->Info().VendorNames()[languageIndex]->Data(),
			appVersion);	
		
		CleanupStack::Pop(application);			
		}	
	}
	
/**
 Locates the relevant file description in the controller and adds the file
 to be restored to the list of files for the new SIS registry object.
 
 This also checks the hash of the file if needed and generates an appropriate temporary
 filename to be used by integrity services.
 
 @param		aFile				file handle to temporary restore file.
 @param		aTempFileName		temporary filename to use with integrity services.
 @param		aHashCheckRequried	whether the restore files hash must be checked against the controllers
 */
void CRestoreController::AddFileDescriptionL(RFile& aFile, TDesC& aTarget, TDes& aTempFilename, TBool aHashCheckRequired)
  	{
  	// Lookup file information and check the hash if needed  	
  	TRestoreFileInformation info = LookupFileInformationL(aFile, aTarget, aHashCheckRequired);
  	
  	// Add the file description to the plan
  	CApplication& application = info.ParentApplication();
  	application.AddFileL(info.FileDescription(), application.UserSelections().Drive());
  	
  	// Construct the temporary file name for this file
  	TParsePtrC parse(aTarget);  	
  	_LIT(KTemporaryFileFormat, "%c:%Stfile-%d-%d-%d-");
  	TUint driveCh(iSystemDriveChar);
  	aTempFilename.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath, 
  		application.ControllerL().Info().Uid().Uid().iUid, application.AbsoluteDataIndex(), info.FileDescription().Index());
  	 
  	// Append a hex representation of a hash of the filename. We need the
  	// temporary filename to be effectively unique, but the filename itself
  	// may be too long.
  	
  	// Use the real target path (after path subsitution) minus the drive letter.
  	HBufC* hashBuf = SecUtils::HexHashL(aTarget.Right(aTarget.Length() - 1));
  	aTempFilename.Append(*hashBuf);
  	delete hashBuf;
  	
  	// finally, remove this file from the list of files to add entries for
  	for (TInt i = 0; i < iInstalledFiles.Count(); ++i)
  		{  		
  		HBufC* filename = iInstalledFiles[i];	
  		if (aTarget.CompareF(*filename) == 0)
  			{
  			delete filename;
  			iInstalledFiles.Remove(i);
  			break;
  			}  		
  		}  	
  	}
  
void CRestoreController::AddEmbeddedAppsAndFilesL()
  	{
	//for each application/controller pair, add fake file and embedded package descriptions
	for (TInt i = 0; i < iTopLevelApplications.Count(); ++i)
		{
		DoAddEmbeddedAppsAndFilesL(iTopLevelApplications[i]->ControllerL().InstallBlock(), *iTopLevelApplications[i]); 
		}
	}

// Preserves the links between embedded applications in the registry, so uninstalling the 
// top level package will attempt to uninstall the packages below it in the tree
// However, these applications are not processed as part of the restore 
// hence they are entirely fake	

void CRestoreController::DoAddEmbeddedAppsAndFilesL(
	const CInstallBlock& aInstallBlock, CApplication& aApplication)
	{
	
	const RPointerArray<CController>& controllers = aInstallBlock.EmbeddedControllers();

  	TInt i;	
	for (i = 0; i < controllers.Count(); ++i)
		{
		
		CController* controller = controllers[i];
		
		CApplication* application = CApplication::NewLC();
		application->SetController(*controller);
		SetApplicationLanguageL(*application);
		aApplication.AddEmbeddedApplicationL(application); // Ownership transfered
		CleanupStack::Pop(application);
		
		}
		
  	const RPointerArray<CFileDescription>& fileDescriptions = aInstallBlock.FileDescriptions();
  	TChar foldedDrive = aApplication.UserSelections().Drive();
  	if (foldedDrive != 255)
  		{
  		foldedDrive.Fold();	// don't convert 'no drive selection' to the y drive 
  		}
  	
  	for (i = 0; i < fileDescriptions.Count(); ++i)
  		{
  		const TDesC& description = fileDescriptions[i]->Target().Data();
  		if (description.Length() != 0)
  			{
  			TChar sisDrive = description[0];
  			if (sisDrive == '$')
  				{
  				sisDrive = iFs.GetSystemDriveChar();	
  				}  			
  			sisDrive.Fold();
  			
  			for (TInt j = 0; j < iInstalledFiles.Count(); j++)
  				{
  				HBufC* filename = iInstalledFiles[j];
  				TChar targetDrive = (*filename)[0];
  				targetDrive.Fold();
  			
				// if the file description matches this filename (taking into account drive letter)
				// add it to the application.
				// This may not be 100% the correct file description, but it will be close enough
				if (description.Mid(1).CompareF(filename->Mid(1)) == 0 &&
				    ((targetDrive == foldedDrive) || (targetDrive == sisDrive)))
				       {
				       aApplication.AddFileL(*fileDescriptions[i], aApplication.UserSelections().Drive());
				       // Cut down the search space some
				       delete filename;
				       iInstalledFiles.Remove(j);
				       break;
				       }
				}
			}
  		}
  		
	const RPointerArray<CIf>& ifBlocks = aInstallBlock.IfStatements();

	for (i = 0; i < ifBlocks.Count(); ++i)
		{
		
		CIf* ifBlock = ifBlocks[i];
		DoAddEmbeddedAppsAndFilesL(ifBlock->InstallBlock(), aApplication);
		
		const RPointerArray<CElseIf>& elseBlocks = ifBlock->ElseIfs();
		
		for (TInt j = 0; j < elseBlocks.Count(); j++)
			{
			
			CElseIf* elseBlock = elseBlocks[j];
			DoAddEmbeddedAppsAndFilesL(elseBlock->InstallBlock(), aApplication);
			
			}
		
		}
		
	}	
// Creates a TPtrC8* which points to the data of controllers stored in MetaData which was backedup.
// Not using HBufC8::NewLC as it allocates memory afresh for the data.
TPtrC8* CRestoreController::CreateRawControllerPtrLC(RDesReadStream& aStream, const TUint8* aBasePtr, TInt& aOffset )
	{
	TCardinality card;
	aStream>>card;
	TInt tmpLen = (TInt)card;
	//Right shift TCardinality by one as its LSB is not
	// used to represent the length of the descriptor
	TInt len = ((TInt)card) >> 1;

	//TCardinality uses 
	// 1 Byte to store the length if it is between 0-127 
	// 2 Bytes if length is between 128-16383
	// 4 Bytes if length > 16383
	
	if ( tmpLen < 128 )
		aOffset+=1;
	else if ( tmpLen < 16384 )
		aOffset+=2;
	else
		aOffset+=4;
	
	TPtrC8* controllerBuf = new (ELeave)TPtrC8(); 
	controllerBuf->Set(aBasePtr+aOffset,len);
	CleanupStack::PushL(controllerBuf);
	
	aOffset += len;
	
	while( len-- )
		aStream.ReadInt8L();

	return controllerBuf;
	}

void CRestoreController::InternalizeControllersL(TDesC8& aController)
	{
	
	RDesReadStream stream(aController);
	CleanupClosePushL(stream);
	TInt offset = 0;
	const TUint8* basePtr = aController.Ptr();
	
	// Drive letter to install to
	iInstallDrive = TChar(stream.ReadUint32L());
	offset+=4;
	
	// We need to count the augmentations to know how many selected drives to
	// read after the list of files.
	TInt augmentationsCount(0);

	TInt controllerCount = stream.ReadInt32L();
	DEBUG_PRINTF2(_L8("Restore - Metadata consists of %d seperate controllers"), 
		controllerCount);
	offset+=4;
	
	while (controllerCount--)
		{
		TPtrC8* controllerBuf = CreateRawControllerPtrLC(stream,basePtr,offset);
		
		User::LeaveIfError(iControllerBinaries.Append(controllerBuf));
		CleanupStack::Pop(controllerBuf);
		
		// Create a CController object from the binary
		
		CDesDataProvider* provider = CDesDataProvider::NewLC(*controllerBuf);
		CController* controller = CController::NewLC(*provider, EAssumeType);
		User::LeaveIfError(iControllers.Append(controller));
		CleanupStack::Pop(controller);
		if (controller->Info().InstallType() == EInstAugmentation)
			{
			augmentationsCount++;
			}
		CleanupStack::PopAndDestroy(provider);
		
		}
		
  	// Trap this to make the metadata file format BC	
  	TInt filesCount = 0;
  	TRAPD(err, filesCount = stream.ReadInt32L());
  	// If this is an old style backup metadata file, it won't include this section
  	if (err == KErrNone)
  		{
  		while (filesCount--)
  			{
  			HBufC* filename = HBufC::NewLC(stream, KMaxTInt);
  			User::LeaveIfError(iInstalledFiles.Append(filename));
  			CleanupStack::Pop(filename);
  			}

 		// Trap reading augmentations' selected drives, which appear after the
 		// file list in metadata files created after the fix for DEF087124.
 		// Leave if the read fails beyond the first augmentation, since we should
 		// have all or none.
  		for (TInt i = 0; i < augmentationsCount; i++)
			{
			if (err == KErrNone)
				{
  				TChar selectedDrive(iInstallDrive);
  				TRAP(err, selectedDrive = stream.ReadInt32L());
  				if (i != 0)
  					{
  					User::LeaveIfError(err);
  					}
				iAugmentationDrives.AppendL(selectedDrive);
				}
			else
				{
				// If we failed on reading the first drive, fill the rest of
				// the array with the selected drive for the base package.
				iAugmentationDrives.AppendL(iInstallDrive);			
				}
			}
  		}
  		
  		// Reading the array of matching device supported languages
  		TRAP(err, InternalizeArrayL(iMatchingSupportedLanguagesArray,stream));

	CleanupStack::PopAndDestroy(&stream);
	}
	
		
CRestoreController* CRestoreController::NewL(TDesC8& aController, CSecurityManager& aSecurityManager, RFs& aFs)
	{
	
	CRestoreController* self = CRestoreController::NewLC(aController, aSecurityManager, aFs);
	CleanupStack::Pop(self);
	return self;
	
	}
		
CRestoreController* CRestoreController::NewLC(TDesC8& aController, CSecurityManager& aSecurityManager, RFs& aFs)
	{
	
	CRestoreController* self = new (ELeave) CRestoreController(aSecurityManager, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aController);
	return self;
		
	}
		
CRestoreController::CRestoreController(CSecurityManager& aSecurityManager, RFs& aFs)
	: iFs(aFs), iSecurityManager(aSecurityManager)
	{
	}
 /**
  Finds the matching file description in the controller and checks the hash if required.
  
  @param		aFile				handle to the file to restore
  @param 	aTarget				path that the file handle should be restored.
  @param		aHashCheckRequired	if the target path is TCB protected and a hash check is required.
  */
 TRestoreFileInformation CRestoreController::LookupFileInformationL(RFile& aFile, TDesC& aTarget, TBool aHashCheckRequired)


	{
	CFileDescription* desc = NULL;
	
	// Search through the controllers backwards in case an executable has been ugpraded with 
	// a partial upgrade
	TBool foundMatch = EFalse;
	TInt i = iControllers.Count() - 1;
	CApplication* application = NULL;
	while (i >= 0 && ! foundMatch)
		{
		CController* controller = iControllers[i];
		application = iTopLevelApplications[i];
		TChar drive = application->UserSelections().Drive();
		desc = FindFileDescriptionL(controller->InstallBlock(), aFile, aTarget, aHashCheckRequired, drive);
				
		if (desc)
			{
			foundMatch = ETrue;
			}
		else
			{
			--i;	// Check the next controller
			}
		}
		
	if (aHashCheckRequired && ! foundMatch)
		{
		// Previously the code would return either KErrNotFound or KErrSISWouldOverWrite
		// both of these errors are incorrect and SBE does not depend on the exact error code
		// in this scenario.
		// N.B. It is already possible for KErrSecurityError to be returned so the backup
		// client MUST be able to handle this.
		DEBUG_PRINTF2(_L("No matching hash for file %S"), &aTarget);
		User::Leave(KErrSecurityError);
		}
	else if (! foundMatch)
		{
		// It's difficult to report VerifyOnRestore mis-matches as a security error because
		// the file could be present in multiple controllers and in multiple install blocks
		// with and without the VerifyOnRestore flag set
		DEBUG_PRINTF2(_L("No match in controller for %S"), &aTarget);
		User::Leave(KErrNotFound);
		}
	
	// This is where things get really interesting ...
	// The restore machine invokes a restore processor for each controller in turn
	// and as well as checking the exe header the restore processor builds a list of SIDs
	// as it goes.
	// When the files are installed private directories are checked against the list of SIDs.
	// BUT
	// If the exe has been upgraded via a partial upgrade then the application pointer here will
	// refer to the partial upgrade that hasn't been checked yet so the SID isn't known and the processor
	// will reject the restore of files to that private dir.
	// Reversing the order of processing the controllers doesn't help because if the exe
	// hasn't been upgraded but the PU deploys a file to the exe's private dir then again the SID
	// isn't known so the install will be rejected.
	
	// The work around is to associate all of the files supplied by SBE for PUs with
	// the first application which will be the SA
	// It's not safe blindly do this for all applications because this would incorrectly associate
	// the augmenation's files with the SP
	if (application->IsPartialUpgrade()) 
		{
		application = iTopLevelApplications[0];
		}
	TRestoreFileInformation result(*desc, *application);
	return result;	
	}
	
CFileDescription* CRestoreController::FindFileDescriptionL(
		const CInstallBlock& aInstallBlock, RFile& aFile, TDesC& aTarget, TBool aHashCheckRequired, const TChar& aDrive)
	{	
	const RPointerArray<CFileDescription> descriptions = aInstallBlock.FileDescriptions();
	CFileDescription* result = NULL;
	
	for (TInt j = 0; j < descriptions.Count(); j++)
		{
		CFileDescription* description = descriptions[j];		
		const TDesC& descTarget = (description->Target()).Data();
		
		// We'll acknowledge that the file matches the description iff:
		// a) The hashes match
		// b) The targets match, taking into account wildcard drive letters
		
		// substitute the drive letter and replace any relevant paths
		HBufC* transDest =
			iSecurityManager.SecurityPolicy().ResolveTargetFileNameLC(descTarget, aDrive);
		
		// check if this file matches the file name		
		TBool match = (transDest->Left(1).CompareF(aTarget.Left(1)) == 0)
						&&(transDest->Mid(1) == aTarget.Mid(1));
		
		// if it does, do we also need to do as hash check to approve this file?						
		if ((aHashCheckRequired || (description->OperationOptions() & EInstVerifyOnRestore)) && match)
			{
			// If the file is TCB protected OR the VerifyOnRestore flag is set the 
			// a match is only valid if the hash also matches
			const CHash& hash = description->Hash();
			TSISHashAlgorithm algorithm = hash.Algorithm();			
			CMessageDigest* temp = CalculateHashL(aFile, algorithm);
			CleanupStack::PushL(temp);			
			match = (temp->Final() == hash.Data());
			DEBUG_PRINTF3(_L("Checking hash candiate for file %S pass = %d"), &aTarget, match);
			CleanupStack::PopAndDestroy(temp);
			}		
		CleanupStack::PopAndDestroy(transDest);		
		if (match)
			{
			return description;
			}
		}
	
	const RPointerArray<CIf>& ifBlocks = aInstallBlock.IfStatements();
	for (TInt i = 0; i < ifBlocks.Count(); i++)
		{		
		CIf* ifBlock = ifBlocks[i];
		if (NULL != (result = FindFileDescriptionL(ifBlock->InstallBlock(), aFile, aTarget, aHashCheckRequired, aDrive)))
			{			
			return result;			
			}
		
		const RPointerArray<CElseIf>& elseBlocks = ifBlock->ElseIfs();		
		for (TInt j = 0; j < elseBlocks.Count(); j++)
			{			
			CElseIf* elseBlock = elseBlocks[j];
			if (NULL != (result = FindFileDescriptionL(elseBlock->InstallBlock(), aFile, aTarget, aHashCheckRequired, aDrive)))
				{				
				return result;			
				}			
			}		
		}		
	return result;	
	}
	
		
CMessageDigest* CRestoreController::CalculateHashL(RFile& aFile, TSISHashAlgorithm aAlgorithm)
	{		
	HBufC8* buf = HBufC8::NewLC(1024);
	TPtr8 bufPtr(buf->Des());
	TInt seekPos = 0;
	aFile.Seek(ESeekStart, seekPos);	
	CMessageDigest* digest = NULL;
	
	switch (aAlgorithm)
		{		
	case EHashAlgSHA1:
		digest = CSHA1::NewL();
		break;
			
	default:
		User::Leave(KErrNotSupported);
		break;		
		}
			
	while (KErrNone == aFile.Read(bufPtr) && bufPtr.Length() != 0)
		{			
		digest->Update(bufPtr);		
		}
			
	CleanupStack::PopAndDestroy(buf);			
	return digest;				
	}
		
TInt CRestoreController::SetApplicationLanguageL(CApplication& aApplication)
	{
	
	// We set either the current locale if available, or the default
	// language.
	
	TLanguage locale = User::Language();
	const CSupportedLanguages& languages = aApplication.ControllerL().SupportedLanguages();
	TInt langIndex = 0;
	
	for (TInt i = 0; i < languages.Count(); i++)
		{
		
		if (locale == languages[i])
			{
			
			langIndex = i;
			break;
			
			}
			
		}
		
	aApplication.UserSelections().SetLanguage(languages[langIndex]);
	return langIndex;
	
	}
	
	
CRestoreController::~CRestoreController()
	{
		
	iControllers.ResetAndDestroy();
	iVerifiers.ResetAndDestroy();
	iControllerBinaries.ResetAndDestroy(); 
	iInstallPlans.ResetAndDestroy();
	iTopLevelApplications.Reset(); // These pointers owned by the plan
	iInstalledFiles.ResetAndDestroy();
	iAugmentationDrives.Close();
	iMatchingSupportedLanguagesArray.Close();
	}
		
		
//
// CSisCertificateVerifier
//

		
CRestoreController::CSisCertificateVerifier* CRestoreController::CSisCertificateVerifier::NewL(CController* aController, TDesC8& aControllerBinary, CSecurityManager& aSecurityManager, CApplication* aApplication)
	{
	
	CSisCertificateVerifier* self = CSisCertificateVerifier::NewLC(aController, aControllerBinary, aSecurityManager, aApplication);
	CleanupStack::Pop(self);
	return self;
	
	}
		
CRestoreController::CSisCertificateVerifier* CRestoreController::CSisCertificateVerifier::NewLC(CController* aController, TDesC8& aControllerBinary, CSecurityManager& aSecurityManager, CApplication* aApplication)
	{
	
	CSisCertificateVerifier* self = new (ELeave) CSisCertificateVerifier(aController, aSecurityManager, aApplication,aControllerBinary);
	CleanupStack::PushL(self);
	return self;
		
	}
		
CRestoreController::CSisCertificateVerifier::CSisCertificateVerifier(CController* aController, CSecurityManager& aSecurityManager, CApplication* aApplication, TDesC8& aControllerBinary)
	: CActive(CActive::EPriorityStandard),
	  iSecurityManager(aSecurityManager),
	  iApplication(aApplication),iControllerBinary(aControllerBinary)
	{
	
	iController = aController;
	CActiveScheduler::Add(this);
	
	}
		
void CRestoreController::CSisCertificateVerifier::StartL(TRequestStatus& aStatus)
	{
	
	iWatcherStatus = &aStatus;
	aStatus = KRequestPending;
	
	TBool isEmbedded = ETrue;
	
	iSecurityManager.VerifyControllerL(
		iControllerBinary, 
		*iController,
		&iResult,
		iValidationResults, 
		iCerts,
		&iGrantableCapabilitySet,
		iAllowUnsigned,
		isEmbedded,
		iStatus);
	
	SetActive();
	
	}
		
void CRestoreController::CSisCertificateVerifier::RunL()
	{
	DEBUG_PRINTF2(_L8("Restore - Controller verification complete, verification status was %d"),
		iResult);
	
	TBool verified = EFalse;
	switch (iResult)
		{
		default:
			// BC break, unknown validation code, abort
			User::Leave(KErrNotSupported);
			break;
		
		case EValidationSucceeded:
			// Chain was validated
			// Increase the trust status of this install process			
			iController->SetTrust(ESisPackageCertificateChainValidatedToTrustAnchor);
			iController->TrustStatus().SetValidationStatus(EValidatedToAnchor);
			verified = ETrue;
			break;				
		case ESignatureNotPresent:
			// This is a special case because we need to look at the policy 
			// setting to determine if unsigned SIS files are allowed at all.
			// Display security warning dialog.
			iController->SetTrust(ESisPackageUnsignedOrSelfSigned);
			iController->TrustStatus().SetValidationStatus(EUnsigned);
 			if (iAllowUnsigned)
  				{
  				verified = ETrue;
  				}	
			break;		
		case ESignatureSelfSigned:	
			iController->SetTrust(ESisPackageCertificateChainNoTrustAnchor);
			iController->TrustStatus().SetValidationStatus(EValidated);
			verified = ETrue;
			break;					   
		case ECertificateValidationError:
		case ENoCertificate:
		case ENoCodeSigningExtension:
		case ENoSupportedPolicyExtension:
			// Unable to validate the chain
			// We apply the same policy as per unsigned SIS files
			iController->SetTrust(ESisPackageValidationFailed);	
			iController->TrustStatus().SetValidationStatus(EInvalid);
			break;
		case ESignatureCouldNotBeValidated:
		case EMandatorySignatureMissing:
			iController->TrustStatus().SetValidationStatus(EInvalid);
			break;
		}
		
	if (verified)
		{
		TRAPD(err, CheckDeviceIdConstraintsL();CheckCapabilitiesL());
		if (err)
			{
			DEBUG_PRINTF(_L8("Restore - Device ID or Capability Check Failed"));
			User::RequestComplete(iWatcherStatus, err);
			}		
		else
			{
			User::RequestComplete(iWatcherStatus, KErrNone);
			}			
		}
	else
		{
		User::RequestComplete(iWatcherStatus, KErrCorrupt);				
		}	
	}
	
		
void CRestoreController::CSisCertificateVerifier::DoCancel()
	{
	
	iSecurityManager.Cancel();
	
	}
	
CRestoreController::CSisCertificateVerifier::~CSisCertificateVerifier()
	{
	
	Cancel();
	iValidationResults.ResetAndDestroy();
	iCerts.ResetAndDestroy();
			
	}

void CRestoreController::CSisCertificateVerifier::CheckDeviceIdConstraintsL()
	{
	const CCertChainConstraints* certChainConstraints = iController->CertChainConstraints();

	const RPointerArray<HBufC>& deviceIDs=iSecurityManager.DeviceIDsInfo();
	SecurityCheckUtil::CheckDeviceIdConstraintsL(certChainConstraints, deviceIDs);
	}

void CRestoreController::CSisCertificateVerifier::CheckCapabilitiesL()
	{	
	TCapabilitySet requestedCaps;
	requestedCaps.SetEmpty();	
	const RPointerArray<CSisRegistryFileDescription>& sidFdList=iApplication->FilesToAdd();
	TInt count=sidFdList.Count();
	for (TInt i=0; i<count; i++)
		{
		const HBufC8* rawFileHeaderCaps = sidFdList[i]->CapabilitiesData();
		if	(rawFileHeaderCaps)
			{
			const TInt KCapSetSize=sizeof(TUint32); // size of a capability bit set
			const TInt KCapSetSizeBits=8*KCapSetSize;
			const TInt KNumCapSets=rawFileHeaderCaps->Size()/KCapSetSize;
			for (TInt set=0; set<KNumCapSets; set++)
				{
				TUint32 capsValue=*(reinterpret_cast<const TUint32*>(rawFileHeaderCaps->Ptr())+set);
				for (TInt capIndex=0; capIndex<KCapSetSizeBits; capIndex++)
					{
					if (capsValue & (0x1<<capIndex))
						{
						TCapability cap=static_cast<TCapability>(capIndex+set*KCapSetSizeBits);
						requestedCaps.AddCapability(cap);
						}
					}
				}		
			}
		}
		
	//Get the CertChainConstraint instance built by the SecurityManager.
	const CCertChainConstraints* certChainConstraints = iController->CertChainConstraints();

	//Get the capbibilies contrained from the CertChainConstraints
	TCapabilitySet initCapConstraints=certChainConstraints->ValidCapabilities();
	TCapabilitySet supportedCapabilitiesByBoth=initCapConstraints;

	//build the capability constraints from the constrained capabilities and root capbilities
	supportedCapabilitiesByBoth.Intersection(iGrantableCapabilitySet);

	// We have a set of granted capabilities, which is the intersection of iCapabilitySet  and 
	// Capbilities constraints(filled in the security manager).
	// Let's see if that's enough for the files to be installed.
	requestedCaps.Remove(supportedCapabilitiesByBoth);
	
	// Any capabilities left in requestedCaps after this are not signed for and not in the certs constraints. 
	// Check if any of them are system capabilities. If so, bail out.
	TCapabilitySet requiredExtraSysCaps(requestedCaps);
	SecurityCheckUtil::RemoveUserCaps(requiredExtraSysCaps, iSecurityManager);	
	if (SecurityCheckUtil::NotEmpty(requiredExtraSysCaps))
		{
		DEBUG_PRINTF(_L8("Restore - Controller requires more system capabilities than are signed for"));
		User::Leave(KErrSecurityError);
		}
	else if (SecurityCheckUtil::NotEmpty(requestedCaps)) //Required more user capabilities then
		{
		// User capabilities are supported by the CertChainConstraints, but not by root certificates.
		if (!initCapConstraints.HasCapabilities(requestedCaps))
			{
			DEBUG_PRINTF(_L8("Restore - Controller requires user capabilities that are not supported by devcert constraints"));
			// User capabilities are not all supported by the CertChainConstraints.
			User::Leave(KErrSecurityError);					
			}
		}
	}
