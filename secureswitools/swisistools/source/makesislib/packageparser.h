/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* handles parsing of PKG file
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __PACKAGE_PARSER_H_
#define __PACKAGE_PARSER_H_

// ===========================================================================
// INCLUDES
// ===========================================================================
#include <iostream>
#include <string>
#include <map>

#include "utils.h"
#include "utility_interface.h"
#include "sisexpression.h"
#include "sisinstallblock.h"
#include "basetype.h"
#include "siswrite.h"
#include "sislanguage.h"

// ===========================================================================
// CONSTANTS
// ===========================================================================

// maximum length of strings used in condition expressions
#define MAX_STRING 255

#define MAX_FUNC_ARGS 2

// Error codes for possible parser failures
enum TParseException
	{
	ErrUnknownLine,
	ErrHeaderAlreadyDefined,
	ErrLanguagesAlreadyDefined,
	ErrLanguagesNotDefined,
	ErrUnknownLanguagesId,
	ErrHeaderNotDefined,
	ErrNoMainDependency,
	ErrFileNotFound,
	ErrPackageNotASISFile,
	ErrBadCondFormat,
	ErrReadFailed,
	ErrBadOption,
	ErrBadString,
	ErrUnexpectedToken,
	ErrInvalidEscape,
	ErrNumberOutOfRange,
	ErrUnknownVariable,
	ErrUninstallNeedsWaitEnd,
	ErrUIDMismatch,
	ErrCannotGetFullPath,
	ErrBadDestinationPath,
	ErrDuplicateUID,
	ErrExceedVersionNumber,
	ErrUnBalancedParenthesis,
	ErrHeaderRejectPreInstalledNonRemovable,
	ErrInvalidInstallFlagOption,
	ErrVersionInvalidRelationalOperator,
	ErrVersionWildcardsNotSupported,
	ErrVersionNegativesNotSupported,
	ErrBadIntegerToStringConversion
	};

/**
 * Error codes for possible interpret SIS failures.
 */
enum TInterpretSisException
	{
	ErrInvalidAppType,
	ErrEmbeddedSisNotSupported,
	ErrInvalidInstallOptions,
	ErrUserOptionsNotSupported,
	ErrHalValuesNotSupported
	};

typedef int TOKEN;

// ===========================================================================
// INTERNAL DATA STRUCTURES CONSTRUCTED WHEN PARSING PKG FILE
// ===========================================================================
/**
 * @internalComponent
 * @released
 */
union VARIANTVAL
// a variant, i.e. either a number or string value
	{
	LONG dwNumber;					// numeric value, e.g. 100
	WCHAR pszString[MAX_STRING];	// string value, e.g. "crystal"
	};

#define MAXOPTLEN 20

struct SParseOpt
	{
	wchar_t pszOpt[MAXOPTLEN];
	DWORD dwOpt;
	};

// ===========================================================================
// CLASS DEFINITIONS
// ===========================================================================

class MParserObserver
// Callback mixin - allows disemination of progress data
	{
public:
	/**
	 * Output message
	 * @param aText Message to display
	 */
	virtual void DoMsg(const wchar_t* aText) const = 0;
	/**
	 * Output 'verbose' messages
	 * @param Message to display
	 */
	virtual void DoVerbage(const wchar_t* aText) const = 0;
	/**
	 * Output error messages
	 * @param Message to display
	 */
	virtual void DoErrMsg(const wchar_t* aText) const = 0;
	/**
	 * Is verbose output required?
	 * @return true if verbose output required else false.
	 */
	virtual bool Verbose() const = 0;
	/**
	 * Reset the verbosity status
	 * @param aVerbose true to which on verbose or else false. 
	 */
	virtual bool SetVerbose(bool aVerbose) = 0;
	/**
	 * Set the Line number which is being processed
	 * @param aLineNumber new line number
	 */
	virtual void SetLineNumber(int aLineNumber) = 0;
	/**
	 * Add error message for use later on
	 * @param aError Error code
	 */
	virtual void AddInterpretSisError(TInterpretSisException aError) = 0; 
	};

class CInterpretSisHelper;

class CSISFileDescription;
typedef std::map<const std::wstring, CSISFileDescription> TDuplicates;
typedef TDuplicates::const_iterator TDuplicatesConstIter;

