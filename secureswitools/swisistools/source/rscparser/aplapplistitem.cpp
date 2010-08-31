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
// aplapplistitem.cpp
//
/** 
* @file aplapplistitem.cpp
*
* @internalComponent
* @released
*/
#include "aplapplistitem.h"
#include "barsc2.h"
#include "parse.h"
#include "uidtype.h"

//
// class CAppLocalOpaqueDataInfo
//

//Default Constructor for class CAppLocalOpaqueDataInfo
CAppLocalOpaqueDataInfo::CAppLocalOpaqueDataInfo()
	: iServiceUid(0),
	  iLocale(0),
	  iOpaqueData(NULL)
{
}

//Destructor for class CAppLocalOpaqueDataInfo
CAppLocalOpaqueDataInfo::~CAppLocalOpaqueDataInfo()
{
	delete iOpaqueData;
}

//Constructor for class CAppLocalOpaqueDataInfo
CAppLocalOpaqueDataInfo::CAppLocalOpaqueDataInfo(TInt aLocale, TUint32 aServiceUid, Ptr8* aOpaqueData)
	: iLocale(aLocale),
	  iServiceUid(aServiceUid),
	  iOpaqueData(aOpaqueData)
{
	assert(iOpaqueData);
}


CAppLocalOpaqueDataInfo* CAppLocalOpaqueDataInfo::NewL(TInt aLocale, TUint32 aServiceUid, Ptr8* aOpaqueData)
{
	CAppLocalOpaqueDataInfo* self=new CAppLocalOpaqueDataInfo(aLocale, aServiceUid, aOpaqueData);
	if(NULL==self)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	return self;	
}


Ptr8* CAppLocalOpaqueDataInfo::GetOpaqueData()
{
	return iOpaqueData;
}

TUint32 CAppLocalOpaqueDataInfo::GetServiceUid()
{
	return iServiceUid;
}

TInt CAppLocalOpaqueDataInfo::GetLocale()
{
	return iLocale;
}

//
// Class CAppLocalizableInfo
//
CAppLocalizableInfo::~CAppLocalizableInfo()
{
	delete iCaption;
	delete iShortCaption;
	delete iGroupName;
	delete iIconFileName;
	if(iViewDataArray)
		iViewDataArray->clear();
	delete iViewDataArray;
	delete iOpaqueData;
}

CAppLocalizableInfo::CAppLocalizableInfo(const std::string& aRegistrationFileName, const std::string& aLocalizeFilePath)
	: iNumOfAppIcons(0),
	iRegistrationFilePath(aRegistrationFileName),
	iLocalPath(aLocalizeFilePath)
{
	iCaption = NULL;
	iShortCaption = NULL;
	iIconFileName = NULL;
	iGroupName = NULL;
	iViewDataArray = NULL;
	iOpaqueData = NULL;
}

CAppLocalizableInfo* CAppLocalizableInfo::NewL(const std::string& aRegistrationFileName,const std::string& aLocalizeFilePath)
{
	CAppLocalizableInfo* self=new CAppLocalizableInfo(aRegistrationFileName, aLocalizeFilePath);
	if(NULL==self)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	return self;	
}


void CAppLocalizableInfo::SetCaption(Ptr16* Caption)
{	
	iCaption = Caption;
}
void CAppLocalizableInfo::SetShortCaption(Ptr16* ShortCaption)
{
	iShortCaption = ShortCaption;
}
void CAppLocalizableInfo::SetGroupName(Ptr16* GroupName)
{
	iGroupName = GroupName;
}
void CAppLocalizableInfo::SetIconFileName(Ptr16* IconFileName)
{
	iIconFileName = IconFileName;
}

void CAppLocalizableInfo::SetNumOfAppIcons(TInt NumOfAppIcons)
{
	iNumOfAppIcons = NumOfAppIcons;
}

void CAppLocalizableInfo::SetLocale(TInt Locale)
{
	iLocale = Locale;
}

void CAppLocalizableInfo::SetOpaqueData(Ptr8* OpaqueData)
{
	iOpaqueData = OpaqueData;
}

std::vector<CAppViewData*>* CAppLocalizableInfo::GetViewDataArray()
{
	return iViewDataArray;
}
Ptr16* CAppLocalizableInfo::GetGroupName()
{
	return iGroupName;
}

Ptr16* CAppLocalizableInfo::GetCaption()
{
	return iCaption;
}

