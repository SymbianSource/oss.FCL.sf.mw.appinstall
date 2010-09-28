/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dblayer.h"
#include "exception.h"
#include "logs.h"
#include "util.h"
#include "symbiantypes.h"
#include "utility.h"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iostream>

#include <memory>
#include "toolsconf.h"

typedef std::vector<std::string>::iterator StringIterator;
typedef std::vector<std::string>::const_iterator ConstStringIterator;
typedef std::vector<std::wstring>::const_iterator ConstWstringIterator;
typedef std::vector<XmlDetails::TScrEnvironmentDetails>::const_iterator ScrEnvIterator;
typedef std::vector<XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName>::const_iterator ScrEnvLocSwTypeNameIterator;
typedef std::vector<XmlDetails::TScrEnvironmentDetails::TCustomAccessList>::const_iterator ScrEnvCustomAccessIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponent>::const_iterator CompIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentLocalizable>::const_iterator CompLocIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentProperty>::const_iterator CompPropIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>::const_iterator CompFileIter;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty>::const_iterator FilePropIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo>::const_iterator CompApplicationRegistrationInfoIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute>::const_iterator ApplicationAttributeIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType>::const_iterator ApplicationDataTypeIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo>::const_iterator ApplicationServiceInfoIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo>::const_iterator ApplicationLocalizableInfoIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute>::const_iterator LocalizableAttributeIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData>::const_iterator ViewDataIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes>::const_iterator ViewDataAttributeIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty>::const_iterator ApplicationPropertyIterator;


#ifdef __LINUX__

/*
* Platform specific feature
*
* In WINDOWS : sizeof(wchar_t) = 2
* In LINUX   : sizeof(wchar_t) = 4
*/

static utf16WString utf32WString2utf16WString(std::wstring aParameter)
{
	int strLen = aParameter.length();
	const wchar_t * source = aParameter.c_str();
	unsigned short int* buffer = new unsigned short int[strLen + 1];

	// Using a temp variable in place of buffer as ConvertUTF32toUTF16 modifies the source pointer passed.
	unsigned short int* temp = buffer;

	ConvertUTF32toUTF16(&source, source + strLen, &temp,  temp + strLen, lenientConversion);

	// Appending NUL to the converted buffer.
	*temp = 0;

	utf16WString utf16Ws;
	utf16Ws.resize(strLen);

	// The built-in basic_string template class copy operation
	// truncates when a NUL is encountered when a c_str() is
	// used to construct the required string.
	// So, if aParameter is any hashable string having the
	// syntax : swtypeName + L'\0' + someId then, we will end
	// up returning only part of the converted UTF-16 string.
	// Hence, we resort to the explicit copy operation with
	// two bytes at a time.
	while( strLen-- )
	{
		utf16Ws[ strLen ] = buffer[ strLen ];
	}

	delete[] buffer;

	return utf16Ws;
}

#else

// We need not do anything for WINDOWS, since the windows wstring
// will already be in UTF-16 encoding.
#define utf32WString2utf16WString(aParameter) (aParameter)

#endif



const int KMaxDrives=26;

CDbLayer::CDbLayer(const std::string& aDllPath, const std::string& aSqlDbName)
	{
	iScrDbHandler = new CDbProcessor(aDllPath, aSqlDbName);
	}

CDbLayer::~CDbLayer()
	{
	delete iScrDbHandler;
	}

void CDbLayer::CreateScrDatabase(const std::vector<std::string>& aCreateDbQueries)
	{
	for (ConstStringIterator schemaIterator=aCreateDbQueries.begin() ; schemaIterator < aCreateDbQueries.end(); ++schemaIterator )
		{
		ExecuteStatement(*schemaIterator);
		}
	}	

void CDbLayer::PopulateScrDatabase(const std::vector<XmlDetails::TScrEnvironmentDetails>& aScrEnvDetails)
	{
	try
		{
		std::string beginTransaction("BEGIN;");
		ExecuteStatement(beginTransaction);
		PopulateDatabase(aScrEnvDetails);
		std::string commitTransaction("COMMIT;");
		ExecuteStatement(commitTransaction);
		}
		catch(CException& /*aException*/)
			{
			std::string rollbackTransaction("ROLLBACK;");
			ExecuteStatement(rollbackTransaction);
			std::string errMsg = "Failed to populate SCR database with environment details.";
			LOGERROR(errMsg);
			throw CException(errMsg,ExceptionCodes::ESqlCorrupt);
			}
	}

void CDbLayer::PopulatePreProvisionDetails(const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetailList)
	{
	try
		{
		LOGENTER("CDbLayer::PopulatePreProvisionDetails()");
		AddPreProvisionDetails(aPreProvisionDetailList);
		}
		catch(CException& aException)
			{
			std::string rollbackTransaction("ROLLBACK;");
			ExecuteStatement(rollbackTransaction);
			std::string errMsg = "Failed to populate SCR database with pre provision details.";
			LOGERROR(aException.GetMessageA());
			throw CException(errMsg,ExceptionCodes::ESqlCorrupt);
			}
	}

void ExecuteSwTypeNameStatement(const std::auto_ptr<CStatement>& aStatement, unsigned long aSwTypeId, TInt aLocale, const std::wstring& aSwTypeName)
	{
	aStatement->BindInt(1, aSwTypeId);
	aStatement->BindInt(2, aLocale);
	aStatement->BindStr(3, aSwTypeName);
	aStatement->ExecuteStatement();
	aStatement->Reset();
	}

