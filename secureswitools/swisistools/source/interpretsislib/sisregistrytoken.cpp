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
*
*/


#include "sisregistrytoken.h"
#include "serialiser.h"
#include "deserialiser.h"

SisRegistryToken::SisRegistryToken () : SisRegistryPackage(0, L"", L"", 0),
										iDrives(0),
										iUnused1(0),
										iSids(),
										iVersion(),
										iLanguage(0),
										iUnused2(0),
										iUnused3(0)
	{
	}

SisRegistryToken::SisRegistryToken(
	TUint32 aIndex, 
	const std::wstring& aVendorName, 
	const std::wstring& aPackageName, 
	TUint32 aPackageUid,
	TUint32 aDrive, 
  	TUint32 aLanguage,
	const Version& aVersion,
	const Controllers& aControllerInfos,
	TUint32 aDrives) 
	: SisRegistryPackage(
		aIndex, 
		aVendorName,
		aPackageName,
		aPackageUid),
	  iSelectedDrive(toupper(aDrive)),
	  iDrives(aDrives),
	  iUnused1(0),
	  iVersion(aVersion),
	  iLanguage(aLanguage),
	  iUnused2(0),
	  iUnused3(0)
	{
	SetControllerInfo(aControllerInfos);
	}

SisRegistryToken::~SisRegistryToken ()
	{
	TInt i;
	for (i = 0 ; i < iControllerInfo.size() ; ++i)
		delete iControllerInfo[i];
	}

void SisRegistryToken::Internalize(Deserialiser& des)
	{
	SisRegistryPackage::Internalize(des);
	des >> iDrives
		>> iUnused1
		>> iSids
		>> iControllerInfo
		>> iVersion
		>> iLanguage
		>> iSelectedDrive
		>> iUnused2 
		>> iUnused3;  
	}

void SisRegistryToken::Externalize(Serialiser& ser)
	{
	SisRegistryPackage::Externalize(ser);
	ser << iDrives
		<< iUnused1
		<< iSids
		<< iControllerInfo
		<< iVersion
		<< iLanguage
		<< iSelectedDrive
		<< iUnused2 
		<< iUnused3; 
	}

void SisRegistryToken::StoreControllerInfo(const Controllers& aControllerInfo)
	{
	for (std::vector<ControllerInfo*>::const_iterator curr = aControllerInfo.begin() ;
		 curr != aControllerInfo.end() ;
		 ++curr)
		{
		ControllerInfo* p = new ControllerInfo(**curr);
		p->SetOffset(iControllerInfo.size());
		iControllerInfo.push_back(p);
		}
	}

void SisRegistryToken::SetControllerInfo(const Controllers& aControllerInfo)
	{
	iControllerInfo.resize(aControllerInfo.size());
	Controllers::const_iterator scurr = aControllerInfo.begin();
	Controllers::const_iterator end   = aControllerInfo.end();

	Controllers::iterator dcurr = iControllerInfo.begin();

	while (scurr != end)
		{
		*dcurr++ = new ControllerInfo(**scurr++);
		}
	}





	
