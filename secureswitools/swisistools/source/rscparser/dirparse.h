// dirparse.h

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
//

/**
 @file 
 @internalComponent
 @released
*/

#ifndef	__DIRPARSE_H__
#define	__DIRPARSE_H__

#pragma warning (disable: 4786)

// System includes
#include <iostream>
#include <string>
#include "xmlgenerator.h"
#include "xmlparser.h"
#include "aplappinforeader.h"
#include "aplapplistitem.h"
#include "interpretsis.h"
#include "dbhelper.h"

/**
 * Identify if input file is a Registration Resource File.
 @ Param : aRegistrationFileName - File name to be scanned.
 */
TInt FindRegistrationResourceFileL(std::string& aRegistrationFileName);
/**
 * Get the path of Database "scr.db"
 */
std::string GetDbPath(const CParameterList* aParamList);
/**
 * Creates Class TAppDataType for XML Parsing
 */
void CreateAppDataType(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo& aAppDataType, 
								const TDataTypeWithPriority* aTemp);
/**
 * Creates Class TViewData for XML Parsing
 */
void CreateViewDataL(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo& aLocalizableAttribute, 
								const CAppLocalizableInfo* aAppInfoReader);

/**
 * Creates Class TViewDataAttributes for XML Parsing
 */
void CreateViewDataAttributes(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData& aViewData, 
							const std::wstring& aName,const std::wstring& aValue, 
							const bool aIsIntValue, const int aIsStr8Bit);

/**
 * Creates Class TLocalizableAttribute for XML Parsing
 */
void CreateLocalizableAttribute(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo& aLocalizableAttribute, 
								const std::wstring& aName,const std::wstring& aValue, const bool aIsIntValue, const int aIsStr8Bit);
/**
 * Creates Class TAppAttribute for XML Parsing
 */
void CreateAppAttribute(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppAttribute, 
								const std::wstring& aName,const std::wstring& aValue, const bool aIsIntValue, const int aIsStr8Bit );
/**
 * Creates Class TAppAttribute for XML Parsing
 */
void CreateAppProperty(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppProperty, 
								const std::string& aStrValue, const int aLocale, const int aServiceUid, const bool aIsStr8Bit );

/**
 * Creates Class TOpaqueDataType for XML Parsing
 */
void CreateOpaqueDataType(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppOpaqueData, 
								const std::string& aStrValue, const int aLocale, const int aServUid);

/**
 * Creates Class TAppLocalizableInfo for XML Parsing
 */
void CreateLocalizableInfoL(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppLocalizableInfo, 
								const CAppLocalizableInfo* tp);
/**
 * Creates Class TAppServiceInfo for XML Parsing
 */
void CreateAppServiceInfo(XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppServiceInfo, 
								const TUid& aUid, const std::vector<TDataTypeWithPriority*>& aDataType);

/**
 * Creates Class TApplicationRegistrationInfo for XML Parsing
 */
 
void CreateApplicationRegistrationInfoL(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
								const CAppInfoReader* aAppInfoReader);

/**
 * Update Installation Information in Database using SRCTOOL
 */
void UpdateInstallationInformation_xml(const CParameterList* aParamList,XmlDetails::TScrPreProvisionDetail aScrPreProvisionDetail);
/**
 * Generate XML for Class AppInfoReader and call the function to Update Installation Information in Database using SRCTOOL
 */
void ReadApplicationInformationFromResourceFilesL(XmlDetails::TScrPreProvisionDetail& aScrPreProvisionDetail, 
															const std::string& aFileName,const CParameterList* aParamList, 
															const CInterpretSIS& aInterpretSis, int& aNewFileFlag);

/**
 * Parse the Directory for resource file.
 */
void ParseResourceDir(const CParameterList* aParamList, const CInterpretSIS& aInterpretSis);

/**
 * Backup hash files for re-installation (SA over SA or PU over PU)
 */
void BackupHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath);

/**
 * Restore hash files for re-installation (SA over SA or PU over PU)
 */
void RestoreHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath);

/**
 * Utility function to Replace "/" to "\"
 */
int FirstInvalidDirSeparatorSizetap(std::wstring& aPath, std::wstring::size_type& aIndex);

/**
 * Utility function to Replace "/" to "\"
 */
std::wstring FixPathDelimiterstap( const std::wstring& aString );
#endif //__DIRPARSE_H__
