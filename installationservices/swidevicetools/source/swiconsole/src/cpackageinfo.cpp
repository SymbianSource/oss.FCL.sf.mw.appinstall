/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CPackageInfo: This class is used to display details about installed
* packages.
* @internalComponent
*
*/

 

#include "cpackageinfo.h"
#include "ciohandler.h"
#include <swiconsole.rsg>
  
#include "cleanuputils.h" // CleanupResetAndDestroyPushL
#include "swiutility.h"		// CleanupResetPushL

const TInt KPackageUidStringLen = 20;
const TInt KPackageLinesTobeDisplayed = 4;
_LIT(KUidDisplayFormat, "0x%08x");
_LIT(KTabCharacter, "\t");

// Extern variable defined in main.cpp
extern CIoHandler* gIoHandler;


CPackageInfo* CPackageInfo::NewLC()
	{
	CPackageInfo *self = new (ELeave) CPackageInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPackageInfo* CPackageInfo::NewL()
	{
	CPackageInfo *self = CPackageInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CPackageInfo::ConstructL()
	{
	User::LeaveIfError(iRegistrySession.Connect());
	}

CPackageInfo::~CPackageInfo()
	{
	iRegistrySession.Close();
	}

void CPackageInfo::DisplayPackageL(const Swi::CSisRegistryPackage& aPackage)
	{
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackage.Uid() ));
	CleanupClosePushL(entry);

	RPointerArray<Swi::CSisRegistryPackage> packages;
	CleanupResetAndDestroyPushL(packages);
	entry.AugmentationsL(packages);
	if(!gIoHandler->IsDisplayableInPage(KPackageLinesTobeDisplayed))
		{
		gIoHandler->PauseScreenL();
		gIoHandler->ClearConsoleL();
		}
	gIoHandler->WriteL(KLineBreaker, EIoConsoleType);
	gIoHandler->WriteL(R_PKG_NAME_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(aPackage.Name(), EIoConsoleType);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(aPackage.Vendor(), EIoConsoleType);
	gIoHandler->WriteL(R_PKG_UID_MSG, EIoConsoleType);
	TBuf<KPackageUidStringLen> pkgUid;
	pkgUid.Format(KUidDisplayFormat, aPackage.Uid());
	gIoHandler->WriteLineL(pkgUid, EIoConsoleType);
	TInt augmentationCount = packages.Count();
	if(augmentationCount)
		{
		gIoHandler->WriteToPageL(KLineBreaker, EIoConsoleType);
		gIoHandler->WriteToPageL(R_PKG_AUGUMENTATIONS_MSG, EIoConsoleType);
		}
	
	for(int i = 0; i < augmentationCount; ++i)
		{
		if(!gIoHandler->IsDisplayableInPage(KPackageLinesTobeDisplayed))
			{
			gIoHandler->PauseScreenL();
			gIoHandler->ClearConsoleL();
			}
		gIoHandler->WriteL(KTabCharacter, EIoConsoleType);
		gIoHandler->WriteL(R_PKG_NAME_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(packages[i]->Name(), EIoConsoleType);
		gIoHandler->WriteL(KTabCharacter, EIoConsoleType);
		gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(aPackage.Vendor(), EIoConsoleType);
		gIoHandler->WriteL(KTabCharacter, EIoConsoleType);
		gIoHandler->WriteL(R_PKG_UID_MSG, EIoConsoleType);
		TBuf<KPackageUidStringLen> pkgUid;
		pkgUid.Format(KUidDisplayFormat, packages[i]->Uid());
		gIoHandler->WriteLineL(pkgUid, EIoConsoleType);
		}
	
	CleanupStack::PopAndDestroy(2, &entry);
	}

void CPackageInfo::DisplayPackageL(const TUid& aPackageUid)
	{
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackageUid));
	CleanupClosePushL(entry);
	
	gIoHandler->WriteL(KLineBreaker, EIoConsoleType);
	gIoHandler->WriteLineL(KStringLine, EIoConsoleType);
	
	Swi::CSisRegistryPackage* registryPackage = entry.PackageL();
	CleanupStack::PushL(registryPackage);
	DisplayPackageL(*registryPackage);
	
	CleanupStack::PopAndDestroy(2, &entry);
	}


void CPackageInfo::ListInstalledAppsL()
	{
	RArray<TUid> packageUids;
	CleanupResetPushL(packageUids);
	
	iRegistrySession.InstalledUidsL(packageUids);
	
	TInt nCount = packageUids.Count();
	
	gIoHandler->ClearConsoleL();
	gIoHandler->WriteLineL(KStringLine, EIoConsoleType);
	
	for(TInt i = 0; i < nCount; i++)
		{
		DisplayPackageL(packageUids[i]);
		}
	gIoHandler->WriteLineL(KStringLine, EIoConsoleType);
	CleanupStack::PopAndDestroy(&packageUids);
	}
	
Swi::CSisRegistryPackage* CPackageInfo::GetSisRegistryPackageL(
							const TDesC& aPackageName, 
							const TDesC& aVendorName)
	{
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackageName, aVendorName));
	CleanupClosePushL(entry);
	
	Swi::CSisRegistryPackage* registryPackage = entry.PackageL();
	
	CleanupStack::PopAndDestroy(&entry);
	
	return registryPackage;
	}

TBool CPackageInfo::CheckPkgAndVendor(
							const Swi::CSisRegistryPackage& aSisRegistry, 
							const TDesC& aPackageName, 
							const TDesC& aVendorName)
	{
	if(	0 == aPackageName.CompareF(aSisRegistry.Name()) && 
		0 == aVendorName.CompareF(aSisRegistry.Vendor()))
		{
		return ETrue;
		}
	return EFalse;
	}
Swi::CSisRegistryPackage* CPackageInfo::GetSisRegistryPackageL(
							const TUid& aPackageUid,
							const TDesC& aPackageName, 
							const TDesC& aVendorName)
	{
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackageUid));
	CleanupClosePushL(entry);
	
	Swi::CSisRegistryPackage* registryPackage = entry.PackageL();
	CleanupStack::PushL(registryPackage);

	if(!CheckPkgAndVendor(*registryPackage, aPackageName, aVendorName))
		{
		RPointerArray<Swi::CSisRegistryPackage> packages;
		CleanupResetAndDestroyPushL(packages);
		entry.AugmentationsL(packages);
		TInt i = packages.Count() - 1;
		
		for(; i >= 0; --i)
			{
			if(CheckPkgAndVendor(*packages[i], aPackageName, aVendorName))
				{
				break;
				}
			}
		
		if(i < 0)
			{
			User::Leave(KErrNotFound);
			}
		
		Swi::CSisRegistryPackage* temp = Swi::CSisRegistryPackage::NewL(*packages[i]);
		
		CleanupStack::PopAndDestroy(&packages);
		CleanupStack::PopAndDestroy(registryPackage);	
		registryPackage = temp;
		CleanupStack::PushL(registryPackage);
		}
	
	CleanupStack::Pop(registryPackage);
	CleanupStack::PopAndDestroy(&entry);
	
	return registryPackage;
	}


Swi::CSisRegistryPackage* CPackageInfo::GetSisRegistryPackageL(
						const TUid& aPackageUid)
	{
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackageUid));
	CleanupClosePushL(entry);
	
	Swi::CSisRegistryPackage* registryPackage = entry.PackageL();
	
	CleanupStack::PopAndDestroy(&entry);
	
	return registryPackage;
	}

