/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <f32file.h>

#include "securitymanager.h"
#include "swi/sishash.h"
#include "swi/sisdataprovider.h"
#include "swi/sissignaturealgorithm.h"
#include "swi/sissignaturecertificatechain.h"
#include "swi/siscertificatechain.h"
#include "swi/sisinfo.h"
#include "swi/sissupportedoptions.h"
#include "swi/sissupportedlanguages.h"
#include "swi/sisprerequisites.h"
#include "swi/sislogo.h"
#include "swi/sisproperties.h"
#include "swi/sisinstallblock.h"
#include "swi/sistruststatus.h"
#include "hashcontainer.h"
#include "certchainconstraints.h"
#include "devinfosupportclient.h"

#include "log.h"

// PKIX dependencies
#include <pkixcertchain.h>
#include <x509keys.h> 
#include <ocsp.h>
#include <securitydefsconst.h>

#include <ccertattributefilter.h>
#include <ct/mcttoken.h>
//#include <mctcertstore.h>
#include "cfstokentypeclient.h"

// Crypto dependencies
#include <hash.h>
#include <swicertstore.h>

// SecMan includes
#include "chainvalidator.h"
#include "certificateretriever.h"
#include "signatureverifier.h"
#include "securitypolicy.h"
#include "revocationhandler.h"

const TInt KFileBufferSize = 1024;  // 1k buffer used to read the data to be hashed
_LIT(KSecurityManagerName, "_Security_Manager_");


using namespace Swi;

//
// Life Cycle methods
//

