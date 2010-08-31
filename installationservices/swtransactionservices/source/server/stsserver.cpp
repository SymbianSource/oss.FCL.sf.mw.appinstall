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
#include "usiflog.h"
#include <scs/securityutils.h>

namespace Usif
{
static const TUint stsServerRangeCount = 5;

static const TInt stsServerRanges[stsServerRangeCount] =
	{
	0,							                                      //Range 0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	                                      //Range 1 - EBaseSession to (EBaseSession+ERegisterNew)-1. ==> from ECreateTransaction to EGetId inclusive
	CScsServer::EBaseSession|static_cast<TInt>(ERegisterNew),         //Range 2 - (EBaseSession+ERegisterNew) to (EBaseSession+ERollBackAllPending)-1 ==> from ERegisterNew to EOverwrite inclusive
	CScsServer::EBaseSession|static_cast<TInt>(ERollBackAllPending),  //Range 3 - (EBaseSession+ERollBackAllPending) to ((EBaseSession+ERollBackAllPending)+1)-1==> only ERollBackAllPending
	CScsServer::EBaseSession|static_cast<TInt>(ERollBackAllPending)+1 //Range 4 - (EBaseSession+ERollBackAllPending)+1 to KMaxTInt inclusive  (i.e: ScsImpl::EPreCloseSession)
	};

static const TUint8 stsServerElementsIndex[stsServerRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 : is not supported.
	CPolicyServer::EAlwaysPass,   // Range 1
	CPolicyServer::ECustomCheck,  // Range 2 : custom check will be made made for each function to see if client is authorised to do that
	0,                            // Range 3 : Can only be issued by KUidDaemon
	CPolicyServer::EAlwaysPass    // Range 4
	};							

static const CPolicyServer::TPolicyElement stsServerElements[] =
	{
	{_INIT_SECURITY_POLICY_S0(KUidDaemon), CPolicyServer::EFailClient}, //special handling of function ERollBackAllPending 
	};

static const CPolicyServer::TPolicy stsServerPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connects
	stsServerRangeCount,
	stsServerRanges,
	stsServerElementsIndex,
	stsServerElements,
	};

//
//CStsServer
//

CStsServer::CStsServer()
/**
	Intializes the STS server object with its version and policy.
 */
	:	CScsServer(TVersion(KStsVerMajor, KStsVerMinor, KStsVerBuild),stsServerPolicy)
		{
		//empty
		}

CStsServer::~CStsServer()
/**
	Destructor. Cleanup the STS server.
 */
	{
	iTransactionWrapperContainer.ResetAndDestroy();
	}
	
	
CStsServer* CStsServer::NewLC()
/**
	Factory function allocates new, initialized instance of CStsServer.

	@return		New, initialized instance of CStsServer
				which is left on the cleanup stack.
 */
	{
	CStsServer *so = new (ELeave) CStsServer();
	CleanupStack::PushL(so);
	so->ConstructL();
	return so;
	}


void CStsServer::ConstructL()
/**
	Second phase constructor starts the STS server.
 */
	{
	// Roll back all previous transactions in case the STS process died unexpectedly
	// Note: error returned from RollBackAllPendingL() is ignored at this point to prevent STS server startup from failing
	// It is important to roll back before calling CScsServer::ConstructL as the roll back may be longer than the shutdown period for the timer started by that function
	TRAP_IGNORE(RollBackAllPendingL());
	
	CScsServer::ConstructL(KStsServerShutdownPeriod); 
		
	StartL(KStsServerName);
	}


CScsSession* CStsServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CStsServer by allocating a new instance of CStsServerSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of CStsServerSession which is owned by the caller.
 */
	{
	return CStsServerSession::NewL(*this, aMessage);
	}

void CStsServer::RollBackAllPendingL()
/**
	Rolls back all pending transactions that have persistent information stored in the transaction
	path in the file system.
	Important note: this function can only be invoked if there are no active transactions in the server as it is
	not intended to roll back active transactions. (ie: intended to be invoked at boot-up time and STS start up only) 
 */
	{
	if(iTransactionWrapperContainer.Count()!=0) 
		{
		User::Leave(KErrInUse); //there can be no active transaction when this function is invoked 
		}
	CIntegrityServices::RollbackAllL();
	}

TInt TransactionWrapperLinearOrderByIdKey(const TStsTransactionId *id, const CReferenceCountedTransactionWrapper &aSecond)
	{
	return *id-aSecond.TransactionId();
	}

TInt TransactionWrapperLinearOrder(const CReferenceCountedTransactionWrapper &aFirst, const CReferenceCountedTransactionWrapper &aSecond)
	{
	return aFirst.TransactionId()-aSecond.TransactionId();
	}

