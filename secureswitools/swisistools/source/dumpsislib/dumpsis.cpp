/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif 

#include <iostream>
#include <sstream>    // for strstream, wistringstream
#include <stdio.h>
#include <sys/stat.h>
#include <algorithm>
#include <functional>
#include <set>

#include "dumpsis.h"
#include "siscontroller.h"
#include "exception.h"
#include "utf8_wrapper.h"

CDumpSis::CDumpSis(const std::wstring& aSISFileName, bool aVerbose)
		: 	iController(NULL),
			iSisFileName(aSISFileName),
			iVerbose(aVerbose),
			iIsStub(false)
	{
	bool isSiSFile = CSISContents::IsSisFile(aSISFileName);
	if(isSiSFile)
		{
		iSisContents.Load (iSisFileName);
		iController = &iSisContents.Controller();
		}
	else
		{
		iIsStub = true;
		iController = new CSISController;
		SISLogger::Log(L"Dumping Stub SIS Controller\n");
		iController->Load(iSisFileName);
		}
	}

CDumpSis::~CDumpSis()
	{
	if(iIsStub)
		{
		delete iController;
		}
	}

void CDumpSis::CreatePackage(const std::wstring& aPkgFileName)
	{
	std::wistringstream inStream (std::ios::in | std::ios::out);
	std::wostream outStream (inStream.rdbuf ());

	outStream << wchar_t(0xfeff);

	if(iIsStub)
		{
		iController->AddPackageEntry (outStream, iVerbose);
		}
	else
		{
		iSisContents.AddPackageEntry (outStream, iVerbose);
		}
	
	std::wstring str = inStream.str();
#ifndef __TOOLS2_LINUX__
	WriteToFile(aPkgFileName, reinterpret_cast<const TUint8*>(str.c_str()), str.length()*2);
#else
	TUint32 size = str.size();
	const wchar_t * source = str.c_str();

	unsigned short int* buffer = new unsigned short int[size*2];

	// Using a temp variable in place of buffer as ConvertUTF32toUTF16 modifies the source pointer passed.
	unsigned short int* temp = buffer;

	//Converting to UTF-16 because makesis do not support UTF-32.	
	ConvertUTF32toUTF16(&source, source + size, &temp,  temp + (size*2), lenientConversion);

	TUint32 buflen = temp-buffer;
	// Appending NULL to the converted buffer.
	*temp = NULL;

	WriteToFile(aPkgFileName, reinterpret_cast<const TUint8*>(buffer), buflen*2);
	delete[] buffer;
#endif
	}

void CDumpSis::CreatePackage(const CSISController& aSisController, const std::wstring& aPkgFileName)
	{
	std::wistringstream inStream (std::ios::in | std::ios::out);
	std::wostream outStream (inStream.rdbuf ());

	outStream << wchar_t(0xfeff);
	aSisController.AddPackageEntry (outStream, iVerbose);
	std::wstring str = inStream.str();
#ifndef __TOOLS2_LINUX__
	WriteToFile(aPkgFileName, reinterpret_cast<const TUint8*>(str.c_str()), str.length()*2);
#else
	TUint32 size = str.size();
	const wchar_t * source = str.c_str();
	
	unsigned short int* buffer = new unsigned short int[size*2];
	
	// Using a temp variable in place of buffer as ConvertUTF32toUTF16 modifies the source pointer passed.
	unsigned short int* temp = buffer;
	
	//Converting to UTF-16 because makesis do not support UTF-32.	
	ConvertUTF32toUTF16(&source, source + size, &temp,	temp + (size*2), lenientConversion);
	
	TUint32 buflen = temp-buffer;
	// Appending NULL to the converted buffer.
	*temp = NULL;
	
	WriteToFile(aPkgFileName, reinterpret_cast<const TUint8*>(buffer), buflen*2);
	delete[] buffer;
#endif
	}

