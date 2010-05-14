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
* This file defines a file package parser for the Reference Installer.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef __SIFREFPKGPARSER_H__
#define __SIFREFPKGPARSER_H__

#include <scs/scsclient.h>
#include <f32file.h>
#include <usif/sif/sifcommon.h>

namespace Usif
	{
	// Helpful typedefs
	typedef RArray<TLanguage> RLanguageArray;
	typedef RCPointerArray<HBufC> RCHBufCArray;

	/**
	A package file parser. This refrence implementation reads an entire file into a buffer
	and then parses it line by line. The parser operates synchronously and therefore
	the processing of a package cannot be cancelled.
	*/
	class CSifRefPkgParser: public CBase
		{
	public:
		/**
		Creates an instance of the parser and parses a package file by file name.
		
		@param aFileName The name of a package file to be parsed.
		@return Pointer to an instance of the parser.
		*/
		static CSifRefPkgParser* NewL(const TDesC& aFileName);

		/**
		Destroys an instance of the parser.
		*/
		virtual ~CSifRefPkgParser();

		/**
		Gets a reference to an array of languages supported by the package.
		*/
		const RLanguageArray& Languages() const;

		/**
		Gets a reference to an array of localized names of a component in the package.
		The sequence of the names in the array corresponds to the sequence of
		the languages returned by Languages().
		*/
		const RCHBufCArray& ComponentNames() const;

		/**
		Gets a reference to an array of localized names of the vendor of a component in
		the package. The sequence of the names in the array corresponds to the sequence of
		the languages returned by Languages().
		*/
		const RCHBufCArray& VendorNames() const;

		/**
		Gets a reference to an array of files of a component in the package.
		*/
		const RCHBufCArray& Files() const;

		/**
		Gets the version of a component in the package.
		*/
		const TVersion& Version() const;

		/**
		A helper method. Gets the index of aLanguage in the array of languages supported by
		a component in the package.
		
		@param aLanguage Language to be found.
		@return The index of aLanguage in the array of supported languages.
		*/
		TInt GetLanguageIndex(TLanguage aLanguage) const;

	private:
		CSifRefPkgParser();
		CSifRefPkgParser(const CSifRefPkgParser&);
		CSifRefPkgParser& operator=(const CSifRefPkgParser&);

		void ParseL(const TDesC& aFileName);
		void ParseL(RFile& aFileHandle);
		void LineHandlerL(const TPtrC8& aName, const TPtrC8& aValue);
		void CheckInvariantL() const;

		RLanguageArray iLanguages;
		RCHBufCArray iComponentNames;
		RCHBufCArray iVendorNames;
		RCHBufCArray iFiles;
		TVersion iVersion;
		};

	} // namespace Usif

#endif // __SIFREFPKGPARSER_H__
