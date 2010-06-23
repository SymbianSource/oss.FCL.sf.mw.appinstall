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
* signatureverfier.h
*
*/


/**
 @file
 @released
 @internalTechnology
*/

#ifndef __SIGNATUREVERFIER_H__
#define __SIGNATUREVERFIER_H__

#include <e32base.h>
#include <signed.h>

class CMessageDigest;
class CASN1EncSequence;

namespace Swi
{

  /**
   * This Security Manager helper class takes care of verifying signatures.
   */
  class CSignatureVerifier : public CBase
  {
  public : 

    /**
     * Creates a new signature verifier
     *
     * @return A new signature verifier
     */
    static CSignatureVerifier* NewL();

	/**
     * Creates a new signature verifier and leaves it on the cleanup stack
     *
     * @return A new signature verifier
	 */
    static CSignatureVerifier* NewLC();
    
    ~CSignatureVerifier();    

  public : // Business methods

	/**
	 * This is the main service provided by this class, it verifies a signature.
	 *
	 * @param aSchemeOid       The OID of the signature scheme used
	 * @param aPublicKey       The public key used to verity the signature
	 * @param aControllerData  The signed data
	 * @param aSignatureData   The actual signature data
	 *
	 * @return ETrue if the signature can be verified, EFalse otherwise
	 */	
	TBool VerifySignatureL(const TDesC& aSchemeOid, 
						   const CSubjectPublicKeyInfo& aPublicKey, 
						   const TPtrC8& aControllerData,
						   const TDesC8& aSignatureData);
  

  protected : // Helpers

	/**
	 * An internal method which given a signature OID converts it 
	 * to its corresponding internal TAlgorithmId code
	 * 
	 * @param aSchemeOid The OID we wish to convert
	 * 
	 * @return The TAlgorithmId identifier corresponding to the given OID
	 */
	TAlgorithmId OidToIdL(const TDesC& aSchemeOid);

	/**
	 * Helper method which builds an ASN.1 Digest info structure for
	 * RSA signatures. RSA signatures work on digest info structure rather 
	 * than on raw digest as DSA does. After the method call the ASN.1 Sequence
	 * returned is left on the cleanup stack.
	 *
	 * @param aDigest    The actual message digest
	 * @param aPublicKey The RSA public key
	 * @param aDigestId  The digest identifier
	 *
	 * @return An ASN.1 structure containing the digest info needed to either
	 *         verify or sign some data
	 */  
	CASN1EncSequence* DigestInfoLC(const TDesC8& aDigest, const CRSAPublicKey& aPublicKey, 
	                               TAlgorithmId aDigestId);	

	/**
	 * This internal method creates an appropriate CMessageDigest given a certain
	 * digest OID
	 *
	 * @param aSchemeOid The OID of the digest scheme we are interested in
	 *
	 * @return An instance of CMessageDigest corresponding to the given OID
	 */	                           
	CMessageDigest* CreateDigestL(const TDesC& aSchemeOid);
	

  private : // Life cycle methods
    CSignatureVerifier();
    void ConstructL();      

  private : 
  };

} // namespace Swi

#endif // __SIGNATUREVERFIER_H__
