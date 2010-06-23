/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Print functions used in debug mode 
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __SWI_LOG_H__
#define __SWI_LOG_H__

#include <e32debug.h>

class TTruncateOverflowHandler16 : public TDes16Overflow
	{
	public:
		virtual void Overflow(TDes16& aDes);
	};
	
inline void TTruncateOverflowHandler16::Overflow(TDes16&)
	{

	}
	
class TTruncateOverflowHandler8 : public TDes8Overflow
	{
	public:
		virtual void Overflow(TDes8& aDes);
	};
	
inline void TTruncateOverflowHandler8::Overflow(TDes8&)
	{
   
	}

namespace Swi
{

#ifdef _DEBUG

#define DEBUG_PRINTF(a) {Swi::DebugPrintf(__LINE__, __FILE__, a);}
#define DEBUG_PRINTF2(a, b) {Swi::DebugPrintf(__LINE__, __FILE__, a, b);}
#define DEBUG_PRINTF3(a, b, c) {Swi::DebugPrintf(__LINE__, __FILE__, a, b, c);}
#define DEBUG_PRINTF4(a, b, c, d) {Swi::DebugPrintf(__LINE__, __FILE__, a, b, c, d);}
#define DEBUG_PRINTF5(a, b, c, d, e) {Swi::DebugPrintf(__LINE__, __FILE__, a, b, c, d, e);}

#define DEBUG_CODE_SECTION(a) TRAP_IGNORE({ a; }) 

// UTF-8 overload of the DebufPrintf method. Should be used by default,
// since it's cheaper both in CPU cycles and stack space.

inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC8> aFormat, ...)
	{
	TTruncateOverflowHandler8 overflowHandler8;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<512> buffer;
	_LIT8(KSwiLogPrefix, "[SWI] ");
	_LIT8(KSwiLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	buffer.Append(KSwiLogPrefix);
	buffer.AppendFormat(KSwiLineFileFormat, now.Int64(), aLine, aFile);
	buffer.AppendFormatList(aFormat, list, &overflowHandler8);
	_LIT8(KSwiMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KSwiMsgEnd().Length()))
		{
		buffer.Append(KSwiMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}
	
// Unicode DebufPrintf overload

inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC16> aFormat, ...)
	{
	TTruncateOverflowHandler16 overflowHandler16;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<256> header;
	_LIT8(KSwiLogPrefix, "[SWI] ");
	_LIT8(KSwiLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	header.Append(KSwiLogPrefix);
	header.AppendFormat(KSwiLineFileFormat, now.Int64(), aLine, aFile);
	
	TBuf<256> buffer;
	buffer.Copy(header);
	buffer.AppendFormatList(aFormat, list , &overflowHandler16);
	_LIT(KSwiMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KSwiMsgEnd().Length()))
		{
		buffer.Append(KSwiMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}

#else

#define DEBUG_PRINTF(a)
#define DEBUG_PRINTF2(a, b)
#define DEBUG_PRINTF3(a, b, c)
#define DEBUG_PRINTF4(a, b, c, d)
#define DEBUG_PRINTF5(a, b, c, d, e)

#define DEBUG_CODE_SECTION(a)

#endif


} // namespace Swi

#endif // __SWI_LOG_H__