EXPORT_C CSecurityManager* CSecurityManager::NewL()
	{
	CSecurityManager* self = CSecurityManager::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
 
EXPORT_C CSecurityManager* CSecurityManager::NewLC()
	{
	CSecurityManager* self = new(ELeave) CSecurityManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSecurityPolicy& CSecurityManager::SecurityPolicy() const
	{
	return *iSecPolicy;
	}

CSecurityManager::CSecurityManager() : CActive(EPriorityNormal) 
	{
	CActiveScheduler::Add(this);	
	}

CSecurityManager::~CSecurityManager()
	{	
	Deque();

	if (iCurrentPkixChain)
		{
		delete iCurrentPkixChain;
		}

	if (iCertStore)
		{
		iCertStore->Release();
		}
	
	if (iCertificateRetriever)
		{
		delete iCertificateRetriever;
		}
		
	if (iChainValidator)
		{
		delete iChainValidator;
		}
		
	if (iRevocationHandler)
		{
		delete iRevocationHandler;
		}
		
	iValidPkixChains.ResetAndDestroy();
	iUntrustedRoots.ResetAndDestroy();
	iDeviceIDs.ResetAndDestroy();
	TInt count = iTrustedRoots.Count();
	// Release certificates from the list
	for (TInt i = 0; i < count; ++i)
		{
		iTrustedRoots[0]->Release(); 
		iTrustedRoots.Remove(0);
		}
	iTrustedRoots.Close();	
	iCertMetaInfo.Reset();
	iFs.Close();
	}


void CSecurityManager::ConstructL()
	{	
	User::LeaveIfError(iFs.Connect());
	iSecPolicy = CSecurityPolicy::GetSecurityPolicyL();	
	iCertStore=CSWICertStore::NewL(iFs);
	}
	
	
//
// Active Objects methods
//

void CSecurityManager::RunL()
	{
	DEBUG_PRINTF3(_L8("Security Manager - RunL(). State: %d, Status: %d."),
		iState, iStatus.Int());
	
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());     // Hop into RunError()
		}
		
	switch (iState)
		{ 			
		case ERetrievedTrustedRoots:
			{
			iCertMetaInfo.Reset();
			TInt trustedCertCount = iTrustedRoots.Count();
			for (TInt i = 0; i < trustedCertCount; ++i)
				{
				CCTCertInfo& certInfo=*iTrustedRoots[i];
				DEBUG_CODE_SECTION(			
					DEBUG_PRINTF2(_L("Security Manager - SWI certstore contains trust anchor certificate '%S'"),
						&(certInfo.Label()));
						);
				TCertMetaInfo metaInfo = iCertStore->CertMetaInfoL(certInfo);
				iCertMetaInfo.AppendL(metaInfo);
				if (metaInfo.iIsMandatory)		
					{
					iMandatoryCertDNCount++;
					DEBUG_CODE_SECTION(			
					DEBUG_PRINTF2(_L("Security Manager - SWI certstore contains mandatory certificate '%S'"),
						&(certInfo.Label()));
						);
					}
				if (metaInfo.iIsSystemUpgrade)		
					{
					DEBUG_CODE_SECTION(			
					DEBUG_PRINTF2(_L("Security Manager - SWI certstore contains system upgrade certificate '%S'"),
						&(certInfo.Label()));
						);
					}
				}
			// After we get here, we have retrieved mandatory certificates
			// We may have an unsigned sis file in which case we do not want to verify the block
			// since there isn't one, so bail here with the correct error
			if (iChains.Count() == 0)
					{
					if (iMandatoryCertDNCount == 0)
						{
						*iResult = ESignatureNotPresent; // No mandatory certs, so just say unsigned
						}
					else
						{
						*iResult = EMandatorySignatureMissing; // more important error code overrides
						}
					
					User::RequestComplete(iClientStatus, KErrNone);							
					break;
					}
	
		
			VerifyBlockL(iCurrentChain);			
			}
			break;
			
		case EValidatingChain :
			{
			CPKIXValidationResultBase* result = (*iValidationResultsOut)[iCurrentChain];
			::TValidationStatus resultStatus = result->Error(); 	

			DEBUG_PRINTF3(_L8("Security Manager - Certificate validation result was %d. (If applicable, error on certificate %d.)"),
				resultStatus.iReason, resultStatus.iCert);

			iCurrentChain++; // Next chain to validate

			if (resultStatus.iReason != EValidatedOK)
				{
				// we can discard the invalid cert chain
				delete iCurrentPkixChain;
				iCurrentPkixChain = NULL;
				
				// ooops
				if (iChains.Count() > iCurrentChain)   // This is the ValidateANY policy
					{
					VerifyBlockL(iCurrentChain);
					break;						
					}
				}
			else
				{
				iHasValidated = ETrue; // At least one chain has been validated! Hurrah!
				
				// check if a chain has validated that is not self signed
				if (*iResult != ESignatureSelfSigned)
					{
					iHasValidatedTrusted = ETrue;
					}
				
			    // From here on the chain has been validated successfully
						
			    iTotalCapabilitiesOut->Union(iCurrentCapabilities); // Update the total capabilities
			
			    // Reduce the count from the list of mandatory certs
			    UpdateListOfMissingRequiredCertsL(iCurrentPkixChain->Cert(iCurrentPkixChain->Count()-1));
				
				// Updage the System Upgrade status of the Certificate
				UpdateSystemUpgradeCertStatusL(iCurrentPkixChain->Cert(iCurrentPkixChain->Count()-1));
				
			    User::LeaveIfError(iValidPkixChains.Append(iCurrentPkixChain));
			    // this chain will be used for OCSP checking if required, so record it.
			    iController->AddChainIndex(iCurrentChain-1);
			    
			    iCurrentPkixChain = NULL;
				}

			if (iCurrentChain == iChains.Count())
				{
				if (iMandatoryCertDNCount == 0)
					{
					// Mandatory certs prerequisites met				

					if (iHasValidated)
						{
						//Build the certificate chain constraints
						CCertChainConstraints* certChainConstraints(0);
						TRAPD(err, certChainConstraints=CCertChainConstraints::NewL(iValidPkixChains));
						if (err)
							{
							*iResult = ECertificateValidationError;
							User::RequestComplete(iClientStatus, KErrNone);
							return;
							}

						*iResult = iHasValidatedTrusted ? EValidationSucceeded : ESignatureSelfSigned;
						
						//Pass the certificate chain constraints ownership to controller
						Sis::CController* controller=const_cast<Sis::CController*>(iController);
						controller->SetCertChainConstraints(certChainConstraints);

						//Set the dev cert found flag if dev cert first time found
						if (iDevCertWarningState==ENoDevCerts && 
							(certChainConstraints->SIDsAreConstrained() 
							||certChainConstraints->VIDsAreConstrained()
							||certChainConstraints->DeviceIDsAreConstrained()
							||certChainConstraints->CapabilitiesAreConstrained()))
							{
							DEBUG_PRINTF(_L("Security Manager - At least one certificate chain contains a devcert."));
							iDevCertWarningState=EFoundDevCerts;
							}

						//Check if the device id is contrained and the device Id has been retrieved
						if (certChainConstraints->DeviceIDsAreConstrained() && iDeviceIDs.Count()==0)
							{
				 			//Retrieve and Save the device ID list then complete
				 			RDeviceInfo deviceInfo;
				 			User::LeaveIfError(deviceInfo.Connect());
				 			CleanupClosePushL(deviceInfo);
 							const RPointerArray<HBufC>& tempbuf=deviceInfo.DeviceIdsL();
				 			for (TInt i=0;i<tempbuf.Count();i++)
					 			{
					 			HBufC* element=tempbuf[i]->AllocLC();
				  				iDeviceIDs.AppendL(element);
				  				CleanupStack::Pop(element);	 				
					 			}
					 		CleanupStack::PopAndDestroy(&deviceInfo);					
							}						
						iState = EChecksDone;
						TRequestStatus* status = &iStatus;
						User::RequestComplete(status, KErrNone);
						SetActive();
						return;
						}
					}

				if (iMandatoryCertDNCount != 0)
					{
					*iResult = EMandatorySignatureMissing;  // We did not meet mandatory cert requirements!
					}
				else
					{
					if (!iHasValidated) 
						{
						*iResult = ECertificateValidationError; // No chain ever validated 
						}
					}
	
				User::RequestComplete(iClientStatus, KErrNone);							
				}
			else
				{
				VerifyBlockL(iCurrentChain);
				return;					
				}
			}
			break;

		case ERevalidatingChain:
			{
			TInt chainIndex = (*iChainListIndices)[iCurrentChain];
			
			CPKIXValidationResultBase* result = (*iValidationResultsOut)[iCurrentChain];
			::TValidationStatus resultStatus = result->Error(); 	

			DEBUG_PRINTF3(_L8("Security Manager - Certificate validation result was %d. (If applicable, error on certificate %d.)"),
				resultStatus.iReason, resultStatus.iCert);
				
			iCurrentChain++; // Next chain to validate

			if (resultStatus.iReason != EValidatedOK)
				{
				// We can discard the invalid chain.
				delete iCurrentPkixChain;
				iCurrentPkixChain = NULL;
				
				// ooops
				if (iChains.Count() > chainIndex)  	
					{ // This is the ValidateANY policy
					VerifyBlockL(chainIndex);
					break;						
					}
				}
			else
				{
				iHasValidated = ETrue; // At least one chain has been validated! Hurrah!
				
				// check if a chain has validated that is not self signed
				if (*iResult != ESignatureSelfSigned)
					{
					iHasValidatedTrusted = ETrue;
					}
				
				// From here on the chain has been validated successfully
						
				iTotalCapabilitiesOut->Union(iCurrentCapabilities); // Update the total capabilities
			
				// Reduce the count from the list of mandatory certs
				UpdateListOfMissingRequiredCertsL(iCurrentPkixChain->Cert(iCurrentPkixChain->Count()-1));
			
				// Updage the System Upgrade status of the Certificate
				UpdateSystemUpgradeCertStatusL(iCurrentPkixChain->Cert(iCurrentPkixChain->Count()-1));
				
				iValidPkixChains.AppendL(iCurrentPkixChain);
					
				iCurrentPkixChain = NULL;
				}

			if (chainIndex == iChains.Count())
				{
				if (iHasValidated)
					{
					//Build the certificate chain constraints
					CCertChainConstraints* certChainConstraints(0);
					TRAPD(err, certChainConstraints=CCertChainConstraints::NewL(iValidPkixChains));
					if (err)
						{
						*iResult = ECertificateValidationError;
						User::RequestComplete(iClientStatus, KErrNone);
						return;
						}
					else
						{
						*iResult = iHasValidatedTrusted ? EValidationSucceeded : ESignatureSelfSigned;								
						}

					//Pass the certificate chain constraints ownership to controller
					Sis::CController* controller=const_cast<Sis::CController*>(iController);
					controller->SetCertChainConstraints(certChainConstraints);
					
					//Check if the device id is contrained and the device Id has been retrieved
					if (certChainConstraints->DeviceIDsAreConstrained() && iDeviceIDs.Count()==0)
						{
				 		//Retrieve and Save the device ID list then complete
				 		RDeviceInfo deviceInfo;
				 		User::LeaveIfError(deviceInfo.Connect());
				 		CleanupClosePushL(deviceInfo);
 						const RPointerArray<HBufC>& tempbuf=deviceInfo.DeviceIdsL();
				 		for (TInt i=0;i<tempbuf.Count();i++)
					 		{
							HBufC* element=tempbuf[i]->AllocLC();
			  				iDeviceIDs.AppendL(element);
			  				CleanupStack::Pop(element);	 				
					 		}
					 	CleanupStack::PopAndDestroy(&deviceInfo);					
						}						
					iState = EChecksDone;
					TRequestStatus* status = &iStatus;
					User::RequestComplete(status, KErrNone);
					SetActive();
					return;
					}

				else
					{
					*iResult = ECertificateValidationError; // No chain ever validated 
					}

				User::RequestComplete(iClientStatus, KErrNone);							
				}
			else
				{
				VerifyBlockL(chainIndex);
				return;					
				}
			}
			break;

		case EOCSPCheck:
			{
			iState = EChecksDone;
			ProcessOcspOutcomesL();
			
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();			
			}
			break;
			
		case EChecksDone:
			{
			// If the validation is done by certs passed by third party(API user)
			// instead of cert-store then even on successful validation
			// the result of validation is ESignatureSelfSigned. 
			// But if iUntrustedRoots count is not zero then the result is
			// really ESignatureSelfSigned.
			if (NULL != iRootCerts && iHasValidated)
				{
				*iResult = EValidationSucceeded;
				}
			// Complete the client and be happy
			User::RequestComplete(iClientStatus, KErrNone);			
			}
			break;
			
		default:
			{
			User::Panic(KSecurityManagerName, 1); 		
			}
		}
	}


