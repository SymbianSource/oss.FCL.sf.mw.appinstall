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


#ifndef __SECURITYMANAGER_H__
#define __SECURITYMANAGER_H__

#include <e32base.h>
#include <securitydefs.h>
#include <ct/rmpointerarray.h>
#include <swi/msisuihandlers.h>
#include "swi/siscontroller.h"
#include "securitypolicy.h"
#include <pkixcertchain.h>

class CCTCertInfo;
class CSWICertStore;
class TCertMetaInfo;

namespace Swi 
{

class CChainValidator;
class CCertificateRetriever;
class CRevocationHandler;
class CHashContainer;

namespace Sis
	{
	class CSignatureCertificateChain;
	}

class MSisDataProvider;

enum TDevCertWarningState
	{
	ENoDevCerts	=0,
	EFoundDevCerts	=1,
	EDevCertsWarned =2	
	};	
	
/** 
 * The SecurityManager is the SWI component which takes care of the cryptographic
 * protections in the installation process. Specifically it verifies the signatures
 * in signed SISX files, validates their corresponding certificate chains,
 * builds the list of granted capabilities and enforces mandatory certificates
 * constraints. Moreover, driven from a compile time modifiable security policy,
 * enforces additional constraints on the valid certificates, e.g. Code Signing 
 * and other X509 extensions. It performs OCSP checks for the relevant certificate
 * chains as well. 
 * <P>
 * To accomplish its task the Security Manager relies on a number of helper 
 * classes:
 * <ul>
 *   <li> CCertificateRetriever to retrieve mandatory certificates
 *   <li> CSignatureVerifier to verify signatures
 *   <li> CChainValidator to validate PKIX certificate chains
 *   <li> CRevocationHandler to perform OCSP checks
 *   <li> CSecurityPolicy to access the compile time configurable security policy
 * </ul>
 * 
 * @released
 * @internalTechnology 
 */
class CSecurityManager : public CActive
	{

public: // life-cycle methods

	/**
	 * Constructs a new CSecurityManager object
	 *
	 * @return A new security manager
	 */
	IMPORT_C static CSecurityManager* NewL();
	
	/**
	 * Constructs a new CSecurityManager object and leaves it on the cleanup stack
	 *
	 * @return A new security manager
	 */
	IMPORT_C static CSecurityManager* NewLC();
	
	virtual ~CSecurityManager();

public: // from CActive 

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
		

public: // business methods

	/**
	 * This method verifies a SISX controller. It checks:
	 * <ul>
	 *   <li> Whether the controller is signed
	 *   <li> Whether at least a signature per certificate chain can be 
	 *        verified
	 *   <li> Whether at least a certificate chain can be validated 
	 *        successfully
	 *   <li> Whether we can meet all the mandatory certificate requirements
	 *   <li> Hands over a collection of capabilities the controller has been 
	 *        signed for
	 * </ul>
	 *
	 * @param aControllerBinary The raw controller data.
	 * @param aController       An instance of Sis::CController representing 
	 *                          the raw data
	 * @param aResultOut        A global result of the operation        
	 * @param aPkixResultsOut   A list of PKIX validation results. To each of 
	 *                          this corresponds an end entity certificate in 
	 *                          aCertsOut
	 * @param aCertsOut         A list of end entity certificates 
	 * @param aCapabilitySetOut The set of capabilities the controller has been
	 *                          signed for
	 * @param aAllowUnsigned    Receives policy flag that controls installation
	 *                          of unsigned SIS files
	 * @param aIsEmbeddedController Controller is embedded.
	 * @param aStatus           The client request status
	 * @param aCheckDateAndTime Indicates whether the validity periods of the certificates 
	 							in the chain are checked and verified.  A value of ETrue 
	 							indicates they are.
	 */
	IMPORT_C void VerifyControllerL(
					TDesC8& aControllerBinary, 
					const Sis::CController& aController, 
					TSignatureValidationResult* aResultOut,
					RPointerArray<CPKIXValidationResultBase>& aPkixResultsOut,
					RPointerArray<CX509Certificate>& aCertsOut,
					TCapabilitySet* aCapabilitySetOut,
					TBool& aAllowUnsigned,
					TBool& aIsEmbeddedController,
					TRequestStatus& aStatus,
					TBool aCheckDateAndTime = ETrue);

