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
*
*/


#include "signatureverifier.h"
#include "log.h"
#include <swi/sisinstallerrors.h>

#include <x509cert.h>
#include <x509keys.h> 
#include <hash.h> 
#include <asn1enc.h>
#include <x509keyencoder.h>

using namespace Swi;


//
// Life cycle methods
//

/*static*/ CSignatureVerifier* CSignatureVerifier::NewL()
	{
	CSignatureVerifier* self = CSignatureVerifier::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CSignatureVerifier* CSignatureVerifier::NewLC()
	{
	CSignatureVerifier* self = new(ELeave) CSignatureVerifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
    
CSignatureVerifier::~CSignatureVerifier()
	{
	}

CSignatureVerifier::CSignatureVerifier()
	{
	}

void CSignatureVerifier::ConstructL()
	{
	}


//
// Business Method
//

TBool CSignatureVerifier::VerifySignatureL(const TDesC& aSchemeOid, 
										   const CSubjectPublicKeyInfo& aPublicKey, 
										   const TPtrC8& aControllerData, 
										   const TDesC8& aSignatureData)
	{
	CMessageDigest* digester = CreateDigestL(aSchemeOid);	
	CleanupStack::PushL(digester);
	
	TPtrC8 digest = digester->Final(aControllerData);
	
	TX509KeyFactory keyFactory;
	TBool result = EFalse;
			
	switch (aPublicKey.AlgorithmId())
		{
		case ERSA :
			{
			DEBUG_PRINTF(_L8("Security Manager - Verifying RSA signature."));			
			
		    //  Get the public key  
		    CRSAPublicKey* publicKey = keyFactory.RSAPublicKeyL(aPublicKey.KeyData());
		    CleanupStack::PushL(publicKey);
		 
		    CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*publicKey);
		 
		    RInteger sig = RInteger::NewL(aSignatureData);
		    CleanupStack::PushL(sig);
		    CRSASignature* theSignature = CRSASignature::NewL(sig);
		    CleanupStack::Pop(&sig);
		    CleanupStack::PushL(theSignature);
		    
			// Do the ASN1 thing
			TAlgorithmId digestAlgId = OidToIdL(aSchemeOid);
			CASN1EncBase* digestInfo = DigestInfoLC(digest, *publicKey, digestAlgId);

		    // DER encode it!
		    HBufC8* digestBuf = HBufC8::NewMaxLC(digestInfo->LengthDER());
		    TPtr8 oct(digestBuf->Des());
		    oct.FillZ();
		 	 
		    TUint writePos = 0;
		    digestInfo->WriteDERL(oct, writePos);		    
		 	     	    
		    result = verifier->VerifyL(*digestBuf, *theSignature);
		    
		    CleanupStack::PopAndDestroy(5, publicKey); //  digestBuf, digestInfo, theSignature
		                                               //  verifier, publicKey											
			}    
			break;

		case EDSA :
			{
			DEBUG_PRINTF(_L8("Security Manager - Verifying DSA signature."));					

			CDSAPublicKey* publicKey = keyFactory.DSAPublicKeyL(aPublicKey.EncodedParams(), 
																aPublicKey.KeyData());
			CleanupStack::PushL(publicKey);

			CVerifier<CDSASignature>* verifier = CDSAVerifier::NewL(*publicKey);
			CleanupStack::PushL(verifier);

			CDSASignature* sig = keyFactory.DSASignatureL(aSignatureData);
			CleanupStack::PushL(sig);
		
			result = verifier->VerifyL(digest, *sig);			

			CleanupStack::PopAndDestroy(3, publicKey); // verifier, sig, publicKey
			}
			break;
			
		default:
			{
			DEBUG_PRINTF(_L8("Security Manager - Signature scheme uses unsupported asymetric algorithm."));
			User::Leave(KErrNotSupported);
			}
		}
			
	CleanupStack::PopAndDestroy(digester);
	return result;
	}
	
	
//
// Helpers
//

CASN1EncSequence* CSignatureVerifier::DigestInfoLC(const TDesC8& digest, 
											   const CRSAPublicKey& publicKey, 
											   TAlgorithmId aDigestId)
     {
     CASN1EncSequence* seq = CASN1EncSequence::NewLC();
     
     // DigestAlgorithmIdentifier   
     TX509RSAKeyEncoder keyEncoder(publicKey, aDigestId);
     CASN1EncSequence* digestAlgID = keyEncoder.EncodeDigestAlgorithmLC();
 
     seq->AddAndPopChildL(digestAlgID); 
     
     // Actual message digest    
     CASN1EncOctetString* octet = CASN1EncOctetString::NewLC(digest);
     seq->AddAndPopChildL(octet);
     
     return seq;
     }

TAlgorithmId CSignatureVerifier::OidToIdL(const TDesC& aSchemeOid)
	{
	if ( (aSchemeOid.Compare(KSHA1WithRSA) != 0) &&
		(aSchemeOid.Compare(KDSAWithSHA1) != 0) )
		{
		User::Leave(KErrDigestNotSupported);
		}
	return ESHA1;
	}

CMessageDigest* CSignatureVerifier::CreateDigestL(const TDesC& aSchemeOid)
	{
	if ( (aSchemeOid.Compare(KSHA1WithRSA) != 0) &&
		(aSchemeOid.Compare(KDSAWithSHA1) != 0) )
		{
		DEBUG_PRINTF(_L8("Security Manager - Signature scheme uses unsupported hash algorithm."));
		User::Leave(KErrSignatureSchemeNotSupported); 
		}
	DEBUG_PRINTF(_L8("Security Manager - Signature scheme uses SHA1 hash algorithm"));
	return CMessageDigestFactory::NewDigestL(CMessageDigest::ESHA1);		
	}

	
