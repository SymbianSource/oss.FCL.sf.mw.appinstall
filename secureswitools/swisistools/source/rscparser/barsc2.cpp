// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
/** 
* @file barsc2.cpp
*
* @internalComponent
* @released
*/
#include <iostream>
#include <sstream>
#include "barsc2.h"
#include <string>

CResourceFile::CResourceFile(const std::string& aName,TUint32 aFileOffset, TInt aFileSize)
	{
	// Creating the implementation instance with a placement new operator.
	new (iImpl) RResourceFileImpl;
	
	// Open the resource file for reading.
	Impl()->OpenL(aName, aFileOffset, aFileSize);
	}


CResourceFile::~CResourceFile()
	{
	RResourceFileImpl* impl = Impl();
	impl->~RResourceFileImpl();
	}


Ptr8* CResourceFile::AllocReadL(const TInt& aResourceId)
	{
	return Impl()->AllocReadL(aResourceId);
	}

/** Initialises the offset value from the first resource.

The function tests to catch cases where the first resource is not an RSS_SIGNATURE.
It assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().
@leave The function leaves if this resource id is not in this
resource file or the file is corrupted. */
void CResourceFile::ConfirmSignatureL()
	{
	Impl()->ConfirmSignatureL();
	}

// Read Uid3 of Resource File
TUid CResourceFile::ReadAppUidL()
	{
	// Unicode compressed RSC file will have 19 bytes header.
	const TUint8 Read_Byte = 4;
	TUint8 header[Read_Byte];
	sTUid uid;
		
	// Read Uid3 of Resource File
	Impl()->ReadL(8,header,Read_Byte);
	// Get the Third UID
	memcpy((TUint8*)&uid.iUid1,header,Read_Byte);

	TUid iUid = {uid.iUid1};
	return iUid;
	}

// Read Uid2 of Resource File
TUid CResourceFile::ReadFileUidL()
	{
	// Unicode compressed RSC file will have 19 bytes header.
	const TUint8 Read_Byte = 4;
	TUint8 header[Read_Byte];
	sTUid uid;
		
	// Read Uid3 of Resource File
	Impl()->ReadL(4,header,Read_Byte);
	// Get the Third UID
	memcpy((TUint8*)&uid.iUid1,header,Read_Byte);

	TUid iUid = {uid.iUid1};
	return iUid;
	}

TBool CResourceFile::OwnsResourceId(const TInt& aResourceId) const
	{
	return Impl()->OwnsResourceId(aResourceId);
	}


RResourceFileImpl* CResourceFile::Impl()
	{
	return reinterpret_cast <RResourceFileImpl*> (iImpl);
	}


const RResourceFileImpl* CResourceFile::Impl() const
	{
	return reinterpret_cast <const RResourceFileImpl*> (iImpl);
	}


void CResourceFileException::Display() const 
	{
	std::ostringstream errDispStream;
	errDispStream<<iValue;
	errDispStream<<std::endl;

// Note: Remove the following tow sentences after integrating with Interpretsis.	
	std::string str = errDispStream.str();
	std::cout<<str;

//  Note: Put this when integrating with InterpretSis
//	std::wstring finalMessage = Utf8ToUcs2( errDispStream.str() );
//  LERROR( finalMessage );
	}

	/**
	Get the exception message
	*/
std::string CResourceFileException::GetMsg()
	{
		return iValue;
	}

