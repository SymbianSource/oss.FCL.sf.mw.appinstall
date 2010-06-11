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
* Implements the test steps for transaction management APIs in the SCR
*
*/


#include "transactionmanagementsteps.h"

using namespace Usif;


// -----------CScrApplyTransactionStep-----------------

CScrApplyTransactionStep::CScrApplyTransactionStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrApplyTransactionStep::ImplTestStepPreambleL()
	{
	// empty
	}


void CScrApplyTransactionStep::ImplTestStepL()
	{
	User::LeaveIfError(iScrSession.Connect());
	iScrSession.CreateTransactionL();
	
	// Add a new component into SCR database
	TComponentId componentId= AddNonLocalisableComponentL(iScrSession);
	AppendSharedComponentIdL(componentId);
	
	// Register a file to the component
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);
	iScrSession.RegisterComponentFileL(componentId, fileName);
	
	// Get a property from config file and set it.
	CPropertyEntry *property = GetPropertyFromConfigLC(ETrue);
	if(property->PropertyType() != CPropertyEntry::EIntProperty)
		{
		ERR_PRINTF1(_L("The property is not an integer property which is expected!"));
		User::Leave(KErrArgument);
		}
	CIntPropertyEntry *propertyInt = dynamic_cast<CIntPropertyEntry*>(property);
	iScrSession.SetComponentPropertyL(componentId, propertyInt->PropertyName(), propertyInt->IntValue());
	CleanupStack::PopAndDestroy(propertyInt);
	
	// Commit the transaction if it is marked in the configuration
	TBool isCommitTransaction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("CommitTransaction"), isCommitTransaction);
	if(isCommitTransaction)
		{
		iScrSession.CommitTransactionL();
		}
	
	// Cancel thetransaction if it is marked in the configuration
	TBool isCancelTransaction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("CancelTransaction"), isCancelTransaction);
	if(isCancelTransaction)
		{
		iScrSession.RollbackTransactionL();
		}
	
	// Close the session
	iScrSession.Close();
	}

void CScrApplyTransactionStep::ImplTestStepPostambleL()
	{
	// empty
	}


// -----------CScrCreateTransactionStep-----------------

void OpenSessionLC(RSoftwareComponentRegistry& aSession)
	{
	User::LeaveIfError(aSession.Connect());
	CleanupClosePushL(aSession);
	}

void CreateTransactionL(RSoftwareComponentRegistry& aSession)
	{
	aSession.CreateTransactionL();
	}
	
void OpenSubsessionL(RSoftwareComponentRegistry aSession, RSoftwareComponentRegistryView& aSubsession)
	{
	CComponentFilter *filter = CComponentFilter::NewL(); // NewL is used for the sake of coverage
	CleanupStack::PushL(filter);
	aSubsession.OpenViewL(aSession, filter);
	CleanupStack::PopAndDestroy(filter);
	}	

void OpenSubsessionLC(RSoftwareComponentRegistry aSession, RSoftwareComponentRegistryView& aSubsession)
	{
	OpenSubsessionL(aSession, aSubsession);
	CleanupClosePushL(aSubsession);
	}

CScrCreateTransactionStep::CScrCreateTransactionStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrCreateTransactionStep::ImplTestStepPreambleL()
	{
	// empty
	}


