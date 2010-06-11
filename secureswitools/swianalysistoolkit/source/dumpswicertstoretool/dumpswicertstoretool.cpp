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



#include "dumpswicertstoretool.h"
#include "options.h"
#include "datalocator.h"
#include "osinterface.h"

#ifdef _MSC_VER
#include <new>
#include <new.h>
#endif /* _MSC_VER */

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

	Options* options = 0;
	//if no arguments are specified,display help msg and exit.
	if(argc < 2)
		{
		Options::DisplayUsage();
		return 0;
		}
	try
		{
		options = new Options(argc, argv) ;
		//Required in order to delete the der files which are present in the current directory.
		string curDirectory = ".";
		string extension = "der";
		OSInterface::DeleteFilesFromDirectory(curDirectory , extension);
		Tool.Run(*options);
		}

	catch (Exceptions aErr)
		{
		Options::DisplayError (aErr);
		delete options;
		return aErr;
		}

	catch (bad_alloc&)
		{
		cout << " Error Allocating Memory " << endl ;
		}

	delete options;
	return 0;
	}

// Class DumpSWICertstoreTool
class DumpSWICertstoreTool Tool;

DumpSWICertstoreTool::DumpSWICertstoreTool()
	:iTocRevision (0),
	iErrors (0)
	{
	}

void DumpSWICertstoreTool::Run(const Options& aOptions)
	{
	if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
		{
		ofstream iniFile("swicertstore.txt",ios::out);
		iniFile.close();
		}

	//if both ROM and C:\Based certstore are specified.
	if((aOptions.CBasedWritableCertstore()) && (aOptions.ROMCertstore()))
		{
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			cout << "Merged ROM Based and C:BASED Writable Certstore " << endl << endl;
			}

		//calls C:\Based certstore first
		bool isCBasedValid = InitializeCBasedCertstore(aOptions);
		if(!isCBasedValid)
			{
			cout << "C: Based Writable Certstore is not a Permanent File Store " << endl << endl;
			}
		
		//ROM Based certstore called next.
		bool isROMValid = InitializeROMCerstore(aOptions);
		if(!isROMValid)
			{
			cout << (aOptions.ROMCertstoreFileName()).c_str() << " : Not a Permanent File Store " <<endl <<endl;
			}
		}

	//when only ROM Based dat file specified.
	else if (!(aOptions.CBasedWritableCertstore()) && (aOptions.ROMCertstore()))
		{
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			cout <<"ROM Based SwiCertStore :" << endl << endl;
			}
		bool isROMValid = InitializeROMCerstore(aOptions);
		if(!isROMValid)
			{
			throw ENotAPermanentFileStore;;
			}
		}

	//when only C:\Based Certstore is specified
	else if((aOptions.CBasedWritableCertstore()) && !(aOptions.ROMCertstore()))
		{
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			cout << " C:\\ Based Writable SwiCertstore :" << endl << endl;
			}
		bool isCBasedValid = InitializeCBasedCertstore(aOptions);
		if(!isCBasedValid)
			{
			throw ENotAPermanentFileStore;
			}
		}

	if(aOptions.CreateCCIFile() || aOptions.CreateDetailCCIFile())
		{
		cout << "[CAPABILITIES]" <<endl;
		cout << iIssuer <<endl;
		cout << "[MANDATORY]" <<endl;
		cout << iMandatory <<endl;
		}
	}

bool DumpSWICertstoreTool::InitializeCBasedCertstore(const Options& aOptions)
	{
	StoreFile* store = 0;
	int size = aOptions.GetDirectoryFiles().size();
	if(size == 0)
		{
		if(!aOptions.CreateCCIFile() && !aOptions.CreateDetailCCIFile())
			{
			cout << "No files in the specified Writable Certstore" << endl << endl;
			}
		}

	for(int i = size-1; i >= 0; i--)
		{
		string fileName = aOptions.GetDirectoryFiles().at(i);
		store = new StoreFile();
		bool isFileValid = store->CheckFileValidity(fileName.c_str(),aOptions);
		//supposing the file is corrupt,the previous file is considered,else there will be only 1 file in the directory.
		if(isFileValid)
			{
			Initialize(aOptions,*store);
			delete store;
			return true;
			}
		delete store;
		}
	
	return false;
	}

bool DumpSWICertstoreTool::InitializeROMCerstore(const Options& aOptions)
	{
	StoreFile* store = 0;
	store = new StoreFile();
	bool isFileValid = store->CheckFileValidity((aOptions.ROMCertstoreFileName()).c_str(),aOptions);
	if(isFileValid)
		{
		Initialize(aOptions,*store);
		delete store;
		return true;
		}

	delete store;
	return false;
	}

void DumpSWICertstoreTool::Initialize(const Options& aOptions , StoreFile& aStore)
	{
	Stream root = aStore.RootStream();
	DataLocator dataLoc(root, aStore.File());
	StreamReader reader(dataLoc.GetStream());
	reader.Initialise();
	Handle metadataHandle(reader.ReadInt32());

	Stream metadataStream = aStore.FindStream(metadataHandle);
	SwiCertStoreMetaData  metadata(metadataStream, aStore);
	metadata.Read(aOptions);
	iIssuer.append(metadata.GetIssuer());
	iMandatory.append(metadata.GetMandatoryStatusOfAllCertstoreCerts());
	}

ostream& DumpSWICertstoreTool::Error()
	{
	++Tool.iErrors;
	return cerr << "error: ";
	}

ostream& DumpSWICertstoreTool::Warning()
	{
	++Tool.iErrors;
	return cerr << "warning: ";
	}

DumpSWICertstoreTool::~DumpSWICertstoreTool()
	{
	}