void CDumpSis::ExtractFiles(const std::wstring& aTargetDir, TExtractionLevel aLevel)
	{
	std::wstring targetDir = aTargetDir;
	CreateTargetDir(targetDir);
	
	switch(aLevel)
		{
		case EOnlyPackage:
			{
			ExtractBasePackageFile(targetDir);
			break;
			}
		case ECertificates:
			{
			ExtractCertificates(*iController, targetDir);
			break;
			}
		case EBaseFiles:
			{
			ExtractFiles(*iController, targetDir);
			break;
			}
		case EAllDataFiles:
			{
			ExtractBasePackageFile(targetDir);	
			ExtractFiles(*iController, targetDir);
			ExtractNestedSisFile(targetDir, *iController, false, 0, iSisContents.SisData().DataUnitCount());
			break;
			}
		case EAllButCerts:
			{
			ExtractBasePackageFile(targetDir);	
			ExtractFiles(*iController, targetDir);
			ExtractNestedSisFile(targetDir, *iController, true, 0, iSisContents.SisData().DataUnitCount());
			break;
			}
		case EEverything:
			{
			ExtractBasePackageFile(targetDir);	
			ExtractFiles(*iController, targetDir);
			ExtractNestedSisFile(targetDir, *iController, true, 0, iSisContents.SisData().DataUnitCount());
			ExtractAllCertificates(targetDir);
			break;
			}
		}
	}

void CDumpSis::ExtractAllCertificates(const std::wstring& aTargetDir)
	{
	ExtractCertificates(*iController, aTargetDir);
	
	TControllerMap controllerList;
	iController->InstallBlock().GetEmbeddedControllers(controllerList);
	
	for (TControllerMapConstIter iter = controllerList.begin(); iter != controllerList.end(); ++iter)
		{
		wchar_t pathName[20];
		swprintf(pathName, 20, L"sis%d", iter->first);

		std::wstring extractionPath = aTargetDir + KSisDirectorySeparator + pathName;
		CreateTargetDir(extractionPath);
		ExtractCertificates(*iter->second, extractionPath);
		}	
	}
void CDumpSis::ExtractCertificates(const CSISController& aSisController, const std::wstring& aTargetDir)
	{
	TUint32 certChainCount = aSisController.SignatureCount();
	for(TUint32 i = 0; i < certChainCount; ++i)
		{
		wchar_t certName[30];
		swprintf(certName, 30, L"certChain%d%d.der", aSisController.DataIndex(), i+1);
		std::wstring certFullPath = aTargetDir + KSisDirectorySeparator + certName;
		const CCertChainData certChain = aSisController.SignatureCertChain(i).CertificateChain();
		const TUint8* certChainData = certChain.CertificateData().Data();
		TUint32 certSize =  certChain.CertificateData().Size();
		WriteToFile(certFullPath, certChainData, certSize);
		}
	}


void CDumpSis::ExtractBasePackageFile(const std::wstring& aTargetDir)
	{
	std::wstring pkgFileName = iSisFileName;
	pkgFileName = FixPathDelimiters(pkgFileName);
	SisFileNameToPkgFileName(pkgFileName);
	pkgFileName = aTargetDir + KSisDirectorySeparator + pkgFileName;
	CreatePackage(pkgFileName);
	}