void CDbLayer::PopulateDatabase(const std::vector<XmlDetails::TScrEnvironmentDetails>& aScrEnvDetails)
	{
	LOGENTER("CDbLayer::PopulateDatabase()");
	std::string insertSoftwareType("INSERT INTO SoftwareTypes(SoftwareTypeId,SifPluginUid,LauncherExecutable) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtSwType(iScrDbHandler->PrepareStatement(insertSoftwareType));
	
	std::string insertSTWithoutLE("INSERT INTO SoftwareTypes(SoftwareTypeId,SifPluginUid) VALUES(?,?);");
	std::auto_ptr<CStatement> stmtSwTypeWithoutLE(iScrDbHandler->PrepareStatement(insertSTWithoutLE));
	
    std::string insertCustomAccessList("INSERT INTO CustomAccessList(SoftwareTypeId,SecureId,AccessMode) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtCustomAccessList(iScrDbHandler->PrepareStatement(insertCustomAccessList));
	
	std::string insertSwTypeName("INSERT INTO SoftwareTypeNames(SoftwareTypeId,Locale,Name) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtSwTypeName(iScrDbHandler->PrepareStatement(insertSwTypeName));
		
	std::string insertMimeType("INSERT INTO MimeTypes(SoftwareTypeId,MimeType) VALUES(?,?);");
	std::auto_ptr<CStatement> stmtMimeType(iScrDbHandler->PrepareStatement(insertMimeType));

	for(ScrEnvIterator aScrEnvIterator = aScrEnvDetails.begin(); aScrEnvIterator != aScrEnvDetails.end(); ++aScrEnvIterator)
		{
		// To maintain the consistency of CRC value across the platforms(WINDOWS and LINUX), we are
		// using UTF-16 string in CRC generation.
		utf16WString utf16Ws = utf32WString2utf16WString(aScrEnvIterator->iUniqueSoftwareTypeName);
		unsigned int swTypeId = Util::Crc32(utf16Ws.c_str(),aScrEnvIterator->iUniqueSoftwareTypeName.length()*2);

		if (!aScrEnvIterator->iLauncherExecutable.empty())
		{
		stmtSwType->BindInt(1, swTypeId);
		stmtSwType->BindInt(2, aScrEnvIterator->iSifPluginUid);
		stmtSwType->BindStr(3, aScrEnvIterator->iLauncherExecutable);
		stmtSwType->ExecuteStatement();
		stmtSwType->Reset();
		}
		else
		{
		stmtSwTypeWithoutLE->BindInt(1, swTypeId);
		stmtSwTypeWithoutLE->BindInt(2, aScrEnvIterator->iSifPluginUid);
		stmtSwTypeWithoutLE->ExecuteStatement();
		stmtSwTypeWithoutLE->Reset();
		}
		// First insert unique sw type name
		const TInt uniqueSwTypeNameLocale = 0;
		ExecuteSwTypeNameStatement(stmtSwTypeName, swTypeId, uniqueSwTypeNameLocale, aScrEnvIterator->iUniqueSoftwareTypeName);
		// Then, insert localized sw type names.
		for(ScrEnvLocSwTypeNameIterator swTypeNameIter = aScrEnvIterator->iLocalizedSoftwareTypeNames.begin(); swTypeNameIter != aScrEnvIterator->iLocalizedSoftwareTypeNames.end(); ++swTypeNameIter)
			{
			ExecuteSwTypeNameStatement(stmtSwTypeName, swTypeId, swTypeNameIter->iLocale, swTypeNameIter->iName);
			}
                
        for(ScrEnvCustomAccessIterator customAccessIter = aScrEnvIterator->iCustomAccessList.begin(); customAccessIter != aScrEnvIterator->iCustomAccessList.end(); ++customAccessIter)
			{
        	stmtCustomAccessList->BindInt(1, swTypeId);
            stmtCustomAccessList->BindInt(2, customAccessIter->iSecureId);
            stmtCustomAccessList->BindInt(3, customAccessIter->iAccessMode);
            stmtCustomAccessList->ExecuteStatement();
            stmtCustomAccessList->Reset();
			}
		
		for(ConstWstringIterator mimeIter= aScrEnvIterator->iMIMEDetails.begin(); mimeIter != aScrEnvIterator->iMIMEDetails.end(); ++mimeIter)
			{
			stmtMimeType->BindInt(1, swTypeId);
			stmtMimeType->BindStr(2, *mimeIter);
			stmtMimeType->ExecuteStatement();
			stmtMimeType->Reset();
			}
		}
	LOGEXIT("CDbLayer::PopulateDatabase()");

	}

void CDbLayer::AddPreProvisionDetails(const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetailList)
	{
	LOGENTER("CDbLayer::AddPreProvisionDetails()");
	for(CompIterator aCompIterator = aPreProvisionDetailList.iComponents.begin(); aCompIterator != aPreProvisionDetailList.iComponents.end(); ++aCompIterator)
		{	
		std::string beginTransaction("BEGIN;");
		ExecuteStatement(beginTransaction);
		
		int componentId = 0;
		if (!AddComponentDetails(*aCompIterator, aPreProvisionDetailList.iSoftwareTypeName))
			{
			componentId = iScrDbHandler->LastInsertedId();
			AddComponentLocalizables(componentId,aCompIterator->iComponentLocalizables);
			AddComponentProperties(componentId,aCompIterator->iComponentProperties);
			AddComponentFiles(componentId,aCompIterator->iComponentFiles);
			AddComponentDependencies(componentId, aCompIterator->iComponentDependency, aPreProvisionDetailList.iSoftwareTypeName);
			}
		AddApplicationRegistrationInfo(componentId,aCompIterator->iApplicationRegistrationInfo);
		std::string commitTransaction("COMMIT;");
		ExecuteStatement(commitTransaction);

		}
	LOGEXIT("CDbLayer::AddPreProvisionDetails()");
	}

bool CDbLayer::AddComponentDetails(const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::wstring& aSoftwareTypeName)
	{
	LOGENTER("CDbLayer::AddComponentDetails()");
	std::string insertComponents;
	XmlDetails::TScrPreProvisionDetail::TComponentDetails 
		componentDetail = aComponent.iComponentDetails;
	if (aComponent.iComponentDetails.iIsRomApplication)
		{
		LOGINFO("Is rom app");
		return true;
		}
	LOGINFO("Not rom app");

	// To maintain the consistency of CRC value across the platforms(WINDOWS and LINUX), we are
	// using UTF-16 string in CRC generation.
    utf16WString utf16Ws = utf32WString2utf16WString(aSoftwareTypeName);
	unsigned int swTypeId = Util::Crc32(utf16Ws.c_str(),aSoftwareTypeName.length()*2);


	std::wstring strGlobalId = componentDetail.iGlobalId;
	
	if(!strGlobalId.empty())
		{
		insertComponents = "INSERT INTO Components(SoftwareTypeId, SoftwareTypeName, Removable, Size, ScomoState, InstalledDrives, OriginVerified, Hidden, GlobalIdHash, GlobalId, Version, InstallTime) VALUES(?,?,?,?,?,?,?,?,?,?,?,?);";
		}
	else
		{
		insertComponents = "INSERT INTO Components(SoftwareTypeId, SoftwareTypeName, Removable, Size, ScomoState, InstalledDrives, OriginVerified, Hidden, Version, InstallTime) VALUES(?,?,?,?,?,?,?,?,?,?);";
		}	
		
	std::auto_ptr<CStatement> stmtComponents(iScrDbHandler->PrepareStatement(insertComponents));

	
	stmtComponents->BindInt(1, swTypeId);
	stmtComponents->BindStr(2, aSoftwareTypeName);
	stmtComponents->BindInt(3, componentDetail.iIsRemovable);
	stmtComponents->BindInt64(4, componentDetail.iSize);
	stmtComponents->BindInt(5, componentDetail.iScomoState);
	
	// the installed drives has to be retrieved and calculated from the list of files
	int installedDrives = GetInstalledDrives(aComponent.iComponentFiles);
			
	stmtComponents->BindInt(6, installedDrives);
	stmtComponents->BindInt(7, componentDetail.iOriginVerified);
	stmtComponents->BindInt(8, componentDetail.iIsHidden);

	std::wstring version;
	version =	componentDetail.iVersion.iMajor+L"." \
				+ componentDetail.iVersion.iMinor+L"." \
				+ componentDetail.iVersion.iBuild ;

	std::wstring localTime = GetLocalTime();
	
	if(!strGlobalId.empty())
		{
		std::wstring concatGlobalId = aSoftwareTypeName + L'\0' + strGlobalId;

		// To maintain the consistency of CRC value across the platforms(WINDOWS and LINUX), we are
		// using UTF-16 string in CRC generation.
		utf16WString utf16Ws = utf32WString2utf16WString(concatGlobalId);
		unsigned int globalIdHash = Util::Crc32(utf16Ws.c_str(),concatGlobalId.length()*2);

		stmtComponents->BindInt(9, globalIdHash);
		stmtComponents->BindStr(10, concatGlobalId);
		stmtComponents->BindStr(11, version);
		stmtComponents->BindStr(12, localTime);
		}
	else
		{
		stmtComponents->BindStr(9, version);
		stmtComponents->BindStr(10, localTime);
		}
	
	stmtComponents->ExecuteStatement();
	stmtComponents->Reset();
	LOGEXIT("CDbLayer::AddComponentDetails()");
	return false;
	}

int CDbLayer::GetInstalledDrives(const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>& aComponentFiles )
	{
	int installedDrives = 0;
	for(CompFileIter compFileIter = aComponentFiles.begin(); compFileIter != aComponentFiles.end(); ++compFileIter)
		{
		if(compFileIter->iLocation.empty())
			continue;
			
			char drive = static_cast<char>(compFileIter->iLocation[0]);
			drive = tolower(drive);

			if(drive < 'a' || drive > 'z' )
				{
				throw CException("Invalid drive specified in file location.",ExceptionCodes::EInvalidArgument);
				}

			installedDrives |= 1 << (drive-'a');
			
		}
	return installedDrives;
	}

const std::wstring CDbLayer::GetLocalTime()
	{
	time_t localTime;
	localTime = time(NULL);
	tm* ttm = localtime(&localTime);
	
	const std::wstring year		= Util::IntegerToWideString(ttm->tm_year + 1900);
	const std::wstring month	= Util::IntegerToWideString(ttm->tm_mon+1);
	const std::wstring day		= Util::IntegerToWideString(ttm->tm_mday);
	const std::wstring hour		= Util::IntegerToWideString(ttm->tm_hour);
	const std::wstring minute	= Util::IntegerToWideString(ttm->tm_min);
	const std::wstring second	= Util::IntegerToWideString(ttm->tm_sec);

	wchar_t result[16]={0};
	result[16] = '0';
	
#ifdef GCC_COMPILER
	swprintf(result,(sizeof(result)/sizeof(wchar_t)),L"%04s%02s%02s:%02s%02s%02s",year.c_str(),month.c_str(),day.c_str(),hour.c_str(),minute.c_str(),second.c_str());
#else	
	swprintf(result,L"%04s%02s%02s:%02s%02s%02s",year.c_str(),month.c_str(),day.c_str(),hour.c_str(),minute.c_str(),second.c_str());
#endif
	std::wstring wstr(result);
	return wstr;
	}

void CDbLayer::AddComponentLocalizables( int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentLocalizable>& aComponentLocalizable)
	{
	LOGENTER("CDbLayer::AddComponentLocalizables()");
	std::string insertComponentLocalizable("INSERT INTO ComponentLocalizables(ComponentId,Locale,Name,Vendor) VALUES(?,?,?,?);");
	std::auto_ptr<CStatement> stmtComponentLocalizable(iScrDbHandler->PrepareStatement(insertComponentLocalizable));

	for(CompLocIterator compLocIter = aComponentLocalizable.begin(); compLocIter != aComponentLocalizable.end() ; ++compLocIter )
		{
		stmtComponentLocalizable->BindInt(1, aComponentId);
		stmtComponentLocalizable->BindInt(2, compLocIter->iLocale);
		stmtComponentLocalizable->BindStr(3, compLocIter->iName);
		stmtComponentLocalizable->BindStr(4, compLocIter->iVendor);
		stmtComponentLocalizable->ExecuteStatement();
		stmtComponentLocalizable->Reset();
		}
	LOGEXIT("CDbLayer::AddComponentLocalizables()");
	}

void CDbLayer::AddComponentProperties( 
	int aComponentId, 
	const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentProperty>& aComponentProperty)
	{
	LOGENTER("CDbLayer::AddComponentProperties()");
	std::string insertComponentProperties;
	
	for(CompPropIterator compPropIter = aComponentProperty.begin(); compPropIter != aComponentProperty.end() ; ++compPropIter )
		{
		if(compPropIter->iIsIntValue == 1)
			{
			insertComponentProperties = "INSERT INTO ComponentProperties(Name,Locale,ComponentId,IntValue,IsStr8Bit) VALUES(?,?,?,?,?);";
			}
		else
			{
			insertComponentProperties = "INSERT INTO ComponentProperties(Name,Locale,ComponentId,StrValue,IsStr8Bit) VALUES(?,?,?,?,?);";
			}
		std::auto_ptr<CStatement> stmtComponentProperty(iScrDbHandler->PrepareStatement(insertComponentProperties));

		stmtComponentProperty->BindStr(1, compPropIter->iName);
		stmtComponentProperty->BindInt(2, compPropIter->iLocale);
		stmtComponentProperty->BindInt(3, aComponentId);
		if(compPropIter->iIsIntValue == 1)
			{
			TInt64 intValue = Util::WideCharToInt64(compPropIter->iValue.c_str());
			stmtComponentProperty->BindInt64(4, intValue);
			}
		else
			{
			if(compPropIter->iIsStr8Bit)
				{
				std::string str = wstring2string(compPropIter->iValue);
				std::string decodedString = Util::Base64Decode(str);
				stmtComponentProperty->BindBinary(4, decodedString);
				}
			else
				{
				stmtComponentProperty->BindStr(4, compPropIter->iValue);
				}
			}

		
		stmtComponentProperty->BindInt(5, compPropIter->iIsStr8Bit);
	
		stmtComponentProperty->ExecuteStatement();
		stmtComponentProperty->Reset();
		}
	LOGEXIT("CDbLayer::AddComponentProperties()");
	}

void CDbLayer::AddComponentFiles(int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>& aComponentFiles)
	{
	LOGENTER("CDbLayer::AddComponentFiles()");
	for(CompFileIter compFile = aComponentFiles.begin() ; compFile != aComponentFiles.end() ; ++compFile)
		{
		AddLocation(aComponentId,compFile->iLocation);
		int cmpFileId = iScrDbHandler->LastInsertedId();
		AddFileProperties(cmpFileId,compFile->iFileProperties);
		}
	LOGEXIT("CDbLayer::AddComponentFiles()");
	}


void CDbLayer::AddComponentDependencies	(	int aComponentId, 
											const XmlDetails::TScrPreProvisionDetail::TComponentDependency& aComponentDependency,
											const std::wstring& aSoftwareTypeName
										)
	{
	LOGENTER("CDbLayer::AddComponentDependencies()");
	std::wstring dependentId = aComponentDependency.iDependentId;

	if(dependentId.empty())
		return;
	
	std::wstring dependantGlobalId = aSoftwareTypeName + L'\0' + dependentId;
	
	const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail> 
		aComponentDependencyDetails = aComponentDependency.iComponentDependencyList;

	std::vector<XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail>::const_iterator compDepIter;

	for(compDepIter = aComponentDependencyDetails.begin() ; compDepIter != aComponentDependencyDetails.end() ; ++compDepIter)
		{
		std::wstring concatGlobalId = dependantGlobalId + compDepIter->iSupplierId;

		// To maintain the consistency of CRC value across the platforms(WINDOWS and LINUX), we are
		// using UTF-16 string in CRC generation.
		utf16WString utf16Ws = utf32WString2utf16WString(concatGlobalId);
		unsigned int globalIdHash = Util::Crc32(utf16Ws.c_str(),concatGlobalId.length()*2);

		utf16Ws = utf32WString2utf16WString(dependantGlobalId);
		unsigned int dependantIdHash = Util::Crc32(utf16Ws.c_str(),dependantGlobalId.length()*2);

		utf16Ws = utf32WString2utf16WString(compDepIter->iSupplierId);
		unsigned int supplierIdHash = Util::Crc32(utf16Ws.c_str(),compDepIter->iSupplierId.length()*2);


		std::string insertComponentDeps("INSERT INTO ComponentDependencies(GlobalIdHash,DependantGlobalIdHash, SupplierGlobalIdHash, DependantGlobalId,SupplierGlobalId,VersionFrom,VersionTo) VALUES(?,?,?,?,?,?,?);");
		std::auto_ptr<CStatement> stmtComponentDeps(iScrDbHandler->PrepareStatement(insertComponentDeps));
		
		stmtComponentDeps->BindInt( 1 ,globalIdHash);
		stmtComponentDeps->BindInt( 2 ,dependantIdHash);
		stmtComponentDeps->BindInt( 3 ,supplierIdHash);
		stmtComponentDeps->BindStr( 4 ,dependentId);
		stmtComponentDeps->BindStr( 5 ,compDepIter->iSupplierId);
		stmtComponentDeps->BindStr( 6 ,compDepIter->iFromVersion);
		stmtComponentDeps->BindStr( 7 ,compDepIter->iToVersion);
		
		stmtComponentDeps->ExecuteStatement();
		stmtComponentDeps->Reset();
	
		}	
	LOGEXIT("CDbLayer::AddComponentDependencies()");
	}


void CDbLayer::AddLocation(int aComponentId,  const std::wstring& aLocation)
	{
	std::string insertComponentFileDetails("INSERT INTO ComponentsFiles(ComponentId,LocationHash,Location) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtComponentFileDetails(iScrDbHandler->PrepareStatement(insertComponentFileDetails));
	
	stmtComponentFileDetails->BindInt(1,aComponentId);
	
	int length = aLocation.length()*2 ;

	// generate hash for location
	std::wstring location = aLocation;
	std::transform(	location.begin(), location.end(), location.begin(), tolower);

	#ifdef __TOOLS2_LINUX__

	// To maintain the consistency of the LocationHash value(essentially the CRC of
	// Location string) across the WINDOWS and LINUX platforms, we reconstruct the
	// location to have WINDOWS specific path.

    std::wstring::size_type idx = 0;
     while( (idx = location.find(L"//", idx)) != std::wstring::npos)
     {
    	 location.replace( idx, 2, L"\\\\" );
     }

     idx = 0;

     while( (idx = location.find(L"/", idx)) != std::wstring::npos)
     {
    	 location.replace( idx, 1, L"\\" );
     }

	#endif


	// To maintain the consistency of CRC value across the platforms(WINDOWS and LINUX), we are
	// using UTF-16 string in CRC generation.

	utf16WString utf16Ws = utf32WString2utf16WString(location);
	unsigned int hash = Util::Crc32(utf16Ws.c_str(),length);

	stmtComponentFileDetails->BindInt(2,hash);
	stmtComponentFileDetails->BindStr(3,aLocation);
	stmtComponentFileDetails->ExecuteStatement();
	stmtComponentFileDetails->Reset();
		
	}

void CDbLayer::AddFileProperties(int aCmpFileId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty>& aFileProperty)
	{
	std::string insertFileProperties;
	
	for(FilePropIterator filePropIter = aFileProperty.begin(); filePropIter != aFileProperty.end() ; ++filePropIter )
		{
		if(filePropIter->iIsIntValue == 1)
			{
			insertFileProperties = "INSERT INTO FileProperties(Name,CmpFileId,IntValue) VALUES(?,?,?);";
			}
		else
			{
			insertFileProperties = "INSERT INTO FileProperties(Name,CmpFileId,StrValue,IsStr8Bit) VALUES(?,?,?,?);";
			}
		std::auto_ptr<CStatement> stmtFileProperty(iScrDbHandler->PrepareStatement(insertFileProperties));

		stmtFileProperty->BindStr(1, filePropIter->iName);
		stmtFileProperty->BindInt(2, aCmpFileId);
		
		// we know by default the locale is 0 for file properties 
		// hence for setting the IsStr8Bit all we need to check is whether
		// IntValue is present or not
		if(filePropIter->iIsIntValue)
			{
			int intValue = Util::WideCharToInteger(filePropIter->iValue.c_str());
			stmtFileProperty->BindInt(3, intValue);
			}
		else
			{
			std::string str = wstring2string(filePropIter->iValue);
			std::string decodedString = Util::Base64Decode(str);
			stmtFileProperty->BindBinary(3, decodedString);
			stmtFileProperty->BindInt(4, 1);
			}

		stmtFileProperty->ExecuteStatement();
		stmtFileProperty->Reset();
		}
	}


void CDbLayer::AddApplicationRegistrationInfo( int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo>& aApplicationRegistrationInfo)
	{
	LOGENTER("CDbLayer::AddApplicationRegistrationInfo()");
	int appflag = 0;
	int appUid = 0;
	int error = 0;

	for(CompApplicationRegistrationInfoIterator compApplicationRegistrationInfoIter = aApplicationRegistrationInfo.begin(); compApplicationRegistrationInfoIter != aApplicationRegistrationInfo.end() ; ++compApplicationRegistrationInfoIter )
	{
		try {
			appflag = 0;
			appUid = AddAppAttribute(aComponentId, compApplicationRegistrationInfoIter->iApplicationAttribute);
		}
		catch(CException& aException)
		{
			LOGERROR(aException.GetMessageA());
			LOGERROR("one of AddAppAttribute is already present in database");
			appflag = 1;
			error = 1;
		}

		if(appflag)
			continue;
				
		if (appUid != 0)
			{
			AddFileOwnershipInfo(appUid, compApplicationRegistrationInfoIter->iFileOwnershipInfo);
			AddServiceInfo(appUid, compApplicationRegistrationInfoIter->iApplicationServiceInfo);
			AddAppLocalizableInfo(appUid, compApplicationRegistrationInfoIter->iApplicationLocalizableInfo);
			AddProperty(appUid, compApplicationRegistrationInfoIter->iApplicationProperty);
			}
		else
			{
			std::string errMsg = "AppUid is null.";
			LOGERROR(errMsg);
			throw CException(errMsg,ExceptionCodes::ESqlArgumentError);
			}
	}

	if(error)
	{
		std::string errMsg = "Duplicate entry present in database.";
		LOGERROR(errMsg);
		throw CException(errMsg,ExceptionCodes::EFileExists);
	}

	LOGEXIT("CDbLayer::AddApplicationRegistrationInfo()");
	}

int CDbLayer::AddAppAttribute( int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute>& aAppAttribute)
	{
	LOGENTER("CDbLayer::AddAppAttribute()");
	std::string insertAppAttributes;
	
	insertAppAttributes = "INSERT INTO AppRegistrationInfo(AppUid,ComponentId,AppFile,TypeId,Attributes,Hidden,Embeddable,Newfile,Launch,GroupName,DefaultScreenNumber) VALUES(?,?,?,?,?,?,?,?,?,?,?);";
	std::auto_ptr<CStatement> stmtAppAttribute(iScrDbHandler->PrepareStatement(insertAppAttributes));

	stmtAppAttribute->BindInt64(2, aComponentId);

	//Assigning Default Values
	TInt64 intVal = 0; 
	stmtAppAttribute->BindInt64(1, intVal); //AppUid
	stmtAppAttribute->BindInt64(4, intVal); //TypeId
	stmtAppAttribute->BindInt64(5, intVal); //Attributes
	stmtAppAttribute->BindInt64(6, intVal); //Hidden
	stmtAppAttribute->BindInt64(7, intVal); //Embeddable
	stmtAppAttribute->BindInt64(8, intVal); //Newfile
	stmtAppAttribute->BindInt64(9, intVal); //Launch
	stmtAppAttribute->BindInt64(11, intVal); //DefaultScreenNumber
	std::wstring strVal(L"");
	stmtAppAttribute->BindStr(3, strVal);  //AppFile
	stmtAppAttribute->BindStr(10, strVal); //GroupName
	
	int appUid = 0;
	std::string appfile;
	for(ApplicationAttributeIterator applicationAttributeIter = aAppAttribute.begin(); applicationAttributeIter != aAppAttribute.end() ; ++applicationAttributeIter )
		{
		if (applicationAttributeIter->iName == L"AppUid")
			{
			LOGINFO("CDbLayer::AddAppAttribute()- appuid");
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(1, intValue);
			appUid = intValue;
			}
		else if (applicationAttributeIter->iName == L"AppFile")
			{
			stmtAppAttribute->BindStr(3, applicationAttributeIter->iValue);
			if (applicationAttributeIter->iValue.length() == 0)
				{
				std::string errMsg = "Invalid app file.";
				LOGERROR(errMsg);
				throw CException(errMsg,ExceptionCodes::ESqlArgumentError);
				}
			
			}
		else if (applicationAttributeIter->iName == L"TypeId")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(4, intValue);
			}
		else if (applicationAttributeIter->iName == L"Attributes")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(5, intValue);
			}
		else if (applicationAttributeIter->iName == L"Hidden")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(6, intValue);
			}
		else if (applicationAttributeIter->iName == L"Embeddable")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(7, intValue);
			}
		else if (applicationAttributeIter->iName == L"Newfile")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(8, intValue);
			}
		else if (applicationAttributeIter->iName == L"Launch")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(9, intValue);
			}
		else if (applicationAttributeIter->iName == L"GroupName")
			{
			stmtAppAttribute->BindStr(10, applicationAttributeIter->iValue);
			}
		else if (applicationAttributeIter->iName == L"DefaultScreenNumber")
			{
			TInt64 intValue = Util::WideCharToInt64(applicationAttributeIter->iValue.c_str());
			stmtAppAttribute->BindInt64(11, intValue);
			}
		else
			{
			std::string errMsg = "Invalid application attribute.";
			LOGERROR(errMsg);
			throw CException(errMsg,ExceptionCodes::ESqlArgumentError);
			}
		}

		try
		{
			stmtAppAttribute->ExecuteStatement();
			stmtAppAttribute->Reset();
		}
		catch(CException& aException)
		{
			LOGERROR(aException.GetMessageA());
			LOGERROR("AppUid Already Present in Database");
		}

	LOGEXIT("CDbLayer::AddAppAttribute()");
	return appUid;
	}

