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
* Implements test steps for the subsessions interfaces in the SCR
*
*/


#include "subsessionsteps.h"
#include <scs/cleanuputils.h>
#include "tscrdefs.h"

using namespace Usif;

TBool IsEqual(const CComponentEntry& aLhsEntry, const CComponentEntry& aRhsEntry)
	{
	return (aLhsEntry == aRhsEntry);
	}

TBool IsEqual(const HBufC& aLhs, const HBufC& aRhs)
	{
	return const_cast<HBufC &>(aLhs).Des() == const_cast<HBufC &>(aRhs).Des();
	}

template <class T> TBool VerifyMatchingL(const T* aObj, RPointerArray<T>& aExpectedArray)
	{
	TInt pos = aExpectedArray.Find(aObj, IsEqual);
	if (pos != KErrNotFound)
		{
		T* foundObj = aExpectedArray[pos];
		delete foundObj;
		aExpectedArray.Remove(pos);
		return ETrue;
		}
	return EFalse;
	}


// -----------CScrGetComponentStep-----------------

CScrComponentRegistrySubsessionStep::CScrComponentRegistrySubsessionStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrComponentRegistrySubsessionStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrComponentRegistrySubsessionStep::GetComponentEntriesFromConfigL(RPointerArray<CComponentEntry>& aEntries)
	{
	TInt componentsCount(0);
	if (!GetIntFromConfig(ConfigSection(), KComponentsCountName, componentsCount))
		{
		CComponentEntry *componentEntry = GetComponentEntryFromConfigLC();
		aEntries.AppendL(componentEntry);
		CleanupStack::Pop(componentEntry);
		}
	
	for (TInt i = 0; i < componentsCount; ++i)
		{
		CComponentEntry *componentEntry = GetComponentEntryFromConfigLC(EFalse,i);
		aEntries.AppendL(componentEntry);
		CleanupStack::Pop(componentEntry);
		}
	}

void CScrComponentRegistrySubsessionStep::VerifyNonReturnedEntriesL(const RPointerArray<CComponentEntry>& aExpectedEntries)
	{
	// Check if we need to verify the returned components
	// Performance tests don't need verification.
	TBool noVerification = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("NoVerification"), noVerification);
	if(noVerification) return;

	if (aExpectedEntries.Count() > 0)
		{
		TComponentId componentId = aExpectedEntries[0]->ComponentId(); 
		const TDesC& componentName = aExpectedEntries[0]->Name();
		ERR_PRINTF3(_L("At least one expected entry was not returned by the API - id %d, name %S"), componentId, &componentName);
		SetTestStepResult(EFail);
		}	
	}

void CScrComponentRegistrySubsessionStep::TestSingleModeL(const RSoftwareComponentRegistryView& aSubSession, RPointerArray<CComponentEntry>& aExpectedEntries)
	{
	// Check if we need to verify the returned components
	// Performance tests don't need verification. Because the impact verification on performance results.
	// In addition, we cannot simply verify some test cases such as retrieving all components or removable components. 
	// Because, the perforamce test databases are populated with random data, and in the future we cannot guarantee 
	// the values will stay same if the databases are recreated.
	
	TBool noVerification = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("NoVerification"), noVerification);

	// Read continuously entries from the sub-session
	while (1)
		{
		CComponentEntry* componentEntry = aSubSession.NextComponentL();
		if (componentEntry == NULL)
			break; // No more entries were found
		CleanupStack::PushL(componentEntry);
		
		if(noVerification)
			{
			CleanupStack::PopAndDestroy(componentEntry);
			continue;
			}
		// For each entry, check whether it was found in the expected array.
		// If found, delete it from the expected array. If not, fail the test step
		if (!VerifyMatchingL(componentEntry, aExpectedEntries))
			{
			ERR_PRINTF2(_L("The API returned an unexpected entry with id %d"), componentEntry->ComponentId());
			SetTestStepResult(EFail);		
			CleanupStack::PopAndDestroy(componentEntry);
			return;
			}
				
		CleanupStack::PopAndDestroy(componentEntry);
		}
			
	// At the end, check whether there were expected entries which were not found
	VerifyNonReturnedEntriesL(aExpectedEntries);
	}

void CScrComponentRegistrySubsessionStep::TestSetModeL(const RSoftwareComponentRegistryView& aSubSession, TInt aSetSize, RPointerArray<CComponentEntry>& aExpectedEntries)
	{
	// Check if we need to verify the returned components
	// Performance tests don't need verification. See TestSingleModeL for more information.
	TBool noVerification = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("NoVerification"), noVerification);

	while (1)
		{
		RPointerArray<CComponentEntry> componentSet;
		CleanupResetAndDestroyPushL(componentSet);
		
		aSubSession.NextComponentSetL(aSetSize, componentSet);
		TInt returnedComponentsCount = componentSet.Count(); 
		if (returnedComponentsCount <= 0)
			{
			CleanupStack::PopAndDestroy(&componentSet);
			break;
			}
		
		if(noVerification) 
			{
			CleanupStack::PopAndDestroy(&componentSet);
			continue;
			}
				
		for (TInt i = 0; i < returnedComponentsCount; ++i)
			{
			if (!VerifyMatchingL(componentSet[i], aExpectedEntries))
				{
				ERR_PRINTF2(_L("The API returned an unexpected entry with id %d"), componentSet[i]->ComponentId());
				SetTestStepResult(EFail);				
				CleanupStack::PopAndDestroy(&componentSet);
				return;
				}
			}
		
		CleanupStack::PopAndDestroy(&componentSet);
		}
	
	VerifyNonReturnedEntriesL(aExpectedEntries); 
	}

