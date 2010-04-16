/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the basic test step for the Software Component Registry test harness
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef SUBSESSIONSTEPS_H
#define SUBSESSIONSTEPS_H

#include "tscrstep.h"


class CScrTestServer;

_LIT(KScrComponentRegistrySubsessionStep, "ScrComponentRegistrySubsession");
_LIT(KScrFileListSubsessionStep, "ScrFileListSubsession");
_LIT(KScrAppViewSubsessionStep, "ScrAppViewSubsession");

class CScrComponentRegistrySubsessionStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR RSoftwareComponentRegistryView interface
 */
	{
public:
	 CScrComponentRegistrySubsessionStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
private:	
	void TestSingleModeL(const Usif::RSoftwareComponentRegistryView& aSubSession, RPointerArray<Usif::CComponentEntry>& aExpectedEntries);
	void TestSetModeL(const Usif::RSoftwareComponentRegistryView& aSubSession, TInt aSetSize, RPointerArray<Usif::CComponentEntry>& aExpectedEntries);
	void VerifyNonReturnedEntriesL(const RPointerArray<Usif::CComponentEntry>& aExpectedEntries);
	void GetComponentEntriesFromConfigL(RPointerArray<Usif::CComponentEntry>& aEntries);
	};

class CScrFileListSubsessionStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR RSoftwareComponentRegistryFilesList interface
 */
	{
public:
	CScrFileListSubsessionStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
private:
	void TestSingleModeL(const Usif::RSoftwareComponentRegistryFilesList& aSubSession, RPointerArray<HBufC>& expectedFiles);
	void TestSetModeL(const Usif::RSoftwareComponentRegistryFilesList& aSubSession, TInt aSetSize, RPointerArray<HBufC>& expectedFiles);
	void VerifyNonReturnedFilesL(const RPointerArray<HBufC>& aExpectedFiles);
	void ReportUnexpectedFileL(const TDesC& aUnexpectedFileName);
	};


class CAppInfoView : public CScrTestStep
/**
 	TEF test step which exercises the SCR RApplicationInfoView interface
 */
	{
public:
    CAppInfoView(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
private:
	//
	void TestSessionL(TInt aNoOfEntries, const Usif::RApplicationInfoView& aSubSession, RPointerArray<Usif::TAppRegInfo>& aExpectedEntries);
	void GetAppEntryFromConfigL(RPointerArray<Usif::TAppRegInfo>& aEntries);
	void VerifyNonReturnedEntriesL(const RPointerArray<Usif::TAppRegInfo>& aExpectedEntries);
	};

#endif /* SUBSESSIONSTEPS_H */
