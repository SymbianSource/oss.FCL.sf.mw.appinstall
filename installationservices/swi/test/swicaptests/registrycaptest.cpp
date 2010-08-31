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


/**
 @file
*/

#include "registrycaptest.h"

#include "sisregistrywritablesession.h"
#include "sisregistrywritableentry.h"
#include "sisregistrysession.h"
#include "sisregistrypackage.h"
#include "sisregistryentry.h"
#include "sisrevocationentry.h"
#include "sisregistryserver.h"
#include "application.h"
#include "sistruststatus.h"
#include "sisregistryfiledescription.h"
#include "sisregistrydependency.h"
#include "userselections.h"
#include "siscontroller.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"
#include "cleanuputils.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swtypereginfo.h"
#endif
#include <e32def.h>
#include <f32file.h>

using namespace Swi;

_LIT(KPublicRegistryCapTestName, "Registry public (no caps) interface capability test");
_LIT(KReadUserDataRegistryCapTestName, "Registry public (ReadUserData) interface capability test");
_LIT(KPrivateRegistryCapTestName, "Registry private interface capability test");
_LIT(KDaemonRegistryCapTestName, "Registry daemon interface capability test");
_LIT(KRevocationRegistryCapTestName, "Registry revocation interface capability test");
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
_LIT(KSifServerRegistryCapTestName, "Registry SIF interface security test");
_LIT(KSisRegistryTCBCapTestName, "Sis Registry interface requiring TCB security test");
#endif

