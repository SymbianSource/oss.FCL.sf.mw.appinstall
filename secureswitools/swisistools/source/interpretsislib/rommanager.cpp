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

// User includes
#include "rommanager.h"
#include "parameterlist.h"
#include "stringutils.h"
#include "is_utils.h"

// String constants
#ifndef __TOOLS2_LINUX__
const std::wstring KRomManagerSysBinPath 			= L"z:\\sys\\bin\\";
const std::string KRomManagerRomDrive               = "Z:\\";
#else
const std::wstring KRomManagerSysBinPath 			= L"z:/sys/bin/";
const std::string KRomManagerRomDrive               = "Z:/";
#endif
const std::string KRomManagerRomLogProcessingFile   = "Processing file ";
const std::string KRomManagerRomLogReadingResource  = "Reading resource ";
const std::string KRomManagerRomLogReadingResource2 = " to rom linear address";
const std::string KRomManagerRomLogUids             = "Uids:                    ";
const std::string KRomManagerRomLogSecureId         = "Secure ID:               ";
const std::string KRomManagerRomLogVendorId         = "Vendor ID:               ";
const std::string KRomManagerRomLogDeviceFileName   = "Device file name:        ";


RomEntry::RomEntry( const std::string& aSourceLogFile )
:   iSourceLogFile( aSourceLogFile ), iSecInfo( 0 ), iUid1( 0 ), iUid2( 0 ), iUid3( 0 )
    {
    iSecInfo = new SBinarySecurityInfo();
    iEnvFileName = "";
    iRomFileName = "";
    }


RomEntry::~RomEntry()
    {
    delete iSecInfo;
    }


RomManager::RomManager()
    {
    }

RomManager* RomManager::New( const CParameterList& aParamList )
    {
    RomManager* ret = NULL;
    
    //
    // Are we dealing with a full EPOC32-based environment, i.e. "-z" on the Command Line
    // or are we working with ROM/ROFSBUILD log files ("-r").
    //	
    if ( aParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet))
		{
	    ret = new RomManagerEmpty();
		}
	else if ( aParamList.RomLogFileNames().size() )
        {
        ret = new RomManagerLogFiles( aParamList.RomLogFileNames() );
        }
    else
        {
        ret = new RomManagerFileSystem( aParamList.RomDrivePath() );
        }
    //
    return ret;
    }


RomManagerFileSystem::RomManagerFileSystem( const std::wstring& aPath )
:   iRomPath( aPath )
    {
    }

bool RomManagerFileSystem::RomFileExists( const std::wstring& aFileName )
    {
    std::wstring target = std::wstring( aFileName );
    ConvertToLocalPath( target, iRomPath );
    bool exists = FileExists( target );
    return exists;
    }

void RomManagerFileSystem::FindAllAdornedVariants(const std::wstring& aSearchNameWild, std::list<std::wstring>& aAdornedFileNamesFound)
	{
	// do nothing
	}

bool RomManagerFileSystem::SidExistsInRom(std::wstring& aFile, TUint32 aSid)
	{
	std::wstring romDir(KRomManagerSysBinPath);
    ConvertToLocalPath( romDir, iRomPath );

	// search through the binaries on the ROM drive
	// to find a matching SID
	std::list<std::wstring> dirContents;
	GetDirContents(romDir, dirContents);

	std::list<std::wstring>::iterator curr = dirContents.begin();
	std::list<std::wstring>::iterator end = dirContents.end();
	for ( ; curr != end; ++curr )
		{
			std::wstring romFile(KRomManagerSysBinPath);
			romFile.append(*curr);

			SBinarySecurityInfo info;
			ReadSecurityInfo(info,romFile);

			TUint32 sid = info.iSecureId;

			if (sid != 0 && sid == aSid)
			{
				aFile = KRomManagerSysBinPath + romFile;
				return true;
			}
		}
	return false;
	}

TInt RomManagerFileSystem::ReadSecurityInfo( SBinarySecurityInfo& aInfo, const std::wstring aFileName )
    {
	// Need to convert the file name to the local path so it can find it
	std::wstring target = std::wstring( aFileName );
	ConvertToLocalPath( target, iRomPath );
    return ::ReadSecurityInfo(aInfo,target);
    }

