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
* CLogger: This class provides the functionality of logging.
* @internalComponent
*
*/

 
 
#include "clogger.h"
#include <f32file.h>

_LIT(KLogDir, "\\logs\\swiconsole\\");
_LIT(KLogRelativePath, "swiconsole");

const TInt KInternalBufferSize = 256;

CLogger::CLogger():
			iUseDate(ETrue),
			iUseTime(ETrue)
	{
	}

CLogger* CLogger::NewL(const TDesC& aLogFile)
	{
	CLogger *self = NewLC(aLogFile);
	CleanupStack::Pop(self);
	
	return self;
	}

CLogger* CLogger::NewLC(const TDesC& aLogFile)
	{
	CLogger *self = new (ELeave) CLogger();
	CleanupStack::PushL(self);
	self->ConstructL(aLogFile);
	
	return self;
	}


void CLogger::ConstructL(const TDesC& aLogFile)
	{
	CreateLogDirL();
	TFileName tempName(aLogFile);
	TParse parseFileName;
	parseFileName.Set(tempName, NULL, NULL);
	
	User::LeaveIfError(iLogger.Connect());
	
	TFileName logFileName(KLogDir);
	logFileName.Append(parseFileName.NameAndExt());
	
	iLogger.CreateLog(KLogRelativePath, parseFileName.NameAndExt(), EFileLoggingModeAppend);
	
	iInternalBuffer.Create(KInternalBufferSize);
	}

CLogger::~CLogger()
	{
	FlushLog();
	iLogger.CloseLog();
	iLogger.Close();
	iInternalBuffer.Close();
	}


void CLogger::WriteL(const TDesC16& aText)
	{
	TInt finalLength = aText.Length() + iInternalBuffer.Length();
	if(iInternalBuffer.MaxLength() < finalLength)
		{
		iInternalBuffer.ReAllocL(finalLength);
		}
	iInternalBuffer.Append(aText);
	}
	
void CLogger::WriteAndFlushL(const TDesC16& aText)
	{
	WriteL(aText);
	FlushLog();
	}

void CLogger::FlushLog()
	{
	iLogger.Write(iInternalBuffer);
	iInternalBuffer.Zero();
	}

void CLogger::CreateLogDirL()
	{
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	TInt retValue = fileSession.MkDirAll(KLogDir);
	if(KErrNone != retValue && KErrAlreadyExists != retValue)
		{
		User::LeaveIfError(retValue);
		}
	CleanupStack::PopAndDestroy(&fileSession);//fileSession.Close();
	}

void CLogger::UseDateAndTime(TBool aUseDate, TBool aUseTime)
	{
	iLogger.SetDateAndTime(aUseDate, aUseTime);
	}

