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
* @file
* This file provides the class declaration of CLogger.
* @internalComponent
*
*/

 

 
#ifndef _LOG_ENGINE_H_
#define _LOG_ENGINE_H_

#include <flogger.h>
#include <f32file.h>

class CLogger: public CBase
	{
public: // Public Constructors and destructors
	~CLogger();
	
	static CLogger* NewL(const TDesC& aLogFile);
	static CLogger* NewLC(const TDesC& aLogFile);

public: // Public Member Functions
	void WriteL(const TDesC16& aText);
	void WriteAndFlushL(const TDesC16& aText);
	void FlushLog();
	void UseDateAndTime(TBool aUseDate, TBool aUseTime);

private: // Private constructors
	CLogger();
	//ConstructL(const TDesC& aLogFile);
	void ConstructL(const TDesC& aLogFile);

private: // Private Member Functions
	void CreateLogDirL();
//	void GetDateTimeL(TDes& aDate, TDes& aTime);

private: // Private Data Members
	RFileLogger iLogger;
	RBuf		iInternalBuffer;
	TBool		iUseDate;
	TBool		iUseTime;
	};
#endif // _LOG_ENGINE_H_