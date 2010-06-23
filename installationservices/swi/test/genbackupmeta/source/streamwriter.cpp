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
* CStreamWriter.cpp
* For writing data to a file in accordance to symbian descriptor format.
* @internalComponent
*
*/


#include "streamwriter.h"

using std::string;
using std::ofstream;

CStreamWriter::CStreamWriter( const string& aFileName )
	{
	iFileStream.open(aFileName.c_str(), std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
	}

CStreamWriter::~CStreamWriter()
	{
	iFileStream.flush();
	iFileStream.close();
	}

void CStreamWriter::Write(char* aData, int aLength)
	{
	iFileStream.write(aData, aLength);
	}

void CStreamWriter::Externalize(const char* aData, const int aLength, const bool aWide)
	{
	Externalize(aLength,aWide);
	iFileStream.write(aData, aLength);
	}

void CStreamWriter::Externalize(const int aLength, const bool aWide)
	{
	TUint32 descriptorInfo = CreateDescriptorInfo(aLength,aWide);
	// write the descriptor info
	if( descriptorInfo <= 0xff)
		{
		iFileStream.write( reinterpret_cast<char *>(&descriptorInfo), sizeof(TUint8) );
		}
	else if( descriptorInfo <= 0xffff)
		{
		iFileStream.write( reinterpret_cast<char *>(&descriptorInfo), sizeof(TUint16) );	
		}
	else
		{
		iFileStream.write( reinterpret_cast<char *>(&descriptorInfo), sizeof(TUint32) );	
		}
	}

TUint32 CStreamWriter::CreateDescriptorInfo(const TUint32 aLength, const bool aIsWide)
	{
	TUint32 descriptor=0;
	TUint32 bitLength = GetBitLength(aLength);
	TUint8 shiftValue;

	if(bitLength <= 6)
		{
		shiftValue = 2;
		}
	else if(bitLength <= 13 )
		{
		descriptor |= 0x01;
		shiftValue = 3;
		}
	else if(bitLength <= 28)
		{
		descriptor |= 0x03;
		shiftValue = 4;
		}
	
	if(aIsWide)
		{
		descriptor |= 0x01 << (shiftValue-1);
		}
	
	descriptor |= aLength<< shiftValue; 
	return descriptor;
	}

TUint32 CStreamWriter::GetBitLength(TUint32 aLength)
	{
	TUint32 count=0;
	do
	{
		++count;
	} while((aLength>>= 1) != 0);

	return count;
	}