CPublicRegistryCapTest* CPublicRegistryCapTest::NewL()
	{
	CPublicRegistryCapTest* self=new(ELeave) CPublicRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPublicRegistryCapTest::CPublicRegistryCapTest()
	{
	}
	
void CPublicRegistryCapTest::ConstructL()
	{
	SetNameL(KPublicRegistryCapTestName);
	}

void CPublicRegistryCapTest::RunTestL()
	{
	Swi::RSisRegistrySession session;
	CleanupClosePushL(session);
		
	TInt err = session.Connect();
	
	if (KErrNone != err)
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	// RSisregistryEntry
	Swi::RSisRegistryEntry entry;
	
// open an entry to do tests on
	TUid uid = { 0x101f7989}; // Package known to exist
	if (KErrNone != CheckFailL(entry.Open(session, uid), _L("Open(TUid)")))
		{
		CleanupStack::PopAndDestroy(&session);
		return;
		}
	CleanupClosePushL(entry);
	
	TRAP(err, entry.IsSignedL());
	CheckFailL(err, _L("RSisregistryEntry::IsSignedL"));

	TRAP(err, entry.IsInRomL());
	CheckFailL(err, _L("RSisregistryEntry::IsInRomL"));
	
	TRAP(err, entry.VersionL());
	CheckFailL(err, _L("RSisregistryEntry::VersionL"));

	TRAP(err, entry.LanguageL());
	CheckFailL(err, _L("RSisregistryEntry::LanguageL"));

	TRAP(err, entry.SelectedDriveL());
	CheckFailL(err, _L("SelectedDriveL"));

	TRAP(err, entry.TrustStatusL());
	CheckFailL(err, _L("RSisregistryEntry::TrustStatusL"));

	HBufC* name=NULL;
	TRAP(err, name = entry.PackageNameL());
	CleanupStack::PushL(name);
	CheckFailL(err, _L("RSisregistryEntry::PackageNameL"));
	CleanupStack::PopAndDestroy(name);
	
	TRAP(err, name= entry.UniqueVendorNameL());
	CleanupStack::PushL(name);
	CheckFailL(err, _L("RSisregistryEntry::UniqueVendorNameL"));
	CleanupStack::PopAndDestroy(name);
	
	TRAP(err, entry.LocalizedVendorNameL());
	CheckFailL(err, _L("RSisregistryEntry::LocalizedVendorNameL"));


	RPointerArray<HBufC8> chains;
	TRAP(err, entry.CertificateChainsL(chains));
	CleanupResetAndDestroyPushL(chains);
	CheckFailL(err, _L("RSisregistryEntry::CertificateChainsL"));
	CleanupStack::PopAndDestroy(&chains);

	TRAP(err, entry.PropertyL(0));
	CheckFailL(err, _L("RSisregistryEntry::PropertyL"));

	Swi::CSisRegistryPackage* package=NULL;
	TRAP(err, package=entry.PackageL());
	delete package;
	CheckFailL(err, _L("RSisregistryEntry::PackageL"));

	RPointerArray<HBufC8> controllers;
	TRAP(err, entry.ControllersL(controllers));
	CleanupResetAndDestroyPushL(controllers);
	CheckFailL(err, _L("RSisregistryEntry::ControllersL"));
	CleanupStack::PopAndDestroy(&controllers);
	
	TUid nonExistentUid ={0};
	TRAP(err, session.IsInstalledL(nonExistentUid));
	CheckFailL(err, _L("RSisRegistrySession::IsInstalledL(TUid)"));

	TBuf8<5> controller;
	TRAP(err, session.IsInstalledL(controller));
	CheckFailL(err, _L("RSisRegistrySession::IsInstalledL(TDesC8&)"));

// These two are unimplemented in the server
//	TRAP(err, session.HashL(_L("C:\\a filename.txt")));
//	CheckFailL(err, _L("RSisRegistrySession::HashL"));
//	TRAP(err, session.ModifiableL(_L("C:\\a filename.txt")));
//	CheckFailL(err, _L("RSisRegistrySession::ModifiableL"));

	TUid sid={0};
	TRAP(err, session.SidToPackageL(sid));
	CheckFailL(err, _L("RSisRegistrySession::SidToPackageL"));
	
	TRAP(err, entry.RemovableL());
	CheckFailL(err, _L("RSisregistryEntry::RemovableL"));	

#ifdef 	SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TRAP(err, session.IsFileRegisteredL(_L("some.file")));
	CheckFailL(err, _L("RSisregistrySession::IsFileRegisteredL"));		

	TRAP(err, session.GetComponentIdForUidL(uid));
	CheckFailL(err, _L("RSisregistrySession::GetComponentIdForUidL"));
#endif

	Swi::RSisRegistryWritableEntry writableEntry;
	
	TUid uidOpen = { 0x101f7989}; // Package known to exist
	
	if (KErrNone != writableEntry.Open(session, uidOpen))
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	CleanupClosePushL(writableEntry);

	RPointerArray<Swi::CSisRegistryFileDescription> fileDescriptions;
	TRAP(err, writableEntry.FileDescriptionsL(fileDescriptions));
	CleanupResetAndDestroyPushL(fileDescriptions);
	CheckFailL(err, _L("FileDescriptionsL"));
	CleanupStack::PopAndDestroy(&fileDescriptions);

	TRAP(err, writableEntry.PreInstalledL());
	CheckFailL(err, _L("PreInstalledL"));

	TRAP(err, writableEntry.IsPresentL());
	CheckFailL(err, _L("RSisregistryEntry::IsPresentL"));

	TRAP(err, writableEntry.IsAugmentationL());
	CheckFailL(err, _L("RSisregistryEntry::IsAugmentationL"));

	TRAP(err, writableEntry.UidL());
	CheckFailL(err, _L("RSisregistryEntry::UidL"));

	RArray<TUid> sids;
	TRAP(err, writableEntry.SidsL(sids));
	CleanupClosePushL(sids);
	CheckFailL(err, _L("RSisregistryEntry::Sids"));
	CleanupStack::PopAndDestroy(&sids);

	RPointerArray<HBufC> files;
	TRAP(err, writableEntry.FilesL(files));
	CleanupResetAndDestroyPushL(files);
	CheckFailL(err, _L("RSisregistryEntry::FilesL"));
	CleanupStack::PopAndDestroy(&files);

	_LIT(KFileName ,"\\a filename.txt");
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KFileName);
	TRAP(err, writableEntry.HashL(fileName));
	CheckFailL(err, _L("RSisregistryEntry::HashL"));

	RPointerArray<HBufC> packageNames;
	RPointerArray<HBufC> vendorNames;
	TRAP(err, writableEntry.AugmentationsL(packageNames, vendorNames));
	CleanupResetAndDestroyPushL(vendorNames);
	CleanupResetAndDestroyPushL(packageNames);
	CheckFailL(err, _L("RSisregistryEntry::AugmentationsL(RPointerArray<HBufC>&,RPointerArray<HBufC>&)"));
	CleanupStack::PopAndDestroy(&packageNames);
	CleanupStack::PopAndDestroy(&vendorNames);

	RPointerArray<Swi::CSisRegistryPackage> packages;
	CleanupResetAndDestroyPushL(packages);
	TRAP(err, writableEntry.AugmentationsL(packages));
	CheckFailL(err, _L("RSisregistryEntry::AugmentationsL(RPointerArray<CSisRegistryPackage>&)"));
	CleanupStack::PopAndDestroy(&packages);

	TRAP(err, writableEntry.SizeL());
	CheckFailL(err, _L("RSisregistryEntry::SizeL"));	
	
	CleanupStack::PopAndDestroy(3, &session);
	}