void CDumpSis::ExtractFiles(const CSISController& aSisController, const std::wstring& aTargetDir, int aBaseIndex)
	{
	if(iIsStub)
		{
		// There is no data associated with stub sis file.
		return;
		}
	TFileDescList fileList;
	aSisController.InstallBlock().GetFileList(fileList);
	// Controller's index is relative to base controller's index.
	TUint32 dataIndex = aBaseIndex + aSisController.DataIndex();
	if(dataIndex >= iSisContents.SisData().DataUnitCount())
		{
		return;
		}
	const CSISDataUnit& dataUnit = iSisContents.SisData().DataUnit(dataIndex);
	
	const CSISLogo& logo = aSisController.Logo();
	if(!logo.WasteOfSpace())
		{
		const CSISFileDescription& logoFile = logo.FileDesc();
		const wchar_t* fileName = logoFile.GetFileName();
		std::wstring filePath = aTargetDir + KSisDirectorySeparator + fileName;
		const CSISFileData& fileData = dataUnit.FileData(logoFile.FileIndex());
		WriteToFile(filePath, fileData.Data(), fileData.UncompressedSize());
		delete[] const_cast<wchar_t*>(fileName);
		}
	
	
	if(dataUnit.FileCount() <= 0)
		{
		return; // No files present to extract.
		}

	for(int i = 0; i < fileList.size(); ++i)
		{
		const CSISFileDescription* fdesc = fileList[i];
		const wchar_t* fileName = fdesc->GetFileName();
		std::wstring filePath = aTargetDir + KSisDirectorySeparator + fileName;
		const CSISFileData& fileData = dataUnit.FileData(fdesc->FileIndex());
		WriteToFile(filePath, fileData.Data(), fileData.UncompressedSize());
		delete[] const_cast<wchar_t*>(fileName);
		}
	}

void CDumpSis::ExtractNestedSisFile(const std::wstring& aTargetDir, 
									const CSISController& aController,
									bool aExtractSis,
									int aStartIndex, 
									int aEndIndex)
	{
	TControllerMap controllerList;
	aController.InstallBlock().GetEmbeddedControllers(controllerList);
	
	for (TControllerMapConstIter iter = controllerList.begin(); iter != controllerList.end(); ++iter)
		{
		TControllerMapConstIter nextIter = iter;
		int curindex = iter->second->DataIndex();
		int maxDataUnit = 0;
		if (++nextIter != controllerList.end())
			{
			maxDataUnit = nextIter->first;
			}
		else
			{
			maxDataUnit = aEndIndex;
			}
		wchar_t pathName[20];
#ifdef _MSC_VER
		swprintf(pathName, L"sis%d", iter->second->GetControllerID());
#else
		swprintf(pathName, 20, L"sis%d", iter->second->GetControllerID());
#endif //_MSC_VER
		
		if(aExtractSis)
			{
			std::wstring sisPath = aTargetDir + KSisDirectorySeparator + pathName + L".sis";
			CSISController* ctl = const_cast<CSISController*>(iter->second);
			CreateEmbeddedSis(sisPath, *ctl, aStartIndex + iter->first, maxDataUnit);
			}

		std::wstring extractionPath = aTargetDir + KSisDirectorySeparator + pathName;
		CreateTargetDir(extractionPath);
		ExtractFiles(*iter->second, extractionPath, aStartIndex);
		extractionPath = extractionPath + KSisDirectorySeparator + pathName + L".pkg";
		CreatePackage(*iter->second, extractionPath);
		ExtractNestedSisFile(aTargetDir, *(iter->second), aExtractSis, aStartIndex + iter->first, maxDataUnit);
		}
	}

void CDumpSis::CreateTargetDir(std::wstring& aTargetDir)
	{
	aTargetDir = FixPathDelimiters(aTargetDir);
	if(CreateDir(aTargetDir.c_str()) != 0)
		{
		return;
		}

	int err = GetErrorValue();
	
	switch (err)
		{
		case ERROR_ALREADY_EXISTS:
			{
			int attrs;

			if (0xFFFFFFFF != (attrs = FileAttributes(aTargetDir.c_str())))
				{
				if (!(attrs & FILE_ATTRIBUTE_DIRECTORY))
					{
					throw CSISException::EDirIsFile;
					}
				else if (attrs & FILE_ATTRIBUTE_READONLY)
					{
					throw CSISException::EPermissionDenied;
					}
				}
			else
				{
				throw CSISException::EInvalidDestination;
				}
			break;
			}
		case ERROR_PATH_NOT_FOUND:
			CreateDirectoriesRecursively(aTargetDir);
			break;

		case ERROR_ACCESS_DENIED:
			throw CSISException::EInvalidDestination;
			break;

		case ERROR_INVALID_NAME:
			aTargetDir = L".";
			break;

		default:
			throw CSISException::EInvalidDestination;
		}
	}


