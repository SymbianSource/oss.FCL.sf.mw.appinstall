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
* Definition of the CSignatureAlgorithm
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISSIGNATUREALGORITHM_H__
#define __SISSIGNATUREALGORITHM_H__

#include <e32base.h>
#include "sisfield.h"


namespace Swi 
{
namespace Sis
 {
class CString;
class TPtrProvider;

/**
 * This class represents a SISSignatureAlgorithm. SISSignatureAlgorithm is a basic structure found in SIS files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CSignatureAlgorithm : public CField
	{
public:

	/**
	 * This creates a new CSignatureAlgorithm object.
	 *
	 * @param aDataProvider      An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureAlgorithm representing the data read.	 	 	 
	 */
	IMPORT_C static CSignatureAlgorithm* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureAlgorithm object and places it on the cleanup stack.
	 *
	 * @param aDataProvider      An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureAlgorithm representing the data read.	 	 	 	 
	 */
	IMPORT_C static CSignatureAlgorithm* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureAlgorithm object in place.
	 *
	 * @param aDataProvider      An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureAlgorithm representing the data read.	 	 	 	 
	 */

	IMPORT_C static CSignatureAlgorithm* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSignatureAlgorithm object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider      An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSignatureAlgorithm representing the data read.	 	 	 	 
	 */

	IMPORT_C static CSignatureAlgorithm* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CSignatureAlgorithm();

private:

	CSignatureAlgorithm();

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

public: // Accessors

	const CString& AlgorithmIdentifier() const;

private:

	CString* iAlgorithmIdentifier;
	};

// inline functions from CSignatureAlgorithm;

inline const CString& CSignatureAlgorithm::AlgorithmIdentifier() const
	{
	return 	*iAlgorithmIdentifier;
	}


 } // namespace Sis
} // namespace Swi

#endif
