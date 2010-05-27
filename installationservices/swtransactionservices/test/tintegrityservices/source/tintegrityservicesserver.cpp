/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tintegrityservicesserver.h"
#include "tintegrityservicescommon.h"
#include "integrityservices.h"
#include <e32debug.h>

_LIT(KFailAddingNewFile, "EFailAddingNewFile");
_LIT(KFailRemovingFile, "EFailRemovingFile");
_LIT(KFailAddingTempFile, "EFailAddingTempFile");
_LIT(KFailRestoringFile, "EFailRestoringFile");
_LIT(KFailDeletingFile, "EFailDeletingFile");
_LIT(KFailInstallComplete, "EFailInstallComplete");
_LIT(KFailNewFilesRemoved, "EFailNewFilesRemoved");
_LIT(KFailOldFilesRestored, "EFailOldFilesRestored");
_LIT(KFailTempFilesRemoved, "EFailTempFilesRemoved");
_LIT(KFailBackupFilesRemoved, "EFailBackupFilesRemoved");

_LIT(KFailBeforeJournal, "EBeforeJournal");
_LIT(KFailAfterJournal, "EAfterJournal");
_LIT(KFailBeforeAction, "EBeforeAction");
_LIT(KFailAfterAction, "EAfterAction");

using namespace Usif;

static void PanicClient(const RMessagePtr2& aMessage, 
						TIntegrityServicesServerPanic aPanic)
	{
	aMessage.Panic(KIntegrityServicesServerName, aPanic);
	}

/////
///// CIntegrityServicesServer
/////

CIntegrityServicesServer* CIntegrityServicesServer::NewLC()
	{
	CIntegrityServicesServer* self = new(ELeave) CIntegrityServicesServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CIntegrityServicesServer::AddSession()
	{
	++iSessionCount;
	iShutdown->Cancel();
	}
	
void CIntegrityServicesServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		RDebug::Print(_L("Server shutting down"));
		}
	}

CIntegrityServicesServer::~CIntegrityServicesServer()
	{
	delete iShutdown;
	RDebug::Print(_L("Server destructor"));
	}

CIntegrityServicesServer::CIntegrityServicesServer() : CServer2(EPriorityNormal, ESharableSessions)
	{
	}

void CIntegrityServicesServer::ConstructL()
	{
	StartL(KIntegrityServicesServerName);
	
	iShutdown = CShutdownTimer::NewL();
	
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	}

CSession2* CIntegrityServicesServer::NewSessionL(const TVersion& /*aVersion*/, 
						 const RMessage2& /*aMessage*/) const
	{
	  return CIntegrityServicesSession::NewL();
	}

CIntegrityServicesSession* CIntegrityServicesSession::NewL()
	{
	CIntegrityServicesSession* self = CIntegrityServicesSession::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
	
CIntegrityServicesSession* CIntegrityServicesSession::NewLC()
	{
	CIntegrityServicesSession* self = new(ELeave) CIntegrityServicesSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CIntegrityServicesSession::CIntegrityServicesSession() : CSession2()
	{
	}
	
void CIntegrityServicesSession::ConstructL()
	{
	iIntegrityServices = CIntegrityServices::NewL(0);
	}
	
void CIntegrityServicesSession::CreateL()
	{
	RDebug::Print(_L("Session CreateL"));
	Server().AddSession();
	}



CIntegrityServicesSession::~CIntegrityServicesSession()
	{
	delete iIntegrityServices;
	Server().DropSession();
	}

CIntegrityServicesServer& CIntegrityServicesSession::Server()
	{
	return *static_cast<CIntegrityServicesServer*>(
		const_cast<CServer2*>(CSession2::Server()));
	}

/////
///// CIntegrityServicesSession		
/////

void CreateTempTestFileL(TDesC& aFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile tempFile;
	CleanupClosePushL(tempFile);
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailAddingTempFile, CIntegrityServices::EBeforeAction, aFileName);
	User::LeaveIfError(tempFile.Create(fs, aFileName, EFileWrite));
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailAddingTempFile, CIntegrityServices::EAfterAction, aFileName);
	CleanupStack::PopAndDestroy(2, &fs);
	}

void CreateNewTestFileL(TDesC& aFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);	
	if (aFileName.Length() > 0 && aFileName[aFileName.Length() - 1] == KPathDelimiter)
		{  // A new test directory is created
		User::LeaveIfError(fs.MkDir(aFileName));
		}
	else
		{  // A new test file is created
		RFile newFile;
		CleanupClosePushL(newFile);
		CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailAddingNewFile, CIntegrityServices::EBeforeAction, aFileName);
		User::LeaveIfError(newFile.Create(fs, aFileName, EFileWrite));
		CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailAddingNewFile, CIntegrityServices::EAfterAction, aFileName);
		CleanupStack::PopAndDestroy(&newFile);		
		}
	CleanupStack::PopAndDestroy(&fs);	
	}

