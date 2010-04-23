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
* @file
* This file provides the class declaration of CPackageInfo.
* @internalComponent
*
*/

 
 
#ifndef _PACKAGE_INFO_H_
#define _PACKAGE_INFO_H_

#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <swi/sisregistryentry.h>

// Forward declaration
class TUid;

class CPackageInfo:public CBase
	{
public: // Public Member Functions
	~CPackageInfo();
	void DisplayPackageL(const TUid& aPackageUID);
	void ListInstalledAppsL();
	Swi::CSisRegistryPackage* GetSisRegistryPackageL(const TDesC& aPackageName, const TDesC& aVendorName);
	Swi::CSisRegistryPackage* GetSisRegistryPackageL(const TUid& aPackageUID, const TDesC& aPackageName, const TDesC& aVendorName);
	Swi::CSisRegistryPackage* GetSisRegistryPackageL(const TUid& aPackageUID);
	
	// Static member functions
	static CPackageInfo* NewLC();
	// Static member functions
	static CPackageInfo* NewL();

private: // Private Member Functions
	// Default Constructor
	CPackageInfo() {}
	// Copy Constructor
	CPackageInfo(const CPackageInfo&){}
	void ConstructL();
	
	void DisplayPackageL(const Swi::CSisRegistryPackage& aSisRegistry);
	
	TBool CheckPkgAndVendor(const Swi::CSisRegistryPackage& aSisRegistry, const TDesC& aPackageName, const TDesC& aVendorName);

private: // Private Member Variables
	Swi::RSisRegistrySession iRegistrySession;
	};

#endif //_PACKAGE_INFO_H_