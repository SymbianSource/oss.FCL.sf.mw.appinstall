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
// Description: File to Parse Resource File for Class AppRegistrationInfo and Create Input for
//			 XML Generator.
//


#include "parameterlist.h"
#include "stringutils.h"
#include "is_utils.h"
#include "dirparse.h"
#include "barsc2.h"
#include "commontypes.h"
#include "sisregistry.h"
#include "errors.h"
#include "dbconstants.h"
#include "parse.h"
#include "utility_interface.h"

using namespace std;

/**
@internalTechnology
*/
std::wstring AppResourceFileExtension = L".rsc";

/**
 * Identify if input file is a Registration Resource File.
 @ Param : aRegistrationFileName - File name to be scanned.
 */
TInt FindRegistrationResourceFileL(std::string& aRegistrationFileName)
{
	CResourceFile* registrationFile = NULL;
	try {
			registrationFile = new CResourceFile(aRegistrationFileName, 0, 0);
			if(NULL==registrationFile)
			{
				std::cout<<"Failed : Error in Reading File. Memory Allocation Failed"<<std::endl;
				return 1;
			}
			else
			{
				TUid iAppUid = registrationFile->ReadFileUidL();
				if(KUidAppRegistrationResourceFile == iAppUid.GetUid())
				{
					delete registrationFile;
					return 0;
				}
			}
		}
		catch(const CResourceFileException& aObject)
		{
			if(registrationFile)
				delete registrationFile;
			return 1;
		}
	
	delete registrationFile;
	return 1;
}


/**
 * Creates Class TAppDataType for XML Parsing
 */

std::wstring Ptr8_2_Wstring(const Ptr8* aBuf)
{
	std::wstring str2(aBuf->GetLength(), L'\0'); // Make room for characters

	TUint8* temp = aBuf->GetPtr();
	std::copy(temp,temp+aBuf->GetLength(),str2.begin());

	return str2;
}


void CreateAppDataType(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo& aAppDataType, 
								const TDataTypeWithPriority* aTemp)
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType componentDataType;

	componentDataType.iPriority = aTemp->iPriority;
	componentDataType.iType = Ptr8_2_Wstring(aTemp->iDataType.GetDataType());

	aAppDataType.iDataType.push_back(componentDataType);
}

/**
 * Creates Class TViewData for XML Parsing
 */
void CreateViewDataL(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo& aLocalizableAttribute, 
							const CAppLocalizableInfo* aAppInfoReader)
{
	std::wstring sStr1;
	std::wstring sStr;

	std::vector<CAppViewData*>* viewDataArray = aAppInfoReader->GetViewDataArray();

	if(viewDataArray)
	{
		for(TInt i = 0; i<viewDataArray->size(); i++)
		{

			XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData ViewData;
			CAppViewData *temp = viewDataArray->at(i);
	
			sStr = DbConstants::CompUID;
			TUid ID = temp->Uid();
			sStr1 = Utils::IntegerToWideString(ID.GetUid());
			CreateViewDataAttributes(ViewData,sStr,sStr1,true,false);
	
			sStr = DbConstants::CompScreenMode;
			TInt SMode = temp->GetScreenMode();
	
			sStr1 = Utils::IntegerToWideString(SMode);
			CreateViewDataAttributes(ViewData,sStr,sStr1,true,false);
			
			sStr = DbConstants::CompCaption;
			Ptr16* caption = temp->GetCaption();
			if(caption)
				sStr1 = Ptr16ToWstring(caption);
			else
				sStr1=L'\0';		
			CreateViewDataAttributes(ViewData,sStr,sStr1,false,false);
			
			sStr = DbConstants::CompNumberOfIcons;
			TInt IconNum = temp->GetNumOfViewIcons();
			sStr1 = Utils::IntegerToWideString(IconNum);
			CreateViewDataAttributes(ViewData,sStr,sStr1,true,false);
	
			sStr = DbConstants::CompIconFile;
			Ptr16* IconName = temp->GetIconFileName();
			if(IconName)
				sStr1 = Ptr16ToWstring(IconName);
			else
				sStr1=L'\0';
			CreateViewDataAttributes(ViewData,sStr,sStr1,false,false);

			aLocalizableAttribute.iViewData.push_back(ViewData);
		}
	}
}