    /**
	 * This method verifies a SISX controller given a list of chain indices. 
	 * It checks:
	 * <ul>
	 *   <li> Whether the controller is signed
	 *   <li> Whether at least a signature per certificate chain can be 
	 *        verified
	 *   <li> Whether at least a certificate chain can be validated 
	 *        successfully
	 *   <li> Whether we can meet all the mandatory certificate requirements
	 *   <li> Hands over a collection of capabilities the controller has been 
	 *        signed for
	 * </ul>
	 *
	 * @param aControllerBinary The raw controller data.
	 * @param aController       An instance of Sis::CController representing 
	 *                          the raw data.
	 * @param aChainListIndices List of indices to chains to be validated.
	 * @param aResultOut        A global result of the operation.
	 * @param aPkixResultsOut   A list of PKIX validation results. To each of 
	 *                          this corresponds an end entity certificate in 
	 *                          aCertsOut.
	 * @param aCertsOut         A list of end entity certificates 
	 * @param aCapabilitySetOut The set of capabilities the controller has been
	 *                          signed for.
	 * @param aAllowUnsigned    Receives policy flag that controls installation
	 *                          of unsigned SIS files.
	 * @param aStatus           The client request status.
	 */		
	IMPORT_C void ReverifyControllerL(
					TDesC8& aControllerBinary, 
					const Sis::CController& aController, 
					const RArray<TInt>& aChainListIndices,
					TSignatureValidationResult* aResultOut,
					RPointerArray<CPKIXValidationResultBase>& aPkixResultsOut,
					RPointerArray<CX509Certificate>& aCertsOut,
					TCapabilitySet* aCapabilitySetOut,
					TBool& aAllowUnsigned,
					TRequestStatus& aStatus);

	/**
	 * Verifies the hash of a the data pointed to by a given data provider.
	 *
	 * @param aDataProvider A data provider which gives access to the file we want to check
	 * @param aDigest The digest of the data pointed to by the given data provider
	 *
	 * @return ETrue if the has could be verified, EFalse otherwise.
	 */								
	IMPORT_C TBool VerifyFileHashL(MSisDataProvider& aDataProvider,
								   const CHashContainer& aDigest);

	/**
	 * Perform an OCSP check on the certificate chain which have been 
	 * previously validated by a call to VerifyControllerL(). This method can 
	 * be called \b only after VerifyControllerL(). This makes the Security 
	 * Manager a stateful object. But this is better than requiring the caller
	 * to store all the intermediate data.
	 *
	 * @param aOcspUri              [in]  The default OCSP URI as specified by 
	 *                                    the UI or read from the policy.
	 * @param aIap					[in, out] The IAP to use for this connection 
	 *									  (zero means show dialog)
	 * @param aRevocationMessageOut [out] An output paramter containing the 
	 *                                    result of the check
	 * @param aOcspOutcomeOut       [out] A list of OCSP outcomes. To each of 
	 *                                    this corresponds a certificate in the 
	 *                                    \e aCertOut list.
	 * @param aCertOut              [out] A list of end entity certificates.
	 * @param aStatus               [in,out] The client status which will be 
	 *                                    completed upon termination.
	 */								
	IMPORT_C void PerformOcspL(const TDesC8&  aOcspUri, 
							   TUint32& aIap,
							   TRevocationDialogMessage* aRevocationMessageOut, 
							   RPointerArray<TOCSPOutcome>& aOcspOutcomeOut, 
							   RPointerArray<CX509Certificate>& aCertOut,
							   TRequestStatus& aStatus);
							   
	/**
	 * Fills certificate info array from the X509 certificate array which was
	 * filled by the security manager.
	 *
	 * @param aCertificates [in]  Array of X509 certificates constructed from 
	 *                            controller.
	 * @param aCertInfos    [out] Array of certificate info objects.
	 */
	IMPORT_C static void FillCertInfoArrayL(
		const RPointerArray<CX509Certificate>& aCertificates,
		RPointerArray<CCertificateInfo>& aCertInfos);
	
	/**
	 * Retrieves end certificates from iController and stores them in 
	 * iCertificates.
	 *
	 * @param aController [in]  SIS controller object.
	 * @param aCerts      [out] Receives EE certificates from the controller.
	 */
	IMPORT_C void GetCertificatesFromControllerL(
		const Sis::CController& aController,
		RPointerArray<CX509Certificate>& aCerts);

	/**
	 * Returns the security policy associated with the security manager.
	 *
	 * @return The security policy object associated with the security manager.
	 */
	IMPORT_C CSecurityPolicy& SecurityPolicy() const;

	/**
	 * Returns the DeviceIDs retrieved from the Mobile.
	 *
	 * @return The array of the device IDs.
	 */
	IMPORT_C const RPointerArray<HBufC>& DeviceIDsInfo() const;
	
