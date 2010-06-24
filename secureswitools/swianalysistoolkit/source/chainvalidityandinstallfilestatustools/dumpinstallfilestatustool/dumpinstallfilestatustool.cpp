/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <memory>
#include "dumpinstallfilestatustool.h"
#include "osinterface.h"
#ifdef _MSC_VER
#include <new>
#include <new.h>
#endif /* _MSC_VER */
using namespace std;

#ifdef _MSC_VER
// House-keeping to allow ::new to throw rather than return NULL in MSVC++
int throwingHandler(size_t)
{
	throw std::bad_alloc();
	return 0;
}
#endif /* _MSC_VER */


// entry point
int main(int argc,char *argv[])
	{
#ifdef _MSC_VER
	_set_new_handler (throwingHandler);	// Make ::new throw rather than return NULL
#endif /* _MSC_VER */

	if(argc < 2)
		{
		Options::DisplayUsage();
		return 0;
		}

	OpenSSL_add_all_algorithms ();
	OpenSSL_add_all_ciphers ();
	OpenSSL_add_all_digests ();

	try
		{
		auto_ptr<DumpInstallFileStatusTool> tool(new DumpInstallFileStatusTool);
		auto_ptr<Options> options(new Options(argc,argv));
		tool->Run(*options);
		}
	
	catch (Exceptions& aErr)
		{
		Options::DisplayError (aErr);
		return aErr;
		}
	
	catch (bad_alloc&)
		{
		cout << "Error Allocating Memory " << endl ;
		}

	return 0;
	}

// Class DumpInstallFileStatusTool
DumpInstallFileStatusTool::DumpInstallFileStatusTool()
	:iSISData(0) ,
	iChainValidity(0)
	{
	}

void DumpInstallFileStatusTool::Run(const Options& aOptions)
	{
	int dumpchainvalidityData = 0 ;
	LaunchProcess launch;
	ofstream fileName;

	iChainValidity = new DumpChainValidityTool();
	iChainValidity -> Run(aOptions);
	dumpchainvalidityData = iChainValidity -> GetUnifiedCaps();

	std::string inputParameter = KDumpsisInputParameter;
	inputParameter.append(aOptions.SISFile());
	string dumpsisData = launch.ExecuteProcess(inputParameter,2);
	
	fileName.open(KEciFile.c_str(),ios::out);
	fileName.write(dumpsisData.c_str(),dumpsisData.length());
	fileName.close();
	

	CloseHandle(launch.piProcessInfo.hProcess);
	CloseHandle(launch.piProcessInfo.hThread);

	iSISData = new SISFileData();
	int isExe = iSISData->EciFileRead(KEciFile.c_str(),aOptions);
	//in case sis file doesn't contain any executable.
	if(!isExe)
		{
		return;
		}

	int mergedCaps = (aOptions.UserCapabilities())|(dumpchainvalidityData);
	const SISFileData::ExeLabelAndCapabilitiesList* exeList = iSISData->ExeNameAndCapabilityList();
	while(exeList)
		{
		Correlate(exeList -> iExeCapability, mergedCaps, dumpchainvalidityData, exeList -> iExeLabel);
		exeList = exeList -> iNext;
		}
	remove(KEciFile.c_str());	
	}

void DumpInstallFileStatusTool::Correlate(const int aExeCapabilities , int MergedCapabilities , int aUnifiedSigningCertCaps , const string& aExeLabel)
	{
	int missingCaps;
	int loc = aExeLabel.find(".exe" , 0);
	int pos = aExeLabel.find(".dll" , 0);
	//correlate only if the file is an exe or dll.
	if(loc > 0 || pos > 0 )
		{
		cout << "Executable : " << endl << endl;
		//when exe has no capabilities.
		if(aExeCapabilities == 0)
			{
			cout << aExeLabel << endl << endl;
			cout << "FILE STATUS : Has No Capabilities ." << endl << endl;
			}
	

		//check whether exe capabilities are satisified by signing cert caps.In case unified caps(from chainvalidity) is 0, do not consider them.
		else if(((aExeCapabilities & aUnifiedSigningCertCaps) == aExeCapabilities) && aUnifiedSigningCertCaps )
			{
			cout<< aExeLabel << endl << endl;
			cout<<"FILE STATUS : Capabilities are satisfied by Signing Certificates ." << endl << endl;
			}
		
		//checking whether exe caps are satisfied by signing chain 
		//in conjunction with user specified capabilities(MergedCapabilities)
		else if((aExeCapabilities & MergedCapabilities) == (aExeCapabilities))
			{
			cout << aExeLabel << endl << endl;
			cout <<"FILE STATUS : Capabilities are satisfied by Signing Certificates ." << endl;
			cout<<"\t\t in conjunction with User Capabilities" << endl << endl;
			}

		else
			{
			cout << aExeLabel << endl << endl;
			cout <<"FILE STATUS : Capabilities are not satisfied ." << endl << endl;
			cout <<"Missing Capabilities List :" << endl << endl;
			int result = ((MergedCapabilities) & (aExeCapabilities));
			missingCaps = (aExeCapabilities - result);
			DisplayMissingCapabilities(missingCaps);
			}
		}
	}

void DumpInstallFileStatusTool::DisplayMissingCapabilities(int aMissingCaps)
	{
	int missingCapabilities = 0;
	while(aMissingCaps)
		{
		if((aMissingCaps & 0x01))
			{
			if(missingCapabilities < KNumberOfCaps)
				{
				cout << CapabilityList[missingCapabilities] << endl;;
				}
			else
				{
				cout << "Unknown" << missingCapabilities << endl;
				}
			}

	aMissingCaps = aMissingCaps >> 1;
	missingCapabilities++;
		}

	cout << endl;
	}

DumpInstallFileStatusTool::~DumpInstallFileStatusTool()
	{
	delete iSISData;
	delete iChainValidity;
	}




