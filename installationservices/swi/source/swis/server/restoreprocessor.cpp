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
*
*/


#include "restoreprocessor.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#include "swtypereginfo.h"
#include "installswtypehelper.h"
#else
#include "integrityservices.h"
#endif

#include "application.h"
#include "hashcontainer.h"
#include "secutils.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sistruststatus.h"
#include "log.h"
#include "securitycheckutil.h"
#include "sidcache.h"
#include <f32file.h>

using namespace Swi; 


//
// CRestoreProcessor
//
	
CRestoreProcessor::CRestoreProcessor(const CPlan& aPlan, const TDesC8& aControllerBuffer,
	CSecurityManager& aSecurityManager,	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,RSwiObserverSession& aObserver)
	: CActive(CActive::EPriorityStandard),
	  iVerifiers(aVerifiers),
	  iSecurityManager(aSecurityManager),
	  iControllerBuffer(aControllerBuffer),	  
	  iPlan(aPlan),
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	  iStsSession(aStsSession),
	  iRegistrySession(aRegistrySession),
#else
	  iIntegrityServices(aIntegrityServices),
#endif
	  iSystemDriveChar(RFs::GetSystemDriveChar()),
	  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	
	}
		
CRestoreProcessor* CRestoreProcessor::NewL(const CPlan& aPlan, const TDesC8& aControllerBuffer, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
	RArray<TUid>& aSids, RSwiObserverSession& aObserver)
	{
	CRestoreProcessor* self = CRestoreProcessor::NewLC(aPlan, aControllerBuffer, aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			aStsSession, aRegistrySession,
#else
			aIntegrityServices, 
#endif
			aVerifiers, aSids, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
		
CRestoreProcessor* CRestoreProcessor::NewLC(const CPlan& aPlan, const TDesC8& aControllerBuffer, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
	RArray<TUid>& aSids, RSwiObserverSession& aObserver)
	{
	CRestoreProcessor* self = new (ELeave) CRestoreProcessor(aPlan, aControllerBuffer, aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		aStsSession, aRegistrySession,
#else
		aIntegrityServices, 
#endif
		aVerifiers, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSids);
	return self;
	}

void CRestoreProcessor::InstallFileL(const CSisRegistryFileDescription& aRegistryFileDescription)
	{	
	TParsePtrC parse(aRegistryFileDescription.Target());
	
	TFileName tempFileName;
	_LIT(KTemporaryFileFormat, "%c:%Stfile-%d-%d-%d-");
	TUint driveCh(iSystemDriveChar);
	tempFileName.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath,
		iApplication->ControllerL().Info().Uid().Uid().iUid, iApplication->AbsoluteDataIndex(), aRegistryFileDescription.Index());

	// Append a hex representation of a hash of the filename. We need the
	// temporary filename to be effectively unique, but the filename itself
	// may be too long.
	// Use the entire path minus the drive letter because the drive letter can change.
	// For consistency, the target name must be the name from controller not the target
	// after the path substitution code.
	HBufC* hashBuf = SecUtils::HexHashL(parse.FullName().Right(parse.FullName().Length() - 1));
	tempFileName.Append(*hashBuf);
	delete hashBuf;
	
	DEBUG_PRINTF3(_L("Restore - Installing File '%S' from '%S'"), &aRegistryFileDescription.Target(), &tempFileName);

  	TEntry entry;
  	TInt err = iFs.Entry(tempFileName, entry);
  	if (err == KErrNone)
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TRAP(err, iStsSession.RemoveL(aRegistryFileDescription.Target()));
#else
  		TRAP(err, iIntegrityServices.RemoveL(aRegistryFileDescription.Target()));
#endif
  		if (err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
  			{
			User::Leave(err);
			}
		
		TInt err = iFs.MkDirAll(aRegistryFileDescription.Target());
  		if (err != KErrNone && err != KErrAlreadyExists)
  			{
  			User::LeaveIfError(err);
  			}
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
  		iStsSession.RegisterNewL(aRegistryFileDescription.Target());
#else
  		iIntegrityServices.AddL(aRegistryFileDescription.Target());
#endif
  		User::LeaveIfError(iFileMan->Move(tempFileName, aRegistryFileDescription.Target()));
		}
else
 		{
 		DEBUG_PRINTF3(_L("Cannot find '%S' assuming SBE will restore public data err = %d"), &tempFileName, err);
		} 

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Parse the file if it carries software type registration info
	if (InstallSoftwareTypeHelper::IsValidSwRegFileL(aRegistryFileDescription.Target(),
													 iApplication->ControllerL().Info().Uid().Uid().iUid))
		{
		InstallSoftwareTypeHelper::ParseRegFileL(iFs,
												 aRegistryFileDescription.Target(),
												 iSoftwareTypeRegInfoArray);
		}
#endif

	}

void CRestoreProcessor::ConstructL(RArray<TUid>& aSids)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	iFileMan = CFileMan::NewL(iFs);
	for (TInt i = 0; i < aSids.Count(); ++i)
		{
		iSids.AppendL(aSids[i]);
		}
	}

