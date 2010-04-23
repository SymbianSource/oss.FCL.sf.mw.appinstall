/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CSwiObserverWaiter. See class and function definitions for more information.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "swiobserverwaiter.h"

using namespace Swi;

//
//CSwiObserverWaiter
//

CSwiObserverWaiter::CSwiObserverWaiter(CSwiObserverSession* aSession, const RMessage2& aMessage)
/**
	This private constructor initializes the superclass and prevents direct instantiation.
	   
	@param	aSession		Session on which this request was launched.
	@param	aMessage		Standard server-side handle to message.
 */
	: CAsyncRequest(aSession, NULL, aMessage)
	{
	//empty
	}
	

CSwiObserverWaiter::~CSwiObserverWaiter()
/**
	Destructor.
 */
	{
	//empty
	}
	

CSwiObserverWaiter* CSwiObserverWaiter::NewLC(CSwiObserverSession* aSession, const RMessage2& aMessage)
/**
	Factory function allocates a new, initialized instance of CSwiObserverWaiter,
	registered for the supplied session.
	   
	If construction is successful, the caller should pass ownership to
	the SCS framework (TransferToScsFrameworkL), and pop the CSwiObserverWaiter
    from the cleanupstack. From this point onwards the request should be completed 
    by the CSwiObserverWaiter/CAsyncRequst code, so the caller must not return or 
    leave with any code except KErrScsSetupAsync. 
	   
	   @param	aSession		Session on which this request was launched.
	   @param	aMessage		Standard server-side handle to message.
	   @return					New, initialized instance of CSwiObserverWaiter
 */
	{
	CSwiObserverWaiter *sow = new(ELeave) CSwiObserverWaiter(aSession,aMessage);
	CleanupStack::PushL(sow);
	return sow;
	}

void CSwiObserverWaiter::DoCancel()
	/**
	   Override CActive::DoCancel() method.
	   This function completes our internal request so it does not hang.
	*/
	{
	if(iStatus == KRequestPending)
		{
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status,KErrCancel);
		}
	}


void CSwiObserverWaiter::Startup()
/**
	Sets its own status member to KRequestPending and makes itself active.
 */
	{
	iStatus = KRequestPending;
	SetActive();
	}