void CDbLayer::AddFileOwnershipInfo( int aAppUid, const std::vector<std::wstring>& aFileOwnershipInfo)
	{
	LOGENTER("CDbLayer::AddFileOwnershipInfo()");
	std::string insertFileOwnershipInfo;
	
	for(ConstWstringIterator cWstringIter = aFileOwnershipInfo.begin(); cWstringIter != aFileOwnershipInfo.end() ; ++cWstringIter )
		{  
		insertFileOwnershipInfo = "INSERT INTO FileOwnershipInfo(AppUid,FileName) VALUES(?,?);";
		std::auto_ptr<CStatement> stmtFileOwnershipInfo(iScrDbHandler->PrepareStatement(insertFileOwnershipInfo));
		
		stmtFileOwnershipInfo->BindInt64(1, aAppUid);
		stmtFileOwnershipInfo->BindStr(2, *cWstringIter);
			
		stmtFileOwnershipInfo->ExecuteStatement();
		stmtFileOwnershipInfo->Reset();
		}
	LOGEXIT("CDbLayer::AddFileOwnershipInfo()");
	}

void CDbLayer::AddServiceInfo( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo>& aApplicationServiceInfo)
	{
	LOGENTER("CDbLayer::AddServiceInfo()");
	std::string insertServiceInfo;
	
	for(ApplicationServiceInfoIterator serviceInfoIter = aApplicationServiceInfo.begin(); serviceInfoIter != aApplicationServiceInfo.end() ; ++serviceInfoIter )
		{  
		insertServiceInfo = "INSERT INTO ServiceInfo(AppUid,Uid) VALUES(?,?);";
		std::auto_ptr<CStatement> stmtServiceInfo(iScrDbHandler->PrepareStatement(insertServiceInfo));
		
		stmtServiceInfo->BindInt64(1, aAppUid);
		stmtServiceInfo->BindInt64(2, serviceInfoIter->iUid);
		stmtServiceInfo->ExecuteStatement();
		stmtServiceInfo->Reset();
		
		int serviceId = iScrDbHandler->LastInsertedId();
		std::string insertDataType;
		
		const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType>& datatype = serviceInfoIter->iDataType;
		for(ApplicationDataTypeIterator dataTypeIter = datatype.begin(); dataTypeIter != datatype.end() ; ++dataTypeIter )
			{  
			insertDataType = "INSERT INTO DataType(ServiceId,Priority,Type) VALUES(?,?,?);";
			std::auto_ptr<CStatement> stmtDataType(iScrDbHandler->PrepareStatement(insertDataType));
			
			stmtDataType->BindInt64(1, serviceId);
			stmtDataType->BindInt64(2, dataTypeIter->iPriority);
			#ifdef __TOOLS2_LINUX__
			stmtDataType->BindStr(3, dataTypeIter->iType, avoidSlashConversion);
			#else
			stmtDataType->BindStr(3, dataTypeIter->iType);
			#endif
				
			stmtDataType->ExecuteStatement();
			stmtDataType->Reset();
			}
		}
	LOGEXIT("CDbLayer::AddServiceInfo()");
	}