	/**
	 * Resets Security Manager's list of valid certificate chains.
	 * This is required to ensure that embedded SIS files do not include the certificate
	 * chains of their parent SIS file.
	 */
	IMPORT_C void ResetValidCertChains();	

	/** Calculates the hash of a file
	@param aDataProvider Provides access to the data from the file
	@param aAlgorithm The algorithm used to generate the hash
	@return A buffer containing the hash result
	*/
	IMPORT_C HBufC8* CalculateHashLC(MSisDataProvider& aDataProvider, CMessageDigest::THashId aAlgorithm);
	
	/**
	Set the devcert warning state.
	@param aDevCertWarningState developer mode certificate warning state
	@return none
	*/
	IMPORT_C void SetDevCertWarningState(TInt aDevCertWarningState);

	/**
	Get the devcert warning state.
	@param aDevCertWarningState developer mode certificate warning state
	@return none
	*/
	IMPORT_C TInt GetDevCertWarningState();
	
	/*
	 * This function sets the user provided list of root certs. These certs will be used
	 * for validation instead of fetching root certs from the certstore for validation.
	 *
	 * @param aX509CertArray caller provided list of root certs.
	 * @return none
	 */
	IMPORT_C void SetRootCerts(RPointerArray<CX509Certificate>* aX509CertArray);
	
	
protected: // Helper functions

	/**
	 * This helper function verifies a given certificate/signature block.
	 * The signature block structure is defined in 
	 * SGL.GT0188.251  Paragraph 4.3
	 * Each block may contain multiple signatures using different
	 * signature schemes.
	 *
	 * @param aChainIndex The index of the chain in iChains which is to be verified.
	 */
	void VerifyBlockL(TInt aChainIndex);

	/**
	 * This helper function calculates the size of the raw
	 * SISX controller data.
	 *
	 * @param iChainsToAdd How many certificate/signature chains to add to the size
	 *                     starting from the most internal one.
	 *
	 * @return The controller size (including iChainsToAdd certificate chains)
	 */
	TInt SignedSize(TInt iChainsToAdd) const;
		
	/**
	 * It reduces the count based on the given certificate from the iMandatoryCertDNCount (count of mandatory 
	 * certificates). If the certificate is not found nothing happens. Note 
	 * that for a controller to be successfully validated the iMandatoryCertDNCount must 
	 * be zero.
	 *
	 * @param aCertificate The certificate to be compared
	 */
	void UpdateListOfMissingRequiredCertsL(const CCertificate& aCertificate); 
	
	/**
	 * It update the given certificate to be validated by System upgrade if the certificate is found in the  
	 * trust anchors list. If the certificate is not found nothing happens. 
	 *
	 * @param aCertificate The certificate to be compared
	 */
	void UpdateSystemUpgradeCertStatusL(const CCertificate& aCertificate);
	
	/**
	 * Translates the set of TOCSPOutcome values into a single 
	 * TRevocationDialogMessage value and sets iRevocationMessage to this value.
	 */
	void ProcessOcspOutcomesL();
	
private:

	CSecurityManager();

	/**
	 * Second phase constructor for CSecurityManager. It attempts to construct
	 * a CSecurityPolicy object, consequently it accesses the security policy
	 * file. If for any reason no such file is present this method will leave.
	 * This renders impossible to create a security manager and, hence, to 
	 * perform an installation.
	 */
	void ConstructL();
	

private: // fields

	/**
	 * The list of SisSignatureCertificateChain blocks in the actual
	 * controller. Each of these must be validated.
	 *
	 */
	RPointerArray<Sis::CSignatureCertificateChain> iChains; // We do not own this!
	
	/**
	 * The list of the validation results. Each of this corresponds
	 * to the SisSignatureCertificateChain in iChains of corresponding
	 * index. This list is populated by VerifyBlockL(), the PKIX 
	 * validator sets the result during the validation process.
	 */
	RPointerArray<CPKIXValidationResultBase>* iValidationResultsOut;

	/**
	 * List of the successfully constructed PKIX chains.
	 * This list contains only the chains which have been
	 * successfully validated.
	 */
	RPointerArray<CX509Certificate>* iEndCertificatesOut;

	/**
	 * The total capabilities so far. This is the union of the
	 * capabilities of every root of every pkix chain which 
	 * has validated successfully.
	 */
	TCapabilitySet* 				 iTotalCapabilitiesOut;

	RPointerArray<TOCSPOutcome>*	 iOcspOutcomeOut;

	TRevocationDialogMessage*		 iRevocationMessage;
	
	TSignatureValidationResult* 	 iResult;