void CSecurityManager::ProcessOcspOutcomesL() 
	{
	
	 // Identify the worst outcome overall, and the worst in each chain.
 	// The OCSP outcomes are in the same order as the valid chains and
 	// certificates within them, with the number of outcomes per chain
 	// one fewer than the number of certs in the chain.
 

	TInt numOutcomes=iRevocationHandler->TransactionCount();
	TInt currentChain = 0;
 	TInt chainErrorLevel = 0;
 
 	// Since the number of outcomes is 1 fewer than the number of certs in the
 	// PkixChain, the index for the last outcome of the first chain will be
 	// 2 lower than the number of certs in the chain.
 	TInt chainLastOutcomeIndex = iValidPkixChains[0]->Count() - 2;
 	TInt chainWorstOutcome = 0;
 	
	// This variable is used to record the most serious error condition.
	TInt errorLevel = 0;

	DEBUG_PRINTF2(_L8("Security Manager - OCSP check complete. %d OCSP outcomes."), numOutcomes);

	for (TInt i=0; i<numOutcomes; i++)
		{
		const TOCSPOutcome& outcome=iRevocationHandler->Outcome(i);
		
		DEBUG_PRINTF4(_L8("Security Manager - OCSP outcome %d; Status: %d, Result: %d."),
			i, outcome.iStatus, outcome.iResult);
		
		if (outcome.iResult == OCSP::ERevoked)
			{
			// If any certificate is revoked then installation must be aborted
 			// but we still want to identify the worst case for each chain for
 			// the purposes of the error dialog.

			*iRevocationMessage = ECertificateStatusIsRevoked;
			errorLevel = 6;
 			chainWorstOutcome = i;
 			
 			// Skip the rest of the outcomes for this chain.
 			i = chainLastOutcomeIndex;
			}
		else if (outcome.iResult != OCSP::EGood)
			{			
			switch (outcome.iStatus)			
				{
				// permanent errors, no user interaction is required
				case OCSP::ENoServerSpecified:
				case OCSP::EClientInternalError:
				case OCSP::EMalformedRequest:
				case OCSP::EUnknownResponseType:
				case OCSP::EClientUnauthorised:
				case OCSP::EUnknownCriticalExtension:
				case OCSP::EMissingCertificates:
				case OCSP::ESignatureRequired:				
				case OCSP::EThisUpdateTooLate:
				case OCSP::EThisUpdateTooEarly:
				case OCSP::ENextUpdateTooEarly:
				case OCSP::ECertificateNotValidAtValidationTime:				
				case OCSP::ENonceMismatch:
					if (errorLevel < 5)
						{
						errorLevel = 5;
						*iRevocationMessage = EInvalidCertificateStatusInformation;
						}
					if (chainErrorLevel < 5)
 						{
 						chainErrorLevel = 5;
 						chainWorstOutcome = i;
 						}
					break;
				case OCSP::EResponseSignatureValidationFailure:
					if (errorLevel < 4)
						{
						errorLevel = 4;
						*iRevocationMessage = EResponseSignatureValidationFailure;			
						}	
					if (chainErrorLevel < 4)
 						{
 						chainErrorLevel = 4;
 						chainWorstOutcome = i;
						}
					break;
				// permanent errors, ask the user	
				case OCSP::EValid:
					if (errorLevel < 3)
						{
						errorLevel = 3;
						*iRevocationMessage = ECertificateStatusIsUnknown;
						}	
					if (chainErrorLevel < 3)
 						{
 						chainErrorLevel = 3;
 						chainWorstOutcome = i;
 						}
					break;					
				// transient errors - user can retry
				case OCSP::EInvalidURI:					
					if (errorLevel < 2)
						{
						errorLevel = 2;
						*iRevocationMessage = EInvalidRevocationServerUrl;
						}
					if (chainErrorLevel < 2)
 						{
 						chainErrorLevel = 2;
 						chainWorstOutcome = i;
						}
					break;
				case OCSP::ETryLater:
				case OCSP::ETransportError:
				case OCSP::EServerInternalError:
				case OCSP::EMissingNonce:
					if (errorLevel < 1)
						{
						errorLevel = 1;
						*iRevocationMessage = EUnableToObtainCertificateStatus;
						}	
 					if (chainErrorLevel < 1)
 						{
 						chainErrorLevel = 1;
 						chainWorstOutcome = i;
 						}

					break;	
				default:
					// All possible OCSP responses should be checked
					__ASSERT_ALWAYS(EFalse, User::Leave(KErrArgument));
					break;
				}	
			}

 		// If this is the end of the chain, store the worst outcome for the
 		// chain in the output outcome array.
 		if (i == chainLastOutcomeIndex)
 			{
 			TOCSPOutcome* chainOutcome=new(ELeave) TOCSPOutcome(iRevocationHandler->Outcome(chainWorstOutcome));
 			CleanupStack::PushL(chainOutcome);
 			iOcspOutcomeOut->AppendL(chainOutcome);
 			CleanupStack::Pop(chainOutcome);
 
 			// If there are more chains to consider, get set to identify the
 			// worst outcome in the next chain.
 			if (++currentChain < iValidPkixChains.Count())
 				{
 				chainErrorLevel = 0;
 				chainWorstOutcome = i + 1;
 				chainLastOutcomeIndex += iValidPkixChains[currentChain]->Count() - 1;
 				}
 			}

		}
	}