void CDbLayer::AddAppLocalizableInfo( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo>& aApplicationLocalizableInfo)
{
	LOGENTER("CDbLayer::AddAppLocalizableInfo()");
	for(ApplicationLocalizableInfoIterator localizableInfoIter = aApplicationLocalizableInfo.begin(); localizableInfoIter != aApplicationLocalizableInfo.end() ; ++localizableInfoIter )
	{
		AddLocalizableAttribute(aAppUid, localizableInfoIter->iLocalizableAttribute);
		int localAppInfoId = iScrDbHandler->LastInsertedId();
		AddViewData(localAppInfoId, localizableInfoIter->iViewData);
	}
	LOGEXIT("CDbLayer::AddAppLocalizableInfo()");
}

void CDbLayer::AddLocalizableAttribute( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute>& aApplicationLocalizableAttribute)
{
	LOGENTER("CDbLayer::AddLocalizableAttribute()");
	std::string insertAppLocalizableInfo;
		
	insertAppLocalizableInfo = "INSERT INTO LocalizableAppInfo(AppUid,ShortCaption,GroupName,Locale,CaptionAndIconId) VALUES(?,?,?,?,?);";
	std::auto_ptr<CStatement> stmtAppLocalizableInfo(iScrDbHandler->PrepareStatement(insertAppLocalizableInfo));

	//Assigning default value
	TInt64 intVal = 0;
	stmtAppLocalizableInfo->BindInt64(4, intVal); //Locale
	stmtAppLocalizableInfo->BindInt64(5, intVal); //CaptionAndIconId
	std::wstring strVal(L"");
	stmtAppLocalizableInfo->BindStr(2, strVal);   //ShortCaption
	stmtAppLocalizableInfo->BindStr(3, strVal);   //GroupName
	
	std::string insertCaptionAndIconInfo;
		
	insertCaptionAndIconInfo = "INSERT INTO CaptionAndIconInfo(Caption,NumberOfIcons,IconFile) VALUES(?,?,?);";
	std::auto_ptr<CStatement> stmtCaptionAndIconInfo(iScrDbHandler->PrepareStatement(insertCaptionAndIconInfo));

	//Assigning default value
	stmtCaptionAndIconInfo->BindStr(1, strVal);   //Caption
	stmtCaptionAndIconInfo->BindInt64(2, intVal); //NumberOfIcons
	stmtCaptionAndIconInfo->BindStr(3, strVal);   //IconFile


	bool captionAndIconInfoPresent = 0;
	//for every TLocalizableAttribute
	stmtAppLocalizableInfo->BindInt64(1, aAppUid);
	for(LocalizableAttributeIterator localizableAttributeIter = aApplicationLocalizableAttribute.begin(); localizableAttributeIter != aApplicationLocalizableAttribute.end() ; ++localizableAttributeIter )
	{
		if (localizableAttributeIter->iName == L"ShortCaption")
		{
			stmtAppLocalizableInfo->BindStr(2, localizableAttributeIter->iValue);
		}
		else if(localizableAttributeIter->iName == L"GroupName")
		{
			stmtAppLocalizableInfo->BindStr(3, localizableAttributeIter->iValue);
		}
		else if(localizableAttributeIter->iName == L"Locale")
		{
			TInt64 intValue = Util::WideCharToInt64(localizableAttributeIter->iValue.c_str());
			stmtAppLocalizableInfo->BindInt64(4, intValue);
		}
		else if(localizableAttributeIter->iName == L"Caption")
		{
			stmtCaptionAndIconInfo->BindStr(1, localizableAttributeIter->iValue);
			captionAndIconInfoPresent = 1;
		}
		else if(localizableAttributeIter->iName == L"NumberOfIcons")
		{
			TInt64 intValue = Util::WideCharToInt64(localizableAttributeIter->iValue.c_str());
			stmtCaptionAndIconInfo->BindInt64(2, intValue);
			captionAndIconInfoPresent = 1;
		}
		else if(localizableAttributeIter->iName == L"IconFile")
		{
			stmtCaptionAndIconInfo->BindStr(3, localizableAttributeIter->iValue);
			captionAndIconInfoPresent = 1;
		}
		else
		{
			std::string errMsg = "Invalid localizable attribute.";
			LOGERROR(errMsg);
			throw CException(errMsg,ExceptionCodes::ESqlArgumentError);
		}
	}
	if (captionAndIconInfoPresent)
	{
		stmtCaptionAndIconInfo->ExecuteStatement();
		stmtCaptionAndIconInfo->Reset();
		int captionAndIconIdForLocalizableInfo = iScrDbHandler->LastInsertedId();
		stmtAppLocalizableInfo->BindInt64(5, captionAndIconIdForLocalizableInfo);
	}
	stmtAppLocalizableInfo->ExecuteStatement();
	stmtAppLocalizableInfo->Reset();
	LOGEXIT("CDbLayer::AddLocalizableAttribute()");
}