void CRestoreProcessor::ExtractHashL(const CSisRegistryFileDescription& aFileToProcess)
	{			
	TBuf<32> hashPath;	
	TUint driveCh(iSystemDriveChar); // can't pass TChar to Format
	hashPath.Format(KHashPathFormat, driveCh, &KHashPath);	
	TParse hashFileName;
	hashFileName.Set(hashPath, &aFileToProcess.Target(), NULL);

	TEntry hashEntry;
	TInt err = iFs.Entry(hashFileName.FullName(), hashEntry);

	if (err == KErrNone)
		{
		// Hash file exists, need to remove the old one.
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iStsSession.RemoveL(hashFileName.FullName());
#else
		iIntegrityServices.RemoveL(hashFileName.FullName());
#endif
		}
	
	err = iFs.MkDirAll(hashFileName.DriveAndPath());
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		} 

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.RegisterNewL(hashFileName.FullName());
#else
	iIntegrityServices.AddL(hashFileName.FullName());
#endif

	DEBUG_PRINTF2(_L("Restore - Extracting hash to file '%S'"),
		&hashFileName.FullName());

	RFile file;
	User::LeaveIfError(file.Create(iFs, hashFileName.FullName(),
		EFileWrite | EFileShareExclusive | EFileStream));
		
	CleanupClosePushL(file);
	const TDesC8& hashData = aFileToProcess.Hash().Data();
	User::LeaveIfError(file.Write(hashData));
	CleanupStack::PopAndDestroy(&file);
	}
			
TBool CRestoreProcessor::DoStateInitializeL()
	{
	iCurrent = 0;
	return ETrue;
	}


TBool CRestoreProcessor::DoStateInstallFilesL()
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{
		if(0 == iCurrent)
			{//Write package header before logging the first install event
			CObservationHeader *header = CObservationHeader::NewLC(iApplication->ControllerL().Info().Uid().Uid(), 
						static_cast<TPackageType>(iApplication->ControllerL().Info().InstallType()), Swi::EOpInstall);
			iObserver.AddHeaderL(*header);
			CleanupStack::PopAndDestroy(header);
			}
			
		//Get file description	
		CSisRegistryFileDescription* regFileDes = iApplication->FilesToAdd()[iCurrent++];											
		//Complete actual file installation
		InstallFileL(*regFileDes);
		TUint8 fileFlag(EFileAdded);
		if(regFileDes->Sid().iUid)
			{//if the file has a SID, it can be exe or dll
			if(SecUtils::IsExeL(regFileDes->Target()))
				{//Set file exe flag.
				fileFlag |= Swi::EFileExe;
				}
			else if(SecUtils::IsDllL(regFileDes->Target()))
				{//Set file dll flag.
				fileFlag |= Swi::EFileDll;
				}
			}
				
		//Write the name of the file into the observation file
		CObservationData *event = CObservationData::NewLC(regFileDes->Target(),regFileDes->Sid(),fileFlag);	
		iObserver.AddEventL(*event);
		CleanupStack::PopAndDestroy(event);
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
TBool CRestoreProcessor::DoStateUpdateRegistryL()
	{
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistryWritableSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif
		
	Swi::RSisRegistryEntry entry;
	TBool entryInROM(EFalse);	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt err = entry.Open(iRegistrySession, iApplication->ControllerL().Info().Uid().Uid());
#else
	TInt err = entry.Open(session, iApplication->ControllerL().Info().Uid().Uid());
#endif
	if (err == KErrNone)
		{
		CleanupClosePushL(entry);
		entryInROM = entry.IsInRomL();
		CleanupStack::PopAndDestroy(&entry);
		}

	if (iApplication->IsUpgrade() || iApplication->IsPartialUpgrade() || (iApplication->IsInstall() && entryInROM)) 	
 		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistrySession.UpdateEntryL(*iApplication, iControllerBuffer, iSoftwareTypeRegInfoArray, iStsSession.TransactionIdL());
			}
		else
			{
			iRegistrySession.UpdateEntryL(*iApplication, iControllerBuffer, iStsSession.TransactionIdL());
			}
#else
 		session.UpdateEntryL(*iApplication, iControllerBuffer, iIntegrityServices.TransactionId());
#endif
 		}
 	else
 		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistrySession.AddEntryL(*iApplication, iControllerBuffer, iSoftwareTypeRegInfoArray, iStsSession.TransactionIdL());
			}
		else
			{
			iRegistrySession.AddEntryL(*iApplication, iControllerBuffer, iStsSession.TransactionIdL());
			}
#else
 		session.AddEntryL(*iApplication, iControllerBuffer, iIntegrityServices.TransactionId());
