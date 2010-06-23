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
* log.h
*
*/


#ifndef __SWI_LOG_H__
#define __SWI_LOG_H__

#if  defined(_DEBUG) && defined(__SWI_LOGGING__)
#define __FLOGGING__
#endif

#include <e32std.h>
#include <f32file.h>
#include <flogger.h>

namespace Swi
{

_LIT(KLogFileName, "swi.log");
_LIT(KLogDirName, "swi");
_LIT(KCustomLogName, "\\swi.log");

#ifdef __FLOGGING__

#define FLOG(a) {Swi::FPrint(a);}
#define FTRACE(a) {a;}

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<32> logFile(KLogFileName);
	RFileLogger::WriteFormat(KLogDirName, logFile, EFileLoggingModeAppend, 
	aFmt,list);
	VA_END(list);
	}

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    TBuf<32> logFile(KLogFileName);
    RFileLogger::HexDump(KLogDirName, logFile, EFileLoggingModeAppend, 
    	0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

#else
#ifdef __SWI_LOGGING2__ // custom logging

	#define FLOG(a) {Swi::FPrint(a);}
	#define FTRACE(a) {a;}

inline void FPrint(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	
	TBuf<2048> buffer;
	buffer.FormatList(aFmt, list);

	TPtrC8 bufferPtr;
	bufferPtr.Set(reinterpret_cast<const TUint8*>(buffer.Ptr()), buffer.Length()*2);

	RFs fs;
	if (KErrNone!=fs.Connect())
		{
		return;
		}
	
	RFile file;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> customLogName (sysDrive.Name());
	customLogName.Append(KCustomLogName);
	
	TInt err=file.Open(fs, customLogName, EFileWrite);
	if (KErrNone!=err)
		{
		TInt err=file.Create(fs, customLogName, EFileWrite|EFileStream);
		
		if (err==KErrNone)
			{
			_LIT8(KUnicodeHeader,"\xff\xfe");
			file.Write(KUnicodeHeader);
			}
		else
			{
			fs.Close();
			return;
			}
		}
	else
		{
		TInt offset=0;
		file.Seek(ESeekEnd, offset);
		}
	
	file.Write(bufferPtr);
	
	_LIT8(KCr,"\x0d\x00\x0a\x00");
	file.Write(KCr());
	
	file.Close();
	fs.Close();
	}

inline void FHex(const TUint8* aPtr, TInt aLen)
	{
	const TInt buflen = 2048;
	TBuf<buflen> buffer2;
	
	for (TInt i = 0 ; i < aLen ; ++i)
	    {
		if (buffer2.Length() == buflen)
			{
			FPrint(buffer2);
			buffer2.SetLength(0);
			}
		buffer2.AppendNumFixedWidthUC(aPtr[i], EHex, 2);
	    }
	FPrint(buffer2);
	}

inline void FHex(const TDesC8& aDes)
	{
	FHex(aDes.Ptr(), aDes.Length());
	}

#else // no logging
	#define FLOG(a)
	#define FTRACE(a)

	inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/,...) 
	    {
	    }

	inline void FHex(const TUint8* /*aPtr*/, TInt /*aLen*/)
	    {
	    }

	inline void FHex(const TDesC8& /*aDes*/)
	    {
	    }

#endif
#endif //   __FLOGGING__

} // namespace Swi

#endif // __SWI_LOG_H__
