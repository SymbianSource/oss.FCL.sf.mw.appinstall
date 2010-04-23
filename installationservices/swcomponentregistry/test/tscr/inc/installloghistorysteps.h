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
* Defines test steps for the install log retrieving interface in the Software Component Registry
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef INSTALLLOGHISTORYSTEPS_H
#define INSTALLLOGHISTORYSTEPS_H

#include "tscrstep.h"
#include "tscraccessor_client.h"
class CScrTestServer;
class Usif::CScrLogEntry;

_LIT(KScrGetLogEntriesStep, "SCRGetLogEntries");
_LIT(KScrSetLogFileReadOnlyAttrStep, "SCRSetLogFileReadOnlyAttr");
_LIT(KScrCreateLogFileStep, "SCRCreateLogFile");

class CScrGetLogEntriesStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR RetrieveLogEntriesL interface
 */
	{
public:
	CScrGetLogEntriesStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();

private:
	void GetLogEntriesFromConfigL(RPointerArray<Usif::CScrLogEntry>& aLogEntries);
	TBool CompareLogEntriesL(RPointerArray<Usif::CScrLogEntry>& aFoundLogEntries, RPointerArray<Usif::CScrLogEntry>& aExpectedLogEntries);
	};

class CScrSetLogFileReadOnlyAttrStep : public CScrTestStep
/**
 	TEF test step sets the SCR log file's readonly attribute.
 */
	{
public:
	CScrSetLogFileReadOnlyAttrStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrCreateLogFileStep : public CScrTestStep
/**
 	TEF test step creates a log file including a preset number of records.
 */
	{
public:
	CScrCreateLogFileStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

#endif /* INSTALLLOGHISTORYSTEPS_H */