void CreateViewDataAttributes(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData& aViewData, 
							const std::wstring& aName,const std::wstring& aValue, 
							const bool aIsIntValue, const int aIsStr8Bit)
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes ViewDataAttributes;

	ViewDataAttributes.iName = aName;
	ViewDataAttributes.iValue = aValue;
	ViewDataAttributes.iIsIntValue = aIsIntValue;
	ViewDataAttributes.iIsStr8Bit = aIsStr8Bit;

	aViewData.iViewDataAttributes.push_back(ViewDataAttributes);
}

/**
 * Creates Class TLocalizableAttribute for XML Parsing
 */

void CreateLocalizableAttribute(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo& aLocalizableAttribute, 
										const std::wstring& aName, const std::wstring& aValue, 
										const bool aIsIntValue, const int aIsStr8Bit)
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute LocalizableAttribute;

	LocalizableAttribute.iName = aName;
	LocalizableAttribute.iValue = aValue;
	LocalizableAttribute.iIsIntValue = aIsIntValue;
	LocalizableAttribute.iIsStr8Bit = aIsStr8Bit;

	aLocalizableAttribute.iLocalizableAttribute.push_back(LocalizableAttribute);
}

/**
 * Creates Class TAppLocalizableInfo for XML Parsing
 */
void CreateLocalizableInfoL(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppLocalizableInfo, 
									const CAppLocalizableInfo* aInfoReader)
{
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo componentLocalizableInfo;

		Ptr16* shortCaption = aInfoReader->GetShortCaption();
		std::wstring sStr = DbConstants::CompShortCaption;
		std::wstring sStr1;
		
		if(shortCaption)
			sStr1 = Ptr16ToWstring(shortCaption);
		else
			sStr1=L'\0';
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,false,false);	

		Ptr16* caption = aInfoReader->GetCaption();
		sStr = DbConstants::CompCaption;
		if(caption)
			sStr1 = Ptr16ToWstring(caption);
		else
			sStr1=L'\0';
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,false,false);
		
		TInt  iNumOfAppIcons = aInfoReader->GetNumOfAppIcons();
		sStr = DbConstants::CompNumberOfIcons;
		sStr1 = Utils::IntegerToWideString(iNumOfAppIcons);
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,true,false);

		TInt  iLocale = aInfoReader->GetLocale();
		sStr = DbConstants::CompLocale;
		sStr1 = Utils::IntegerToWideString(iLocale);
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,true,false);

		Ptr16* iIconFileName = aInfoReader->GetIconFileName();
		sStr = DbConstants::CompIconFile;
		if(iIconFileName)
			sStr1 = Ptr16ToWstring(iIconFileName);
		else
			sStr1=L'\0';
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,false,false);

		Ptr16* iGroupName = aInfoReader->GetGroupName();
		sStr = DbConstants::CompGroupName;
		if(iGroupName)
			sStr1 = Ptr16ToWstring(iGroupName);
		else
			sStr1=L'\0';
		CreateLocalizableAttribute(componentLocalizableInfo,sStr,sStr1,false,false);

		CreateViewDataL(componentLocalizableInfo,aInfoReader);

		aAppLocalizableInfo.iApplicationLocalizableInfo.push_back(componentLocalizableInfo);
}


/**
 * Creates Class TAppServiceInfo for XML Parsing
 */

void CreateAppServiceInfo(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppServiceInfo, 
									const TUid& aUid, const std::vector<TDataTypeWithPriority*>& aDataType) 
{
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo componentServiceInfo;

		componentServiceInfo.iUid = aUid.GetUid();
		for(TInt ii = 0; ii<aDataType.size(); ii++)
		{
			TDataTypeWithPriority *temp = aDataType.at(ii);
			CreateAppDataType(componentServiceInfo,temp);
		}
	
		aAppServiceInfo.iApplicationServiceInfo.push_back(componentServiceInfo);
}

/**
 * Creates Class TAppAttribute for XML Parsing
 */

void CreateAppAttribute(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppAttribute, 
								const std::wstring& aName,const std::wstring& aValue, 
								const bool aIsIntValue, const int aIsStr8Bit )
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute componentAttribute;

	componentAttribute.iName = aName;
	componentAttribute.iValue = aValue;
	componentAttribute.iIsIntValue = aIsIntValue;
	componentAttribute.iIsStr8Bit = aIsStr8Bit;

	aAppAttribute.iApplicationAttribute.push_back(componentAttribute);
}

/**
 * Creates Class TAppAttribute for XML Parsing
 */

