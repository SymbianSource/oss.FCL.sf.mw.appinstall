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


#include "osinterface.h"
#include <windows.h>
#include <Shlwapi.h>
#include "symbiantypes.h"

const int KLen = 256;

const std::vector <STRING>  OSInterface::ExtractFilesFromDirectory(const STRING& aRootDir,const STRING& aExtension)                    
	{
	STRING			pathOfFile;
	STRING			patternOfString;
	WIN32_FIND_DATA fileInfo;

    patternOfString = aRootDir + _T("\\*.*") ;

	HANDLE hToFile = ::FindFirstFile(patternOfString.c_str(), &fileInfo);	
	
	std::vector<STRING> aDirFiles;

	if(hToFile != INVALID_HANDLE_VALUE)
		{
		do
			{
			if(fileInfo.cFileName[0] != '.')
				{
				pathOfFile.erase();
				
				pathOfFile = aRootDir + _T("\\")+ fileInfo.cFileName;
				STRING file = fileInfo.cFileName;
								
				STRING extOfString = file.substr(file.rfind(_T(".")) + 1);
				
				if(aExtension.length())
					{
					if(extOfString == aExtension)
						{
						aDirFiles.push_back(pathOfFile);
						}
					}
				else
					{
					bool valid = IsInteger(file);
					if(valid)
						{
						aDirFiles.push_back(pathOfFile);
						}
					}
				}
			}while(::FindNextFile(hToFile , &fileInfo));

		::FindClose(hToFile);
	  	}
	return aDirFiles;
	}


void OSInterface::DeleteFilesFromDirectory(const STRING& aRootDir,const STRING& aExtension)                    
	{
	STRING			patternOfString;
	WIN32_FIND_DATA fileInfo;
    patternOfString = aRootDir + _T("\\*.*") ;
	
	HANDLE hToFile = ::FindFirstFile(patternOfString.c_str(), &fileInfo);	
	
	if(hToFile != INVALID_HANDLE_VALUE)
		{
		do
			{
			if(fileInfo.cFileName[0] != '.')
				{
				
				STRING file = fileInfo.cFileName;
				
				STRING extOfString = file.substr(file.rfind(_T(".")) + 1);

				if(extOfString == aExtension)
					{
					::DeleteFile(file.c_str());
					}
				}		
			}while(::FindNextFile(hToFile , &fileInfo));

		::FindClose(hToFile);
	  	}
	}

bool OSInterface::IsInteger(const STRING& aFileName) 
	{ 
	const _TCHAR* name = aFileName.c_str();
	while(*name >= '0' && *name <= '9')
		{
		name++;
		}
	if(*name != 0)
		{
		return false;
		}
	return true;
	} 


int OSInterface::CheckIfDirectory(const STRING& aFile) 
	{
	return ::PathIsDirectory(aFile.c_str());
	}

int OSInterface::DeleteDirectory(STRING& aDirectory) 
	{
	return ::RemoveDirectory(aDirectory.c_str());
	}

void OSInterface::ReadPrivateProfile(const STRING& aIniSectionName , const STRING& aRootLabel ,const STRING& aCertstoreFile , _TCHAR* aBuf)
	{
	::GetPrivateProfileString(aIniSectionName.c_str() , aRootLabel.c_str(), _T(""), aBuf, KLen , aCertstoreFile.c_str());
	}