void CSecurityManager::DoCancel()
	{
	DEBUG_PRINTF2(_L8("Security Manager - Cancelling in state %d."), iState);
	
	switch (iState)
		{
		case ERetrievedTrustedRoots:
			{
			iCertificateRetriever->Cancel();
			}
			break;
			
		case EValidatingChain:
		case ERevalidatingChain:
			{
			iChainValidator->Cancel();		
			}
			break;
			
		case EOCSPCheck:
			{
			iRevocationHandler->Cancel();
			}
			break;
			
		case EChecksDone:	
		default: 
			{
			// Do nothing
			}
		}
	if(iClientStatus)
		{
		User::RequestComplete(iClientStatus,KErrCancel);		
		}
	}

TInt CSecurityManager::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Security Manager - RunError(). Error code %d."), aError);
	
	User::RequestComplete(iClientStatus, aError);			
	return KErrNone; 
	}


//
// Business methods
//

EXPORT_C void CSecurityManager::PerformOcspL(const TDesC8& aOcspUri, 
											 TUint32& aIap,
											 TRevocationDialogMessage* aRevocationMessageOut, 
											 RPointerArray<TOCSPOutcome>& aOcspOutcomeOut, 
											 RPointerArray<CX509Certificate>& aCertOut,
											 TRequestStatus& aStatus)
	{	
	Cancel();
	
	DEBUG_PRINTF2(_L8("Security Manager - Performing OCSP with revocation server at %S."),
		&aOcspUri);

	// Reset and re-populate certificate list to contain only end certificates
 	// from valid chains, so that the end cert list can be correlated with the
 	// list of outcomes.
 	aCertOut.ResetAndDestroy();
 	TInt numChains = iValidPkixChains.Count();
 	for (TInt index = 0; index < numChains; index++)
 		{
 		CX509Certificate* certOut = CX509Certificate::NewLC(iValidPkixChains[index]->Cert(0));
 		aCertOut.AppendL(certOut);
 		CleanupStack::Pop(certOut);
 		}
 		
 	DEBUG_PRINTF2(_L8("Security Manager - Validating %d certificate chains for this controller."), numChains);

	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;

	iOcspOutcomeOut = &aOcspOutcomeOut;
	iRevocationMessage = aRevocationMessageOut;
	
	// Make sure to delete earlier object
	if (iRevocationHandler)
		{
		delete iRevocationHandler;
		iRevocationHandler = NULL;
		}	
	iRevocationHandler = CRevocationHandler::NewL(*iCertStore);
	iRevocationHandler->SetDefaultURIL(aOcspUri);
	iState = EOCSPCheck;
	
	iRevocationHandler->SendRequestL(iValidPkixChains, aIap, iStatus);
	
	SetActive();
	}

