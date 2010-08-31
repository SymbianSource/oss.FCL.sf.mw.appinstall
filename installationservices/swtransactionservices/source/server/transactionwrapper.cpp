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
* Implements CStsServer.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "transactionwrapper.h"

namespace Usif
{
CReferenceCountedTransactionWrapper::CReferenceCountedTransactionWrapper()
	{
	
	}

CReferenceCountedTransactionWrapper::~CReferenceCountedTransactionWrapper()
	{
	delete iTrPtr;
	}

/*static*/ CReferenceCountedTransactionWrapper* CReferenceCountedTransactionWrapper::NewLC(TStsTransactionId aTransactionID)
	{
	CReferenceCountedTransactionWrapper* wrapper= new (ELeave) CReferenceCountedTransactionWrapper();
	CleanupStack::PushL(wrapper);
	wrapper->ConstructL(aTransactionID);
	return wrapper;
	}

void CReferenceCountedTransactionWrapper::ConstructL(TStsTransactionId aTransactionID)
	{
	iTrPtr = CIntegrityServices::NewL(aTransactionID);
	iRefCount=0;
	iCompleted=EFalse;
	}

CIntegrityServices* CReferenceCountedTransactionWrapper::Attach()
	{
	++iRefCount; 
	return iTrPtr; 
	}

TInt CReferenceCountedTransactionWrapper::Detach() 
	{
	return --iRefCount;
	}
}//end of namespace Usif
