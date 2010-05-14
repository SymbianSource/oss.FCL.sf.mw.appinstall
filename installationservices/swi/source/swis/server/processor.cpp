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
* Application Processor
*
*/


/**
 @file
 @released
 @internalTechnology 
*/

#include <hash.h>
#include "processor.h"

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "integrityservices.h"
#endif

#include "sishelperclient.h"
#include "sisregistryfiledescription.h"
#include "sisregistryentry.h"
#include "sisstring.h"
#include "hashcontainer.h"
#include "siscontroller.h"
#include "application.h"
#include "log.h"
#include "secutils.h"
#include "sisuihandler.h"
#include "filesisdataprovider.h"
#include "securitymanager.h"
#include "sislauncherclient.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "plan.h"
#include "securitycheckutil.h"
#include "progressbar.h"
#include "sidcache.h"
#include "planner.h"
#include "sisregistrypackage.h"
#include <f32file.h>
#include <e32property.h>
#include <swi/swispubsubdefs.h>

using namespace Swi;

const TChar KRomDrive = 'z';

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CProcessor::CProcessor(const CPlan& aPlan, RUiHandler& aUiHandler,
		Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper, 
		RSwiObserverSession& aObserver)
	: CActive(EPriorityStandard), 
	iSystemDriveChar(RFs::GetSystemDriveChar()),
	iRegistryWrapper(aRegistryWrapper),
	iPlan(aPlan),	
	iUiHandler(aUiHandler),
	iStsSession(aStsSession),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
#else
CProcessor::CProcessor(const CPlan& aPlan, RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver)
	: CActive(EPriorityStandard),
	iSystemDriveChar(RFs::GetSystemDriveChar()),
	iPlan(aPlan),	
	iUiHandler(aUiHandler),
	iIntegrityServices(aIntegrityServices),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
#endif

CProcessor::~CProcessor()
	{
	Cancel();
	iFs.Close();
	iSidsAdded.Close();
	iSidsRemoved.Close();
	iSidsToShutdown.Close();
	}

void CProcessor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());	
	}

void CProcessor::ProcessApplicationL(const CApplication& aApplication, TRequestStatus& aClientStatus)
	{
	iApplication = &aApplication;
	 
	iClientStatus = &aClientStatus;
	aClientStatus = KRequestPending;
	iErrorCode = 0;
	
	// We only get the validation status of the package for uninstallation here 	
	if (ApplicationL().IsUninstall())
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		RSisRegistryEntry entry;		
		User::LeaveIfError(entry.OpenL(iRegistryWrapper.RegistrySession(), ApplicationL().PackageL()));			
		CleanupClosePushL(entry);
		iValidationStatus=entry.TrustStatusL().ValidationStatus();
		CleanupStack::PopAndDestroy(&entry);
#else
		RSisRegistrySession session;
		User::LeaveIfError(session.Connect());
		CleanupClosePushL(session);		
		RSisRegistryEntry entry;		
		User::LeaveIfError(entry.OpenL(session, ApplicationL().PackageL()));			
		CleanupClosePushL(entry);
		iValidationStatus=entry.TrustStatusL().ValidationStatus();
		CleanupStack::PopAndDestroy(&entry);
		CleanupStack::PopAndDestroy(&session);
#endif
		}
	else
		{
		iValidationStatus =	ApplicationL().ControllerL().TrustStatus().ValidationStatus();
		}
		
	iSidsAdded.Reset();
	iSidsRemoved.Reset();
	iSidsToShutdown.Reset();
		
	SwitchState(EInitialize);
	}

void CProcessor::ProcessPlanL(TRequestStatus& aClientStatus)
	{
	ProcessApplicationL(iPlan.ApplicationL(), aClientStatus);
	}

void CProcessor::Reset()
	{
	}

void CProcessor::DoCancel()
	{
	iCancelled=ETrue;
	User::RequestComplete(iClientStatus, KErrCancel);
	}

