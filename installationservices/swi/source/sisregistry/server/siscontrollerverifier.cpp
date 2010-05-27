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
* CSisControllerVerifier class implementation
* @released
* @internalComponent
*
*/


#include <s32mem.h>

#include "log.h"
#include "arrayutils.h"
#include "cleanuputils.h"
#include "siscontrollerverifier.h"
#include "sisregistryserversession.h"
#include "dessisdataprovider.h"
#include "securitymanager.h"

using namespace Swi;

CSisControllerVerifier* CSisControllerVerifier::NewL(const RMessage2& aMessage)
	{
	CSisControllerVerifier* self = new (ELeave) CSisControllerVerifier(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CSisControllerVerifier::CSisControllerVerifier(const RMessage2& aMessage)
				:CActive(EPriorityStandard), 
				iMessage(aMessage),
				isVerified(EFalse),
				iIsEmbedded(ETrue)
	{
	}

void CSisControllerVerifier::ConstructL()
	{
	CActiveScheduler::Add(this);
	iSecurityManager = Swi::CSecurityManager::NewL();
	}
	

CSisControllerVerifier::~CSisControllerVerifier()
	{
	Cancel();
	delete iSecurityManager;
	delete iDesProvider;
	delete iController;
	
	iPkixResultsOut.ResetAndDestroy();
	iCertsOut.ResetAndDestroy();
	iX509RootCertArray.ResetAndDestroy();
	}
	
void CSisControllerVerifier::RunL()
	{
	if(KErrNone != iStatus.Int())
		{
		iMessage.Complete(iStatus.Int());
		return;
		}
	
	if(iResultOut != EValidationSucceeded)
		{
		isVerified = EFalse;
		}
	else
		{
		if(iCurrentController >= 0)
			{
			VerifyControllerL((*iControllers)[iCurrentController--]);
			return;
			}
		
		isVerified = ETrue;
		}
		
	CompleteRequestL();
	}

void CSisControllerVerifier::CompleteRequestL()
	{
	TPckg<TBool> packageResult(isVerified);

	iMessage.WriteL(EIpcArgument0, packageResult);
	iMessage.Complete(KErrNone);
	}
	
TInt CSisControllerVerifier::RunError(TInt)
	{
	return KErrNone;
	}

void CSisControllerVerifier::DoCancel()
	{
	iSecurityManager->DoCancel();
	}

void CSisControllerVerifier::VerifyControllerL(RPointerArray<HBufC8>& aControllers)
	{
	DEBUG_PRINTF(_L("CSisRegistry::VerifyControllerSignature"));
	
	TInt srcLen = iMessage.GetDesLengthL(EIpcArgument1);
	HBufC8* buffer = HBufC8::NewLC(srcLen);
	TPtr8 dest = buffer->Des();

	// read data in the buffer
	iMessage.ReadL(EIpcArgument1, dest, 0);

	// lets store all in a stream
	RDesReadStream stream(dest);
    CleanupClosePushL(stream);

	iX509RootCertArray.ResetAndDestroy();
	InternalizePointerArrayL(iX509RootCertArray, stream);

	CleanupStack::PopAndDestroy(2, buffer);
	
	iControllers = &aControllers;
	iCurrentController = iControllers->Count() - 1;
	VerifyControllerL((*iControllers)[iCurrentController--]);
	}

void CSisControllerVerifier::VerifyControllerL(HBufC8* aRawController)
	{
	delete iDesProvider;
	delete iController;
	
	iDesProvider = CDesDataProvider::NewL(*aRawController);
	// read the controller
	iController = Sis::CController::NewL(*iDesProvider, Sis::EAssumeType);
	
	// If user provided certificates are there
	if(0 != iX509RootCertArray.Count())
		{
		iSecurityManager->SetRootCerts(&iX509RootCertArray);
		}

	// Clean the output files
	iPkixResultsOut.ResetAndDestroy();
	iCertsOut.ResetAndDestroy();
	
	TBool checkDateAndTime;
	TPckg<TBool> pkgDateCheck(checkDateAndTime);
	iMessage.ReadL(EIpcArgument2, pkgDateCheck);
	
	SetActive();
	iSecurityManager->VerifyControllerL(*aRawController,
										*iController,
										&iResultOut,
										iPkixResultsOut,
										iCertsOut,
										&iCapabilitySetOut,
										iAllowUnsigned,
										iIsEmbedded,
										iStatus,
										checkDateAndTime);

	}
	