TStsTransactionId CStsServer::CreateTransactionID()
	{
	TTime currentTime;
	TStsTransactionId transactionID;
	do
		{
		currentTime.UniversalTime();
		transactionID = I64LOW(currentTime.Int64());
		}
    while(IsExistingTransaction(transactionID));
    return transactionID;
	}

TBool CStsServer::IsExistingTransaction(TStsTransactionId aTransactionID)
	{
	return (FindActiveTransaction(aTransactionID)>=0);
	} 

CIntegrityServices* CStsServer::CreateTransactionL()
	{
	TStsTransactionId id = CreateTransactionID();
	CReferenceCountedTransactionWrapper* wrapper = CReferenceCountedTransactionWrapper::NewLC(id);
	iTransactionWrapperContainer.InsertInOrderL(wrapper, TLinearOrder<CReferenceCountedTransactionWrapper>(&TransactionWrapperLinearOrder));
	CleanupStack::Pop(wrapper);
	return wrapper->Attach();
	}

CIntegrityServices* CStsServer::AttachTransactionL(TStsTransactionId aTransactionID)
	{
	TInt pos=FindActiveTransaction(aTransactionID);
	if(pos>=0) //joining an existing active transaction
		{
		return iTransactionWrapperContainer[pos]->Attach();
		}
	User::Leave(KErrNotFound);
	return 0;
	}

void CStsServer::ReleaseTransactionL(CIntegrityServices* &aTransactionPtr, TBool aMarkAsCompleted /*=EFalse*/)
	{
	__ASSERT_DEBUG(aTransactionPtr, User::Invariant());
	TInt pos=FindActiveTransaction(aTransactionPtr->TransactionId());
	__ASSERT_DEBUG(pos>=0, User::Invariant());
	CReferenceCountedTransactionWrapper* wrapper=iTransactionWrapperContainer[pos];
	if(aMarkAsCompleted)
		wrapper->SetCompleted(); //set transaction as completed (this should be done after a succesful commit or rollback) 
	TInt refCount=wrapper->Detach();
	__ASSERT_DEBUG(refCount>=0, User::Invariant());
	if(refCount==0) 
		{
		if(!wrapper->IsCompleted()) //do an auto roll back if and only if the transaction has not been committed or rolled-back previously
			aTransactionPtr->RollBackL();
		delete wrapper;
		iTransactionWrapperContainer.Remove(pos);
		}
	aTransactionPtr=0;
	}
/*
 This function is a slight optimization of CStsServer::ReleaseTransactionL() as it is intended to be called
 on cleanup of a committed or rolled-back transaction that obviously don't need auto rollbak again.
 */
void CStsServer::FinaliseTransactionL(CIntegrityServices* &aTransactionPtr)
	{
	ReleaseTransactionL(aTransactionPtr, ETrue);
	}

TInt CStsServer::FindActiveTransaction(TStsTransactionId aTransactionID)
	{
	return iTransactionWrapperContainer.FindInOrder(aTransactionID, &TransactionWrapperLinearOrderByIdKey);
	}

CPolicyServer::TCustomResult CStsServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)		
	{
	TInt functionId(StripScsFunctionMask(aMsg.Function()));
	CPolicyServer::TCustomResult ret(EFail);
		
	switch(functionId)
		{
		case ERegisterNew:
		case ECreateNew:
		case ERemove:
		case ERegisterTemporary:
		case ECreateTemporary:
		case EOverwrite:
			ret = CheckIfFileModificationAllowedL(aMsg)? EPass : EFail;
			break;
		}
	return ret;
	}

/*static*/TBool CStsServer::CheckIfFileModificationAllowedL(const RMessage2& aMsg)
	{
	RBuf filePath;
	filePath.CreateL(KMaxFileName);
	filePath.CleanupClosePushL();
	aMsg.ReadL(KFilePathIPCSlot, filePath, 0);

	// Retrieve the required capabilities for write access to this path
	TCapabilitySet requiredCapabilities = SecCommonUtils::FileModificationRequiredCapabilitiesL(filePath, aMsg.SecureId());
	
	TBool result = EFalse;
	TBool allFilesRequired = requiredCapabilities.HasCapability(ECapabilityAllFiles);
	TBool tcbRequired = requiredCapabilities.HasCapability(ECapabilityTCB);
	
	// Test whether the client has at least one of the required capabilities
	if (allFilesRequired)
		result = aMsg.HasCapability(ECapabilityAllFiles);
	if (!result && tcbRequired)
		result = aMsg.HasCapability(ECapabilityTCB);
	if (!allFilesRequired && !tcbRequired)
		result = ETrue;
	
	CleanupStack::PopAndDestroy(&filePath);
	return result;
	}	
}//end of namespace Usif