void CreateAppProperty(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppProperty, 
								const std::string& aStrValue, const int aLocale,
								const int aServiceUid, const bool aIsStr8Bit )
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty componentproperty;

	componentproperty.iName = DbConstants::CompOpaqueData;
	componentproperty.iLocale = aLocale;
	componentproperty.iServiceUid = aServiceUid;
	componentproperty.iIntValue = 0;
	componentproperty.iStrValue = aStrValue;
	componentproperty.iIsStr8Bit = aIsStr8Bit;

	aAppProperty.iApplicationProperty.push_back(componentproperty);
}

/**
 * Creates Class TOpaqueDataType for XML Parsing
 */

void CreateOpaqueDataType(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppOpaqueData, 
								const std::string& aStrValue, const int aLocale, const int aServUid)
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType componentData;

	componentData.iLocale = aLocale;
	componentData.iServiceUid = aServUid;

	 componentData.iOpaqueData = aStrValue;
	 componentData.iIsBinary = true;
	 aAppOpaqueData.iOpaqueDataType.push_back(componentData);
}

/**
 * Get the path of Database "scr.db"
 */

std::string GetDbPath(const CParameterList* aParamList)
{
	if	( 
		(aParamList->RomLogFileNames().size() != 0 && aParamList->RomDrivePath().size() == 0 ) ||
		aParamList->IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet) 
		)
		{
			#ifdef __LINUX__
			return wstring2string(aParamList->SystemDrivePath()) + "/sys/install/scr/scr.db";
			#else
			return wstring2string(aParamList->SystemDrivePath()) + "\\sys\\install\\scr\\scr.db";
			#endif
		}

	#ifdef __LINUX__
	return wstring2string(aParamList->RomDrivePath()) + "/sys/install/scr/provisioned/scr.db";
	#else
	return wstring2string(aParamList->RomDrivePath()) + "\\sys\\install\\scr\\provisioned\\scr.db";
	#endif
}

/**
 * Update Installation Information in Database using SRCTOOL
 */

void UpdateInstallationInformation_xml(const CParameterList* aParamList,
												XmlDetails::TScrPreProvisionDetail aScrPreProvisionDetail)
{
	CXmlGenerator xmlGenerator;

	#ifndef __TOOLS2_LINUX__
		char* tmpFileName = tmpnam(NULL);	
	#else
		char tmpFileName[] = "/tmp/interpretsis_preprovision_XXXXXX";	
		int temp_fd;
		temp_fd=mkstemp(tmpFileName); 
		fclose(fdopen(temp_fd,"w"));
	#endif

	std::wstring filename(string2wstring(tmpFileName));

	int isRomApplication = 1;
	xmlGenerator.WritePreProvisionDetails(filename , aScrPreProvisionDetail, isRomApplication);						

	#ifdef __LINUX__
	std::string executable = "scrtool";
	#else 
	std::string executable = "scrtool.exe";
	#endif

	std::string command;

	command = executable + " -d " + GetDbPath(aParamList) + " -p " + tmpFileName;

	cout << "Updating DB : " << command << endl;

	int error = system(command.c_str());
	
	if(error != 0)
	{
		LERROR(L"Scrtool failed to upload the database registry entry.");
	}
		
	error = remove(tmpFileName);

	if(error != 0)
	{
		LERROR(L"Temporary file removal failed.");
	}	
}

/**
 * Creates Class TApplicationRegistrationInfo for XML Parsing
 */


