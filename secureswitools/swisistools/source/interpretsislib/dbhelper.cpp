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


#include "dbhelper.h"
#include "dbprocessor.h"
#include "dbconstants.h"
#include "is_utils.h"
#include "sisregistryobject.h"
#include "sisregistrytoken.h"
#include "sisregistrypackage.h"
#include "errors.h"
#include "../common/exception.h"

#include <sstream>
#include <stdexcept>

const int DbHelper::Absent = -1;

DbHelper::~DbHelper()
	{
	delete iScrDbHandler;
	}

DbHelper::DbHelper( const std::wstring& aDrivePath, bool aIsSystemDrive)
	{

  #ifdef __LINUX__
  std::string dllName = "sqlite-3.6.1.so"; 	  	  	 
  #else 	  	  	 
  std::string dllName = "sqlite3.dll"; 
  #endif		
	std::string dbName; 
	
	if (aIsSystemDrive)
	#ifdef __TOOLS2_LINUX__
		dbName = wstring2string(aDrivePath) + "/sys/install/scr/scr.db";
	#else
		dbName = wstring2string(aDrivePath) + "\\sys\\install\\scr\\scr.db";
	#endif
	else 
	#ifdef __TOOLS2_LINUX__
		dbName = wstring2string(aDrivePath) + "/sys/install/scr/provisioned/scr.db";
	#else
		dbName = wstring2string(aDrivePath) + "\\sys\\install\\scr\\provisioned\\scr.db";
	#endif

	std::wstring wdbname(string2wstring(dbName));
	//Create the SCR DB if not present in the System Drive
	if(!FileExists(wdbname))
		{
		const char* epocRoot = getenv("EPOCROOT");
		if(NULL == epocRoot)
			{
				std::string emsg(" EPOCROOT environment variable not specified.");
				int retCode = ExceptionCodes::EEnvNotSpecified;
				throw InterpretSisError(emsg,retCode);
			}
		std::string epocRootStr(epocRoot); 

		#ifndef __TOOLS2_LINUX__
			std::wstring swEnvInfo = string2wstring(epocRootStr) + L"epoc32\\tools\\create_db.xml";
		#else
			std::wstring swEnvInfo = string2wstring(epocRootStr) + L"epoc32/tools/create_db.xml";
		#endif

		if(FileExists(swEnvInfo))
			{
			#ifdef __TOOLS2_LINUX__
				std::string executable = "scrtool";
			#else
				std::string executable = "scrtool.exe";
			#endif
			std::string command;
			
			command = executable + " -c " + dbName + " -f " + wstring2string(swEnvInfo);
			
			std::cout << "Creating DB : " << command << std::endl;
			
			int error = system(command.c_str());
			if(error != 0)
				{
					std::string err = "Scrtool failed to create the database.";
					throw InterpretSisError(err, DATABASE_UPDATE_FAILED);
				}
			}
		else
			{
				LERROR(L"Failed to create the database.");
				std::string emessage(" XML file /epoc32/tools/create_db.xml which contains software environment information is not found.");
				int returnCode = ExceptionCodes::EFileNotPresent;
				throw InterpretSisError(emessage,returnCode);
			}
		}

	try
		{
		iScrDbHandler = new CDbProcessor(dllName, dbName);		
		}
	catch(CException& aException)
		{
		int returnCode = aException.GetCode();
		std::wstring emessage = string2wstring( aException.GetMessageA() );

		returnCode = (returnCode == ExceptionCodes::ELibraryLoadError) ? LIBRARYLOAD_ERROR : returnCode;
		throw InterpretSisError(emessage,returnCode);		
		}
	}

TBool DbHelper::IsInstalled(TInt32 aUid) const
	{
	std::string selectUid("SELECT IntValue FROM ComponentProperties WHERE Name='CompUid' and IntValue=?;");
	std::auto_ptr<CStatement> stmtUid(iScrDbHandler->PrepareStatement(selectUid));
		
	stmtUid->BindInt(1, aUid);
	
	if(!stmtUid->ProcessNextRow())
		return false;
		
	TInt32 uid = stmtUid->IntColumn(0);
	return (uid == aUid);
	}

TBool DbHelper::IsAppUidInstalled(TUint32 aAppUid) const
	{
	std::string selectUid("SELECT AppFile FROM AppRegistrationInfo WHERE AppUid=?;");
	std::auto_ptr<CStatement> stmtUid(iScrDbHandler->PrepareStatement(selectUid));
		
	stmtUid->BindInt(1, aAppUid);
	
	if(!stmtUid->ProcessNextRow())
		return false;
		
	return true;
	}

TInt32 DbHelper::GetComponentId(TInt32 aUid, const std::wstring aPackageName, const std::wstring aVendorName ) const
	{
	std::string selectUid("SELECT ComponentId from ComponentProperties where Name='CompUid' and IntValue=? and ComponentId IN (SELECT ComponentId FROM \
	ComponentProperties WHERE Name='CompUniqueVendorName' and StrValue=? and ComponentId IN (SELECT ComponentId FROM ComponentLocalizables WHERE Name=?));");
	std::auto_ptr<CStatement> stmtCompid(iScrDbHandler->PrepareStatement(selectUid));
		
	stmtCompid->BindInt(1, aUid);
	stmtCompid->BindStr(2, aVendorName);
	stmtCompid->BindStr(3, aPackageName);

	TInt32 componentId = DbHelper::Absent;
	if(stmtCompid->ProcessNextRow())
		{
		componentId = stmtCompid->IntColumn(0);
		}
	return componentId;	
	}

