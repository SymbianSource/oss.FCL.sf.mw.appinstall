/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the entry point for the interpretsis program
*
*/


#pragma warning (disable: 4786)

// System include
#include <iostream>
#include <fstream>

// User include
#include "commandparser.h"
#include "interpretsis.h"
#include "logger.h"
#include "../common/exception.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "dirparse.h"
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK


#ifndef _WIN32
#include <ctype.h>

//__ctype_b was removed from glibc. This is a workaround to fix the linking problem
extern "C"
    {
    const unsigned short int** __ctype_b()
        {
        return __ctype_b_loc();
        }
    }

#endif // _WIN32


int main(int argc, const char* argv[])
	{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	std::string dbpath;
#endif	
	bool pauseWhenDone = false;

	int result= SUCCESS;
	std::wofstream* logFile = NULL;
	try
		{
		CCommandParser options;
		CInterpretSIS::TParamPtr paramList(options.ParseOptions(argc, argv));
		
		CParameterList* paramPtr = paramList.get(); 
		if(NULL == paramPtr)
			{
			return 0;
			}

		if (options.LogFile().size() > 0)
			{
			logFile = new std::wofstream(wstring2string(options.LogFile()).c_str(), std::ios::app);
			Logger::SetStream(*logFile);
			}
		else
			{
			Logger::SetStream(std::wcout);
			}

		Logger::SetLevel( options.WarningLevel() );

		CInterpretSIS interpretSis(paramList);
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		//Parsing Preprovisioned Resource file in not done if RegistryVersionExists
		if(!paramPtr->RegistryVersionExists())
		{
			if ( paramPtr->IsFlagSet(CParameterList::EFlagsResourceFilePathSet)) 
			{
				ParseResourceDir(paramPtr, interpretSis);

				if (NULL != logFile)
				{
					bool val = logFile->is_open();
					logFile->close();
					delete logFile;
				}
				return result;
			}
			else
			{
				ParseResourceDir(paramPtr, interpretSis);
			}
		}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
	
		result = interpretSis.Install();

		// Uninstall the sis files
		interpretSis.Uninstall();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
		if(result != 0)
			dbpath=GetDbPath(paramPtr);
#endif
		}
    catch( CCommandParser::CmdLineException err )
		{
		CCommandParser::DisplayError(err);
		result = CMDLINE_ERROR;
		}
    catch(CParameterList::TParamException err)
    	{
    	CParameterList::DisplayError(err);
    	CCommandParser::DisplayUsage();
    	result = CMDLINE_ERROR;
    	}
    catch(const ConfigManagerException& e)
    	{
		LERROR(L"Config Manager Error - ");
		e.Display();
		result = CONFIG_ERROR;
    	}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	catch(const CResourceFileException& aObject)
		{
		LERROR(L"Resource File Parsing Error - ");
		aObject.Display();
		result = RSC_PARSING_ERROR;
		}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
    catch(const RomManagerException& e)
    	{
		LERROR(L"ROM Manager Error - ");
		e.Display();
		result = ROM_MANAGER;
    	}
	catch  (InterpretSisError& e)
		{
		LERROR(L"\t" << string2wstring(e.what()));
		result =  e.GetErrorCode();
		}
	catch (CSISException e)
		{
		LERROR(L"FileContents Error - ");
		std::cout << e.what() << std::endl;
		result = INVALID_SIS;
		}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK				
	catch (CException& e)
		{
		LERROR(L"DB Manager Error - ");
		std::cout << "DB Manager Error - " << e.GetMessageA() << " Code: " << e.GetCode() << std::endl;
		result = DB_EXCEPTION;
		}
#endif
	catch (std::exception &err)
		{
		std::wstring emessage = string2wstring( err.what() );
		LERROR( L"Error: " << emessage);
		result = STD_EXCEPTION;
		}
	catch (...)
		{
		result = UNKNOWN_EXCEPTION;
		LERROR(L"Unknown Error" << std::endl);
		}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	//Restore Database in case of installation error for SA over SA and PU case
	if(result != 0)
	{
		std::string BackupDb(dbpath);
		BackupDb.append("_backup");
		if (FileExists(string2wstring(BackupDb)))
		{	
			#ifdef __TOOLS2_LINUX__
				std::string command = "mv " + BackupDb + " " + dbpath;
			#else
				std::string command = "move " + BackupDb + " " + dbpath;
			#endif
			int err = system(command.c_str());
			if (err != 0)
				LERROR(L"Failed to Restore src.db ");
		}
	}
#endif
	if (NULL != logFile)
	{
		bool val = logFile->is_open();
		logFile->close();
		delete logFile;
	}

	return result;
}
