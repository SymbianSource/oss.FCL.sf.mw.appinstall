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
* makesis main function
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __MAKESIS_H__
#define __MAKESIS_H__

// ===========================================================================
// INCLUDES
// ===========================================================================

#pragma warning(disable: 4786)
#include "parsecmd.h"
#include "packageparser.h"
#include "utils.h"
#include "siswrite.h"

#include <string>
#include <vector>

typedef std::vector< std::pair<std::wstring, WORD> > InterpretSisErrors;
typedef InterpretSisErrors::value_type InterpretSisError;
// ===========================================================================
// CLASS DEFINITION
// ===========================================================================

class CMakeSIS : public MParserObserver
// The main application object. Controls the SIS file generation process
	{
public:
	CMakeSIS();
		
	int  RunL(int argc, wchar_t* argv[]);
	
	bool ParseSource();
	bool WriteTarget();
	
public: // From MParserObserver
	virtual void DoMsg(const wchar_t* aText) const;
	virtual void DoVerbage(const wchar_t* aText) const;
	virtual void DoErrMsg(const wchar_t* aText) const;
	virtual bool Verbose() const;
	virtual bool Dump() const;
	virtual bool SetVerbose(bool aVerbose);
	virtual void SetLineNumber(int aLineNumber);
	virtual void AddInterpretSisError(TInterpretSisException aError);

private:
	void DoMsg(const wchar_t* aText1, const wchar_t* aText2) const;
	void ShowBanner();
	void ShowUsage ();
	void ShowSyntax();
	void ShowSSL ();
	std::wstring GetVersionInfo(char *aRequest);
	
	void ShowUtilsError(TUtilsException err);
	void ShowParseError(TParseException err);
	void ShowCommandLineError(TCommandLineException err);
	void AddErrMsg(const wchar_t* pszText);
	void DisplayInterpretSisReport() const;
	void ShowVersionFunctionSyntax() const;

private:
	int 				iLineNo;
	bool 				iDump;
	bool 				iVerbose;
	CParseCmd			iCmdOptions;
	InterpretSisErrors	iInterpretSisReport;
	};


//Inline member functions

inline bool CMakeSIS::Verbose() const
	{
	return iVerbose; 
	}

inline bool CMakeSIS::Dump() const
	{
	return iDump; 
	}

inline bool CMakeSIS::SetVerbose(bool aVerbose)
	{
	bool prev = iVerbose; 
	iVerbose = aVerbose; 
	return prev; 
	}

inline void CMakeSIS::SetLineNumber(int aLineNumber)
	{
	iLineNo = aLineNumber; 
	}


#endif // __MAKESIS_H__