TInt32 DbHelper::GetComponentId(TInt32 aUid, TInt32 aInstallType ) const
	{
	std::string selectUid("SELECT ComponentId from ComponentProperties where Name='CompUid' and IntValue=? and ComponentId IN (SELECT ComponentId FROM \
	ComponentProperties WHERE Name='CompInstallType' and IntValue=?) ");
	std::auto_ptr<CStatement> stmtCompid(iScrDbHandler->PrepareStatement(selectUid));

	stmtCompid->BindInt(1, aUid);
	stmtCompid->BindInt(2, aInstallType);
	
	TInt32 componentId = DbHelper::Absent;
	if(	stmtCompid->ProcessNextRow())
		{
		componentId = stmtCompid->IntColumn(0);
		}
	
	return componentId;	
	}

TInt32 DbHelper::GetComponentId( TInt32 aUid ) const
	{
	std::string selectCompId(	"SELECT ComponentId FROM ComponentProperties WHERE Name='CompUid' \
								AND IntValue=? AND ComponentId IN (SELECT ComponentId from \
								ComponentProperties WHERE Name='CompIndex' AND IntValue='0');"	
							);

	std::auto_ptr<CStatement> stmtCompId(iScrDbHandler->PrepareStatement(selectCompId));
		
	stmtCompId->BindInt(1, aUid);
	
	// this might contain multiple entries for the specific uid but as it has been
	// ordered by propertyid the first entry would be of the first installation.
	if(stmtCompId->ProcessNextRow())
		{
		return stmtCompId->IntColumn(0);
		}

	return DbHelper::Absent;
	}

std::vector<TInt32> DbHelper::GetComponentIdList( TInt32 aUid ) const
	{
	std::vector<TInt32> componentIdList;
	std::string selectCompId("SELECT ComponentId FROM ComponentProperties WHERE Name='CompUid' and IntValue=? order by PropertyId;");
	std::auto_ptr<CStatement> stmtCompId(iScrDbHandler->PrepareStatement(selectCompId));
		
	stmtCompId->BindInt(1, aUid);
	
	// this might contain multiple entries for the specific uid but as it has been
	// ordered by propertyid the first entry would be of the first installtion.
	while(stmtCompId->ProcessNextRow())
		{
		componentIdList.push_back(stmtCompId->IntColumn(0));
		}

	return componentIdList;
	}