void CreateApplicationRegistrationInfoL(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
												const CAppInfoReader* aAppInfoReader)
{
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo iApplicationRegistrationInfo;

	Ptr16* iAppBinaryFullName = aAppInfoReader->AppBinaryFullName();
	std::wstring sAppAttributeName = DbConstants::CompAppFile;
	std::wstring sAppAttributeValue = Ptr16ToWstring(iAppBinaryFullName);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,false,false);

	TUidType iAppBinaryUidType = aAppInfoReader->AppBinaryUidType();;
	sAppAttributeName = DbConstants::CompAppUid;
	TUid Uid = iAppBinaryUidType[2];
	sAppAttributeValue = Utils::IntegerToWideString(Uid.GetUid());
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompTypeId;
	TInt TypeId = -1367772926;
	sAppAttributeValue = Utils::IntegerToWideString(TypeId);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	TAppCapability iCapability;
	aAppInfoReader->Capability(iCapability);

 	sAppAttributeName = DbConstants::CompAttributes;
	sAppAttributeValue = Utils::IntegerToWideString(iCapability.iAttributes);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompHidden;
	sAppAttributeValue = Utils::IntegerToWideString(iCapability.iAppIsHidden);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompEmbeddable;
	sAppAttributeValue = Utils::IntegerToWideString(iCapability.iEmbeddability);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompNewfile;
	sAppAttributeValue = Utils::IntegerToWideString(iCapability.iSupportsNewFile);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompLaunch;
	sAppAttributeValue = Utils::IntegerToWideString(iCapability.iLaunchInBackground);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	sAppAttributeName = DbConstants::CompGroupName;
	if(iCapability.iGroupName)
		sAppAttributeValue = Ptr16ToWstring(iCapability.iGroupName);
	else
		sAppAttributeValue=L'\0';
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,false,false);

	TUint iDefaultScreenNumber = aAppInfoReader->DefaultScreenNumber();
	sAppAttributeName = DbConstants::CompDefaultScreenNumber;
	sAppAttributeValue = Utils::IntegerToWideString(iDefaultScreenNumber);
	CreateAppAttribute(iApplicationRegistrationInfo,sAppAttributeName,sAppAttributeValue,true,false);

	std::vector<Ptr16*>* ownedFileArray = aAppInfoReader->OwnedFiles();
	std::wstring sFileName;
	if(ownedFileArray)
	{
		for(TInt ii = 0; ii<ownedFileArray->size(); ii++)
		{
			Ptr16 *temp = ownedFileArray->at(ii);
			if(temp)
				sFileName = Ptr16ToWstring(temp);
			else
				sFileName = L'\0';
			iApplicationRegistrationInfo.iFileOwnershipInfo.push_back(sFileName);
		}
	}

	std::string sOpaqueData;
	TInt iIndexOfFirstOpenService;
	std::vector<TAppServiceInfo*>* iServiceArray = aAppInfoReader->ServiceArray(iIndexOfFirstOpenService);
	if(iServiceArray)
	{
		for(TInt i = 0; i<iServiceArray->size(); i++)
		{
			TAppServiceInfo* temp = iServiceArray->at(i);
			TUid iUid = temp->GetUid();
			std::vector<TDataTypeWithPriority*> iData = temp->GetDataType();
			CreateAppServiceInfo(iApplicationRegistrationInfo,iUid,iData);

			int servUid = iUid.GetUid();
			std::vector<CAppLocalOpaqueDataInfo*> opaqueDataArray = aAppInfoReader->GetOpaqueDataArray();
			for(TInt i = 0; i<opaqueDataArray.size(); i++)
			{
				CAppLocalOpaqueDataInfo* tp = opaqueDataArray.at(i);
			
				TUint32 serviceUid = tp->GetServiceUid();
				if(servUid == serviceUid)
				{
					Ptr8*	opaqueData = tp->GetOpaqueData();

					if(opaqueData)
						sOpaqueData.assign(opaqueData->GetPtr(), opaqueData->GetPtr()+opaqueData->GetLength());
					else
						sOpaqueData = '\0';
					TInt locale = tp->GetLocale();
					CreateOpaqueDataType(iApplicationRegistrationInfo,sOpaqueData,locale,serviceUid);
				}
			}
		}
	}

	std::vector<CAppLocalizableInfo*> aAppLocalizable = aAppInfoReader->LocalizableInfo();

	for(TInt i = 0; i<aAppLocalizable.size(); i++)
	{
		CAppLocalizableInfo* tp = aAppLocalizable.at(i);
		CreateLocalizableInfoL(iApplicationRegistrationInfo,tp);
	}

	std::vector<CAppLocalOpaqueDataInfo*> opaqueDataArray = aAppInfoReader->GetOpaqueDataArray();
	for(TInt i = 0; i<opaqueDataArray.size(); i++)
	{
		CAppLocalOpaqueDataInfo* tp = opaqueDataArray.at(i);

		TUint32 serviceUid = tp->GetServiceUid();
		if(0 == serviceUid)
		{
			Ptr8* 	opaqueData = tp->GetOpaqueData();

			if(opaqueData)
				sOpaqueData.assign(opaqueData->GetPtr(), opaqueData->GetPtr()+opaqueData->GetLength());
			else
				sOpaqueData='\0';
			
			TInt locale = tp->GetLocale();
			CreateOpaqueDataType(iApplicationRegistrationInfo,sOpaqueData,locale,serviceUid);
		}
	}

	aComponent.iApplicationRegistrationInfo.push_back(iApplicationRegistrationInfo);
}


/**
 * Get the path of Localizefile.
 */

