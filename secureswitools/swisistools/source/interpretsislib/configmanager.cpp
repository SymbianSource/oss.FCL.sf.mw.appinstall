/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#pragma warning (disable: 4786)

// System includes
#include <iostream>
#include <fstream>
#include <hal_data.h>
#include <algorithm>
 

// User includes
#include "stringutils.h"
#include "is_utils.h"
#include "configmanager.h"

// Enumerations
enum TConfigAttributeFlag
    {
    EConfigFlagTypeNotSupported = 1,
    EConfigFlagTypeNumeric      = 2,
    EConfigFlagTypeBoolean      = 4,
    EConfigFlagTypeYesNo        = 8,
	EConfigFlagTypeString		= 16
    };

// Structures
struct ConfigAttribute
	{
	const wchar_t* iName;
	TUint32 iId;
    TUint32 iFlags;
	};

// Constants
const std::string KConfigAttributeHexPrefix = "0x";
const std::string KConfigAttributeValueTrue = "TRUE";
const std::string KConfigAttributeValueFalse = "FALSE";
const std::string KConfigAttributeValueYes = "YES";
const std::string KConfigAttributeValueNo = "NO";

const ConfigAttribute KConfigAttributes[] =
	{
    // HAL entries
	{L"MANUFACTURER",				HALData::EManufacturer,                 EConfigFlagTypeNumeric },
	{L"MANUFACTURERHARDWAREREV",	HALData::EManufacturerHardwareRev,      EConfigFlagTypeNumeric },
	{L"MANUFACTURERSOFTWAREREV",	HALData::EManufacturerSoftwareRev,      EConfigFlagTypeNumeric },
	{L"MANUFACTURERSOFTWAREBUILD",	HALData::EManufacturerSoftwareBuild,    EConfigFlagTypeNumeric },
	{L"MODEL",						HALData::EModel,                        EConfigFlagTypeNumeric },
	{L"MACHINEUID",					HALData::EMachineUid,                   EConfigFlagTypeNumeric },
	{L"DEVICEFAMILY",				HALData::EDeviceFamily,                 EConfigFlagTypeNumeric },
	{L"DEVICEFAMILYREV",			HALData::EDeviceFamilyRev,              EConfigFlagTypeNumeric },
	{L"CPU",						HALData::ECPU,                          EConfigFlagTypeNumeric },
	{L"CPUARCH",					HALData::ECPUArch,                      EConfigFlagTypeNumeric },
	{L"CPUABI",						HALData::ECPUABI,                       EConfigFlagTypeNumeric },
	{L"CPUSPEED",					HALData::ECPUSpeed,                     EConfigFlagTypeNumeric },
	{L"SYSTEMTICKPERIOD",			HALData::ESystemTickPeriod,             EConfigFlagTypeNumeric },
	{L"MEMORYRAM",					HALData::EMemoryRAM,                    EConfigFlagTypeNumeric },
	{L"MEMORYRAMFREE",				HALData::EMemoryRAMFree,                EConfigFlagTypeNumeric },
	{L"MEMORYROM",					HALData::EMemoryROM,                    EConfigFlagTypeNumeric },
	{L"MEMORYPAGESIZE",				HALData::EMemoryPageSize,               EConfigFlagTypeNumeric },
	{L"POWERBACKUP",				HALData::EPowerBackup,                  EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"KEYBOARD",					HALData::EKeyboard,                     EConfigFlagTypeNumeric },
	{L"KEYBOARDDEVICEKEYS",			HALData::EKeyboardDeviceKeys,           EConfigFlagTypeNumeric },
	{L"KEYBOARDAPPKEYS",			HALData::EKeyboardAppKeys,              EConfigFlagTypeNumeric },
	{L"KEYBOARDCLICK",				HALData::EKeyboardClick,                EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"KEYBOARDCLICKVOLUMEMAX",		HALData::EKeyboardClickVolumeMax,       EConfigFlagTypeNumeric },
	{L"DISPLAYXPIXELS",				HALData::EDisplayXPixels,               EConfigFlagTypeNumeric },
	{L"DISPLAYYPIXELS",				HALData::EDisplayYPixels,               EConfigFlagTypeNumeric },
	{L"DISPLAYXTWIPS",				HALData::EDisplayXTwips,                EConfigFlagTypeNumeric },
	{L"DISPLAYYTWIPS",				HALData::EDisplayYTwips,                EConfigFlagTypeNumeric },
	{L"DISPLAYCOLORS",				HALData::EDisplayColors,                EConfigFlagTypeNumeric },
	{L"DISPLAYCONTRASTMAX",			HALData::EDisplayContrastMax,           EConfigFlagTypeNumeric },
	{L"BACKLIGHT",					HALData::EBacklight,                    EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"PEN",						HALData::EPen,                          EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"PENX",						HALData::EPenX,                         EConfigFlagTypeNumeric },
	{L"PENY",						HALData::EPenY,                         EConfigFlagTypeNumeric },
	{L"PENDISPLAYON",				HALData::EPenDisplayOn,                 EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"PENCLICK",					HALData::EPenClick,                     EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"PENCLICKVOLUMEMAX",			HALData::EPenClickVolumeMax,            EConfigFlagTypeNumeric },
	{L"MOUSE",						HALData::EMouse,                        EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"MOUSEX",						HALData::EMouseX,                       EConfigFlagTypeNumeric },
	{L"MOUSEY",						HALData::EMouseY,                       EConfigFlagTypeNumeric },
	{L"MOUSEBUTTONS",				HALData::EMouseButtons,                 EConfigFlagTypeNumeric },
	{L"CASESWITCH",					HALData::ECaseSwitch,                   EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"LEDS",						HALData::ELEDs,                         EConfigFlagTypeNumeric },
	{L"INTEGRATEDPHONE",			HALData::EIntegratedPhone,              EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"DISPLAYBRIGHTNESS",			HALData::EDisplayBrightness,            EConfigFlagTypeNumeric },
	{L"DISPLAYBRIGHTNESSMAX",		HALData::EDisplayBrightnessMax,         EConfigFlagTypeNumeric },
	{L"KEYBOARDBACKLIGHTSTATE",		HALData::EKeyboardBacklightState,       EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"ACCESSORYPOWER",				HALData::EAccessoryPower,               EConfigFlagTypeNumeric | EConfigFlagTypeBoolean | EConfigFlagTypeYesNo },
	{L"SYSTEMDRIVE",				HALData::ESystemDrive,                  EConfigFlagTypeNumeric },
	{L"FPHARDWARE",					HALData::EHardwareFloatingPoint,        EConfigFlagTypeNumeric },
	{L"NUMHALATTRIBUTES",			HALData::ENumHalAttributes,             EConfigFlagTypeNumeric },

    // Custom entries for Interpretsis only
	{L"LANGUAGE",					KVariableLanguage,                      EConfigFlagTypeNumeric },
	{L"DRIVE",						KVariableDrive,							EConfigFlagTypeString },
	{L"DEVICE_SUPPORTED_LANGUAGE",	KVariableDevSupLng,						EConfigFlagTypeNumeric }
	};