void CScrCreateTransactionStep::ImplTestStepL()
	{
	RSoftwareComponentRegistry session1;
	RSoftwareComponentRegistry session2;
	RSoftwareComponentRegistryView subsession;
	RSoftwareComponentRegistryView subsession2;
	
	TBool isBeginTransactionBeforeSubsession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("BeginTransactionBeforeSubsession"), isBeginTransactionBeforeSubsession);
	if(isBeginTransactionBeforeSubsession)
		{
		OpenSessionLC(session1);
		CreateTransactionL(session1);
		OpenSessionLC(session2);
		OpenSubsessionLC(session2, subsession);
		CleanupStack::PopAndDestroy(3, &session1);
		}
	
	TBool isOpenSubsessionInTheSameSession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("OpenSubsessionInTheSameSession"), isOpenSubsessionInTheSameSession);
	if(isOpenSubsessionInTheSameSession)
		{
		OpenSessionLC(session1);
		CreateTransactionL(session1);
		OpenSubsessionLC(session1, subsession);
		CleanupStack::PopAndDestroy(2, &session1);
		
		// Now do the same,  but in reverse order
		OpenSessionLC(session1);
		OpenSubsessionLC(session1, subsession);		
		CreateTransactionL(session1);
		CleanupStack::PopAndDestroy(2, &session1);
		}
		
	TBool isBeginTransactionAfterSubsession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("BeginTransactionAfterSubsession"), isBeginTransactionAfterSubsession);
	if(isBeginTransactionAfterSubsession)
		{		
		OpenSessionLC(session1);
		OpenSessionLC(session2);
		OpenSubsessionLC(session1, subsession);
		CreateTransactionL(session2);
		CleanupStack::PopAndDestroy(3, &session1);
		}
	
	TBool isBeginTransactionInTheSameSession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("BeginTransactionInTheSameSession"), isBeginTransactionInTheSameSession);
	if(isBeginTransactionAfterSubsession)
		{		
		OpenSessionLC(session1);
		OpenSubsessionLC(session1, subsession);
		CreateTransactionL(session1);
		CleanupStack::PopAndDestroy(2, &session1);
		}
		
	TBool isBeginAfterAnotherBegin(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("BeginTransactionAfterAnotherBegin"), isBeginAfterAnotherBegin);
	if(isBeginAfterAnotherBegin)
		{
		OpenSessionLC(session1);
		CreateTransactionL(session1);
		OpenSessionLC(session2);
		CreateTransactionL(session2);
		CleanupStack::PopAndDestroy(2, &session1);
		}
	
	TBool isMultipleSubsessions(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("MultipleSubsessions"), isMultipleSubsessions);
	if(isMultipleSubsessions)
		{
		OpenSessionLC(session1);
		OpenSubsessionLC(session1, subsession);
		OpenSessionLC(session2);
		OpenSubsessionLC(session2, subsession2);
		CleanupStack::PopAndDestroy(4, &session1);
		}
	
	TBool isWriteAfterBegin(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("WriteAfterBeginTransaction"), isWriteAfterBegin);
	if(isWriteAfterBegin)
		{
		OpenSessionLC(session1);
		CreateTransactionL(session1);
		OpenSessionLC(session2);
		session2.AddComponentL(_L("test"),_L("vendor"),_L("plain"));	
		CleanupStack::PopAndDestroy(2, &session1);
		}
	
	TBool isReadAfterBegin(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("ReadAfterBeginTransaction"), isReadAfterBegin);
	if(isReadAfterBegin)
		{
		OpenSessionLC(session1);
		CreateTransactionL(session1);
		OpenSessionLC(session2);
		CComponentEntry *entry = CComponentEntry::NewLC();
		session2.GetComponentL(1, *entry);	
		CleanupStack::PopAndDestroy(3, &session1);
		}
		
	TBool mutatingOperationWithSubsession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("MutatingOperationWithSubsession"), mutatingOperationWithSubsession);
	if(mutatingOperationWithSubsession)
		{
		OpenSessionLC(session1);
		OpenSubsessionLC(session1, subsession);
		OpenSessionLC(session2);
		session2.AddComponentL(_L("test"),_L("vendor"),_L("plain"));
		CleanupStack::PopAndDestroy(3, &session1); // session2, subsession
		}	
		
	TBool closeSessionWithoutClosingSubsession(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("CloseSessionWithoutClosingSubsession"), closeSessionWithoutClosingSubsession);
  	if(closeSessionWithoutClosingSubsession)
   		{
   		OpenSessionLC(session1);
  		CreateTransactionL(session1);
  		OpenSubsessionL(session1, subsession);
  		OpenSubsessionL(session1, subsession2);
  		CComponentEntry *entry = subsession.NextComponentL();
  		CleanupStack::PushL(entry);
  		CComponentEntry *entry2 = subsession2.NextComponentL();
  		CleanupStack::PushL(entry2);		
  		CleanupStack::PopAndDestroy(3, &session1); // entry, entry2
		}
	
	
	TBool isMultipleActiveSubsessions (EFalse);
	GetBoolFromConfig(ConfigSection(), _L("MultipleActiveSubsessions"), isMultipleActiveSubsessions);
 	if(isMultipleActiveSubsessions)
 	    {
		OpenSessionLC(session1);
 	 	OpenSubsessionLC(session1, subsession);
 	 	TInt foundRecords (0);
 	 	CComponentEntry *entry = subsession.NextComponentL();
 	 	if(entry)
			{
			++foundRecords;
 	 	 	delete entry;
 	 	 	entry = NULL;
 	 	 	}

		OpenSessionLC(session2);
 	 	CComponentFilter *filter = CComponentFilter::NewL(); // NewL is used for the sake of coverage
 	 	CleanupStack::PushL(filter);
 	 	filter->SetNameL(_L("nonexistingcomponentnamefortransactionmanagementtest"));
 	 	filter->SetVendorL(_L("nonexistingvendornamefortransactionmanagementtest"));
 	 	subsession2.OpenViewL(session2, filter);
 	 	CleanupStack::PopAndDestroy(filter);
 	 	CleanupClosePushL(subsession2);
 	 	
 	 	CComponentEntry *entry2 = subsession2.NextComponentL();
 	 	if(entry2)
			{
 	 	 	delete entry2;
 	 	 	entry2 = NULL;
 	 	 	entry2 = subsession2.NextComponentL();
 	 	 	ERR_PRINTF1(_L("Subsession2 has unexpectedly retrieved a component!"));
 	 	 	SetTestStepResult(EFail);
 	 	 	}
		CleanupStack::PopAndDestroy(2, &session2); // session2, subsession2

 	 	entry = subsession.NextComponentL();
 	 	while(entry)
			{
 	 	 	++foundRecords;
 	 	 	delete entry;
 	 	 	entry = NULL;
 	 	 	entry = subsession.NextComponentL();
 	 	 	}
		if(foundRecords < 2)
			{
 	 	 	ERR_PRINTF1(_L("Subsession1 has unexpectedly reached the end of the view!"));
 	 	 	SetTestStepResult(EFail);
 	 	 	}
		CleanupStack::PopAndDestroy(2, &session1);
 	 	}	
	}