bool DbHelper::GetIsRemovable( TInt32 aComponentId ) const
	{
	std::string selectIsRemovable("SELECT Removable FROM Components WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtIsRemovable(iScrDbHandler->PrepareStatement(selectIsRemovable));
		
	stmtIsRemovable->BindInt(1, aComponentId);
	
	if(!stmtIsRemovable->ProcessNextRow())
		return false;

	return (stmtIsRemovable->IntColumn(0) == 1);
	}

bool DbHelper::GetInRom( TInt32 aComponentId ) const
	{
	std::string selectInRom("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name='CompInRom';");
	std::auto_ptr<CStatement> stmtInRom(iScrDbHandler->PrepareStatement(selectInRom));
		
	stmtInRom->BindInt(1, aComponentId);
	
	if(!stmtInRom->ProcessNextRow())
		return DbConstants::KDefaultIsInRom;

	return (stmtInRom->IntColumn(0) == 1);
	}

std::wstring DbHelper::GetStubFileName( TInt32 aComponentId ) const
	{
	std::string selectStubFileName("SELECT StrValue FROM ComponentProperties WHERE ComponentId=? and Name=?;");
	std::auto_ptr<CStatement> stmtStubFileName(iScrDbHandler->PrepareStatement(selectStubFileName));
		
	stmtStubFileName->BindInt(1, aComponentId);
	stmtStubFileName->BindStr(2, DbConstants::CompStubFileName);
	
	if(stmtStubFileName->ProcessNextRow())
		{
		return stmtStubFileName->StrColumn(0);
		}

	return L"\0";
	}

std::wstring DbHelper::GetPackageName( TInt32 aComponentId ) const
	{
	std::string selectPckgName("SELECT Name FROM ComponentLocalizables WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtPckgName(iScrDbHandler->PrepareStatement(selectPckgName));
		
	stmtPckgName->BindInt(1, aComponentId);
	
	if(stmtPckgName->ProcessNextRow())
		{
		return stmtPckgName->StrColumn(0);
		}

	return NULL;
	}

TUint32 DbHelper::GetUid( TInt32 aComponentId ) const
	{
	std::string selectUid("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name='CompUid';");
	std::auto_ptr<CStatement> stmtUid(iScrDbHandler->PrepareStatement(selectUid));
		
	stmtUid->BindInt(1, aComponentId);
	
	if(stmtUid->ProcessNextRow())
		{
		return stmtUid->IntColumn(0);
		}

	return DbHelper::Absent;
	}


TUint32 DbHelper::GetUidFromFileName( const std::wstring& aFileName ) const
	{
	std::string selCompId("SELECT ComponentId FROM ComponentsFiles WHERE Location=?;");
	std::auto_ptr<CStatement> stmtCompId(iScrDbHandler->PrepareStatement(selCompId));
		
	stmtCompId->BindStr(1, aFileName.c_str());
	
	TInt32 componentId = 0;
	if(stmtCompId->ProcessNextRow())
		{
		componentId = stmtCompId->IntColumn(0);
		return GetUid(componentId);
		}

	std::string selCompPropsId("SELECT ComponentId FROM ComponentProperties WHERE StrValue=? and Name like 'WCFile%';");
	std::auto_ptr<CStatement> stmtCompPropsId(iScrDbHandler->PrepareStatement(selCompPropsId));
		
	stmtCompPropsId->BindStr(1, aFileName.c_str());
	
	if(stmtCompPropsId->ProcessNextRow())
		{
		componentId = stmtCompPropsId->IntColumn(0);
		return GetUid(componentId);
		}

	return 0;
	}


TUint32 DbHelper::GetInstallType( TInt32 aComponentId ) const
	{
	std::string selectInstallType("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name='CompInstallType';");
	std::auto_ptr<CStatement> stmtInstallType(iScrDbHandler->PrepareStatement(selectInstallType));
		
	stmtInstallType->BindInt(1, aComponentId);
	
	if(stmtInstallType->ProcessNextRow())
		return stmtInstallType->IntColumn(0);

	return DbConstants::KDefaultInstallType;
	}

TInt DbHelper::GetIndex( TInt32 aComponentId) const
	{
	return GetComponentPropertyIntValue(aComponentId,DbConstants::CompIndex);
	}

void DbHelper::RemoveEntry( TInt32 aComponentId ) const
	{

	std::vector<TInt32> appUidList = GetAppRegistrationInfoAppUid(aComponentId);
	std::vector<TInt32> ::iterator appUidIter;
	for( appUidIter = appUidList.begin(); appUidIter != appUidList.end() ; ++appUidIter )
	{
		// delete FileOwnershipInfo
		std::string delFileOwnerInfo("DELETE FROM FileOwnershipInfo WHERE AppUid=?;");
		std::auto_ptr<CStatement> stmtdelFileOwnerInfo(iScrDbHandler->PrepareStatement(delFileOwnerInfo));
		stmtdelFileOwnerInfo->BindInt(1, *appUidIter);
		stmtdelFileOwnerInfo->ExecuteStatement();

		// delete AppProperties
		std::string delAppProperties("DELETE FROM AppProperties WHERE AppUid=?;");
		std::auto_ptr<CStatement> stmtAppProperties(iScrDbHandler->PrepareStatement(delAppProperties));
		stmtAppProperties->BindInt(1, *appUidIter);
		stmtAppProperties->ExecuteStatement();

		std::vector<TInt32> serviceIdList = GetServiceInfoServiceID(*appUidIter);
		
		std::vector<TInt32> ::iterator serviceIdIter;
		for( serviceIdIter = serviceIdList.begin(); serviceIdIter != serviceIdList.end() ; ++serviceIdIter )
		{
			// delete DataType
			std::string delDataType("DELETE FROM DataType WHERE ServiceId=?;");
			std::auto_ptr<CStatement> stmtDataType(iScrDbHandler->PrepareStatement(delDataType));
			stmtDataType->BindInt(1, *serviceIdIter);
			stmtDataType->ExecuteStatement();
		}

		// delete ServiceInfo
		std::string delServiceInfo("DELETE FROM ServiceInfo WHERE AppUid=?;");
		std::auto_ptr<CStatement> stmtServiceInfo(iScrDbHandler->PrepareStatement(delServiceInfo));
		stmtServiceInfo->BindInt(1, *appUidIter);
		stmtServiceInfo->ExecuteStatement();

		std::vector<TInt32> localAppInfoIdList = GetLocalAppInfoID(*appUidIter);
		
		std::vector<TInt32> ::iterator localAppInfoIdIter;
		for( localAppInfoIdIter = localAppInfoIdList.begin(); localAppInfoIdIter != localAppInfoIdList.end() ; ++localAppInfoIdIter )
		{
			std::vector<TInt32> viewDataCaptionIdList = GetViewDataCaptionAndIconID(*localAppInfoIdIter);
			
			std::vector<TInt32> ::iterator viewDataCaptionIdIter;
			for( viewDataCaptionIdIter = viewDataCaptionIdList.begin(); viewDataCaptionIdIter != viewDataCaptionIdList.end() ; ++viewDataCaptionIdIter )
			{
				// delete CaptionAndIconInfo
				std::string delCaptionInfo("DELETE FROM CaptionAndIconInfo WHERE CaptionAndIconId=?;");
				std::auto_ptr<CStatement> stmtCaptionInfo(iScrDbHandler->PrepareStatement(delCaptionInfo));
				stmtCaptionInfo->BindInt(1, *viewDataCaptionIdIter);
				stmtCaptionInfo->ExecuteStatement();
			}

			// delete ViewData
			std::string delViewData("DELETE FROM ViewData WHERE LocalAppInfoId=?;");
			std::auto_ptr<CStatement> stmtViewData(iScrDbHandler->PrepareStatement(delViewData));
			stmtViewData->BindInt(1, *localAppInfoIdIter);
			stmtViewData->ExecuteStatement();
		}

		std::vector<TInt32> LocalCaptionIdList = GetLocalCaptionAndIconID(*appUidIter);
		std::vector<TInt32> ::iterator LocalCaptionIdIter;
		for( LocalCaptionIdIter = LocalCaptionIdList.begin(); LocalCaptionIdIter != LocalCaptionIdList.end() ; ++LocalCaptionIdIter )
		{
			// delete CaptionAndIconInfo
			std::string delCaptionInfo("DELETE FROM CaptionAndIconInfo WHERE CaptionAndIconId=?;");
			std::auto_ptr<CStatement> stmtCaptionInfo(iScrDbHandler->PrepareStatement(delCaptionInfo));
			stmtCaptionInfo->BindInt(1, *LocalCaptionIdIter);
			stmtCaptionInfo->ExecuteStatement();
		}

		// delete LocalizableAppInfo
		std::string delLocalizableAppInfo("DELETE FROM LocalizableAppInfo WHERE AppUid=?;");
		std::auto_ptr<CStatement> stmtLocalizableAppInfo(iScrDbHandler->PrepareStatement(delLocalizableAppInfo));
		stmtLocalizableAppInfo->BindInt(1, *appUidIter);
		stmtLocalizableAppInfo->ExecuteStatement();

	}

	// delete AppRegistrationInfo
	std::string delAppRegistrationInfo("DELETE FROM AppRegistrationInfo WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtAppRegistrationInfo(iScrDbHandler->PrepareStatement(delAppRegistrationInfo));
	stmtAppRegistrationInfo->BindInt(1, aComponentId);
	stmtAppRegistrationInfo->ExecuteStatement();

	
	// delete component properties
	std::string delCompProps("DELETE FROM ComponentProperties WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtDelCompProps(iScrDbHandler->PrepareStatement(delCompProps));
	stmtDelCompProps->BindInt(1, aComponentId);
	stmtDelCompProps->ExecuteStatement();

	// delete component file properties
	std::string delFileProps("DELETE from FileProperties WHERE CmpFileId IN \
				(SELECT CmpFileId FROM ComponentsFiles WHERE ComponentId=?);");
	std::auto_ptr<CStatement> stmtDelFileProps(iScrDbHandler->PrepareStatement(delFileProps));
	stmtDelFileProps->BindInt(1, aComponentId);
	stmtDelFileProps->ExecuteStatement();

	// delete component files
	std::string delCompFiles("DELETE FROM ComponentsFiles WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtCompFiles(iScrDbHandler->PrepareStatement(delCompFiles));
	stmtCompFiles->BindInt(1, aComponentId);
	stmtCompFiles->ExecuteStatement();

	// delete component localizables
	std::string delCompLocs("DELETE FROM ComponentLocalizables WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtCompLocs(iScrDbHandler->PrepareStatement(delCompLocs));
	stmtCompLocs->BindInt(1, aComponentId);
	stmtCompLocs->ExecuteStatement();

	// delete component dependencies
	std::string delCompDeps("DELETE FROM ComponentDependencies WHERE GlobalIdHash IN \
				(SELECT GlobalIdHash FROM Components WHERE ComponentId=?);");
	std::auto_ptr<CStatement> stmtCompDeps(iScrDbHandler->PrepareStatement(delCompDeps));
	stmtCompDeps->BindInt(1, aComponentId);
	stmtCompDeps->ExecuteStatement();

	// delete components
	std::string delComps("DELETE FROM Components WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtComps(iScrDbHandler->PrepareStatement(delComps));
	stmtComps->BindInt(1, aComponentId);
	stmtComps->ExecuteStatement();

	}

int DbHelper::GetSelectedDrive( TInt32 aComponentId )
	{
	std::string selectDrive("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name='CompSelectedDrive';");
	std::auto_ptr<CStatement> stmtDrive(iScrDbHandler->PrepareStatement(selectDrive));
		
	stmtDrive->BindInt(1, aComponentId);
	
	if(stmtDrive->ProcessNextRow())
		return stmtDrive->IntColumn(0);
	
	return DbConstants::KDefaultInstalledDrive;
	}

bool DbHelper::InternalizeIfExists(SisRegistryObject& aSisRegistryObject, const TUint32 aUid ) const
	{
	TInt32 componentId = GetComponentId(aUid);
	if(componentId == DbHelper::Absent)
		{
		return false;
		}

	// retrieve all details from componentproperties
	GetComponentDetails(componentId, aSisRegistryObject);

	GetComponentLocalizables(componentId, aSisRegistryObject);

	// retrieve all files for this component Id from the componentfiles table
	GetFileDescriptions(componentId, aSisRegistryObject);

	GetComponentProperties(componentId, aSisRegistryObject);

	return true;
	}

bool DbHelper::SidExists(const TUint32 aSid) const
	{
	std::string selectSid("SELECT IntValue FROM ComponentProperties WHERE Name like 'CompSid%' and IntValue=?;");
	std::auto_ptr<CStatement> stmtSid(iScrDbHandler->PrepareStatement(selectSid));
		
	stmtSid->BindInt(1, aSid);
	if(!stmtSid->ProcessNextRow())
		return false;

	TInt32 sid = stmtSid->IntColumn(0);
	
	return ( sid == aSid );
	}

TUint32 DbHelper::GetUid(const TUint32 aSid) const
	{
	std::string selectUid("SELECT IntValue FROM ComponentProperties WHERE ComponentId in \
						(SELECT ComponentId from ComponentProperties where Name like 'CompSid%' and IntValue=?) and Name='CompUid';");
	std::auto_ptr<CStatement> stmtUid(iScrDbHandler->PrepareStatement(selectUid));
		
	stmtUid->BindInt(1, aSid);
	if(stmtUid->ProcessNextRow())
		return stmtUid->IntColumn(0);

	return DbHelper::Absent;
	}

 
TUint32 DbHelper::GetUid( const std::wstring& aSidFile ) const
	{
	std::string selectCompId("SELECT ComponentId FROM ComponentProperties WHERE StrValue=? and Name like 'CompSidFileName%';");
	std::auto_ptr<CStatement> stmtCompId(iScrDbHandler->PrepareStatement(selectCompId));
		
	stmtCompId->BindStr(1, aSidFile);
	if(!stmtCompId->ProcessNextRow())
		return DbHelper::Absent;

	TInt32 componentId = stmtCompId->IntColumn(0);
	
	return GetUid(componentId);
	
	}

std::wstring DbHelper::SidToFileName(TUint32 aSid) const
	{	
	std::string selectCompId("SELECT ComponentId,Name FROM ComponentProperties WHERE Name like 'CompSid%' and IntValue=?;");
	std::auto_ptr<CStatement> stmtCompId(iScrDbHandler->PrepareStatement(selectCompId));
		
	stmtCompId->BindInt(1, aSid);
	if(!stmtCompId->ProcessNextRow())
		return NULL;

	int componentId = stmtCompId->IntColumn(0);

	std::wstring sidName = stmtCompId->StrColumn(1);

	std::wstring finalString(DbConstants::CompSidFileName);
	finalString += sidName.substr(DbConstants::CompSid.length() );

	std::string selectFile("SELECT StrValue FROM ComponentProperties WHERE ComponentId=? and Name=?;");
	
	std::auto_ptr<CStatement> stmtFile(iScrDbHandler->PrepareStatement(selectFile));
		
	stmtFile->BindInt(1, componentId);
	stmtFile->BindStr(2, finalString);

	if(!stmtFile->ProcessNextRow())
		return NULL;
	
	return stmtFile->StrColumn(0);
	

	}

void DbHelper::GetComponentDetails(TInt32 aComponentId, SisRegistryObject& aSisRegistryObject) const
	{
	
	std::string selectComponents(	"SELECT Removable, Hidden, Version FROM Components WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtSelectComponents(iScrDbHandler->PrepareStatement(selectComponents));

	stmtSelectComponents->BindInt(1, aComponentId);
	if(!stmtSelectComponents->ProcessNextRow())
		return;

	// isRemovable
	TBool isRemovable = false;
	if(stmtSelectComponents->IntColumn(0) == 1)
		{
		isRemovable = true;
		}
	aSisRegistryObject.SetIsRemovable(isRemovable);
	
	// isHidden
	TBool isHidden = false;
	if(stmtSelectComponents->IntColumn(1) == 1)
		{
		isHidden = true;
		}
	aSisRegistryObject.SetIsHidden(isHidden);
	
	std::wstring textVersion = stmtSelectComponents->StrColumn(2);
	
	std::wstringstream strMajor(textVersion.substr(0,1));
	int major = 0;
	strMajor >> major;
	std::wstringstream strMinor(textVersion.substr(2,3));
	int minor = 0;
	strMinor >> minor;
	std::wstringstream strBuild(textVersion.substr(4,4));
	int build = 0;
	strBuild >> build;

	Version version(major,minor,build);

	// sisregistrytoken
	aSisRegistryObject.SetVersion(version);
	}

void DbHelper::GetComponentLocalizables(TInt32 aComponentId, SisRegistryObject& aSisRegistryObject) const
	{
	// do we need to specify locale
	std::string selectCompLocalizables( "SELECT Name,Vendor FROM ComponentLocalizables WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtCompLocalizables(iScrDbHandler->PrepareStatement(selectCompLocalizables));

	stmtCompLocalizables->BindInt(1, aComponentId);
	if(!stmtCompLocalizables->ProcessNextRow())
		return;

	std::wstring pckgName = stmtCompLocalizables->StrColumn(0);
	std::wstring vendor = stmtCompLocalizables->StrColumn(1);

	// sisregistrypackage
	aSisRegistryObject.SetPackageName(pckgName);
	// sisregistryobject
	aSisRegistryObject.SetVendorLocalizedName(vendor);

	}

void DbHelper::GetFileDescriptions( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const
	{
	std::vector<FileDescription*> fileDescriptions;
	GetFileDescriptionFromComponentProperties( aComponentId, fileDescriptions );
	GetFileDescriptionFromFileProperties( aComponentId, fileDescriptions );

	aSisRegistryObject.SetFileDescriptions(fileDescriptions);

	}

std::vector<FileDescription*> DbHelper::GetFileDescriptions( TInt32 aComponentId ) const
	{
	std::vector<FileDescription*> fileDescriptions;
	GetFileDescriptionFromComponentProperties( aComponentId, fileDescriptions );
	GetFileDescriptionFromFileProperties( aComponentId, fileDescriptions );
	return fileDescriptions;
	}

void DbHelper::GetFileDescriptionFromComponentProperties( int aComponentId, std::vector<FileDescription*>& aFileDescriptions) const
	{
	std::string selectWildCardCount( "SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name='CompWildCardFileCount';");
	std::auto_ptr<CStatement> stmtWildCardCount(iScrDbHandler->PrepareStatement(selectWildCardCount));
	
	stmtWildCardCount->BindInt(1, aComponentId);
	if(!stmtWildCardCount->ProcessNextRow())
		return;

	int wildCardCount = stmtWildCardCount->IntColumn(0);

	for(int count=0; count < wildCardCount; ++count)
		{
		
		FileDescription* fileDescription = new FileDescription();

		std::string selectWildCardCount( "SELECT Name,Locale,IntValue,StrValue,IsStr8Bit FROM ComponentProperties WHERE ComponentId=? and Name like ? order by PropertyId;");
		std::auto_ptr<CStatement> stmtWildCardCount(iScrDbHandler->PrepareStatement(selectWildCardCount));
		
		std::wstring namePattern = L"WCFile%";
		std::wstringstream strNamePattern;
		strNamePattern << namePattern << count;
				
		stmtWildCardCount->BindInt(1, aComponentId);
		stmtWildCardCount->BindStr(2, strNamePattern.str());
		
		AddFileDescription(stmtWildCardCount.get(), *fileDescription, count);
		
		aFileDescriptions.push_back(fileDescription);
		}
	}

void DbHelper::GetFileDescriptionFromFileProperties( int aComponentId, std::vector<FileDescription*>& aFileDescriptions) const
	{
	// retrieve the location form the component files table
	std::string selLocation( "SELECT CmpFileId,Location FROM ComponentsFiles WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtLocation(iScrDbHandler->PrepareStatement(selLocation));

	stmtLocation->BindInt(1, aComponentId);

	while( stmtLocation->ProcessNextRow() )
		{
		FileDescription* fileDescription = new FileDescription();
		fileDescription->SetTarget(stmtLocation->StrColumn(1));

		int cmpFileId = stmtLocation->IntColumn(0);

		std::string selFileProps( "SELECT Name,IntValue,StrValue FROM FileProperties WHERE CmpFileId=?;");
		std::auto_ptr<CStatement> stmtFileProps(iScrDbHandler->PrepareStatement(selFileProps));
		stmtFileProps->BindInt(1, cmpFileId);
		
		AddFileDescription(stmtFileProps.get(), *fileDescription, -1);
		aFileDescriptions.push_back(fileDescription);			
		}
	}

void DbHelper::AddFileDescription(CStatement* aStatement, FileDescription& aFileDescription, TInt aCount ) const
	{
	bool hashId = false;
	bool hashData = false;
	HashContainer hashContainer;

	if( aCount == -1 )
		{
		while(aStatement->ProcessNextRow())
			{
			std::wstring name = aStatement->StrColumn(0);

			if(name == DbConstants::FileMimeType )
				{
				aFileDescription.SetMimeType(aStatement->StrColumn(2));	
				}
			else if(name == DbConstants::FileSisFileOperation )
				{
				int sisFileOperation = aStatement->IntColumn(1);
				aFileDescription.SetOperation(sisFileOperation);
				}
			else if( name == DbConstants::FileFileOperationOptions)
				{
				int fileOperationOptions = aStatement->IntColumn(1);
				aFileDescription.SetOperationOptions(fileOperationOptions);
				}
			else if( name == DbConstants::FileUncompressedLength )
				{
				TInt64 uncompressedLength = aStatement->Int64Column(1);
				aFileDescription.SetUncompressedLength( uncompressedLength );
				}
			else if( name == DbConstants::FileIndex )
				{
				int fileIndex = aStatement->IntColumn(1);
				aFileDescription.SetIndex( fileIndex );
				}
			else if( name == DbConstants::FileSid )
				{
				int fileSid = aStatement->IntColumn(1);
				aFileDescription.SetSid( fileSid );
				}
			else if( name == DbConstants::FileHashAlgorithm )
				{
				hashContainer.SetHashId(aStatement->IntColumn(1));
				hashId = true;
				if(hashData)
					{
					aFileDescription.SetHash(hashContainer);
					hashId = hashData = false;
					}
				}
			else if( name == DbConstants::FileHashData )
				{
				hashContainer.SetData(wstring2string(aStatement->StrColumn(2)));
				hashData = true;
				if(hashId)
					{
					aFileDescription.SetHash(hashContainer);
					hashId = hashData = false;
					}
				}
			} // while
		}
	else
		{
		while(aStatement->ProcessNextRow() )
			{
			std::wstring name = aStatement->StrColumn(0);

			if(name == FormatDbString(DbConstants::WCFileName,aCount) )
				{
				aFileDescription.SetTarget(aStatement->StrColumn(3));	
				}
			} // while
		}
	}


void DbHelper::GetComponentProperties( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const
	{
	TUint32 installType = GetComponentPropertyIntValue(aComponentId, DbConstants::CompInstallType, DbConstants::KDefaultInstallType);
	aSisRegistryObject.SetInstallType(installType);
	aSisRegistryObject.SetDependencies(GetDependencies(aComponentId));
	aSisRegistryObject.SetEmbeddedPackages(GetEmbeddedPackages(aComponentId));
	aSisRegistryObject.SetProperties(GetProperties(aComponentId));
	
	// inrom
	bool status = GetComponentPropertyIntValue(aComponentId,DbConstants::CompInRom, DbConstants::KDefaultIsInRom);
	aSisRegistryObject.SetInRom(status);

	// issigned
	status = GetComponentPropertyIntValue(aComponentId,DbConstants::CompIsSigned, DbConstants::KDefaultIsSigned);
	aSisRegistryObject.SetSigned(status);

	// signedbysucert
	status = GetComponentPropertyIntValue(aComponentId,DbConstants::CompSignedBySuCert, DbConstants::KDefaultIsSignedBySuCert);
	aSisRegistryObject.SetSigned(status);

	// deletepreinstalled
	status = GetComponentPropertyIntValue(aComponentId,DbConstants::CompDeletablePreinstalled, DbConstants::KDefaultIsDeletablePreInstalled);
	aSisRegistryObject.SetPreInstalled(status);

	// trust
	int trust = GetComponentPropertyIntValue(aComponentId,DbConstants::CompTrust, DbConstants::KDefaultTrustValue);
	aSisRegistryObject.SetTrust(trust);
	
	// isRemoveWithLastDependent
	int removeWithLastDep = GetComponentPropertyIntValue(aComponentId,DbConstants::CompIsRemoveWithLastDependent, DbConstants::KDefaultRemoveWithLastDependent);
	aSisRegistryObject.SetRemoveWithLastDependent(removeWithLastDep);

	// trustTimeStamp
	TInt64 timeStamp = 0;
	aSisRegistryObject.SetTrustTimeStamp(timeStamp);

	// trustStatus
	aSisRegistryObject.SetTrustStatus(GetTrustStatus(aComponentId));

	// installchain indices
	aSisRegistryObject.SetInstallChainIndices(GetInstallChainIndices(aComponentId));

	//////// sisregistrytoken ///////////////////////

	aSisRegistryObject.SetSids(GetSids(aComponentId));

	aSisRegistryObject.SetControllerInfo(GetControllerInfo(aComponentId));

	// language
	TInt language = GetComponentPropertyIntValue( aComponentId, DbConstants::CompLanguage, DbConstants::KDefaultInstalledLanguage );
	aSisRegistryObject.SetLanguage(language);

	//selected drive
	TInt selectedDrive = GetComponentPropertyIntValue( aComponentId, DbConstants::CompSelectedDrive, DbConstants::KDefaultInstalledDrive );
	aSisRegistryObject.SetSelectedDrive(selectedDrive);

	//////// sisregistrypackage ///////////////////////

	// uid
	TUint32 uid = GetComponentPropertyIntValue( aComponentId, DbConstants::CompUid );
	aSisRegistryObject.SetUid(uid);
	
	// unique vendor name
	std::wstring uniqueVendorName = GetComponentPropertyStrValue( aComponentId, DbConstants::CompUniqueVendorName );
	aSisRegistryObject.SetVendorName(uniqueVendorName);

	// index
	TInt index = GetComponentPropertyIntValue( aComponentId, DbConstants::CompIndex );
	aSisRegistryObject.SetIndex(index);

	}

Dependencies DbHelper::GetDependencies( TInt32 aComponentId ) const
	{
	std::vector<SisRegistryDependency*> dependencyList;
	
	// retrieve the location form the component files table
	std::string selDependencies( "SELECT SupplierGlobalId,VersionFrom,VersionTo FROM ComponentDependencies WHERE GlobalIdHash IN \
							(SELECT GlobalIdHash from Components where ComponentId=? );");
	std::auto_ptr<CStatement> stmtDependencies(iScrDbHandler->PrepareStatement(selDependencies));

	stmtDependencies->BindInt(1, aComponentId);

	while( stmtDependencies->ProcessNextRow() )
		{
		std::wstring supplierId = stmtDependencies->StrColumn(0);
		TUint32 uid = Utils::WideStringToInteger(supplierId.substr(4,supplierId.length()));

		SisRegistryDependency* registryDependency = new SisRegistryDependency();
		registryDependency->SetUid(uid);
		registryDependency->SetFromVersion(FormatStringToVersion(stmtDependencies->StrColumn(1)));
		registryDependency->SetToVersion(FormatStringToVersion(stmtDependencies->StrColumn(2)));
		dependencyList.push_back(registryDependency);
		}
	
	return dependencyList;
	}

Packages DbHelper::GetEmbeddedPackages( TInt32 aComponentId ) const
	{
	std::vector<SisRegistryPackage*> packageList;
	
	int embPckgCount = GetComponentPropertyIntValue(aComponentId,DbConstants::CompEmbeddedPackageCount, DbConstants::KDefaultEmbeddedPackageCount);

	for(int count=0 ; count<embPckgCount ; ++count)
		{
		
		TUint32 uid = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompEmbeddedPackageUid, count);
		std::wstring name = GetComponentPropertyStrValue(aComponentId, DbConstants::CompEmbeddedPackageName, count);
		std::wstring vendor = GetComponentPropertyStrValue(aComponentId, DbConstants::CompEmbeddedPackageVendor, count);
		TUint32 pckgIndex = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompEmbeddedPackageIndex, count);
		SisRegistryPackage* registryPckg = new SisRegistryPackage(uid, name, vendor, pckgIndex);
		packageList.push_back(registryPckg);
		}
	
	return packageList;
	}

Properties DbHelper::GetProperties( TInt32 aComponentId ) const
	{
	std::vector<SisRegistryProperty*> propList;
	
	int propCount = GetComponentPropertyIntValue(aComponentId,DbConstants::CompPropertyCount, DbConstants::KDefaultCompPropertyCount);

	for(int count=0 ; count<propCount ; ++count)
		{
		TUint32 key = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompPropertyKey, count);
		TUint32 value = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompPropertyValue, count);
		SisRegistryProperty* regProp = new SisRegistryProperty();
		regProp->SetKey(key);
		regProp->SetValue(value);
		propList.push_back(regProp);
		}
	
	return propList;
	}

TrustStatus DbHelper::GetTrustStatus( TInt32 aComponentId ) const
	{
	TrustStatus trustStatus;
	
	TUint32 validationStatus = GetComponentPropertyIntValue(aComponentId, DbConstants::CompTrustValidationStatus);
	
	trustStatus.SetValidationStatus(validationStatus);

	return trustStatus;
	}

std::vector<TInt> DbHelper::GetInstallChainIndices( TInt32 aComponentId ) const
	{
	std::vector<TInt> installChainIndices;
	
	int installChainIndexCount = GetComponentPropertyIntValue(aComponentId,DbConstants::CompInstallChainIndexCount, 
			DbConstants::KDefaultChainIndex);

	for(int count=0 ; count<installChainIndexCount ; ++count)
		{
		TInt chainValue = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompInstallChainIndex, count);
		installChainIndices.push_back(chainValue);
		}
	
	return installChainIndices;
	}

Sids DbHelper::GetSids( TInt32 aComponentId ) const
	{
	std::vector<TUint32> sidList;
	
	int sidCount = GetComponentPropertyIntValue(aComponentId,DbConstants::CompSidCount, DbConstants::KDefaultSidCount);

	for(int count=0 ; count<sidCount ; ++count)
		{
		TUint32 sid = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompSid, count);
		sidList.push_back(sid);
		}

	return sidList;
	}

Controllers DbHelper::GetControllerInfo( TInt32 aComponentId ) const
	{
	std::vector<ControllerInfo*> controllerInfoList;
	
	int controllerCount = GetComponentPropertyIntValue(aComponentId,DbConstants::CompControllerCount, DbConstants::KDefaultControllerCount);

	for(int count=0 ; count<controllerCount ; ++count)
		{
		ControllerInfo* controllerInfo = new ControllerInfo();
		std::wstring strVersion = GetComponentPropertyStrValue(aComponentId, DbConstants::CompControllerVersion, count);
		Version version = FormatStringToVersion(strVersion);

		TInt offset = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompControllerOffset, count);
		TInt hashAlgo = GetComponentPropertyIntFormattedValue(aComponentId, DbConstants::CompControllerHashAlgorithm, count);
		std::string hashData(wstring2string(GetComponentPropertyStrValue(aComponentId, DbConstants::CompControllerHashData, count)));
		HashContainer hashContainer;
		hashContainer.SetHashId(hashAlgo);
		hashContainer.SetData(hashData);
		
		controllerInfo->SetVersion(version);
		controllerInfo->SetOffset(offset);
		controllerInfo->SetHashContainer(hashContainer);

		controllerInfoList.push_back(controllerInfo);
		}
	
	return controllerInfoList;
	}

TInt DbHelper::GetIndex( TUint32 aPackageUid) const
	{
	TInt32 componentId = GetComponentId(aPackageUid);
	return GetComponentPropertyIntValue(componentId, DbConstants::CompIndex);
	}

TInt32 DbHelper::GetComponentPropertyIntValue(TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aDefaultValue) const
	{
	TInt32 propertyValue = 0;
	
	std::string selectIntProperty("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name=?;");
	std::auto_ptr<CStatement> stmtCompProperty(iScrDbHandler->PrepareStatement(selectIntProperty));
		
	stmtCompProperty->BindInt(1, aComponentId);
	stmtCompProperty->BindStr(2, aComponentPropertyName);

	if(!stmtCompProperty->ProcessNextRow())
		return aDefaultValue;

	return stmtCompProperty->IntColumn(0);
	}

std::vector<TInt32> DbHelper::GetServiceInfoServiceID(TInt32 aAppUid) const
{
	std::vector<TInt32> serviceIdList;
	std::string selectServiceId("SELECT ServiceId FROM ServiceInfo WHERE AppUid=?;");
	std::auto_ptr<CStatement> stmtServiceId(iScrDbHandler->PrepareStatement(selectServiceId));
	
	stmtServiceId->BindInt(1, aAppUid);
	while(stmtServiceId->ProcessNextRow())
	{
		serviceIdList.push_back(stmtServiceId->IntColumn(0));
	}
	
	return serviceIdList;
}


std::vector<TInt32> DbHelper::GetLocalAppInfoID(TInt32 aAppUid) const
{
	std::vector<TInt32> localAppInfoIdList;
	std::string selectLocalAppInfoId("SELECT LocalAppInfoId FROM LocalizableAppInfo WHERE AppUid=?;");
	std::auto_ptr<CStatement> stmtLocalAppInfoId(iScrDbHandler->PrepareStatement(selectLocalAppInfoId));
	
	stmtLocalAppInfoId->BindInt(1, aAppUid);
	while(stmtLocalAppInfoId->ProcessNextRow())
	{
		localAppInfoIdList.push_back(stmtLocalAppInfoId->IntColumn(0));
	}
	
	return localAppInfoIdList;
}


std::vector<TInt32> DbHelper::GetLocalCaptionAndIconID(TInt32 aAppUid) const
{
	std::vector<TInt32> localCaptionAndIconIdList;
	std::string selectLocalCaptionAndIconId("SELECT CaptionAndIconId FROM LocalizableAppInfo WHERE AppUid=?;");
	std::auto_ptr<CStatement> stmtLocalCaptionAndIconId(iScrDbHandler->PrepareStatement(selectLocalCaptionAndIconId));
	
	stmtLocalCaptionAndIconId->BindInt(1, aAppUid);
	while(stmtLocalCaptionAndIconId->ProcessNextRow())
	{
		localCaptionAndIconIdList.push_back(stmtLocalCaptionAndIconId->IntColumn(0));
	}
	
	return localCaptionAndIconIdList;
}


std::vector<TInt32> DbHelper::GetViewDataCaptionAndIconID(TInt32 aLocalAppInfoID) const
{
	std::vector<TInt32> viewCaptionIdList;
	std::string selectViewCaptionId("SELECT CaptionAndIconId FROM ViewData WHERE LocalAppInfoId=?;");
	std::auto_ptr<CStatement> stmtViewCaptionId(iScrDbHandler->PrepareStatement(selectViewCaptionId));
	
	stmtViewCaptionId->BindInt(1, aLocalAppInfoID);
	while(stmtViewCaptionId->ProcessNextRow())
	{
		viewCaptionIdList.push_back(stmtViewCaptionId->IntColumn(0));
	}
	
	return viewCaptionIdList;
}


std::vector<TInt32> DbHelper::GetAppRegistrationInfoAppUid(TInt32 aComponentId) const
{
	std::vector<TInt32> appUidList;
	std::string selectAppUid("SELECT AppUid FROM AppRegistrationInfo WHERE ComponentId=?;");
	std::auto_ptr<CStatement> stmtAppUid(iScrDbHandler->PrepareStatement(selectAppUid));

	stmtAppUid->BindInt(1, aComponentId);
	while(stmtAppUid->ProcessNextRow())
	{
		appUidList.push_back(stmtAppUid->IntColumn(0));
	}

	return appUidList;
}

TInt32 DbHelper::GetComponentPropertyIntFormattedValue(TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aIntFormatter) const
	{
	TInt32 propertyValue = 0;
	
	std::string selectIntProperty("SELECT IntValue FROM ComponentProperties WHERE ComponentId=? and Name=?;");
	std::auto_ptr<CStatement> stmtCompProperty(iScrDbHandler->PrepareStatement(selectIntProperty));

	std::wstring formattedCompPropName = FormatDbString(aComponentPropertyName, aIntFormatter);
			
	stmtCompProperty->BindInt(1, aComponentId);
	stmtCompProperty->BindStr(2, formattedCompPropName);

	if(!stmtCompProperty->ProcessNextRow())
		return DbHelper::Absent;
	
	return stmtCompProperty->IntColumn(0);
	}

std::wstring DbHelper::GetComponentPropertyStrValue(TInt32 aComponentId, std::wstring aComponentPropertyName) const
	{
	TInt32 propertyValue = 0;
	
	std::string selectStrProperty("SELECT StrValue FROM ComponentProperties WHERE ComponentId=? and Name=?;");
	std::auto_ptr<CStatement> stmtCompProperty(iScrDbHandler->PrepareStatement(selectStrProperty));

	stmtCompProperty->BindInt(1, aComponentId);
	stmtCompProperty->BindStr(2, aComponentPropertyName);

	if(!stmtCompProperty->ProcessNextRow())
		return NULL;
	
	return stmtCompProperty->StrColumn(0);
	}

std::wstring DbHelper::GetComponentPropertyStrValue(TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aIntFormatter) const
	{
	
	std::wstring formattedCompPropName = FormatDbString(aComponentPropertyName, aIntFormatter);
			
	return GetComponentPropertyStrValue(aComponentId, formattedCompPropName);
	}

std::wstring DbHelper::FormatDbString(std::wstring aString, int aValue) const
	{
	std::wstringstream formatter;
	formatter << aString << aValue;
	return formatter.str();
	}

Version DbHelper::FormatStringToVersion(std::wstring aVersionStr) const
	{
	
	std::wstringstream strMajor(aVersionStr.substr(0,1));
	int major = 0;
	strMajor >> major;
	std::wstringstream strMinor(aVersionStr.substr(2,3));
	int minor = 0;
	strMinor >> minor;
	std::wstringstream strBuild(aVersionStr.substr(4,4));
	int build = 0;
	strBuild >> build;

	return Version(major,minor,build);

	}