ConfigManager::ConfigManager(const CParameterList& aParamList)
    {
	const std::wstring& fileName = aParamList.ConfigFileName();

    if ( FileExists( fileName ) )
        {
    	std::string fName;
        fName = wstring2string( fileName );
        //
        std::ifstream stream;
	    stream.open( fName.c_str(), std::ios::binary );
        //
        try
            {
            ReadFile( stream );
            stream.close();
            }
        catch( const ConfigManagerException& e )
            {
            stream.close();
            throw e;
            }
        }

	// Add the Rom drive and System drive definition to the DriveMap
	// So all the drives info. will be available from ConfigManager
	AddRomAndSystemDrives(aParamList);
    }


ConfigManager::~ConfigManager()
    {
	// Clean up iDrives
	for( DrivesMap::iterator it=iDrives.begin(); it != iDrives.end(); it++ )
        {
        DriveAttributes* entry = it->second;
		delete entry;
        }
	iDeviceSupportedLanguages.clear();
	iMatchingSupportedLanguages.clear();
    }


void ConfigManager::AddRomAndSystemDrives(const CParameterList& aParamList)
	{
	if ( aParamList.IsFlagSet(CParameterList::EFlagsZDriveSet)) 
		{
		CheckAndAddDrive('z', aParamList.RomDrivePath());
		}

	if ( IsTargetDrivePresent('z') )
		{
		if (aParamList.IsFlagSet(CParameterList::EFlagsRomRofsLogFilesSet) &&
			aParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet) == 0 )
			{
			// z drive is set via ROM/ROFS logs, otherwise, command parser would have throw an exception
			std::stringstream err;
			err << "Cannot specify both -r option in command line and define the directory "
				<< "representing the ROM drive in config file";
			throw ConfigManagerException( ConfigManagerException::ETypeDriveError, err.str());
			}
		}
	else
		{
		if (!aParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet))
			{
			// z drive defiend by ROM/ROFS logs
			if (aParamList.IsFlagSet(CParameterList::EFlagsRomRofsLogFilesSet))
				{
				CheckAndAddDrive('z', L"");
				}
			else
				{
				// z drive is not set via config.ini or by -z option
				std::stringstream err;
				err << "Missing -r option and the ROM drive representing directory is not defined; "
					<< "Must specify one of them (BUT not both)";
				throw ConfigManagerException( ConfigManagerException::ETypeDriveError, err.str());
				}
			}
		}
	
	if (aParamList.IsFlagSet(CParameterList::EFlagsCDriveSet)) 
		{
		CheckAndAddDrive(aParamList.SystemDriveLetter(), aParamList.SystemDrivePath());
		}

	if ( !IsTargetDrivePresent(aParamList.SystemDriveLetter()) )
		{
		std::stringstream err;
		err << "The directory representing the system drive is not defined";
		throw ConfigManagerException( ConfigManagerException::ETypeDriveError, err.str());
		}
	}


