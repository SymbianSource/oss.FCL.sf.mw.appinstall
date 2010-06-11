/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file CONFIGMANAGER.H
 @internalComponent
 @released
*/
#ifndef	CONFIGMANAGER_H
#define	CONFIGMANAGER_H

#pragma warning (disable: 4786)

// System includes
#include <istream>
#include <list>
#include <vector>
#include <map>
#include <exception>

// User includes
#include "symbiantypes.h"
#include "parameterlist.h"


// Structures referenced
struct ConfigAttribute;

const static int KVariableLanguage = 0x1000;
const static int KVariableDrive = 0x1001;
const static int KVariableDevSupLng = 0x1002;

// Structures
struct DriveAttributes
	{
	std::wstring 	iDir;
	bool			iExternal;
	
	DriveAttributes() : iExternal(false) {};
	};

typedef std::map< int /* drive letter */, DriveAttributes* > DrivesMap;

/** 
 * @publishedPartner
 * @released
 */
class ConfigManagerException 
    {
public: // Enumerations
    enum TType
        {
        ETypeKeywordNotSupported = 0,
        ETypeInvalidValueForKey,
        ETypeDuplicateDefinition,
		ETypeInvalidDirectory,
		ETypeDriveError
        };

public:
    inline ConfigManagerException( TType aType, const std::string& aKey, const std::string& aValue, int aLineNumber )
    :   iType( aType ), iKey( aKey ), iValue( aValue ), iLineNumber( aLineNumber )
        {
        }

	inline ConfigManagerException( TType aType, const std::string& aValue )
    :   iType( aType ), iValue( aValue ), iLineNumber( 0 )
        {
        }

    ~ConfigManagerException();

public:
    void Display() const;

private:
    TType iType;
    std::string iKey;
    std::string iValue;
    int iLineNumber;
    };

/**
 * @internalComponent
 * @released
 */
class ConfigManager
    { 
public: // Constructors & destructor
    ConfigManager(const CParameterList& aParamList);
    ~ConfigManager();
    
public: // API
    void SetValue( TUint32 aKey, TUint32 aValue );
    bool ValueExists( TUint32 aKey ) const;
    TUint32 ValueById( TUint32 aKey ) const;
    static std::string AttributeNameById( TUint32 aId );
	std::wstring GetLocalDrivePath(int aDrive);
	bool IsTargetDrivePresent(int aDrive) const;
	bool IsTargetDriveExt(int aDrive) const;
	bool AddDrive(DriveAttributes* aDrive, const int aDriveLetter);
	std::vector<TInt>& GetDeviceSupportedLanguages();
	void CheckAndAddDrive(const int aDriveLetter, const std::wstring& aDir,
							const bool aExternal = false);
	const DrivesMap& GetDrivesMap() const 
		{ return iDrives; }
	void AddMatchingSupportedLanguages(TInt aMatchingSupportedLangauges);
	std::vector<TInt>& GetMatchingSupportedLanguages();

private: // Internal methods
	void AddRomAndSystemDrives(const CParameterList& aParamList);
    void ReadFile( std::ifstream& aStream );
    void ProcessLine( const std::string& aLine, int aLineNumber );
    void ConvertLineData( const std::string& aKey, std::string& aValue, int aLineNumber );
    bool AddDeviceSupportedLanguage (TInt langId);

private: // Utility functions
    static const ConfigAttribute* AttributeByName( const std::string& aName );
    static bool TryStringToNumeric( const std::string& aText, TUint32& aNumber );
    static bool TryStringToBool( const std::string& aText, TUint32& aNumber );
    static bool TryStringToYesNo( const std::string& aText, TUint32& aNumber );
	static int ConvertToDriveAttributes( const std::string& aString, DriveAttributes* aDrive );

private: // Type definitions
    typedef std::map< TUint32 /* attribute id */, TUint32 /* attribute value */ > ConfigurationMap;
	std::vector<TInt>                       iDeviceSupportedLanguages; // Vector containing the list of device supported languages
	std::vector<TInt>                       iMatchingSupportedLanguages; // Vector containing the list of matching supported languages from the SIS files
private: // Data members
    ConfigurationMap	iValues;
	DrivesMap			iDrives;
    };

#endif
