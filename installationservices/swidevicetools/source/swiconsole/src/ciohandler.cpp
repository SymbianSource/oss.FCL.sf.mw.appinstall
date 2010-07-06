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
* cpackageinfo.cpp
* This file provides implementation for CIoHandler class
* CIoHandler: This class handles all the I/O activities like reading from
* console, writing to a log file, etc.
* @internalComponent
*
*/

#include <e32cons.h> 		// ConsoleBase

// User includes
#include "ciohandler.h"
#include "clogger.h"
#include "cpreferencehandler.h"
#include "swiconsoleerrors.h"

// Constants and literals
const TInt KProgressBarFactor	= 3;
const TInt KCursorOffset		= 2;
const TInt KProgressOffset		= 1;
const TInt KDateAdjustment		= 1;
const TInt KAllocationBlock		= 50;

_LIT(KCurProgressSymbol, "#");
_LIT(KCompleteProgressSymbol, "-");
_LIT(KDateFormatString, "%d/%d/%d");
_LIT(KConsoleName, "SwiConsole");


CIoHandler::CIoHandler():
			iIsLoggingEnabled(EFalse),
			iIsProgressBarEnabled(EFalse)
	{
	}

CIoHandler::~CIoHandler()
	{
	iResourceFile.Close();
	iFileSession.Close();
	delete iLogger;
	delete iConsole;
	}

// Second phase constructor
void CIoHandler::ConstructL()
	{
	iConsole = Console::NewL(KConsoleName,TSize(KConsFullScreen,KConsFullScreen));
	iProgressBarLen = iConsole->ScreenSize().iWidth;
	iProgressBarLen -= iProgressBarLen/KProgressBarFactor;

	User::LeaveIfError(iFileSession.Connect()); 
	_LIT(KResourceFile, "swiconsole.rsc"); 
	TFileName privatePath;
	iFileSession.PrivatePath(privatePath);
	
	TFileName resourceFilename(_L("Z:"));
	resourceFilename.Append(privatePath);
	resourceFilename.Append(KResourceFile());
	
	iResourceFile.OpenL(iFileSession, resourceFilename);
	}