EXPORT_C void CSecurityManager::VerifyControllerL(
				TDesC8& aRawController,
				const Sis::CController& aController, 
				TSignatureValidationResult* aResultOut, 
				RPointerArray<CPKIXValidationResultBase>& aPkixResultsOut,
				RPointerArray<CX509Certificate>& aCertsOut,
				TCapabilitySet* aCapabilitySetOut,
				TBool& aAllowUnsigned,
				TBool& aIsEmbeddedController,
				TRequestStatus& aStatus,
				TBool aCheckDateAndTime)
	{
	DEBUG_PRINTF(_L8("Security Manager - Validating Controller"));
	
	iResult = aResultOut;
	iChains = aController.SignatureCertificateChains();	
	iController = &aController;
	iEndCertificatesOut = &aCertsOut;
	iValidationResultsOut = &aPkixResultsOut;
	iTotalCapabilitiesOut = aCapabilitySetOut;
	iIsEmbeddedController = aIsEmbeddedController;
	iCheckDateAndTime = aCheckDateAndTime;
	
	// Initialy assume that the chain is not validated
	iHasValidated = EFalse;
	iHasValidatedTrusted = EFalse;
	
	*iResult = ESignatureNotPresent;
	aAllowUnsigned=iSecPolicy->AllowUnsigned();
	
	
	iClientStatus = &aStatus;  // Store caller's request status
	*iClientStatus = KRequestPending;
		
	if (iChains.Count() == 0)
		{
		User::RequestComplete(iClientStatus, KErrNone);	
		return;	
		}
	
	iRawController.Set(aRawController);
	iCurrentChain = 0;
	
	// First retrieve all CA certificates

	delete iCertificateRetriever;
	iCertificateRetriever = NULL;
	iCertificateRetriever = CCertificateRetriever::NewL(*iCertStore);
	
	delete iChainValidator;
	iChainValidator = NULL;
	iChainValidator = CChainValidator::NewL(*iCertStore, *iSecPolicy);
	
	iState = ERetrievedTrustedRoots;
	SetActive();

	// No need to fetch certificates from the certstore if the root certs
	// are already provided by the caller. or
	// The root Certificates already fetched from certstore in case of embedded packages

	if(NULL == iRootCerts && iTrustedRoots.Count() == 0)
		{
		iCertificateRetriever->RetrieveCACertificates(
			iTrustedRoots, iStatus);
		}
	else
		{
		TRequestStatus* tempStatus = &iStatus;
		User::RequestComplete(tempStatus, KErrNone);
		}
	}
	