Ptr16* CAppLocalizableInfo::GetShortCaption()
{
	return iShortCaption;
}

Ptr16* CAppLocalizableInfo::GetIconFileName()
{
	return iIconFileName;
}

TInt CAppLocalizableInfo::GetNumOfAppIcons()
{
	return iNumOfAppIcons;
}

TInt CAppLocalizableInfo::GetLocale()
{
	return iLocale;
}	

Ptr8* CAppLocalizableInfo::GetOpaqueData()
{
	return iOpaqueData;
}

//Read ViewData struct of Localizable resource file.
void CAppLocalizableInfo::ReadViewDataL(RResourceReader& aResourceReader)
{
	// read LEN WORD STRUCT view_list[]
	const TInt numOfViews = aResourceReader.ReadInt16L();

	if (numOfViews > 0)
	{
		iViewDataArray = new std::vector<CAppViewData*>;
		if(NULL==iViewDataArray)
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
	}

	for(TInt view = 0; view < numOfViews; ++view)
	{
		CAppViewData* viewData = CAppViewData::NewL();
		aResourceReader.ReadUint32L(); // skip over LONG reserved_long
		aResourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// read LONG uid
		const TUid viewUid =  {aResourceReader.ReadInt32L()};
		viewData->SetUid(viewUid);
		// read LONG screen_mode
		const TInt screenMode = aResourceReader.ReadInt32L();
		viewData->SetScreenMode(screenMode);

		aResourceReader.ReadUint32L(); // skip over LONG reserved_long
		aResourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// read LTEXT caption
		PtrC16* viewCaption = aResourceReader.ReadTPtrCL();

		if(NULL != viewCaption)
		{
			viewData->SetCaptionL(viewCaption);

			viewCaption->iPtr = NULL; //To Avoid double delete in destructor. To be destroyed by RResourceReader.
			delete viewCaption;
		}
		// read WORD number_of_icons
		const TInt numOfViewIcons = aResourceReader.ReadInt16L();
		viewData->SetNumOfViewIcons(numOfViewIcons);

		// read LTEXT icon_file
		PtrC16* viewIconFile = aResourceReader.ReadTPtrCL();

		if(NULL != viewIconFile)
		{
			ConvertToPlatformSpecificPath(viewIconFile->iPtr, viewIconFile->iMaxLength);
			Ptr16*	fullViewIconFileName = ViewDataIconFileNameL(viewIconFile);
			if (fullViewIconFileName)
			{
				viewData->SetIconFileNameL(fullViewIconFileName);
				viewData->SetNonMbmIconFile(true);
				delete fullViewIconFileName;
			}
			else
			{
				viewIconFile->iPtr = NULL;
				viewIconFile->iMaxLength = 0;

				if (numOfViewIcons > 0 && iIconFileName)
				{ // default to app icon filename
					viewIconFile->iPtr = iIconFileName->GetPtr(); 
					viewIconFile->iMaxLength = iIconFileName->GetLength();
				std::string errMsg= "Failed : ServiceInfo Icon File not present in Device.";
				throw CResourceFileException(errMsg);
				}
			}
		}

		iViewDataArray->push_back(viewData);
		if(NULL != viewIconFile)
		{
			viewIconFile->iPtr = NULL; //To Avoid double delete in destructor. To be destroyed by RResourceReader.
		}
		delete viewIconFile;
	}

}


