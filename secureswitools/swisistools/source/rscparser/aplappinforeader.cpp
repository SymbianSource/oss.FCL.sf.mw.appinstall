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
// aplappinforeader.cpp
//
/** 
* @file aplappinforeader.cpp
*
* @internalComponent
* @released
*/

#include <stdlib.h> 
#include <iostream.h> 
#include <string> 
#include "aplappinforeader.h"
#include "barsc2.h"
#include "barsread2.h"
#include <cassert>
#include "parse.h"
#include "uidtype.h"
#include "stringutils.h"
#include "is_utils.h"
#include "apsecutils.h"
#include "parse.h"

using namespace std;

const TUint KNonLocalized = 0;

#define REINTERPRET_CAST(type,exp) (reinterpret_cast<type>(exp))

const TUint KResourceOffsetMask = 0xFFFFF000;

#ifdef __LINUX__
std::string KAppBinaryPathAndExtension("/sys/bin/.exe");
#else
std::string KAppBinaryPathAndExtension("\\sys\\bin\\.exe");
#endif

const TInt KAppRegistrationInfoResourceId = 1;

// The 2nd UID that defines a resource file as being an application registration resource file.
const TUid KUidAppRegistrationFile = {0x101F8021};

/**
@internalTechnology
*/
std::string KAppResourceFileExtension(".rsc");

#ifdef __LINUX__
std::string KLitPathForUntrustedRegistrationResourceFiles("/private/10003a3f/import/apps/");
#else
std::string KLitPathForUntrustedRegistrationResourceFiles("\\private\\10003a3f\\import\\apps\\");
#endif

//
// CAppInfoReader
//

// The behaviour of the Take-methods of this class is a little non-standard, as it
// transfers ownership of the pointer owned by a CAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).


CAppInfoReader* CAppInfoReader::NewL(const std::string& aRegistrationFileName, TUid aAppUid, const std::string& aLocalizeFilePath)
{
	CAppInfoReader* self = new CAppInfoReader(aRegistrationFileName, aAppUid, aLocalizeFilePath);
	if(NULL == self)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	return self;
}

CAppInfoReader::CAppInfoReader(const std::string& aRegistrationFileName, TUid aAppUid, const std::string& aLocalizeFilePath) :
	iAppUid(aAppUid),
	iDefaultScreenNumber(0),
	iNonMbmIconFile(false),
	iApplicationLanguage(ELangNone),
	iIndexOfFirstOpenService(KErrNotFound),
	iRegistrationFileName(aRegistrationFileName),
	iDrivePath(aLocalizeFilePath)
	{

	 	iAppBinaryFullName = NULL;;
		iCapability.iGroupName = NULL;
		iCaption = NULL;
		iShortCaption = NULL;
		iViewDataArray = NULL;;
		iOwnedFileArray = NULL;
		iIconFileName = NULL;
	 	iLocalisableResourceFileName = NULL;
		iServiceArray = NULL;
	}

CAppInfoReader::~CAppInfoReader()
{
	delete iAppBinaryFullName;
	delete iCaption;
	delete iShortCaption;
	if(iViewDataArray)
		iViewDataArray->clear();
	delete iViewDataArray;
	if(iOwnedFileArray)
		iOwnedFileArray->clear();
	delete iOwnedFileArray;
	delete iIconFileName;
	delete iLocalisableResourceFileName;
	if(iServiceArray)
		iServiceArray->clear();
	delete iServiceArray;
	if(iAppLocalizableInfo.size())
		iAppLocalizableInfo.clear();
	if(iOpaqueDataArray.size())
		iOpaqueDataArray.clear();
}

Ptr16* CAppInfoReader::AppBinaryFullName()
{
	Ptr16* fileName = iAppBinaryFullName;
	return fileName;
}

TUidType CAppInfoReader::AppBinaryUidType() const
{
	return iAppBinaryUidType;
}

TUid CAppInfoReader::AppUid() const
{
	return iAppUid;
}

void CAppInfoReader::Capability(TAppCapability& aCapabilityBuf) const
{
	memcpy(&aCapabilityBuf, &iCapability,sizeof(TAppCapability));
}

TUint CAppInfoReader::DefaultScreenNumber() const
{
	return iDefaultScreenNumber;
}

Ptr16* CAppInfoReader::Caption()
{
	Ptr16* caption = iCaption;
	iCaption = NULL; // ownership transferred to caller
	return caption;
}

Ptr16* CAppInfoReader::ShortCaption()
{
	Ptr16* shortCaption = iShortCaption;
	iShortCaption = NULL; // ownership transferred to caller
	return shortCaption;
}

TInt CAppInfoReader::NumOfAppIcons() const
{
	return iNumOfAppIcons;
}

std::vector<CAppViewData*>* CAppInfoReader::Views()
{
	std::vector<CAppViewData*>* viewDataArray = iViewDataArray;
	iViewDataArray = NULL; // ownership transferred to caller
	return viewDataArray;
}

std::vector<CAppLocalizableInfo*> CAppInfoReader::LocalizableInfo()
{
	std::vector<CAppLocalizableInfo*> AppLocalizableInfo = iAppLocalizableInfo;
	return AppLocalizableInfo;
}

std::vector<CAppLocalOpaqueDataInfo*> CAppInfoReader::GetOpaqueDataArray()
{
	std::vector<CAppLocalOpaqueDataInfo*> opaqueDataArray = iOpaqueDataArray;
	return opaqueDataArray;
}