EXPORT_C void CSecurityManager::ReverifyControllerL(
				TDesC8& aRawController,
				const Sis::CController& aController, 
				const RArray<TInt>& aChainListIndices,
				TSignatureValidationResult* aResultOut, 
				RPointerArray<CPKIXValidationResultBase>& aPkixResultsOut,
				RPointerArray<CX509Certificate>& aCertsOut,
				TCapabilitySet* aCapabilitySetOut,
				TBool& aAllowUnsigned,
				TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Security Manager - Re-Verifying Controller"));
	
	iResult = aResultOut;
	iChains = aController.SignatureCertificateChains();	
	iController = &aController;
	iChainListIndices = &aChainListIndices;
	iEndCertificatesOut = &aCertsOut;
	iValidationResultsOut = &aPkixResultsOut;
	iTotalCapabilitiesOut = aCapabilitySetOut;
	iIsEmbeddedController = ETrue;
	

	*iResult = ESignatureNotPresent;
	aAllowUnsigned = iSecPolicy->AllowUnsigned();
	
	iClientStatus = &aStatus;  // Store caller's request status
	*iClientStatus = KRequestPending;
		
	if (iChains.Count() == 0)
		{
		// This is an unsigned SISX file
		User::RequestComplete(iClientStatus, KErrNone);	
		return;	
		}
	
	iRawController.Set(aRawController);
    iCurrentChain = 0;
    
    delete iCertificateRetriever;
    iCertificateRetriever = NULL;
    iCertificateRetriever = CCertificateRetriever::NewL(*iCertStore);
    
    delete iChainValidator;
    iChainValidator = NULL;
	iChainValidator = CChainValidator::NewL(*iCertStore, *iSecPolicy);

	// We may have an unsigned sis file in which case we do not want to verify the block
	// since there isn't one, so bail here with the correct error
	if (iChains.Count() == 0)
		{
		User::RequestComplete(iClientStatus, KErrNone);							
		}
	else 
		{
		TInt currentChain = (*iChainListIndices)[iCurrentChain];
		VerifyBlockL(currentChain);		
		}	
	}
		
TInt CSecurityManager::SignedSize(TInt iChainsToAdd) const
	{
	TInt64 size = 0;

	const Sis::CInfo& info = iController->Info();	
	size += info.Length() + info.HeaderSize() + info.PaddingSize();

	const Sis::CSupportedOptions& options = iController->SupportedOptions();
	size += options.Length() + options.HeaderSize() + options.PaddingSize();

	const Sis::CSupportedLanguages& languages = iController->SupportedLanguages();
	size += languages.Length() + languages.HeaderSize() + languages.PaddingSize();

	const Sis::CPrerequisites& prerequisites = iController->Prerequisites();
	size += prerequisites.Length() + prerequisites.HeaderSize() + prerequisites.PaddingSize();

	const Sis::CProperties& props = iController->Properties();
	size += props.Length() + props.HeaderSize() + props.PaddingSize();

	const Sis::CLogo* logo = iController->Logo();
	if (logo != NULL)
		{
		size += logo->Length() + logo->HeaderSize() + logo->PaddingSize();		
		}
		
	const Sis::CInstallBlock& installblock = iController->InstallBlock();
	size += installblock.Length() + installblock.HeaderSize() + installblock.PaddingSize();

	for (TInt k = 0; k < iChainsToAdd; k++)
		{
		size += iChains[k]->Length() + iChains[k]->HeaderSize() + iChains[k]->PaddingSize();
		}

	return I64INT(size);	
	}

