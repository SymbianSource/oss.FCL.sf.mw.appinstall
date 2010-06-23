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
* Definition of the Swi::Sis::CSignature
*
*/


/**
 @file
 @released
 @internalTechnology 
*/

#ifndef __SISSIGNATURE_H__
#define __SISSIGNATURE_H__

#include <e32base.h>

#include "sisfield.h"
#include "sisblob.h"

namespace Swi
{
namespace Sis
 {

class CSignatureAlgorithm;
class TPtrProvider;

/**
 * This class represents a SisSignature. SisSignature is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CSignature : public CField
	{
public:

	/**
	 * This creates a new CSignature object.
	 *
	 * @param aDataProvider      An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignature representing the data read.	 
	 */
	IMPORT_C static CSignature* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignature object and places it on the cleanup stack.
	 *
	 * @param aDataProvider       An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour  Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignature representing the data read.	 
	 */
	IMPORT_C static CSignature* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignature object in place.
	 *
	 * @param aDataProvider       An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour  Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignature representing the data read.	 
	 */

	IMPORT_C static CSignature* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignature object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider       An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour  Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignature representing the data read.	 
	 */
	 
	IMPORT_C static CSignature* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CSignature();

private:

	CSignature();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:

	/**
	 * Accessor for the actual signature data.
	 * 
	 * @return The raw signature data, suitable for signature verification.
	 */
	TPtrC8 Data() const;

	/**
	 * Accessor for the signature algorithm.
	 * 
	 * @return The signature algorith used.
	 */
	const CSignatureAlgorithm& Algorithm() const;


private:
	CSignatureAlgorithm *iAlgorithm;
	CBlob *iData;

	};

// inline functions from CSignature

inline TPtrC8 CSignature::Data() const
	{
	return iData->Data();
	}
	
inline const CSignatureAlgorithm& CSignature::Algorithm() const
	{
	return *iAlgorithm;
	}


 } // namespace Sis
} //namespace Swi



#endif
