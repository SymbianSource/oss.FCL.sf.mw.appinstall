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
*     CXmlParser implementation
*
*
*/


#include <SenDomFragment.h>
#include <SenXmlReader.h>
#include <SenElement.h>
#include "ADMXmlParser.h"
#include "ADMPackageInfo.h"
#include "macros.h"
#include "config.h"

_LIT(KSisExtension,".sis");

CXmlParser* CXmlParser::NewLC()
	{
	CXmlParser* object = new ( ELeave ) CXmlParser();
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CXmlParser* CXmlParser::NewL()
	{
	CXmlParser* object = CXmlParser::NewLC();
	CleanupStack::Pop();
	return object;
	}

CXmlParser::CXmlParser()
	:iXmlReader(NULL),
	iDomFragment(NULL)
	{
	}

CXmlParser::~CXmlParser()
	{
	iRfs.Close();

	if ( iXmlReader )
		{
		delete iXmlReader;
		iXmlReader = NULL;
		}
	if ( iDomFragment )
		{
		delete iDomFragment;
		iDomFragment = NULL;
		}
	}

void CXmlParser::ConstructL()
	{
	User::LeaveIfError(iRfs.Connect());
	}

void CXmlParser::InitL()
	{
	// Do the cleanup as this function could be
	// called multiple times.
	if ( iXmlReader )
	   {
	   delete iXmlReader;
	   iXmlReader = NULL;
	   }

	if ( iDomFragment )
	   {
	   delete iDomFragment;
	   iDomFragment = NULL;
	   }

	iXmlReader = CSenXmlReader::NewL(EErrorOnUnrecognisedTags);
	iDomFragment = CSenDomFragment::NewL();

	iXmlReader->SetContentHandler(*iDomFragment);
	iDomFragment->SetReader(*iXmlReader);
	}

void CXmlParser::GetChangesInfoL(const TDesC& aChangesFileName, CPackageInfo *packageInfo,CDepTree* mainTree)
	{
	InitL();

	iXmlReader->ParseL(iRfs, aChangesFileName);

	// Get package uid
	// Leave if package uid not present
	CSenElement* packageElement;
	packageElement = iDomFragment->AsElement().Element(KFieldDepPackageUid);
	if (packageElement == NULL)
		{
		User::Leave(EMissingPackageUid);
		}
	TPtrC8 packageUidPtr = packageElement->Content();
	TUint32 packageUid=0;
	if (StringToInt(packageUidPtr,packageUid) != KErrNone)
		{
		User::Leave(EInvalidPackageUid);
		}

	//Check whether the packageId received in changes file is same as the one already stored in the packageinfo from dependency info.
	//If there is mismatch, exit. If it matches, proceed further.
	if(packageUid != packageInfo->iPackageUid)
		{
		User::Leave(EIncorrectPackageUid);
		}

	packageElement = iDomFragment->AsElement().Element(KFieldDepPackageName);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iPackageName );
		packageInfo->iPackageName = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingPackageName);
		}

	// Get dependent package vendor
	packageElement = iDomFragment->AsElement().Element(KFieldDepVendor);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iVendor );
		packageInfo->iVendor = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingVendorName);
		}

	// Get Priority for the drive
	//This is optional.Hence no Leave for Missing Priority.
	//Default is 0.
	packageElement = iDomFragment->AsElement().Element(KFieldDrivePriority);
	if(packageElement)
		{
		TPtrC8 drivePtr = packageElement->Content();
		if (StringToInt(drivePtr,packageInfo->iDrivePriority) != KErrNone)
			{
			User::Leave(EInvalidDrivePriority);
			}
		}

	// Get the mandatory drive.
	//This is optional.Hence no Leave for Missing info.
	//Default is none.
	packageElement = iDomFragment->AsElement().Element(KFieldMandatoryInstallDrive);
	if(packageElement)
		{
		packageInfo->iMandatoryInstallDrive = packageElement->ContentUnicodeL()->Des()[0];
		}

	GetVersionInfoL(packageInfo,mainTree->GetMachineId());
	}

