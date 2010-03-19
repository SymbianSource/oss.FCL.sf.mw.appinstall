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
* Defines the test step which deletes SCR database file. 
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef SCRDBDELETE_H
#define SCRDBDELETE_H

#include "tscrstep.h"
#include "tscraccessor_client.h"
class CScrTestServer;

_LIT(KScrDeleteDbFileStep, "SCRDeleteDbFile");
_LIT(KScrCopyDbFileStep, "SCRCopyDbFile");

class CScrDeleteDbFileStep : public CScrTestStep
/**
 	TEF test step which deletes the SCR DB database.
 	If the file cannot be deleted, it gives other tries in the specified amount of time.
 */
	{
public:
	CScrDeleteDbFileStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		

private:
	void DeleteFileL(const TDesC& aFilePath);
	
private:
	RScrAccessor iScrAccessor;
	};


class CScrCopyDbFileStep : public CScrTestStep
/**
 	TEF test step which copies a SCR database into the given location.
 */
	{
public:
	CScrCopyDbFileStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	
private:
	RScrAccessor iScrAccessor;
	};
	
#endif /* SCRDBDELETE_H */
