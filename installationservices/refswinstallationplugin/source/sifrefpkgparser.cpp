/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file implements a package file parser for the Reference Installer.
*
*/


#include "sifrefpkgparser.h"
#include <f32file.h>
#include "usiflog.h"
#include <usif/usiferror.h>

using namespace Usif;

/**
A set of helper functions for the parser.
*/
namespace
	{
	_LIT8(KTxtCRLF, "\r\n");
	_LIT8(KTxtLanguages, "Languages");
	_LIT8(KTxtComponentName, "ComponentName");
	_LIT8(KTxtComponentVendor, "ComponentVendor");
	_LIT8(KTxtComponentVersion, "ComponentVersion");
	_LIT8(KTxtFile, "File");

	/** Maximum length of a value string in a package file */
	const TInt KMaxValueLength = 0xFF;

	void SplitLineL(const TPtrC8& aLine, TPtrC8& aName, TPtrC8& aValue)
		{
		const TInt colon = aLine.Locate(':');
		if (colon == KErrNotFound)
			{
			DEBUG_PRINTF2(_L8("Missing colon in line: %S"), &aLine);
			User::Leave(KErrCorrupt);
			}

		aName.Set(aLine.Left(colon));
		aValue.Set(aLine.Mid(colon+1));
		}

	void SplitValuesL(const TPtrC8& aValues, RCHBufCArray& aValueArray)
		{
		TPtrC8 values(aValues);
		while (values.Size() > 0)
			{
			TInt quot = values.Locate('"');
			if (quot == KErrNotFound)
				{
				return;
				}
			values.Set(values.Mid(quot+1));
			quot = values.Locate('"');
			if (quot == KErrNotFound)
				{
				DEBUG_PRINTF2(_L8("Missing closing quotation mark in line: %S"), &aValues);
				User::Leave(KErrCorrupt);
				}
			TPtrC8 token(values.Left(quot));

			HBufC* value = HBufC::NewLC(KMaxValueLength);
			TPtr valPtr(value->Des());
			valPtr.Copy(token);
			aValueArray.AppendL(value);
			CleanupStack::Pop(value);

			values.Set(values.Mid(quot+1));
			}
		}

	void ParseComponentVersionL(const TPtrC8& aStringVersion, TVersion& aVersion)
		{
		// Check if already defined
		if (aVersion.iMajor || aVersion.iMinor || aVersion.iBuild)
			{
			DEBUG_PRINTF(_L8("Multiple definition of Component Version"));
			User::Leave(KErrCorrupt);
			}

		// Convert the major version number of Component Version
		TLex8 lex(aStringVersion);
		lex.SkipSpace();
		TInt err = lex.Val(aVersion.iMajor);
		if (err != KErrNone)
			{
			DEBUG_PRINTF(_L8("Failed parsing the major version number of Component Version"));
			User::LeaveIfError(err);
			}
		lex.Inc();

		// Convert the minor version number of Component Version
		err = lex.Val(aVersion.iMinor);
		if (err != KErrNone)
			{
			DEBUG_PRINTF(_L8("Failed parsing the minor version number of Component Version"));
			User::LeaveIfError(err);
			}
		lex.Inc();

		// Convert the build version number of Component Version
		err = lex.Val(aVersion.iBuild);
		if (err != KErrNone)
			{
			DEBUG_PRINTF(_L8("Failed parsing the build version number of Component Version"));
			User::LeaveIfError(err);
			}
		}

	TLanguage LanguageMapL(TDesC& aLanguage)
		{
		if (aLanguage == _L("EN"))
			{
			return ELangEnglish;
			}
		else if (aLanguage == _L("HU"))
			{
			return ELangHungarian;
			}
		else if (aLanguage == _L("PL"))
			{
			return ELangPolish;
			}

		DEBUG_PRINTF2(_L8("Language %S is not supported by this Reference Installer"), &aLanguage);
		User::Leave(KErrSifUnsupportedLanguage);
		return ELangNone;// Suppress warning
		}
	}