void CDumpSis::CreateDirectoriesRecursively(std::wstring aTargetDir)
	{
	int pos = aTargetDir.find(L'/');
	
	if(0 == pos)
		{
		// Path starting with "/" can be skipped
		pos = aTargetDir.find(L'/', 1);
		}
	
	while(pos != std::wstring::npos)
		{
		std::wstring path = aTargetDir.substr(0, pos);
		
		if (0 == CreateDir(path.c_str()))
			{
			int err = GetErrorValue();
			switch(err)
				{
				case ERROR_ACCESS_DENIED:
					{
					// If this is caused by a path like "D:" we can continue
					bool validPath = ((aTargetDir.size() >= 2) && (isalpha(aTargetDir[0])) && (aTargetDir[1] ==L':'));
					if (!validPath)
						{
						throw CSISException::EInvalidDestination;
						}
					break;
					}
	
				case ERROR_ALREADY_EXISTS:
					// NOOP
					break;
	
				default:
					throw CSISException::EInvalidDestination;
				}
			}
		pos = aTargetDir.find(L'/', pos+1);
		}

	if (0 == CreateDir(aTargetDir.c_str()))
		{
		if ( GetErrorValue() != ERROR_ALREADY_EXISTS)
			{
			throw CSISException::EInvalidDestination;
			}
		}
	}

void CDumpSis::SisFileNameToPkgFileName(std::wstring& aFileName)
	{
	int pos = aFileName.find_last_of(L'/');
	if(std::wstring::npos != pos)
		{
		aFileName = aFileName.substr(pos+1);
		}

	for(int i = 0; i < aFileName.size(); ++i)
		{
		aFileName[i] = tolower(aFileName[i]);
		}
	
	pos = aFileName.rfind(L".sis");
	if(std::wstring::npos == pos)
		{
		pos = aFileName.rfind(L".ctl");
		}

	aFileName = aFileName.substr(0, pos);
	aFileName.append(L".pkg");
	}

void CDumpSis::CreateEmbeddedSis(const std::wstring& aFileName, CSISController& aController, int aStart, int aEnd)
	{
	const CSISData& parentSisData = iSisContents.SisData();
	TUint32 dataIndex = aController.DataIndex();
	aController.SetDataIndex(0);
	CSISCompressed<CSISController> compressedController(aController);
	aController.SetDataIndex(dataIndex);

	CSISData sisData;
	for (int i = aStart; i < aEnd; ++i)
		{
		sisData.AddDataUnit(parentSisData.DataUnit(i));
		}
	
	CSISContents contents(	const_cast<CSISControllerChecksum&>(iSisContents.ControllerChecksum()),
							const_cast<CSISDataChecksum&>(iSisContents.DataChecksum()),
							compressedController,
							sisData
						 	);
	
	contents.Save(aFileName);
	}

void CDumpSis::GetCapVerifiedFileList(TFileCapTestList& aFileCapList)
	{
	TFileDescList fileList;
	iSisContents.Controller().InstallBlock().GetFileList(fileList);
	const CSISDataUnit& dataUnit = iSisContents.SisData().DataUnit(0);
	int fileCount = fileList.size();
	for(int i = 0; i < fileCount; ++i)
		{
		const CSISFileDescription* fdesc = fileList[i];
		const wchar_t* fileName = L"symbiantemp.xyz";
		const CSISFileData& fileData = dataUnit.FileData(fdesc->FileIndex());
		WriteToFile(fileName, fileData.Data(), fileData.UncompressedSize());
		CSISCapabilities capObj;
		capObj.ExtractCapabilities(fileName);
		TFileCapTest fileCapTest;
		fileCapTest.iFileDesc = fdesc;
		fileCapTest.iActualCap = capObj.Capabilities();
		fileCapTest.iCapVerified = (fileCapTest.iActualCap == fileList[i]->Capabilities())? true : false;
		aFileCapList.push_back(fileCapTest);
		MakeSISDeleteFile(fileName);
		}
	}


// End Of File
