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


#ifndef	DESERIALISER_H
#define	DESERIALISER_H

#include <iostream>
#include <stdexcept>
#include <istream>
#include <vector>
#include "serialisable.h"
#include "cardinality.h"
#include "ucmp.h"
#include "utf8_wrapper.h"
#include "is_utils.h"

#pragma warning (disable: 4800)


/**
* @file DESERIALISER.H
*
* @internalComponent
* @released
*/
class Deserialiser
	{
public:
	Deserialiser(std::istream& stream) : iStream(stream)
	{}

	Deserialiser& operator>>(Serialisable& val)
	{
	val.Internalize(*this);
	return *this;
	}

	// integer types
	Deserialiser& operator>>(TInt64& val)
	{ iStream.read(reinterpret_cast<char*>(&val),8); return *this; }

	Deserialiser& operator>>(TInt& val)
	{ iStream.read((char*)&val,4); return *this; }

	Deserialiser& operator>>(TInt16& val)
	{ iStream.read((char*)&val,2); return *this; }

	Deserialiser& operator>>(TInt8& val)
	{ iStream.read((char*)&val,1) ; return *this; }

	Deserialiser& operator>>(TUint64& val)
	{ iStream.read(reinterpret_cast<char*>(&val),8);  return *this; }

	Deserialiser& operator>>(TUint32& val)
	{ iStream.read((char*)&val,4);  return *this; }

	Deserialiser& operator>>(TUint16& val)
	{ iStream.read((char*)&val,2); return *this; }

	Deserialiser& operator>>(TUint8& val)
	{iStream.read((char*)&val,1); return *this; }

	Deserialiser& operator>>(bool& val)
	{
	TUint32 x = 0;
	iStream.read((char*)&x,4);
	val = (bool)x;
	return *this;
	}


	template <class T>
	Deserialiser& operator>>(std::vector<T>& val)
	{
	TUint32 size = 0;
	*this >> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		*this >> val[i];
		}
	return *this;
	}

#ifndef _MSC_VER

	template <class T>
	Deserialiser& operator>>(std::vector<T*>& val)
	{
	TUint32 size = 0;
	*this >> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new T;
		*this >> *val[i];
		}
	return *this;
	}

#endif

	Deserialiser& operator>>(std::string& val)
	{

	Cardinality card;
	*this >> card;
	TUint32 size = card;

	char* buff = 0;
	if (size & 0x01)
		{
		// 8-bit
		size = size >> 1;
		buff = new char[(int)size];
		iStream.read(buff, size);
		}
	else
		{
		throw std::runtime_error("Decoding unicode into std::string");
		}
	val.assign(buff, size);
	delete buff;
	return *this;
	}

#ifdef __TOOLS2_LINUX__

	Deserialiser& operator>>(std::wstring& val)
	{
		Cardinality card;
		*this >> card;
		TUint32 size = card;

		TUint32 sizebackup = size;

		unsigned short int* buff = 0;
	
		if (size & 0x01)
		{
			throw std::runtime_error("Decoding 8bit text into std::wstring");
		}
		else
		{
			size = size >> 1;
			buff = new unsigned short int[(int)size];
			TUnicodeExpander exp;
			TMemoryUnicodeSink sink((TUint16*)buff);
			exp.ExpandL(sink, *this ,size);
			unsigned short int* source = buff;
			wchar_t buffer[size];
			// Using a temp variable in place of buffer as ConvertUTF16toUTF32 modifies the source pointer passed.
			wchar_t* temp = buffer;
	
			ConvertUTF16toUTF32(&source, source + sizebackup, &temp,  temp + size, lenientConversion);
	
			// Appending NUL to the converted buffer.
			*temp = 0;
			val.assign(buffer, size);

		}

		ConvertToForwardSlash(val);

		delete buff;
		return *this;
	}

#else

	Deserialiser& operator>>(std::wstring& val)
	{

	Cardinality card;
	*this >> card;
	TUint32 size = card;

	wchar_t* buff = 0;
	if (size & 0x01)
		{
		throw std::runtime_error("Decoding 8bit text into std::wstring");
		}
	else
		{
		size = size >> 1;
		buff = new wchar_t[(int)size];
		TUnicodeExpander exp;
		TMemoryUnicodeSink sink((TUint16*)buff);
		exp.ExpandL(sink, *this ,size);
		}
	val.assign(buff, size);
	delete buff;
	return *this;
	}
#endif

	void read(TUint8* aDst, TUint32 aCount)
	{
	iStream.read((char*)aDst, aCount);
	}


private:
	std::istream& iStream;
	};

#endif	/* DESERIALISER_H */
