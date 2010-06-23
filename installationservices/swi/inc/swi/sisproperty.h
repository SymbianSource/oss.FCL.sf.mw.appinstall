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
* Definition of the Swi::Sis::CProperty
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISPROPERTY_H__
#define __SISPROPERTY_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
class CString;
class TPtrProvider;

/**
 * This class represents a SisProperty. SisProperty is a basic structure found in SIS files.
 * Each property represented by this object is stored as a (key,value) pair. Both
 * key and value are 32 bits integers.
 *
 * For a full description see SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CProperty : public CField
	{
public:
	/**
	 * This creates a new CProperty object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperty representing the data read.	 	 
	 */
	IMPORT_C static CProperty* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperty object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperty representing the data read.	 	 
	 */
	IMPORT_C static CProperty* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperty object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperty representing the data read.	 	 
	 */

	IMPORT_C static CProperty* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CProperty object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CProperty representing the data read.	 	 
	 */

	IMPORT_C static CProperty* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CProperty();

	/**
	 * Accessor for the key of the property. 
	 *
	 * @return The key of this property as a 32 bits integer
	 */
	inline TInt32 Key() const;
	
	/**
	 * Accessor for the value of the property.
	 *
	 * @return The value of the property as a 32 bits inetger.
	 */
	inline TInt32 Value() const;

private:

	CProperty();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	TInt32 iKey;

	TInt32 iValue;
	};


// inline functions from CProperty

TInt32 CProperty::Key() const
	{
	return iKey;		
	}

TInt32 CProperty::Value() const
	{
	return iValue;		
	}
	




 } // namespace Sis
} //namespace Swi



#endif