void ConfigManager::SetValue( TUint32 aKey, TUint32 aValue )
    {
    iValues[ aKey ] = aValue;
    }


bool ConfigManager::ValueExists( TUint32 aKey ) const
    {
    ConfigurationMap::const_iterator it = iValues.find( aKey );
    bool found = ( it != iValues.end() );
    return found;
    }


TUint32 ConfigManager::ValueById( TUint32 aKey ) const
    {
    assert( ValueExists( aKey ) );
    ConfigurationMap::const_iterator it = iValues.find( aKey );
    const TUint32 ret = it->second;
    return ret;
    }


void ConfigManager::ReadFile( std::ifstream& aStream )
    {
    int lineNumber = 1;
    std::string line;
    //
    while( std::getline( aStream, line ) )
        {
        ProcessLine( line, lineNumber++ );
        }
    }


void ConfigManager::ProcessLine( const std::string& aLine, int aLineNumber )
    {
	std::string line = aLine;

    // Strip comments
	std::string::size_type commentPos = line.find( "//" );
    if ( commentPos != std::string::npos )
        {
        line = line.substr( 0, commentPos );
        }

    if  ( line.length() )
        {
    	std::string::size_type breakPos = line.find( "=" );
        if ( breakPos != std::string::npos )
            {
        	std::string value = line.substr( breakPos + 1 );
            line = line.substr( 0, breakPos );

            // Trim & tidy up before the conversion phase
            line = StringUtils::TrimWhiteSpace( line );
            value = StringUtils::TrimWhiteSpace( value );

            if ( line.length() && value.length() )
                {
                ConvertLineData( line, value, aLineNumber );
                }
            }
        }
    }

std::vector<TInt>& ConfigManager::GetDeviceSupportedLanguages()
	{
		return iDeviceSupportedLanguages;
	}

