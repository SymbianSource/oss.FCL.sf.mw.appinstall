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


#include "dumpchainvaliditytool.h"
#include "osinterface.h"
using namespace std;

// Class DumpChainValidityTool
DumpChainValidityTool::DumpChainValidityTool()
	:iUnifiedCaps (0) ,
	iStore (0),
	iChain (0)
	{
	}


void DumpChainValidityTool::Run(const Options& aOptions)
	{
	string dumpSwiCertstoreData ;
	string directory ;
	ExecuteSupportingExe(aOptions , directory);
	iStore = new SWICertStore();
	iChain = new SISCertificateChain();
	iStore->Run(aOptions);
	int isSISSigned = iChain->Run(aOptions ,*iStore ,directory);
	if(!isSISSigned)
		{
		remove(CCIFile.c_str());
		return;
		}

	//if and only if any root certs validate the chain ,will this function be called
	if(iChain->GetValidatedRootSignature().size() > 0 || iChain->GetDevCertValidatedRootSignature().size() > 0)
		{
		iStore->UnifyCapabilitiesList(*iChain);
		}

	//Checks whether all the mandatory rootcerts in swicertstore have been used for signing sis file.
	//if no then installation may fail,hence provides info the user in case installation fails,even though 
	//the sis file is signed by a rootcert.
	iChain->VerifyMandatory(*iStore);

#ifdef DUMPCHAINVALIDITYTOOL
	if(aOptions.List())
		{
		cout<<"Unified Capabilities List : "<<endl<<endl;
		iStore->DisplayUnifiedCapabilities(*iChain);
		}
#endif

	//required by dumpinstallfilestatus.
#ifdef DUMPINSTALLFILESTATUSTOOL
	iUnifiedCaps = iStore->GetUnifiedCapabilities();
#endif
	
	RemoveFiles(directory);
	int success = OSInterface::DeleteDirectory(directory);
	if(!success)
		{
		cout << "Directory could not be deleted." << endl;
		}
	remove(CCIFile.c_str());
	}


void DumpChainValidityTool::ExecuteSupportingExe(const Options& aOptions , string& directory )
	{
	string dumpSwiCertstoreData ;
	string inputParameter ;
	ofstream fileName ;
	LaunchProcess launch ;

	inputParameter = SignSISInputParameter;
	inputParameter.append(aOptions.SISFile());
	directory = launch.ExecuteProcess(inputParameter , 0);

	inputParameter = DumpSwiCertstoreInputParameter;
	inputParameter.append(aOptions.CertstoreFile());
	dumpSwiCertstoreData = launch.ExecuteProcess(inputParameter  ,1);
	inputParameter.empty();
		
	if(WAIT_FAILED != WaitForMultipleObjects(2,launch.PrHandle,true,10))
		{
		if(launch.GetDumpSwiCertstoreExitCode() == 0 && launch.GetSignSISExitCode() == 0)
				{
				fileName.open("swicertstore.cci",ios::out);
				fileName.write(dumpSwiCertstoreData.c_str(),dumpSwiCertstoreData.length());
				fileName.close();
				}
		}

	else
		{
#ifdef DUMPCHAINVALIDITYTOOL
		cout << "Wait Error "<< GetLastError() << endl ; 
#endif
		ExitProcess(0); 
		}

	CloseHandle(launch.piProcessInfo.hProcess);
	CloseHandle(launch.piProcessInfo.hThread);
	}


void DumpChainValidityTool::RemoveFiles(string& aDirectorypath)
	{
	int certNumber = 1;
	char chainIndex[4];
	while(1)
		{
		string dirPath;
		dirPath = aDirectorypath;
		itoa(certNumber,chainIndex,10);
		dirPath.append("\\cert");
		dirPath.append(chainIndex) ;
		dirPath.append(".pem");
		if(remove(dirPath.c_str()) == -1)
			{
			return;
			}

		certNumber++;
		}
	}


DumpChainValidityTool::~DumpChainValidityTool()
	{
	delete iChain;
	delete iStore;
	}


