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
* CQueueProcessor - implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include "queueprocessor.h"
#include "sislauncherclientserver.h"
#include "sislauncherserver.h"
#include "log.h"
#include <e32base.h>

using namespace Swi;

CQueueProcessor::CQueueProcessor(CSisLauncherServer& aServer) : 
	CActive(EPriorityStandard),	iQueue(), iServer(&aServer)
	{
	CActiveScheduler::Add(this);
	}

CQueueProcessor::~CQueueProcessor()
	{
	Cancel();
	Cleanup();
	ResetQueue();
	}

void CQueueProcessor::Cleanup()
	{
	DEBUG_PRINTF(_L8("Sis Launcher Server - Cleaning up queue"));	
	if (iEcomSession)
		{
		iEcomSession->CancelNotifyOnChange(iStatus);
		iEcomSession->Close();
		iEcomSession->FinalClose();
		iEcomSession = 0;
		}
	}

void CQueueProcessor::ConstructL()
	{
	// nothing to do
	}
CQueueProcessor* CQueueProcessor::NewL(CSisLauncherServer& aServer)
	{
	CQueueProcessor* me = new (ELeave) CQueueProcessor(aServer);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

void CQueueProcessor::AddToQueueL(const RMessage2& aMessage)
	{
	//message args are (fshandleidx, filehandleidx, wait, mime)
	// except for RunExe which are (filename, notused , wait)
	
	// adopt file handle
	RFile file;
	CleanupClosePushL(file);
	
	// or get filename
	TFileName filename;
		
	if (aMessage.Function() == EQueueRunExecutable)
		{
		aMessage.ReadL(0, filename);
		}
	else
		{
		file.AdoptFromClient(aMessage,0,1);
		}
	
	// decode the wait flag
	TBool wait;
	TPckg <TBool> waitPckg(wait);
	aMessage.ReadL(2, waitPckg);

	// and mime if we need it.
	HBufC8* mimeType = 0;
	
	
	if (aMessage.Function() == EQueueStartByMimeByHandle)
		{
		TInt srcLen = aMessage.GetDesLengthL(3);
		mimeType = HBufC8::NewL(srcLen);
		CleanupStack::PushL(mimeType);
		TPtr8 ptr(mimeType->Des());
		aMessage.ReadL(3, ptr);
		}
	
	CItem* item = CItem::NewL(
			aMessage.Function(),
			filename,
			file,
			wait, 
			mimeType);
	
	if (mimeType!=0) 
		{
		CleanupStack::Pop(mimeType);
		}
	
	CleanupStack::Pop(&file); 
	CleanupStack::PushL(item);
	
	iQueue.AppendL(item);
	
	CleanupStack::Pop(item);
	}
void  CQueueProcessor::ResetQueue()
	{
	iQueue.ResetAndDestroy();
	}


TInt  CQueueProcessor::ExecuteQueue()
	{
	// this should be enforced by the client. 
	ASSERT(iQueue.Count()> 0);
		
	iServer->LongServerShutdown();
	TRAPD(err, iEcomSession = &REComSession::OpenL());
	if (err == KErrNone)
		{			
		DEBUG_PRINTF2(_L8("Sis Launcher Server - Registering for ECOM notification"), iStatus.Int());			
		iStatus=KRequestPending;
		iEcomSession->NotifyOnChange(iStatus);
		SetActive();
		}
	return err;
	}

void CQueueProcessor::DoCancel()
	{
	DEBUG_PRINTF2(_L8("Sis Launcher Server - Cancelling wait for ECOM notification"), iEcomSession);			
	Cleanup();
	}

void CQueueProcessor::RunL()
	{
	DEBUG_PRINTF3(_L8("Sis Launcher Server - Starting to process %d queued files (status %d)"),
			iQueue.Count(),iStatus.Int());
		
	if (iStatus.Int() == KErrNone)
		{
		DoExecuteQueue();
		}
	}

TInt CQueueProcessor::RunError(TInt /*aError*/)
	{
	Cancel();
	return KErrNone;
	}

void CQueueProcessor::DoExecuteQueue()
	{
#ifndef SWI_TEXTSHELL_ROM
	iServer->CancelShutdown();
	for (int i = 0; i < iQueue.Count(); ++i)
		{
		CItem* item = iQueue[i]; 
		if (item != 0)
			{
			// note - there is nothing we can do to handle run errors so ignore them.
			switch (item->iType)
				{
				case EQueueRunExecutable:
					TRAP_IGNORE(iServer->RunExecutableL(*item->iFilename,item->iWait));
					break; 
				case EQueueStartDocumentByHandle:
					TRAP_IGNORE(iServer->StartDocumentL(item->iFile, item->iWait));	
					break;
				case EQueueStartByMimeByHandle:
					TRAP_IGNORE(iServer->StartByMimeL(item->iFile, *item->iMime, item->iWait));	
					break;
				default:
					break;
				}
			// we must clean up otherwise the handles are locked and files can't be deleted
			// until the handles
			delete item;
			iQueue[i] = 0;
			}
		}
	iServer->ShortServerShutdown();
#endif	
	}

TBool CQueueProcessor::ItemsQueued()
	{
	return iQueue.Count() > 0;
	}

CQueueProcessor::CItem::CItem() 
	{
	
	}

CQueueProcessor::CItem::~CItem() 
	{
	delete iMime;
	delete iFilename;
	iFile.Close();
	}

CQueueProcessor::CItem* CQueueProcessor::CItem::NewL(
		TInt aFunction, TFileName& aFilename, RFile& aFile, 
		TBool aWait, HBufC8* aMimeType)
	{
	CItem* me     = new (ELeave) CItem();
	CleanupStack::PushL(me);

	me->iType     = aFunction;
	me->iFilename = aFilename.AllocL();
	me->iFile     = aFile;
	me->iWait     = aWait;
	me->iMime     = aMimeType;
	
	CleanupStack::Pop(me);
	
	return me;
	}


	
