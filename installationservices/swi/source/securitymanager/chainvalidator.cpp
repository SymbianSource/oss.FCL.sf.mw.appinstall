/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "chainvalidator.h"

// PKIX dependencies
#include <pkixcertchain.h>
#include <x509certext.h>
#include <swicertstore.h>
#include <ccertattributefilter.h>
#include <securitydefsconst.h>

// SecMan includes
#include "securitypolicy.h"

#include "log.h"

using namespace Swi;

_LIT(KChainValidatorName, "_Chain_Validator_");

//
// Life cycle methods
//

/*static*/ CChainValidator* CChainValidator::NewL(CSWICertStore& aCerstore, CSecurityPolicy& aSecurityPolicy)
	{
	CChainValidator* self = CChainValidator::NewLC(aCerstore, aSecurityPolicy);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CChainValidator* CChainValidator::NewLC(CSWICertStore& aCerstore, CSecurityPolicy& aSecurityPolicy)
	{
	CChainValidator* self = new(ELeave) CChainValidator(aCerstore, aSecurityPolicy);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
    
CChainValidator::~CChainValidator()
	{
	Deque();
	
	iMatchingCerts.Close();
	
	if (iFilter)
		{
		delete iFilter;
		iFilter = NULL;
		}
	delete iEncodedCertBuf;
	}

CChainValidator::CChainValidator(CSWICertStore& aCerstore, 
								 CSecurityPolicy& aSecurityPolicy) : CActive(EPriorityNormal), iCertStore(aCerstore), iSecurityPolicy(aSecurityPolicy)
	{
	CActiveScheduler::Add(this);	
	}

void CChainValidator::ConstructL()
	{
	}

//
// from CActive 
//
    
void CChainValidator::RunL()
	{
	DEBUG_PRINTF3(_L8("Security Manager - CChainValidator::RunL(). State: %d, Status: %d."),
		iState, iStatus.Int());
	
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());     // Hop into RunError()
		}
	
	switch (iState)
		{
		case EValidatingChain:
			{
			TValidationStatus resultStatus = iValidationResultOut->Error(); 	
			TInt certCount = iPkixChain->Count();
						
			if (resultStatus.iReason != EValidatedOK)
				{
				DEBUG_PRINTF(_L8("Security Manager - Chain failed to validate."));
				
				*iResultOut = ECertificateValidationError; 
				User::RequestComplete(iClientStatus, KErrNone);
				}
			else
				{
				
				DEBUG_CODE_SECTION(
					const CX509Certificate& root = iPkixChain->Cert(certCount-1);
				
					HBufC* subject = root.SubjectL();
					DEBUG_PRINTF2(_L("Security Manager - Root Certificate subject: %S"), subject);
					delete subject;
				);
				
				// If the root is untrusted, we don't need to fetch the capabilities for it
				
 				if (iIsUntrustedRoot)
 					{
 					DEBUG_PRINTF(_L8("Security Manager - Root certificate untrusted. Not fetching capabilities."));
 					
 					iState = EDone;
 					iCapabilitySetOut->SetEmpty();
 					// Reflect the fact that at least one "trusted" signature makes the package trusted
 					*iResultOut = (*iResultOut == EValidationSucceeded) ? EValidationSucceeded : ESignatureSelfSigned;
 					User::RequestComplete(iClientStatus, KErrNone);					
 					}
 				else
 					{
 					DEBUG_PRINTF(_L8("Security Manager - Root certificate trusted. Fetching associated capabilities."));
 					
 					iState = ERetrieveCapabilities;
 					delete iFilter;
 					iFilter = NULL;
 					iFilter = CCertAttributeFilter::NewL();
 					iFilter->SetOwnerType(ECACertificate); // We only want root certs!	
 					iFilter->SetSubjectKeyId(iPkixChain->Cert(certCount-1).KeyIdentifierL()); // (certCount - 1) = Root 
					// Reset the array here to avoid compounding the number of certs we retrieve
					iMatchingCerts.Close();
					iCertStore.List(iMatchingCerts, *iFilter, iStatus);
 					SetActive();
 					}
				}
			}
			break;

		case ERetrieveCapabilities:
			{
			if (iMatchingCerts.Count() != 1) 
				{
				// Not for the first one in iMatchingCerts array
				if (iMatchCertsCount>=1) 
					{
					CX509Certificate* cert=CX509Certificate::NewLC(*iEncodedCertBuf);						
					//if the one matches serial number and issuer name as well
					if (cert->IsEqualL(iPkixChain->Cert(iPkixChain->Count()-1)))
						{
						DEBUG_PRINTF(_L8("Security Manager - Found matching root certificate for capabilities retrieval."));
						
						CleanupStack::PopAndDestroy(cert);
						delete iEncodedCertBuf;
						iEncodedCertBuf=NULL;
						iState = EDone;
						TCertMetaInfo metaInfo = iCertStore.CertMetaInfoL(*iMatchingCerts[iMatchCertsCount-1]);
						*iCapabilitySetOut = metaInfo.iCapabilities;
						iMatchCertsCount=0;	
						*iResultOut = EValidationSucceeded;
						User::RequestComplete(iClientStatus, KErrNone);	
						return;
						}
					CleanupStack::PopAndDestroy(cert);	
					delete iEncodedCertBuf;
					iEncodedCertBuf=NULL;							
					}
				// when iMatchCertsCount is from 0 to iMatchingCerts.Count()-1, retrieve the certificate
				if (iMatchCertsCount<iMatchingCerts.Count())
					{
					DEBUG_PRINTF3(_L8("Security Manager - Searching for correct certificate. Processing match %d of %d."),
						iMatchCertsCount+1, iMatchingCerts.Count()); 
					
					TInt certSize=iMatchingCerts[iMatchCertsCount]->Size();
					iEncodedCertBuf=HBufC8::NewL(certSize); 
					TPtr8 encodeCertBuf=iEncodedCertBuf->Des();								
					iCertStore.Retrieve(*iMatchingCerts[iMatchCertsCount], encodeCertBuf, iStatus);
					iState = ERetrieveCapabilities;
					iMatchCertsCount++;
					SetActive();														
					}
				else
					{
					DEBUG_PRINTF(_L8("Security Manager - Error. Failed to find a certificate in store to retrieve capabilities from."));
					
					// something went horribly wrong, when iMatchCertsCount==iMatchingCerts.Count()
					delete iEncodedCertBuf;
					iEncodedCertBuf=NULL;
					iMatchCertsCount=0;												
					*iResultOut = ECertificateValidationError; 
					User::RequestComplete(iClientStatus, KErrGeneral);	
					return;	
					}																						
				}
			else
				{
				iState = EDone;
				TCertMetaInfo metaInfo = iCertStore.CertMetaInfoL(*iMatchingCerts[0]);
				*iCapabilitySetOut = metaInfo.iCapabilities;
				*iResultOut = EValidationSucceeded;
				User::RequestComplete(iClientStatus, KErrNone);
				
				DEBUG_PRINTF(_L8("Security Manager - Sucessfully retrieved capabilities from store."));
				}			
			}
			break;
			
	    default:
			{
			User::Panic(KChainValidatorName, 1);					
			}
		}
	}


