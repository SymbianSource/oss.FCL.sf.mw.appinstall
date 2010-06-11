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
* Definition of the Swi::Sis::CSignatureCertificateChain
*
*/


/**
 @file
 @released
 @internalTechnology
*/

#ifndef __SISSIGNATURECERTIFICATECHAIN_H__
#define __SISSIGNATURECERTIFICATECHAIN_H__

#include <e32base.h>
#include "sisfield.h"

#include "sissignature.h"

namespace Swi
{
namespace Sis
 {

class CCertificateChain;

/**
 * This class represents a SisSignatureCertificateChain. SisSignatureCertificateChain is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CSignatureCertificateChain : public CField
	{
public:

	/**
	 * This creates a new CSignatureCertificateChain object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureCertificateChain representing the data read.	 
	 */
	IMPORT_C static CSignatureCertificateChain* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureCertificateChain object and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureCertificateChain representing the data read.
	 */
	IMPORT_C static CSignatureCertificateChain* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureCertificateChain object in place.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureCertificateChain representing the data read.
	 */

	IMPORT_C static CSignatureCertificateChain* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureCertificateChain object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureCertificateChain representing the data read.
	 */

	IMPORT_C static CSignatureCertificateChain* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CSignatureCertificateChain();

private:

	CSignatureCertificateChain();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in place constructor.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */

	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:

	/**
	 * Accessor for the signatures contained by this entity.
	 *
	 * @return An array containing all the signature objects found in this entity.
	 */
	const RPointerArray<CSignature>& Signatures() const;

	/**
	 * Accessor for the certificate chains contained by this entity.
	 *
	 * @return An array containing all the certificate chain objects found in this entity.
	 */
	const CCertificateChain& CertificateChain() const;

private:

	RPointerArray<CSignature> iSignatures;

	CCertificateChain* iCertificateChain;
	};

// inline functions from CSignatureCertificateChain

inline const RPointerArray<CSignature>& CSignatureCertificateChain::Signatures() const
	{
	return iSignatures;
	}

inline const CCertificateChain& CSignatureCertificateChain::CertificateChain() const
	{
	return *iCertificateChain;
	}

 } // namespace Sis
} //namespace Swi



#endif