CReadUserDataRegistryCapTest* CReadUserDataRegistryCapTest::NewL()
	{
	CReadUserDataRegistryCapTest* self=new(ELeave) CReadUserDataRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CReadUserDataRegistryCapTest::CReadUserDataRegistryCapTest()
	{
	SetCapabilityRequired(ECapabilityReadUserData);
	}
	
void CReadUserDataRegistryCapTest::ConstructL()
	{
	SetNameL(KReadUserDataRegistryCapTestName);
	}

void CReadUserDataRegistryCapTest::RunTestL()
	{
	Swi::RSisRegistrySession session;
	CleanupClosePushL(session);
		
	TInt err = session.Connect();
	
	if (KErrNone != err)
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}
	
// RSisRegistrySession tests
	RArray<TUid> uids;
	TRAP(err, session.InstalledUidsL(uids));
	CleanupClosePushL(uids);
	CheckFailL(err, _L("RSisRegistrySession::InstalledUidsL"));
	CleanupStack::PopAndDestroy(&uids);
	
	RPointerArray<Swi::CSisRegistryPackage> packages;
	TRAP(err, session.InstalledPackagesL(packages));
	CleanupResetAndDestroyPushL(packages);
	CheckFailL(err, _L("RSisRegistrySession::InstalledPackagesL"));
	CleanupStack::PopAndDestroy(&packages);

	RPointerArray<Swi::CSisRegistryPackage> removablePackages;
	TRAP(err, session.RemovablePackagesL(removablePackages));
	CleanupResetAndDestroyPushL(removablePackages);
	CheckFailL(err, _L("RSisRegistrySession::RemovablePackagesL"));
	CleanupStack::PopAndDestroy(&removablePackages);
	
	// start debug.
	
	Swi::RSisRegistryWritableEntry entry;
		
	TUid uidOpen = {0x101f7989}; // Package known to exist
	
	if (KErrNone != entry.Open(session, uidOpen))
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	CleanupClosePushL(entry);
		
	TRAP(err, entry.DependentPackagesL(packages));
	packages.ResetAndDestroy();
	CheckFailL(err, _L("DependentPackagesL"));
				
	RPointerArray<Swi::CSisRegistryDependency> dependencies;
	TRAP(err, entry.DependenciesL(dependencies));
	dependencies.ResetAndDestroy();
	CheckFailL(err, _L("DependenciesL"));
	TRAP(err, entry.EmbeddedPackagesL(packages));
	packages.ResetAndDestroy();
	CheckFailL(err, _L("EmbeddedPackagesL"));

	TRAP(err, entry.EmbeddingPackagesL(packages));
	packages.ResetAndDestroy();
	CheckFailL(err, _L("EmbeddingPackagesL"));
	
	CleanupStack::PopAndDestroy(2, &session);
	}



CPrivateRegistryCapTest* CPrivateRegistryCapTest::NewL()
	{
	CPrivateRegistryCapTest* self=new(ELeave) CPrivateRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPrivateRegistryCapTest::CPrivateRegistryCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	SetSidRequired(Swi::KSwisSecureId);
	}
	
void CPrivateRegistryCapTest::ConstructL()
	{
	SetNameL(KPrivateRegistryCapTestName);
	}