Ptr16* CAppLocalizableInfo::ViewDataIconFileNameL(const PtrC16* aIconFileName) const
{
	Ptr16* filename = NULL;
	if (aIconFileName->iMaxLength == 0)
		return NULL;

	/*
	 * aIconFileName may contain a valid string in some format (for eg. URI format) other than path to a regular file on disk
	 * and that can be a mbm or non-mbm file. Such a filename will be reported as invalid filename by iFs.IsValidName() method. 
	 * aIconFileName will be returned since it is a valid string. 
	 */	
	ParsePtrC parsePtr(aIconFileName);

	if ( parsePtr.IsWild() || !parsePtr.PathPresent() || !parsePtr.NamePresent() )
	{
		parsePtr.SetToNull();
		return NULL;
	}

	filename = new Ptr16(aIconFileName->iMaxLength);
	if(NULL==filename || NULL == filename->GetPtr())
	{
		parsePtr.SetToNull();
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	filename->UpdateLength(aIconFileName->iMaxLength);
	BufCpy(filename->GetPtr(),aIconFileName->iPtr,aIconFileName->iMaxLength);

	std::wstring iFileName = Ptr16ToWstring(filename);
	std::wstring iFilePath = string2wstring(iLocalPath);
	std::string fullnamewitoutdrive = parsePtr.FullNameWithoutDrive();
	std::wstring iFilename1 = string2wstring(fullnamewitoutdrive);

	if(parsePtr.DrivePresent())
		iFilePath.append(iFilename1);
	else
		iFilePath.append(iFileName);

	// check for fully qualified icon filename
	if (parsePtr.DrivePresent() && FileExists(iFileName))
	{
		parsePtr.SetToNull();
		return filename;	
	}
	else
	{
		// check for icon file on same drive as localisable resource file
		std::string localisableResourceFileDrive = parsePtr.Drive();
		TInt ret = FindWild(aIconFileName);
		if(ret == 0 && FileExists(iFilePath))
		{
			parsePtr.SetToNull();
			return filename;
		}
		else
		{
			ParsePtrC parsePtr_reg(iRegistrationFilePath);
			std::string registrationFileDrive = parsePtr_reg.Drive();

			if(registrationFileDrive.compare(localisableResourceFileDrive))
			{
				// check for icon file on same drive as registration file
				std::string registrationfilepath(parsePtr_reg.FullPath());
				registrationfilepath.append(parsePtr.NameAndExt());
				std::wstring iRegFilePath = string2wstring(registrationfilepath);
				std::string iconfile = parsePtr.StrName();
			
				ret = FindWild(registrationFileDrive,iconfile,localisableResourceFileDrive);
				if(ret == 0 && FileExists(iRegFilePath))
				{
					parsePtr_reg.SetToNull();
					parsePtr.SetToNull();
					return filename;
				}
			}
			parsePtr_reg.SetToNull();
		}
	}
	parsePtr.SetToNull();	//To Avoid double delete in destructor. To be destroyed by RResourceReader.
							//Same as iIconFileName data member of class CAppInfoReader. Destructor will delete.
	return NULL;					// Or to be deleted by calling function.
}

//
// Class CAppViewData
//

CAppViewData::~CAppViewData()
{
	delete iCaption;
	delete iIconFileName;
}

CAppViewData::CAppViewData()
	: iNonMbmIconFile(false),iNumOfViewIcons(0),iScreenMode(0)
{
	iCaption = NULL;
	iIconFileName = NULL;
	iUid = NullUid;
}

CAppViewData* CAppViewData::NewL()
{
	CAppViewData* self=new CAppViewData();
	if(NULL==self)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	return self;	
}

void CAppViewData::SetUid(TUid aUid)
{
	iUid=aUid;
}

void CAppViewData::SetScreenMode(TInt aScreenMode)
{
	iScreenMode=aScreenMode;
}

void CAppViewData::SetCaptionL(const PtrC16* aCaption)
{
	if(aCaption != NULL)
	{
		iCaption=new Ptr16(aCaption->iMaxLength);
		if(NULL==iCaption || NULL==iCaption->GetPtr())
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		iCaption->UpdateLength(aCaption->iMaxLength);
		BufCpy(iCaption->GetPtr(),aCaption->iPtr,aCaption->iMaxLength);
	}
	else
		iCaption = NULL;
}

void CAppViewData::SetIconFileNameL(const Ptr16* aFileName)
{
	if(aFileName != NULL)
	{
		iIconFileName = new Ptr16(aFileName->GetLength());
		if(NULL==iIconFileName || NULL==iIconFileName->GetPtr())
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		iIconFileName->UpdateLength(aFileName->GetLength());
		BufCpy(iIconFileName->GetPtr(),aFileName->GetPtr(),aFileName->GetLength());
	}
	else
		iIconFileName = NULL;
}

void CAppViewData::SetNumOfViewIcons(TInt aNumOfViewIcons)
{
	iNumOfViewIcons = aNumOfViewIcons;
}

void CAppViewData::SetNonMbmIconFile(TBool aNonMbmIconFile)
{
	iNonMbmIconFile = aNonMbmIconFile;
}

TUid CAppViewData::Uid() const
{
	return iUid;
}

Ptr16* CAppViewData::GetCaption()
{
	return iCaption;
}
TInt CAppViewData::GetScreenMode()
{
	return iScreenMode;
}
Ptr16* CAppViewData::GetIconFileName()
{
	return iIconFileName;
}
TInt CAppViewData::GetNumOfViewIcons()
{
	return iNumOfViewIcons;
}