void CXmlParser::GetVersionInfoL(CPackageInfo *packageInfo,TUint32 machineId)
	{
	RPointerArray<CSenElement> versionInfo;
	CleanupClosePushL(versionInfo);

	iDomFragment->AsElement().ElementsL(versionInfo,KFieldVersionInformation);
	const TInt versionInfoCount(versionInfo.Count());
	TVersion latestVersion(0,0,0);
	TInt latestVersionCount = -1;
	CSenElement* packageElement;

	//Changes file will contain different versions of the package along
	//with the list of supporrted devices for that perticular version.

	//If no version was specified in the dep file, then the latest should be read.
	//While reading the latest, it should be confirmed whether the same supports the given device.

	for (TInt i=0; i<versionInfoCount; ++i)
		{
		packageElement = versionInfo[i]->Element(KFieldDepPackageVersion);
		if (packageElement == NULL)
			{
			User::Leave(EMissingDepVersion);
			}

		TPtrC8 versionPtr = packageElement->Content();
		TVersion ver;
		if (SetVersion(versionPtr, ver) == EFalse)
			{
			User::Leave(EInvalidVersion);
			}

		//Check whether the MachineID of this device is supported for this corresponding version in the changes file.
		if(checkMachineIdL(versionInfo[i],machineId))
			{
			if((CompareVersions(packageInfo->iVersion,ver) == EEqualVersion)||(CompareVersions(packageInfo->iVersion,ver) == EGreaterSecondVersion))
				{
				//Version specified in the dep file of parent matches the version in the changes file which inturn matches to the MachineID. :)
				//Or there exists a greater version in changes file.
				latestVersion = ver;
				latestVersionCount = i;
				}
			}
		}//end of for loop

	if(latestVersionCount == -1)
		{
		//No matching Machine IDs at all for any version specified in the changes file.
		User::Leave(EUnsupportedDevice);
		}
	else
		{
		packageInfo->iVersion = latestVersion;
		}

	// Get dependent package URL
	packageElement= versionInfo[latestVersionCount]->Element(KFieldDepUrl);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iUrl );
		TPtrC8 urlPtr = packageElement->Content();

		HBufC8* url = urlPtr.AllocL();

		packageInfo->iUrl = url;
		}
	else
		{
		User::Leave(EMissingPackageUrl);
		}


	packageElement = versionInfo[latestVersionCount]->Element(KFieldDepFileName);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iDepFileName );
		packageInfo->iDepFileName = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingDepFileName);
		}

	packageElement = versionInfo[latestVersionCount]->Element(KFieldInstallPackageName);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iSisPackageName );
		packageInfo->iSisPackageName = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingSisFileName);
		}

	packageElement = versionInfo[latestVersionCount]->Element(KFieldDateOfSubmission);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iDateOfSubmission );
		packageInfo->iDateOfSubmission = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingSubmissionDate);
		}
	//TODO: There needs to be a separate function to convert the Date format into a string format.
	//Call that function here.
	packageElement = versionInfo[latestVersionCount]->Element(KFieldDateOfModification);
	if (packageElement)
		{
		DELETE_IF_NONNULL( packageInfo->iDateOfModification );
		packageInfo->iDateOfModification = packageElement->ContentUnicodeL();
		}
	else
		{
		User::Leave(EMissingModificationDate);
		}

	packageElement = versionInfo[latestVersionCount]->Element(KFieldDownloadSize);
	TPtrC8 downloadSizePtr = packageElement->Content();
	TUint32 downloadSize=0;
	if (StringToInt(downloadSizePtr,downloadSize) != KErrNone)
		{
		User::Leave(EInvalidPackageUid);
		}
	if(downloadSize)
		{
		packageInfo->iDownloadSize = downloadSize;
		}
	else
		{
		User::Leave(EMissingDownloadSize);
		}

	packageElement = versionInfo[latestVersionCount]->Element(KFieldInstalledSize);
	TPtrC8 installedSizePtr = packageElement->Content();
	TUint32 installedSize=0;
	if (StringToInt(installedSizePtr,installedSize) != KErrNone)
		{
		User::Leave(EInvalidPackageUid);
		}
	if(installedSize)
		{
		packageInfo->iInstalledSize = installedSize;
		}
	else
		{
		User::Leave(EMissingInstalledSize);
		}
	CleanupStack::PopAndDestroy(&versionInfo);
}