void CPrivateRegistryCapTest::RunTestL()
	{
	Swi::RSisRegistryWritableSession session;
	CleanupClosePushL(session);
		
	if (KErrNone != session.Connect())
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	TInt err = 0;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TRAP(err, session.RegenerateCacheL());
	CheckFailL(err, _L("RegenerateCacheL"));
#endif
	
		{
		// setup the application
		Swi::CApplication* app=Swi::CApplication::NewLC();
		TUint64 id=0;
		RFs fs;
		User::LeaveIfError(fs.Connect());
		
		RFile file;
		_LIT(KControllerFile, "z:\\tswi\\swicaptests\\data\\controller");
		User::LeaveIfError(file.Open(fs, KControllerFile, EFileRead | EFileShareReadersOnly));
		CleanupClosePushL(file);
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		HBufC8* buffer = HBufC8::NewLC(fileSize);
		TPtr8 ptr(buffer->Des());
		User::LeaveIfError(file.Read(0, ptr, fileSize));
	
		// Read the controller
		CDesDataProvider* desProvider = CDesDataProvider::NewLC(*buffer);
		Sis::CController* controller = Sis::CController::NewL(*desProvider, Sis::EAssumeType);

		app->SetInstall(*controller);
		app->UserSelections().SetLanguage(ELangEnglish);
		app->UserSelections().SetDrive(2);
		
		TRAP(err, session.AddEntryL(*app, *buffer, id));
		CheckFailL(err, _L("AddEntryL"));
		
		TRAP(err, session.UpdateEntryL(*app, *buffer, id));
		CheckFailL(err, _L("UpdateEntryL"));

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		RCPointerArray<Usif::CSoftwareTypeRegInfo> regInfoArray;
		CleanupClosePushL(regInfoArray);
		
		TRAP(err, session.AddEntryL(*app, *buffer, regInfoArray, id));
		CheckFailL(err, _L("AddEntryL for LEEs"));
		
		TRAP(err, session.UpdateEntryL(*app, *buffer, regInfoArray, id));
		CheckFailL(err, _L("UpdateEntryL for LEEs"));
		
		CleanupStack::PopAndDestroy(&regInfoArray);
#endif

		CleanupStack::PopAndDestroy(desProvider); 
		CleanupStack::PopAndDestroy(buffer); 
		CleanupStack::PopAndDestroy(&file); 
		CleanupStack::PopAndDestroy(app); // app, file, buffer, desProvider, controller
		}

	Swi::RSisRegistryWritableEntry entry;
	
	TUid uidOpen = {0x101f7989}; // Package known to exist
	
	if (KErrNone != entry.Open(session, uidOpen))
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	CleanupClosePushL(entry);

	TRAP(err, entry.InstallTypeL());
	CheckFailL(err, _L("InstallTypeL"));

	CleanupStack::PopAndDestroy(2, &session);
	}