RomManagerLogFiles::RomManagerLogFiles( const std::list<std::wstring>& aLogFileNames )
:   iLogFileNames( aLogFileNames )
    {
    for( std::list<std::wstring>::const_iterator it = aLogFileNames.begin() ; it != aLogFileNames.end(); it++ )
        {
    	std::string narrowLogFileName = wstring2string( *it );
    	std::string narrowUpperCaseLogFileName = StringUtils::ToUpper( narrowLogFileName );

        // Get the base name
        int dotPos = narrowUpperCaseLogFileName.find( ".LOG" );
        if ( dotPos == std::string::npos )
            {
            }
        else
            {
            const std::string baseName = narrowLogFileName.substr( 0, dotPos );

            // Two step process. First, read the log, then, if the log file didn't
            // contain sufficient meta data (old version of ROFS/ROMBUILD) we resort
            // to reading a corresponding OBY too (if it exists).
            const bool needToReadOby = ReadLogFile( narrowLogFileName );

            if ( needToReadOby )
                {           
                ReadObyFile( baseName + ".OBY" );
                }
            }
        }
    }


RomManagerLogFiles::~RomManagerLogFiles()
    {
    for( RomEntryMap::iterator it=iEntriesIndexedByEnvFileName.begin(); it != iEntriesIndexedByEnvFileName.end(); it++ )
        {
        RomEntry* entry = (*it).second;
		delete entry;
        }
    }


bool RomManagerLogFiles::RomFileExists( const std::wstring& aFileName )
    {
    bool exists = false;

    // Get filename & convert it to uppercase, since our map key is
    // also in upper case form.
    std::string narrowFileName = wstring2string( aFileName );
    narrowFileName = StringUtils::ToUpper( narrowFileName );

    // Do we have a corresponding entry?
    RomEntry* entry = iEntriesIndexedByRomFileName[ narrowFileName ];
    if ( entry != NULL )
        {
        exists = true;
        }
    //
    return exists;
    }

void RomManagerLogFiles::FindAllAdornedVariants(const std::wstring& aSearchNameWild, std::list<std::wstring>& aAdornedFileNamesFound)
	{
	RomEntryMap::const_iterator curr = iEntriesIndexedByRomFileName.begin();
	RomEntryMap::const_iterator end = iEntriesIndexedByRomFileName.end();

	std::wstring searchNameWild = StringUtils::ToUpper(aSearchNameWild);

	for ( ; curr != end ; ++curr)
		{
		std::wstring romFile = string2wstring(curr->first);
		if (StringUtils::WildcardCompare(searchNameWild,romFile))
			{
			aAdornedFileNamesFound.push_back(romFile);
			}
		}
	}

bool RomManagerLogFiles::SidExistsInRom(std::wstring& aFile, TUint32 aSid)
	{
	RomEntryMap::const_iterator curr = iEntriesIndexedByRomFileName.begin();
	RomEntryMap::const_iterator end = iEntriesIndexedByRomFileName.end();

	for ( ; curr != end ; ++curr)
		{
		RomEntry* romEntry = curr->second;
		if (romEntry)
			{
			if (romEntry->Type() == RomEntry::ETypeBinary)
				{
				TUint32 romSid = romEntry->SecurityInfo().iSecureId;
				if (romSid != 0 && romSid == aSid)
					{
					aFile = string2wstring(romEntry->RomFileName());
					return true;
					}
				}
			}
		}
	return false;
	}

TInt RomManagerLogFiles::ReadSecurityInfo( SBinarySecurityInfo& aInfo, const std::wstring aFileName )
    {
    TInt err = -1;

    // Get filename & convert it to uppercase, since our map key is
    // also in upper case form.
    std::string narrowFileName = wstring2string( aFileName );
    narrowFileName = StringUtils::ToUpper( narrowFileName );

    // Do we have a corresponding entry?
    RomEntry* entry = iEntriesIndexedByRomFileName[ narrowFileName ];
    if ( entry != NULL )
        {
		if ( entry->Type() == RomEntry::ETypeBinary)
			{
			aInfo.iSecureId = entry->SecurityInfo().iSecureId;
			aInfo.iVendorId = entry->SecurityInfo().iVendorId;
			err = 0;
			}		
        }
    //
    return err;
    }


void RomManagerLogFiles::ReadObyFile( const std::string& aFileName )
    {
	std::wstring fileName = string2wstring( aFileName );
    if ( FileExists( fileName ) )
        {
    	std::ifstream stream;
	    stream.open( aFileName.c_str(), std::ios::binary );
        //
        if ( !stream.fail() )
            {
            try
                {
                std::string line;
                //
                while( std::getline( stream, line ) )
                    {
                    line = StringUtils::TrimWhiteSpace( line );
                    if ( line.length() )
                        {
                        ProcessObyFileLine( aFileName, line );
                        }
                    }
                //
                stream.close();
                }
            catch( const RomManagerException& e )
                {
                stream.close();
                throw e;
                }
            }
        else
            {
            throw RomManagerException( RomManagerException::ETypeUnableToOpenCorrespondingOBY, aFileName );
            }
        }
    else
        {
        throw RomManagerException( RomManagerException::ETypeUnableToFindCorrespondingOBY, aFileName );
        }
    }


