/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
 @internalComponent 
*/

#ifndef LAUNCHPROCESS_H
#define LAUNCHPROCESS_H

#define  BUFSIZE 2048
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>

const std::string SignSISChainDirectoryName = "Chain";
/**
 * The LaunchProcess provides methods which executes the supporting executables SignSIS.exe and DumpSWICertstoreTool.exe
 * @internalComponent 
 */

class LaunchProcess
{
public:

	LaunchProcess();

	~LaunchProcess();

	/** This function takes the command line input in order to execute the supporting executables
	   SignSIS.exe and DumpSWICertstoreTool.exe.
	   @param	aParameters		Command line input of the Supporting Executable.
	   @param	aProcID			Supporting executable index.
	  */
	std::string ExecuteProcess(std::string aParameters, int aProcID) ;

public:

	HANDLE PrHandle[2];
	PROCESS_INFORMATION piProcessInfo;
	DWORD GetSignSISExitCode() const { return iSignSISExitCode ;}
	DWORD GetDumpSwiCertstoreExitCode() const { return iDumpSwiCertstoreExitCode ;}

#ifdef DUMPINSTALLFILESTATUSTOOL
	DWORD GetDumpSISExitCode() const{ return iDumpSISExitCode ;}
#endif

private:

	/** Displys the reason for exit of DumpSWICertstoreTool.exe.
	   @param	aExitCode	Exitcode of DumpSWICertstoreTool.exe.
	  */
	void DisplayError(const DWORD& aExitCode);

private:

	DWORD iSignSISExitCode;
	DWORD iDumpSwiCertstoreExitCode;

#ifdef DUMPINSTALLFILESTATUSTOOL
	DWORD iDumpSISExitCode;
#endif
};

#endif//LAUNCHPROCESS_H
