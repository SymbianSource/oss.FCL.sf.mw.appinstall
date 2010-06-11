/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @released
 @internalTechnology
*/

#include "log.h"

#include "securitymanager.h"
#include "sistruststatus.h"
#include "sisrevocationmanager.h"
#include "sisregistryserversession.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "swi/msisuihandlers.h"

_LIT(KMyModuleName, "_SISREVOCATION_MANAGER_");

using namespace Swi;

//
// Life Cycle methods
//

EXPORT_C CSisRevocationManager* CSisRevocationManager::NewL(CSisRegistrySession& aSession)
	{
	CSisRevocationManager* self = CSisRevocationManager::NewLC(aSession);
	CleanupStack::Pop(self);
	return self;
	}
 
EXPORT_C CSisRevocationManager* CSisRevocationManager::NewLC(CSisRegistrySession& aSession)
	{
	CSisRevocationManager* self = new(ELeave) CSisRevocationManager(aSession);
	CleanupStack::PushL(self);
	return self;
	}

CSisRevocationManager::CSisRevocationManager(CSisRegistrySession& aSession) : 
    CActive(EPriorityNormal),
    iSession(&aSession)
	{
	CActiveScheduler::Add(this);	
	}

CSisRevocationManager::~CSisRevocationManager()
	{
	// Remove active object from active scheduler
	Deque(); 
	Cleanup();
	}

//
// Active Objects methods
//

void CSisRevocationManager::RunL()
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - CSisrevocationManager::RunL (State: %d.)"), iState);
	
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());     // Hop into RunError()
		}
		
	switch (iState)
		{ 			
		case EVerifyChains:
			VerifcationRequestL();
			break;
			
		case ERevocationCheck:
			{
			switch (iSignatureValidationResult)
				{
				case EValidationSucceeded:
					PerformOcspRequestL();	
					break;
				default:
					iState = ERevocationComplete;
					// Self complete
					TRequestStatus* status  = &iStatus;
					User::RequestComplete(status, KErrNone);
					SetActive();
					break;
				}	
			}

			break;
			
		case ERevocationComplete:
			{
			SetTrustStatusL();	
			
			// Complete the client
			iMessage.Complete(KErrNone);	
            Cleanup();	
			}
			break;
			
		default:
			{
			User::Panic(KMyModuleName, 1); 		
			}
		}
	}

void CSisRevocationManager::DoCancel()
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Cancelling Revocation Manager"));
	
	if (iSecurityManager)
	    {
		iSecurityManager->Cancel();
		}
	if (iState != EIdle)
	    {
	    iMessage.Complete(KErrCancel);
	    iState = EIdle;	
	    }
	}

TInt CSisRevocationManager::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Revocation Manager Failed with error code %d."), aError);
	
	iMessage.Complete(aError);	
	Cleanup();
	return KErrNone; 
	}

//
// Business methods
//	
EXPORT_C void CSisRevocationManager::RevocationStatusRequestL(
														HBufC8* aRawController,
                                                        const Sis::CController* aController,
                                                        TSisTrustStatus& aTrustStatus,
                                                        const RArray<TInt>& aCertChainIndices, 
													    const TDesC8& aOcspUri,
														const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KMyModuleName, KErrInUse));
	ASSERT(EIdle == iState);
	
	iStatus = KRequestPending;
	iRawController = aRawController;
	iController = aController;
	iChains = aController->SignatureCertificateChains();	
	iSecurityManager = CSecurityManager::NewL();	
	
	// Store caller's request status
	iMessage = aMessage;

	iState = EVerifyChains;
		
	iTrustStatus = aTrustStatus;
	
	iOcspUri = aOcspUri.AllocL();
	
	iCertChainIndices = aCertChainIndices;
	
	// Self complete
	TRequestStatus* status  = &iStatus;
	User::RequestComplete(status, KErrNone);

	SetActive();
	}

void CSisRevocationManager::VerifcationRequestL()
	{
	ASSERT(EVerifyChains == iState);
	
	// Get current controller data.
	TPtrC8 data(iRawController->Mid(iController->DataOffset()));

	iSecurityManager->ReverifyControllerL(data,
										  *iController,
										  iCertChainIndices,
										  &iSignatureValidationResult,
										  iValidationResultsOut,
										  iEndCertificates,
										  &iCapabilitySet,
										  iAllowUnsigned,
										  iStatus);	
	
	iState = ERevocationCheck;
	SetActive();
	}

