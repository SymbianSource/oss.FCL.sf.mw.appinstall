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

#ifndef __CHAINVALIDATOR_H__
#define __CHAINVALIDATOR_H__

#include <e32base.h>
#include <swi/msisuihandlers.h>
#include <ct/rmpointerarray.h>

class CSWICertStore;
class CPKIXCertChainBase;
class CCertAttributeFilter;

namespace Swi
{
  class CSecurityPolicy;
  
  /**
   * This Security Manager helper class takes care of validating certificate chains.
   * It also maps a chain which resolved to a root certificate to its capability set.
   *
   */
  class CChainValidator : public CActive 
  {
  public:

  	/**
  	 * Creates a new Chain Validator object with the given backend certstore and security
  	 * policy object.
  	 *
  	 * @param aCerstore 	  The backend certstore to be used
  	 * @param aSecurityPolicy The backend policy accessor to be used
  	 *
  	 * @return A new Chain Validator
  	 */
    static CChainValidator* NewL(CSWICertStore& aCerstore, CSecurityPolicy& aSecurityPolicy);

  	/**
  	 * Creates a new Chain Validator object with the given backend certstore and security
  	 * policy object and leaves it on the cleanup stack.
	 *
  	 * @param aCerstore       The backend certstore to be used
  	 * @param aSecurityPolicy The backend policy accessor to be used
  	 *
  	 * @return A new Chain Validator
  	 */
    static CChainValidator* NewLC(CSWICertStore& aCerstore, CSecurityPolicy& aSecurityPolicy);
    
    ~CChainValidator();

  public: // from CActive 
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);	

  public: // Business methods 
  
  	/**
  	 * The main service provided by this class. Given a PKIX certficate chain it
  	 * attempts to validate it using the backend certstore and security policy.
  	 *
  	 * @param aPkixChain The certificate chain to validate. The chain is built upon
  	 *                   by adding any intermediate and root certificates required.
  	 *                   It might therefore change upon successfull validation.
  	 *
  	 * @param aResultOut An output parameter stating the result of the validation.
  	 *
  	 * @param aValidationResultOut An output parameter stating the result of the 
  	 *                             validation.
  	 *
  	 * @param aCapablitySetOut The set of capabilities assigned to the root certificate 
  	 *                         (if any) to which the chain resolved to.
  	 *
  	 * @param aClientStatus The client request status.
  	 */
    void ValidateChainL(CPKIXCertChainBase& aPkixChain, 
			TSignatureValidationResult& aResultOut,    
			CPKIXValidationResultBase& aValidationResultOut,
			TCapabilitySet& aCapablitySetOut,
 			TBool aIsUntrustedRoot,  
			TRequestStatus& aClientStatus);

  protected:
  
  	/**
  	 * This method checks whether the end entity certificate of the current chain
  	 * has a certain extension OID, e.g. code signing.
  	 *
  	 * It is meant to be used internally.
  	 * 
  	 * @param aExtensionOid The OID we are cheking for
  	 *
  	 * @return ETrue if the OID is present, EFalse otherwise
  	 */
	TBool CheckExtensionL(const TDesC& aExtensionOid);
	
	/**
  	 * This method checks whether the end entity certificate of the current chain
  	 * has one of the code signing extension OIDs
  	 *
  	 * It is meant to be used internally.
  	 * 
  	 * @param aAlternativeCodeSigningOids The list of OIDs we are cheking for
  	 *
  	 * @return ETrue if at least one of the OIDs is present, EFalse otherwise
  	 */
	TBool CheckExtensionL(const RPointerArray<HBufC> aAlternativeCodeSigningOids);
	
 	/**
   	This method checks whether the end entity certificate of the current chain
   	has a certain extension, e.g. Extended Key Usage. It is meant to be used internally.
   	@param aExtensionOid The OID of the standard extension
   	@return ETrue if the OID is present, EFalse otherwise
   	*/
 	TBool IsExtensionPresent(const TDesC& aExtensionOid);
	
    
  private:
    CChainValidator(CSWICertStore/*MCTCertStore*/& aCerstore, CSecurityPolicy& aSecurityPolicy);
    void ConstructL();  

  private:
  	
  	/**
  	 * The client request status.
  	 */
    TRequestStatus* iClientStatus;
    
    /**
     * The underlying backend certstore.
     */
    CSWICertStore& iCertStore;
    
    /**
     * The backend security policy accessor. 
     */
	CSecurityPolicy& iSecurityPolicy;
	
	/**
	 * The overall result of the validation.
	 */
	TSignatureValidationResult* iResultOut;	
	
	/**
	 * The certificate chain being validated.
	 */
	CPKIXCertChainBase* iPkixChain;
	
	/**
	 * The plkix validation result, this is an output parameter.
	 */
	CPKIXValidationResultBase* iValidationResultOut;

	/**
	 * The set of capabilities assigned to the root certificate (if it exist)
	 * to which the certificate chain has resolved.
	 */
	TCapabilitySet*	iCapabilitySetOut;

	/**
	 * An attribute filter to select only CA certificates. This is used as we
	 * need to list the certificates in the backend certstore to retrieve their
	 * capabilities. In our case we are interested only with the eventual root of
	 * the chain to be validated.
	 */
	CCertAttributeFilter* iFilter; 
	
	/**
	 * The list of certificates returned by the certstore when we look for
	 * the root of the chain. It will <b>always</b> contain at most 1 certificate.
	 */
	RMPointerArray<CCTCertInfo> iMatchingCerts; 
	
	//Indicates whether or not the chain is rooted with an untrusted certificate
	 
	TBool iIsUntrustedRoot;

    enum TChainValidatorState
	   {
	   EValidatingChain,
	   ERetrieveCapabilities,
	   EDone,
       } iState;
      
	/**
	 * counter used to go through the key-matched certs
	 */   
    TInt iMatchCertsCount;
    
    /**
	 * Buffer used to hold the encoded certificate
	 */    
    HBufC8* iEncodedCertBuf;
  };
  
} // namespace Swi


#endif // __CHAINVALIDATOR_H__
