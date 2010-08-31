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
*
*/


#ifndef	SERIALISER_H
#define	SERIALISER_H

#include <iostream>
#include <ostream>
#include <vector>
#include "symbiantypes.h"
#include "serialisable.h"
#include "ucmp.h"
#include "cardinality.h"
#include "utf8_wrapper.h"

/**
* @file SERIALISER.H
*
* @internalComponent
* @released
*/
class Serialiser
	{
public:
	Serialiser(std::ostream& stream) : iStream(stream)
	{}

	// integer types
	Serialiser& operator<<(TInt64& val)
	{ iStream.write(reinterpret_cast<char*>(&val),8); return *this; }

	Serialiser& operator<<(TInt& val)
	{ iStream.write((char*)&val,4); return *this; }

	Serialiser& operator<<(TInt16& val)
	{ iStream.write((char*)&val,2); return *this; }

	Serialiser& operator<<(TInt8& val)
	{ iStream.write((char*)&val,1); return *this; }

	Serialiser& operator<<(TUint64& val)
	{ iStream.write(reinterpret_cast<char*>(&val),8); return *this; }

	Serialiser& operator<<(TUint32& val)
	{ iStream.write((char*)&val,4); return *this; }

	Serialiser& operator<<(TUint16& val)
	{ iStream.write((char*)&val,2); return *this; }

	Serialiser& operator<<(TUint8& val)
	{ iStream.write((char*)&val,1); return *this; }

	Serialiser& operator<<(bool& val)
	{ TUint32 v = ((TUint32)val); return *this << v; }

	// STL containers
	Serialiser& operator<<(std::string& val)
	{
	Cardinality card;
	TUint32 size = (val.size() << 1) + 0x01;
	card.SetSize(size);
	*this << card;
	iStream.write(val.c_str(),val.size());

	return *this;
	}

	Serialiser& operator<<(std::wstring& val)
	{

	std::wstring tempval = val.c_str();
	
#ifdef __TOOLS2_LINUX__
	std::wstring::size_type idx = 0;
	while( (idx = tempval.find(L"/", idx)) != std::wstring::npos)
        {
		tempval.replace( idx, 1, L"\\" );
        }
#endif

	Cardinality card;
	TUint32 size = tempval.size() << 1;

#ifdef __TOOLS2_LINUX__
	const wchar_t * source = tempval.c_str();
	unsigned short int* buffer = new unsigned short int[size];

	// Using a temp variable in place of buffer as ConvertUTF32toUTF16 modifies the source pointer passed.
	unsigned short int* temp = buffer;

	ConvertUTF32toUTF16(&source, tempval.c_str() + tempval.size(), &temp,  temp + size, lenientConversion);

	// Appending NULL to the converted buffer.
	*temp = NULL;
#endif
	
	card.SetSize(size);
	*this << card;

	TUnicodeCompressor comp;

#ifdef __TOOLS2_LINUX__
	TMemoryUnicodeSource src((TUint16*)buffer);
#else
	TMemoryUnicodeSource src((TUint16*)tempval.c_str());
#endif

	//The compressed unicode output could end up larger than the input, thus restricting the output to KMaxTInt.
	comp.CompressL(*this, src, KMaxTInt, tempval.size());
#ifdef __TOOLS2_LINUX__
	delete[] buffer;
#endif
	return *this;
	}

	template <class T>
	Serialiser& operator<<(std::vector<T>& val)
	{
	TUint32 size = val.size();
	*this << size;
	for (TUint32 i = 0; i < size ; ++i)
		{
			*this << val[i];
		}
	return *this;
	}

	// direct write access to buffer
	void write(const TUint8* aSrc, TUint32 aCount)
	{
	iStream.write((const char*)aSrc, aCount);
	}


	// serialisable objects
	Serialiser& operator<<(Serialisable& val)
	{ val.Externalize(*this); return *this; }

	Serialiser& operator<<(Serialisable* val)
	{ val->Externalize(*this); return *this; }

private:
	std::ostream& iStream;
	};


#endif	/* SERIALISER_H */
