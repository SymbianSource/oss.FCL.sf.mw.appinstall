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
* For parsing command line arguments.
* @internalComponent
*
*/


#ifndef	CMDPARSER_H
#define	CMDPARSER_H

#pragma warning( disable : 4786)

#include <iostream>
#include <vector>

class TExceptionHandler;

class CCmdParser
	{
	
	public:
		// Forward declaration for function prototypes
		enum TOptions;

	public:
		CCmdParser (int aArgc, const char** aArgv);
		static void Display(CCmdParser::TOptions);

		void ValidateOptions () const throw (TExceptionHandler);
		void LoadParams() throw (TExceptionHandler);

		// accessor functions for retrieving all parameters
		const std::string& GetUid () const;
		const int GetDrive () const;
		const std::vector<std::string>& GetSISFileNames () const;
		const std::vector<std::string>& GetFileNames () const;
		const std::string& GetOpDir () const;
		const std::string& GetOpFileName() const;

		bool IsOptionsSet() const;

	private:
		CCmdParser (const CCmdParser&);
		CCmdParser& operator =(const CCmdParser&);

		
		void CheckOptionsFormat() const throw (TExceptionHandler);
		void CheckMandatoryOptions() const throw (TExceptionHandler);
		void CheckOrder() const throw (TExceptionHandler);
		void CheckDuplicate() const throw (TExceptionHandler);

		void Parse (const char* aPath, std::string& aDirName, std::string& aFileName) const;
		
	public:
		enum TOptions
			{
			EVersion	=1,
			EDescription=2,
			ECopyright	=4,
			EUsage		=8,
			EHelp		=16,
			EAll		=31,
			};

	private:
		const int iTokenCount;
		const char** iTokens;

		std::string iUid;
		int iDriveName;
		std::vector<std::string> iSISFileNames;
		std::vector<std::string> iFileNames;
		std::string iOpDir;
		std::string iOpFileName;	
	
	};

#endif // CMDPARSER_H