void ConfigManager::ConvertLineData( const std::string& aKey, std::string& aValue, int aLineNumber )
    {
    const ConfigAttribute* attrib = AttributeByName( aKey );
    if ( attrib )
        {
        // Now try to convert the value string according to the attribute's
        // conversion specification.
        if ( attrib->iFlags & EConfigFlagTypeNotSupported )
            {
            throw ConfigManagerException( ConfigManagerException::ETypeKeywordNotSupported, 
                                          aKey,
                                          aValue,
                                          aLineNumber );
            }
        else
            {
            TUint32 num = 0;

            // First try numeric conversion, since this satsifies most of
            // the entries. Then try bool / yes / no
            if ( ( attrib->iFlags & EConfigFlagTypeNumeric ) && TryStringToNumeric( aValue, num ) )
                {
            	if ( attrib->iId == KVariableDevSupLng )
            		{
            		if ( !AddDeviceSupportedLanguage(num))
            			{
    					throw ConfigManagerException( ConfigManagerException::ETypeDuplicateDefinition, 
    													aKey, aValue, aLineNumber );
    					}
                	return;
            		}

                }
            else if ( ( attrib->iFlags & EConfigFlagTypeBoolean ) && TryStringToBool( aValue, num ) )
                {
                }
            else if ( ( attrib->iFlags & EConfigFlagTypeYesNo ) && TryStringToYesNo( aValue, num ) )
                {
                }
			else if ( ( attrib->iFlags & EConfigFlagTypeString ) && (attrib->iId == KVariableDrive) )
				{
	            DriveAttributes* newDrive = new DriveAttributes();
				int driveLetter = ConvertToDriveAttributes(StringUtils::ToLower(aValue), newDrive);
				
				if (driveLetter == 0)
					{
					delete newDrive;
					throw ConfigManagerException( ConfigManagerException::ETypeInvalidValueForKey, 
													aKey, aValue, aLineNumber );
					}
				
				if ( !IsDirectory(newDrive->iDir) )
					{
					delete newDrive;
					throw ConfigManagerException( ConfigManagerException::ETypeInvalidDirectory, 
													aKey, aValue, aLineNumber );
					}

				if (!AddDrive(newDrive, driveLetter))
					{
					delete newDrive;
					throw ConfigManagerException( ConfigManagerException::ETypeDuplicateDefinition, 
													aKey, aValue, aLineNumber );
					}

				return;
				}
            else
                {
                throw ConfigManagerException( ConfigManagerException::ETypeInvalidValueForKey, 
                                              aKey,
                                              aValue,
                                              aLineNumber );
                }

            // Assume okay if got here.
            if ( !ValueExists( attrib->iId ) )
                {
                SetValue( attrib->iId, num );
                }
            else
                {
                throw ConfigManagerException( ConfigManagerException::ETypeDuplicateDefinition, 
                                              aKey,
                                              aValue,
                                              aLineNumber );
                }
            }
        }
	else
		{ 
	 	// DEF117196
	 	// Config files can be created by the licensee from HAL HDA files. It is cumbersome if they 
	 	// produce an error and halt if there is an attribute defined in the config file which is not 
	 	// relevant to interpretsis.
	 	// Instead a warning is produced.
		std::ostringstream stream;
    	stream << "Unsupported keyword at line " << aLineNumber << " of ini file [" << aKey << " = " << aValue << "] ";
    	stream << std::endl;
    	std::wstring finalMessage = string2wstring( stream.str() );
		LWARN( finalMessage );
		}
    }


std::string ConfigManager::AttributeNameById( TUint32 aId )
    {
	std::string ret = "Unknown/Unsupported attribute";
    //
    const int attributeCount = sizeof( KConfigAttributes ) / sizeof( ConfigAttribute );
	for( int i=0; i<attributeCount; i++ )
		{
		if  ( KConfigAttributes[i].iId == aId )
			{
			std::wstring entry( KConfigAttributes[i].iName );
            ret = wstring2string( entry );
			}
		}
    //
    return ret;
    }