#endif
 		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Registration of MIME types of the software types being installed to AppArc
	InstallSoftwareTypeHelper::RegisterMimeTypesL(iSoftwareTypeRegInfoArray);
#else
	CleanupStack::PopAndDestroy(&session);
#endif
	return ETrue;
	}

TBool CRestoreProcessor::DoStateProcessFilesL()
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{
		
		// determine the temporary file name from the registry description
		// and controller
		CSisRegistryFileDescription* aFileToProcess = iApplication->FilesToAdd()[iCurrent++];
		TParsePtrC parse(aFileToProcess->Target());
		
		_LIT(KTemporaryFileFormat, "%c:%Stfile-%d-%d-%d-");
		TFileName tempFileName;		
		TUint driveCh(iSystemDriveChar); // Can't pass TChar to Format.
		tempFileName.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath, 
			iApplication->ControllerL().Info().Uid().Uid().iUid, iApplication->AbsoluteDataIndex(), aFileToProcess->Index());
		
		// Append a hex representation of a hash of the filename. We need the
		// temporary filename to be effectively unique, but the filename itself
		// may be too long.
		// Use the entire path minus the drive letter because the drive letter can change.
		// For consistency, the target name must be the name from controller not the target
		// after the path substitution code.		
		HBufC* hashBuf = SecUtils::HexHashL(parse.FullName().Right(parse.FullName().Length() - 1));
		tempFileName.Append(*hashBuf);
		delete hashBuf;

		//if the file is an exe or dll, which in restore machine
		//is more or less guarantee'd, then we need to add the hash
		// to /sys/hash
		
  		TEntry entry;
  		TInt err = iFs.Entry(tempFileName, entry);
  		if (err == KErrNone)
  			{
  			// Process the file if the temporary actually exists
  			TInt err = SecurityCheckUtil::ProcessFileL(*iApplication, iFs, iSids,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
  				iStsSession, 
#else
  				iIntegrityServices, 
#endif
				*aFileToProcess, tempFileName);
  			if (err != KErrNone)
  				{
  				User::Leave(KErrCorrupt);
  				}
  				
  			// extract a hash file for this, if the target is /sys/bin	
  			if (parse.Path().CompareF(KBinPath) == 0)
  				{
  				ExtractHashL(*aFileToProcess);
  				}
  			}  			
		SwitchState(ECurrentState);
		return EFalse;		
		}
	else 
		{			
		iCurrent = 0;
		return ETrue;		
		}		
	}
	
TBool CRestoreProcessor::DoStateVerifyPathsL() 
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{		
		CSisRegistryFileDescription* fileToProcess = iApplication->FilesToAdd()[iCurrent++];
		
		SecurityCheckUtil::TProtectedDirectoryCheckError dummyErrorCode;
		// sis file signed by Su Cert are allowed to restore files in private dir 
		// without corresponding executable in the package.
		TBool pathValid = SecurityCheckUtil::CheckProtectedDirectoriesL(
				fileToProcess->Target(),
				fileToProcess->Operation(),
				iSids,
				dummyErrorCode);

		if (!pathValid && !iApplication->IsInstallSuCertBased())
			{
			User::Leave(KErrAccessDenied);
			}	
			
		SwitchState(ECurrentState);
		return EFalse;		
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
void CRestoreProcessor::RunL()
	{
	DEBUG_PRINTF3(_L8("Restore - Restore processing state machine, State: %d, Status: %d"),
		iState, iStatus.Int());
	
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());

	switch(iState)
		{	
	case EInitialize:
		if (DoStateInitializeL())
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
			SwitchState(EInstallFiles);
			}
		break;

	case EInstallFiles:
		if (DoStateInstallFilesL())
			{
			SwitchState(EUpdateRegistry);
			}
		break;

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

void CRestoreProcessor::SwitchState(TProcessingState aNextState)
	{
	if (aNextState!=ECurrentState)
		{
		iState=aNextState;
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

	
void CRestoreProcessor::DoStateFinishedL()
	{
	User::RequestComplete(iClientStatus, KErrNone);
	}

TInt CRestoreProcessor::RunError(TInt aError)
	{
	
	User::RequestComplete(iClientStatus, aError);
	return KErrNone;
	
	}
	
void CRestoreProcessor::ProcessApplicationL(const CApplication& aApplication, TRequestStatus& aClientStatus)
	{
	iApplication = &aApplication;
	iClientStatus = &aClientStatus;
	aClientStatus = KRequestPending;
	
	iState = EInitialize;
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CRestoreProcessor::ProcessPlanL(TRequestStatus& aClientStatus)
	{
	ProcessApplicationL(iPlan.ApplicationL(), aClientStatus);
	}
	
void CRestoreProcessor::DoCancel()
	{
	}

CRestoreProcessor::~CRestoreProcessor()
	{
	delete iFileMan;
	iFs.Close();
	iSids.Close();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iSoftwareTypeRegInfoArray.Close();
#endif
	}