void RomManagerLogFiles::ProcessObyFileLine( const std::string& aFileName, std::string& aLine )
    {
    int breakPos = std::string::npos;
    RomEntry::TType type = RomEntry::ETypeBinary;
    //
    if ( StringUtils::CheckForMatch( "file=", aLine ) )
        {
        breakPos = aLine.find_first_of( "\"" );
        type = RomEntry::ETypeBinary;
        }
    else if ( StringUtils::CheckForMatch( "data=", aLine ) )
        {
        breakPos = aLine.find_first_of( "\"" );
        type = RomEntry::ETypeData;
        }
    else if ( StringUtils::CheckForMatch( "extension[", aLine ) )
        {
        // extension[0x0B080004]=\epoc32\release\ARMV5\urel\esound.ldd 	"Sys\Bin\esound.ldd"
        const std::string KRemainingExtensionDataText = "0xXXXXXXXX]=";

        if ( aLine.length() > KRemainingExtensionDataText.length() )
            {
            aLine = aLine.substr( KRemainingExtensionDataText.length() );
            breakPos = aLine.find_first_of( "\"" );
            }
        }
    else if ( StringUtils::CheckForMatch( "primary[", aLine ) )
        {
        // primary[0x0A080004]=\epoc32\release\ARMV5\urel\_ekern.exe 	"Sys\Bin\ekern.exe"
        const std::string KRemainingPrimaryDataText = "0xXXXXXXXX]=";

        if ( aLine.length() > KRemainingPrimaryDataText.length() )
            {
            aLine = aLine.substr( KRemainingPrimaryDataText.length() );
            breakPos = aLine.find_first_of( "\"" );
            }
        }
    else if ( StringUtils::CheckForMatch( "device[", aLine ) )
        {
        // device[0x0C080004]=\epoc32\release\ARMV5\urel\esound.ldd 	"Sys\Bin\esound.ldd"
        const std::string KRemainingDeviceDataText = "0xXXXXXXXX]=";

        if ( aLine.length() > KRemainingDeviceDataText.length() )
            {
            aLine = aLine.substr( KRemainingDeviceDataText.length() );
            breakPos = aLine.find_first_of( "\"" );
            }
        }

    if ( breakPos != std::string::npos )
        {
        // The env file name must be upper case, since our string indicies
        // require this.
    	std::string envFileName = StringUtils::TrimWhiteSpace( aLine.substr( 0, breakPos ) );
        envFileName = StringUtils::ToUpper( envFileName );

        std::string romFileName = StringUtils::TrimWhiteSpace( aLine.substr( breakPos ) );
        romFileName = KRomManagerRomDrive + romFileName.substr( 1, romFileName.size() - 2 );

        // See if we can locate the ROM entry based upon the environment
        // file name. This should agree with the pre-parsed ROMBUILD/ROFSBUILD
        // log file that we read prior to this method.
        RomEntry* file = iEntriesIndexedByEnvFileName[ envFileName ];

        // Update the entry with the (presumed missing) 'in-place' Symbian OS
        // file name.
        if ( file != NULL )
            {
            romFileName = StringUtils::ToUpper( romFileName );
            file->SetRomFileName( romFileName );

            // Update type info
            file->SetType( type );

            // ... and make an index entry in the index-by-rom-filename-map
            iEntriesIndexedByRomFileName[ romFileName ] = file;
            }
        }
    }


bool RomManagerLogFiles::ReadLogFile( const std::string& aFileName )
    {
    bool obyParsingRequired = true;
    std::ifstream stream;
	stream.open( aFileName.c_str(), std::ios::binary );

    if ( !stream.fail() )
        {
        try
            {
            std::string line;
            RomEntry* currentEntry = NULL;
            //
            while( std::getline( stream, line ) )
                {
                line = StringUtils::TrimWhiteSpace( line );
                if ( line.length() )
                    {
                    ProcessLogFileLine( line, currentEntry, aFileName, obyParsingRequired );
                    }
                }
            //
            stream.close();
            }
        catch( const RomManagerException& e )
            {
            stream.close();
            throw e;
            }
        }
    else
        {
        throw RomManagerException( RomManagerException::ETypeUnableToOpenLogFile, aFileName );
        }
    //
    return obyParsingRequired;
    }


