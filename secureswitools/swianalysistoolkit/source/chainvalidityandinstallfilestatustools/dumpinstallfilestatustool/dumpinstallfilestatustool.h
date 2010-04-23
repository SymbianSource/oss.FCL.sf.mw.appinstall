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
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __DUMPINSTALLFILESTATUSTOOL_H__
#define __DUMPINSTALLFILESTATUSTOOL_H__
#pragma warning(disable: 4786)

#include "options.h"
#include "launchprocess.h"
#include "dumpchainvaliditytool.h"
#include "siscertificatechain.h"
#include "certificatechain.h"
#include "swicertstore.h"
#include "sisfiledata.h"
#include "osinterface.h"

#include <algorithm>
#include <map>

const std::string KDumpsisInputParameter = "dumpsis.exe -y -l ";

class DumpChainValidityTool;
class SISFileData;

class DumpInstallFileStatusTool
{
public:

	DumpInstallFileStatusTool();

	~DumpInstallFileStatusTool();

	/** 
	   Creates DumpChainValidityTool object and extracts the unified capabilities(union of capabilities 
	   of the root certificates used for validating the sis file obtained from DumpChainValidity).Calls 
	   the ExecuteProcess() of the LaunchProcess class ,which spawns a child process to execute dumpsis.exe 
	   from which executable and their corressponing capabilities information are obtained which is then 
	   written to a file(execapabilities.eci).
	  */
	void Run(const Options& aOptions);

private:

	/**
	  Correlates each executable capabilities with (1)Signing Certificates Capabilities.
	  (2)Signing Certificates Capabilities in conjunction with User Specified Capabilities.
	  Shows the file status as to whether each executable capabilities are satisified by either 
	  (1) or (2) or not satisfied.If not satisfied ,displays the missing capabilities
	  @param	aExeCapabilities			Executable Capabilities (obtained from ECIFileRead()).
	  @param	aMergedCapabilities			Union of capabilities list(obtained from dumpchainvaliditytool 
	 										and user specified capabilities on cmd line)
	  @param	aUnifiedSigningCertCaps		Capability List(from dumpchainvaliditytool)
	  @param	aExeLabel					Executable Name(obtained from ECIFileRead()).
	 */
	void Correlate(const int aExeCapabilities ,int aMergedCapabilities ,int aUnifiedSigningCertCaps ,const std::string& aExeLabel);

	/**
	   Displays those executable capabilities which are neither satisified by signing chains nor with 
	   signing chains in conjunction with user specified capabilities.
	   @param  aMissingCapabilities		Missing capabilitites as obtained from Correlate() function.
	  */
	void DisplayMissingCapabilities(int aMissingCapabilities);
	
private:

	SISFileData *iSISData;
	DumpChainValidityTool *iChainValidity;
};

#endif

