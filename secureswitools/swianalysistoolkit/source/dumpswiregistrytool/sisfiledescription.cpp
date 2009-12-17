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


#include "sisfiledescription.h"

const char* const  FileOperationOptionsList[]=
	{
	"Reserved",
	"FileRunOptionInstall",
	"FileRunOptionUninstall",
	"FileRunOptionByMimeType",
	"FileRunOptionWaitEnd",
	"FileRunOptionSendEnd",
	"Reserved",
	"Reserved",
	"Reserved",
	"FileTextOptionContinue",
	"FileTextOptionSkipIfNo",
	"FileTextOptionAbortIfNo",
	"FileTextOptionExitIfNo",
	"Reserved",
	"Reserved",
	"VerifyOnRestore"
	};

const char* const FileOperationList[]=
	{
	"Install",
	"Run",
	"Text",
	"Null"
	};

SISFileDescription::SISFileDescription()
	:iTarget (0) ,
	iMimeType (0) ,
	iOperation (0) ,
	iOperationOptions (0) ,
	iAlgorithm (0) ,
	iHash (0) ,
	iUncompressedLengthl (0) ,
	iUncompressedLengthh (0) ,
	iIndex (0) ,
	iSid (0)
	{
	iAlgorithms[0] = "MD2";
	iAlgorithms[1] = "MD5";
	iAlgorithms[2] = "SHA1";
	iAlgorithms[3] = "HMAC";
	}


void SISFileDescription::ExtractSISFileDescription(StreamReader& aReader )
	{
	iTarget = aReader.ReadDescriptor();
	iMimeType = aReader.ReadDescriptor();
	iOperation = aReader.ReadInt32();
	//indicates which options are applicable to the processing of this file during installation
	iOperationOptions = aReader.ReadInt32();
	//indicates the algorithm used to generate the hash
	iAlgorithm = aReader.ReadInt32();
	iHash = aReader.ReadDescriptor();
	iUncompressedLengthl = aReader.ReadInt32();
	iUncompressedLengthh = aReader.ReadInt32();
	iIndex = aReader.ReadInt32();
	iSid = aReader.ReadInt32();
	}

void SISFileDescription::DisplaySISFileDescription(int i)
	{
	int j = 0 ;
	int k = 0 ;
	cout << "Target" << i+1 << ":" << iTarget << endl ;
	while(iOperation)
			{
			if(iOperation & 0x01)
				{
				cout << "File Operation Type :" << FileOperationList[j] << endl;
				}
			j++;
			iOperation = iOperation >> 1;
			}

	while(iOperationOptions)
		{
		if(iOperationOptions & 0x01)
			{
			cout << "File Operation Options Type :" << FileOperationOptionsList[k] << endl;
			}
			k++;
			iOperationOptions = iOperationOptions >> 1;
		}
	
	if(iAlgorithms.find(iAlgorithm) !=iAlgorithms.end())
		{
		cout << "Algorithm :" << iAlgorithms[iAlgorithm] << endl;
		}
	else
		{
		cout << "Algorithm :" << "Unknown" << endl;
		}

	cout <<"SID :" << std::hex << "0x" << iSid << endl << endl;
	}

SISFileDescription::~SISFileDescription()
	{
	delete [] iTarget;
	delete [] iMimeType;
	delete [] iHash;
	}



