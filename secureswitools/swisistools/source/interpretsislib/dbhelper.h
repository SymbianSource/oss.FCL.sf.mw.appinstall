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
* DbHelper - Interfaces for accessing details from the database.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef DBHELPER_H
#define DBHELPER_H

#pragma warning(disable: 4786)

#include <string>
#include <vector>
#include "symbiantypes.h"

class CDbProcessor;
class SisRegistryObject;
class SisRegistryToken;
class SisRegistryPackage;
class FileDescription;
class CStatement;
class SisRegistryDependency;
class Version;
class SisRegistryProperty;
class TrustStatus;
class ControllerInfo;

class DbHelper
	{
	public:
		DbHelper( const std::wstring& aDrivePath, bool aIsSystemDrive);
		~DbHelper();
		TBool IsInstalled(TInt32 aUid) const;
		TBool IsAppUidInstalled(TUint32 aAppUid) const;
		bool InternalizeIfExists(SisRegistryObject& aSisRegistryObject, const TUint32 aUid ) const;
		
		bool GetIsRemovable( TInt32 aComponentId ) const;
		bool GetInRom( TInt32 aComponentId ) const;
		std::wstring GetStubFileName( TInt32 aComponentId ) const;
		std::wstring GetPackageName( TInt32 aComponentId ) const;
		TUint32 GetUid( TInt32 aComponentId ) const;
		TUint32 GetInstallType( TInt32 aComponentId ) const;
		void RemoveEntry( TInt32 aComponentId ) const;
		int GetSelectedDrive( TInt32 aComponentId );
		std::vector<FileDescription*> GetFileDescriptions( TInt32 aComponentId ) const;
		bool SidExists(const TUint32 aSid) const;
		TUint32 GetUid(const TUint32 aSid) const;
		std::wstring SidToFileName(TUint32 aSid) const;
		TUint32 GetUid( const std::wstring& aSidFile ) const;

		TInt32 GetComponentId(TInt32 aUid, const std::wstring aPackageName, const std::wstring aVendorName ) const;
		TInt32 GetComponentId(TInt32 aUid, TInt32 aInstallType ) const;
		TInt32 GetComponentId( TInt32 aUid ) const;
		std::vector<TInt32> GetComponentIdList( TInt32 aUid ) const;
		TInt GetIndex( TInt32 aComponentId) const;
		TUint32 GetUidFromFileName( const std::wstring& aFileName ) const;
		TInt GetIndex( TUint32 aPackageUid) const;

	private:
		std::vector<TInt32> GetAppRegistrationInfoAppUid(TInt32 aComponentId) const;
		std::vector<TInt32> GetServiceInfoServiceID(TInt32 aAppUid) const;
		std::vector<TInt32> GetLocalAppInfoID(TInt32 aAppUid) const;
		std::vector<TInt32> GetLocalCaptionAndIconID(TInt32 aAppUid) const;
		std::vector<TInt32> GetViewDataCaptionAndIconID(TInt32 aLocalAppInfoID) const;
		TInt32 GetComponentPropertyIntValue( TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aDefaultValue = DbHelper::Absent ) const;
		TInt32 GetComponentPropertyIntFormattedValue(TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aIntFormatter) const;
		std::wstring GetComponentPropertyStrValue(TInt32 aComponentId, std::wstring aComponentPropertyName, TInt32 aIntFormatter) const;
		std::wstring GetComponentPropertyStrValue(TInt32 aComponentId, std::wstring aComponentPropertyName) const;

		void GetComponentDetails( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const;
		void GetComponentLocalizables( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const;
		void GetFileDescriptions( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const;
		void GetFileDescriptionFromComponentProperties( int aComponentId, std::vector<FileDescription*>& aFileDescriptions) const;
		void GetFileDescriptionFromFileProperties( int aComponentId, std::vector<FileDescription*>& aFileDescriptions) const;
		void GetComponentProperties( TInt32 aComponentId, SisRegistryObject& aSisRegistryObject ) const;
		std::vector<SisRegistryDependency*> GetDependencies( TInt32 aComponentId ) const;
		std::vector<SisRegistryPackage*> DbHelper::GetEmbeddedPackages( TInt32 aComponentId ) const;
		std::vector<SisRegistryProperty*> GetProperties( TInt32 aComponentId ) const;
		TrustStatus GetTrustStatus( TInt32 aComponentId ) const;
		std::vector<TInt> GetInstallChainIndices( TInt32 aComponentId ) const;
		std::vector<TUint32> GetSids( TInt32 aComponentId ) const;
		std::vector<ControllerInfo*> GetControllerInfo( TInt32 aComponentId ) const;

	private:
		// other helper methods
		std::wstring FormatDbString(std::wstring aString, int aValue) const;
		void AddFileDescription(CStatement* aStatement, FileDescription& aFileDescription, TInt aCount ) const;
		Version FormatStringToVersion(std::wstring aVersionStr) const;

	private:
		CDbProcessor* iScrDbHandler;
		
	public:
		const static int Absent;
	};

#endif // DBHELPER_H