std::vector<Ptr16*>* CAppInfoReader::OwnedFiles()
{
	std::vector<Ptr16*>* ownedFileArray = iOwnedFileArray;
	iOwnedFileArray = NULL; // ownership transferred to caller
	return ownedFileArray;
}

Ptr16* CAppInfoReader::IconFileName()
{
	Ptr16* iconFileName = iIconFileName;
	iIconFileName = NULL; // ownership transferred to caller
	ConvertToPlatformSpecificPath(iconFileName->GetPtr(), iconFileName->GetLength());
	return iconFileName;
}

TBool CAppInfoReader::NonMbmIconFile() const
{
	return iNonMbmIconFile;
}

Ptr16* CAppInfoReader::LocalisableResourceFileName()
{
	Ptr16* localisableResourceFileName = iLocalisableResourceFileName;
	iLocalisableResourceFileName = NULL; // ownership transferred to caller
	return localisableResourceFileName;
}

TLanguage CAppInfoReader::AppLanguage() const
{
	return iApplicationLanguage;
}

std::vector<TAppServiceInfo*>* CAppInfoReader::ServiceArray(TInt& aIndexOfFirstOpenService)
{
	std::vector<TAppServiceInfo*>* serviceArray = iServiceArray;
	iServiceArray = NULL;
	aIndexOfFirstOpenService = iIndexOfFirstOpenService;
	return serviceArray;
}

TBool CAppInfoReader::ReadL(const std::vector<FileDescription*>& aFileDescription, std::string& aRomPath, int aInRom )
{
		TUint fileOffset = 0;
		TInt fileLength = 0;
		TUid firstUid(KExecutableImageUidVal);
		TUid middleUid(KUidApp);
		TInt err;
		TInt LocalizeError = 0;
		TUid reguid;
		CResourceFile* registrationFile = new CResourceFile(iRegistrationFileName, fileOffset, fileLength);
		if(NULL==registrationFile)
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
	
		if(registrationFile)
		{
			iAppUid = registrationFile->ReadAppUidL();
			if(!iAppUid.GetUid())
			{
				std::string errMsg= "Failed : Invalid Resource File. Null Application UID.";
				throw CResourceFileException(errMsg);
			}

			reguid = registrationFile->ReadFileUidL();
			if(reguid.GetUid()!= KUidAppRegistrationResourceFile)
			{
				std::string errMsg= "Failed : Invalid Resource File. UID2 is not defined.";
				throw CResourceFileException(errMsg);
			}
		}
	
		// set the TUidType for the app binary
		// cannot read the TEntry info from the app binary because it's in \sys\bin
		iAppBinaryUidType = TUidType(firstUid, middleUid, iAppUid); 
		
		RResourceReader resourceReader;
		resourceReader.OpenL(registrationFile, KAppRegistrationInfoResourceId); 
	
		TUint localisableResourceId = 1; // only initialising this here to keep the compiler happy, as it's concerned that the variable might be used without having been initialised. The variable should be initialised later, before it's used
	
		try {
				ReadMandatoryInfoL(resourceReader);
			
				err = ReadNonLocalisableInfoL(resourceReader, localisableResourceId);
	
				if (!err)
				{	
					err = ReadNonLocalisableOptionalInfoL(resourceReader, registrationFile);
				}
			}
			catch(const CResourceFileException& aObject)
			{
			
				delete registrationFile;
				cout<< aObject.GetMsg()<< endl;
				return EFalse; // might have read something, but failed to setup enough info to make it worthwhile trying to read any more
			}
		
		TBool useDefaultIcons = ETrue;
	
		if(iLocalisableResourceFileName)
		{
			std::string localizeFileName = Ptr16ToString(iLocalisableResourceFileName);

			std::vector<FileDescription*>::const_iterator filedesIter;
			//std::wstring iFile;
			std::wstring iLocalizeFile;
			//std::string aFileName;
			std::string iLocalFilePath;

			std::string iLocalFileExt(".r");
			
			for(filedesIter = aFileDescription.begin() ; filedesIter != aFileDescription.end(); ++filedesIter)
			{
				iLocalizeFile = (*filedesIter)->GetLocalFile();
			
				iLocalFilePath = wstring2string(iLocalizeFile);

				if((iLocalFilePath.find(localizeFileName,0) == std::wstring::npos) 
						|| (iLocalFilePath.find(iLocalFileExt,0) == std::wstring::npos))
					continue;

				size_t found;
				std::string Locale;
				int iLocale = 0;
			
				found=iLocalFilePath.find_last_of(".");
				if(found)
					Locale = iLocalFilePath.substr(found+2);

				iLocale = atoi(Locale.c_str()); 
				if(!iLocale)
					iLocale = KNonLocalized;
			
				CResourceFile* tlocalisableFile = NULL;

				if(aInRom)
				{
					std::string LocalFile = FullNameWithoutDrive(iLocalFilePath);
					iLocalFilePath = aRomPath + LocalFile;
				}

				try {
					// open the localisable resource file	
					tlocalisableFile = new CResourceFile(iLocalFilePath, 0, 0);
					if(NULL==tlocalisableFile)
					{
						std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
						throw CResourceFileException(errMsg);
					}
	
					if (!err)
					{
						if (tlocalisableFile && (localisableResourceId & KResourceOffsetMask))
							tlocalisableFile->ConfirmSignatureL();

						err = ReadLocalisableInfoLoopL(*tlocalisableFile, localisableResourceId, useDefaultIcons, iLocale);
					}
						
					delete tlocalisableFile;
				}
				catch(const CResourceFileException& aObject)
				{
					LocalizeError = 1;
					aObject.Display();
					delete tlocalisableFile;
				}
			}
		}
	
	// if anything went wrong, we tell the caller that the read was unsuccessful. Some
	// of the members of this class may contain data which is not complete, but this doesn't matter
	// because the caller shouldn't try to access the data if the read was unsuccessful
	TBool readSuccessful = 0;
	if(!LocalizeError)
		readSuccessful = (err == KErrNone);
	delete registrationFile;
	return readSuccessful;
}