CIoHandler* CIoHandler::NewL()
	{
	CIoHandler* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CIoHandler* CIoHandler::NewLC()
	{
	CIoHandler* self = new (ELeave) CIoHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

HBufC* CIoHandler::GetStringFromResourceLC(TInt aStringUid) const
	{
	HBufC8* dataBuffer = iResourceFile.AllocReadLC(aStringUid);

    TResourceReader resourceReader;
    resourceReader.SetBuffer(dataBuffer);
    
    TInt bufferLength = dataBuffer->Length();

	HBufC* stringBuffer = HBufC::NewL(bufferLength);
	TPtr ptrString = stringBuffer->Des();
	ptrString.Copy(resourceReader.ReadTPtrC());

    // clean up data buffer
    CleanupStack::PopAndDestroy(); // finished with dataBuffer
    CleanupStack::PushL(stringBuffer);
    
    return stringBuffer;
	}

	
void CIoHandler::GetStringFromResourceL(TDes& aString, TInt aStringUid) const
	{
	HBufC8* dataBuffer = iResourceFile.AllocReadLC(aStringUid);

    TResourceReader resourceReader;
    resourceReader.SetBuffer(dataBuffer);
    
    __ASSERT_DEBUG(resourceReader.ReadTPtrC().Length() < aString.MaxLength(), 
    				User::Panic(KGeneralPanicString, KInvalidArgument));
    
	aString.Copy(resourceReader.ReadTPtrC());

    // clean up data buffer
    CleanupStack::PopAndDestroy(); // finished with dataBuffer
	}

void CIoHandler::WriteToConsoleL(const TDesC& aString) const
	{
	if(	iIsProgressBarEnabled)
		{
		// This piece of code is used to redraw progress bar
		TSize screenSize = iConsole->ScreenSize();
		
		if(iConsole->WhereY() == screenSize.iHeight - KProgressOffset)
			{
			// Clear the current progress bar
			ClearLine();
			// write the string
			iConsole->Printf(aString);
			// Save the cursor position
			TPoint curPos = iConsole->CursorPos();

			// If the string passed is not a line breaker
			// then add line breaker
			if(aString.Compare(KLineBreaker))
				{
				iConsole->Printf(KLineBreaker);
				--curPos.iY;
				}
			
			RedrawProgressBarL();
			iConsole->SetPos(curPos.iX, curPos.iY);
			return;
			}
		}
	iConsole->Printf(aString);
	}	


void CIoHandler::WriteL(const TDesC& aString, TIoType aIoType) const
	{
	switch(aIoType)
		{
		case EIoBoth:
		case EIoConsoleType:
			{
			WriteToConsoleL(aString);
			if(EIoBoth != aIoType)
				{
				break;
				}
			}
		case EIoLogType:
			{
			if(!iIsLoggingEnabled)
				{
				break;
				}
			if(!aString.Compare(KLineBreaker))
				{
				iLogger->FlushLog();
				break;
				}
			iLogger->WriteL(aString);
			break;
			}
		default:
			{
			User::Leave(KSwiInvalidSwitchCase);
			}
		}
	}

void CIoHandler::WriteLineL(const TDesC& aString, TIoType aIoType) const
	{
	switch(aIoType)
		{
		case EIoBoth:
		case EIoConsoleType:
			{
			WriteToConsoleL(aString);
			WriteToConsoleL(KLineBreaker);
			if(EIoBoth != aIoType)
				{
				break;
				}
			}
		case EIoLogType:
			{
			if(!iIsLoggingEnabled)
				{
				break;
				}
			iLogger->WriteAndFlushL(aString);
			break;
			}
		default:
			{
			User::Leave(KSwiInvalidSwitchCase);
			}
		}
	}
	
void CIoHandler::WriteToPageL(const TDesC& aString, TBool aInsertLineBreak) const
	{
	TSize screenSize = iConsole->ScreenSize();
	if(iConsole->WhereY() >= screenSize.iHeight - KCursorOffset)
		{
		iConsole->Printf(KLineBreaker);
		if(!iPreferenceHandler || ESwiNormalMode == iPreferenceHandler->GetOperationMode())
			{
			PauseScreenL();
			}
		ClearConsoleL();
		}
	WriteToConsoleL(aString);
	if(aInsertLineBreak)
		{
		WriteToConsoleL(KLineBreaker);
		}
	}


void CIoHandler::WriteL(TInt aResourceID, TIoType aIoType) const
	{
	HBufC *string = GetStringFromResourceLC(aResourceID);
	WriteL(*string, aIoType);
	CleanupStack::PopAndDestroy(string);
	}
	
void CIoHandler::WriteLineL(TInt aResourceID, TIoType aIoType) const
	{
	HBufC *string = GetStringFromResourceLC(aResourceID);
	WriteLineL(*string, aIoType);
	CleanupStack::PopAndDestroy(string);
	}

void CIoHandler::WriteToPageL(TInt aResourceID, TBool aInsertLineBreak) const
	{
	TSize screenSize = iConsole->ScreenSize();
	if(iConsole->WhereY() >= screenSize.iHeight - KCursorOffset)
		{
		if(!iPreferenceHandler || ESwiNormalMode == iPreferenceHandler->GetOperationMode())
			{
			PauseScreenL();
			}
		ClearConsoleL();
		}
	HBufC *string = GetStringFromResourceLC(aResourceID);
	WriteToConsoleL(*string);
	if(aInsertLineBreak)
		{
		WriteToConsoleL(KLineBreaker);
		}
	CleanupStack::PopAndDestroy(string);
	}


void CIoHandler::WriteL(const TDateTime& aDate, TIoType aIoType) const
	{
	TBuf<KDateStringLen> dateString;
	dateString.Format(KDateFormatString, aDate.Day() + KDateAdjustment, aDate.Month() + KDateAdjustment, aDate.Year());
	WriteL(dateString, aIoType);
	}

void CIoHandler::WriteLineL(const TDateTime& aDate, TIoType aIoType) const
	{
	TBuf<KDateStringLen> dateString;
	dateString.Format(KDateFormatString, aDate.Day() + KDateAdjustment, aDate.Month() + KDateAdjustment, aDate.Year());
	WriteLineL(dateString, aIoType);
	}

void CIoHandler::WriteErrorMsgL(const TDesC& aString) const
	{
	ClearLine();
	iConsole->Printf(aString);
	TInt initialPos = iConsole->WhereY();
	iConsole->Printf(KLineBreaker);
	TInt finalPos = iConsole->WhereY();
	ClearLine();
	HBufC *string = GetStringFromResourceLC(R_PRESS_ANY_KEY_MSG);
	iConsole->Printf(*string);
	CleanupStack::PopAndDestroy(string);
	iConsole->Getch();
	if(initialPos == finalPos)
		{
		--initialPos;
		}
	ClearLine();
	RedrawProgressBarL();
	iConsole->SetPos(0, initialPos);
	ClearLine();
	}

void CIoHandler::WriteErrorMsgL(TInt aResourceID) const
	{
	HBufC *string = GetStringFromResourceLC(aResourceID);
	WriteErrorMsgL(*string);
	CleanupStack::PopAndDestroy(string);
	}

TBool CIoHandler::ReadStringFromConsoleL(TDes& aString, TBool aLimitedLength)
	{
	// This infinte loop terminates when user hits an "enter" key
	// Or the maximum size of the buffer is hit
	FOREVER
		{
		// Get a key(character) from the console
		TKeyCode ch = iConsole->Getch();
		
		switch(ch)
			{
			case EKeyEnter:
				{
				return EFalse;
				}
			case EKeyBackspace:
				{
				if(0 != aString.Length())
					{
					// Back-space only takes the cursor one position back
					// So to delete a character blank-space is inserted at
					// that position and later cursor is again adjusted.
					iConsole->Printf(_L("%c%c%c"), EKeyBackspace, 
												   EKeySpace, 
												   EKeyBackspace);
					// Delete the character from the target string also. 
					aString.Delete(aString.Length() - 1, 1);
					}
				break;
				}
			case EKeyEscape:
				{
					User::Leave(KErrCancel);
				}
			default:
				{
				// IsCharacterKey will return true if ch is a displayable
				// character else it will return false.
				if(IsCharacterKey(ch))
					{
					TInt maxBufferLength = aString.MaxLength();
					if(aString.Length() == maxBufferLength)
						{
						continue;
						}
					iConsole->Printf(_L("%c"), ch);
					aString.Append(ch);
					if(aString.Length() == maxBufferLength && !aLimitedLength)
						{
						return ETrue;
						}
					}
				}
			}
		}
	}

void CIoHandler::ReadStringL(TDes& aString)
	{
	aString.Zero();
	ReadStringFromConsoleL(aString);
	}


void CIoHandler::ReadStringL(RBuf& aString)
	{
	aString.Zero();
	while(ReadStringFromConsoleL(aString, EFalse))
		{
		aString.ReAllocL(aString.MaxLength() + KAllocationBlock);
		}
	}
	
TInt CIoHandler::ReadIntL(TInt& aValue)
	{
	TBuf<KIntStringLen> string;
	ReadStringL(string);
	TLex lexObj(string);
	return lexObj.Val(aValue);
	}
	
void CIoHandler::ReadIntArrayL(RArray<TInt>& aIntArr, TBool aAllowDuplicates)
	{
	RBuf string;
	string.Create(KIntArrayStringLen);
	CleanupClosePushL(string);
	ReadStringL(string);
	string.TrimLeft();
	
	if(0 == string.Length())
		{
		return;
		}
	
	TBool bFlag = ETrue;
	
	do
		{
		TInt pos = string.Find(_L(" "));

		if(KErrNotFound == pos)
			{
			pos = string.Length();
			bFlag = EFalse;
			}

		TInt number = 0;
		TLex lexObj(string);
		User::LeaveIfError(lexObj.Val(number));

		if(aAllowDuplicates)
			{
			aIntArr.AppendL(number);
			}
		else
			{
			aIntArr.InsertInOrderL(number);
			}
			
		string.Delete(0, pos);
		
		string.TrimLeft();
		}while(bFlag && string.Length() > 0);
		
	CleanupStack::PopAndDestroy(&string);
	}
	
void CIoHandler::InitLoggingL(TDesC& aLogFile)
	{
	__ASSERT_DEBUG(!iIsLoggingEnabled, User::Panic(KGeneralPanicString, KErrArgument));
	iIsLoggingEnabled = ETrue;
	iLogger = CLogger::NewL(aLogFile);
	
	HBufC *string = GetStringFromResourceLC(R_LOG_LINE_MSG);
	iLogger->WriteAndFlushL(*string);
	CleanupStack::PopAndDestroy(string);
	}
	
void CIoHandler::SetLoggingState(TBool aEnableLogging)
	{
	iIsLoggingEnabled = aEnableLogging;
	}

void CIoHandler::ClearConsoleL() const
	{
	iConsole->ClearScreen();
	RedrawProgressBarL();
	}

void CIoHandler::ClearLine() const
	{
	iConsole->SetPos(0);
	iConsole->ClearToEndOfLine();
	}
	
	
void CIoHandler::PauseScreenL() const
	{
	HBufC *string = GetStringFromResourceLC(R_PRESS_ANY_KEY_MSG);
	WriteL(*string, EIoConsoleType);
	CleanupStack::PopAndDestroy(string);
	iConsole->Getch();
	iConsole->Printf(KLineBreaker);
	}

TInt CIoHandler::ProgressBarLength() const
	{
	return (iCurProgressValue * iProgressBarLen)/iFinalProgressValue;
	}
void CIoHandler::SetFinalProgressValueL(TInt aFinalValue)
	{
	if(0 == aFinalValue)
		{
		return;
		}
	iFinalProgressValue = aFinalValue;
	iCurProgressValue = iProgressCursor = 0;
	iIsProgressBarEnabled = ETrue;
	
	TPoint curPos = iConsole->CursorPos();
	TSize screenSize = iConsole->ScreenSize();
	
	iConsole->SetPos(0, screenSize.iHeight - KProgressOffset);

	HBufC *string = GetStringFromResourceLC(R_PROGRESS_INFO);
	iConsole->Printf(*string);
	CleanupStack::PopAndDestroy(string);
	
	iProgressStartPos = iConsole->WhereX();
	for(TInt i = iProgressBarLen; i >= 0; --i)
		{
		iConsole->Printf(KCompleteProgressSymbol);
		}
	
	iConsole->SetPos(curPos.iX, curPos.iY);
	}

void CIoHandler::UpdateProgressBar(TInt aUpdateValue)
	{
	if(!iIsProgressBarEnabled)
		{
		return;
		}
	TPoint curPos = iConsole->CursorPos();
	TSize screenSize = iConsole->ScreenSize();
	iCurProgressValue += aUpdateValue;
	TInt progessBarLen = ProgressBarLength();
	if(progessBarLen > iProgressCursor)
		{
		iConsole->SetPos(iProgressStartPos + iProgressCursor, screenSize.iHeight - KProgressOffset);
		for(TInt i = progessBarLen - iProgressCursor; i > 0; --i)
			{
			iConsole->Printf(KCurProgressSymbol);
			}
		iProgressCursor = progessBarLen;	
		}
	iConsole->SetPos(curPos.iX, curPos.iY);
	}

void CIoHandler::RedrawProgressBarL() const
	{
	if(!iIsProgressBarEnabled)
		{
		return;
		}
	TSize screenSize = iConsole->ScreenSize();
	iConsole->SetPos(0, screenSize.iHeight - KProgressOffset);

	HBufC *string = GetStringFromResourceLC(R_PROGRESS_INFO);
	iConsole->Printf(*string);
	CleanupStack::PopAndDestroy(string);
	
	TInt progessBarLen = ProgressBarLength();

	for(TInt i = 0; i < iProgressBarLen; ++i)
		{
		if(i < progessBarLen)
			{
			iConsole->Printf(KCurProgressSymbol);
			}
		else
			{
			iConsole->Printf(KCompleteProgressSymbol);
			}
		}
	}

void CIoHandler::SetPreferenceHandler(CPreferenceHandler* aPreferenceHandler)
	{
	iPreferenceHandler = aPreferenceHandler;
	}

TBool CIoHandler::IsDisplayableInPage(TInt aLineCount)
	{
	if(iPreferenceHandler && ESwiNormalMode != iPreferenceHandler->GetOperationMode() )
		{
		return ETrue;
		}
	TSize screenSize = iConsole->ScreenSize();
	if(iConsole->WhereY() + aLineCount >= screenSize.iHeight - KCursorOffset)
		{
		return EFalse;
		}
		
	return ETrue; 
	}


