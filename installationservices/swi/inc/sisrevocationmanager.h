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

#ifndef SISREVOCATIONMANAGER_H
#define SISREVOCATIONMANAGER_H

#include "swi/msisuihandlers.h"

namespace Swi 
{
class CSisRegistrySession;	 
class CSecurityManager;

namespace Sis
	{
	class CSignatureCertificateChain;
	class CController;
	}

/** 
 * CSisRevocationManager is responsible for managing the SIS registry's revocation
 * services.
 * 
 * @released
 * @internalTechnology 
 */
class CSisRevocationManager : public CActive
	{

public: // life-cycle methods

	/**
	 * Constructs a new CSisRevocationManager object
	 *
	 * @param aSession The CSisRegistrySession to use
	 * @return A new security manager
	 */
	IMPORT_C static CSisRevocationManager* NewL(CSisRegistrySession& aSession);
	
	/**
	 * Constructs a new CSisRevocationManager object and leaves it on the cleanup stack
	 *
	 * @param aSession The CSisRegistrySession to use
	 * @return A new security manager
	 */
	IMPORT_C static CSisRevocationManager* NewLC(CSisRegistrySession& aSession);
		
	IMPORT_C ~CSisRevocationManager();

protected: 
	// Inherited from CActive 
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
		
public: // business methods
								 
	/**
	 * This method verifies a SISX controller and checks the revocationstatus. 
	 *
	 * @note This method will take ownership of the aController parameter and
	 * will destroy it on completion.
	 *
	 * @param aRawController    (in) Raw controller data to perform revocation on.
     * @param aController       (in) Controller object to perform revocation on.
	 * @param aTrustStatus      (in/out) Intput with list of chain indexes to be checked.
	 *							Output trust status.
	 * @param aCertChainIndices (in) List of indices to chains to be validated.	
	 * @param aOcspUri			(in) The default OCSP URI as specified by the UI or 
	 *							read from the policy.
	 * @param aIap				(in) The IAP to use for the revocation check
	 * @param aMessage          (in/out) The client request status.
	 */
	IMPORT_C void RevocationStatusRequestL(HBufC8* aRawController,
						             	   const Sis::CController* aController,
						                   TSisTrustStatus& aTrustStatus,
	                                       const RArray<TInt>& aCertChainIndices,
									       const TDesC8& aOcspUri,
									       const RMessage2& aMessage);
									 
private: // Helper functions		
	

	/**
	 * Verify the certificate chains which are contained in aChainList
	 *
	 */
	void VerifcationRequestL();
	
	/**
	 * Perform an OCSP check on the certificate chain which have been 
	 * previously validated by a call to VerifcationRequestL(). 
	 */
	void PerformOcspRequestL();
	
	/**
	 * Set trust status according to the results received from checks.
	 *
	 */
	void SetTrustStatusL();	

	/**
	 * Constructor
	 */ 	
	CSisRevocationManager(CSisRegistrySession& aSession);
		
	/**
	 * Delete all resources
	 */
	void Cleanup();
	
private: // fields

	/**
	 * List of chain indexes on which the revocation checks will be made is 
	 * contained in TSisTrustStatus.
	 */
	TSisTrustStatus iTrustStatus;
	
	/**
	 * Container holding the OCSP results.
	 */	
	RPointerArray<TOCSPOutcome> iOcspOutcomeOut;

	/** 
	 * The internal state of the state machine
	 */
    enum TState
	   {
	   EIdle,
  	   EVerifyChains,
   	   ERevocationCheck,
   	   ERevocationComplete
       } iState;	

	/// for returning status to client
	RMessagePtr2 iMessage;
	
	/**
	 * The list of SisSignatureCertificateChain blocks in the actual
	 * controller. Each of these must be validated.
	 *
	 */
	RPointerArray<Sis::CSignatureCertificateChain> iChains; // We do not own this!	
		      
	/// handles to service providers
	CSisRegistrySession* iSession;
	CSecurityManager* iSecurityManager;
	const Sis::CController* iController;
	
	/// output result from verification request
	TSignatureValidationResult iSignatureValidationResult;
	
	/**
	 * The list of the validation results. Each of this corresponds
	 * to the SisSignatureCertificateChain in iChains of corresponding
	 * index. This list is populated by VerifyBlockL(), the PKIX 
	 * validator sets the result during the validation process.
	 */
	RPointerArray<CPKIXValidationResultBase> iValidationResultsOut;
	
	/// End certificates validated by security manager
	RPointerArray<CX509Certificate> iEndCertificates;
	
	/// The set of capabilities the controller has been signed for
	TCapabilitySet iCapabilitySet;
		
	/// The default OCSP URI
	HBufC8* iOcspUri;
	
	/// OCSP result dialog message (not used)
	TRevocationDialogMessage iOcspMsg;

	/// Policy flag that controls installation of unsigned SIS files
	TBool iAllowUnsigned;
	
	/// List of indices to chains to be validated
	RArray<TInt> iCertChainIndices;
	
	/**
	 * A raw version of the SisController which is signed.
	 * This is passed to us by the client. We need it to verify
	 * the signatures.
	 */	
	HBufC8* iRawController;
	
	TUint32 iIap;
	};

} //namespace Swi

#endif // #ifndef SISREVOCATIONMANAGER_H