const ConfigAttribute* ConfigManager::AttributeByName( const std::string& aName )
    {
	std::string upperCased( aName );
    upperCased = StringUtils::ToUpper( upperCased );
    std::wstring searchFor;
    searchFor = string2wstring( upperCased );
    //
    const int attributeCount = sizeof( KConfigAttributes ) / sizeof( ConfigAttribute );
    const ConfigAttribute* ret = NULL;
    //
	for( int i=0; i<attributeCount; i++ )
		{
		if  ( KConfigAttributes[i].iName == searchFor )
			{
			ret = &KConfigAttributes[i];
			break;
			}
		}
    //
    return ret;
    }


bool ConfigManager::TryStringToNumeric( const std::string& aText, TUint32& aNumber )
    {
    bool convertedOk = false;
    //
    std::istringstream stringStream( aText );

    // Check for hex prefix
    if ( aText.length() > 2 && aText.substr( 0, 2 ) == KConfigAttributeHexPrefix )
        {
        // Skip 0x prefix
        stringStream.ignore( 2 );
        stringStream >> std::hex >> aNumber;
        convertedOk = ( !stringStream.fail() );
        }
    else
        {
        stringStream >> aNumber;
        convertedOk = ( !stringStream.fail() );
        }
    //
    return convertedOk;
    }


bool ConfigManager::TryStringToBool( const std::string& aText, TUint32& aNumber )
    {
    bool convertedOk = false;
    //
    std::string text = StringUtils::ToUpper( aText );
    if ( text == KConfigAttributeValueTrue )
        {
        aNumber = true;
        convertedOk = true;
        }
    else if ( text == KConfigAttributeValueFalse )
        {
        aNumber = false;
        convertedOk = true;
        }
    //
    return convertedOk;
    }


bool ConfigManager::TryStringToYesNo( const std::string& aText, TUint32& aNumber )
    {
    bool convertedOk = false;
    //
    std::string text = StringUtils::ToUpper( aText );
    if ( text == KConfigAttributeValueYes )
        {
        aNumber = true;
        convertedOk = true;
        }
    else if ( text == KConfigAttributeValueNo )
        {
        aNumber = false;
        convertedOk = true;
        }
    //
    return convertedOk;
    }


int ConfigManager::ConvertToDriveAttributes( const std::string& aString, DriveAttributes* aDrive)
    {
	std::string::const_iterator it = aString.begin();
	std::string::const_iterator end = aString.end();
	std::string::const_iterator currentPos = it;
	int driveLetter = 0;

	if (currentPos != end)
        {
		std::string temp;

        currentPos = std::find(it, end, ' ');

		if (currentPos == end)
			{
			return 0;
			}

		// Set the drive representation location
		aDrive->iDir = string2wstring(std::string(it, (currentPos-it)));
			
		temp =  StringUtils::TrimWhiteSpace( std::string(currentPos, end) );

		it = temp.begin();
		end = temp.end();

		// Look for the next 'space' if found
		currentPos = std::find(it, end, ' ');

		if (currentPos - it != 1)
			{
			// The drive letter is more than one character
			return 0;
			}
		
		// Set the target drive letter
		driveLetter = tolower(*it);

		if (currentPos == end)
			{
			// The external attribute is not set, i.e. leave it as the default value.
			return driveLetter;
			}

		temp =  StringUtils::TrimWhiteSpace( std::string(currentPos, end) );

		// Set the external attribute to true
		if (StringUtils::ToLower(temp) == "ext")
			aDrive->iExternal = true;
      
		return driveLetter;
		}

	return 0;
	}


std::wstring ConfigManager::GetLocalDrivePath(int aDrive)
{
	DrivesMap::const_iterator it = iDrives.find( aDrive );

	if (it == iDrives.end())
	{
		std::stringstream err;
		err << "Drive: " << aDrive << " not defined";
		throw ConfigManagerException( ConfigManagerException::ETypeDriveError, err.str());
	}

    return it->second->iDir;
}


