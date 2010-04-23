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
*
*/


#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

// System Includes
#include <iostream>
#include <openssl/sha.h>
// User includes
#include "controllerinfo.h"
#include "deserialiser.h"
#include "serialiser.h"

// SisX includes
#include "siscontroller.h"


ControllerInfo::~ControllerInfo ()
	{
	}

void ControllerInfo::Internalize(Deserialiser& des)
	{
	des >> iVersion >> iOffset >> iHashContainer;
	}
void ControllerInfo::Externalize(Serialiser& ser)
	{
	ser << iVersion << iOffset << iHashContainer;
	}

void ControllerInfo::CalculateAndSetHash(const CSISController& aController, 
										TUint16 aRegFileMajorVersion, 
										TUint16 aRegFileMinorVersion)
	{
	unsigned char hash[SHA_DIGEST_LENGTH];
	memset(hash, 0, SHA_DIGEST_LENGTH);
	
	const unsigned char* buffer = aController.RawBuffer();

	SHA_CTX  ctx;
	SHA1_Init(&ctx);
	if(aRegFileMajorVersion > 5 || (aRegFileMajorVersion == 5 && aRegFileMinorVersion > 1))
		{
		int controllerLength = aController.ControllerSizeForHash();
		SHA1_Update(&ctx, buffer, controllerLength);
		}
	else
		{
		int controllerLength = aController.RawBufferSize();
		SHA1_Update(&ctx, &controllerLength, 4);
		SHA1_Update(&ctx, buffer, controllerLength);
		}
	SHA1_Final(hash,&ctx);
	
	std::string hashStr((char*)hash, SHA_DIGEST_LENGTH);
	iHashContainer.SetData(hashStr);
	iHashContainer.SetHashId(HashContainer::EHashSHA);
	}


#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<ControllerInfo*>& val)
	{
	TUint32 size = 0;
	aInput>> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new ControllerInfo;
		aInput >> *val[i];
		}
	return aInput;
	}
#endif 
