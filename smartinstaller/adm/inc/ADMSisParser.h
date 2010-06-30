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


#ifndef __ADMSISPARSER_H__
#define __ADMSISPARSER_H__

#include <e32base.h>
#include <f32file.h>

// Forward Declaration
class CPackageInfo;
class CDepTree;
namespace Swi {
namespace Sis {
class CController;
}
}

class CSisParser: public CBase
	{
public:
	static CSisParser* NewL();
	static CSisParser* NewLC();
	~CSisParser();

	CPackageInfo* GetDepContentsL(const TDesC& aDepFileName, CPackageInfo *packageInfo,CDepTree* mainTree);

public:

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
		EDeviceNotSupported,
		};

private:
	void ConstructL();
	CSisParser();
	void SetDependenciesL(CPackageInfo* aPackageInfo, CDepTree* mainTree, Swi::Sis::CController* controller, const TDesC& aFileName);
	void SetLanguageSpecificNamesL(Swi::Sis::CController* aController, CPackageInfo* aPackage);
private:
	RFs iRfs;
	};

#endif __ADMSISPARSER_H__