void CSisRevocationManager::PerformOcspRequestL()
	{
	iIap = 0;
	iSecurityManager->PerformOcspL(iOcspUri->Des(),
								   iIap,
								   &iOcspMsg,
								   iOcspOutcomeOut,
								   iEndCertificates,
								   iStatus);
	iState = ERevocationComplete;
	SetActive();
	}
	
void CSisRevocationManager::SetTrustStatusL()
	{		
	TBool validated = EFalse;
	
	TValidationStatus validationStatus = EUnknown;
	
	switch (iSignatureValidationResult)
		{	
		case EValidationSucceeded:
			validated = ETrue;
			validationStatus = EValidatedToAnchor;
			break;
			
		case ESignatureSelfSigned:
			validated = ETrue;
			validationStatus = EValidated;			
			break;		

		case ESignatureNotPresent:
			validationStatus = EUnsigned;
			break;	
			
        case ECertificateValidationError:
		case ENoCertificate:
		case ENoCodeSigningExtension:
		case ENoSupportedPolicyExtension:
		case ESignatureCouldNotBeValidated:
		case EMandatorySignatureMissing:
            validationStatus = EInvalid;
			break;
		default:
			// BC break, unknown validation code, abort
			User::Leave(KErrNotSupported);
			break;			
		}
		
	TRevocationStatus revocationStatus = EUnknown2;
		
	if (validated)
		{
	
		for (TInt index = 0; index < iOcspOutcomeOut.Count(); index++)
			{
			if (iOcspOutcomeOut[index]->iResult == OCSP::EUnknown)
				{	
				switch (iOcspOutcomeOut[index]->iStatus)	
					{	
					case OCSP::ETransportError:
					case OCSP::EInvalidURI:
					case OCSP::EServerInternalError:
					case OCSP::ETryLater:
					case OCSP::EMissingCertificates:
					case OCSP::EResponseSignatureValidationFailure:
						// Possibly transient error, may retry.
						revocationStatus = EOcspTransient;
						break;
		
					case OCSP::EClientInternalError:
					case OCSP::ENoServerSpecified:
					case OCSP::EMalformedResponse:
					case OCSP::EUnknownResponseType:
					case OCSP::EUnknownCriticalExtension:
					case OCSP::ESignatureRequired:
					case OCSP::EClientUnauthorised:
					case OCSP::EThisUpdateTooLate:
					case OCSP::EThisUpdateTooEarly:
					case OCSP::ENextUpdateTooEarly:
					case OCSP::ECertificateNotValidAtValidationTime:
					case OCSP::ENonceMismatch:	
					case OCSP::EMissingNonce:		
					case OCSP::EMalformedRequest:
						// This is a permanent error, unable to retry.
						revocationStatus = EOcspUnknown;
						break;
		
					case OCSP::EValid:
                        revocationStatus = EOcspUnknown;
						break;

						// not a valid status for this state
					default:
						// Unknown value, must be a BC break!
						User::Leave(KErrNotSupported);
						break;			
					}	
					
				break;
				}
			else if (iOcspOutcomeOut[index]->iResult == OCSP::ERevoked)
				{
				revocationStatus = EOcspRevoked;
				break;		
				}
			else 
				{
				// OCSP of chain is good
				revocationStatus = EOcspGood;
				}	
			}
		}
	else // not validated
		{	
		revocationStatus = EOcspNotPerformed;
		}	

	
	// set trust status		
	TTime time;
	time.UniversalTime();
	iTrustStatus.SetLastCheckDate(time);
	
	iTrustStatus.SetValidationStatus(validationStatus);		
	iTrustStatus.SetRevocationStatus(revocationStatus);
	if (validated 
	    && (revocationStatus == EOcspGood 
	        || revocationStatus == EOcspUnknown 
	        || revocationStatus == EOcspRevoked ))
		{
		iTrustStatus.SetResultDate(iTrustStatus.LastCheckDate());
		}

	iSession->UpdateTrustStatusL(iController->Info().Uid().Uid(),
	                            iTrustStatus);
	}

void CSisRevocationManager::Cleanup()
	{
    iState = EIdle;
//	iMessage = 0;
	
    delete iController;
	iController = 0;
	
    delete iRawController;
	iRawController = 0;
        
	iEndCertificates.ResetAndDestroy();			
    
    iOcspOutcomeOut.ResetAndDestroy();
    
    iValidationResultsOut.ResetAndDestroy();	
    
    delete iOcspUri;
    iOcspUri = 0;

    delete iSecurityManager;
	iSecurityManager = 0;
	}
