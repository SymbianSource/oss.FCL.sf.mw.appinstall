/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Application Processor.
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#include <hash.h>
#include "installationprocessor.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#include "swtypereginfo.h"
#include "installswtypehelper.h"
#include "cleanuputils.h"
#include "userselections.h"
#include "sissupportedlanguages.h"
#else
#include "integrityservices.h"
#endif

#include "sisregistryentry.h"
#include "sishelperclient.h"
#include "sisregistryfiledescription.h"
#include "sisregistrypackage.h"
#include "sisstring.h"
#include "hashcontainer.h"
#include "siscontroller.h"
#include "application.h"
#include "userselections.h"
#include "log.h"
#include "secutils.h"
#include "sisuihandler.h"
#include "filesisdataprovider.h"
#include "securitymanager.h"
#include "securitypolicy.h"
#include "sislauncherclient.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "plan.h"
#include "sidcache.h"
#include "sistruststatus.h"
#include "securitycheckutil.h"
#include "sisfieldtypes.h"
#include "progressbar.h"
#include "fileextractor.h"
#include "securitycheckutil.h"
#include <s32strm.h>
using namespace Swi;


_LIT(KApparcRegDir, "\\private\\10003a3f\\import\\apps\\");
_LIT(KSisExt, ".sis");


const TInt KSwiDaemonUid = 0x10202DCE;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallationProcessor* CInstallationProcessor::NewL(const CPlan& aPlan, CSecurityManager &aSecurityManager, 
	RSisHelper& aSisHelper, RUiHandler& aUiHandler, 
	Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
	const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	{
	CInstallationProcessor* self = CInstallationProcessor::NewLC(aPlan, 
		aSecurityManager, aSisHelper, aUiHandler, aStsSession, aRegistryWrapper, 
		aControllerData, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
#else
CInstallationProcessor* CInstallationProcessor::NewL(const CPlan& aPlan, CSecurityManager &aSecurityManager, 
	RSisHelper& aSisHelper, RUiHandler& aUiHandler, 
	CIntegrityServices& aIntegrityServices, const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	{
	CInstallationProcessor* self = CInstallationProcessor::NewLC(aPlan, 
		aSecurityManager, aSisHelper, aUiHandler, aIntegrityServices, aControllerData, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
#endif

CInstallationProcessor* CInstallationProcessor::NewL(CInstallationProcessor& aProcessor)
	{
	CInstallationProcessor* self = CInstallationProcessor::NewLC(aProcessor.Plan(), 
		aProcessor.iSecurityManager, aProcessor.iSisHelper, aProcessor.UiHandler(), 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		aProcessor.TransactionSession(), aProcessor.iRegistryWrapper,
#else
		aProcessor.IntegrityServices(),
#endif
		aProcessor.iControllerData, aProcessor.Observer());
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallationProcessor* CInstallationProcessor::NewLC(
	const CPlan& aPlan, CSecurityManager &aSecurityManager, 
	RSisHelper& aSisHelper, RUiHandler& aUiHandler, 
	Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
	const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	{
	CInstallationProcessor* self = new(ELeave) CInstallationProcessor(aPlan, 
		aSecurityManager, aSisHelper, aUiHandler, aStsSession, aRegistryWrapper, 
		aControllerData, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
#else
CInstallationProcessor* CInstallationProcessor::NewLC(
	const CPlan& aPlan, CSecurityManager &aSecurityManager, 
	RSisHelper& aSisHelper, RUiHandler& aUiHandler, 
	CIntegrityServices& aIntegrityServices, const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	{
	CInstallationProcessor* self = new(ELeave) CInstallationProcessor(aPlan, 
		aSecurityManager, aSisHelper, aUiHandler, aIntegrityServices, aControllerData, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
#endif

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallationProcessor::CInstallationProcessor(const CPlan& aPlan, 
	CSecurityManager &aSecurityManager, RSisHelper& aSisHelper, 
	RUiHandler& aUiHandler, Usif::RStsSession& aStsSession, 
	CRegistryWrapper& aRegistryWrapper,
	const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	: CProcessor(aPlan, aUiHandler, aStsSession, aRegistryWrapper, aObserver), 
	iSecurityManager(aSecurityManager), 
	iSisHelper(aSisHelper),
	iControllerData(aControllerData)
	{
	}
#else
CInstallationProcessor::CInstallationProcessor(const CPlan& aPlan, 
	CSecurityManager &aSecurityManager, RSisHelper& aSisHelper, 
	RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices, 
	const TDesC8& aControllerData, RSwiObserverSession& aObserver)
	: CProcessor(aPlan, aUiHandler, aIntegrityServices, aObserver), 
	iSecurityManager(aSecurityManager), 
	iSisHelper(aSisHelper),
	iControllerData(aControllerData)
	{
	}
#endif

CInstallationProcessor::~CInstallationProcessor()
	{
	Cancel();
	
	delete iEmbeddedProcessor;
	delete iFileExtractor;
		
	iFilesToCopy.ResetAndDestroy();
	iApparcRegFiles.ResetAndDestroy();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	iApparcRegFilesForParsing.ResetAndDestroy();
	iApparcRegFileData.ResetAndDestroy();
	delete iAppRegExtractor;	
#endif	
	iLoader.Close();
	iSkipFile.Close();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iSoftwareTypeRegInfoArray.Close();
#endif
	}

void CInstallationProcessor::ConstructL()
	{
	User::LeaveIfError(iLoader.Connect());
		
	CProcessor::ConstructL();
	
	iFileExtractor=CFileExtractor::NewL(Fs(), iSisHelper, UiHandler(), Plan().AppInfoL());
	}

void CInstallationProcessor::DoCancel()
	{
	CProcessor::DoCancel();
	if (iEmbeddedProcessor && iEmbeddedProcessor->IsActive())
		{
		iEmbeddedProcessor->Cancel();
		}
	
	if (iFileExtractor->IsActive())
		{
		iFileExtractor->Cancel();
		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK  	
	if (iAppRegExtractor != NULL)
        {
		if (iAppRegExtractor->IsActive())
			{
			iAppRegExtractor->Cancel();
			}
		}
#endif
	}

void CInstallationProcessor::DisplayFileL(const CSisRegistryFileDescription& aFileDescription, Sis::TSISFileOperationOptions aFileOperationOption)
	{
	// Default to continue
	TFileTextOption fileTextOption(EInstFileTextOptionContinue);
	bool forceAbortFlag = EFalse;
	if (aFileOperationOption & Sis::EInstFileTextOptionSkipIfNo)
		{
		fileTextOption=EInstFileTextOptionSkipOneIfNo;
		}
	else if (aFileOperationOption & Sis::EInstFileTextOptionAbortIfNo)
		{
		fileTextOption=EInstFileTextOptionAbortIfNo;
		}
	else if (aFileOperationOption & Sis::EInstFileTextOptionExitIfNo)
		{
		fileTextOption=EInstFileTextOptionExitIfNo;
		}
	else if (aFileOperationOption & Sis::EInstFileTextOptionForceAbort)
		{
		//converts FA option to TC option.
		fileTextOption=EInstFileTextOptionContinue;
		forceAbortFlag = ETrue;
		}
			
	TFileName temporaryFileName;
		
	TemporaryFileNameLC(aFileDescription, temporaryFileName);

	EnsureTemporaryInstallDirExistsL(temporaryFileName);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TransactionSession().RegisterTemporaryL(temporaryFileName);
#else
	IntegrityServices().TemporaryL(temporaryFileName);
#endif
		
	//	Extract this file to its temporary location.
	// However this file may have already been extracted, due to displaying it as
	// text, so we don't need to extract it again under this circumstance
	RFile temporaryFile;
	TInt err = temporaryFile.Create(Fs(), temporaryFileName, EFileWrite);
	if (err==KErrNone)
		{
		// We are passing the file handle to SISHelper but still need to close 
		// it here.
		CleanupClosePushL(temporaryFile);
		User::LeaveIfError(iSisHelper.ExtractFileL(Fs(), temporaryFile,
			aFileDescription.Index(), ApplicationL().AbsoluteDataIndex(), UiHandler()));
		CleanupStack::PopAndDestroy(&temporaryFile);
		}
	else if (err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	User::LeaveIfError(temporaryFile.Open(Fs(), temporaryFileName, EFileRead));
	CleanupClosePushL(temporaryFile);
	
	TInt fileSize=0;
	User::LeaveIfError(temporaryFile.Size( fileSize));
	HBufC8* text=HBufC8::NewMaxLC(fileSize);
	TPtr8 textPtr(text->Des());

	User::LeaveIfError(temporaryFile.Read(textPtr));	

	CDisplayText* displayText = NULL;

	displayText = CDisplayText::NewLC(Plan().AppInfoL(), fileTextOption, textPtr);
	UiHandler().ExecuteL(*displayText);

	switch (fileTextOption)
		{
		case EInstFileTextOptionContinue:
		//for FA option ,raise a TC dialog and abort the installation.
		if(forceAbortFlag)
			{
			User::Leave(KErrCancel);
			}
		break;
		
		case EInstFileTextOptionSkipOneIfNo:
			{
			iSkipFile.AppendL(displayText->ReturnResult());
			}
		break;
	
		case EInstFileTextOptionAbortIfNo:
		case EInstFileTextOptionExitIfNo:
		if (!displayText->ReturnResult())	
			{
			User::Leave(KErrCancel);
			}
		break;			
		}

	CleanupStack::PopAndDestroy(3, &temporaryFile);
	}

TBool CInstallationProcessor::ExtractFileL(CSisRegistryFileDescription& aFileToExtract)
	{
	DEBUG_PRINTF2(_L("Install Server - Installation Processor Extracting File '%S'"),
		&aFileToExtract.Target());
	
	if (ApplicationL().IsPreInstalledApp() || ApplicationL().IsPreInstalledPatch()
			|| aFileToExtract.Operation() == Sis::EOpNull)
		{
		// This file will not really be copied, but still need to update the progress bar.
		// Increment for extract stage (scaled by file size)
		TInt ammount = ProgressBarFileIncrement(aFileToExtract.UncompressedLength());
		// Increment for install/copy stage
		ammount += KProgressBarEndIncrement;
		UiHandler().UpdateProgressBarL(Plan().AppInfoL(), ammount);
		// move onto next file
		return EFalse;	
		}

	TFileName temporaryFileName;
		
	TemporaryFileNameLC(aFileToExtract, temporaryFileName);

	// Add to the list of files to copy
	CFileCopyDescription* fileCopyDescription=CFileCopyDescription::NewLC(
		temporaryFileName, aFileToExtract);
	User::LeaveIfError(iFilesToCopy.Append(fileCopyDescription));
	CleanupStack::Pop(fileCopyDescription);	// Ownership is transferred

	EnsureTemporaryInstallDirExistsL(temporaryFileName);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TransactionSession().RegisterTemporaryL(temporaryFileName);
#else
	IntegrityServices().TemporaryL(temporaryFileName);
#endif

	iFileExtractor->ExtractFileL(ApplicationL().AbsoluteDataIndex(), aFileToExtract, temporaryFileName, iStatus);
	return ETrue;
	}


void CInstallationProcessor::DoExtractHashL(const CSisRegistryFileDescription& aFileToProcess)
	{
	TParsePtrC targetPath(aFileToProcess.Target());
	// If the target is in sys\bin, write its hash into \sys\hash and name
	// it after the file.  The check in the process file state will ensure
	// that it must be a valid exe or dll to get this far.
	if (targetPath.Path().CompareF(KBinPath) == 0)
		{
		ExtractHashL(aFileToProcess.Target(), aFileToProcess.Hash());
		}	
	}

void CInstallationProcessor::CheckHashL(const CSisRegistryFileDescription& aFileToProcess, const TDesC& aCurrentFileName)
	{
	const CHashContainer& hash = aFileToProcess.Hash();
	
	// Calculate and check correct hash value
	CFileSisDataProvider* fileDataProvider=CFileSisDataProvider::NewLC(Fs(), aCurrentFileName);
	TBool hashIsValid=iSecurityManager.VerifyFileHashL(*fileDataProvider,hash);
	CleanupStack::PopAndDestroy(fileDataProvider);
	if (!hashIsValid)
		{
		User::Leave(KErrCorrupt);
		}
	}

	

void CInstallationProcessor::EnsureTemporaryInstallDirExistsL(const TDesC& aFileTarget)
	{
	TInt err = Fs().MkDirAll(aFileTarget);
	if (err!= KErrNone && err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	}

void CInstallationProcessor::TemporaryFileNameLC(const CSisRegistryFileDescription& aFileToExtract, TDes& aTemporaryFileName)
	{
	static TInt localTempCnt = 0;
	TChar drive;
	if(aFileToExtract.Target().Length())
		{
		// set temporary drive to be same as final target
		drive = aFileToExtract.Target()[0];
		}
	else
		{
		// set temporary drive to the user selected drive
		drive = ApplicationL().UserSelections().Drive();
		}
	 		
 	// If no drive has been selected then use the C drive for
 	// temporary files. This should only happen if the SIS file
 	// just contains text files to display but not install.
 	if (drive == TChar(KNoDriveSelected))
 		{
 		drive = iSystemDriveChar;
		}
	
	// construct the temporary filename
	// Format is  {Drive}:\sys\install\temp\file-{Data Unit}-{File Index}-{LocalCounter}
	_LIT(KTemporaryFileFormat, "%c:%Sfile-%d-%d-%d");
	TUint driveCh(drive); // Can't pass TChar to Format.
	
	aTemporaryFileName.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath, 
							 ApplicationL().AbsoluteDataIndex(), aFileToExtract.Index(),
							 localTempCnt++);
	}

///\short Extracts executable hash from controller to file for the loader
void CInstallationProcessor::ExtractHashL(const TFileName& aFileName, 
	const CHashContainer& aHash)
	{	
	const TDesC8& hashData = aHash.Data();
		
	TBuf<32> hashPath;	
	TUint driveCh(iSystemDriveChar); // can't pass TChar to Format
	hashPath.Format(KHashPathFormat, driveCh, &KHashPath);	
	
	TParse hashFileName;
	hashFileName.Set(hashPath, &aFileName, NULL);
	TInt err = Fs().MkDirAll(hashFileName.DriveAndPath());
	if (err!=KErrNone && err!=KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	// If the hash already exists, leave with KErrAlreadyExists
	TEntry hashEntry;
	if ( KErrNone == Fs().Entry(hashFileName.FullName(), hashEntry))
		{
		User::Leave( KErrAlreadyExists );
		}

	DEBUG_PRINTF2(_L("Install Server - Installation Processor, extracting hash file '%S'"), 
		&(hashFileName.FullName()));

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TransactionSession().RegisterNewL(hashFileName.FullName());
#else
	IntegrityServices().AddL(hashFileName.FullName());
#endif

	RFile file;
	User::LeaveIfError(file.Create(Fs(), hashFileName.FullName(), 
		EFileWrite|EFileShareExclusive|EFileStream));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(hashData));
	CleanupStack::PopAndDestroy(&file);
	//Write a SWI event log for this hash file
	TUint8 fileFlag(EFileAdded);
	CObservationData *event = CObservationData::NewLC(hashFileName.FullName(),TUid::Null(),fileFlag);
	Observer().AddEventL(*event);
	CleanupStack::PopAndDestroy(event);
	}

void CInstallationProcessor::VerifyInstallPathL(const CSisRegistryFileDescription& aFileDescription)
	{
	// Check the target path of every file is legal
	SecurityCheckUtil::TProtectedDirectoryCheckError targetError;
	// sis file signed by Su Cert are allowed to install files in private dir 
	// without corresponding executable in the package.
	if (ApplicationL().IsInstallSuCertBased())
		{
		return;
		}

	TBool pathOk = SecurityCheckUtil::CheckProtectedDirectoriesL(
		aFileDescription.Target(), aFileDescription.Operation(), iSidsAdded, targetError);
	if (!pathOk)
		{
		/// Initialized to  EUiSIDMismatch to make RVCT Compiler happy / supress the warnings during ARM build.
		TErrorDialog uiError = EUiSIDMismatch;
		switch(targetError)
			{
			case SecurityCheckUtil::ESIDMismatch:
			uiError = EUiSIDMismatch;
			break;
			
			case SecurityCheckUtil::EInvalidFileName:
			uiError = EUiInvalidFileName;
			break;
			
			default:
			User::Leave(KErrNotSupported);
			}
		CDisplayError* cmd=CDisplayError::NewLC(Plan().AppInfoL(),uiError,KNullDesC);
		UiHandler().ExecuteL(*cmd);
		CleanupStack::PopAndDestroy(cmd);
		User::Leave(KErrAccessDenied);			
		}		
	}

void CInstallationProcessor::InstallFileL(const CFileCopyDescription& aFileCopyDescription)
	{
	DEBUG_PRINTF2(_L("Install Server - Installation Processor, installing file to '%S'"),
		&aFileCopyDescription.FileDescription().Target());
	
	// move file to it's final location
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TransactionSession().RegisterNewL(aFileCopyDescription.FileDescription().Target());
#else
	IntegrityServices().AddL(aFileCopyDescription.FileDescription().Target());
#endif
	
	// Move try 1 of 3
	TInt err =Fs().Rename(aFileCopyDescription.TemporaryFileName(), aFileCopyDescription.FileDescription().Target());
	if(err != KErrNone)
		{
		// Move failed - maybe dir did not exist
		(void) Fs().MkDirAll(aFileCopyDescription.FileDescription().Target());
		// Move try 2 of 3
		err =Fs().Rename(aFileCopyDescription.TemporaryFileName(), aFileCopyDescription.FileDescription().Target());
		if(err != KErrNone)
			{
			// Maybe destination already exists?
			// Is this possible? Or would install have deleted file earlier!?!?

			// Clear Read only attributes and delete the file
			Fs().SetAtt(aFileCopyDescription.FileDescription().Target(), 0, KEntryAttReadOnly);
			
			iLoader.Delete(aFileCopyDescription.FileDescription().Target()); // ignore failure here since the rename will fail instead
			
			// Move try 3 of 3
			err =Fs().Rename(aFileCopyDescription.TemporaryFileName(), aFileCopyDescription.FileDescription().Target());
			}
		}

	User::LeaveIfError(err); // Did we manage to do the move?

	// Update progress bar for the copy/install of this file
	UiHandler().UpdateProgressBarL(Plan().AppInfoL(), KProgressBarEndIncrement);
	
	AddApparcFilesInListL(aFileCopyDescription.FileDescription().Target());

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Parse the file if it carries software type registration info
	if (InstallSoftwareTypeHelper::IsValidSwRegFileL(aFileCopyDescription.FileDescription().Target(),
													 ApplicationL().ControllerL().Info().Uid().Uid().iUid))
		{
		InstallSoftwareTypeHelper::ParseRegFileL(Fs(),
												 aFileCopyDescription.FileDescription().Target(),
												 iSoftwareTypeRegInfoArray);
		}
#endif

	// Launch the file if RI flag is set.
	if(ShouldLaunchL(aFileCopyDescription.FileDescription()))
		{
		LaunchFileL(aFileCopyDescription.FileDescription());
		}
	
	}

// State processing functions

TBool CInstallationProcessor::DoStateInitializeL()
	{
	iUiState = EInitialize;
	iCurrent = 0;
	iFilesToCopyCurrent = 0;
	iFilesToCopy.ResetAndDestroy();
	iApparcRegFiles.ResetAndDestroy();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iApparcRegFilesForParsing.ResetAndDestroy();
	iApparcRegFileData.ResetAndDestroy();
#endif
	return ETrue;
	}
	
TBool CInstallationProcessor::DoStateProcessEmbeddedL()
	{
	if (iCurrent < ApplicationL().EmbeddedApplications().Count())
		{
		EmbeddedProcessorL().ProcessApplicationL(*ApplicationL().EmbeddedApplications()[iCurrent++], iStatus);
		WaitState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
TBool CInstallationProcessor::DoStateProcessSkipFilesL()
	{
	TInt count = ApplicationL().FilesToSkipOnInstall().Count();
	// create a modifyable reference
	CApplication& app = const_cast <CApplication&>(ApplicationL());
	CPlan& plan = const_cast <CPlan&>(Plan());

	for (TInt i = 0; i < count; ++i)		
		{
		if (iSkipFile[i])
			{
			CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToSkipOnInstall()[i];
			app.AddFileL(fileDescription);
			plan.AddInstallFileForProgress(fileDescription.UncompressedLength());
			}
		}
	iCurrent = 0;
	return ETrue;	
	}
	
TBool CInstallationProcessor::DoStateExtractFilesL()
	{
	if (iCurrent < ApplicationL().FilesToAdd().Count())
		{
		if (iUiState != EExtractFiles)
			{
			iUiState = EExtractFiles;
			// Signal to UI we are extracting files
			CHandleCancellableInstallEvent* cmd = CHandleCancellableInstallEvent::NewLC(Plan().AppInfoL(), EEventCopyingFiles, 0, KNullDesC);
			UiHandler().ExecuteL(*cmd);
			CleanupStack::PopAndDestroy(cmd);
			}
			
		CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToAdd()[iCurrent++];
		
		if(ExtractFileL(fileDescription))
			{
			WaitState(ECurrentState);
			}
		else
			{
			SwitchState(ECurrentState);
			}
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}

bool CInstallationProcessor::FileIsApparcReg(const TDesC& aFilename) const
	{
	TParsePtrC filename(aFilename);

	return filename.Path().CompareF(KApparcRegDir) == 0;
	}

TBool CInstallationProcessor::DoStateProcessFilesL()
	{
	DEBUG_PRINTF(_L8("Install Server - Processing Files"));
	if (iCurrent < ApplicationL().FilesToAdd().Count() )
		{
		CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToAdd()[iCurrent];
		
		if(fileDescription.Operation() != Sis::EOpNull)
			{
			if (ApplicationL().IsPreInstalledApp() || ApplicationL().IsPreInstalledPatch())
				{
				// For pre-installed SISes, filenames could be "e:\foo.txt", but we
				// want them to install successfully on "f:". So re-write stub file
				// references to be the same drive as where the SIS file is located
				TChar drive = ApplicationL().UserSelections().Drive();
				TFileName target = fileDescription.Target();
				target[0] = drive;
				
				// use filenames from the file descriptions
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				TInt error = SecurityCheckUtil::ProcessFileL(ApplicationL(), Fs(), iSidsAdded,
					TransactionSession(), fileDescription, target);
#else
				TInt error = SecurityCheckUtil::ProcessFileL(ApplicationL(), Fs(), iSidsAdded,
					IntegrityServices(), fileDescription, target);
#endif
				if (error != KErrNone)
					{
					ReportErrorL(TErrorDialog(error));
					}
				
				// This is either preinstalled or is a propagation and as of Pdef115573 only files under /sys, /resource or 
				// which have the VERIFY tag set need to have their hashes checked.
				if ((fileDescription.OperationOptions() & Swi::Sis::EInstVerifyOnRestore) || SecurityCheckUtil::IsTargetTcbWriteProtected(fileDescription.Target()))
					{
					CheckHashL(fileDescription, target);
					}
				}
			else
				{
				// use temporary file names
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				TInt error = SecurityCheckUtil::ProcessFileL(ApplicationL(), Fs(),
					iSidsAdded, TransactionSession(), fileDescription,
					iFilesToCopy[iFilesToCopyCurrent]->TemporaryFileName());
#else
				TInt error = SecurityCheckUtil::ProcessFileL(ApplicationL(), Fs(),
					iSidsAdded, IntegrityServices(), fileDescription,
					iFilesToCopy[iFilesToCopyCurrent]->TemporaryFileName());
#endif
				if (error != KErrNone)
					{
					ReportErrorL(TErrorDialog(error));
					}	
				CheckHashL(fileDescription, iFilesToCopy[iFilesToCopyCurrent]->TemporaryFileName());
				iFilesToCopyCurrent++;
				}
			}

		++iCurrent;
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		
		// Finally, if this is an addition to an existing package, add SIDs from that
		// package to the list of SIDs attached to this package
		
		if (ApplicationL().IsPartialUpgrade() ||
			ApplicationL().IsAugmentation()	||
			ApplicationL().IsPreInstalledPatch())
			{
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			RSisRegistrySession session;
			User::LeaveIfError(session.Connect());
			CleanupClosePushL(session);
#endif
			
			RSisRegistryEntry entry;
			// Planning stage already established the entry exists
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			TInt err = entry.Open(iRegistryWrapper.RegistrySession(), ApplicationL().ControllerL().Info().Uid().Uid());
#else
			TInt err = entry.Open(session, ApplicationL().ControllerL().Info().Uid().Uid());
#endif
			if (err == KErrNotFound)
			{
			  User::Leave(KErrMissingBasePackage);	
			}
			
			User::LeaveIfError(err);	
		  
			CleanupClosePushL(entry);
			
			RArray<TUid> preinstalledSids;
			CleanupClosePushL(preinstalledSids);
			entry.SidsL(preinstalledSids);
			
			TInt sids(preinstalledSids.Count());
			for (TInt i = 0; i < sids; ++i)
				{
				DEBUG_PRINTF2(_L("Install Server - Processing Files - Appending SID %08x"), preinstalledSids[i]);
				iSidsAdded.Append(preinstalledSids[i]);
				}
			
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			CleanupStack::PopAndDestroy(2, &entry); // presintalledSids
#else
			CleanupStack::PopAndDestroy(3, &session); // entry, presintalledSids
#endif			
			}
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		
		// Find out all the regisration resource files associated with this package UID and add to the list of 
		// files to be processed later for parsing		
		TRAPD(err, AddAppArcRegResourceFilesL());
		if ( err != KErrNotFound && err != KErrNone)
		    User::Leave(err);
#endif
		iCurrent = 0;
		return ETrue;
		}
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CInstallationProcessor::AddAppArcRegResourceFilesL()
    {
    RSisRegistryEntry entry;
    TInt err = KErrNone;
    if ( !ApplicationL().IsUninstall() )
        err = entry.Open(iRegistryWrapper.RegistrySession(), ApplicationL().ControllerL().Info().Uid().Uid());
    else
        return;
    
    User::LeaveIfError(err);
    CleanupClosePushL(entry);
    AddAppArcRegResourceFilesForRegEntryL(entry);
    
    RPointerArray<CSisRegistryPackage> augmentationArray;
    CleanupResetAndDestroyPushL(augmentationArray);
    entry.AugmentationsL(augmentationArray);
    CleanupStack::Pop(&augmentationArray);
    CleanupStack::PopAndDestroy(&entry);    
        
    CleanupResetAndDestroyPushL(augmentationArray);
    TInt count = augmentationArray.Count();
    for ( TInt i=0; i < count; ++i)
        {
        TInt err = entry.OpenL(iRegistryWrapper.RegistrySession(),*augmentationArray[i]);
        User::LeaveIfError(err);
        CleanupClosePushL(entry);
        AddAppArcRegResourceFilesForRegEntryL(entry);
        CleanupStack::PopAndDestroy(&entry);
        }
    
    CleanupStack::PopAndDestroy(&augmentationArray);
    }

void CInstallationProcessor::AddAppArcRegResourceFilesForRegEntryL(RSisRegistryEntry& aEntry)
    {
    RPointerArray<HBufC> filesArray;
    CleanupResetAndDestroyPushL(filesArray);
    aEntry.FilesL(filesArray);    
    TInt count = filesArray.Count();
    CSisRegistryPackage *regPkg = aEntry.PackageL();
    CleanupStack::PushL(regPkg);
    
    for (TInt i=0; i<count; ++i)
        {
        if (FileIsApparcReg(*filesArray[i]))
            {
            CAppRegFileData *tmpAppRegFileData = CAppRegFileData::NewLC(*filesArray[i],*regPkg);
            iApparcRegFilesForParsing.AppendL(tmpAppRegFileData);
            CleanupStack::Pop(tmpAppRegFileData);
            }
        }
    CleanupStack::PopAndDestroy(2,&filesArray);
    }
#endif

TBool CInstallationProcessor::DoStateVerifyPathsL()
	{
	if (iCurrent < ApplicationL().FilesToAdd().Count())
		{
		CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToAdd()[iCurrent++];
		VerifyInstallPathL(fileDescription);	
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
TBool CInstallationProcessor::DoStateInstallFilesL()
	{
	if (ApplicationL().IsPreInstalledApp() || ApplicationL().IsPreInstalledPatch())
		{
		// Only need to install hash for files in pre-installed application.
		if (iCurrent < ApplicationL().FilesToAdd().Count())
			{
			const CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToAdd()[iCurrent++];
			
			if(fileDescription.Operation() != Sis::EOpNull)
				{
				DoExtractHashL(fileDescription);
				
				// Add apparc registerd files in list.
				AddApparcFilesInListL(fileDescription.Target());
				
				// Launch the file if RI flag is set.
				if(ShouldLaunchL(fileDescription))
					{
					LaunchFileL(fileDescription);
					}
					
				AddEventToLogL(fileDescription);
				}
				
			SwitchState(ECurrentState);
			return EFalse;
			}
		}
	else if (iCurrent < iFilesToCopy.Count())
		{
		const CFileCopyDescription& fileCopyDescription = *iFilesToCopy[iCurrent++];
		DoExtractHashL(fileCopyDescription.FileDescription());
		InstallFileL(fileCopyDescription);
		AddEventToLogL(fileCopyDescription.FileDescription());
		SwitchState(ECurrentState);
		return EFalse;
		}
	iCurrent = 0;
	return ETrue;
	}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TBool CInstallationProcessor::DoParseApplicationRegistrationFilesL()
    {
    if (iCurrent == 0)
        DEBUG_PRINTF2(_L("Install Server - DoParseApplicationRegistrationFilesL - Number of Application registration resource files to be parsed %d"), iApparcRegFilesForParsing.Count());
    
	if (iAppRegExtractor != NULL)
        {
        if (iAppRegExtractor->GetErrorCode() == KErrCorrupt)
            {
            delete iApparcRegFilesForParsing[--iCurrent];
            iApparcRegFilesForParsing.Remove(iCurrent);
            }
        }
    
    if (iCurrent < iApparcRegFilesForParsing.Count())
        { 
        TDesC& fileDescription = (iApparcRegFilesForParsing[iCurrent++]->GetAppRegFile());   
        // Continue processing the next file if a registration resource file is not found(in case of SA over SA)
        TRAPD(err,ParseRegistrationResourceFileL(fileDescription));        
        if(KErrNotFound == err || KErrPathNotFound == err)
            {               
            delete iApparcRegFilesForParsing[--iCurrent];
            iApparcRegFilesForParsing.Remove(iCurrent); 
            TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrNone);     
            WaitState(ECurrentState);
            }
        else if(KErrNone != err )
            {
            User::Leave(err);
            }
        else
            {
            WaitState(ECurrentState);
            }
        return EFalse;
        }
    else
        {
        iCurrent = 0;
        return ETrue;
        }
    }

TBool CInstallationProcessor::ParseRegistrationResourceFileL(const TDesC& aTargetFileName)
    {
    DEBUG_PRINTF2(_L("Install Server - ParseRegistrationResourceFileL - Parsing '%S' registration resource file"), &aTargetFileName);
    if ( NULL == iAppRegExtractor )
        {
        DEBUG_PRINTF(_L("Install Server - ParseRegistrationResourceFileL - Creating CAppRegExtractor for async parsing of registration resource file"));
        CApplication& app = const_cast<CApplication&>(ApplicationL());        
        iAppRegExtractor = CAppRegExtractor::NewL(Fs(), iApparcRegFileData);       
        }
    iAppRegExtractor->ExtractAppRegInfoSizeL(aTargetFileName, iStatus);
    return EFalse;
    }
#endif

TBool CInstallationProcessor::DoStateDisplayFilesL()
	{
	if (ApplicationL().IsPreInstalledApp() || ApplicationL().IsPreInstalledPatch())
		{
		return ETrue;
		}
	if (iCurrent < ApplicationL().FilesToDisplayOnInstall().Count())
		{		
		const CSisRegistryFileDescription& fileDescription = *ApplicationL().FilesToDisplayOnInstall()[iCurrent++];		
		DisplayFileL(fileDescription, fileDescription.OperationOptions());
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}		
	}

TBool CInstallationProcessor::DoStateUpdateRegistryL()
	{
	// destroy the memory heavy file copy descriptions
	iFilesToCopy.ResetAndDestroy();

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	RArray<TAppUpdateInfo> affectedApps;    
	CleanupClosePushL(affectedApps);
	const CApplication& application = ApplicationL();
	//if there are reg files in the package or if its an upgrade (in case of SA (with app) over SA(with no app))
	if(iApparcRegFilesForParsing.Count() != 0 || application.IsUpgrade())
	    {
	    //Create the list of Application Uids which are affected by the Installation    	    	    
	    Usif::TComponentId componentId = 0;
	    RArray<Usif::TComponentId> componentIds;
	    CleanupClosePushL(componentIds);
	    RArray<TUid> existingAppUids;
	    CleanupClosePushL(existingAppUids);
	    TAppUpdateInfo existingAppInfo;     
	    TUid packageUid = application.ControllerL().Info().Uid().Uid();
	    
        if(application.IsUpgrade()) 
            {	 	    
            //Get all componentIds
            iRegistryWrapper.RegistrySession().GetComponentIdsForUidL(packageUid, componentIds);            
            TInt count = componentIds.Count();
            if(0 == count)
                {
                DEBUG_PRINTF(_L("ComponentIDs not found for the base package"));
                User::Leave(KErrNotFound);
                }
                                
            //SA over SA
            if(application.ControllerL().Info().InstallType() == Sis::EInstInstallation )
                {        
                //Get the compid for base package
                componentId = iRegistryWrapper.RegistrySession().GetComponentIdForUidL(packageUid);               
                      
                TInt index = componentIds.Find(componentId);

                //Exclude the Base SA compId from the list 
                componentIds.Remove(index);
                //Get the apps for Base SA compId and mark them as to be deleted
                existingAppUids.Reset();
                TRAPD(err,iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentId, existingAppUids);)  
                //If Base Package does not contain any app then GetAppUidsForComponentL will return KErrNotFound, ignore the error else leave
                if (KErrNone != err && KErrNotFound != err)
                    {
                    User::Leave(err);
                    }
                
                for(TInt i = 0 ; i < existingAppUids.Count(); ++i)
                    {
                    existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppUninstalled);    
                    affectedApps.AppendL(existingAppInfo);
                    }                
                //Get the apps for Remaining CompIds and mark them as to be upgraded               
                for(TInt i = 0 ; i < componentIds.Count(); ++i)
                    {
                    existingAppUids.Reset();                    
                    //If there are no apps within the existing components (SP's) then it will return KErrNotFound
                    TRAPD(err, iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentIds[i], existingAppUids);) 
                    if (KErrNone != err && KErrNotFound != err)
                        {
                        User::Leave(err);
                        }
                    
                    for(TInt k = 0 ; k < existingAppUids.Count(); ++k)
                        {
                        existingAppInfo = TAppUpdateInfo(existingAppUids[k], EAppInstalled);    
                        affectedApps.AppendL(existingAppInfo);
                        }
                    }                                
                }
                
            //SP over SP
            if(application.ControllerL().Info().InstallType() == Sis::EInstAugmentation)
                {
                componentId = iRegistryWrapper.RegistrySession().GetComponentIdForPackageL(application.PackageL().Name(), application.PackageL().Vendor());     
                //Get the apps for Base SP compId and mark them as to be deleted
                existingAppUids.Reset();
                //If there are no apps within the existing component (SP) then it will return KErrNotFound
                TRAPD(err, iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentId, existingAppUids);) 
                if (KErrNone != err && KErrNotFound != err)
					{
                    User::Leave(err);
					}
                
                for(TInt i = 0 ; i < existingAppUids.Count(); ++i)
                   {
                   existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppUninstalled);    
                   affectedApps.Append(existingAppInfo);
                   }                                  
                }        
            }	                   
        CleanupStack::PopAndDestroy(2, &componentIds);
	    }		
#endif
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Now that we are ready to make changes to the registry we start a transaction
	// Note that the commit/rollback action is subsequently taken by the later steps of the state machine
	iRegistryWrapper.StartMutableOperationsL();
#else
	RSisRegistryWritableSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif
	
	TInt64 offset = 0;
	if (!ApplicationL().IsUninstall())
		{
		
		const Swi::Sis::CController& controller = ApplicationL().ControllerL();
	
		// If this is the top level controller, strip the type field from it
		// all controllers must be registered as if they were contained in a SIS
		// array
	
		offset = controller.DataOffset();
		if (offset == 0)
			{
			// top level controller
			offset = 4;
			TFileName fileName;
			// Check whether we should create a SIS stub
			if(IsApplicationPermittedInStub(ApplicationL())
				&& !iSisHelper.IsStubL()
				&& iSecurityManager.SecurityPolicy().AllowPackagePropagate() &&  IsStubSisFileRequiredL())  
				{
				TChar drive = ApplicationL().StubDrive();
				if(CheckEmbeddedAppsInstalledOnSameDrive(ApplicationL().EmbeddedApplications(), drive))
					{
					// Create a stub SIS file for the auto-propagation feature.
					CreateStubSisFileL(fileName);
					// Register the stub SIS file at SWI registry, so that it will be removed on unistallation.
					RegisterStubSisFileL(fileName);
					}
				}			
			
			// Check whether we are installing from an existing SIS stub
			if(iSisHelper.IsStubL())
				{
				// Check whether the controller was a preinstalled type
				// if so then it's a preinstalled stub, not a propagated app stub
				Sis::TInstallType installType = ApplicationL().ControllerL().Info().InstallType();
				if(installType != Sis::EInstPreInstalledPatch && installType != Sis::EInstPreInstalledApp)
					{
					// If we are installing from a removable media stub
					// we need to add the stub to the list of files to
					// remove during uninstall
					// we don't need to create it because we are 
					// already installing from a SIS stub
					iSisHelper.GetSisFileNameL(fileName);
					RegisterStubSisFileL(fileName);
					}
				else
					{
					// If this preinstalled package is deletable and the policy
					// allows deletion of pre-installed files, add the stub sis
					// file to the files to remove on uninstall.
					CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
					if (securityPolicy->DeletePreinstalledFilesOnUninstall()
						&& ApplicationL().IsDeletablePreinstalled())
						{
						// Get filename of stub sis file
						TFileName stubFile;
						iSisHelper.GetSisFileNameL(stubFile);
						RegisterStubSisFileL(stubFile);
						}
					}
				}
			}
		}
	
	TPtrC8 thisController(iControllerData.Mid(offset));
	
	if (ApplicationL().IsUpgrade() 
		 || ApplicationL().IsPartialUpgrade())
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistryWrapper.RegistrySession().UpdateEntryL(ApplicationL(), thisController, iSoftwareTypeRegInfoArray, TransactionSession().TransactionIdL());
			}
		else
			{
			iRegistryWrapper.RegistrySession().UpdateEntryL(ApplicationL(), thisController, TransactionSession().TransactionIdL());
			}
		TInt count = iApparcRegFileData.Count();
        for (int i = 0; i < count; i++)
            {
		    iRegistryWrapper.RegistrySession().UpdateEntryL(ApplicationL(), *iApparcRegFileData[i], iApparcRegFilesForParsing[i]->GetSisRegistryPackage());
		    }
#else
		session.UpdateEntryL(ApplicationL(), thisController, IntegrityServices().TransactionId());
#endif
		}
	else if (ApplicationL().IsInstall()
				|| ApplicationL().IsAugmentation() 
				|| ApplicationL().IsPreInstalledApp()
				|| ApplicationL().IsPreInstalledPatch())
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistryWrapper.RegistrySession().AddEntryL(ApplicationL(), thisController, iSoftwareTypeRegInfoArray, TransactionSession().TransactionIdL());
			}
		else
			{
			iRegistryWrapper.RegistrySession().AddEntryL(ApplicationL(), thisController, TransactionSession().TransactionIdL());
			}
		TInt count = iApparcRegFileData.Count();
		for (int i = 0; i < count; i++)
		    {
                iRegistryWrapper.RegistrySession().AddEntryL(*iApparcRegFileData[i], iApparcRegFilesForParsing[i]->GetSisRegistryPackage());
		    }
#else
		session.AddEntryL(ApplicationL(), thisController, IntegrityServices().TransactionId());
#endif
		}
	else if (ApplicationL().IsUninstall())
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iRegistryWrapper.RegistrySession().DeleteEntryL(ApplicationL().PackageL() , TransactionSession().TransactionIdL()); 
#else
		session.DeleteEntryL(ApplicationL().PackageL() , IntegrityServices().TransactionId());
#endif
		}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CleanupStack::PopAndDestroy(&session);
#else
	// Registration of MIME types of the software types being installed to AppArc.
	// This operation is not transactional so it must be done after the software types
	// have been successfully registered to the SCR by the following calls:
	// RSisRegistryWritableSession::AddEntryL()
	// RSisRegistryWritableSession::UpdateEntryL()
	InstallSoftwareTypeHelper::RegisterMimeTypesL(iSoftwareTypeRegInfoArray);
	
	//if there are reg files in the package or if its an upgrade (in case of SA (with app) over SA(with no app))
	if(iApparcRegFilesForParsing.Count() != 0 || application.IsUpgrade())
	    {
	    //Create the list of Application Uids which are affected by the Installation                           
        RArray<Usif::TComponentId> componentIds;
        CleanupClosePushL(componentIds);
        RArray<TUid> newAppUids;    
        CleanupClosePushL(newAppUids);
        TAppUpdateInfo existingAppInfo, newAppInfo;     
        TUid packageUid = application.ControllerL().Info().Uid().Uid();
        //Get all componentIds for the application
        componentIds.Reset();
        iRegistryWrapper.RegistrySession().GetComponentIdsForUidL(packageUid, componentIds);
        TInt count = componentIds.Count();
        
        //Get the apps for All CompIds               
        for(TInt i = 0 ; i < count; i++)
            {
            newAppUids.Reset();                    
            TRAPD(err,iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentIds[i], newAppUids))
            if (KErrNone != err && KErrNotFound != err)
                {
                User::Leave(err);
                }
            
            for(TInt i = 0 ; i < newAppUids.Count(); ++i)
                {
                existingAppInfo = TAppUpdateInfo(newAppUids[i], EAppUninstalled);
                TInt index = 0;
                index = affectedApps.Find(existingAppInfo);
                if(KErrNotFound != index)
                    {
                    affectedApps.Remove(index);
                    }
                existingAppInfo = TAppUpdateInfo(newAppUids[i],EAppInstalled);
                affectedApps.AppendL(existingAppInfo);            
                }        
            }
        
        for(TInt i = 0; i < affectedApps.Count(); i++)
           {
           DEBUG_PRINTF2(_L("AppUid is 0x%x"), affectedApps[i].iAppUid);
           DEBUG_PRINTF2(_L("Action is %d"), affectedApps[i].iAction);
           }
        
        //Updating apps to be notified
        RArray<TAppUpdateInfo> currentNotifiableApps;
        CleanupClosePushL(currentNotifiableApps);
        const_cast<CPlan&>(Plan()).GetAffectedApps(currentNotifiableApps);
        TInt appCount = affectedApps.Count();
        for(TInt k = 0; k < appCount ; ++k)
            {
            TInt count = currentNotifiableApps.Count();
            TUid appUid = affectedApps[k].iAppUid;
            //compare the apps present in the package currently being processed with the existing set of affected apps,
            //if alredy exists then update else add it to the list
            for(TInt index = 0; index < count ; ++index)
               {
               if(appUid == currentNotifiableApps[index].iAppUid)
                   {           
                   currentNotifiableApps.Remove(index);   
                   break;
                   }
               }
            currentNotifiableApps.AppendL(affectedApps[k]);
            }
        
        const_cast<CPlan&>(Plan()).ResetAffectedApps();
        const_cast<CPlan&>(Plan()).SetAffectedApps(currentNotifiableApps);
        
        CleanupStack::PopAndDestroy(3, &componentIds);
	    }
	CleanupStack::PopAndDestroy(&affectedApps);
#endif
	return ETrue;
	}

// CFileCopyDescription 

/*static*/ CInstallationProcessor::CFileCopyDescription* CInstallationProcessor::CFileCopyDescription::NewL(const TDesC& aTemporaryFileName, const CSisRegistryFileDescription& aFileDescription)
	{
	CFileCopyDescription* self=CFileCopyDescription::NewLC(aTemporaryFileName, aFileDescription);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CInstallationProcessor::CFileCopyDescription* CInstallationProcessor::CFileCopyDescription::NewLC(const TDesC& aTemporaryFileName, const CSisRegistryFileDescription& aFileDescription)
	{
	CFileCopyDescription* self=new (ELeave) CFileCopyDescription(aFileDescription);
	CleanupStack::PushL(self);
	self->ConstructL(aTemporaryFileName);
	return self;
	}

CInstallationProcessor::CFileCopyDescription::CFileCopyDescription(const CSisRegistryFileDescription& aFileDescription)
	:iFileDescription(aFileDescription)
	{
	}

void CInstallationProcessor::CFileCopyDescription::ConstructL(const TDesC& aTemporaryFileName)
	{
	iTemporaryFileName=aTemporaryFileName.AllocL();
	}

CInstallationProcessor::CFileCopyDescription::~CFileCopyDescription()
	{
	delete iTemporaryFileName;	
	}

CInstallationProcessor& CInstallationProcessor::EmbeddedProcessorL()
	{
	if (!iEmbeddedProcessor)
		{
		iEmbeddedProcessor=CInstallationProcessor::NewL(*this);
		}
	return *iEmbeddedProcessor;
	}
	
void CInstallationProcessor::ReportErrorL(TErrorDialog aError)
	{
	CDisplayError* cmd=CDisplayError::NewLC(Plan().AppInfoL(),aError,KNullDesC);
	UiHandler().ExecuteL(*cmd);
	CleanupStack::PopAndDestroy(cmd);
	User::Leave(KErrSecurityError);	
	}

TBool CInstallationProcessor::CheckEmbeddedAppsInstalledOnSameDrive(RPointerArray<CApplication> aArray, TChar aDrive)
	{
	for(TInt i = 0; i < aArray.Count(); i++)
		{
		CApplication& application = *aArray[i];
		
		if(!IsApplicationPermittedInStub(application))
			{
			return EFalse;
			}	
		else if(application.StubDrive() != aDrive)
			{
			// Embedded Application must be installed on the same drive
			// as the parent application otherwise the removable media
			// stub will reference files outside the media card
			return EFalse;
			}
		else if(!CheckEmbeddedAppsInstalledOnSameDrive(application.EmbeddedApplications(), aDrive))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool CInstallationProcessor::IsApplicationPermittedInStub(const CApplication& aApplication)
	{
	if(	aApplication.IsPreInstalledApp() || 
		aApplication.IsPreInstalledPatch() ||
		aApplication.IsUninstall() ||
		aApplication.IsPartialUpgrade() ||
		!aApplication.CanPropagate())
		{
		// These installation types are not permitted in
		// a removable media stub
		// We cannot be certain that all required files will be present
		// on the media card when it is inserted into another device
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}


void CInstallationProcessor::CreateStubSisFileL(TFileName &aFileName)
	{
	// Do all this file stuff in SWI so we have necessary capabilities
	_LIT(KStubDelimiter,      "_");
	
	TUid appUid = ApplicationL().ControllerL().Info().Uid().Uid();
	TChar drive = ApplicationL().StubDrive();
	// build SwiDaemon Pathname
	aFileName.Append(drive);
	aFileName.Append(KDriveDelimiter);
	aFileName.Append(KPrivatePath);
	aFileName.AppendNumFixedWidth(KSwiDaemonUid, EHex, 8);
	aFileName.Append(KPathDelimiter);
	aFileName.AppendNumFixedWidth(appUid.iUid, EHex, 8);	
		
		
	Swi::Sis::TInstallType installType = ApplicationL().ControllerL().Info().InstallType();	
		
	switch(installType)
		{
		/* 
		If a device has more than one slot, we may install a PA/PP to a different media card. 
		In this case,we'll still create a stub SIS file even though we're installing from a media card.
		*/

		// If an Installation type is SA/PA then append _0 after the stub UID.
		case Swi::Sis::EInstInstallation:
		case Swi::Sis::EInstPreInstalledApp:
			{
			aFileName.Append(KStubDelimiter);
			aFileName.Append('0');	
			break;
			}
				
		// If an Installation type is SP/PP then append _augNumber+1 after the stub UID.
		case Swi::Sis::EInstPreInstalledPatch:
		case Swi::Sis::EInstAugmentation:
			{
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			RSisRegistrySession session;
			User::LeaveIfError(session.Connect());
			CleanupClosePushL(session);
#endif

			RSisRegistryEntry entry;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			User::LeaveIfError(entry.Open(iRegistryWrapper.RegistrySession(), appUid));
#else
			User::LeaveIfError(entry.Open(session, appUid));
#endif
			CleanupClosePushL(entry);	

			/*
			Get the Augmentations number and append the number in stub file name(DEF107470).
			This way we impose order on the installation sequence when the card is inserted on another device.
			Since SWI daemon processes stub files in alphabetic order, it will install augmentations only after the original package
			*/
			TInt augNumber = entry.AugmentationsNumberL();	
			User::LeaveIfError(augNumber);
			aFileName.Append(KStubDelimiter);
			aFileName.AppendFormat(_L("%d"),augNumber+1);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			CleanupStack::PopAndDestroy(&entry);
#else
			CleanupStack::PopAndDestroy(2, &session);
#endif
			break;
			}

		default: 
			/* 
			If it is not SA/PA/SP/PP, then this function shouldn't have been invoked, 
			as these are the only package types which can appear according to Functional Specification 
			on a media card.
			*/
			ASSERT(EFalse);
		}

	aFileName.Append(KSisExt);

	TEntry entry;
	if (KErrNone == Fs().Entry(aFileName, entry))
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TransactionSession().RemoveL(aFileName);
#else
		IntegrityServices().RemoveL(aFileName);	
#endif
		}

	// Notify integrity support that we've created a stub
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TransactionSession().RegisterNewL(aFileName);
#else
	IntegrityServices().AddL(aFileName);
#endif
			
	// create SwiDaemon Private directory on the target drive
	TInt ret = Fs().MkDirAll(aFileName);
	if (ret!= KErrNone && ret != KErrAlreadyExists)
		{
		User::Leave(ret);
		}

			
	// Create the stub file if required
	RFile file;
	User::LeaveIfError(file.Create(Fs(), aFileName, EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(file);
			
	TInt err = iSisHelper.CreateSisStub(file);
	CleanupStack::PopAndDestroy(&file);
		
	if(err != KErrNone)
		{
		// something went wrong while creating the stub
		// delete the incorrectly created file
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TransactionSession().RemoveL(aFileName);
#else
		IntegrityServices().RemoveL(aFileName);
#endif
		User::Leave(err);
		}
	}

void CInstallationProcessor::RegisterStubSisFileL(const TFileName &aFileName)
	{
	// Add the stub file to the list of files to be removed during uninstall
	CSisRegistryFileDescription* fileDescription 
			= CreateSisStubRegistryFileDescriptionLC(Fs(),aFileName);
		
	// cast away constness because we need to add a file to remove into the application
	CApplication& application  = const_cast<CApplication&>(ApplicationL());
	application.AddSisStubFileL(*fileDescription);
		
	CleanupStack::PopAndDestroy(fileDescription);
	}


TBool CInstallationProcessor::IsStubSisFileRequiredL()
	{
	// Check that drive is removable
	// we only write Stubs to removable media so they
	// will install when the same media card is inserted
	// in a different Symbian OS device
	TChar drive = ApplicationL().StubDrive();
	
	TDriveInfo driveInfo;
	TInt driveNum;
	RFs::CharToDrive(drive, driveNum);
	Fs().Drive(driveInfo, driveNum);
	
	if(driveInfo.iDriveAtt & KDriveAttRemovable)
		return ETrue;
	else
		return EFalse;
	}

CSisRegistryFileDescription* CInstallationProcessor::CreateSisStubRegistryFileDescriptionLC(RFs& aFs, const TDesC& aFileName)
	{
	_LIT(KDataTypeSisx, "x-epoc/x-sisx-app");
	
	// This will leave if the file does not exist
	CFileSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(aFs, aFileName);
	
	// create a SHA1 hash of the Sis Stub file 
	HBufC8* hash = iSecurityManager.CalculateHashLC(*dataProvider, CMessageDigest::ESHA1);
	CHashContainer* hashContainer = CHashContainer::NewLC(CMessageDigest::ESHA1, *hash);

	TInt64 fileLength = 0;
	dataProvider->Seek(ESeekCurrent, fileLength);
	Sis::TSISFileOperationOptions options = static_cast<Sis::TSISFileOperationOptions>(0);
	
	CSisRegistryFileDescription *fileDescription = 
				CSisRegistryFileDescription::NewL(*hashContainer,
													aFileName,
													KDataTypeSisx(),
													Sis::EOpInstall,
													options,
													fileLength,
													0,
													KNullUid);
													

	CleanupStack::PopAndDestroy(3, dataProvider); // hashContainer, hash, dataProvider
	CleanupStack::PushL(fileDescription);
	return fileDescription; 
	}


void CInstallationProcessor::AddEventToLogL(const CSisRegistryFileDescription& aFileDescription)
/**
	Write an install file event into the swi observation log file.
	
	@param aFileDescription The file whose name will be written into the log file.
 */
	{
	TUint8 fileFlag(EFileAdded);
	TParsePtrC targetPath(aFileDescription.Target());
				
	if (0 == targetPath.Path().CompareF(KBinPath))
		{
		TEntry entry;
		TInt err = Fs().Entry(aFileDescription.Target(), entry);
		
		if(KErrNone == err && entry.IsTypeValid())
			{
			if(SecUtils::IsExe(entry))
				{//Set file exe flag.
				fileFlag |= Swi::EFileExe;
				}
			else if(SecUtils::IsDll(entry))
				{//Set file dll flag.
				fileFlag |= Swi::EFileDll;
				}
			}
		}
		
	CObservationData *event = CObservationData::NewLC(aFileDescription.Target(),aFileDescription.Sid(),fileFlag);
	Observer().AddEventL(*event);
	CleanupStack::PopAndDestroy(event);
	}

void CInstallationProcessor::LaunchFileL(const CSisRegistryFileDescription& aFileDescription)
	{
	if (iApparcRegFiles.Count() > 0)
		{
		// Ask the launcher to notify Apparc of any new reg files
		RSisLauncherSession launcher;
		CleanupClosePushL(launcher);
		User::LeaveIfError(launcher.Connect());

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		RPointerArray<Usif::CApplicationRegistrationData> appRegInfoArray;
		CleanupClosePushL(appRegInfoArray);

		RFs fs;
		CleanupClosePushL(fs);
        User::LeaveIfError(fs.Connect());
		User::LeaveIfError(fs.ShareProtected());
				        
		const TInt appCount = iApparcRegFiles.Count();
		for (TInt i=0; i<appCount; ++i)
			{
			TInt bufLen = iApparcRegFiles[i]->Size();
			HBufC* buf = HBufC::NewLC(bufLen);
			TPtr appFile = buf->Des();
			appFile.Format(_L("%S"), iApparcRegFiles[i]);

			RFile file;
			User::LeaveIfError(file.Open(fs, appFile, EFileRead));
			CleanupClosePushL(file);
			Usif::CApplicationRegistrationData* appRegData  = NULL;
			TRAPD(err, appRegData = launcher.SyncParseResourceFileL(file));
			if (KErrNone == err)
			    {
		         CleanupStack::PushL(appRegData);
		         appRegInfoArray.AppendL(appRegData);
		         CleanupStack::Pop(appRegData);
			    }
			CleanupStack::PopAndDestroy(2,buf);
			}
		
		launcher.NotifyNewAppsL(appRegInfoArray);
        fs.Close();
        appRegInfoArray.ResetAndDestroy();
		CleanupStack::PopAndDestroy(2, &appRegInfoArray); //fs
#else
		launcher.NotifyNewAppsL(iApparcRegFiles);
#endif
		// clean up our list so we don't notify of the files twice
		iApparcRegFiles.ResetAndDestroy();
		
		CleanupStack::PopAndDestroy(&launcher);
		}

	// run the file
	RunFileL(aFileDescription.Target(), aFileDescription.MimeType(),
			aFileDescription.OperationOptions());
	}
	
TBool CInstallationProcessor::ShouldLaunchL(const CSisRegistryFileDescription& aFileDescription)
	{
	if (aFileDescription.Operation() == Sis::EOpRun && 
		aFileDescription.OperationOptions() & Sis::EInstFileRunOptionInstall)
		{
#ifdef SYMBIAN_SWI_RUN_ON_INSTALL_COMPLETE 
		return !(aFileDescription.OperationOptions() & Sis::EInstFileRunOptionAfterInstall);
#else
		return ETrue;
#endif	
		}
	return EFalse;
	}

void CInstallationProcessor::AddApparcFilesInListL(const TDesC& aTargetFileName)
	{
	if (FileIsApparcReg(aTargetFileName))
		{
		// we're installing a reg file so add it to our list. 
		HBufC* tmp = aTargetFileName.AllocLC();
		iApparcRegFiles.AppendL(tmp);
		CleanupStack::Pop(tmp);

#ifdef 	SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	    // we're installing a reg file so add it to our list for parsing it and 
		// populating SCR in EParseApplicationRegistrationFiles state of CProcessor	    
		TInt index = UserSelectedLanguageIndexL(ApplicationL());
		CSisRegistryPackage *regPkg = CSisRegistryPackage::NewLC(ApplicationL().ControllerL().Info().Uid().Uid(),\
		        ApplicationL().ControllerL().Info().Names()[index]->Data(),\
		        ApplicationL().ControllerL().Info().UniqueVendorName().Data());
		CAppRegFileData *appRegData =CAppRegFileData::NewLC(aTargetFileName,*regPkg);
	    iApparcRegFilesForParsing.AppendL(appRegData);
	    CleanupStack::Pop(appRegData);
	    CleanupStack::PopAndDestroy(regPkg);
#endif		
		}		
	}

#ifdef  SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

TInt CInstallationProcessor::UserSelectedLanguageIndexL(const CApplication& aApplication) const
// used to find out which is the index of the selected language, 
// based on the language selection. This will be used for the relevant package and vendor names
    {
    TLanguage language = aApplication.UserSelections().Language();
    
    TInt index = KErrNotFound;
    for (TInt i = 0; i < aApplication.ControllerL().SupportedLanguages().Count(); i++)
        {
        if (aApplication.ControllerL().SupportedLanguages()[i] == language)
            {
            index = i;
            break;  
            }
        }
    User::LeaveIfError(index);
    return index;
    }   

CAppRegFileData* CAppRegFileData::NewL(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage)
    {
    CAppRegFileData* self = CAppRegFileData::NewLC(aAppRegFile, aSisRegistryPackage);
    CleanupStack::Pop(self);
    return self;
    }

CAppRegFileData* CAppRegFileData::NewLC(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage)
    {
    CAppRegFileData* self = new(ELeave) CAppRegFileData();
    CleanupStack::PushL(self);
    self->ConstructL(aAppRegFile, aSisRegistryPackage);
    return self;
    }
void CAppRegFileData::ConstructL(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage)
    {
    iAppRegFile = aAppRegFile.AllocL();
    iSisRegistryPackage = CSisRegistryPackage::NewL(aSisRegistryPackage);
    }
CAppRegFileData::CAppRegFileData()
    {
    }

CAppRegFileData::~CAppRegFileData()
    {
    delete iAppRegFile;
    delete iSisRegistryPackage;
    }

TDesC& CAppRegFileData::GetAppRegFile()
    {
    return *iAppRegFile;
    }

CSisRegistryPackage& CAppRegFileData::GetSisRegistryPackage()
    {
    return *iSisRegistryPackage;
    }
#endif
