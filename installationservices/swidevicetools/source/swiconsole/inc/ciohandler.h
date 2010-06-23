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
* This class provides a common interface for handling I/O operations
* for CConsoleBase class and also for CLogger (which writes the logs)
* @internalComponent
*
*/

 

#ifndef __CIOHANDLER_H__
#define __CIOHANDLER_H__

#include <e32base.h>
#include <f32file.h>
#include <bautils.h>
#include <barsc.h>

#include <swiconsole.rsg>
// Forward declarations
class CLogger;
class CPreferenceHandler;

// Literals
_LIT(KLineBreaker, "\r\n");
_LIT(KStringLine, "----------------------------------------");
const TInt KDefaultStringLen = 100;
const TInt KDateStringLen = 30;
const TInt KIntStringLen = 10;
const TInt KIntArrayStringLen = 256;


// Enums
enum TIoType
	{
	EIoConsoleType,
	EIoLogType,
	EIoBoth
	};


class CIoHandler: public CBase
	{
public: // Public static Member Functions
	static CIoHandler* NewL();
	static CIoHandler* NewLC();
	
public: // Destructor
	~CIoHandler();
	
public: // Public member functions
	/**
	Writes the given string to the specified stream
	@param aString string which needs to be written
	@param aIoType This enum tells where to write the string
	*/
	void WriteL(const TDesC& aString, TIoType aIoType = EIoBoth) const;

	/**
	Writes the given string to the specified stream
	@param aString string which needs to be written
	@param aIoType This enum tells where to write the string
	*/
	void WriteLineL(const TDesC& aString, TIoType aIoType = EIoBoth) const;

	/**
	Writes the given string to the default console. When a page boundary
	is hit it will pause for user input then then next line will be shown 
	in a new page.
	@param aString string which needs to be written
	@param aInsertLineBreak To insert a line break after the string
	*/
	void WriteToPageL(const TDesC& aString, TBool aInsertLineBreak = ETrue) const;

	/**
	Writes the string represented by the resource ID to the specified stream
	
	@param aStringUid This is the resource id of the string.
	@param aIoType This enum tells where to write the string
	*/
	void WriteL(TInt aStringUid, TIoType aType = EIoBoth) const;

	/**
	Writes the string represented by the resource ID to the specified stream
	
	@param aStringUid This is the resource id of the string.
	@param aIoType This enum tells where to write the string
	*/
	void WriteLineL(TInt aStringUid, TIoType aType = EIoBoth) const;

	/**
	Writes the given string to the default console. When a page boundary
	is hit it will pause for user input then then next line will be shown 
	in a new page.
	@param aStringUid This is the resource id of the string.
	@param aInsertLineBreak To insert a line break after the string
	*/
	void WriteToPageL(TInt aStringUid, TBool aInsertLineBreak = ETrue) const;

	/**
	Writes the given string to the specified stream
	@param aDes string which needs to be written
	@param aIoType This enum tells where to write the string
	*/
	void WriteL(const TDateTime&, TIoType = EIoConsoleType) const;

	/**
	Writes the given string to the specified stream
	@param aTime data and time to be written
	@param aIoType This enum tells where to write the string
	*/
	void WriteLineL(const TDateTime& aDateTime, TIoType aIoType = EIoConsoleType) const;

	/**
	This function displays the error message on the console and pauses for
	user input. After user presses any key the cursor is adjusted to the
	original position.
	@param aString string which needs to be written
	*/
	void WriteErrorMsgL(const TDesC& aString) const;

	/**
	This function displays the error message on the console and pauses for
	user input. After user presses any key the cursor is adjusted to the
	original position.
	
	@param aStringUid This is the resource id of the string.
	*/
	void WriteErrorMsgL(TInt aStringUid) const;

	/**
	Retrieves the string from the resource file given the Resource ID
	
	@param aString After reading the resource file the string will be 
				filled in this descriptor.
	@param aStringUid This is the resource id of the string.
	*/
	void GetStringFromResourceL(TDes& aString, TInt aStringUid) const;

	/**
	Retrieves the string from the resource file given the Resource ID
	
	@param aStringUid This is the resource id of the string.
	@return HBufC* Returns poiner to the string.
	*/
	HBufC* GetStringFromResourceLC(TInt aStringUid) const;
	
	/**
	Reads string from the console
	
	@param aString This descriptor will hold the data read from the console.
	*/
	void ReadStringL(TDes& aString);

	/**
	Reads string from the console. This function can take infinte length of
	data as input.
	
	@param aString This descriptor will hold the data read from the console.
	*/
	void ReadStringL(RBuf& aString);

	/**
	Reads an integer from the console
	
	@param aValue This integer will hold the value read from the console.
	@return This function returns KErrNone on success and KErrGeneral on 
	failure
	*/
	TInt ReadIntL(TInt& aValue);
	
	/**
	Reads an array of integer from the console
	
	@param aIntArray This array will hold all the integer value read from the 
	console.
	@param aAllowDuplicates If this variable is true than duplicate values are 
	allowed else duplicates won't be added into the array
	*/
	void ReadIntArrayL(RArray<TInt>& aIntArray, TBool aAllowDuplicates = EFalse);
	
	/**
	This function enables logging and set the log file name
	
	@param aLogFile Log filename
	*/
	void InitLoggingL(TDesC& aLogFile);
	
	/**
	Enables/Disables logging
	
	@param aEnableLogging This variables decides whether to enable logging 
	or not
	*/
	void SetLoggingState(TBool aEnableLogging);
	
	/**
	Clear the console screen
	
	*/
	void ClearConsoleL() const;

	/**
	Clear the line on the console
	
	*/
	void ClearLine() const;

	/**
	Clear the console screen
	
	*/
	void PauseScreenL() const;
	
	/**
	This function is used to initialize progress bar and to set its final
	value
	
	@param aFinalValue This parameter provides the final value of progress bar
	*/
	
	void SetFinalProgressValueL(TInt aFinalValue);
	
	/**
	This function is used to update progress bar
	
	@param aUpdateValue This parameter provides the update value in 
						progress bar
	*/
	void UpdateProgressBar(TInt aUpdateValue);
	
	/**
	This function is used to  set preferences
	
	@param aPreferences Pointer to preferences;
	*/
	void SetPreferenceHandler(CPreferenceHandler* aPreferenceHandler);
	
	/**
	This function checks that given number of lines can be displayed in a 
	single screen
	
	@param aLineCount Number of lines which needs to be displayed;
	@return Whether aLineCount lines can be displayed in a single screen 
	or not
	*/
	TBool IsDisplayableInPage(TInt aLineCount);

private:
	/**
	Reads string from the console. 
	
	@param aString This descriptor will hold the data read from the console.
	@param aLimitedLength If this variable is true then it will allow only
	limited length of data from the console.
	*/
	TBool ReadStringFromConsoleL(TDes& aString, TBool aLimitedLength = ETrue);

	/**
	This function is used to update progress bar
	
	@param aString This descriptor will hold the data to be displayed in
	console.
	*/
	void WriteToConsoleL(const TDesC& aString) const;

	/**
	This function is used to update progress bar
	*/
	void RedrawProgressBarL() const;
	
	/**
	This function is used to update progress bar
	
	@return This function returns the length of the progress bar
	*/
	TInt ProgressBarLength() const;

	/**
	This inline function checks whether a character is a readable character
	or not.
	
	@param aKey Character which needs to be checked.
	@return returns ETrue if it is a readable character else EFalse
	*/
	inline TBool IsCharacterKey(TKeyCode aKey);

private: // Private constructors
	// Private Constructor
	CIoHandler();
	// Private Copy Constructor
	CIoHandler(const CIoHandler&){}
	// Second phase constructor
	void ConstructL();

private: // Private Member Variables
	CPreferenceHandler*	iPreferenceHandler;
	RFs					iFileSession;
	RResourceFile 		iResourceFile;
	CConsoleBase*		iConsole;
	CLogger*			iLogger;
	TBool				iIsLoggingEnabled;
	TBool				iIsProgressBarEnabled;
	TInt				iFinalProgressValue;
	TInt				iCurProgressValue;
	TInt				iProgressStartPos;
	TInt				iProgressCursor;
	TInt				iProgressBarLen;
	};
	
	

// Inline function definitions
inline TBool CIoHandler::IsCharacterKey(TKeyCode aKey)
	{
	if(aKey > EKeyEscape && aKey < EKeyDelete)
		{
		return ETrue;
		}
	
	return EFalse;
	}


#endif //__CIOHANDLER_H__