void CSecurityManager::VerifyBlockL(TInt aChainIndex)
	{		
	DEBUG_PRINTF2(_L8("Security Manager - Validating certificate chain %d."), aChainIndex);

	Sis::CSignatureCertificateChain& chain = *iChains[aChainIndex];	
	
	// First verify the signature
	const RPointerArray<Sis::CSignature> signatures = chain.Signatures();
	
	// Determine the size of the controller data which was signed
	TInt size = SignedSize(aChainIndex);

	const Sis::CCertificateChain& certChain = chain.CertificateChain();
	const TPtrC8 data = certChain.Data();
	
	TInt pos = 0;
	TInt end = data.Length();
	iUntrustedRoots.ResetAndDestroy();
	
	// Find all the self-signed certificates in the chain as possible root candidates
	
	while (pos < end)
		{
		CX509Certificate* decoded = CX509Certificate::NewLC(data, pos);
		if (decoded->IsSelfSignedL())
			{
			// If the root isn't in the trusted list, append it to the untrusted roots
			TBool found = EFalse;
			if(NULL == iRootCerts)
				{
				for (TInt i = 0; i < iTrustedRoots.Count(); i++)
					{
					CCTCertInfo* root = iTrustedRoots[i];
					if (root->SubjectKeyId() == decoded->KeyIdentifierL()) 
						{
						found = ETrue;
						break;
						}
					}
				}
			else
				{
				for (TInt i = iRootCerts->Count() - 1; i >= 0; --i)
					{
					CX509Certificate* root = (*iRootCerts)[i];
					if (root->KeyIdentifierL() == decoded->KeyIdentifierL()) 
						{
						found = ETrue;
						break;
						}
					}
				}
			
			if (!found)
				{
				iUntrustedRoots.AppendL(decoded);
				CleanupStack::Pop(decoded);
				}
			else
				{
				CleanupStack::PopAndDestroy(decoded);	
				}
			}
		else
			{
			CleanupStack::PopAndDestroy(decoded);
			}	
		}
	
	DEBUG_PRINTF2(_L8("Security Manager - Found %d non-trusted candidate root certificates"), iUntrustedRoots.Count());
		
	// If we have self signed roots, don't look in the certstore for a further root
	
	TBool hasUntrustedRoot = (iUntrustedRoots.Count() > 0);
	
	if (NULL != iRootCerts)
		{
		DEBUG_PRINTF(_L8("Security Manager - Validating chain using user provided roots"));
		iCurrentPkixChain = CPKIXCertChainBase::NewL(*iCertStore, data, *iRootCerts);
		hasUntrustedRoot = ETrue; // These root certs are considered untrusted.
		}
	else if(hasUntrustedRoot)
		{
		DEBUG_PRINTF(_L8("Security Manager - Validating chain using untrusted roots"));
		iCurrentPkixChain = CPKIXCertChainBase::NewL(*iCertStore, data, iUntrustedRoots);
		}
	else
		{
		DEBUG_PRINTF(_L8("Security Manager - Validating chain using trusted roots from store"));
		iCurrentPkixChain = CPKIXCertChainBase::NewL(*iCertStore, data, KSwiApplicabilityUid);	
		}
	

	DEBUG_PRINTF2(_L8("Security Manager - Certificate chain contains %d certificates."), iCurrentPkixChain->Count());

	const CX509Certificate& clientCert = iCurrentPkixChain->Cert(0); // This is the ee certificate
	CX509Certificate* endCertOut = CX509Certificate::NewLC(clientCert);
	iEndCertificatesOut->AppendL(endCertOut);
	CleanupStack::Pop(endCertOut);
	
	DEBUG_CODE_SECTION(HBufC* issuer = clientCert.IssuerL(); DEBUG_PRINTF2(_L("Security Manager - End Entity Certificate Issuer: '%S'."), issuer); delete issuer;);
	DEBUG_CODE_SECTION(HBufC* subject = clientCert.SubjectL(); DEBUG_PRINTF2(_L("Security Manager - End Entity Certificate Subject: '%S'."), subject); delete subject;);
	
	const CSubjectPublicKeyInfo& publicKey=  clientCert.PublicKey();
	
	DEBUG_PRINTF2(_L8("Security Manager - SIS file signed %d times with this certificate."), signatures.Count());
	
	CSignatureVerifier* verifier = CSignatureVerifier::NewLC();
	TBool result = EFalse;
	for (TInt k = 0; k < signatures.Count(); k++)
		{
		const TDesC& algorithmOID = signatures[k]->Algorithm().AlgorithmIdentifier().Data();
		TRAP_IGNORE(result = verifier->VerifySignatureL(algorithmOID, publicKey, iRawController.Mid((iIsEmbeddedController ? 4 : iController->HeaderSize()), size), signatures[k]->Data()));
		if (result) 
 			{
			// We have a verify any policy on signatures (inside a given SISSignatureCertificateChain)
			// Cf. SGL.GT0188.251 Section 4.3
			break;   
			}
		}

	CleanupStack::PopAndDestroy(verifier);

	if (!result) // Signature not verified: something is WRONG. Abort.
		{
		DEBUG_PRINTF(_L8("Security Manager - No signature validated."));
		
		*iResult = ESignatureCouldNotBeValidated;
		User::RequestComplete(iClientStatus, KErrNone);	
		return;			
		}
			
	// The signature is good, validate the certificate chain
	
	CPKIXValidationResultBase* validationResult = CPKIXValidationResultBase::NewLC();
	iValidationResultsOut->Append(validationResult);
	CleanupStack::Pop(validationResult);
	
	iState = EValidatingChain;
	iCurrentPkixChain->SetValidityPeriodCheckFatal(iCheckDateAndTime);
	iChainValidator->ValidateChainL(*iCurrentPkixChain, *iResult, *validationResult, 
									iCurrentCapabilities, hasUntrustedRoot, iStatus);
	SetActive();	
	}

EXPORT_C HBufC8* CSecurityManager::CalculateHashLC(MSisDataProvider& aDataProvider, CMessageDigest::THashId aAlgorithm)
	{
	CMessageDigest* digest = CMessageDigestFactory::NewDigestLC(aAlgorithm);
		
	HBufC8* aBuffer = HBufC8::NewMaxLC(KFileBufferSize); 
	TPtr8 aBufferPtr(aBuffer->Des());

	User::LeaveIfError(aDataProvider.Read(aBufferPtr));

	while (aBuffer->Length() != 0)
		{
		digest->Update(*aBuffer);
		User::LeaveIfError(aDataProvider.Read(aBufferPtr));		
		}
	TPtrC8 hash = digest->Final();
	HBufC8* hashBuffer = hash.AllocL();
	CleanupStack::PopAndDestroy(2, digest); // aBuffer, digest
	
	CleanupStack::PushL(hashBuffer);
	return hashBuffer;
	}

