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
* Implementation of  wrapper class to access the software installer registry
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#include "registrywrapper.h"

using namespace Swi;

CRegistryWrapper* CRegistryWrapper::NewL()
	{
	CRegistryWrapper* wrapper = new (ELeave) CRegistryWrapper();
	CleanupStack::PushL(wrapper);
	wrapper->ConstructL();
	CleanupStack::Pop(wrapper);
	return wrapper;
	}

void CRegistryWrapper::StartMutableOperationsL()
	{
	if (!iMutableOperationInProgress)
		{
		iRegistrySession.CreateTransactionL();
		iMutableOperationInProgress = ETrue;
		}
	}

void CRegistryWrapper::CommitMutableOperationsL()
	{
	if (iMutableOperationInProgress)
		{
		iRegistrySession.CommitTransactionL();
		iMutableOperationInProgress = EFalse;
		}
	}

void CRegistryWrapper::RollbackMutableOperationsL()
	{
	if (iMutableOperationInProgress)
		{
		iRegistrySession.RollbackTransactionL();
		iMutableOperationInProgress = EFalse;
		}
	}

CRegistryWrapper::~CRegistryWrapper()
	{
	iRegistrySession.Close();
	}

void CRegistryWrapper::ConstructL()
	{
	User::LeaveIfError(iRegistrySession.Connect());
	}