	/**
	 * Count of the mandatory certificates in the certstore.
	 * SISX files must be signed by chains which resolve
	 * to <b>all</b> of them. See SGL.GT0209.201
	 */
	TInt iMandatoryCertDNCount;
	
	/**
	 * A parsed version of the SisController which is signed.
	 * This is passed to us by the client. We need it to calculate
	 * the amount of data signed by each signature.
	 */
	const Sis::CController* iController;  // We do not own this!

	/**
	 * A raw version of the SisController which is signed.
	 * This is passed to us by the client. We need it to verify
	 * the signatures.
	 */
	TPtrC8 iRawController;
		
	/**
	 * A book keeping index pointing to the current certificate chain
	 * being examined. It starts from 0 (the innermost certificate chain
	 * in the controller)
	 */		
	TInt iCurrentChain;

	/** 
	 * A book keeping flag indicating whether any certificate chain has
	 * been validated successfully.
	 */
	TBool iHasValidated;
	
	/**
	 * The capability set associated with the root (if any) of the
	 * current certificate. This value is set by the chain validator.
	 */
	TCapabilitySet iCurrentCapabilities;

	/**
	 * The current PKIX chain which is being constructed.
	 */
	CPKIXCertChainBase* iCurrentPkixChain;

	/**
	 * A list of all the PKIX chains which have been successfully
	 * validated. If an OCSP check is requested, these chains will
	 * undergo the check.	
	 */
	RPointerArray<CPKIXCertChainBase> iValidPkixChains;

	/** 
	 * Access the SWIS security policy which drives some of our
	 * decision making. E.g. whether end certificated must contain
	 * a CodeSigning extension, etc.
	 */
	CSecurityPolicy* iSecPolicy;

	/**
	 * The underlying backend certificate store
	 */
	CSWICertStore* iCertStore;

	/**
	 * Helper class which retrieves all the <i>mandatory</i>
	 * certificates in the certstore.
	 */
	CCertificateRetriever* iCertificateRetriever;

	/**
	 * Helper class to validate a certificate chain. 
	 * It also collects the capabilities associated with
	 * its root (if any) and whether the root (if any) is
	 * marked as mandatory.
	 */
	CChainValidator* iChainValidator;

	/**
	 * Helper class which takes care of OCSP checks
	 */
	CRevocationHandler* iRevocationHandler;
	
 	/**
 	 * 
	 * A list of self-signed, untrusted root certificates
 	 * included in the chain currently being validated.
	 *
	 */ 
 	 RPointerArray<CX509Certificate> iUntrustedRoots;
 	 
     // A book keeping flag indicating at least one 
     // chain has validated that isn't self signed
     
     TBool iHasValidatedTrusted;
     
     // A list of all the roots in the SWI certstore
     
     RMPointerArray<CCTCertInfo> iTrustedRoots;
     
     TBool iIsEmbeddedController;
     
    /**
 	 * 
	 * A list chain indices to be validated by ReverifyControllerL.
	 *
	 */
     const RArray<TInt>* iChainListIndices;
   
	/** 
	 * The internal state of the security manager.
	 */
    enum TSecManState
	   {
	   // Verification and validation
	   EBuildCertificateList,
  	   EValidatingChain,
  	   
  	   // Reverification and validation of chain set for revocation check
  	   ERevalidatingChain,
  	   
  	   // OCSP Check
   	   EOCSPCheck,
   	   EChecksDone,
   	   ERetrievedTrustedRoots
       } iState;	
	
	/**
	 * A pointer to the client request status.
	 */
	TRequestStatus* iClientStatus;
		
	/**
	 * Handle to File server session
	 */
	 RFs iFs;
 	 
	/**
	 * Device ID retrieved from the device using RDeviceInfo API
	 */
	RPointerArray<HBufC> iDeviceIDs;

	/**
	Represent if the deveploper mode certificate is in the cert chains of an application,
	or it is found but not warned to the user, or the user has been warned.  
	*/	
	TDevCertWarningState iDevCertWarningState;	
	
	/*
	 * This member pointer is used to hold user provided list of root cert for 
	 * certificate validation. The ownership of this variable remains with the
	 * caller of SetRootCerts function.
	 */

	RPointerArray<CX509Certificate>* iRootCerts;

	/**
	 * Additional Information about the Certificate in the Certstore.
	 */
	RArray<TCertMetaInfo> iCertMetaInfo;
	
	/**
	 * Indicates when verifying the certificate if we check the validity period.
	 */
	TBool iCheckDateAndTime;
	};

} //namespace Swi

#endif // #ifndef __SECURITYMANAGER_H__