std::string GetLocalizeFilePath(const std::string& aFileName, const CParameterList* aParamList)
{
	if(aParamList->IsFlagSet(CParameterList::EFlagsResourceFilePathSet))
	{
		size_t found;
		std::string folder;
		#ifdef __LINUX__
		found=aFileName.find("private/10003a3f/");
		#else
		found=aFileName.find("private\\10003a3f\\");
		#endif

		if( found != string::npos )
			folder = aFileName.substr(0,found);
		else
		{
				#ifdef __LINUX__
				std::string errMsg= "Failed : Resource File Path should contain /private/10003a3f/";
				#else
				std::string errMsg= "Failed : Resource File Path should contain \\private\\10003a3f\\";
				#endif
				throw CResourceFileException(errMsg);
		}

		return folder;
	}
	else
		return wstring2string(aParamList->RomDrivePath());
}


/**
 * Generate XML for Class AppInfoReader and call the function to Update Installation Information in Database using SRCTOOL
 */
void ReadApplicationInformationFromResourceFilesL(XmlDetails::TScrPreProvisionDetail& aScrPreProvisionDetail, 
															const std::string& aFileName,const CParameterList* aParamList, 
															const CInterpretSIS& aInterpretSis, int& aNewFileFlag)
{

	std::string LocalizePath = GetLocalizeFilePath(aFileName, aParamList);
	
	CAppInfoReader* appInfoReader = NULL;
	appInfoReader = CAppInfoReader::NewL(aFileName, NullUid, LocalizePath);	
	if (!appInfoReader)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	else
	{
		TBool readSuccessful=EFalse;

		readSuccessful= appInfoReader->ReadL();

		if (readSuccessful)
		{
			const SisRegistry* SisReg( aInterpretSis.GetSisRegistry());
			const DbHelper* dbHelper( SisReg->GetDbHelper());
				
			TUid aAppUid = appInfoReader->AppUid();
			int val = dbHelper->IsAppUidInstalled(aAppUid.GetUid());
			if(!val)
			{
				aNewFileFlag = 1;
				// for sis file entries the software type would always be Native.
				aScrPreProvisionDetail.iSoftwareTypeName = L"native";;
						
				XmlDetails::TScrPreProvisionDetail::TComponent component;
				CreateApplicationRegistrationInfoL(component,appInfoReader);
				aScrPreProvisionDetail.iComponents.push_back(component);
			}
			else
			{
				std::cout<<",Ignoring : AppUid "<<hex<<aAppUid.iUid<<" already present in SCR.";
			}
		}
		else
		{
			delete appInfoReader;
			std::string errMsg= "Reading Resource File failed.";
			throw CResourceFileException(errMsg);
		}
		delete appInfoReader;
	}

}

/**
 * Parse the Directory for resource file.
 */

void ParseResourceDir(const CParameterList* aParamList, const CInterpretSIS& aInterpretSis)
{
	std::wstring aFilePath;
	int iNewFileFlag = 0;

	if ( aParamList->IsFlagSet(CParameterList::EFlagsResourceFilePathSet)) 
	{
	 	aFilePath = aParamList->ResourceFilePath();
	}
	else
	{
		 aFilePath = aParamList->RomDrivePath();
		 #ifdef __LINUX__
		 aFilePath.append(L"/private/10003a3f/apps");
		 #else
		 aFilePath.append(L"\\private\\10003a3f\\apps");
		 #endif
	}

	int iCount = 0;
	std::list<std::wstring> regDirs;
	GetDirContents( aFilePath, regDirs );

	// store the object in registry database
	XmlDetails::TScrPreProvisionDetail scrPreProvisionDetail;

	std::list<std::wstring>::iterator curr = regDirs.begin();
	for( curr = regDirs.begin(); curr != regDirs.end(); ++curr )
	{
		try
		{
			if (curr->find(AppResourceFileExtension,0) != std::wstring::npos)
			{
				iCount++;
			    std::string fName;
		        fName = wstring2string( *curr );
				std::string FilePath = wstring2string(aFilePath);
				#ifdef __LINUX__				
				FilePath.append("/");
				#else
				FilePath.append("\\");
				#endif

				FilePath.append(fName);
				std::cout<<"Parsing - "<<fName<<" ";
				ReadApplicationInformationFromResourceFilesL(scrPreProvisionDetail,FilePath,aParamList,aInterpretSis,iNewFileFlag);
				std::cout<<" "<<std::endl;
			}
		}
		catch(const CResourceFileException& aObject)
		{
			//LERROR(L"Resource File Parsing Error - ");
			//aObject.Display();
		}
	}

	if(iNewFileFlag)
		UpdateInstallationInformation_xml(aParamList,scrPreProvisionDetail);
	
	if(!iCount)
		LINFO(L"No Resource File in the Directory Specified - ");
}