void CDbLayer::AddViewData( int alocalAppInfoId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData>& aViewData)
{
	LOGENTER("CDbLayer::AddViewData()");

	for(ViewDataIterator viewDataIter = aViewData.begin(); viewDataIter != aViewData.end() ; ++viewDataIter )
	{
		AddViewDataAttributes(alocalAppInfoId, viewDataIter->iViewDataAttributes);
	}

	LOGEXIT("CDbLayer::AddViewData()");
}


void CDbLayer::AddViewDataAttributes( int alocalAppInfoId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes>& aViewDataAttribute)
	{
	LOGENTER("CDbLayer::AddViewData()");
	std::string insertViewData;

	insertViewData = "INSERT INTO ViewData(LocalAppInfoId,Uid,ScreenMode,CaptionAndIconId) VALUES(?,?,?,?);";
	std::auto_ptr<CStatement> stmtViewData(iScrDbHandler->PrepareStatement(insertViewData));


	//Assigning Default Value
	TInt64 intVal = 0;
	stmtViewData->BindInt64(2, intVal); //Uid
	stmtViewData->BindInt64(3, intVal); //ScreenMode
	stmtViewData->BindInt64(4, intVal); //CaptionAndIconId

	std::string insertCaptionAndIconInfo;
		
	insertCaptionAndIconInfo = "INSERT INTO CaptionAndIconInfo(Caption,NumberOfIcons,IconFile) VALUES(?,?,?);";
	std::auto_ptr<CStatement> stmtCaptionAndIconInfo(iScrDbHandler->PrepareStatement(insertCaptionAndIconInfo));

	//Assigning Default Value
	stmtCaptionAndIconInfo->BindInt64(2, intVal); //NumberOfIcons
	std::wstring strVal(L"");
	stmtCaptionAndIconInfo->BindStr(1, strVal); //Caption
	stmtCaptionAndIconInfo->BindStr(3, strVal); //IconFile

	bool captionAndIconInfoPresent = 0;
	//for every TViewData
	stmtViewData->BindInt64(1, alocalAppInfoId);

	for(ViewDataAttributeIterator viewDataIter = aViewDataAttribute.begin(); viewDataIter != aViewDataAttribute.end() ; ++viewDataIter )
		{
		if (viewDataIter->iName == L"Uid")
			{
			TInt64 intValue = Util::WideCharToInt64(viewDataIter->iValue.c_str());
			stmtViewData->BindInt64(2, intValue);
			}
		else if(viewDataIter->iName == L"ScreenMode")
			{
			TInt64 intValue = Util::WideCharToInt64(viewDataIter->iValue.c_str());
			stmtViewData->BindInt64(3, intValue);
			}
		else if(viewDataIter->iName == L"Caption")
			{
			stmtCaptionAndIconInfo->BindStr(1, viewDataIter->iValue);
			captionAndIconInfoPresent = 1;
			}
		else if(viewDataIter->iName == L"NumberOfIcons")
			{
			TInt64 intValue = Util::WideCharToInt64(viewDataIter->iValue.c_str());
			stmtCaptionAndIconInfo->BindInt64(2, intValue);
			captionAndIconInfoPresent = 1;
			}
		else if(viewDataIter->iName == L"IconFile")
			{
			stmtCaptionAndIconInfo->BindStr(3, viewDataIter->iValue);
			captionAndIconInfoPresent = 1;
			}
		else
			{
			std::string errMsg = "Invalid view data attribute.";
			LOGERROR(errMsg);
			throw CException(errMsg,ExceptionCodes::ESqlArgumentError);
			}
		}
	if (captionAndIconInfoPresent)
		{
		stmtCaptionAndIconInfo->ExecuteStatement();
		stmtCaptionAndIconInfo->Reset();
		int captionAndIconIdForViewData = iScrDbHandler->LastInsertedId();
		stmtViewData->BindInt64(4, captionAndIconIdForViewData);
		}
	stmtViewData->ExecuteStatement();
	stmtViewData->Reset();
	LOGEXIT("CDbLayer::AddViewData()");
	}