void CScrComponentRegistrySubsessionStep::ImplTestStepL()
	{	
	CComponentFilter* componentFilter = ReadComponentFilterFromConfigLC();
	INFO_PRINTF1(_L("Read component filter from configuration"));
	
	INFO_PRINTF1(_L("Opened subsession to SCR"));
	
	//Get testing mode from configuration (sets or single iteration)
	//Delegate the actual invocation and comparison according to the testing mode	
	TInt setSize = GetSetSizeFromConfigL();
	INFO_PRINTF2(_L("Test set size %d"), setSize);
	
	// Read expected results from config
	RPointerArray<CComponentEntry> expectedComponentEntries;
	CleanupResetAndDestroyPushL(expectedComponentEntries);
	GetComponentEntriesFromConfigL(expectedComponentEntries);
	StartTimer();
	// Open the subsession
	RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession);	
	subSession.OpenViewL(iScrSession, componentFilter);
		
	if (setSize == 1)
		TestSingleModeL(subSession, expectedComponentEntries);
	else
		TestSetModeL(subSession, setSize, expectedComponentEntries);
		
	CleanupStack::PopAndDestroy(3, componentFilter); // expectedComponentEntries, subSession, componentFilter
	}

void CScrComponentRegistrySubsessionStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrFileListSubsessionStep-----------------

CScrFileListSubsessionStep::CScrFileListSubsessionStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrFileListSubsessionStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrFileListSubsessionStep::VerifyNonReturnedFilesL(const RPointerArray<HBufC>& aExpectedFiles)
	{
	if (aExpectedFiles.Count() > 0)
		{
		HBufC* fileName = aExpectedFiles[0]; 
		ERR_PRINTF2(_L("At least one expected files was not returned by the API - name %S"), fileName);
		SetTestStepResult(EFail);
		}	
	}

void CScrFileListSubsessionStep::ReportUnexpectedFileL(const TDesC& aUnexpectedFileName)
	{
	ERR_PRINTF2(_L("The API returned an unexpected file entry with name %S"), &aUnexpectedFileName);
	SetTestStepResult(EFail);		
	}

void CScrFileListSubsessionStep::TestSingleModeL(const RSoftwareComponentRegistryFilesList& aSubSession, 
													 RPointerArray<HBufC> &aExpectedFiles)
	{
	FOREVER
		{
		HBufC *nextFile = aSubSession.NextFileL();
		if (!nextFile)
			break;
		CleanupStack::PushL(nextFile);
		
		if (!VerifyMatchingL(nextFile, aExpectedFiles))
			{
			ReportUnexpectedFileL(*nextFile);
			CleanupStack::PopAndDestroy(nextFile);
			return;
			}
		
		CleanupStack::PopAndDestroy(nextFile);
		}
	
	VerifyNonReturnedFilesL(aExpectedFiles);
	}

void CScrFileListSubsessionStep::TestSetModeL(const RSoftwareComponentRegistryFilesList& aSubSession, TInt aSetSize, 
											 	  RPointerArray<HBufC>& aExpectedFiles)
	{
	FOREVER
		{
		RPointerArray<HBufC> foundFiles;
		CleanupResetAndDestroyPushL(foundFiles);
		
		aSubSession.NextFileSetL(aSetSize, foundFiles);
		TInt returnedFilesCount = foundFiles.Count(); 
		if (returnedFilesCount <= 0)
			{
			CleanupStack::PopAndDestroy(&foundFiles);
			break;
			}
		for (TInt i = 0; i < returnedFilesCount; ++i)
			{
			if (!VerifyMatchingL(foundFiles[i], aExpectedFiles))
				{
				ReportUnexpectedFileL(*foundFiles[i]);
				CleanupStack::PopAndDestroy(&foundFiles);
				return;
				}
			}
		
		CleanupStack::PopAndDestroy(&foundFiles);
		}
	VerifyNonReturnedFilesL(aExpectedFiles);
	}


void CScrFileListSubsessionStep::ImplTestStepL()
	{	
	TInt componentId = GetComponentIdL();
	RSoftwareComponentRegistryFilesList subSession;
	CleanupClosePushL(subSession);
	
	subSession.OpenListL(iScrSession, componentId);
	
	RPointerArray<HBufC> expectedFiles;
	CleanupResetAndDestroyPushL(expectedFiles);
	
	GetFileNameListFromConfigL(expectedFiles);
	TInt setSize = GetSetSizeFromConfigL();

	if (setSize > 1)
		TestSingleModeL(subSession, expectedFiles);
	else
		TestSetModeL(subSession, setSize, expectedFiles);	
			
	CleanupStack::PopAndDestroy(2, &subSession); //expectedEntries 
	}

void CScrFileListSubsessionStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}