void CProcessor::RunL()
	{
	DEBUG_PRINTF3(_L8("Install Server - Processor State Machine, State: %d, Status: %d"),
		iState, iStatus.Int());
	
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());
	
	if (iCancelled)
		{
		User::Leave(KErrCancel);
		}
		
	switch(iState)
		{
	case EInitialize:
		if (DoStateInitializeL())
			{
			SwitchState(EProcessEmbedded);
			}
		break;
		
	case EProcessEmbedded:
		if (DoStateProcessEmbeddedL())
			{
			SwitchState(EShutdownAllApps);
			}
		break;
	
	case EShutdownAllApps:
		if (DoStateShutdownAllAppsL())
			{
			SwitchState(ECheckApplicationInUse);
			}
		break;

	case ECheckApplicationInUse:
		if (DoStateCheckApplicationInUseL())
			{
			SwitchState(EShutdownExe);
			}
		break;
	
	case EShutdownExe:
		if (DoStateShutdownExeL())
			{
			SwitchState(EDisplayFiles);
			}
		break;
	
	case EDisplayFiles:
		if (DoStateDisplayFilesL())
			{
			SwitchState(EProcessSkipFiles);
			}
		break;

	case EProcessSkipFiles:
		if (DoStateProcessSkipFilesL())
			{
			SwitchState(EExtractFiles);
			}
		break;

	case EExtractFiles:
		if (DoStateExtractFilesL())
			{
			SwitchState(EProcessFiles);
			}
		break;

	case EProcessFiles:
		if (DoStateProcessFilesL())
			{
			SwitchState(EVerifyPaths);
			}
		break;

	case EVerifyPaths:
		if (DoStateVerifyPathsL())
			{
			SwitchState(ERemoveFiles);
			}
		break;

	case ERemoveFiles:
		if (DoStateRemoveFilesL())
			{
			SwitchState(ERemovePrivateDirectories);
			}
		break;

	case ERemovePrivateDirectories:
		if (DoStateRemovePrivateDirectoriesL())
			{
			SwitchState(EInstallFiles);
			}
		break;
	
	case EInstallFiles:
		if (DoStateInstallFilesL())
			{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK			
			SwitchState(EParseApplicationRegistrationFiles);
#else
			SwitchState(EUpdateRegistry);
#endif			
			}
		break;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	case EParseApplicationRegistrationFiles:
	    if (DoParseApplicationRegistrationFilesL())
	        {
	        SwitchState(EUpdateRegistry);
	        }
	    break;
#endif		
	    
	case EUpdateRegistry:
		if (DoStateUpdateRegistryL())
			{
			SwitchState(EFinished);
			}
		break;
	
	case EFinished:
		DoStateFinishedL();
		break;
		
	default:
		User::Leave(KErrGeneral);
		break;
		}
	}