class CPackageParser
	{
public:
	/**
	 * Constructor
	 * @param aObserver - observer to parsing messages. UI implementor needs
	 * 			to implement this to get the parsing details.
	 */
	CPackageParser(MParserObserver& aObserver);
	/**
	 * The function parses the package file and creates a sis file.
	 * In case of invalid package file the function throws exception. 
	 * See TParseException for details.
	 * If aReportInterpretSisError variable it true then this function 
	 * will Report whether the interpretsis would not be able to provision 
	 * this file. For details about errors refer TInterpretSisException.
	 * 
	 * @param aPkgFileName pkg file name
	 * @param aSisFileName output sis file name
	 * @param aMakeStub boolean which tells whether to create stub or not.
	 * @param aReportInterpretSisError whether to report interpretsis failure or not.
	 */
	void MakeSisL(const wchar_t* aPkgFileName, const wchar_t* aSisFileName, bool aMakeStub, bool aReportInterpretSisError);
	
	/**
	 * This function will parse the package file. In case of any error it will
	 * throw exception (TParseException).
	 */
	void ParseL (const wchar_t* aPkgFileName);
	/**
	 * Set the directory to search for files. The package file and all the files referenced
	 * by the package is searched in this path.
	 */
	void SetSearchDirectory(const wchar_t* aPath);
	/**
	 * Create a hex dump of the created sis file.
	 */
	inline void Dump(std::ostream& aStream);

private:
	HANDLE OpenPackageFile(const wchar_t* aPkgFileName);
	void ParseEmbeddedBlockL (CSISInstallBlock& aInstall);
	void ParseLanguagesL();
	void ParseHeaderL();
	void ParseVendorNameL ();
	void ParseLogoL ();
	void ParseFileL (CSISInstallBlock& aInstall);
	void ParsePackageL (CSISInstallBlock& aInstall);
	void ParseOptionsBlockL ();
	void ParseLanguageBlockL (CSISInstallBlock& aInstall);
	void ParseIfBlockL (CSISInstallBlock& aInstall);
	void ParseDependencyL();
	void ParseTargetDeviceL();
	void ParseSignatureL();
	void ParsePropertyL ();
	void ParseCommentL ();
	void ParseVendorUniqueNameL ();
	void ParseLogicalOp (CSISExpression& aExpression);
	void ParseRelation(CSISExpression& aExpression);
	void ParseUnary(CSISExpression& aExpression);
	void ParseFactor(CSISExpression& aExpression);
	std::wstring ParseVersionArgs();

	DWORD ParseOption(const SParseOpt* options, DWORD dwNumOptions, DWORD* pdwOptions);
	TVersion CPackageParser::ParseVersion();
	std::wstring ConvertToString(const TInt32 aValue, std::ios_base& (*aBase)(std::ios_base&));
	
	//Function added for supporting supported language option
	// in package file format.
	std::wstring ParseSupportedLangArgs();
	
	void ExpectToken(TOKEN aToken);
	void GetNextToken(const TBool aDisablePkgKeywordCheck = 0);
	bool GetStringToken();
	WORD ParseEscapeChars();
	void GetAlphaNumericToken();
	void GetNumericToken();
	bool IsNumericToken();
	void GetNextChar();

	// validate the major , minor and build version.
	void ValidateVersion(TInt32& aMajor, TInt32& aMinor, TInt32& aBuild);

	const _TCHAR* GetTokenText(TOKEN aToken);
	
	// changed wstring to wchar_t for suite to prepend the path before filename.
	static TUint32 Find(wchar_t *aWhat, CSISFileDescription& aFileDesciption);
	static void Set(std::wstring& aWhat, CSISFileDescription& aData);

	inline bool InterpretSis() const;

	bool DoesExist(LPWSTR pszFile, DWORD *pdwSize);	 // Does the component file exist, how big is it
	bool DoesFileExist (std::wstring& aFileName, TUint64& aSize);
	
	void ReportInterpretSisError(TInterpretSisException aException);
	
	void ComputeAndSetHash(CSISFileDescription& aFileDesc);

private: // Private Member variables
	bool				iValidSISFile;
	bool 				iMakeStub;
	bool 				iReportInterpretSisError;
	bool 				iEnoughForStub;
	bool 				iIsHeaderDefined;
	bool 				iUniqueVendorName;
	bool 				iLocalisedVendorNames;
	wchar_t 			iPkgChar;
	int					iLineNo;					// The line we are currently on
	int 				iCurrentLang;				// If we are in a lang/lang file block - which lang are we processing
	TOKEN 				iToken;
	HANDLE 				iFileHandle;
	MParserObserver&	iObserver;					 // The obseverer object
	CSISXWriter 		iSISXWriter;		// Stuff all our intermediate data in here
	VARIANTVAL 			iTokenValue;
	wchar_t 			iSearchDir[PATHMAX];				 // Directory to search for component files

	static TDuplicates 	iSrcFiles;
	};

// Inline member functions
inline bool CPackageParser::InterpretSis() const
	{
	return iReportInterpretSisError; 
	}

inline void CPackageParser::Dump(std::ostream& aStream)
	{
	iSISXWriter.Dump(aStream);
	}
#endif // __PACKAGE_PARSER_H_

