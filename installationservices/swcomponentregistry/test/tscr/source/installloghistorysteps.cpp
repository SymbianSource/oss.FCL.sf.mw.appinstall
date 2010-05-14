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
*
*/


#include "installloghistorysteps.h"
#include <scs/cleanuputils.h>
#include "tscrdefs.h"
#include <usif/scr/screntries.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

using namespace Usif;

_LIT(KScrLogFile, "!:\\private\\10285bc0\\scr.log");

// -----------CScrGetLogEntriesStep-----------------

CScrGetLogEntriesStep::CScrGetLogEntriesStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetLogEntriesStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetLogEntriesStep::GetLogEntriesFromConfigL(RPointerArray<CScrLogEntry>& aLogEntries)
	{
	TInt logCount = 0;
	if (!GetIntFromConfig(ConfigSection(), KLogsCountParamName, logCount))
		PrintErrorL(_L("Logs count was not found!"), KErrNotFound);
		
	if (logCount < 0)
			PrintErrorL(_L("Logs count is negative!"), KErrNotFound);
	
	TBuf<MAX_SCR_PARAM_LENGTH> componentNameParam, swTypeNameParam, versionParam, operationTypeParam, globalIdParam;
	
	for(TInt index=0; index < logCount; ++index)	
		{
		componentNameParam = KComponentName;
		swTypeNameParam = KSoftwareTypeName;
		versionParam = KVersionName;
		operationTypeParam = KOperationType;
		globalIdParam = KGlobalIdName;
			
		GenerateIndexedAttributeNameL(componentNameParam, index);
		GenerateIndexedAttributeNameL(swTypeNameParam, index);
		GenerateIndexedAttributeNameL(versionParam, index);
		GenerateIndexedAttributeNameL(operationTypeParam, index);
		GenerateIndexedAttributeNameL(globalIdParam, index);
		
		TPtrC componentName;
		if (!GetStringFromConfig(ConfigSection(), componentNameParam, componentName))
			{
			ERR_PRINTF2(_L("The component name param %S could not be found in configuration."), &componentNameParam);
			User::Leave(KErrNotFound);
			}
		
		TPtrC swTypeName;
		if (!GetStringFromConfig(ConfigSection(), swTypeNameParam, swTypeName))
			{
			ERR_PRINTF2(_L("The software type name param %S could not be found in configuration."), &swTypeNameParam);
			User::Leave(KErrNotFound);
			}
		
		TPtrC version;
		if (!GetStringFromConfig(ConfigSection(), versionParam, version))
			{
			ERR_PRINTF2(_L("The version param %S could not be found in configuration."), &versionParam);
			User::Leave(KErrNotFound);
			}
		
		TInt operationType;
		if (!GetIntFromConfig(ConfigSection(), operationTypeParam, operationType))
			{
			ERR_PRINTF2(_L("The operation type param %S could not be found in configuration."), &operationTypeParam);
			User::Leave(KErrNotFound);
			}
		
		TPtrC globalId;
		if (!GetStringFromConfig(ConfigSection(), globalIdParam, globalId))
			{
			ERR_PRINTF2(_L("The global id param %S could not be found in configuration."), &globalIdParam);
			User::Leave(KErrNotFound);
			}
		
		CScrLogEntry *logEntry = CScrLogEntry::NewLC(componentName, swTypeName, globalId, version, static_cast<TScrComponentOperationType>(operationType));
		aLogEntries.AppendL(logEntry);
		CleanupStack::Pop(logEntry); // Ownership is transferred
		}
	}

TBool operator ==(CScrLogEntry& aLhsEntry, CScrLogEntry& aRhsEntry)
	{
	return (aLhsEntry.ComponentName() == aRhsEntry.ComponentName() && 
			aLhsEntry.SoftwareTypeName() == aRhsEntry.SoftwareTypeName()&& 
			aLhsEntry.ComponentVersion() == aRhsEntry.ComponentVersion()&&
			aLhsEntry.GlobalId() == aRhsEntry.GlobalId()&&
			aLhsEntry.OperationType() == aRhsEntry.OperationType());
	}

TBool operator !=(CScrLogEntry& aLhsEntry, CScrLogEntry& aRhsEntry)
	{
	return !(aLhsEntry == aRhsEntry);
	}

