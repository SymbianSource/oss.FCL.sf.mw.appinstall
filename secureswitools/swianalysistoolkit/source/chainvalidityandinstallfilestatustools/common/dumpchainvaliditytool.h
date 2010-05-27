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

#ifndef __DUMPCHAINVALIDITYTOOL_H__
#define __DUMPCHAINVALIDITYTOOL_H__
#pragma warning(disable: 4786)

#include "options.h"
#include "swicertstore.h"
#include "osinterface.h"
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <fstream>

const std::string CCIFile = "swicertstore.cci";
const std::string SignSISInputParameter = "signsis.exe -p ";
const std::string DumpSwiCertstoreInputParameter = "dumpswicertstoretool.exe -y ";

const int KLength = 256 ;

class SWICertStore;
class SISCertificateChain;

class DumpChainValidityTool
{
public:

	DumpChainValidityTool();

	~DumpChainValidityTool();

	/**Creates SWICertstore and SISCertificateChain objects and calls the function which executes
	   the supporting executables (SignSIS.exe and DumpSWICertstoreTool.exe).
	  */
	void Run(const Options& aOptions);

	/**Returns the unified list of capabilities granted by all the validated chains.
	   This is used by DumpInstallFileStatusTool.
	  */
	int GetUnifiedCaps() const {return iUnifiedCaps;}

private:

	/**Calls the ExecuteProcess() of the LaunchProcess Class ,which spawns a child process to execute
	   signsis.exe from which certificate chains are obtained(1 pem file per chain) and DumpSWICertstoretool.exe
	   from which certificate and capabilities information of the dat file are obtained which is then written
	   to a file(swicertstore.cci)
	   @param	aDirectory	Is populated with the directory name "Chain" containing all the 
							certificate chain (1 pem file per chain).
	  */
	void ExecuteSupportingExe(const Options& aOptions , std::string& aDirectory);

	/**Deletes the Chain directory along with the files it holds once the tool is executed.
	   @param	aDirectory	The chain directory to be deleted.
	  */
	void RemoveFiles(std::string& aDirectory);

private:
	
	int iUnifiedCaps;
	SWICertStore* iStore;
	SISCertificateChain* iChain;
};
#endif