// reads as much info as it can
// at least captions and icons must be setup on return from this method (using defaults if necessary)
TBool CAppInfoReader::ReadL()
{
	TUint fileOffset = 0;
	TInt fileLength = 0;
	TUid firstUid(KExecutableImageUidVal);
	TUid middleUid(KUidApp);
	TInt err;
	TInt LocalizeError = 0;
	TUid reguid;
	
	CResourceFile* registrationFile = new CResourceFile(iRegistrationFileName, fileOffset, fileLength);
	if(NULL==registrationFile)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	if(registrationFile)
	{
		iAppUid = registrationFile->ReadAppUidL();
		if(!iAppUid.GetUid())
		{
			std::string errMsg= "Failed : Invalid Resource File. Null Application UID.";
			throw CResourceFileException(errMsg);
		}

		reguid = registrationFile->ReadFileUidL();
		if(reguid.GetUid()!= KUidAppRegistrationResourceFile)
		{
			std::string errMsg= "Failed : Invalid Resource File. UID2 is not defined.";
			throw CResourceFileException(errMsg);
		}
	}

	// set the TUidType for the app binary
	// cannot read the TEntry info from the app binary because it's in \sys\bin
	iAppBinaryUidType = TUidType(firstUid, middleUid, iAppUid);	
	
	RResourceReader	resourceReader;
	resourceReader.OpenL(registrationFile, KAppRegistrationInfoResourceId);	

	TUint localisableResourceId = 1; // only initialising this here to keep the compiler happy, as it's concerned that the variable might be used without having been initialised. The variable should be initialised later, before it's used

	try {
			ReadMandatoryInfoL(resourceReader);
		
			err = ReadNonLocalisableInfoL(resourceReader, localisableResourceId);

			if (!err)
			{	
				err = ReadNonLocalisableOptionalInfoL(resourceReader, registrationFile);
			}
		}
		catch(const CResourceFileException& aObject)
		{
		
			delete registrationFile;
			cout<< aObject.GetMsg()<< endl;
			return EFalse; // might have read something, but failed to setup enough info to make it worthwhile trying to read any more
		}
	
	TBool useDefaultIcons = ETrue;

	if(iLocalisableResourceFileName)
	{
		std::string localizeFileName = Ptr16ToString(iLocalisableResourceFileName);
		std::string folder;
		std::string file;
		size_t found;
		std::string iLocalPath(iDrivePath);

		#ifdef __LINUX__
	  	found=localizeFileName.find_last_of("//");
		#else
		found=localizeFileName.find_last_of("/\\");
		#endif

	  	if(found)
	  		folder = localizeFileName.substr(0,found);
	  	else
		{
			#ifdef __LINUX__
			folder.assign("/");	  		
			#else
			folder.assign("\\");
			#endif
		}
	  	
	  	file = localizeFileName.substr(found+1);
		file.append(".");
		
		iLocalPath.append(folder);
				
		std::wstring iFilePath = string2wstring(iLocalPath);
		std::wstring iFileName = string2wstring(file);

		#ifdef __LINUX__
		iLocalPath.append("/");
		#else
		iLocalPath.append("\\");
		#endif
		
		std::list<std::wstring> locDirs;
		GetDirContents( iFilePath, locDirs );

	 	std::list<std::wstring>::iterator curr = locDirs.begin();
		for( curr = locDirs.begin(); curr != locDirs.end(); ++curr )
		{
			if (curr->find(iFileName,0) != std::wstring::npos)
			{
			    std::string fName;
				std::string sAbsolutePath;
				sAbsolutePath.assign(iLocalPath);
			 	fName = wstring2string( *curr );

			   	sAbsolutePath.append(fName);
				//cout << sAbsolutePath<<endl;
				
				std::string Locale;
				int iLocale = 0;
		
				found=fName.rfind(".r");
				if(found!=string::npos)
					Locale = fName.substr(found+2);
				else
					continue;

				iLocale = atoi(Locale.c_str()); 
			    if(!iLocale)
					iLocale = KNonLocalized;
				   
				CResourceFile* tlocalisableFile = NULL;

				try {
					// open the localisable resource file	
					tlocalisableFile = new CResourceFile(sAbsolutePath, 0, 0);
					if(NULL==tlocalisableFile)
					{
						std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
						throw CResourceFileException(errMsg);
					}

					if (!err)
					{
						if (tlocalisableFile && (localisableResourceId & KResourceOffsetMask))
							tlocalisableFile->ConfirmSignatureL();

						err = ReadLocalisableInfoLoopL(*tlocalisableFile, localisableResourceId, useDefaultIcons, iLocale);
					}
					
					delete tlocalisableFile;
				}
				catch(const CResourceFileException& aObject)
				{
					LocalizeError = 1;
					aObject.Display();
					delete tlocalisableFile;
				}
			}
		}
	}

	// if anything went wrong, we tell the caller that the read was unsuccessful. Some
	// of the members of this class may contain data which is not complete, but this doesn't matter
	// because the caller shouldn't try to access the data if the read was unsuccessful
	TBool readSuccessful = 0;
	if(!LocalizeError)
		readSuccessful = (err == KErrNone);
	delete registrationFile;
	return readSuccessful;
}

