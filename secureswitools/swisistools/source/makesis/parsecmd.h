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
* handles parsing of makesis command line args
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __PARSE_CMD_H_
#define __PARSE_CMD_H_

#include "fieldroot.h"
#include "utils.h"

// ===========================================================================
// CONSTANTS
// ===========================================================================

#define SOURCEFILE		L".pkg"
#define DESTFILE		L".sis"

enum TCommandLineException
	{ErrInsufficientArgs,
	ErrBadCommandFlag,
	ErrBadSourceFile,
	ErrNoSourceFile,
	ErrCannotOpenSourceFile,
	ErrBadTargetFile
	};

// ===========================================================================
// CLASS DEFINITION
// ===========================================================================

class CParseCmd
// Responsable for processing and maintaining the command line options
	{
public:
	CParseCmd();
	
	ParseCommandLine(int argc, _TCHAR *argv[]);
	
	bool  ShowSyntax ()						const { return iShowSyntax;	}
	bool  ShowSSL ()						const { return iShowSSL;		}
	bool  InterpretSisReport ()				const { return iInterpretSisReport; }
	int Flags ()							const { return iOptions;		}
	CSISFieldRoot::TDbgFlag DebugFlags ()	const { return iDebugOptions;	}
	const wchar_t* SourceFile ()					const { return iSource;		}
	const wchar_t* SearchPath ()					const { return iDir;		}
	const wchar_t* TargetFile ();
		
	enum TOptions
		{
		EOptVerbose  = 0x01,
		EOptMakeStub  = 0x02,
		EOptDirectory = 0x04,
		EOptPassword  = 0x08,
		EOptDump	  = 0x10
		};

private:
	void SetDirectory(const wchar_t* pszPath);
	void SetSource(const wchar_t* pszSource);
	void SetTarget(const wchar_t* pszTarget);
		
private:
	wchar_t iDir[PATHMAX];
	wchar_t iSource[PATHMAX];
	wchar_t iTarget[PATHMAX];
		
	int		iOptions;
	bool	iShowSyntax;
	bool	iShowSSL;
	bool	iInterpretSisReport;
	CSISFieldRoot::TDbgFlag iDebugOptions;
	};

#endif // __PARSE_CMD_H_