/**
 * Backup hash files for re-installation (SA over SA or PU over PU)
 */
void BackupHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath)
{
	#ifdef __LINUX__
	std::wstring hashdir = L"$:/sys/hash/";
	#else
	std::wstring hashdir = L"$:\\sys\\hash\\";
	#endif

	std::wstring basename = aFile.substr( aFile.rfind( KDirectorySeparator ) + 1) ;
	if (basename.size() == 0)
	{
		#ifdef __LINUX__
		basename = aFile.substr(aFile.rfind(L"/"));
		#else
		basename = aFile.substr(aFile.rfind(L"\\"));
		#endif
	}

	hashdir[0] = aDriveLetter;
	#ifdef __LINUX__
	std::wstring hashFile = aPath + L"/sys/hash/" + basename;
	#else
	std::wstring hashFile = aPath + L"\\sys\\hash\\" + basename;
	#endif

	if (FileExists(hashFile))
	{
		std::string iLocalFile = wstring2string(hashFile);
		std::string iBackupFile = wstring2string(hashFile);
		iBackupFile.append("_backup");

		int err=FileCopyA(iLocalFile.c_str(),iBackupFile.c_str(),0);
		if (err != 0)
			LERROR(L"Failed to Backup hash file ");
	}
}

/**
 * Restore hash files for re-installation (SA over SA or PU over PU)
 */
void RestoreHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath)
{
	#ifdef __LINUX__
	std::wstring hashdir = L"$:/sys/hash/";
	#else
	std::wstring hashdir = L"$:\\sys\\hash\\";
	#endif
	std::wstring basename = aFile.substr( aFile.rfind( KDirectorySeparator ) + 1) ;
	if (basename.size() == 0)
	{
		#ifdef __LINUX__
		basename = aFile.substr(aFile.rfind(L"/"));
		#else
		basename = aFile.substr(aFile.rfind(L"\\"));
		#endif
	}

	hashdir[0] = aDriveLetter;
	#ifdef __LINUX__
	std::wstring hashFile = aPath + L"/sys/hash/" + basename;
	#else
	std::wstring hashFile = aPath + L"\\sys\\hash\\" + basename;
	#endif	
	std::wstring LocalFile(hashFile);
	hashFile.append(L"_backup");

	if (FileExists(hashFile))
	{
		std::string iLocalFile = wstring2string(LocalFile);
		std::string iBackupFile = wstring2string(hashFile);

		int err = FileMoveA(iBackupFile.c_str(),iLocalFile.c_str());
		if (err != 0)
		    LERROR(L"Failed to Restore hash file ");
	}
}

/**
 * Returns the size of the first occurance of an invalid directory separator.
 * @param aPath Path to be validated.
 * @param aIndex index from which the search begin. On function return this 
 * 				index will point to the illegal directory separator. 
 * @return 0 if path is valid. Else the number of character to be replaced.
 * e.g. /sys/bin/ should be replaced with \sys\bin\
 * and //sys//bin// should be replaced with \sys\bin\
 */ 

// Constants
#ifdef __LINUX__
const std::wstring KSisDirectorySeparatortap( L"/" );
#else
const std::wstring KSisDirectorySeparatortap( L"\\" );
#endif


int FirstInvalidDirSeparatorSizetap(std::wstring& aPath, std::wstring::size_type& aIndex)
	{
	// If path semantics is correct (as needed by sisx library)
	// then the function will return 0
	int ret = 0; 
	int pos = 0;
	#ifdef __LINUX__
	if((pos = aPath.find(L"\\\\", aIndex)) != std::wstring::npos)
	#else
	if((pos = aPath.find(L"//", aIndex)) != std::wstring::npos)
	#endif
		{
		ret = 2;
		}
	#ifdef __LINUX__
	else if((pos = aPath.find(L"\\", aIndex)) != std::wstring::npos)
	#else
	else if((pos = aPath.find(L"/", aIndex)) != std::wstring::npos)
	#endif
		{
		ret = 1;
		}
	aIndex = pos;
	return ret;
	}

std::wstring FixPathDelimiterstap( const std::wstring& aString )
    {
    std::wstring ret = aString;
	std::wstring::size_type idx = 0;
    int len = 0;
	while(len = FirstInvalidDirSeparatorSizetap(ret, idx))
        {
		ret.replace( idx, len, KSisDirectorySeparatortap );
        }
    return ret;
    }
