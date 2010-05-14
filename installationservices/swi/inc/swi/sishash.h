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
* Definition of the Swi::Sis::CHash#
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISHASH_H__
#define __SISHASH_H__

#include <e32base.h>
#include "sisfield.h"
#include "sisblob.h"

namespace Swi
{
namespace Sis
 {

 class TPtrProvider;

/**
 * This class represents a Sishash. Sishash is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @publishedPartner
 */
class CHash : public CField
	{
public:

	/**
	 * This creates a new CHash object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CHash representing the data read.	 	 	 	 	 
	 */
	IMPORT_C static CHash* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CHash object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CHash representing the data read.	 	 	 	 	 
	 */
	IMPORT_C static CHash* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CHash object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CHash representing the data read.	 	 	 	 	 
	 */
	
	IMPORT_C static CHash* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CHash object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CHash representing the data read.	 	 	 	 	 
	 */

	IMPORT_C static CHash* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CHash();
	
public: // Accessor

	inline TSISHashAlgorithm Algorithm() const;

	inline const TDesC8& Data() const;

private:

	CHash();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	TSISHashAlgorithm iAlgorithm;
	
	CBlob* iBlob;
	};

// inline functions from CHash

TSISHashAlgorithm CHash::Algorithm() const
	{
	return iAlgorithm;
	}

const TDesC8& CHash::Data() const
	{
	return iBlob->Data();	
	}

 } // namespace Sis
} //namespace Swi



#endif
