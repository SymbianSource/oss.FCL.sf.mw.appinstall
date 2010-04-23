/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Unicode converter codes
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __UNICODE_CONVERTOR_H__
#define __UNICODE_CONVERTOR_H__
#if _MSC_VER <= 1200
#define FIX_VC6_BROKEN_FSTREAM
#endif

// The default C++ wide streams narrow all wchar_t to char before writing to disk!!!!
// This file defeats the narrowing, whilst still mapping NL <-> CR/NL.

// This code is hard coded to use little endian UTF-16.

// You probably want to #define FIX_VC6_BROKEN_FSTREAM then
// include this file, and do the following to create an output stream:-
//
//	std::wofstream out;
//	locale loc = _ADDFAC(locale::classic(), new NullCodecvt);
//
//	out.imbue(loc); 
//	out.open("c:\\test.txt", std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
//	out << wchar_t(0xfeff); // UTF-16 BOM - swapped by NullCondecvt to ff,fe == little endian
//
// Input streams should work as well, but the user will have to discard the BOM (if present).

#include <iostream>
#ifdef FIX_VC6_BROKEN_FSTREAM
#define fwrite(a,b,c,d) fwrite_hack(a,b,c,d)
#endif
#include <fstream>
#ifdef FIX_VC6_BROKEN_FSTREAM
#undef fwrite
static int fwrite_hack(const void *buffer, size_t size, size_t num, FILE *stream)
{
	// fstream _Fputc does a call of the form fwrite(p, N, 1, fs) == 1 for noconv data
	// fstream uses call of the form fwrite(p, N, 1, fs) == N for partial or ok data.
	// The second call is incorrect!
	// As a hack for VC6 we always say we converted (so _FPutc is not used),
	// and swap the arguments to fwrite...
	return fwrite(buffer, num, size, stream); // write num 1 byte elements
}
#endif

using namespace std;

typedef codecvt<wchar_t, char, mbstate_t> NullCodecvtBase;

// CLASS NullCodecvt
class NullCodecvt : public NullCodecvtBase
	{
public:
	explicit NullCodecvt(size_t _R = 0)
	: NullCodecvtBase(_R) {}

protected:
	virtual result do_in(mbstate_t& _State,
						 const char *fromStart, const char *fromEnd, const char *& fromNext,
						 wchar_t *toStart, wchar_t *toLimit, wchar_t *& toNext) const

	{		
		fromNext = fromStart;
		toNext = toStart;

		result r = noconv;

		int fromBytes = fromEnd - fromStart;
		int toWChars = toLimit - toStart;
		while((fromBytes >= 2) && (toWChars >= 1))
		{
			wchar_t wch = (fromNext[0] << 8) | fromNext[1];
			fromNext += 2;
			fromBytes -= 2;

			if(wch == '\r')
			{
				// Drop CR characters, so did at least partial conversion
				r = partial;
				continue;
			}

			*toNext++ = wch;
			--toWChars;
		}

		if(r == partial)
		{
			// Dropped some CR characters
			if(fromBytes == 0)
			{
				// and converted all input
				r = ok;
			}
		}
		return r;	
	}

	virtual result do_out(mbstate_t& _State,
						  const wchar_t *fromStart, const wchar_t *fromEnd, const wchar_t *& fromNext,
						  char *toStart, char *toLimit, char *& toNext) const
	{
		fromNext = fromStart;
		toNext = toStart;

		result r = noconv;
#ifdef FIX_VC6_BROKEN_FSTREAM
		r = partial;
#endif

		int fromWChars = fromEnd - fromStart;
		int toBytes = toLimit - toStart;
		while((fromWChars >= 1) && (toBytes >= 2))
		{
			if(*fromNext == '\r')
			{
				r = partial;
				// Do not expect CR internally, skip
				++fromNext;
				--fromWChars;
				continue;
			}

			if(*fromNext == '\n')
			{
				r = partial;
				if(toBytes < 4)
				{
					// Not enough space!!!!
					// Hopefully they will call us again with at least do_max_length() bytes (ie. 4)
					break;
				}
				*toNext++ = 0x0d; --toBytes; // 16 bit CR (little endian)
				*toNext++ = 0x00; --toBytes;
				*toNext++ = 0x0a; --toBytes; // 16 bit NL
				*toNext++ = 0x00; --toBytes;
				++fromNext;
				--fromWChars;
				continue;
			}
	
			*toNext++ = (*fromNext & 0x00ff); --toBytes; // little endian
			*toNext++ = (*fromNext & 0xff00) >> 8; --toBytes;
			++fromNext;
			--fromWChars;
			}
		
		if( (r==partial) && (fromWChars == 0))
		{
			r = ok;
		}
		return r;	
	}

	virtual result do_unshift(mbstate_t& _State,
							  char *_F2, char *_L2, char *& _Mid2) const

	{		return noconv;	}

	virtual int do_length(mbstate_t& _State, const char *from,
						  const char *fromEnd, size_t maxInternal) const _THROW0()
	{
		int conWChars = 0;
		const char *fromNext = from;
		while(((fromEnd-fromNext) >= 2) && (conWChars < maxInternal))
		{
			wchar_t wch = (fromNext[0] << 8) | fromNext[1];
			fromNext += 2;
		
			if(wch == '\r')
			{
				// Drop CR characters, so did at least partial conversion
				continue;
			}

			++conWChars;
		}
		return fromNext - from;
	}

	virtual bool do_always_noconv() const _THROW0()
	{	
		// Sometimes we do CRNL -> NL conversion
		return false;	
	}

	virtual int do_max_length() const _THROW0()
	{	
		// This is documented as the max number of external chars (bytes) that could
		// be consumed to create a single wchar_t.....
		// We convert 1 to 1, except for dropping CRs. This probably only, at worst,
		// converts 4 external bytes (16bit CR, 16bit NL) to a single 16 bit NL,
		// but we could consume infinite chars....
		// We will, at worst, convert a single wchar_t to 4 external chars (bytes)
		return 4;
	}

	virtual int do_encoding() const _THROW0()
	{		
		return 0; // Variable length encodings
	}

	}; 


#endif