TBool CXmlParser::checkMachineIdL(CSenElement *versionInfo, TUint32 machineId)
	{
	RPointerArray<CSenElement> deviceInfo;
	CleanupClosePushL(deviceInfo);

	versionInfo->ElementsL(deviceInfo,KFieldSupportedDevice);
	const TInt deviceInfoCount(deviceInfo.Count());

	CSenElement* packageElement;

	// See if the device is supported by this
	// particular version, by comparing the list
	// of machine ids supported by the version
	// with the machine id of device. .
	for(TInt i=0;i<deviceInfoCount;i++)
		{
		packageElement = deviceInfo[i]->Element(KFieldDeviceMachineId);

		if (packageElement == NULL)
			{
			User::Leave(EMissingMachineID);
			}
		TPtrC8 packageUidPtr = packageElement->Content();
		TUint32 changesMachineId = 0;
		if (StringToInt(packageUidPtr,changesMachineId) != KErrNone)
			{
			User::Leave(EInvalidMachineID);
			}
		if(changesMachineId == machineId)
			{
			CleanupStack::PopAndDestroy(&deviceInfo);
			return ETrue;
			}
		}
	CleanupStack::PopAndDestroy(&deviceInfo);
	return EFalse;
	}

CPackageInfo* CXmlParser::GetDepContentsL(const TDesC& aDepFileName, CPackageInfo *packageInfo, CDepTree* aMainTree)
	{
	InitL();

	iXmlReader->ParseL(iRfs, aDepFileName);

	// Get package uid
	// Leave if package uid not present
	CSenElement* packageElement;
	packageElement = iDomFragment->AsElement().Element(KFieldPackageUid);
	if (packageElement == NULL)
		{
		User::Leave(EMissingPackageUid);
		}
	TPtrC8 packageUidPtr = packageElement->Content();
	TUint32 packageUid=0;
	if (StringToInt(packageUidPtr,packageUid) != KErrNone)
		{
		User::Leave(EInvalidPackageUid);
		}

	User::LeaveIfNull(aMainTree);
	if(aMainTree->IsDepTreeEmpty())
		{
		// Add root package(first package) to the list
		packageInfo = CPackageInfo::NewL();
		packageInfo->iPackageUid = packageUid;
		packageInfo->iPackageStatus = EPackageRootToBeInstalled;
		aMainTree->AddPackageInfo(packageInfo);
		}
	else
		{
		User::LeaveIfNull(packageInfo);
		if(packageInfo->iPackageUid != packageUid)
			{
			//the package pointer doesn't match the depfile
			User::Leave(EInvalidPackageUid);
			}
		}

	// Get Dep version
	// Leave if Dep version is not present
	packageElement = iDomFragment->AsElement().Element(KFieldDepVersion);
	if (packageElement == NULL)
		{
		User::Leave(EMissingDepVersion);
		}
	else
		{
		TPtrC8 versionPtr = packageElement->Content();
		TVersion ver;
		if (SetVersion(versionPtr, ver) == EFalse)
			{
			User::Leave(EInvalidVersion);
			}
		packageInfo->iDepVersion = ver;
		}

	// Parse the dependencies
	SetDependenciesL(packageInfo, aMainTree);

	return packageInfo;
	}

