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

#include <openssl/pem.h>
#include <openssl/dsa.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#include <iostream>
#include <fstream>
#include <string>


#include "sissignaturealgorithm.h"
#include "utility.h"
#include "utility_interface.h"


#define RSA_IDENTIFIER				L"1.2.840.113549.1.1.5"
#define DSA_IDENTIFIER				L"1.2.840.10040.4.3"
#define DSA_PRIME_SIZE				1024
#define OLD_PEM_FORMAT_TAG			"Proc-Type"
#define ENCRYPTION_ALGORITHM_TAG	"DEK-Info"
#define PEM_HEADER					"----"


void CSISSignatureAlgorithm::SetAlgorithm (const TAlgorithm aAlgorithm)
	{
	switch (aAlgorithm)
		{
	case EAlgRSA :
		iAlgorithmIdentifier = RSA_IDENTIFIER;
		break;
	case EAlgDSA :
		iAlgorithmIdentifier = DSA_IDENTIFIER;
		break;
	default :
		throw CSISException (CSISException::EIllegal, L"Bad signature algorithm");
		}
	iAlgorithm = aAlgorithm;
	}

CSISSignatureAlgorithm::TAlgorithm CSISSignatureAlgorithm::Algorithm ()	const
	{
	if ((iAlgorithm == EAlgNone) && ! iAlgorithmIdentifier.empty ())
		{
		if (iAlgorithmIdentifier == RSA_IDENTIFIER)
			{
			iAlgorithm = EAlgRSA;
			}
		else
		if (iAlgorithmIdentifier == DSA_IDENTIFIER)
			{
			iAlgorithm = EAlgDSA;
			}
		else
			{
			throw CSISException (CSISException::EIllegal, L"Bad signature algorithm");
			}
		}
	return iAlgorithm;
	}


void CSISSignatureAlgorithm::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISSignatureAlgorithm>::Verify (aLanguages);
	if (	(iAlgorithmIdentifier != RSA_IDENTIFIER) &&
			(iAlgorithmIdentifier != DSA_IDENTIFIER))

		{
		throw CSISException (CSISException::EIllegal, L"Bad signature algorithm");
		}
	}
