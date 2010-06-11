/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the command line options
*
*/


/**
 @file commandparser.h
 @internalComponent
 @released
*/

#ifndef	__COMMAND_PARSER_H__
#define	__COMMAND_PARSER_H__

#pragma warning (disable: 4786)

// System includes
#include <istream>
#include <list>
#include <vector>
#include <map>
#include <string>

// User includes
#include "logger.h"
#include "parameterlist.h"

// Forward declaration
class CParameterList;

class CCommandParser
	{
public: // Type definitions
	typedef std::list<std::wstring> OptionList;
	typedef std::list<InstallSISFile> SISFileList;
	typedef void (CParameterList::*FilePtr)(std::wstring& aString);

public: // Enumerations
    enum CmdLineException
	    {
	    ECmdLineUnknownOption = 0,
	    ECmdLineNoDirArgument,
	    ECmdLineMissingParamFile,
	    ECmdLineNoSisArgument,
        ECmdLineNoRomRofsLogsArgument,
        ECmdLineNoConfigFileArgument,
        ECmdLineNoSisStubPathArgument,
	    ECmdLineMissingParams,
        ECmdLineInvalidLanguage,
		ECmdLineMissingPackageUID,
		ECmdLineInvalidSISFileAttribute
	    };

public: // Constructors & destructor
	CCommandParser();
	~CCommandParser();
	CParameterList* ParseOptions(int argc, const char**argv);

public: // Helper functions
	static void DisplayError(int err);
	static void DisplayUsage();
	static void DisplayVersion();
	static void DisplayOpenSSLCopyright();

public: // Accessors
	inline WarnLevel WarningLevel() const { return iWarnLevel; }
	inline const std::wstring& LogFile() const { return iLogFile; }

	inline static const std::string CommandName() { return "interpretsis"; }

private:
	bool ParseParam(int argc, const char** argv, CParameterList* aParamList);
	void ParseParam(int argc, const std::string& paramFile, CParameterList* aParamList);
	void SetSisRegistryVersion(const std::string& aSisRegVersion);

private:
	void String2SISFileList(CParameterList& aParamList, int aArgc, const char**argv);
    static void String2List(CParameterList& aParamList, FilePtr& aFilePtr, const std::string& aOptionsString);
	static WarnLevel String2Warn(const std::string& level);
    static int String2Language( const std::string& aLanguage );

private:
	typedef std::map<std::string, WarnLevel> WarnMap;

	WarnLevel               iWarnLevel;
	std::wstring            iLogFile;
	static WarnMap          KString2Warning;
	};


bool FileExists(const std::wstring& aFile);
bool RemoveFile(const std::wstring& aFile);
bool CreateFile(const std::wstring& aFile);

#endif	//__COMMAND_PARSER_H__
