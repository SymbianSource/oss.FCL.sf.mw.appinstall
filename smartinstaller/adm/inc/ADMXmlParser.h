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


#ifndef __ADMXMLPARSER_H__
#define __ADMXMLPARSER_H__

#include <e32base.h>
#include <f32file.h>

// Mandatory depfile xml elements
_LIT8(KFieldPackageUid, "PackageUid");
_LIT8(KFieldDepVersion, "Version");

// Optional depfile xml elements
_LIT8(KFieldDependency, "Dependency");
_LIT8(KFieldDepPackageUid, "Uid");
_LIT8(KFieldDepPackageName, "Name");
_LIT8(KFieldDepPackageVersion, "Version");
_LIT8(KFieldDepVendor, "Vendor");
_LIT8(KFieldDepUrl, "Url");
_LIT8(KFieldVersionInformation,"Information");
_LIT8(KFieldInstallPackageName,"InstallPackageName");

// Mandatory changes xml elements
_LIT8(KFieldDepFileName,"DependentFileName");
_LIT8(KFieldDateOfSubmission,"DateOfSubmission");
_LIT8(KFieldDateOfModification,"DateOfModification");
_LIT8(KFieldDownloadSize,"DownloadSize");
_LIT8(KFieldInstalledSize,"InstalledSize");
_LIT8(KFieldDescription,"Description");

_LIT8(KFieldSupportedDevice,"SupportedDevice");
_LIT8(KFieldDeviceName,"Name");
_LIT8(KFieldDeviceMachineId,"MachineId");
_LIT8(KFieldDeviceVersion,"Version");

_LIT8(KFieldDate,"Date");
_LIT8(KFieldDay,"Day");
_LIT8(KFieldMonth,"Month");
_LIT8(KFieldYear,"Year");

_LIT8(KFieldDrivePriority,"DrivePriority");
_LIT8(KFieldMandatoryInstallDrive,"MandatoryInstallDrive");

//<!ELEMENT Package ( Name, Uid, Version, Vendor, Dependencies?, DateOfSubmission, DateOfModification, SupportedDevices, Size, Url*, Description?)>

// Forward Declaration
class CPackageInfo;
class CSenXmlReader;
class CSenDomFragment;
class CDepTree;
class CSenElement;

enum XmlParserError
	{
	EErrorXmlNotWelformed,
	EMissingPackageUid,
	EMissingPackageName,
	EMissingPackageVersion,
	EMissingVendorName,
	EMissingPackageUrl,
	EInvalidPackageUid,
	EIncorrectPackageUid,
	EMissingDepVersion,
	EInvalidVersion,
	EInvalidMachineID,
	EMissingDepFileName,
	EMissingSisFileName,
	EMissingSubmissionDate,
	EMissingModificationDate,
	EMissingDownloadSize,
	EMissingInstalledSize,
	EMissingMachineID,
	EUnsupportedDevice,
	EInvalidDrivePriority,
	};

class CXmlParser: public CBase
	{
public:
	static CXmlParser* NewL();
	static CXmlParser* NewLC();
	~CXmlParser();

	CPackageInfo* GetDepContentsL(const TDesC& aDepFileName, CPackageInfo *packageInfo,CDepTree* mainTree);
	void GetChangesInfoL(const TDesC& aChangesFileName, CPackageInfo *packageInfo,CDepTree* mainTree);

private:
	void ConstructL();
	CXmlParser();
	TInt StringToInt(const TDesC8& aString, TUint32& aInt);
	void SetDependenciesL(CPackageInfo* aPackageInfo, CDepTree* mainTree);
	void GetVersionInfoL(CPackageInfo *packageInfo,TUint32 machineId);
	void  SetDate(const TDesC8& aDatePtr);
	TBool checkMachineIdL(CSenElement *versionInfo,TUint32 machineId);
	void InitL();

private:
	CSenXmlReader* iXmlReader;
	CSenDomFragment* iDomFragment;
	RFs iRfs;
	};

#endif __ADMXMLPARSER_H__
