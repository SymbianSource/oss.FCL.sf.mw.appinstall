/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#include "sissignaturecertificatechain.h"
#include "signaturecertchaindata.h"
#include "siscertificatechain.h"

#include "exception.h"
#include "utility.h"
#include "sissignature.h"
#include "siscontroller.h" 


CSisSignatureCertificateChain::CSisSignatureCertificateChain (CSignatureCertChainData& aSisSignatureCertChain)
						: iSisSignatureCertChain(aSisSignatureCertChain)
	{
	CSISArray <CSignatureData, CSISFieldRoot::ESISSignature>	&signatures = iSisSignatureCertChain.Signatures();
	int count = signatures.size();
	
	iCertificateChain = new CSisCertificateChain(const_cast<CCertChainData&>(iSisSignatureCertChain.CertificateChain()));
	}


CSisSignatureCertificateChain::~CSisSignatureCertificateChain()
	{
	for(int i = 0; i < iSignatures.size(); ++i)
		{
		delete iSignatures[i];
		}
	iSignatures.clear();
	delete iCertificateChain;
	}

void CSisSignatureCertificateChain::Sign (
		const CSISSignatureAlgorithm::TAlgorithm aAlgorithm, const std::wstring& aCertificate,
		const std::wstring& aPrivateKey, const std::wstring& aPassPhrase,  const TUint8* aBuffer, const TUint32 aBufferSize)
	{
	iCertificateChain->Load (aCertificate);
	CSignatureData signatureContent;
	CSignature* signature = new CSignature(signatureContent);
	if (aAlgorithm != CSISSignatureAlgorithm::EAlgNone)
		{
		signatureContent.SetAlgorithm (aAlgorithm);
		}
	signature->Sign (aPrivateKey, aPassPhrase, aBuffer, aBufferSize);
	iSisSignatureCertChain.AddSignature(signatureContent);
	iSignatures.push_back (signature);
	}


void CSisSignatureCertificateChain::VerifySignature (const CSISController* aController, const TSISStream::pos_type aParentHeaderSize) const
	{
	int signatureCount = iSignatures.size ();
	if (signatureCount != 0)
		{
		X509* x509 = iCertificateChain->GetBottomX509 ();
		if (x509)
			{
			int index;
			try
				{
				for (index = 0; index < signatureCount; index++)
					{
					int size = 0;

					if (iSisSignatureCertChain.PreHeaderPos() <= 0)
						{
						size = aController->BufferSize ();
						}
					else
						{
						size = iSisSignatureCertChain.PreHeaderPos() - aParentHeaderSize;
						assert (size <= aController->BufferSize ());
						}
	
					iSignatures [index]->VerifySignature (x509, aController->RawBuffer(), size);
					}
				}
			catch (...)
				{

				CSignature* signature = iSignatures[index];

				if (!signature->SignatureAlgorithm().IsAlgorithmKnown())
					{
					SISLogger::Log(L"Could not verify signature with unknown algorithm, continuing.\n");
					}
				else
					{
					X509_free (x509);
					SISLogger::Log(L"Could not verify signature with known algorithm, exiting.\n");
					throw;
					}
				}
			X509_free (x509);
			}
		}
	}

