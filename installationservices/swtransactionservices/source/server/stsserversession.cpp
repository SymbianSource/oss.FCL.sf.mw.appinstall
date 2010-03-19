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

#include "stsserver.h"
#include "stsserversession.h"
#include <random.h>
#include <e32cmn.h>
#include <e32std.h>
#include <usif/sts/stsdefs.h>
#include "usiflog.h"

namespace Usif
{

CStsServerSession::CStsServerSession(CStsServer& aStsserver)
	: CScsSession(static_cast<CScsServer &>(aStsserver))
	{
	}

CStsServerSession::~CStsServerSession()
	{
	if (iTransactionPtr)
		TRAP_IGNORE(static_cast<CStsServer*>(&iServer)->ReleaseTransactionL(iTransactionPtr));
	}

CStsServerSession* CStsServerSession::NewL(CStsServer &aServer, const RMessage2& /*aMessage*/)
	{
	CStsServerSession* self = new (ELeave) CStsServerSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CStsServerSession::ConstructL()
	{
	CScsSession::ConstructL();
	}


_LIT(KStsPanicCategory, "StsServer");
static const TInt KInvalidMessagePanicCode = KErrArgument;

TBool CStsServerSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L("CStsServerSession::DoServiceL() - Function %d"), static_cast<TStsClientServerFunctionType>(aFunction));							

	TBool ret = ETrue; //used in CScsSession::ServiceL(); if set to ETRue >> aMessage.Complete(KErrNone); is executed
	TStsTransactionId transactionID;
	TPckg<TStsTransactionId> idPkg(transactionID);
	RBuf buf;
	CStsServer* server=static_cast<CStsServer*>(&iServer);
	
	switch(static_cast<TStsClientServerFunctionType>(aFunction))
		{
		case ECreateTransaction:
			{
			if(iTransactionPtr)
				{
				User::Leave(KErrInUse);
				}
			iTransactionPtr = server->CreateTransactionL();
			transactionID = iTransactionPtr->TransactionId();
			aMessage.WriteL(KIdIPCSlot, idPkg);
			break;
			}
		case EOpenTransaction:
			{
			if(iTransactionPtr)
				{
				User::Leave(KErrInUse);
				}
			aMessage.ReadL(KIdIPCSlot, idPkg, 0);
			iTransactionPtr = server->AttachTransactionL(transactionID);
			break;
			}
		case ERegisterNew:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));
			buf.CreateL(KMaxFileName);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			iTransactionPtr->RegisterNewL(buf);
			CleanupStack::PopAndDestroy(&buf);
			break;
			}
		case ECreateNew:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));			
			// Taking the filename size from the IPC Arguments
			// No need to do any validation of filename as it is done by File Server
			TInt srcFileNameLen = aMessage.GetDesLengthL(KFilePathIPCSlot);
			buf.CreateL(srcFileNameLen);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			TPckgBuf<TFileMode> pkgFileMode;
			aMessage.ReadL(KFileModeIPCSlot, pkgFileMode, 0);
			RFile newFile;
			CleanupClosePushL(newFile);
			iTransactionPtr->CreateNewL(buf, newFile, pkgFileMode());
	        User::LeaveIfError(newFile.TransferToClient(aMessage, KFileHandleIPCSlot));
	        ASSERT(aMessage.IsNull());  // message should have been completed
	        ret = EFalse; //message need not be completed in CScsSession::ServiceL()
			CleanupStack::PopAndDestroy(2, &buf);
			break;
			}
		case ERemove:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));			
			buf.CreateL(KMaxFileName);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			iTransactionPtr->RemoveL(buf);
			CleanupStack::PopAndDestroy(&buf);
			break;
			}
		case ERegisterTemporary:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));			
			buf.CreateL(KMaxFileName);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			iTransactionPtr->RegisterTemporaryL(buf);
			CleanupStack::PopAndDestroy(&buf);
			break;
			}
		case ECreateTemporary:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));			
			// Taking the filename size from the IPC Arguments
			// No need to do any validation of filename as it is done by File Server
			TInt srcFileNameLen = aMessage.GetDesLengthL(KFilePathIPCSlot);
			buf.CreateL(srcFileNameLen);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			TPckgBuf<TFileMode> pkgFileMode;
			aMessage.ReadL(KFileModeIPCSlot, pkgFileMode, 0);
			RFile newTempFile;
			CleanupClosePushL(newTempFile);
			iTransactionPtr->CreateTemporaryL(buf, newTempFile, pkgFileMode());
	        User::LeaveIfError(newTempFile.TransferToClient(aMessage, KFileHandleIPCSlot));
	        ASSERT(aMessage.IsNull());  // message should have been completed
	        ret = EFalse; //message need not be completed in CScsSession::ServiceL()
			CleanupStack::PopAndDestroy(2, &buf);
			break;
			}
		case EOverwrite:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));			
			buf.CreateL(KMaxFileName);
			buf.CleanupClosePushL();
			aMessage.ReadL(KFilePathIPCSlot, buf, 0);
			TPckgBuf<TFileMode> pkgFileMode;
			aMessage.ReadL(KFileModeIPCSlot, pkgFileMode, 0);
			RFile newFile;
			CleanupClosePushL(newFile);
			iTransactionPtr->OverwriteL(buf, newFile, pkgFileMode());
	        User::LeaveIfError(newFile.TransferToClient(aMessage, KFileHandleIPCSlot));
	        ASSERT(aMessage.IsNull());  // message should have been completed
	        ret = EFalse; //message need not be completed in CScsSession::ServiceL()
			CleanupStack::PopAndDestroy(2, &buf);
			break;
			}
		case ECommit:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));
			iTransactionPtr->CommitL(); 
			server->FinaliseTransactionL(iTransactionPtr); //releases transaction and erases pointer!
			break;
			}
		case ERollBack:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));
			iTransactionPtr->RollBackL();
			server->FinaliseTransactionL(iTransactionPtr); //releases transaction and erases pointer!
			break;
			}
		case EGetId:
			{
			__ASSERT_ALWAYS(iTransactionPtr, aMessage.Panic(KStsPanicCategory, KInvalidMessagePanicCode));
			transactionID = iTransactionPtr->TransactionId();
			//return ID back to client
			aMessage.WriteL(KIdIPCSlot, idPkg);
			break;
			}
		case ERollBackAllPending:
			{
			server->RollBackAllPendingL();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	return ret; 
	}
}//end of namespace Usif
