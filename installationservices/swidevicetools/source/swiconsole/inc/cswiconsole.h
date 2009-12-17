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
* CSwiConsole class definition. This class manages most of the 
* functionalities of the tool.
* @internalComponent
*
*/



#ifndef __CSWICONSOLE_H_
#define __CSWICONSOLE_H_

// System includes
#include <e32base.h>
#include "cpreferencehandler.h"

// Forward Declarations
class CConsoleUiHandler;
class CPackageInfo;

class CSwiConsole: public CBase
	{
public: //Public Static Member Functions
	static CSwiConsole* NewL(const CPreferenceHandler& aPreferenceHandler);
	static CSwiConsole* NewLC(const CPreferenceHandler& aPreferenceHandler);
	
public: // Public Constructors and Destructors
	~CSwiConsole();
	
public: // Public Member Functions
	void InstallL();
	void UninstallL();
	void ListInstalledPackagesL();

private: // Private Constructors
	CSwiConsole(const CPreferenceHandler &);
	void ConstructL();
	
private: // Private Member Functions
	void VerifyInputL(TOperation aOperation);
	
private: // Private Member Variables
	const CPreferenceHandler&	iPreferenceHandler;
	CConsoleUiHandler*			iUiHandler;
	CPackageInfo* 				iPackageInfo;
	};

#endif // __CSWICONSOLE_H_