void CDbLayer::AddProperty( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty>& aApplicationProperty)
	{
	LOGENTER("CDbLayer::AddProperty()");
	std::string insertApplicationProperty;

	for(ApplicationPropertyIterator appPropertyIter = aApplicationProperty.begin(); appPropertyIter != aApplicationProperty.end() ; ++appPropertyIter )
		{  

		insertApplicationProperty = "INSERT INTO AppProperties(AppUid,Locale,Name,ServiceUid,IntValue,StrValue,IsStr8Bit) VALUES(?,?,?,?,?,?,?);";
		std::auto_ptr<CStatement> stmtAppProperty(iScrDbHandler->PrepareStatement(insertApplicationProperty));
		
		stmtAppProperty->BindInt64(1, aAppUid);
		stmtAppProperty->BindInt(2, appPropertyIter->iLocale);
		stmtAppProperty->BindStr(3, appPropertyIter->iName);
		stmtAppProperty->BindInt(4, appPropertyIter->iServiceUid);
		stmtAppProperty->BindInt(5, appPropertyIter->iIntValue);

		if(appPropertyIter->iIsStr8Bit)
			{
			stmtAppProperty->BindBinary(6, appPropertyIter->iStrValue);
			}
		else
			{
			std::wstring sStr = string2wstring(appPropertyIter->iStrValue);
			stmtAppProperty->BindStr(6, sStr);
			}
		
		stmtAppProperty->BindInt(7, appPropertyIter->iIsStr8Bit);

		stmtAppProperty->ExecuteStatement();
		stmtAppProperty->Reset();
		}
	LOGEXIT("CDbLayer::AddProperty()");
	}

void CDbLayer::ExecuteStatement(const std::string& aStmtStr)
	{
	std::auto_ptr<CStatement> stmt(iScrDbHandler->PrepareStatement(aStmtStr));
	stmt->ExecuteStatement();
	}