void CScrCreateTransactionStep::ImplTestStepPostambleL()
	{
	// empty
	}

// -----------CScrManageAnotherTransactionStep-----------------

CScrManageAnotherTransactionStep::CScrManageAnotherTransactionStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrManageAnotherTransactionStep::ImplTestStepPreambleL()
	{
	// empty
	}


void CScrManageAnotherTransactionStep::ImplTestStepL()
	{
	User::LeaveIfError(iScrSession.Connect());
	iScrSession.CreateTransactionL();
	
	RSoftwareComponentRegistry anotherSession;
	User::LeaveIfError(anotherSession.Connect());
	CleanupClosePushL(anotherSession);
	
	// Commit the transaction if it is marked in the configuration
	TBool isCommitTransaction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("CommitTransaction"), isCommitTransaction);
	if(isCommitTransaction)
		anotherSession.CommitTransactionL();
	
	TBool isCancelTransaction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("CancelTransaction"), isCancelTransaction);
	if(isCancelTransaction)
		anotherSession.RollbackTransactionL();
	
	TBool isInvokeMutatingFunction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("InvokeMutatingFunction"), isInvokeMutatingFunction);
	if(isInvokeMutatingFunction)
		anotherSession.AddComponentL(_L("test"), _L("vendor"),_L("plain"));
	
	TBool isInvokeNonMutatingFunction(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("InvokeNonMutatingFunction"), isInvokeNonMutatingFunction);
	if(isInvokeNonMutatingFunction)
		{
		RArray<TComponentId> componentIdList;
		anotherSession.GetComponentIdsL(componentIdList);
		componentIdList.Close();
		}
	
	CleanupStack::PopAndDestroy(&anotherSession);
	iScrSession.Close();
	}

void CScrManageAnotherTransactionStep::ImplTestStepPostambleL()
	{
	// empty
	}