void CChainValidator::DoCancel()
	{
	DEBUG_PRINTF2(_L8("Security Manager - Chain Validator, cancelling in state %d."), iState);
	
	switch (iState)
		{
		case EValidatingChain:
			{
			iPkixChain->CancelValidate();
			break;
			}
		case ERetrieveCapabilities:
			{
			//iCertStore.Cancel();
			break;
			}
		case EDone:
			{
			// Nothing
			break;
			}
		default:		
			{
			// Nothing
			}
		}
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

TInt CChainValidator::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Security Manager - ChainValidator::RunError(), Error Code: %d."), aError);
	
	User::RequestComplete(iClientStatus, aError);
	return KErrNone;
	}

//
// Business methods
//
void CChainValidator::ValidateChainL(CPKIXCertChainBase& aPkixChain, 
									 TSignatureValidationResult& aResultOut,
									 CPKIXValidationResultBase& aValidationResultOut,
									 TCapabilitySet& aCapablitySetOut,
									 TBool aIsUntrustedRoot,  
									 TRequestStatus& aClientStatus)
	{
	iResultOut = &aResultOut;
	iPkixChain = &aPkixChain;
	iValidationResultOut = &aValidationResultOut;
	iCapabilitySetOut = &aCapablitySetOut;
	iIsUntrustedRoot = aIsUntrustedRoot;
	iClientStatus = &aClientStatus;
	*iClientStatus = KRequestPending;

	// Certificate validation will fail in the following 2 scenarios:
		// if EKU is present, neither standard codesigning OID nor alternative codesigning OID is not present.
		// if EKU is absent and codesigning is mandated in security policy.
	if	((	IsExtensionPresent(KExtendedKeyUsage) && !CheckExtensionL(KCodeSigningOID) && !CheckExtensionL(iSecurityPolicy.AlternativeCodeSigningOid()) ) || 
		 ( 	!IsExtensionPresent(KExtendedKeyUsage) && iSecurityPolicy.MandateCodesigningExtension() ) 
		)
		{
		DEBUG_PRINTF(_L8("Security Manager - Error. End entity key usage does not included code signing (madated by security policy)."));
			
		*iResultOut = ENoCodeSigningExtension;
		User::RequestComplete(iClientStatus, KErrNone);
		return;
		}

		
	if (iSecurityPolicy.MandateCertificatePolicies()) // Custom X509 extensions required!
		{
		RPointerArray<HBufC> oids = iSecurityPolicy.Oids(); // Get custom extension oids		
		for (TInt k=0; k < oids.Count(); k++)
			{
			if (!CheckExtensionL(*oids[k]))
				{
				DEBUG_PRINTF2(_L("Security Manager - Security Policy mandates custom extension '%S' not present in end entity."),
					oids[k]);
				
				*iResultOut = ENoSupportedPolicyExtension;
				User::RequestComplete(iClientStatus, KErrNone);
				return;		
				}
			}
		}

    // In order to comply with RFC 3280, the date used for validating 
    // certificates is now: the 'current' date!
    TTime gmt;
 	if (gmt.UniversalTimeSecure() == KErrNoSecureTime)
		{
		// Fall back to nonsecure time. 
		DEBUG_PRINTF(_L("Security Manager - Secure clock is not available, so using nonsecure clock instead."));
		gmt.UniversalTime();
		}    
	
	iState = EValidatingChain;
	iPkixChain->ValidateL(*iValidationResultOut, gmt, iStatus);	
	SetActive();
	}