// this method reads the minimum information required to register an app
// if this fails (Leaves), we say the read has been unsuccessful

void CAppInfoReader::ReadMandatoryInfoL(RResourceReader& aResourceReader)
{
	aResourceReader.ReadUint32L(); // skip over LONG reserved_long
	aResourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT app_file
	PtrC16* appFile = aResourceReader.ReadTPtrCL();

	if(NULL==appFile)
	{
		std::string errMsg= "Failed : Invalid Resource File Name. Application File Name is Mandatory.";
		throw CResourceFileException(errMsg);
	}

	ConvertToPlatformSpecificPath(appFile->iPtr, appFile->iMaxLength);

	TInt err=0;
	// this object gets used for 2 purposes: first to check that a ParsePtrC can be created over "appFile" without it panicking, and second to construct iAppBinaryFullName
	err = FindWild(appFile); // do this before creating a TParsePtrC, since TParsePtrC's constructor panics if it fails (which would provide an easy way for malware to kill the Apparc server)

	if(err==1)
	{
		std::string errMsg= "Failed : Invalid Resource File Name : Wild Characters Present.";
		throw CResourceFileException(errMsg);
	}
		
	ParsePtrC appFileParser(appFile);

	// read LONG attributes
	iCapability.iAttributes = aResourceReader.ReadUint32L();


	if(!appFileParser.NamePresent())
	{
		appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
		std::string errMsg= "Failed : Invalid Resource File Name : Name Not Present.";
		throw CResourceFileException(errMsg);
	}
		
	std::string appNameWithoutExtension(appFileParser.Name());
	std::string registrationFileDrive;
	
	string::size_type index = iRegistrationFileName.find( ':' );
	if( index != string::npos )
		registrationFileDrive.assign(iRegistrationFileName, index-1, 2);

	if (iCapability.iAttributes & TAppCapability::ENonNative)
	{
		if (!appFileParser.PathPresent() || !appFileParser.ExtPresent())
		{
			appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
			std::string errMsg= "Failed : Invalid Resource File Name : Path And Extension Not Present.";
			throw CResourceFileException(errMsg);
		}

		std::string appFilePath(appFileParser.Path());
		std::string appFileNameAndExt(appFileParser.NameAndExt());

		if (appFileParser.DrivePresent())
			registrationFileDrive = appFileParser.Drive();

		err = FindWild(registrationFileDrive, appFilePath, appFileNameAndExt);
		if(err==1)
		{
			appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
			std::string errMsg= "Failed : Invalid Resource File Name : Wild Characters Present.";
			throw CResourceFileException(errMsg);
		}
	}
	else if (iCapability.iAttributes & TAppCapability::EBuiltAsDll)
	{
		appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
		std::string errMsg= "Failed : legacy dll-style app.";
		throw CResourceFileException(errMsg);
	}
	else
	{
		// exe-style app
		err = FindWild(registrationFileDrive, KAppBinaryPathAndExtension, appNameWithoutExtension);
		if(err==1)
		{
			appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
			std::string errMsg= "Failed : Invalid Resource File Name : Wild Characters Present.";
			throw CResourceFileException(errMsg);
		}
	}

	iAppBinaryFullName = new Ptr16(appFileParser.FullName()->iMaxLength);
	if(NULL==iAppBinaryFullName || NULL == iAppBinaryFullName->GetPtr())
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	iAppBinaryFullName->UpdateLength(appFileParser.FullName()->iMaxLength);
	BufCpy(iAppBinaryFullName->GetPtr(),appFileParser.FullName()->iPtr,appFileParser.FullName()->iMaxLength);

	appFileParser.SetToNull();	//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
	appFile->iPtr = NULL;		//Pointer pointing to the content of Resource file. To be destroyed by RResourceReader.
	delete appFile;
}

/*
* Converts PtrC16* to Ptr16* datatype. Creates a Buffer copy.
*/

