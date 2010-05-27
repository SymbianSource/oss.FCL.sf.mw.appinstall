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
* Definition of the CSISProperties
*
*/


/**
 @file 
 @released
 @internalTechnology
*/


#ifndef __SISPROPERTIES_H__
#define __SISPROPERTIES_H__

#include <e32base.h>
#include "sisfield.h"
#include "sislanguage.h"

namespace Swi
{
namespace Sis
 {

class CProperty;
class TPtrProvider;

/**
 * This class represents a SISProperties. SISProperties is a 
 * basic structure found in  files. It is described in SGL.GT0140.045.
 *
 * @released
 * @internalTechnology
 */
class CProperties : public CField
	{
public:

	/**
	 * This creates a new CProperties object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperties representing the data read.	 	 	 
	 */
	IMPORT_C static CProperties* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperties object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperties representing the data read.	 	 	 
	 */
	IMPORT_C static CProperties* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperties object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperties representing the data read.	 	 	 
	 */

	IMPORT_C static CProperties* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperties object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperties representing the data read.	 	 	 
	 */

	IMPORT_C static CProperties* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CProperties();

private:

	CProperties();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *	 
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:

	/**
	 * This function returns the number of supported options.
	 */
	inline TInt Count() const;
	
	/**
	 * Accessor for the n-th property
	 *
	 * @return The n-th property.
	 */
	inline const CProperty& operator [](TInt n) const;

private:

	/**
	 * The actual list of property objects.
	 */
	RPointerArray<CProperty> iProperties;
	};

// inline functions
TInt CProperties::Count() const
	{
	return iProperties.Count();
	}

const CProperty& CProperties::operator [](TInt n) const
	{
	return *iProperties[n];
	}

 } // namespace Sis
} //namespace Swi

#endif