void CXmlParser::SetDependenciesL(CPackageInfo* aPackageInfo, CDepTree* aMainTree)
	{
	// Get dependencies
	RPointerArray<CSenElement> dependency;
	CleanupClosePushL(dependency);

	iDomFragment->AsElement().ElementsL(dependency,KFieldDependency);
	const TInt DependencyCount(dependency.Count());

	User::LeaveIfNull(aMainTree);
	User::LeaveIfNull(aPackageInfo);


	TBool IsNonRootNodeDep = 1;

	// Check if the dependencies are of the root node.
	// If yes, then clear the flag
	if (aMainTree->GetRootNode() == aPackageInfo)
		{
		IsNonRootNodeDep = 0;
		}

	for (TInt i=0; i<DependencyCount; ++i)
		{
		// Get dependency UID
		CSenElement* packageElement;
		TUint32 depPackageUid = 0;
		packageElement = dependency[i]->Element(KFieldDepPackageUid);
		if (packageElement == NULL)
			{
			User::Leave(EMissingPackageUid);
			}
		TPtrC8 depPackageUidPtr = packageElement->Content();
		if ( StringToInt(depPackageUidPtr,depPackageUid) != KErrNone )
			{
			User::Leave(EInvalidPackageUid);
			}

		// Check if the dependency already present in the tree.
		CPackageInfo* depPackageInfo = aMainTree->LocatePackageInDepTree(depPackageUid);

		// If dependency is listed in the tree,
		// create a new edge for it
		if (depPackageInfo != NULL)
			{
			aPackageInfo->AddEdgeL(depPackageInfo);
			continue;
			}

		// Dependency is not already present, so create a new node
		depPackageInfo = CPackageInfo::NewL();
		aMainTree->AddPackageInfo(depPackageInfo);

		depPackageInfo->iPackageUid = depPackageUid;
		aPackageInfo->AddEdgeL(depPackageInfo);

		// Get dependent package name
		packageElement = dependency[i]->Element(KFieldDepPackageName);
		if (packageElement)
			{
			depPackageInfo->iPackageName = packageElement->ContentUnicodeL();
			}
		else if(IsNonRootNodeDep)
			{
			User::Leave(EMissingPackageName);
			}
		else
			{
			depPackageInfo->iPackageName = HBufC::NewL(
												depPackageUidPtr.Length() +
												KSisExtension().Length());

			TPtr depPackageName(depPackageInfo->iPackageName->Des());
			depPackageName.Copy(depPackageUidPtr);
			depPackageName.Append(KSisExtension);
			}


		// Get dependent package version
		packageElement = dependency[i]->Element(KFieldDepPackageVersion);

		if(packageElement)
			{
			TPtrC8 versionPtr = packageElement->Content();
			TVersion ver;
			if (SetVersion(versionPtr, ver) == EFalse)
				{
				User::Leave(EInvalidVersion);
				}
			depPackageInfo->iVersion = ver;
			}
		else if(IsNonRootNodeDep)
			{
			User::Leave(EMissingPackageVersion);
			}

		// Get dependent package vendor
		packageElement = dependency[i]->Element(KFieldDepVendor);
		if (packageElement)
			{
			depPackageInfo->iVendor = packageElement->ContentUnicodeL();
			}
		else if(IsNonRootNodeDep)
			{
			User::Leave(EMissingVendorName);
			}

#ifdef url_support
		// Get dependent package URL
		packageElement = dependency[i]->Element(KFieldDepUrl);

		if (packageElement)
			{
			TPtrC8 urlPtr = packageElement->Content();
			HBufC8* url = urlPtr.AllocL();

			TPtr8 urlPtr2 = url->Des();
			urlPtr2.Copy(urlPtr);

			depPackageInfo->iUrl = url;
			}
		else if(IsNonRootNodeDep)
			{
			User::Leave(EMissingPackageUrl);
			}
#else
		// Ignore the Url(if specified) for the root node.
		if(IsNonRootNodeDep)
			{
			// Get dependent package URL
			packageElement = dependency[i]->Element(KFieldDepUrl);
			if (packageElement)
				{
				depPackageInfo->iUrl = packageElement->Content().AllocL();
				}
			else
				{
				User::Leave(EMissingPackageUrl);
				}
			}
#endif
		//Set the changes file name
		depPackageInfo->SetChangesFileNameL();
		}
	CleanupStack::PopAndDestroy(&dependency);
	}

TInt CXmlParser::StringToInt(const TDesC8& aString, TUint32& aInt)
	{
	 // Function will return EFalse if string conversion fails.
	 TLex8 lex(aString);
	 lex.SkipSpace();

	 if (lex.Peek() == '0')
		 {
		 lex.Inc();
		 TChar c( lex.Peek() );
		 c.LowerCase();
		 if (c == 'x')
			 {
			 lex.Inc();
			 return ( lex.Val(aInt,EHex) );
			 }
		 else
			 {
			 lex.UnGet();
			 }
		 }
	 return ( lex.Val(aInt,EDecimal) );
	}
