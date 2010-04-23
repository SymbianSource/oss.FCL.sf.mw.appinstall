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
* Implements CSwiObserverSession which is server-side session object.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "swiobserver.h"
#include "swiobserverwaiter.h"

using namespace Swi;

//
//CSwiObserverSession
//

CSwiObserverSession::CSwiObserverSession(CSwiObserver& aServer)
/**
	Intializes the SWI Observer session object with the server handle.
 */
	:	CScsSession(aServer)
		{
		//empty
		}


CSwiObserverSession::~CSwiObserverSession()
/**
	Destructor.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Session Closed"));
	iFile.Close();
	iFullFileName.Close();	
	}

	
CSwiObserverSession* CSwiObserverSession::NewL(CSwiObserver &aServer, const RMessage2& aMessage)
/**
	Factory function allocates new instance of CSwiObserverSession.
	
	@param aServer  Swi Observer object.
	@param aMessage	Standard server-side handle to message. Not used.
	@param aFs      File server session handle.
	@return			New, initialized instance of CSwiObserverSession 
					which is owned by the caller.
 */
	{
	(void) aMessage;
	
	CSwiObserverSession* s = new (ELeave) CSwiObserverSession(aServer);
	CleanupStack::PushL(s);
	s->CScsSession::ConstructL();
	CleanupStack::Pop(s);
	
	return s;
	}


TBool CSwiObserverSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.  Not used.
 */
	{
	TSwiObserverFunction f = static_cast<TSwiObserverFunction>(aFunction);
	CSwiObserver *observer = static_cast<CSwiObserver*>(&iServer);
				
	switch(f)
		{
		case ERegisterToObserver:
			{
			DEBUG_PRINTF(_L8("CSwiObserverSession::DoServiceL register function called"));
			//Check if the session has already been registered to the server.
			if(iSessionProgress & ERegistered)
				{
				//The session has already been registered, do not allow a second registration.
				User::Leave(KErrServerBusy);
				}
			else
				{
				//Set the session progress status as registered. 
				//If the processor is busy and a waiter object is created to keep the client
				//waiting, the status variable should still be registered not to allow the client
				//to cause a second waiter object creation.
				iSessionProgress |= ERegistered;
				
				//If the processor is not busy, allow the requesting client to go through.
				if(observer->ProcessorProgressStatus() != EProcessorBusy)
					{
					DEBUG_PRINTF(_L8("SWI Observer registration done"));
					break;		
					}
				else
					{
					DEBUG_PRINTF(_L8("SWI Observer registration request queued"));
					//SWI Observer processor is busy. A asynchronous waiter object
					//is created and its ownership passed to the SCS framework.
					CSwiObserverWaiter* waiter = CSwiObserverWaiter::NewLC(this,aMessage);
					waiter->TransferToScsFrameworkL();
					CleanupStack::Pop(waiter); // waiter object now owned by SCS framework
					waiter->Startup();
					return EFalse;
					}
				}
			}
			
		case ETransferFileHandle:
			{
			DEBUG_PRINTF(_L8("CSwiObserverSession::DoServiceL transfer file handle function called"));
			//Check if the session has already been registered to the server.
			//If it is not registered, do not service it.
			if(!(iSessionProgress & ERegistered))
				{
				User::Leave(KErrAccessDenied);
				}
					
			observer->AppendFileNameL(iFullFileName);
			
			//Create a log file in the private folder	
			User::LeaveIfError(iFile.Replace(observer->iFs, iFullFileName, EFileWrite | EFileShareAny));
			
			//A new logfile has just been created ==> "turn the flag on" by creating the flag file to show presence of logfiles
			observer->ManipulateFlagFileL(ECreateFlagFile);
			
			//Pass the file	handle to the client
			User::LeaveIfError(iFile.TransferToClient(aMessage, 0));
			
			//The message should have been completed
			ASSERT(aMessage.IsNull());
			
			//The file handle is not needed by the swi observer session after that point.
			iFile.Close();
			
			//RMessage2 object is closed by both TransferToClient() and SCS framework.
			//return EFalse to prevent SCS to close the message object.
			return EFalse;
			}
			
		case ESwiCommit:
			{
			DEBUG_PRINTF(_L8("CSwiObserverSession::DoServiceL commit function called"));
			//Check if the session has already been registered to the server.
			//If it is not registered, do not service it.
			if(!(iSessionProgress & ERegistered))
				{
				User::Leave(KErrAccessDenied);
				}
				
			//Mark that the session has been committed.	
			iSessionProgress |= ECommitted;
			
			//Start processing the file.
			observer->ProcessL();
			break;
			} 
		BULLSEYE_OFF
		}//End of switch. Should never come here.
		BULLSEYE_RESTORE
	return ETrue;
	
	}//End of function
	