TBool CScrGetLogEntriesStep::CompareLogEntriesL(RPointerArray<CScrLogEntry>& aFoundLogEntries, RPointerArray<CScrLogEntry>& aExpectedLogEntries)
	{
	TInt foundEntriesCount = aFoundLogEntries.Count();
	TInt expectedEntriesCount = aExpectedLogEntries.Count();
		
	if (foundEntriesCount != expectedEntriesCount)
		{
		ERR_PRINTF3(_L("The number of expected log entries %d did not match the number of provided log entries %d."), expectedEntriesCount, foundEntriesCount);
		return EFalse; 
		}
		
	for (TInt i = 0; i < foundEntriesCount; ++i)
		{
		if (*aFoundLogEntries[i] != *aExpectedLogEntries[i])
			{
            ERR_PRINTF2(_L("Log entry %d did not match."), i);
            ERR_PRINTF2(_L("current device lang is %d"), User::Language());
			
			ERR_PRINTF6(_L("found values are name %S, type %S, version %S, globalid %S, optype is %d"),
                            &(aFoundLogEntries[i]->ComponentName()),
                            &(aFoundLogEntries[i]->SoftwareTypeName()), 
                            &(aFoundLogEntries[i]->ComponentVersion()), 
                            &(aFoundLogEntries[i]->GlobalId()),
                            aFoundLogEntries[i]->OperationType());

            ERR_PRINTF6(_L("expected values were name %S, type %S, version %S, globalid %S, optype is %d"),
							&(aExpectedLogEntries[i]->ComponentName()) ,
                            &(aExpectedLogEntries[i]->SoftwareTypeName()) , 
                            &(aExpectedLogEntries[i]->ComponentVersion()), 
                            &(aExpectedLogEntries[i]->GlobalId()), 
                            aExpectedLogEntries[i]->OperationType() );
            return EFalse;
			}
		}	
	return ETrue;
	}
	
void CScrGetLogEntriesStep::ImplTestStepL()
	{
	TBool isLogFileReadOnly = EFalse; 
	(void)GetBoolFromConfig(ConfigSection(), _L("LogFileReadOnly"), isLogFileReadOnly); // since this is an optional param, ignore the return code.
	if(isLogFileReadOnly)
		{
		RBuf fn;
		fn.CreateL(KMaxFileName);
		fn.CleanupClosePushL();
		fn.Copy(KScrLogFile);
		RFs fs;
		User::Leave(fs.Connect());
		CleanupClosePushL(fs);
		fn[0] = fs.GetSystemDriveChar();
		User::LeaveIfError(fs.SetAtt(fn, KEntryAttReadOnly, 0));
		CleanupStack::PopAndDestroy(2, &fn); // fn, fs
		}

	RPointerArray<CScrLogEntry> expectedLogEntries;
	CleanupResetAndDestroyPushL(expectedLogEntries);
	GetLogEntriesFromConfigL(expectedLogEntries);
	
	TPtrC *ptrSwTypeName = NULL;
	TPtrC swTypeName;
	if (GetStringFromConfig(ConfigSection(), KSoftwareTypeName, swTypeName))
		{
		ptrSwTypeName = &swTypeName; 
		}
	
	RPointerArray<CScrLogEntry> retrievedLogEntries;
	CleanupResetAndDestroyPushL(retrievedLogEntries);
	
	iScrSession.RetrieveLogEntriesL(retrievedLogEntries, ptrSwTypeName);
	
	if (!CompareLogEntriesL(retrievedLogEntries, expectedLogEntries))
		{
		ERR_PRINTF1(_L("The retrieved log entries are different from the expected ones."));
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(2, &expectedLogEntries); // expectedLogEntries, retrievedLogEntries	
	}

void CScrGetLogEntriesStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrSetLogFileReadOnlyAttrStep-----------------

CScrSetLogFileReadOnlyAttrStep::CScrSetLogFileReadOnlyAttrStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrSetLogFileReadOnlyAttrStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}
	
void CScrSetLogFileReadOnlyAttrStep::ImplTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
				
	RBuf fn;
	fn.CreateL(KMaxFileName);
	fn.CleanupClosePushL();
	fn.Copy(KScrLogFile);
	fn[0] = fs.GetSystemDriveChar();
			
	TBool isLogFileReadOnly = EFalse; 
	if(!GetBoolFromConfig(ConfigSection(), _L("LogFileReadOnly"), isLogFileReadOnly))
		PrintErrorL(_L("LogFileReadOnly flag was not found!"), KErrNotFound);
	
	if(isLogFileReadOnly)
		User::LeaveIfError(fs.SetAtt(fn, KEntryAttReadOnly, 0));
	else
		User::LeaveIfError(fs.SetAtt(fn, 0, KEntryAttReadOnly));
	
	CleanupStack::PopAndDestroy(2, &fs); // fs, fn
	}

void CScrSetLogFileReadOnlyAttrStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrCreateLogFileStep-----------------

CScrCreateLogFileStep::CScrCreateLogFileStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrCreateLogFileStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}
	
void CScrCreateLogFileStep::ImplTestStepL()
	{
	TInt numOfRecords;
	if (!GetIntFromConfig(ConfigSection(), KLogsCountParamName, numOfRecords))
		PrintErrorL(_L("The logs count param could not be found in configuration!"), KErrNotFound);
	
	TComponentId compId (0);
	
	for(TInt i=0; i<numOfRecords; ++i)
		{
		compId = iScrSession.AddComponentL(_L("TestComponentName"), _L("TestComponentVendor"),_L("plain"));
		iScrSession.SetComponentVersionL(compId,_L("1.2.3"));
		}
	}

void CScrCreateLogFileStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}
