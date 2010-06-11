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
* Definition of the CApplication
*
*/


/**
 @file 
*/

#include "application.h"
#include "userselections.h"
#include "sisregistryfiledescription.h"
#include "siscontroller.h"
#include "sisregistrypackage.h"
#include "hashcontainer.h"
#include "secutils.h"
#include "securityinfo.h"

using namespace Swi;


EXPORT_C CApplication* CApplication::NewL()
	{
	CApplication* self=NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApplication* CApplication::NewLC()
	{
	CApplication* self = new (ELeave) CApplication();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// CApplication

CApplication::CApplication() 
	: iShutdown(EFalse),
	  iCanPropagate(ETrue),
	  iPropagateDrive('!'),
	  iDeletablePreinstalled(EFalse)
	{
	}

CApplication::~CApplication()
	{
	// do not destroy, these are only pointers, there are not owned by this class
	iProperties.Reset();

	// owned by this class
	iFilesToAdd.ResetAndDestroy();
	iFilesToDisplayOnInstall.ResetAndDestroy();
	iFilesToRunOnInstall.ResetAndDestroy();
	iFilesToRemove.ResetAndDestroy();
	iFilesToDisplayOnUninstall.ResetAndDestroy();
	iFilesToRunOnUninstall.ResetAndDestroy();
	iFilesToSkipOnInstall.ResetAndDestroy();
	
	iEmbeddedApplications.ResetAndDestroy();
	iDeviceSupportedLanguages.Close();
	iMatchingDeviceLanguages.Close();
	delete iUserSelections;
	delete iUid;
	delete iPackage;
	}


void CApplication::ConstructL()
	{
	iUserSelections = CUserSelections::NewL();
	}


EXPORT_C void CApplication::AddSisStubFileL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	User::LeaveIfError(iFilesToAdd.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C void CApplication::AddFileL(const Sis::CFileDescription& aFileDescription, TChar aDrive)
	{
	aDrive.Fold();
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription, aDrive, iIsStub);
	User::LeaveIfError(iFilesToAdd.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	
	// Update the propagation status
	
	if (iCanPropagate)
		{
		TChar drive = fileDescription->Target()[0];
		drive.Fold();
		if (iPropagateDrive != '!')
			{
			// This is not the first file description, check the drive letter against the first
			iCanPropagate = (iPropagateDrive == drive);
			}
		else
			{
			// This is the first file description, just update the drive
			iPropagateDrive = (drive == '!' ? aDrive : drive);
			}
		}
	
	}

EXPORT_C void CApplication::RemoveFileL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	User::LeaveIfError(iFilesToRemove.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C void CApplication::RemoveFileL(const TDesC& aFileName)
	{
	// Create dummy CSisRegistryFileDescription for a file to be removed based
	// solely on the file name.

	CHashContainer* hashContainer = CHashContainer::NewLC(CMessageDigest::ESHA1, KNullDesC8);

	Sis::TSISFileOperationOptions options = static_cast<Sis::TSISFileOperationOptions>(0);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	// If the file is an EXE get the SID otherwise use KNullUid
	TUid sid(KNullUid);
	if (SecUtils::IsExeL(aFileName))
		{
		TSecurityInfo info;
		SecurityInfo::RetrieveExecutableSecurityInfoL(fs, aFileName, info);
		sid.iUid = info.iSecureId.iId;
		}
	
	// get the file length, this is used later for the progress bar	
	TEntry entry;
	User::LeaveIfError(fs.Entry( aFileName, entry ));	
	TInt64 length = entry.iSize;
		
	CSisRegistryFileDescription* fileDescription = 
		CSisRegistryFileDescription::NewL(*hashContainer,
											aFileName,
											KNullDesC,
											Sis::EOpNull,
											options,
											length,
											0,
											sid);
	CleanupStack::PushL(fileDescription);
	User::LeaveIfError(iFilesToRemove.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	CleanupStack::PopAndDestroy(2);  		// fs, hashContainer
	}


EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToAdd() const
	{
	return iFilesToAdd;
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToRemove() const
	{
	return iFilesToRemove;
	}

EXPORT_C void CApplication::RunFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription, aDrive, iIsStub);
	User::LeaveIfError(iFilesToRunOnInstall.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C void CApplication::RunFileOnUninstallL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	User::LeaveIfError(iFilesToRunOnUninstall.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToRunOnInstall() const
	{
	return iFilesToRunOnInstall;
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToRunOnUninstall() const
	{
	return iFilesToRunOnUninstall;
	}

EXPORT_C void CApplication::DisplayFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription, aDrive, iIsStub);
	User::LeaveIfError(iFilesToDisplayOnInstall.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C void CApplication::DisplayFileOnUninstallL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	User::LeaveIfError(iFilesToDisplayOnUninstall.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToDisplayOnInstall() const
	{
	return iFilesToDisplayOnInstall;
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToDisplayOnUninstall() const
	{
	return iFilesToDisplayOnUninstall;
	}

EXPORT_C TBool CApplication::IsUpgrade() const
	{
	return iFlags & EUpgrade;
	}

EXPORT_C void CApplication::SetUpgrade(const Sis::CController& aController)
	{
	iFlags |= EUpgrade;
	SetController(aController);
	}

EXPORT_C TBool CApplication::IsInstall() const
	{
	return iFlags & EInstall;
	}

EXPORT_C void CApplication::SetInstall(const Sis::CController& aController)
	{
	iFlags |= EInstall;
	SetController(aController);
	}

EXPORT_C TBool CApplication::IsUninstall() const
	{
	return iFlags & EUninstall;
	}

EXPORT_C void CApplication::SetUninstallL(const CSisRegistryPackage& aPackage)
	{
	iFlags |= EUninstall;
	SetPackageL(aPackage);
	}

EXPORT_C TBool CApplication::IsAugmentation() const
	{
	return iFlags & EAugmentation;
	}

EXPORT_C void CApplication::SetAugmentation(const Sis::CController& aController)
	{
	iFlags |= EAugmentation;
	SetController(aController);
	}

EXPORT_C TBool CApplication::IsPartialUpgrade() const
	{
	return iFlags & EPartialUpgrade;
	}

EXPORT_C void CApplication::SetPartialUpgrade(const Sis::CController& aController)
	{
	iFlags |= EPartialUpgrade;
	SetController(aController);
	}
	
EXPORT_C TBool CApplication::IsPreInstalledPatch() const
	{
	return iFlags & EPreInstalledPatch;
	}

EXPORT_C void CApplication::SetPreInstalledPatch(const Sis::CController& aController)
	{
	iFlags |= EPreInstalledPatch;
	SetController(aController);
	}

EXPORT_C void CApplication::SetPreInstalledPatch()
 	{
 	iFlags |= EPreInstalledPatch;
 	}

EXPORT_C TBool CApplication::IsPreInstalledApp() const
	{
	return iFlags & EPreInstalledApp;
	}

EXPORT_C void CApplication::SetPreInstalledApp(const Sis::CController& aController)
	{
	iFlags |= EPreInstalledApp;
	SetController(aController);
	}

EXPORT_C void CApplication::SetPreInstalledApp()
 	{
 	iFlags |= EPreInstalledApp;
 	}

EXPORT_C TBool CApplication::IsInROM(void) const
	{
	return iFlags & EInRom;
	}

EXPORT_C void CApplication::SetInROM(void)
	{
	iFlags |= EInRom;
	}
	
EXPORT_C void CApplication::AddPropertyL(const TProperty& aProperty)
	{
	User::LeaveIfError(iProperties.Append(aProperty));
	}

EXPORT_C const RArray<CApplication::TProperty>& CApplication::Properties() const
	{
	return iProperties;
	}

EXPORT_C const CSisRegistryPackage& CApplication::PackageL() const
	{
	if (!iPackage)
		{
		User::Leave(KErrGeneral);
		}
	return *iPackage;
	}

EXPORT_C void CApplication::SetPackageL(const CSisRegistryPackage& aPackage)
	{
	delete iPackage;
	iPackage=0;
	iPackage=CSisRegistryPackage::NewL(aPackage);
	}

EXPORT_C const Sis::CController& CApplication::ControllerL(void) const
	{
	if (!iController)
		{
		User::Leave(KErrGeneral);
		}
	return (*iController);
	}

EXPORT_C void CApplication::SetController(TUid aUid)
	{
	delete iUid;
	iUid=new(ELeave) TUid(aUid);
	}   
	

EXPORT_C void CApplication::SetController(const Sis::CController& aController)
	{
	iController=&aController;
	}


EXPORT_C const RPointerArray<CApplication>& CApplication::EmbeddedApplications() const
	{
	return iEmbeddedApplications;
	}

EXPORT_C void CApplication::AddEmbeddedApplicationL(CApplication* aApplication)
	{
	User::LeaveIfError(iEmbeddedApplications.Append(aApplication));
	}
	
EXPORT_C TBool CApplication::CanPropagate() const
	{
	return iCanPropagate && (iPropagateDrive != '!');
	}
	
EXPORT_C TChar CApplication::StubDrive() const
	{
	return iPropagateDrive;
	}
	
EXPORT_C void CApplication::CopyDeviceSupportedLanguagesL(const RArray<TInt> &aDeviceSupportedLanguages)
	{
	TInt size = aDeviceSupportedLanguages.Count();
	while (size--)
		User::LeaveIfError(iDeviceSupportedLanguages.Append(aDeviceSupportedLanguages[size]));
	}

EXPORT_C void CApplication::PopulateMatchingDeviceLanguagesL(TInt aLanguageId)
	{
	TRAPD(error,iMatchingDeviceLanguages.InsertInOrderL(aLanguageId))
	if ( error != KErrNone && error != KErrAlreadyExists)
		{
		User::Leave(error);
		}
	}

EXPORT_C const RArray<TInt>& CApplication::GetMatchingDeviceLanguages(void)
	{
	return iMatchingDeviceLanguages;
	}

EXPORT_C TBool CApplication::IsDeviceMatchingLanguage(TInt aLangId)const
	{
		TInt found = iDeviceSupportedLanguages.Find(aLangId);
		if ( found == KErrNotFound )
			{						
			return EFalse;
			}
		
		return ETrue;
     }

 EXPORT_C TBool CApplication::IsInstallSuCertBased() const
	{
	return iFlags & EInstallSuCertBased;	
	}
	
EXPORT_C void CApplication::SetInstallSuCertBased()
	{
	iFlags |= EInstallSuCertBased;	
	}

EXPORT_C const RPointerArray<CSisRegistryFileDescription>& CApplication::FilesToSkipOnInstall() const
	{
	return iFilesToSkipOnInstall;
	}
	
EXPORT_C void CApplication::SkipFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription, aDrive, iIsStub);
	User::LeaveIfError(iFilesToSkipOnInstall.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
	}

EXPORT_C void CApplication::AddFileL(const CSisRegistryFileDescription& aFileDescription)
	{
	CSisRegistryFileDescription* fileDescription = CSisRegistryFileDescription::NewLC(aFileDescription);
	User::LeaveIfError(iFilesToAdd.Append(fileDescription));
	CleanupStack::Pop(fileDescription);
		
	// Update the propagation status
	
	if (iCanPropagate)
		{
		TChar drive = fileDescription->Target()[0];
		drive.Fold();
		if (iPropagateDrive != '!')
			{
			// This is not the first file description, check the drive letter against the first
			iCanPropagate = (iPropagateDrive == drive);
			}
		else
			{
			// This is the first file description, just update the drive
			iPropagateDrive = drive;
			}
		}
		
	}

EXPORT_C void CApplication::SetSisHelper(const RSisHelper& aSisHelper)
	{
	iSisHelper = &aSisHelper;
	}

EXPORT_C RSisHelper& CApplication::GetSisHelper()
	{
	return (const_cast <RSisHelper&> (*iSisHelper));
	}


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void CApplication::SetDrmProtected(TBool aIsDrmProtected)
	{
	iIsDrmProtected = aIsDrmProtected;
	}
#else
EXPORT_C void CApplication::SetDrmProtected(TBool /*aIsDrmProtected*/)
	{
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	}
#endif

EXPORT_C TBool CApplication::IsDrmProtected() const
	{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	return iIsDrmProtected;
#else
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	return EFalse;
#endif
	}