CDaemonRegistryCapTest* CDaemonRegistryCapTest::NewL()
	{
	CDaemonRegistryCapTest* self=new(ELeave) CDaemonRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDaemonRegistryCapTest::CDaemonRegistryCapTest()
	{
	SetSidRequired(Swi::KDaemonSecureId);
	}
	
void CDaemonRegistryCapTest::ConstructL()
	{
	SetNameL(KDaemonRegistryCapTestName);
	}

void CDaemonRegistryCapTest::RunTestL()
	{
	Swi::RSisRegistryWritableSession session;
	CleanupClosePushL(session);
		
	TInt err = session.Connect();
	
	if (KErrNone != err)
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	const TInt KTestDrive=7; // random drive
	TRAP(err, session.AddDriveL(KTestDrive));
	CheckFailL(err, _L("AddDriveL"));

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TRAP(err, session.RemoveDriveL(KTestDrive));
	CheckFailL(err, _L("RemoveDriveL"));
#endif

	TRAP(err, session.RecoverL());
	CheckFailL(err, _L("RecoverL"));
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TRAP(err, session.SetComponentPresenceL(1, EFalse));
	CheckFailL(err, _L("SetComponentPresenceL"));
	
	// Negative tests
	//TRAP(err, session.SetComponentPresenceL(1234, EFalse));
	//CheckFailL(err, _L("SetComponentPresenceL Negative 1"));
    //TRAP(err, session.SetComponentPresenceL(0, EFalse));
    //CheckFailL(err, _L("SetComponentPresenceL Negative 2"));    
    //TRAP(err, session.SetComponentPresenceL(-1, EFalse));
    //CheckFailL(err, _L("SetComponentPresenceL Negative 3"));
#endif

	CleanupStack::PopAndDestroy(&session);
	}


CRevocationRegistryCapTest* CRevocationRegistryCapTest::NewL()
	{
	CRevocationRegistryCapTest* self=new(ELeave) CRevocationRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRevocationRegistryCapTest::CRevocationRegistryCapTest()
	{
	SetCapabilityRequired(ECapabilityNetworkServices);
	SetCapabilityRequired(ECapabilityWriteUserData);
	}
	
void CRevocationRegistryCapTest::ConstructL()
	{
	SetNameL(KRevocationRegistryCapTestName);
	}

void CRevocationRegistryCapTest::RunTestL()
	{
	Swi::RSisRegistryWritableSession session;
	CleanupClosePushL(session);
		
	TInt err = session.Connect();
	
	if (KErrNone != err)
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	Swi::RSisRevocationEntry entry;
	
	TUid uidOpen = {0x101f7989}; // Package known to exist
	
	if (KErrNone != entry.Open(session, uidOpen))
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	CleanupClosePushL(entry);

	TRAP(err, entry.CheckRevocationStatusL(_L8("dummy")));
	CheckFailL(err, _L("CheckRevocationStatus"));

	CleanupStack::PopAndDestroy(2,&session);
	}


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

CSifServerRegistryCapTest* CSifServerRegistryCapTest::NewL()
	{
	CSifServerRegistryCapTest* self=new(ELeave) CSifServerRegistryCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSifServerRegistryCapTest::CSifServerRegistryCapTest()
	{
	SetSidRequired(TUid::Uid(0x10285BCB)); //SIF servers's UID3 (ie: SID)
	}
	
void CSifServerRegistryCapTest::ConstructL()
	{
	SetNameL(KSifServerRegistryCapTestName);
	}

void CSifServerRegistryCapTest::RunTestL()
	{
	Usif::TComponentId componentId(0x01);
	Swi::RSisRegistryWritableSession session;
	CleanupClosePushL(session);
		
	TInt err = session.Connect();
	if (KErrNone != err)
		{
		SetFail();
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	TRAP(err, session.ActivateComponentL(componentId));
	CheckFailL(err, _L("ActivateComponentL"));

	TRAP(err, session.DeactivateComponentL(componentId));
	CheckFailL(err, _L("DeactivateComponentL"));

	CleanupStack::PopAndDestroy(&session);
	}

CSisRegistryTCBCapTest* CSisRegistryTCBCapTest::NewL()
    {
    CSisRegistryTCBCapTest* self=new(ELeave) CSisRegistryTCBCapTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSisRegistryTCBCapTest::CSisRegistryTCBCapTest()
    {
    SetCapabilityRequired(ECapabilityTCB); //API requires TCB capability, so giving it
    }
    
void CSisRegistryTCBCapTest::ConstructL()
    {
    SetNameL(KSisRegistryTCBCapTestName);
    }

void CSisRegistryTCBCapTest::RunTestL()
    {
    Swi::RSisRegistrySession registrySession;
    CleanupClosePushL(registrySession);
    _LIT(regFileName,"c:\\private\\10003a3f\\import\\apps\\dummy_reg.rsc");
    TInt err = registrySession.Connect();
    if (KErrNone != err)
        {
        SetFail();
        CleanupStack::PopAndDestroy(&registrySession);
        return;
        }

    TRAP(err, registrySession.AddAppRegInfoL(regFileName));
    CheckFailL(err, _L("AddAppRegInfoL"));
    
    TRAP(err, registrySession.RemoveAppRegInfoL(regFileName));
    CheckFailL(err, _L("RemoveAppRegInfoL"));

    CleanupStack::PopAndDestroy(&registrySession);
    }
#endif