void RomManagerLogFiles::ProcessLogFileLine( std::string& aLine, RomEntry*& aCurrentEntry, const std::string& aFileName, bool& aObyParsingRequired )
    {
    const bool isBinary = StringUtils::CheckForMatch( KRomManagerRomLogProcessingFile, aLine );
    const bool isData = StringUtils::CheckForMatch( KRomManagerRomLogReadingResource, aLine );
    //
    if ( isBinary || isData )
        {
        // Save any existing entry
        if ( aCurrentEntry != NULL )
            {
            // Env file name
            iEntriesIndexedByEnvFileName[ aCurrentEntry->EnvFileName() ] = aCurrentEntry;

            // Rom file name (if set)
            if ( aCurrentEntry->RomFileName().length() )
                {
                iEntriesIndexedByRomFileName[ aCurrentEntry->RomFileName() ] = aCurrentEntry;
                }
            }

        // If this is a data entry, then we must remove everything after the "to rom linear address"
        // text, since this is not part of the filename.
        if ( isData )
            {
            const std::string::size_type linAddrTextPos = aLine.find( KRomManagerRomLogReadingResource2 );
            if ( linAddrTextPos != std::string::npos )
                {
                aLine = aLine.substr( 0, linAddrTextPos );
                }
            }

        // Indicies are upper case file names.
        aLine = StringUtils::ToUpper( aLine );

        // Create a new entry
        aCurrentEntry = new RomEntry( aFileName );
        aCurrentEntry->SetEnvFileName( aLine );

        // Set type depending on whether its data or binary
        if ( isData )
            {
            aCurrentEntry->SetType( RomEntry::ETypeData );
            }
        else if ( isBinary )
            {
            aCurrentEntry->SetType( RomEntry::ETypeBinary );
            }
        }
    else if ( aCurrentEntry && StringUtils::CheckForMatch( KRomManagerRomLogUids, aLine ) )
        {
    	std::istringstream stringStream( aLine );
        //
        TUint32 uid1 = 0;
        stringStream >> std::hex >> uid1;
        aCurrentEntry->SetUid1( uid1 );
        //
        TUint32 uid2 = 0;
        stringStream >> std::hex >> uid2;
        aCurrentEntry->SetUid2( uid2 );
        //
        TUint32 uid3 = 0;
        stringStream >> std::hex >> uid3;
        aCurrentEntry->SetUid3( uid3 );
        }
    else if ( aCurrentEntry && StringUtils::CheckForMatch( KRomManagerRomLogSecureId, aLine ) )
        {
    	std::istringstream stringStream( aLine );
        //
        TUint32 sid = 0;
        stringStream >> std::hex >> sid;
        aCurrentEntry->SecurityInfo().iSecureId = sid;
        }
    else if ( aCurrentEntry && StringUtils::CheckForMatch( KRomManagerRomLogVendorId, aLine ) )
        {
    	std::istringstream stringStream( aLine );
        //
        TUint32 vid = 0;
        stringStream >> std::hex >> vid;
        aCurrentEntry->SecurityInfo().iVendorId = vid;
        }
    else if ( aCurrentEntry && StringUtils::CheckForMatch( KRomManagerRomLogDeviceFileName, aLine ) )
        {
        // Indicies are upper case file names.
        aLine = StringUtils::ToUpper( KRomManagerRomDrive + aLine );
        aCurrentEntry->SetRomFileName( aLine );

        // If we see this marker, then ROMBUILD & ROFSBUILD are supplied with the extended
        // meta data that includes "in-device" file name, hence we don't need to parse
        // the OBY file in order to obtain this information
        aObyParsingRequired = false;
        }
    }


bool RomManagerEmpty::RomFileExists( const std::wstring& aFileName )
	{
	//default return value as there is no Z drive present
	return false;
	}

TInt RomManagerEmpty::ReadSecurityInfo( SBinarySecurityInfo& aInfo, const std::wstring aFileName )
	{
	// return non zero error value  
	return -1;
	}

void RomManagerEmpty::FindAllAdornedVariants(const std::wstring& aSearchNameWild, std::list<std::wstring>& aAdornedFileNamesFound)
	{
	// default return value as there is no Z drive present
	}

bool RomManagerEmpty::SidExistsInRom(std::wstring& aFile, TUint32 aSid)
	{
	// default return value as there is no Z drive present
	return false;
	}

void RomManagerException::Display() const
    {
	std::ostringstream stream;
    stream << "ROM/ROFS OBY/LOG file problem - ";
    //
	switch( iType )
	{
	case ETypeUnableToOpenLogFile:
		stream << "\'" << iFileName << "\'" << " (ROM/ROFSBUILD LOG file) could not be read";
		break;
	case ETypeUnableToOpenCorrespondingOBY:
		stream << "\'" << iFileName << "\'" << " (ROM/ROFSBUILD OBY file) could not be read";
		break;
	case ETypeUnableToFindCorrespondingOBY:
		stream << "\'" << iFileName << "\'" << " (ROM/ROFSBUILD OBY file) could not be found";
		break;
	
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
