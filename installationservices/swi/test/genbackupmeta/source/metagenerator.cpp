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
* For generating meta-data of a SIS file.
* @internalComponent
*
*/



#include "metagenerator.h"
#include "exceptionhandler.h"
#include "cmdparser.h"
#include "streamwriter.h"
#include "filecontents.h"
#include "siscompressed.h"
#include "preamble.h"
#include "tlv.h"
#include "utils.h"

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>
#include <iostream>
using namespace std;

typedef vector<string>::iterator StringIterator;

CMetaGenerator::CMetaGenerator(	const string& aUid, const int aDriveName, 
								const vector<string>& aSISFileNames, 
								const vector<string>& aFileNames, const string& aMetaFileName)
	:iUid(aUid),
	iDriveName(aDriveName),
	iSISFileNames(aSISFileNames),
	iFileNames(aFileNames),
	iMetaFileName(aMetaFileName)
	{
	iStreamWriter = new CStreamWriter(iMetaFileName);
	}

CMetaGenerator::~CMetaGenerator()
	{
	delete iStreamWriter;
	}

void CMetaGenerator::CleanUp()
	{
	delete iStreamWriter;
	iStreamWriter = NULL;
	remove(iMetaFileName.c_str());
	}

void CMetaGenerator::GenerateMetaData() throw (TExceptionHandler)
	{
	ValidateSISFiles();
	
	if( iSISFileNames.empty() )
		{
		throw TExceptionHandler(TExceptionHandler::ENoValidFile);
		}

	iStreamWriter->Write(reinterpret_cast<char*>(&iDriveName), sizeof(TUint32));	
	
	int controllerCount = iSISFileNames.size();
	iStreamWriter->Write(reinterpret_cast<char *>(&controllerCount), sizeof(controllerCount));

	WriteControllers();

	int fileCount  = iFileNames.size();
	iStreamWriter->Write(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));
	
	for(StringIterator fileIter=iFileNames.begin();fileIter!= iFileNames.end();++fileIter)
		{
		iStreamWriter->Externalize(fileIter->c_str(), fileIter->length(),false);
		}
	}

void CMetaGenerator::ValidateSISFiles() throw (TExceptionHandler)
	{
	StringIterator SISIterator = iSISFileNames.begin();
	
	while(SISIterator != iSISFileNames.end())
		{		
		CFileContents inputfile (SISIterator->c_str());
		
		if(!Preamble::IsPreamble(inputfile))
			{
			throw TExceptionHandler(TExceptionHandler::EFileFormatError);
			}

		int uID = Utils::HexToInt(iUid);
		
		if(!Preamble::IsUidValid(uID))
			{
			SISIterator = iSISFileNames.erase(SISIterator);
			}
		else
			{
			++SISIterator; 	
			}
	} // end of while.
	}

void CMetaGenerator::WriteControllers()
	{
	for(StringIterator SISIter=iSISFileNames.begin() ; SISIter!=iSISFileNames.end() ; ++SISIter)
		{
		CFileContents SISReader(SISIter->c_str());
		
		TUint32 controllerLength = 0;
		// Ownership of totalController with caller.
		char* totalController = const_cast<char*>(GetControllerDetails(controllerLength,SISReader));
		CFileContents SISController(totalController,controllerLength);
		TLV tlv(  EFieldTypeController,SISController );	
		controllerLength = tlv.Length();
		
		// pointer inside already allocated memory(totalController)
		// Ownership with caller. Not to be deleted from this offset.
		/// Deletion of totalController to be done directly.
		const char* controller = tlv.BinaryData();
		
		iStreamWriter->Externalize(sizeof(controllerLength)+controllerLength,true);
		iStreamWriter->Write( reinterpret_cast<char *>(&controllerLength), sizeof(controllerLength) );
		iStreamWriter->Write(const_cast<char*>(controller), controllerLength);
		
		delete totalController;
		}
	}

const char* CMetaGenerator::GetControllerDetails(TUint32& aControllerLength,CFileContents aSISReader) const
	{
	
	TUint32 UID1 = aSISReader.GetTUint32();
	TUint32 UID2 = aSISReader.GetTUint32();
	TUint32 UID3 = aSISReader.GetTUint32();
	TUint32 UIDCheckSum = aSISReader.GetTUint32();

	TLV tlvSISContents(EFieldTypeContents,aSISReader);

	aSISReader = tlvSISContents.iFileContents;
		
	TLV tlvContCRC(EFieldTypeControllerCRC,aSISReader);
	TLV tlvDataCRC(EFieldTypeDataCRC,aSISReader);
	
	TLV tlvCompressed(EFieldTypeCompressed,aSISReader);
	CSISCompressed siscomp(tlvCompressed.iFileContents);
	
	aControllerLength = siscomp.DecompressedLength();
	
	const char* decompController = siscomp.DecompressedData();
	
	// Ownership of memory cleanup passed to the caller.
	char* controller = new char[aControllerLength];
	memcpy(controller,decompController,aControllerLength);
	
	return controller;
	}

std::string CMetaGenerator::FilePrefix = "metadata";