void SetSimulatedFailureL(TPtrC16 aFailType, TPtrC16 aFailPosition, TPtrC16 aFailFileName)
	{
	if(aFailType.CompareF(KFailAddingNewFile) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailAddingNewFile;
		}
	else if(aFailType.CompareF(KFailRemovingFile) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailRemovingFile;
		}
	else if(aFailType.CompareF(KFailAddingTempFile) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailAddingTempFile;
		}
	else if(aFailType.CompareF(KFailRestoringFile) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailRestoringFile;
		}
	else if(aFailType.CompareF(KFailDeletingFile) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailDeletingFile;
		}
	else if(aFailType.CompareF(KFailInstallComplete) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailInstallComplete;
		}
	else if(aFailType.CompareF(KFailNewFilesRemoved) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailNewFilesRemoved;
		}
	else if(aFailType.CompareF(KFailOldFilesRestored) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailOldFilesRestored;
		}
	else if(aFailType.CompareF(KFailTempFilesRemoved) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailTempFilesRemoved;
		}
	else if(aFailType.CompareF(KFailBackupFilesRemoved) == 0)
		{
		CIntegrityServices::iFailType = CIntegrityServices::EFailBackupFilesRemoved;
		}
	else
		{
		// default to EFailNone
		CIntegrityServices::iFailType = CIntegrityServices::EFailNone;
		}
	
	if(aFailPosition.CompareF(KFailBeforeJournal) == 0)
		{
		CIntegrityServices::iFailPosition = CIntegrityServices::EBeforeJournal;
		}
	else if(aFailPosition.CompareF(KFailAfterJournal) == 0)
		{
		CIntegrityServices::iFailPosition = CIntegrityServices::EAfterJournal;
		}
	else if(aFailPosition.CompareF(KFailBeforeAction) == 0)
		{
		CIntegrityServices::iFailPosition = CIntegrityServices::EBeforeAction;
		}
	else if(aFailPosition.CompareF(KFailAfterAction) == 0)
		{
		CIntegrityServices::iFailPosition = CIntegrityServices::EAfterAction;
		}
	else
		{
		if(CIntegrityServices::iFailType != CIntegrityServices::EFailNone)
			{
			// scripting error, FailType specified without FailPosition
			User::Leave(KErrCorrupt);
			}
		}
		
	CIntegrityServices::iFailFileName = aFailFileName;
	CIntegrityServices::iIsFailureTestingEnabled = CIntegrityServices::iFailType != CIntegrityServices::EFailNone;
	}

void CIntegrityServicesSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case EAdd:
			{
			RDebug::Print(_L("ServiceL EAdd"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->RegisterNewL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ERemove:
			{
			RDebug::Print(_L("ServiceL ERemove"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->RemoveL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			
			break;
			}
		case ETemporary:
			{
			RDebug::Print(_L("ServiceL ETemporary"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->RegisterTemporaryL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ECommit:
			{
			RDebug::Print(_L("ServiceL ECommit"));
			TRAPD(err, iIntegrityServices->CommitL());
			aMessage.Complete(err);
			break;
			}
		case ERollBack:
			{
			RDebug::Print(_L("ServiceL ERollBack"));
			TBool all;
			TPckg<TBool> allTransactions(all);
			aMessage.ReadL(0, allTransactions);
			TInt error=KErrNone;
			
			if(all)
				{
				CIntegrityServices::RollbackAllL();
				}
			else
				{
				TRAPD(err, iIntegrityServices->RollBackL());
				error=err;
				}
			aMessage.Complete(error);
			
			break;
			}
			
		case ESetSimulatedFailure:
			{
			HBufC16* aFailType = HBufC16::NewLC(256);
			HBufC16* aPosition = HBufC16::NewLC(256);
			HBufC16* aFailFileName = HBufC16::NewLC(256);
			TPtr16 failType(aFailType->Des());
			TPtr16 position(aPosition->Des());
			TPtr16 failFileName(aFailFileName->Des());
			
			aMessage.ReadL(0, failType);
			aMessage.ReadL(1, position);
			aMessage.ReadL(2, failFileName);
			RDebug::Print(_L("ServiceL ESetSimulatedFailure %S %S %S"), &aFailType, &aPosition, &aFailFileName);
			TRAPD(err, SetSimulatedFailureL(failType, position, failFileName));
			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(3, aFailType);
			break;
			}
		case ECreateNewTestFile:
			{
			RDebug::Print(_L("ServiceL ECreateNewTestFile"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, CreateNewTestFileL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ECreateTempTestFile:
			{
			RDebug::Print(_L("ServiceL ECreateTempTestFile"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, CreateTempTestFileL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
	default:
		{
		PanicClient(aMessage, EPanicIntegrityServicesServerIllegalFunction);
		break;
		}
		}
	}

void CIntegrityServicesSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, EPanicIntegrityServicesServerIllegalFunction);
		}
	CSession2::ServiceError(aMessage, aError);
	}

/////
///// CShutdownTimer
/////
inline CShutdownTimer::CShutdownTimer() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CShutdownTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CShutdownTimer::Start()
	{
	After(KShutdownDelay);
	}

CShutdownTimer* CShutdownTimer::NewL()
	{
	CShutdownTimer* self = new(ELeave) CShutdownTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Initiate server exit when the timer expires
void CShutdownTimer::RunL()
	{
	CActiveScheduler::Stop();
	}