CSifRefPkgParser* CSifRefPkgParser::NewL(const TDesC& aFileName)
	{
	CSifRefPkgParser* self = new (ELeave) CSifRefPkgParser;
	CleanupStack::PushL(self);
	self->ParseL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

CSifRefPkgParser::CSifRefPkgParser(): iVersion(0, 0, 0)
	{
	}

CSifRefPkgParser::~CSifRefPkgParser()
	{
	iLanguages.Close();
	iComponentNames.Close();
	iVendorNames.Close();
	iFiles.Close();
	}

const RLanguageArray& CSifRefPkgParser::Languages() const
	{
	return iLanguages;
	}

const RCHBufCArray& CSifRefPkgParser::ComponentNames() const
	{
	return iComponentNames;
	}

const RCHBufCArray& CSifRefPkgParser::VendorNames() const
	{
	return iVendorNames;
	}

const TVersion& CSifRefPkgParser::Version() const
	{
	return iVersion;
	}

const RCHBufCArray& CSifRefPkgParser::Files() const
	{
	return iFiles;
	}

void CSifRefPkgParser::ParseL(const TDesC& aFileName)
	{
	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = file.Open(fs, aFileName, EFileShareReadersOnly);
	if (err != KErrNone)
		{
		DEBUG_PRINTF3(_L8("Failed to open file: %S with error: %d"), &aFileName, err);
		User::Leave(err);
		}
	CleanupClosePushL(file);

	ParseL(file);

	CleanupStack::PopAndDestroy(2, &fs);
	}

void CSifRefPkgParser::ParseL(RFile& aFileHandle)
	{
	// Read the file into the buffer 
	TInt fileSize = 0;
	User::LeaveIfError(aFileHandle.Size(fileSize));
	HBufC8* buffer = HBufC8::NewLC(fileSize);
	TPtr8 bufPtr = buffer->Des();
	TInt err = aFileHandle.Read(bufPtr);
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Failed to read reference package file with err %d"), err);
		User::LeaveIfError(err);
		}

	// Iterate over the lines
	TPtrC8 lines(*buffer);
	const TInt crlfLen = TPtrC8(KTxtCRLF).Size();
	while (lines.Size() > 0)
		{
		// Find next line
		TPtrC8 line;
		const TInt crlfPos = lines.Find(KTxtCRLF);
		if (crlfPos != KErrNotFound)
			{
			line.Set(lines.Left(crlfPos));
			lines.Set(lines.Mid(crlfPos+crlfLen));
			}
		else
			{
			if (lines.Size() > 0)
				{
				line.Set(lines);
				lines.Set(TPtrC8());
				}
			}
		
		// Split the line into a pair of the name of a tag and its value
		TPtrC8 name, value;
		SplitLineL(line, name, value);
		
		// Hand over the pair to recognition
		LineHandlerL(name, value);
		}
	CleanupStack::PopAndDestroy(buffer);
	
	// Check whether the file we have just parsed wasn't corrupted
	CheckInvariantL();
	}

void CSifRefPkgParser::LineHandlerL(const TPtrC8& aName, const TPtrC8& aValue)
	{
	if (aName == KTxtLanguages)
		{
		if (iLanguages.Count() > 0)
			{
			DEBUG_PRINTF(_L8("Multiple definition of languages"));
			User::Leave(KErrCorrupt);
			}
		RCHBufCArray languages;
		CleanupClosePushL(languages);
		SplitValuesL(aValue, languages);
		const TInt count = languages.Count();
		for (TInt i=0; i<count; ++i)
			{
			iLanguages.AppendL(LanguageMapL(*languages[i]));
			}
		CleanupStack::PopAndDestroy(&languages);
		}
	else if (aName == KTxtComponentName)
		{
		if (iComponentNames.Count() > 0)
			{
			DEBUG_PRINTF(_L8("Multiple definition of Component Name"));
			User::Leave(KErrCorrupt);
			}
		SplitValuesL(aValue, iComponentNames);
		}
	else if (aName == KTxtComponentVendor)
		{
		if (iVendorNames.Count() > 0)
			{
			DEBUG_PRINTF(_L8("Multiple definition of Component Vendor"));
			User::Leave(KErrCorrupt);
			}
		SplitValuesL(aValue, iVendorNames);
		}
	else if (aName == KTxtComponentVersion)
		{
		ParseComponentVersionL(aValue, iVersion);
		}
	else if (aName == KTxtFile)
		{
		HBufC* file = HBufC::NewLC(aValue.Size());
		TPtr fileBuf = file->Des();
		fileBuf.Copy(aValue);
		fileBuf.Trim();
		iFiles.AppendL(file);
		CleanupStack::Pop(file);
		}
	else
		{
		DEBUG_PRINTF2(_L8("Failed due to unrecognized token: %S"), &aName);
		User::Leave(KErrCorrupt);
		}
	}

TInt CSifRefPkgParser::GetLanguageIndex(TLanguage aLanguage) const
	{
	const TInt count = iLanguages.Count();
	for (TInt i=0; i<count; ++i)
		{
		if (iLanguages[i] == aLanguage)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

void CSifRefPkgParser::CheckInvariantL() const
	{
	if (iVersion.iMajor == 0 && iVersion.iMinor == 0 && iVersion.iBuild == 0)
		{
		DEBUG_PRINTF(_L8("Version not specified!"));
		User::Leave(KErrCorrupt);
		}
	if (iLanguages.Count() == 0)
		{
		DEBUG_PRINTF(_L8("Language not specified!"));
		User::Leave(KErrCorrupt);
		}
	if (iLanguages.Count() != iComponentNames.Count())
		{
		DEBUG_PRINTF(_L8("The number of languages and component names don't match!"));
		User::Leave(KErrCorrupt);
		}
	if (iLanguages.Count() != iVendorNames.Count())
		{
		DEBUG_PRINTF(_L8("The number of languages and vendor names don't match!"));
		User::Leave(KErrCorrupt);
		}
	}
