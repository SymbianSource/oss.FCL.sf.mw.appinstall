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
* This file provides the class declaration of CCommandLineParser.
* @internalComponent
*
*/

 
 
#ifndef _H_CMDPARSER_
#define _H_CMDPARSER_

// System header files
#include <e32std.h>

// User defined header files
#include "cpreferences.h"
#include "cpreferencehandler.h"

// Forward Declaration
class CCommandLineArguments;

class CCommandLineParser: public CBase
{
public: // Public Constructors and Destructors
	~CCommandLineParser();
	static CCommandLineParser* NewL();
	static CCommandLineParser* NewLC();

public:	// Public Member Functions
	TOperation ParseArgumentsL();
	CPreferences* GetPreferencesLC();
	CPreferenceHandler* GetPreferenceHandlerLC();

private: // Public Constructors and Destructors
	// Private Default Constructor
	CCommandLineParser() { }
	// Private Copy Constructor
	CCommandLineParser(const CCommandLineParser&) { }
	// Two phase constructor
	void ConstructL();
	
private:// Private Member Functions
	TLanguage GetLanguageTypeL(const TDesC&);
	void FillPreferencesL(CPreferenceHandler&);
	void LogCommandLineArgumentsL(CPreferenceHandler&);

private: // Private Data Members
	CCommandLineArguments* 	iCommandLineArguments;
};

#endif //_H_CMDPARSER_