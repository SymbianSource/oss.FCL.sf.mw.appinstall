/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponent>::const_iterator CompIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentLocalizable>::const_iterator CompLocIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentProperty>::const_iterator CompPropIterator;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>::const_iterator CompFileIter;
typedef std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty>::const_iterator FilePropIterator;

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
		AddPreProvisionDetails(aPreProvisionDetailList);
		}
		catch(CException& aException)
			{
			std::string rollbackTransaction("ROLLBACK;");
			ExecuteStatement(rollbackTransaction);
			std::string errMsg = "Failed to populate SCR database with environment details.";
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
	std::string insertSoftwareType("INSERT INTO SoftwareTypes(SoftwareTypeId,SifPluginUid,InstallerSecureId,ExecutionLayerSecureId) VALUES(?,?,?,?);");
	std::auto_ptr<CStatement> stmtSwType(iScrDbHandler->PrepareStatement(insertSoftwareType));
	
	std::string insertSwTypeName("INSERT INTO SoftwareTypeNames(SoftwareTypeId,Locale,Name) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtSwTypeName(iScrDbHandler->PrepareStatement(insertSwTypeName));
		
	std::string insertMimeType("INSERT INTO MimeTypes(SoftwareTypeId,MimeType) VALUES(?,?);");
	std::auto_ptr<CStatement> stmtMimeType(iScrDbHandler->PrepareStatement(insertMimeType));

	for(ScrEnvIterator aScrEnvIterator = aScrEnvDetails.begin(); aScrEnvIterator != aScrEnvDetails.end(); ++aScrEnvIterator)
		{
		unsigned int swTypeId = Util::Crc32(aScrEnvIterator->iUniqueSoftwareTypeName.c_str(),aScrEnvIterator->iUniqueSoftwareTypeName.length()*2);
		stmtSwType->BindInt(1, swTypeId);
		stmtSwType->BindInt(2, aScrEnvIterator->iSifPluginUid);
		stmtSwType->BindInt(3, aScrEnvIterator->iInstallerSid);
		stmtSwType->BindInt(4, aScrEnvIterator->iExecutionLayerSid);
		stmtSwType->ExecuteStatement();
		stmtSwType->Reset();
		// First insert unique sw type name
		const TInt uniqueSwTypeNameLocale = 0;
		ExecuteSwTypeNameStatement(stmtSwTypeName, swTypeId, uniqueSwTypeNameLocale, aScrEnvIterator->iUniqueSoftwareTypeName);
		// Then, insert localized sw type names.
		for(ScrEnvLocSwTypeNameIterator swTypeNameIter = aScrEnvIterator->iLocalizedSoftwareTypeNames.begin(); swTypeNameIter != aScrEnvIterator->iLocalizedSoftwareTypeNames.end(); ++swTypeNameIter)
			{
			ExecuteSwTypeNameStatement(stmtSwTypeName, swTypeId, swTypeNameIter->iLocale, swTypeNameIter->iName);
			}
		for(ConstWstringIterator mimeIter= aScrEnvIterator->iMIMEDetails.begin(); mimeIter != aScrEnvIterator->iMIMEDetails.end(); ++mimeIter)
			{
			stmtMimeType->BindInt(1, swTypeId);
			stmtMimeType->BindStr(2, *mimeIter);
			stmtMimeType->ExecuteStatement();
			stmtMimeType->Reset();
			}
		}

	}

void CDbLayer::AddPreProvisionDetails(const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetailList)
	{
	for(CompIterator aCompIterator = aPreProvisionDetailList.iComponents.begin(); aCompIterator != aPreProvisionDetailList.iComponents.end(); ++aCompIterator)
		{	
		std::string beginTransaction("BEGIN;");
		ExecuteStatement(beginTransaction);
		
		AddComponentDetails(*aCompIterator, aPreProvisionDetailList.iSoftwareTypeName);
		int componentId = iScrDbHandler->LastInsertedId();
		AddComponentLocalizables(componentId,aCompIterator->iComponentLocalizables);
		AddComponentProperties(componentId,aCompIterator->iComponentProperties);
		AddComponentFiles(componentId,aCompIterator->iComponentFiles);
		AddComponentDependencies(componentId, aCompIterator->iComponentDependency, aPreProvisionDetailList.iSoftwareTypeName);

		std::string commitTransaction("COMMIT;");
		ExecuteStatement(commitTransaction);

		}
	}

void CDbLayer::AddComponentDetails(const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::wstring& aSoftwareTypeName)
	{
	std::string insertComponents;
	XmlDetails::TScrPreProvisionDetail::TComponentDetails 
		componentDetail = aComponent.iComponentDetails;

	unsigned int swTypeId = Util::Crc32(aSoftwareTypeName.c_str(),aSoftwareTypeName.length()*2);
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
		unsigned int globalIdHash = Util::Crc32(concatGlobalId.c_str(),concatGlobalId.length()*2);
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
	}

void CDbLayer::AddComponentProperties( 
	int aComponentId, 
	const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentProperty>& aComponentProperty)
	{
	
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
				std::string str = Util::wstring2string(compPropIter->iValue);
				std::string decodedString = Util::Base64Decode(str);
				stmtComponentProperty->BindBinary(4, str);
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
	
	}

void CDbLayer::AddComponentFiles(int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>& aComponentFiles)
	{
	for(CompFileIter compFile = aComponentFiles.begin() ; compFile != aComponentFiles.end() ; ++compFile)
		{
		AddLocation(aComponentId,compFile->iLocation);
		int cmpFileId = iScrDbHandler->LastInsertedId();
		AddFileProperties(cmpFileId,compFile->iFileProperties);
		
		}
	}


void CDbLayer::AddComponentDependencies	(	int aComponentId, 
											const XmlDetails::TScrPreProvisionDetail::TComponentDependency& aComponentDependency,
											const std::wstring& aSoftwareTypeName
										)
	{
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
				
		unsigned int globalIdHash = Util::Crc32(concatGlobalId.c_str(),concatGlobalId.length()*2);
		unsigned int dependantIdHash = Util::Crc32(dependantGlobalId.c_str(),dependantGlobalId.length()*2);
		unsigned int supplierIdHash = Util::Crc32(compDepIter->iSupplierId.c_str(),compDepIter->iSupplierId.length()*2);
		
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
	}


void CDbLayer::AddLocation(int aComponentId,  const std::wstring& aLocation)
	{
	std::string insertComponentFileDetails("INSERT INTO ComponentsFiles(ComponentId,LocationHash,Location) VALUES(?,?,?);");
	std::auto_ptr<CStatement> stmtComponentFileDetails(iScrDbHandler->PrepareStatement(insertComponentFileDetails));
	
	stmtComponentFileDetails->BindInt(1,aComponentId);
	
	// size does not return the actual binary size of the object
	int length = aLocation.length()*2 ;
	// generate hash for location
	std::wstring location = aLocation;
	std::transform(	location.begin(), location.end(), location.begin(), tolower);

	unsigned int hash = Util::Crc32(location.c_str(),length);
	
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
			std::string str = Util::wstring2string(filePropIter->iValue);
			std::string decodedString = Util::Base64Decode(str);
			stmtFileProperty->BindBinary(3, str);
			stmtFileProperty->BindInt(4, 1);
			}

		stmtFileProperty->ExecuteStatement();
		stmtFileProperty->Reset();
		}
	}

void CDbLayer::ExecuteStatement(const std::string& aStmtStr)
	{
	std::auto_ptr<CStatement> stmt(iScrDbHandler->PrepareStatement(aStmtStr));
	stmt->ExecuteStatement();
	}