Ptr16* CAppInfoReader::CreateFullIconFileNameL(const PtrC16* aIconFileName) const
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

	if (parsePtr.IsWild() || !parsePtr.PathPresent() || !parsePtr.NamePresent())
		return NULL;

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
	std::wstring iFilePath = string2wstring(iDrivePath);
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
			ParsePtrC parsePtr_reg(iRegistrationFileName);
			std::string registrationFileDrive = parsePtr_reg.Drive();

			if(registrationFileDrive.compare(localisableResourceFileDrive))
			{
				// check for icon file on same drive as registration file
				std::string Registrationfilepath(parsePtr_reg.FullPath());
				Registrationfilepath.append(parsePtr.NameAndExt());
				std::wstring iRegFilePath = string2wstring(Registrationfilepath);
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

/*
* Read Localizable resource file.
@ Param: aResourceFile - Localizable file Handler.
@ Param: aResourceId of Localizable file.
*/

TInt CAppInfoReader::ReadLocalisableInfoLoopL(CResourceFile& aResourceFile, TUint aResourceId, TBool& aUseDefaultIcons, TInt& iLocale)
{
	CAppLocalizableInfo* localinfo = CAppLocalizableInfo::NewL(iRegistrationFileName, iDrivePath);
	RResourceReader resourceReader;
	resourceReader.OpenL(&aResourceFile, aResourceId); //original

	localinfo->SetLocale(iLocale);

	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT short_caption
	Ptr16* shortCaption = resourceReader.ReadHBufCL();
	localinfo->SetShortCaption(shortCaption);

	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT caption
	Ptr16* caption = resourceReader.ReadHBufCL();
	if(NULL == caption)
	{
		Ptr16* appBinaryFullName = CAppInfoReader::AppBinaryFullName();

		caption = new Ptr16(appBinaryFullName->GetLength());
		if(NULL == caption)
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
		BufCpy(caption->GetPtr(),appBinaryFullName->GetPtr(),appBinaryFullName->GetLength());
		caption->UpdateLength(appBinaryFullName->GetLength());
	}
	localinfo->SetCaption(caption);
	
	// read WORD number_of_icons
	const TInt numOfIcons = resourceReader.ReadInt16L();
	localinfo->SetNumOfAppIcons(numOfIcons);

	// read LTEXT icon_file
	iIconFileName = NULL;
	PtrC16* iconFile = resourceReader.ReadTPtrCL();

	if(NULL != iconFile)
	{
		ConvertToPlatformSpecificPath(iconFile->iPtr, iconFile->iMaxLength);
		Ptr16*	iconFileName = CreateFullIconFileNameL(iconFile);
		
		if (iconFileName)
		{
			localinfo->SetIconFileName(iconFileName);
			aUseDefaultIcons = EFalse;
			iNonMbmIconFile = ETrue;
			iconFile->iPtr = NULL;
			delete iconFile;
		}
		else
		{
			iconFile->iPtr = NULL;
			delete iconFile;
			std::string errMsg= "Failed : Icon File not present in Device.";
			throw CResourceFileException(errMsg);
		}
	}

	//Read ViewData  of Localizable file.
	localinfo->ReadViewDataL(resourceReader);

	// Read LTEXT group_name
	// If a localised group name has been specified, it overrides
	// The group name (if any), specified by APP_REGISTRATION_INFO

	PtrC16* groupName = resourceReader.ReadTPtrCL();

	if(NULL != groupName)
	{
		if (groupName->iMaxLength > 0)
		{
			Ptr16* gName = new Ptr16(groupName->iMaxLength);
			if(NULL==gName || NULL == gName->GetPtr())
			{
				delete groupName;
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			gName->UpdateLength(groupName->iMaxLength);
			BufCpy(gName->GetPtr(),groupName->iPtr,groupName->iMaxLength);
			localinfo->SetGroupName(gName);
			groupName->iPtr = NULL; //To Avoid double delete in destructor. To be destroyed by RResourceReader.
		}
	}

	iAppLocalizableInfo.push_back(localinfo);
	delete groupName;
	return 0;
}

void CAppInfoReader::ReadOpaqueDataL(TUint aResourceId, CResourceFile* aRegistrationFile, TUint32 aServiceUid)
{
	Ptr8* opaqueData = NULL;
	int iLocale = 0;

	if (aResourceId == 0)
	{
		opaqueData = new Ptr8(1);
		if(NULL==opaqueData || NULL==opaqueData->GetPtr())
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		*(opaqueData->GetPtr()) = 0;
		opaqueData->UpdateLength(1);
	}
	else
	{
		if (aResourceId & KResourceOffsetMask)
		{
			if(iLocalisableResourceFileName)
			{
				std::string localizeFileName = Ptr16ToString(iLocalisableResourceFileName);
				std::string folder;
				std::string file;
				size_t found;
				size_t find;
				std::string iLocalPath(iDrivePath);
		
				#ifdef __LINUX__
				found=localizeFileName.find_last_of("//");
				#else
				found=localizeFileName.find_last_of("/\\");
				#endif

				if(found)
					folder = localizeFileName.substr(0,found);
				else
				{
					#ifdef __LINUX__
					folder.assign("/");
					#else
					folder.assign("\\");
					#endif					
				}
			
				file = localizeFileName.substr(found+1);
				file.append(".");
			
				iLocalPath.append(folder);
					
				std::wstring iFilePath = string2wstring(iLocalPath);
				std::wstring iFileName = string2wstring(file);
				#ifdef __LINUX__
				iLocalPath.append("/");
				#else
				iLocalPath.append("\\");
				#endif

		
				std::list<std::wstring> locDirs;
				GetDirContents( iFilePath, locDirs );
			
				std::list<std::wstring>::iterator curr = locDirs.begin();
				for( curr = locDirs.begin(); curr != locDirs.end(); ++curr )
				{
					if (curr->find(iFileName,0) != std::wstring::npos)
					{
						std::string fName;
						std::string sAbsolutePath;
						std::string Locale;
						sAbsolutePath.assign(iLocalPath);
						fName = wstring2string( *curr );

						find=fName.rfind("backup");
						if(find != string::npos)
							continue;

						sAbsolutePath.append(fName);										
						found=fName.find_last_of(".");
						if(found)
							Locale = fName.substr(found+2);
			
						iLocale = atoi(Locale.c_str()); 
						if(!iLocale)
							iLocale = KNonLocalized;
					   
						CResourceFile* tlocalisableFile = NULL;
			
						// open the localisable resource file	
						tlocalisableFile = new CResourceFile(sAbsolutePath, 0, 0);
						if(NULL==tlocalisableFile)
						{
							std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
							throw CResourceFileException(errMsg);
						}
			
						if (tlocalisableFile)
						{
							if (tlocalisableFile && (aResourceId & KResourceOffsetMask))
								tlocalisableFile->ConfirmSignatureL();

							opaqueData = tlocalisableFile->AllocReadL(aResourceId);

							if(opaqueData)
							{
								const TUint8* currentPtr=opaqueData->GetPtr();//TUint8 pointer is used
									
								//resource string length is limited to 255 characters max.
								const TInt unicodeLength=*currentPtr;
		
								if (unicodeLength!=0)
								{
									if (REINTERPRET_CAST(TUint,(currentPtr+1))&0x1)
									{			
										// The resource compiler puts out a padding byte (arbitrarily 0xab)
										// to ensure the alignment of Unicode strings within each resource.
								
										//Cardanility check. Values in the range 0-127 are stored in a single byte, 
										//128-16383 in two bytes and other values in 4 bytes.
										if((*(currentPtr+1)!=0xab) && (*(currentPtr+3)!=0xab))
										{
											std::string errMsg= "Failed : Trying to access invalid registrationFile";
											throw CResourceFileException(errMsg);
										}
									}
								}
								
								opaqueData->SetPtr(currentPtr);
								opaqueData->SetLength(opaqueData->GetLength());
								
							}
							else
							{
								opaqueData = new Ptr8(1);
								if(NULL==opaqueData || NULL==opaqueData->GetPtr())
								{
									std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
									throw CResourceFileException(errMsg);
								}
								*(opaqueData->GetPtr()) = 0;
								opaqueData->SetLength(1);
							}

							CAppLocalOpaqueDataInfo* opaqueInfo = CAppLocalOpaqueDataInfo::NewL(iLocale, aServiceUid, opaqueData);
							iOpaqueDataArray.push_back(opaqueInfo);
							
						}
						delete tlocalisableFile;
					}
				}
			}
		}
		else
		{	// expecting opaque data to be in the registration file
			assert(aRegistrationFile);
			opaqueData = aRegistrationFile->AllocReadL(aResourceId); //lint !e613 Suppress ossible use of null pointer

			const TUint8* currentPtr=opaqueData->GetPtr();//TUint8 pointer is used

			//resource string length is limited to 255 characters max.
			const TInt unicodeLength=*currentPtr;
		
			if (unicodeLength!=0)
			{
				if (REINTERPRET_CAST(TUint,(currentPtr+1))&0x1)
				{
					// The resource compiler puts out a padding byte (arbitrarily 0xab)
					// to ensure the alignment of Unicode strings within each resource.
					
					//Cardanility check. Values in the range 0-127 are stored in a single byte, 
					//128-16383 in two bytes and other values in 4 bytes.
					if((*(currentPtr+1)!=0xab) && (*(currentPtr+3)!=0xab))
					{
						std::string errMsg= "Failed : Trying to access invalid registrationFile";
						throw CResourceFileException(errMsg);
					}
				}
			}

			opaqueData->SetPtr(currentPtr);
			opaqueData->SetLength(opaqueData->GetLength());
			CAppLocalOpaqueDataInfo* opaqueInfo = CAppLocalOpaqueDataInfo::NewL(iLocale, aServiceUid, opaqueData);
			iOpaqueDataArray.push_back(opaqueInfo);
		}
	}
}

TInt CAppInfoReader::ReadNonLocalisableOptionalInfoL(RResourceReader& aResourceReader, CResourceFile* aRegistrationFile)	
{
	// read LEN WORD STRUCT service_list[]
	TInt serviceCount = 0;
	TInt err = 0;
	// service information was not present in the first release of the registration file
	// APP_REGISTRATION_INFO resource struct
	// this method must not leave if the registration file doesn't contain service information, so the
	// following call to ReadInt16L is trapped to ensure this method doesn't leave just because
	// there is no more information in the resource to read (KErrEof)

	serviceCount = aResourceReader.ReadInt16L();

	if (!iServiceArray && serviceCount > 0)
	{
		iServiceArray = new std::vector<TAppServiceInfo*>;

		if(NULL==iServiceArray){
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
	}
	
	while (serviceCount--)
	{
		const TUid serviceUid = {aResourceReader.ReadUint32L()};
		
		if ((serviceUid == KOpenServiceUid) && (iOpenServiceIsLegacy))
		{
			assert(iIndexOfFirstOpenService == 0);
			// If we found an Open service in the SERVICE_INFO declaration
			// then we must ignore the legacy one
			if ( 0 < iServiceArray->size() )
			{
				(*iServiceArray)[0]->Release();
				iServiceArray->erase(iServiceArray->begin());
			}
			iOpenServiceIsLegacy = EFalse;
			iIndexOfFirstOpenService = KErrNotFound;
		}
		
		std::vector<TDataTypeWithPriority*>* mimeTypesSupported = new std::vector<TDataTypeWithPriority*>;
		if(NULL==mimeTypesSupported)
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}

		ReadMimeTypesSupportedL(aResourceReader, *mimeTypesSupported);
		
		const TUint resourceId = aResourceReader.ReadUint32L();

		ReadOpaqueDataL(resourceId, aRegistrationFile, serviceUid.iUid);

		TAppServiceInfo* serviceInfo = new TAppServiceInfo(serviceUid, *mimeTypesSupported);
		if(NULL==serviceInfo)
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}

		iServiceArray->push_back(serviceInfo);

		if ((serviceUid == KOpenServiceUid) && (iIndexOfFirstOpenService < 0))
			iIndexOfFirstOpenService = iServiceArray->size() - 1;
	}

	// read LLINK opaque_data
	const TUint resourceId = aResourceReader.ReadUint32L();

	ReadOpaqueDataL(resourceId, aRegistrationFile, 0);

	return 0;
}

TInt CAppInfoReader::ReadNonLocalisableInfoL(RResourceReader& aResourceReader, TUint& aLocalisableResourceId)
{
	// read LTEXT localisable_resource_file
	PtrC16*	localisableResourceFileName = aResourceReader.ReadTPtrCL();

	if(NULL == localisableResourceFileName)
	{
		iLocalisableResourceFileName = NULL;
	}
	else
	{
		ConvertToPlatformSpecificPath(localisableResourceFileName->iPtr, localisableResourceFileName->iMaxLength);
		if (localisableResourceFileName->iMaxLength > 0 )
		{
			ParsePtrC parsePtr(localisableResourceFileName);

			if(parsePtr.IsValidName())
			{
				iLocalisableResourceFileName = NULL;
				parsePtr.SetToNull();	//To Avoid double Delete in destructor.
				std::string errMsg= "Failed : Invalid localisable Resource File Name";
				throw CResourceFileException(errMsg);
			}
			
			std::string registrationFileDrive;
	
			string::size_type index = iRegistrationFileName.find( ':' );
			if( index != string::npos )
				registrationFileDrive.assign(iRegistrationFileName, index-1, 2);

			TInt err=0;
			std::string sTemp = parsePtr.StrName();

			err = FindWild(registrationFileDrive, KAppResourceFileExtension, sTemp);
			if(err==1)
			{
				std::string errMsg= "Failed : Invalid drive for Registration File";
				parsePtr.SetToNull();	//To Avoid double Delete in destructor.
				throw CResourceFileException(errMsg);
			}

			iLocalisableResourceFileName = new Ptr16(parsePtr.FullName()->iMaxLength);
			if(NULL==iLocalisableResourceFileName || NULL==iLocalisableResourceFileName->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				parsePtr.SetToNull();	//To Avoid double Delete in destructor.
				throw CResourceFileException(errMsg);
			}

			iLocalisableResourceFileName->UpdateLength(parsePtr.FullName()->iMaxLength);
			BufCpy(iLocalisableResourceFileName->GetPtr(),parsePtr.FullName()->iPtr,parsePtr.FullName()->iMaxLength);

			parsePtr.SetToNull();	//To Avoid double Delete in destructor.
		}
	}
	// read LONG localisable_resource_id
	aLocalisableResourceId = aResourceReader.ReadUint32L();

	iCapability.iAppIsHidden = aResourceReader.ReadInt8L();
	iCapability.iEmbeddability = static_cast<TAppCapability::TEmbeddability>(aResourceReader.ReadInt8L());
	iCapability.iSupportsNewFile = aResourceReader.ReadInt8L();
	iCapability.iLaunchInBackground = aResourceReader.ReadInt8L();

	PtrC16* iTemp = aResourceReader.ReadTPtrCL();

	if(NULL==iTemp || NULL==iTemp->iPtr)
		iCapability.iGroupName = NULL;
	else 
	{
		iCapability.iGroupName = new Ptr16(iTemp->iMaxLength);
		if(NULL==iCapability.iGroupName || NULL == iCapability.iGroupName->GetPtr())
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		iCapability.iGroupName->UpdateLength(iTemp->iMaxLength);
		BufCpy(iCapability.iGroupName->GetPtr(), iTemp->iPtr, iTemp->iMaxLength );
	}
	// read BYTE default_screen_number
	iDefaultScreenNumber = aResourceReader.ReadUint8L();
 	
	//read the datatypes
	std::vector<TDataTypeWithPriority*>* datatypes = new std::vector<TDataTypeWithPriority*>;
	if(NULL==datatypes)
	{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
	}

	ReadMimeTypesSupportedL(aResourceReader, *datatypes);
	//dataTypes is deleted if 
	// A. There are no legacy datatypes
	// B. Control panel plugin apps are not allowed to register MIME types.If they happen to have any, these datatypes should be ignored.
	iServiceArray = NULL;
	if ((iCapability.iAttributes & TAppCapability::EControlPanelItem) || (datatypes->size() == 0))
		delete datatypes;
	else
	{
		iServiceArray = new std::vector<TAppServiceInfo*>;
		if(NULL==iServiceArray)
		{
				delete datatypes;
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
		}

		Ptr8* opaqueData = new Ptr8(1);
		if(NULL==opaqueData || NULL==opaqueData->GetPtr())
		{
			delete datatypes;
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		*(opaqueData->GetPtr()) = 0;
		opaqueData->UpdateLength(0);

		TAppServiceInfo* serviceInfo = new TAppServiceInfo(KOpenServiceUid, *datatypes);
		if(NULL==serviceInfo)
		{
			delete datatypes;
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
		iServiceArray->push_back(serviceInfo);
		iIndexOfFirstOpenService = 0;
		iOpenServiceIsLegacy = ETrue;
	}

	// read LEN WORD STRUCT file_ownership_list[]
	const TInt fileOwnershipArraySize = aResourceReader.ReadInt16L();

	iOwnedFileArray = NULL;
	if (fileOwnershipArraySize > 0)
	{
		iOwnedFileArray = new std::vector<Ptr16*>;
		if(NULL==iOwnedFileArray)
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}
	}
	for (TInt i=0; i < fileOwnershipArraySize; i++)
	{
		PtrC16* fileNamePtr_temp = aResourceReader.ReadTPtrCL();

		if(NULL != fileNamePtr_temp)
		{
			ConvertToPlatformSpecificPath(fileNamePtr_temp->iPtr, fileNamePtr_temp->iMaxLength);
			Ptr16* fileNamePtr = new Ptr16(fileNamePtr_temp->iMaxLength);
			if(NULL==fileNamePtr || NULL==fileNamePtr->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			fileNamePtr->UpdateLength(fileNamePtr_temp->iMaxLength);			  
			BufCpy(fileNamePtr->GetPtr(),fileNamePtr_temp->iPtr,fileNamePtr_temp->iMaxLength);

			iOwnedFileArray->push_back(fileNamePtr);
		}
		else
		{
			Ptr16* fileNamePtr = NULL;
			iOwnedFileArray->push_back(fileNamePtr);
		}
	}

	return 0;
}

// This method can be used to check whether app has a WriteDeviceCap 
// and its sid is trusted
void CAppInfoReader::ReadAppSecurityInfo()
{
    if (!iSecurityInfoHasBeenRead)
    {
		if(NULL==iAppBinaryFullName)
		{
			std::string errMsg= "Failed : Invalid Application Name";
			throw CResourceFileException(errMsg);
		}

		Ptr16* iBinaryName = new Ptr16(iAppBinaryFullName->GetLength());
		if(NULL == iBinaryName)
		{
				std::string errMsg= "Failed : Memory Allocation Failed";
				throw CResourceFileException(errMsg);
		}
		BufCpy(iBinaryName->GetPtr(),iAppBinaryFullName->GetPtr(),iAppBinaryFullName->GetLength());
		iBinaryName->UpdateLength(iAppBinaryFullName->GetLength());

		const TInt err = CApaSecurityUtils::CheckAppSecurity( *iBinaryName, 
                                        iHasWriteDeviceDataCap, iIsSidTrusted, iDrivePath);

		delete iBinaryName;
	
        if ( KErrNone != err )
        {
            iHasWriteDeviceDataCap = EFalse;
            iIsSidTrusted = EFalse;
        }

        iSecurityInfoHasBeenRead = ETrue;   
     }
}


void CAppInfoReader::ReadMimeTypesSupportedL(RResourceReader& aResourceReader,
	std::vector<TDataTypeWithPriority*>& aMimeTypesSupported)
{
	// read LEN WORD STRUCT datatype_list[]
	const TInt dataTypeArraySize = aResourceReader.ReadInt16L();
	if (dataTypeArraySize <= 0)
		return;
	
	for (TInt i=0; i < dataTypeArraySize; i++)
	{
		TDataTypePriority priority = static_cast<TDataTypePriority>(aResourceReader.ReadInt32L());

		//Check for data priority of UnTrusted apps however the trusted apps will not have any restrictions 
		//over the data priority.	
		//If an untrusted app has write device data capability (i.e. still has priority = KDataTypePrioritySystem),
		//do not restrict to KDataTypeUnTrustedPriorityThreshold
		if (priority > KDataTypeUnTrustedPriorityThreshold || priority == KDataTypePrioritySystem )
	    {
		    ReadAppSecurityInfo();

            if (priority == KDataTypePrioritySystem)
            {
                // Check that the app has capability WriteDeviceData
                if (!iHasWriteDeviceDataCap)
                    priority = KDataTypePriorityNormal;
            }
            else
            {
                //data priority for UnTrusted apps would be capped if it is greater than the threshold priority i.e, KMaxTInt16.
                TInt match=iRegistrationFileName.find(KLitPathForUntrustedRegistrationResourceFiles);
                if (match != KErrNotFound && !iIsSidTrusted) 
                {
                    //if registration file is in import directory and 
                    //its sid is in unprotected range - downgrade the priority
                    priority = KDataTypeUnTrustedPriorityThreshold;	
                }
             }
	    }

		PtrC8* dataTypePtr  = aResourceReader.ReadTPtrC8L();

		if(NULL != dataTypePtr)
		{
			TDataType* dataType = new TDataType(dataTypePtr);
			if(NULL==dataType)
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			TDataTypeWithPriority* dataTypeWithPriority = new TDataTypeWithPriority(*dataType, priority);
			if(NULL==dataTypeWithPriority)
			{	
				delete dataType;
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			aMimeTypesSupported.push_back(dataTypeWithPriority);
		}
		else
		{
			TDataTypeWithPriority* dataTypeWithPriority = NULL;
			aMimeTypesSupported.push_back(dataTypeWithPriority);
		}
	}
}


