/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Declares the CXmlParser class for ADM application.
*
*
*/


#include "ADMSisParser.h"
#include "ADMPackageInfo.h"
#include <sisparser.h>
#include <siscontents.h>
#include <siscontroller.h>
#include <sisinfo.h>
#include <sisdependency.h>
#include <sisuid.h>
#include <sisversion.h>
#include <sisprerequisites.h>
#include <sisversionrange.h>
#include <filesisdataprovider.h>
#include <dessisdataprovider.h>
#include <sisversionrange.h>
#include <sisinstallblock.h>
#include <sissupportedlanguages.h>

using namespace Swi;

CSisParser* CSisParser::NewLC()
	{
	CSisParser* object = new ( ELeave ) CSisParser();
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CSisParser* CSisParser::NewL()
	{
	CSisParser* object = CSisParser::NewLC();
	CleanupStack::Pop();
	return object;
	}

CSisParser::CSisParser()
	{
	}

CSisParser::~CSisParser()
	{
	iRfs.Close();
	}

void CSisParser::ConstructL()
	{
	User::LeaveIfError(iRfs.Connect());
	}

//TODO: a http sis data provider would allow partial download?
CPackageInfo* CSisParser::GetDepContentsL(const TDesC& aDepFileName, CPackageInfo *packageInfo,CDepTree* mainTree)
	{
	CFileSisDataProvider *provider = CFileSisDataProvider::NewLC(iRfs, aDepFileName);
	Sis::CContents* contents = Sis::Parser::ContentsL(*provider);
	CleanupStack::PushL(contents);
	HBufC8* controllerpkg = contents->ReadControllerL();
	CleanupStack::PushL(controllerpkg);
	CDesDataProvider* controllerProvider= CDesDataProvider::NewLC(*controllerpkg);
	Sis::CController* controller = CController::NewLC(*controllerProvider);
	SetDependenciesL(packageInfo, mainTree, controller, aDepFileName);

	CleanupStack::PopAndDestroy(controller);
	CleanupStack::PopAndDestroy(controllerProvider);
	CleanupStack::PopAndDestroy(controllerpkg);
	CleanupStack::PopAndDestroy(contents);
	CleanupStack::PopAndDestroy(provider);
	return packageInfo;
	}

void CSisParser::SetDependenciesL(CPackageInfo *packageInfo,CDepTree* mainTree, Sis::CController* controller, const TDesC& aFileName)
	{
	//have now extracted the controller (header information) from the sis file.
	if(mainTree->IsDepTreeEmpty())
		{
		packageInfo = CPackageInfo::NewL();
		packageInfo->iPackageUid = controller->Info().Uid().Uid().iUid;
		packageInfo->iVendor = controller->Info().UniqueVendorName().Data().AllocL();
		packageInfo->iVersion = TVersion(controller->Info().Version().Major(),
				controller->Info().Version().Minor(),
				controller->Info().Version().Build());
		SetLanguageSpecificNamesL(controller, packageInfo);

		packageInfo->iPackageStatus = EPackageRootToBeInstalled;
		//TODO: HACK - ADM expects to use iPackageName as a filename, SIS file contains only UIDs
		packageInfo->iSisPackageName = aFileName.AllocL();
		//TODO: end HACK
		mainTree->AddPackageInfo(packageInfo);
		}

	//Get dependencies
	const RPointerArray<Sis::CDependency> &deps(controller->Prerequisites().Dependencies());
	for(TInt i=0;i<deps.Count();i++)
		{
		const Sis::CDependency* dep(deps[i]);
		TUint32 uid = dep->Uid().Uid().iUid;
		CPackageInfo *dependentPackage = mainTree->LocatePackageInDepTree(uid);
		if(dependentPackage == NULL)
			{
			dependentPackage = CPackageInfo::NewLC();
			dependentPackage->iPackageUid = dep->Uid().Uid().iUid;
			//TODO: HACK - ADM expects to use iPackageName as a filename, SIS file contains only UIDs
			TBuf<8> uidBuf;
			uidBuf.Num(dependentPackage->iPackageUid, EHex);
			dependentPackage->iPackageName = uidBuf.AllocL();
			//TODO: end HACK
			packageInfo->AddEdgeL(dependentPackage);
			mainTree->AddPackageInfo(dependentPackage);
			CleanupStack::Pop(dependentPackage);
			}
		//embedded and outer sis may depend on different version of the same package - in this case, use the higher version number
		const Sis::CVersion& ver = dep->VersionRange()->From();
		if(dependentPackage->iVersion.iMajor < ver.Major() ||
				(dependentPackage->iVersion.iMajor == ver.Major() && dependentPackage->iVersion.iMinor < ver.Minor()) ||
				(dependentPackage->iVersion.iMajor == ver.Major() && dependentPackage->iVersion.iMinor == ver.Minor() && dependentPackage->iVersion.iBuild < ver.Build()))
			{
			dependentPackage->iVersion = TVersion(ver.Major(),
					ver.Minor(),
					ver.Build());
			}
		//also available : dependency names (localised list)
		}

	//Get embedded SIS files' dependencies too
	const RPointerArray<Sis::CController> &embedded = controller->InstallBlock().EmbeddedControllers();
	for(TInt i=0;i<embedded.Count();i++)
		{
		SetDependenciesL(packageInfo, mainTree, embedded[i], KNullDesC);
		}
	}

void CSisParser::SetLanguageSpecificNamesL(Sis::CController* aController, CPackageInfo* aPackageInfo)
	{
	const Sis::CSupportedLanguages &lang = aController->SupportedLanguages();
	TLocale locale;
	TInt langIdx = KErrNotFound;
	// find the language index, so we can access the proper localised
	// names in the sis package
	for (TInt downgradeIdx = -1; downgradeIdx < 3; downgradeIdx++)
		{
		// Go through the language downgrade path
		// See the documentation of TLocale::LanguageDowngrade()
		const TLanguage language = (downgradeIdx == -1)
				? User::Language()
				: locale.LanguageDowngrade(downgradeIdx);
		if (language == ELangNone)
			break;
		for (TInt i = 0; i < lang.Count(); i++)
			{
			if (lang[i] == language)
				{
				langIdx = i;
				break;
				}
			}
		}
	const RPointerArray<Sis::CString>& names = aController->Info().Names();
	// also available: VendorNames()

	// Set localised name for the package
	// Sis package didn't contain localised name, use the default
	if (langIdx != KErrNotFound)
		{
		aPackageInfo->iPackageName = names[langIdx]->Data().AllocL();
		}
	else if (names.Count() > 0)
		{
		aPackageInfo->iPackageName = names[0]->Data().AllocL();
		}
	else
		{
		aPackageInfo->iPackageName = KNullDesC().AllocL();
		}
	}