bool ConfigManager::IsTargetDrivePresent(int aDrive) const
{
	DrivesMap::const_iterator it = iDrives.find( aDrive );

	return (it != iDrives.end());
}


bool ConfigManager::IsTargetDriveExt(int aDrive) const
{
	DrivesMap::const_iterator it = iDrives.find( aDrive );

	if (it == iDrives.end())
	{
		std::stringstream err;
		err << "Drive: " << aDrive << " not defined";
		throw ConfigManagerException( ConfigManagerException::ETypeDriveError, err.str());
	}


    return it->second->iExternal;
}


bool ConfigManager::AddDrive(DriveAttributes* aDrive, const int aDriveLetter)
{
	DrivesMap::const_iterator it = iDrives.find( aDriveLetter );

	if( it != iDrives.end())
	{
		// Drive already present
		return false;
	}

	// Set the map value
	iDrives[aDriveLetter] = aDrive;
	return true;
}


void ConfigManager::CheckAndAddDrive(const int aDrive, const std::wstring& aDir,const bool aExternal)
{
	DrivesMap::iterator it = iDrives.find( aDrive );

	if (it != iDrives.end())
	{
		char drive = aDrive;
	 	std::stringstream warn;
		warn << "Redefining drive: " << drive;
		std::wstring finalMessage = string2wstring( warn.str() );
		LWARN( finalMessage + L" to " + aDir);

		delete it->second;
		iDrives.erase(it);
	}

	DriveAttributes* newDrive = new DriveAttributes();

	newDrive->iDir = aDir;
	newDrive->iExternal = aExternal;
	
	// Set the map value
	iDrives[aDrive] = newDrive;
}


bool ConfigManager::AddDeviceSupportedLanguage(TInt langId)
	{
 	std::vector<int>::const_iterator end = iDeviceSupportedLanguages.end();
	for (std::vector<int>::const_iterator curr = iDeviceSupportedLanguages.begin(); curr != end; ++curr)
		{
			 if ( *curr == langId)
			 {
				 return false;
			 }
		}
	iDeviceSupportedLanguages.push_back(langId);
	return true;
	}
void ConfigManager::AddMatchingSupportedLanguages(TInt aMatchingSupportedLangauges)
	{
 	std::vector<int>::const_iterator end = iMatchingSupportedLanguages.end();
	for (std::vector<int>::const_iterator curr = iMatchingSupportedLanguages.begin(); curr != end; ++curr)
		{
			 if ( *curr == aMatchingSupportedLangauges)
			 {
				 return;
			 }
		}
	iMatchingSupportedLanguages.push_back(aMatchingSupportedLangauges);
	return;
	}

std::vector<TInt>& ConfigManager::GetMatchingSupportedLanguages() 
	{
		return iMatchingSupportedLanguages;
	}
ConfigManagerException::~ConfigManagerException()
    {
    }


void ConfigManagerException::Display() const
    {
	std::ostringstream stream;
    stream << "Configuration problem at line " << iLineNumber << " [" << iKey << " = " << iValue << "]: ";
    //
	switch( iType )
		{
		case ETypeKeywordNotSupported:
			stream << "KEY is not supported";
			break;
		case ETypeInvalidValueForKey:
			stream << "\'" << iValue << "\'" << " is an invalid type or out of range";
			break;
		case ETypeDuplicateDefinition:
			stream << "\'" << iKey << "\'" << "has already been defined previously in this configuration";
			break;
		case ETypeInvalidDirectory:
			stream << "\'" << iValue << "\'" << " directory is not found";
			break;
		case ETypeDriveError:
			LERROR( string2wstring( iValue ) );
			return;

		default:
			stream << "Unknown error";
			break;
		}
    //
    stream << std::endl;
    std::wstring finalMessage = string2wstring( stream.str() );
    //
	LERROR( finalMessage );
    }

