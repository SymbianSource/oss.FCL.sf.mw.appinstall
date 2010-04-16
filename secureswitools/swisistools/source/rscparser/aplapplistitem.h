// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// aplapplistitem.h
//
/** 
* @file aplapplistitem.h
*
* @internalComponent
* @released
*/
#ifndef __APLAPPLISTITEM_H__
#define __APLAPPLISTITEM_H__

#include "apaid.h"
#include "barsread2.h"
#include "stringutils.h"
#include "is_utils.h"

/**
The CAppLocalOpaqueDataInfo class represents the data associated with an application OpaqueData.

@internalComponent
*/
class CAppLocalOpaqueDataInfo
	{
public:
	~CAppLocalOpaqueDataInfo();
	CAppLocalOpaqueDataInfo();
	CAppLocalOpaqueDataInfo(TInt 	aLocale, TUint32 aServiceUid, Ptr8* aOpaqueData);
	static CAppLocalOpaqueDataInfo* NewL(TInt aLocale, TUint32 aServiceUid, Ptr8* aOpaqueData);
	Ptr8* GetOpaqueData();
	TUint32 GetServiceUid();
	TInt GetLocale();

private:
	Ptr8* 	iOpaqueData;
	TUint32	iServiceUid;
	TInt 	iLocale;
	};

/**
The CApaAppViewData class represents the data associated with an application view.

@internalComponent
*/
class CAppViewData
	{
public:
	TUid Uid() const;
	inline Ptr16 Caption() const;
public:
	~CAppViewData();
	static CAppViewData* NewL();
	void SetUid(TUid aUid);
	void SetCaptionL(const PtrC16* aCaption);
	void SetScreenMode(TInt aScreenMode);
	void SetIconFileNameL(const Ptr16* aFileName);
	void SetNonMbmIconFile(TBool aNonMbmIconFile);
	void SetNumOfViewIcons(TInt aNumOfViewIcons);
	Ptr16* GetCaption();
	TInt GetScreenMode();
	Ptr16* GetIconFileName();
	TInt GetNumOfViewIcons();
	CAppViewData();

private:
	Ptr16* iCaption;
	TUid iUid;
	TInt iScreenMode;
	Ptr16* iIconFileName;
	TBool iNonMbmIconFile;
	TInt iNumOfViewIcons;
	};

//
// inlines
//

inline Ptr16 CAppViewData::Caption() const
	{ return *iCaption; }

/**
Reads application information from a combination of localizable registration file,

@internalComponent
*/

class CAppLocalizableInfo
{
public:
		~CAppLocalizableInfo();
	static CAppLocalizableInfo* NewL(const std::string& aRegistrationFileName, const std::string& aLocalizeFilePath);
		CAppLocalizableInfo(const std::string& aRegistrationFileName,const std::string& aLocalizeFilePath);
	void SetCaption(Ptr16* Caption);
	void SetShortCaption(Ptr16* ShortCaption);
	void SetGroupName(Ptr16* GroupName);
	void SetIconFileName(Ptr16* IconFileName);
	void SetNumOfAppIcons(TInt NumOfAppIcons);
	void SetLocale(TInt Locale);
	void SetOpaqueData(Ptr8* OpaqueData);
	void ReadViewDataL(RResourceReader& resourceReader);
	Ptr16* ViewDataIconFileNameL(const PtrC16* aIconFileName) const;
	std::vector<CAppViewData*>* GetViewDataArray();	
	Ptr16* GetGroupName();
	Ptr16* GetCaption();
	Ptr16* GetShortCaption();
	Ptr16* GetIconFileName();
	TInt GetNumOfAppIcons();
	TInt GetLocale();
	Ptr8* GetOpaqueData();
	
private:
	std::vector<CAppViewData*>* iViewDataArray;
	Ptr16* iGroupName;
	Ptr16* iCaption;
	Ptr16* iShortCaption;
	TInt iNumOfAppIcons;
	Ptr16* iIconFileName;
	TInt iLocale;
	Ptr8* iOpaqueData;
	const std::string& iRegistrationFilePath;
	const std::string& iLocalPath;
};


#endif //__APLAPPLISTITEM_H__
