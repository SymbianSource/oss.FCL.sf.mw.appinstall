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
* database.h
* CDatabaseManager - The main purpose of this class is to load the appropriate sqlite library.
* Load the appropriate functions which are to be used by the CDatabase class to access the 
* sqlite library. The second functions of this class is to provide as an interface to receive
* data which is to be updated via the CDatabase class.
*
*/


/**
 @file
 @released
 @internalTechnology
*/

#ifndef SCRDBMANAGER_H
#define SCRDBMANAGER_H

#pragma warning(disable: 4786)

#include "dbprocessor.h"
#include "xmlparser.h"
#include "./sqlite/sqlite3.h"

#include <string>

#ifdef __TOOLS2_LINUX__

// utf16WString represents the UTF-16 data(WINDOWS wstring).
typedef std::basic_string<unsigned short int> utf16WString;

#else

// Under WINDOWS, wstring will be in UTF-16 format itself.
typedef std::wstring utf16WString;

#endif


class CDbLayer
	{
	public:
		CDbLayer(const std::string& aDllPath, const std::string& aSqlDbName);
		~CDbLayer();
		void CreateScrDatabase(const std::vector<std::string>& aCreateDbQueries);
		void PopulateScrDatabase(const std::vector<XmlDetails::TScrEnvironmentDetails>& aScrEnvDetails);
		void PopulatePreProvisionDetails(const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetailList);
	private:
		void CreateDatabase(const std::string& aDbFileName);
		
		void PopulateDatabase(const std::vector<XmlDetails::TScrEnvironmentDetails>& aScrEnvDetails);
		
		void AddPreProvisionDetails(const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetailList);
		
		bool AddComponentDetails(const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::wstring& aSoftwareTypeName);
		
		int GetInstalledDrives(const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>& aComponentFiles );
		
		void AddComponentLocalizables
			( 
			int aComponentId, 
			const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentLocalizable>& aComponentLocalizable
			);
		
		void AddComponentProperties
			( 
			int aComponentId, 
			const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentProperty>& aComponentProperty
			);
		
		void AddComponentFiles(int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile>& aComponentFiles);

		void AddComponentDependencies(int aComponentId, const XmlDetails::TScrPreProvisionDetail::TComponentDependency& aComponentDependency, const std::wstring& aSoftwareTypeName);
		
		void AddComponentDependency
			(
			int aComponentId, 
			const XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail& aComponentDependencyDetail	
			);

		void AddLocation(int aComponentId, const std::wstring& aLocation);
		
		void AddFileProperties(int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty>& aFileProperty);
		
		void ExecuteStatement(const std::string& aStmtStr);
		

		void AddApplicationRegistrationInfo( int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo>& aApplicationRegistrationInfo);

		int AddAppAttribute(int aComponentId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute>& aAppAttribute);

		void AddFileOwnershipInfo(int aAppUid, const std::vector<std::wstring>& aFileOwnershipInfo);

		void AddServiceInfo( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo>& aApplicationServiceInfo);

		void AddAppLocalizableInfo( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo>& aApplicationLocalizableInfo);

		void AddLocalizableAttribute( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute>& aApplicationLocalizableAttribute);

		void AddViewData( int alocalAppInfoId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData>& aViewData);

		void AddViewDataAttributes( int alocalAppInfoId, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes>& aViewDataAttribute);
			
		void AddProperty( int aAppUid, const std::vector<XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty>& aApplicationProperty);
				
		const std::wstring GetLocalTime();
			
	private:
		CDbProcessor* iScrDbHandler;
		
	};

#endif //SCRDBMANAGER_H
