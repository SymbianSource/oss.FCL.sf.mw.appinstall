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


#include "launchprocess.h"
#include "options.h"
using namespace std;

LaunchProcess::LaunchProcess()
	:iSignSISExitCode(0),
	iDumpSwiCertstoreExitCode(0)
#ifdef DUMPINSTALLFILESTATUSTOOL
	,iDumpSISExitCode(0)
#endif
	{
	}

string LaunchProcess::ExecuteProcess(string aParameters, int aProcID)
	{
    string aFile;
    SECURITY_ATTRIBUTES secAttr; 
	ZeroMemory(&secAttr,sizeof(secAttr));
	secAttr.nLength = sizeof(secAttr);
	secAttr.bInheritHandle = TRUE;

	HANDLE readPipe, writePipe;
	CreatePipe(&readPipe,&writePipe,&secAttr,0);

	// Initialization parameters
    STARTUPINFO siStartupInfo;
    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);
	siStartupInfo.dwFlags = STARTF_USESTDHANDLES;
	siStartupInfo.hStdInput = NULL; 
	siStartupInfo.hStdOutput = writePipe; 
	siStartupInfo.hStdError = writePipe;
	char* parameter = const_cast<char*>(aParameters.c_str());
	if (CreateProcess(NULL,parameter , 0, 0, true,
                            NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 0, 0, &siStartupInfo,
                            &piProcessInfo) != false)
		{ 
		PrHandle[aProcID] = piProcessInfo.hProcess;
		CloseHandle(writePipe);

		DWORD reDword; 
		reDword = GetFileSize(readPipe,NULL);
			
		string consoleOutput;
		BOOL res;
		do
			{
			char rawBuf[BUFSIZE];
			res = ::ReadFile(readPipe,rawBuf,sizeof(rawBuf),&reDword,NULL);
			char* actualBuf = new char[reDword+1];
		
			memcpy(actualBuf,rawBuf,reDword);
			actualBuf[reDword] ='\0';
		
			if (res &&  reDword == 0) 
				{ 
				delete [] actualBuf;
				break;
				}
		
			consoleOutput.append(actualBuf);
			delete [] actualBuf;
			}while(res);

		if(aProcID == 0)
			{
			GetExitCodeProcess(piProcessInfo.hProcess,&iSignSISExitCode);
			if(iSignSISExitCode > 0)
				{
				cout << consoleOutput << endl;
				CloseHandle(readPipe);
				throw EErrorExecutingSupportingExeSignsis;
				}
		}

	else if(aProcID == 1)
		{
		GetExitCodeProcess(piProcessInfo.hProcess, &iDumpSwiCertstoreExitCode);
		if(iDumpSwiCertstoreExitCode > 0)
			{
			DisplayError(iDumpSwiCertstoreExitCode);
			CloseHandle(readPipe);
			throw EErrorExecutingSupportingDumpSwiCertstoreExe;
			}
		}

#ifdef DUMPINSTALLFILESTATUSTOOL
		if(aProcID == 2)
		{
		GetExitCodeProcess(piProcessInfo.hProcess,&iDumpSISExitCode);
		if(iDumpSISExitCode > 0)
			{
			cout << consoleOutput << endl;
			CloseHandle(readPipe);
			throw EErrorExecutingSupportingExeDumpsis;
			}
		}
#endif

		CloseHandle(readPipe);
		if(aProcID == 0)
			{
			return aFile = SignSISChainDirectoryName;
			}
		else 
			{
			return consoleOutput;
			}
		}

		else
			{
			// CreateProcess failed. 
			CloseHandle(writePipe);
			CloseHandle(readPipe);
			throw ESupportingExeError;
			}
	}


void LaunchProcess::DisplayError(const DWORD& err)
	{
	const char* msg;
	switch(err)
		{
		case 1:
			msg = "unknown option specified";
			break;
		case 2:
			msg = "Invalid file specified ";
			break;
		case 3:
			msg = "Cannot open file or file does'nt exist";
			break;
		case 4:
			msg = "DAT file specified is not a Permanent File Store";
			break;
		case 5:
			msg = "Invalid Certificate Chain";
			break;
		case 6:
			msg = "Not a C: Based Writable SwiCertstore Directory";
			break;
		case 7:
			msg = "No files in specified C: Based Writable Certstore Directory";
			break;
		default:
			msg = "unknown error";
			break;
		}
	cout << "DumpSWICertStoreTool Execution Error : "<< msg << endl << endl;
	}

LaunchProcess::~LaunchProcess()
	{
	}