void CProcessor::SwitchState(TProcessingState aNextState)
	{
	if (aNextState!=ECurrentState)
		{
		iState=aNextState;
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CProcessor::WaitState(TProcessingState aNextState)
	{
	if (aNextState!=ECurrentState)
		{
		iState=aNextState;
		}
	SetActive();
	}

TInt CProcessor::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Install Server - Processor State Machine RunError, Error code %d"), aError);
	
	User::RequestComplete(iClientStatus, aError);
	return KErrNone;
	}

// implementations of the states provided

void CProcessor::DoStateFinishedL()
	{
	User::RequestComplete(iClientStatus, iErrorCode);
	}

// common state functions

TBool CProcessor::DoStateShutdownAllAppsL()
	{
	if(ApplicationL().ShutdownAllApps())
		{
		iUiState = EShutdownAllApps;
		// Signal to UI apps are being shut down
		CHandleCancellableInstallEvent* cmd = CHandleCancellableInstallEvent::NewLC(Plan().AppInfoL(), EEventShuttingDownApps, 0, KNullDesC);
		UiHandler().ExecuteL(*cmd);
		CleanupStack::PopAndDestroy(cmd);
		
		RSisLauncherSession launcher;
		CleanupClosePushL(launcher);
		User::LeaveIfError(launcher.Connect());
		launcher.ShutdownAllL();
		CleanupStack::PopAndDestroy(&launcher);
		}
	//Here is the first point where a package is actually being started to install.
	//Hence, the package header is logged at that point.
	TUid pkgUid;
	TPackageType pkgType;
	TOperationType opType;
	
	if(ApplicationL().IsUninstall())
		{
		pkgUid = ApplicationL().PackageL().Uid();
		//It is assumed that we don not need pkg type during uninstallation
		pkgType = EUnknownPackage; 
		opType = EOpUninstall;
		}
	else
		{
		pkgUid = ApplicationL().ControllerL().Info().Uid().Uid();
		pkgType = static_cast<TPackageType>(ApplicationL().ControllerL().Info().InstallType());
		opType = EOpInstall;
		}
		
	CObservationHeader *header = CObservationHeader::NewLC(pkgUid, pkgType, opType);
	
	Observer().AddHeaderL(*header);
	CleanupStack::PopAndDestroy(header);
	return ETrue;
	}

TBool CProcessor::DoStateCheckApplicationInUseL()
	{
	const RPointerArray<CSisRegistryFileDescription>& filesToRemove = ApplicationL().FilesToRemove();
	TInt fileCount = filesToRemove.Count();

	if (fileCount > 0)
		{		
		// Find Sids corresponding to Exes we are removing.			
		RArray<TAppInUse> sidsArray;
		CleanupClosePushL(sidsArray);
		
		for (TInt i = 0; i < fileCount; i++)
			{
			TEntry entry;
			TInt err = Fs().Entry(filesToRemove[i]->Target(), entry);
			// if this is an EXE.
			if (err == KErrNone && entry.IsTypeValid() &&
				SecUtils::IsExe(entry))
				{
				//Add it the check array
				TAppInUse temp;
				temp.iAppUid=filesToRemove[i]->Sid();
				temp.iInUse=EFalse;
				sidsArray.AppendL(temp);				
				}
			}			
			
		// If there are any executables, check if they are in use.
		TInt exeCount=sidsArray.Count();
		if (exeCount>0)
			{
			RSisLauncherSession launcher;
			CleanupClosePushL(launcher);
			User::LeaveIfError(launcher.Connect());
			
			//check if they are running	
			TRAPD(err,launcher.CheckApplicationInUseL(sidsArray));
			if(err == KErrNone)
				{
				for (TInt i=0;i<exeCount;i++)
					{
					if (sidsArray[i].iInUse)
						{
						iSidsToShutdown.AppendL(sidsArray[i].iAppUid);												
						}
					}
				}
			else
				{
				User::LeaveIfError(err);
				}
			CleanupStack::PopAndDestroy(&launcher);				
			}
		CleanupStack::PopAndDestroy(&sidsArray);
		}

	// If any Sids are in use, display dialog for applications in use
	// which offers the chance to cancel.
	if(iSidsToShutdown.Count() > 0)
		{
		if(!DisplayApplicationInUseL())
			{
			User::Leave(KErrCancel);	
			}
		else 
			{
			// User has chosen to shut down apps.  Set shutdown timeout from
			// software install security policy.
			CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
			iShutdownTimeout = secPolicy->ApplicationShutdownTimeout();
			}
		}

	return ETrue;
	}
	
TBool CProcessor::DoStateShutdownExeL()
	{
	if (iSidsToShutdown.Count()>0)
		{
		// If this is the first app to shut down, signal to UI apps are
		// being shut down.
		if(iUiState != EShutdownExe)
			{
			iUiState = EShutdownExe;
			CHandleCancellableInstallEvent* cmd = CHandleCancellableInstallEvent::NewLC(Plan().AppInfoL(), EEventShuttingDownApps, 0, KNullDesC);
			UiHandler().ExecuteL(*cmd);
			CleanupStack::PopAndDestroy(cmd);
			}
				
		RSisLauncherSession launcher;
		CleanupClosePushL(launcher);
		User::LeaveIfError(launcher.Connect());
		
		launcher.ShutdownL(iSidsToShutdown, iShutdownTimeout);
			
		CleanupStack::PopAndDestroy(&launcher);		
		}	
	return ETrue;
	}

TBool CProcessor::DoStateRemoveFilesL()
	{
	if (iCurrent < ApplicationL().FilesToRemove().Count())
		{
		if (iUiState != ERemoveFiles)
			{
			iUiState = ERemoveFiles;
			// Signal to UI we are removing files
			CHandleCancellableInstallEvent* cmd = CHandleCancellableInstallEvent::NewLC(Plan().AppInfoL(), EEventRemovingFiles, 0, KNullDesC);
			UiHandler().ExecuteL(*cmd);
			CleanupStack::PopAndDestroy(cmd);
			}
		RemoveFileL(*ApplicationL().FilesToRemove()[iCurrent++]);					
		SwitchState(ECurrentState);
		return EFalse;		
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}

TBool CProcessor::DoStateRemovePrivateDirectoriesL()
	{
	if (iCurrent < iSidsRemoved.Count())
		{
		// Remove the private directory if the sid is not being replaced
		// and it is not in ROM too (we could be removing an exe which
		// eclipsed one in ROM.)
		if(iSidsAdded.Find(iSidsRemoved[iCurrent]) == KErrNotFound)
			{
			CSidCache* sidCache;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			sidCache = CSidCache::NewLC(iStsSession);
#else
			sidCache = CSidCache::NewLC(iIntegrityServices);
#endif
			if (!sidCache->IsCachedL(iSidsRemoved[iCurrent]))
				{
				RemovePrivateDirectoryL(iSidsRemoved[iCurrent]);
				}
			CleanupStack::PopAndDestroy(sidCache);
			}
		iCurrent++;
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
TBool CProcessor::DoStateProcessFilesL()
	{
	return ETrue;
	}
	
// utility functions

void CProcessor::RunFileL(const TDesC& aFileName, const TDesC& aMimeType, Sis::TSISFileOperationOptions aFileOperationOption)
	{
	CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
	if (!secPolicy->AllowRunOnInstallUninstall() && iValidationStatus < EValidatedToAnchor )
		{
		return;			
		}
		
	TBool wait = EFalse;
	if((aFileOperationOption & Sis::EInstFileRunOptionWaitEnd) 
	   || ApplicationL().IsUninstall() || iState == ERemoveFiles)
		{
		// always wait for completion or timeout when uninstalling since 
		// cannot remove the file when it is in use!
		wait = ETrue;
		}
	
	RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
	User::LeaveIfError(launcher.Connect());
	
	// Is the file an executable ?
	TEntry entry;
	User::LeaveIfError(Fs().Entry(aFileName, entry));
	if (entry.IsTypeValid() && SecUtils::IsExe(entry))
		{
		launcher.RunExecutableL(aFileName, wait);
		}	
	else 
		{
		RFile file;
		CleanupClosePushL(file);				
		if (aFileOperationOption & Sis::EInstFileRunOptionByMimeType)
			{
			HBufC8* mimeType = HBufC8::NewLC(aMimeType.Length());
			TPtr8 ptr = mimeType->Des();
			ptr.Copy(aMimeType);
			TRAPD(err, launcher.StartByMimeL(aFileName, *mimeType, wait));
			if (err!=KErrNone) 
				{
				User::LeaveIfError(file.Open(iFs, aFileName, EFileShareExclusive|EFileWrite));
				TRAP_IGNORE(launcher.StartByMimeL(file, *mimeType, wait));
				}
			CleanupStack::PopAndDestroy(mimeType);		
			}
		else
			{
			TRAPD(err, launcher.StartDocumentL(aFileName, wait));		
			if (err!=KErrNone)
				{
				User::LeaveIfError(file.Open(iFs, aFileName, EFileShareExclusive|EFileWrite));
				TRAP_IGNORE(launcher.StartDocumentL(file, wait));	
				}
			}
		CleanupStack::PopAndDestroy(&file);		
		}
	CleanupStack::PopAndDestroy(&launcher);
	}
	
TBool CProcessor::IsSafeUninstallModeSetL()
	{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt safeModeValue(0);
	TInt res = RProperty::Get(KUidInstallServerCategory, KUidSafeModeUninstallKey, safeModeValue);
	if (res != KErrNone && res != KErrNotFound)
		User::Leave(res);
	return (res == KErrNone && safeModeValue == KSwisSafeModeUninstallEnabled);
#else
	return EFalse;
#endif
	}
	
void CProcessor::RemoveFileL(const CSisRegistryFileDescription& aFileDescription)
	{
	DEBUG_PRINTF2(_L("Install Server - Processor State Machine, Removing File '%S'"),
		&aFileDescription.Target());
	
	TInt err;
	//Explicitly set boolean to keep armv5 compiler happy.
	TBool skipRemove(EFalse);
	TBool romBasedExe(EFalse);
	TInt driveNo;
	
	err = Fs().CharToDrive(aFileDescription.Target()[0], driveNo);

	if (err == KErrArgument)
		{
		skipRemove = ETrue;
		}
	else
		{
		// Should leave if RFs::CharToDrive returned anything other than
		// KErrNone or KErrArgument, since the problem is more serious than an
		// invalid path.
		User::LeaveIfError(err);
		}

	TBool isPreInstalled = ApplicationL().IsPreInstalledApp() ||
							ApplicationL().IsPreInstalledPatch();

	// Run files only if not pre-installed
	if (!isPreInstalled)
		{
		if (aFileDescription.Operation() == Sis::EOpRun &&
			 aFileDescription.OperationOptions() & Sis::EInstFileRunOptionUninstall)
			{		
			// Check whether safe mode is enabled - don't run the executable in this case

			if (!IsSafeUninstallModeSetL())
				{
				// ignore failure to run on uninstall, so a failing executable does
				// not block a removal.
				TRAP_IGNORE(RunFileL(aFileDescription.Target(), aFileDescription.MimeType(), aFileDescription.OperationOptions()));
				}
			}	
		}
	else if (!skipRemove)
		{
		// For preinstalled packages, we will delete files if permitted by
		// the swipolicy and if the application is tagged as deletable preinstalled
		// (based on whether the stub sis file was writable at install time.)
		// The swipolicy check is paranoid, since the application should only
		// be marked as deletable preinstalled if the swipolicy allowed it at
		// install time.
		CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
		skipRemove = !(ApplicationL().IsDeletablePreinstalled() && secPolicy->DeletePreinstalledFilesOnUninstall());
		}

	// If we're not already skipping from previous checks, skip if it's
	// preinstalled and ROM or write-protected or read-only, or filenull on
	// ROM.
	if (!skipRemove)
		{
		if ((isPreInstalled || aFileDescription.Operation() == Sis::EOpNull))
			{
			TDriveInfo driveInfo;
			err = Fs().Drive(driveInfo, driveNo);
			if ((err != KErrNone) || (driveInfo.iDriveAtt & KDriveAttRom) ||
			(isPreInstalled && (driveInfo.iMediaAtt & KMediaAttWriteProtected)))
				{
				skipRemove = ETrue;
				}
			else if (isPreInstalled)
				{
				TEntry entry;
				err = Fs().Entry(aFileDescription.Target(), entry);
				skipRemove = ((err != KErrNone) || entry.IsReadOnly());
				}
			}
		}
	//Flag for observation log
	TUint8 fileFlag(EFileDeleted);	
	
	// for the files in sys\bin directory, related hash values need to be removed
	TParsePtrC targetPath(aFileDescription.Target());
	if (targetPath.Path().CompareF(KBinPath) == 0)
		{
		
		// Check wheather the file is referenced by one of the ROM stub files.
		CSidCache* sidCache;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		sidCache = CSidCache::NewLC(iStsSession);
#else
		sidCache = CSidCache::NewLC(iIntegrityServices);
#endif
		TUid fileSid = aFileDescription.Sid();
		romBasedExe = sidCache->IsCachedL(fileSid);
		CleanupStack::PopAndDestroy(sidCache);
		
		// Backup and later delete the hash	
		TBuf<32> hashPath;	
		TUint driveCh(iSystemDriveChar); // can't pass TChar to Format
		hashPath.Format(KHashPathFormat, driveCh, &KHashPath);	
		
		TParse hashFileName;
		hashFileName.Set(hashPath, &aFileDescription.Target(), NULL);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TRAP(err, TransactionSession().RemoveL(hashFileName.FullName()));
#else
		TRAP(err, IntegrityServices().RemoveL(hashFileName.FullName()));
#endif
		// Display error dialog if the file cannot be removed.
		if(err == KErrInUse)
			{
			CDisplayError* displayCannotDelete=CDisplayError::NewLC(iPlan.AppInfoL(), EUiCannotDelete, hashFileName.FullName());
			iUiHandler.ExecuteL(*displayCannotDelete);
			CleanupStack::PopAndDestroy(displayCannotDelete);
			}
		if(err != KErrNotFound && err != KErrPathNotFound && err != KErrNotReady && err != KErrCorrupt)
			{
			User::LeaveIfError(err);
			}
		//Create an SWI event for the hash file					
		CObservationData *event = CObservationData::NewLC(hashFileName.FullName(),TUid::Null(),fileFlag);
		//Write the event into the observation log file
		Observer().AddEventL(*event);
		CleanupStack::PopAndDestroy(event);

		// If we're skipping removal of an exe we also don't want to remove
		// associated private directories, so we only need to track the SIDs
		// for exes being removed.
		if (!skipRemove)
			{
			TEntry entry;
			err = Fs().Entry(aFileDescription.Target(), entry);

			// When an executable file is removed from the system drive, it may have a counterpart 
			// in ROM storage. This may happen if the ROM file was upgraded with an SU package.			TFileName target(aFileDescription.Target());
			TFileName target(aFileDescription.Target());
			target[0] = KRomDrive;
			TEntry romEntry;
			TInt error = Fs().Entry(target, romEntry);
			TBool existInRom = (error == KErrNone);
	
			// if this is an EXE, add to list of SIDS removed for later
			// cleanup of private directories.
			if (err == KErrNone && entry.IsTypeValid() && !romBasedExe && !existInRom)
				{
				if(SecUtils::IsExe(entry))
					{
					TUid sid = aFileDescription.Sid();
					User::LeaveIfError(iSidsRemoved.Append(sid));
					//Set file exe flag.
					fileFlag |= Swi::EFileExe;	
					}
				else if(SecUtils::IsDll(entry))
					{
					//Set file dll flag
					fileFlag |= Swi::EFileDll;
					}
				}
			}
		}
		
	if (!skipRemove)
		{
		TInt err = KErrNone;
		// use transaction support to backup and later delete the file
		// ignore missing files, media cards not present or corrupt media

		TPtrC fileToRemove;
		if(targetPath.IsWild() || targetPath.NameAndExt().Length() == 0)
			{
			fileToRemove.Set(targetPath.DriveAndPath());
			}
		else
			{
			fileToRemove.Set(aFileDescription.Target());
			}
		
		// Skip targets less than 4 characters, since this won't be a valid path
		// for transaction service RemoveL().
		if (fileToRemove.Length() > 3)
			{
			err = RemoveWithRetryAttemptL(fileToRemove);

			if(err !=KErrNone && err != KErrNotFound && err != KErrPathNotFound	&& err != KErrNotReady && err != KErrCorrupt)
				{
				CDisplayError* displayCannotDelete=CDisplayError::NewLC(iPlan.AppInfoL(), EUiCannotDelete, fileToRemove);
				iUiHandler.ExecuteL(*displayCannotDelete);
				CleanupStack::PopAndDestroy(displayCannotDelete);
				User::Leave(err);
				}
			//Create an SWI event					
			CObservationData *event = CObservationData::NewLC(fileToRemove,aFileDescription.Sid(),fileFlag);
			//Write the event into the observation log file
			Observer().AddEventL(*event);
			CleanupStack::PopAndDestroy(event);
			}
		}
	// Update progress bar by one for uninstall of file
	iUiHandler.UpdateProgressBarL(iPlan.AppInfoL(), KProgressBarUninstallAmount);
	}

void CProcessor::RemovePrivateDirectoryL(TUid aSid)
	{
	DEBUG_PRINTF2(_L8("Removing private directories \\private\\%08x\\"), aSid.iUid);
	
	_LIT(KPrivatePath, "?:\\private\\");
	const TInt KPathLength = 8;
	TFileName privatePath = KPrivatePath();
	privatePath.AppendNumFixedWidthUC(aSid.iUid, EHex, KPathLength);
	privatePath.Append(KPathDelimiter);
	
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));
	for(TInt drive = EDriveA; drive <= EDriveZ; drive++)
		{
		// only attempt removal on writeable drives that are present
		if (driveList[drive])
			{
			TDriveInfo info;
			TInt err = iFs.Drive(info, drive);
			if(err == KErrNone)
				{
				if (!(info.iMediaAtt & KMediaAttWriteProtected) && info.iType!=EMediaNotPresent)
					{
					TChar driveLetter;
					User::LeaveIfError(iFs.DriveToChar(drive, driveLetter));
					privatePath[0] =  static_cast<TText> (driveLetter);

					err = RemoveWithRetryAttemptL(privatePath);

					if (err != KErrNone && err != KErrNotReady &&
						err != KErrNotFound && err != KErrPathNotFound && err != KErrCorrupt)
						{
						User::Leave(err);
						}
					}
				}
			}
		}
	}

TBool CProcessor::DisplayApplicationInUseL()
	{
	RPointerArray<TDesC> appName;
		
	User::LeaveIfError(appName.Append(&Plan().AppInfoL().AppName()));
	CleanupClosePushL(appName);

	CDisplayApplicationsInUse* displayApplicationInUse=CDisplayApplicationsInUse::NewLC(Plan().AppInfoL(),appName);
	UiHandler().ExecuteL(*displayApplicationInUse);
	TBool result = displayApplicationInUse->ReturnResult();
	
	CleanupStack::PopAndDestroy(2,&appName);
	return (result);
	}
	
void CProcessor::RunBeforeShutdown()
	{
	// If safe mode is set, we should not run any executables
	TBool isSafeUninstallModeSet(EFalse);
	TRAP_IGNORE(isSafeUninstallModeSet = IsSafeUninstallModeSetL());
	if (isSafeUninstallModeSet)	
		return;
		
	const RPointerArray<CSisRegistryFileDescription>& descriptions = Plan().FilesToRunBeforeShutdown();	
	TInt count(descriptions.Count());
	for (TInt i = 0; i < count; i++)
		{
		CSisRegistryFileDescription* fileName = descriptions[i];
		DEBUG_PRINTF2(_L("Install Server - Processor State Machine, Running the RBS executable '%S'"),
				&fileName->Target());
		
		// ignore failure to run before shutdown during uninstall, so a failing executable does
		// not block uninstallation.Retaining the RunRemove behaviour. 
		TRAP_IGNORE(RunFileL(fileName->Target(), fileName->MimeType(), fileName->OperationOptions()));
		}	
	}

TInt CProcessor::RemoveWithRetryAttemptL(TDesC& aFileToRemove)
	{
	TInt err = KErrNone;
	TInt noOfDetletionAttempts=1;
			
	do {
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	    TRAP(err, TransactionSession().RemoveL(aFileToRemove));
#else
	    TRAP(err, IntegrityServices().RemoveL(aFileToRemove));
#endif
        DEBUG_PRINTF2(_L8("Deletion attempt %d"), noOfDetletionAttempts);
        noOfDetletionAttempts++;
        User::After(KRetryInterval);
		} while ((err == KErrInUse ||err==KErrAccessDenied )&& noOfDetletionAttempts <= KMaxNoOfDeletionAttempts );
	return err;
	}