TBool CChainValidator::IsExtensionPresent(const TDesC& aExtensionOid)
	{
    TInt numberOfCertInChain = iPkixChain->Count();
    TBool result = EFalse;
    
    if (numberOfCertInChain > 0)
	    {
        const CX509Certificate& eeCert = iPkixChain->Cert(0);  // This is the end entity certificate of the chain!
        const CX509CertExtension* extension = eeCert.Extension(aExtensionOid);
        if (extension)
        	{
            result = ETrue;
            }
	    }
	return result;
	}
    
TBool CChainValidator::CheckExtensionL(const TDesC& aExtensionOid)
	{
    TInt numberOfCertInChain = iPkixChain->Count();
    TBool result = EFalse;
    
    if (aExtensionOid!=KNullDesC && numberOfCertInChain > 0)
	    {
        const CX509Certificate& eeCert = iPkixChain->Cert(0);  // This is the end entity certificate of the chain!
        const CX509CertExtension* extension = eeCert.Extension(KExtendedKeyUsage);
        if (extension)
            {
            TInt dummy = 0;
            CX509ExtendedKeyUsageExt* extendedKeyUsage = 
                CX509ExtendedKeyUsageExt::NewL(extension->Data(), dummy);
            const CArrayPtrFlat<HBufC>& usages = extendedKeyUsage->KeyUsages();

			for (TInt k = 0; k < usages.Count(); k++)
                {
                if (usages[k]->Compare(aExtensionOid) == 0)
                    {
                    result = ETrue;
                    break;
                    }
                }
            delete extendedKeyUsage; // Cannot leave before we get here
            }
	    }
	return result;
	}

TBool CChainValidator::CheckExtensionL(const RPointerArray<HBufC> aAlternativeCodeSigningOids)
	{	
	TBool retVal = EFalse;
	TInt count = aAlternativeCodeSigningOids.Count();
	TInt numberOfCertInChain = iPkixChain->Count();	
	
	if((numberOfCertInChain > 0) && (count > 0))
		{
		const CX509Certificate& eeCert = iPkixChain->Cert(0);  // This is the end entity certificate of the chain!
        const CX509CertExtension* extension = eeCert.Extension(KExtendedKeyUsage);
        if (extension)
            {
            TInt dummy = 0;
            CX509ExtendedKeyUsageExt* extendedKeyUsage = 
                CX509ExtendedKeyUsageExt::NewL(extension->Data(), dummy);
            const CArrayPtrFlat<HBufC>& usages = extendedKeyUsage->KeyUsages();

			for (TInt k = 0; k < usages.Count(); k++)
                {
                for(TInt j = 0; j < count; j++)
                	{
                	if (usages[k]->Compare(*aAlternativeCodeSigningOids[j]) == 0)
                    	{
                    	retVal = ETrue;
                    	break;
                    	}
                	}
                }
            delete extendedKeyUsage; // Cannot leave before we get here
            }
		}
	
	return retVal;
	}
