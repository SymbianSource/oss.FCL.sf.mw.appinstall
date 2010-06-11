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


/**
 @file 
 @internalComponent
 @released
*/

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include <openssl/err.h>
#include <openssl/x509.h>
#include <iostream>

#include "commontypes.h"
#include "sisblob.h"
#include "signsis.h"
#include "sissignaturecertificatechain.h"
#include "sisinstallblock.h"
#include "sissignature.h" 

CSignSis::CSignSis(const std::wstring aSisFileName)
	{
	ERR_load_crypto_strings ();
	OpenSSL_add_all_algorithms ();
	OpenSSL_add_all_ciphers ();
	OpenSSL_add_all_digests ();

	iSisContents.Load(aSisFileName);
	Verify();
	}

void CSignSis::SignSis(	const std::wstring& aOutputFileName, 
						const std::wstring& aCertificate, 
						const std::wstring& aPrivateKey, 
						const std::wstring& aPassPhrase, 
						const CSISSignatureAlgorithm::TAlgorithm aAlgorithm)
	{
	CSignatureCertChainData scc;
	CSisSignatureCertificateChain certChain(scc);
	CSISController& controller = const_cast<CSISController&>(iSisContents.Controller());
	certChain.Sign(aAlgorithm, aCertificate, aPrivateKey, aPassPhrase, controller.RawBuffer(), controller.BufferSize());
	controller.AddSignatureChain(scc);
	Verify();
	iSisContents.PrepareCrcs();
	iSisContents.Save(aOutputFileName);
	}

void CSignSis::RemoveSignature(const std::wstring& aOutputFileName)
	{
	CSISController& controller = const_cast<CSISController&>(iSisContents.Controller());
	controller.RemoveSignature();
	Verify();
	iSisContents.PrepareCrcs();
	iSisContents.Save(aOutputFileName);
	}
void CSignSis::RemoveAllSignatures(const std::wstring& aOutputFileName)
	{
	CSISController& controller = const_cast<CSISController&>(iSisContents.Controller());
	int signatureCount = 0;
	do
		{
		controller.RemoveSignature();
		signatureCount = controller.SignatureCount();
		}while(signatureCount > 0);
	iSisContents.PrepareCrcs();
	iSisContents.Save(aOutputFileName);
	}

void CSignSis::Verify()
	{
	// Verify CSISContents
	iSisContents.Verify(iSisContents.LanguageCount());
	const CSISController& controller = iSisContents.Controller();
	int signatureCount = controller.SignatureCount();
	// Verify all the signatures
	for(int i = 0; i < signatureCount; ++i)
		{
		CSignatureCertChainData& scc = const_cast<CSignatureCertChainData&>(controller.SignatureCertChain(i));
		CSisSignatureCertificateChain certChain(scc);
		certChain.VerifySignature(&controller, controller.PostHeaderPos());
		}
	// Verify stored hash with hash calculated from the files
	// Skip the sis files with no data present (pre-installed apps)
	if(iSisContents.SisData().DataPresent())
		{
		// Check that the hash values stored in each file description correlate with the actual file data 
		// provided. This prevents a sis file being signed if it contains an incorrect hash. It 
		// thus prevents the situation where a malicious file with the specified hash can later be 
		// substituted into the signed sis file.   
		VerifyHashAgainstFileData(iSisContents.InstallBlock(), iSisContents.DataUnit());
		}
	}

void CSignSis::VerifyHashAgainstFileData(const CSISInstallBlock& aInstallBlock, const CSISDataUnit& aDataUnit)
	{
	TFileDescList fileList;
	aInstallBlock.GetFileList(fileList);
	for(int i = 0; i < fileList.size(); ++i)
		{
		const CSISFileDescription* fdesc = fileList[i];

		// Retrieve the stored hash data 
		const CSISHash& storedSisHash = fdesc->Hash();   
		CSISBlob storedDigestBlob = storedSisHash.Blob();
		TUint8* storedDigest = storedDigestBlob.Data();

		// Check if the algorithm is correct
 		CSISException::ThrowIf (!(storedSisHash.VerifyAlgorithm()), CSISException::EVerification, "unsupported hash algorithm");

		// First check if the hash exists (to cater for the FILENULL case)					
		if (storedDigest)  
			{
			// Make a call to compute the hash of the actual file data
			TSHADigest digest;
			const CSISFileData& fileData = aDataUnit.FileData(fdesc->FileIndex());
			CalculateHash(digest, fileData.Data (), fileData.UncompressedSize ());
			
			CSISHash computedSISHash;
			computedSISHash.SetHash(digest, SHA_DIGEST_LENGTH);
															 
			// Extract the computed hash data 													
			CSISBlob computedBlob = computedSISHash.Blob();
			TUint8* computedDigest = computedBlob.Data();
			
								 
		 	// The computed data shouldn't be Null (as it has already been determined that 
		 	// the stored digest is not Null)
			CSISException::ThrowIf ((!computedDigest), CSISException::EVerification, "computed hash data is NULL");	

																	 			 
			// The two hashes must match to ensure integrity of the sis file.
			CSISException::ThrowIf (!(computedBlob == storedDigestBlob), CSISException::EVerification, "Stored and computed hash values do not match");	
			}
		}
	}

CSignSis::~CSignSis()
	{
	EVP_cleanup ();
	ERR_free_strings ();
	}