EXPORT_C TBool CSecurityManager::VerifyFileHashL(MSisDataProvider& aDataProvider, const CHashContainer& aDigest)
	{
	HBufC8* hashBuffer = CalculateHashLC(aDataProvider, aDigest.Algorithm());

	TBool result = EFalse;

	TPtrC8 hash = hashBuffer->Des();
	if (hash.Compare(aDigest.Data()) == 0)
		{
		result = ETrue;
		}

	CleanupStack::PopAndDestroy(hashBuffer);
	return result;
	}
	
void CSecurityManager::UpdateListOfMissingRequiredCertsL(const CCertificate& aCertificate) 
	{
	// Find aCertificate in iTrustedRoots and marked as nonmandatory for temporary!
	TInt count = iTrustedRoots.Count();
	for (TInt k = 0; k < count; k++)
		{
		CCTCertInfo* certInfo = iTrustedRoots[k];
		if (iCertMetaInfo[k].iIsMandatory && !certInfo->SubjectKeyId().Compare(aCertificate.KeyIdentifierL()))
			{
			DEBUG_PRINTF2(_L("Security Manager - Mandatory certificate '%S' satisfied."), &(certInfo->Label()));
			// set the Mandatory is false so that next time this certificate won't be compared
			iCertMetaInfo[k].iIsMandatory = 0;
			iMandatoryCertDNCount--;
			break;
			}
		}
	}

void CSecurityManager::UpdateSystemUpgradeCertStatusL(const CCertificate& aCertificate)
	{	
	// Find aCertificate in iTrustedRoots and update the system upgrade trust status
	TInt count = iTrustedRoots.Count();
	for (TInt k = 0; k < count; k++)
		{
		CCTCertInfo* certInfo = iTrustedRoots[k];
		if (iCertMetaInfo[k].iIsSystemUpgrade && certInfo->SubjectKeyId() == aCertificate.KeyIdentifierL())
			{
			DEBUG_PRINTF2(_L("Security Manager - System upgrade certificate '%S' satisfied."), &(certInfo->Label()));
			// create a modifyable reference to the current controller
			Sis::CController& controller = const_cast <Sis::CController&>(*iController);
			// set the SuCert validation status
			controller.SetSignedBySuCert(ETrue);
			break;
			}
		}
	
	}

EXPORT_C void CSecurityManager::GetCertificatesFromControllerL(
	const Sis::CController& aController,
	RPointerArray<CX509Certificate>& aCerts)
	{
	// Go through all SIS chains and extract end certificates from them.
	const RPointerArray<Sis::CSignatureCertificateChain>& chains=
		aController.SignatureCertificateChains();
	for (TInt i=0; i<chains.Count(); i++)
		{
		Sis::CSignatureCertificateChain& sigCertChain=*chains[i];
		const Sis::CCertificateChain& certChain=
			sigCertChain.CertificateChain();
		// Construct PKIX cert chain from raw data in the controller.
		CPKIXCertChainBase* pkixChain=CPKIXCertChainBase::NewLC(*iCertStore,
			certChain.Data(), KSwiApplicabilityUid);
		
		// Extract end entity certificate and store it in the member array.
		const CX509Certificate& endCert=pkixChain->Cert(0);
		CX509Certificate* endCertCopy=CX509Certificate::NewLC(endCert);
		User::LeaveIfError(aCerts.Append(endCertCopy));
		CleanupStack::Pop(endCertCopy);
		
		// Cleanup.
		CleanupStack::PopAndDestroy(pkixChain);
		}
	}

EXPORT_C void CSecurityManager::FillCertInfoArrayL(
	const RPointerArray<CX509Certificate>& aCertificates,
	RPointerArray<CCertificateInfo>& aCertInfos)
	{
	for (TInt i=0; i<aCertificates.Count(); i++)
		{
		CCertificateInfo* certInfo=CCertificateInfo::NewLC(*aCertificates[i]);
		aCertInfos.AppendL(certInfo);
		CleanupStack::Pop(certInfo);
		}
	}

EXPORT_C const RPointerArray<HBufC>& CSecurityManager::DeviceIDsInfo() const
 	{
 	return iDeviceIDs;
 	}

EXPORT_C void CSecurityManager::ResetValidCertChains() 
	{
	iValidPkixChains.ResetAndDestroy();
	}
	
EXPORT_C void CSecurityManager::SetDevCertWarningState(TInt aDevCertWarningState)
	{
	iDevCertWarningState=static_cast<TDevCertWarningState>(aDevCertWarningState);	
	}

EXPORT_C TInt CSecurityManager::GetDevCertWarningState()
	{
	return iDevCertWarningState;
	}


EXPORT_C void CSecurityManager::SetRootCerts(RPointerArray<CX509Certificate>* aX509CertArray)
	{
	iRootCerts = aX509CertArray;
	}

	
	



