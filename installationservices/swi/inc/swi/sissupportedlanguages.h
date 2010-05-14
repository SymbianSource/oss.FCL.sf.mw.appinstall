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
* Definition of the CSISSupportedLanguages
*
*/


/**
 @file 
 @released
 @internalTechnology
*/


#ifndef __SISSUPPORTEDLANGUAGES_H__
#define __SISSUPPORTEDLANGUAGES_H__

#include <e32base.h>
#include "sisfield.h"
#include "sislanguage.h"

namespace Swi
{
namespace Sis
 {

class CLanguage;
class TPtrProvider;

/**
 * This class represents a SupportedLanguages. SupportedLanguages is a 
 * basic structure found in  files. It is described in SGL.GT0140.045.
 *
 * @released
 * @internalTechnology 
 */
class CSupportedLanguages : public CField
	{
public:

	/**
	 * This creates a new CSupportedLanguages object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedLanguages representing the data read.		 	 
	 */
	IMPORT_C static CSupportedLanguages* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedLanguages object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedLanguages representing the data read.		 	 	 
	 */
	IMPORT_C static CSupportedLanguages* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedLanguages object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedLanguages representing the data read.		 	 
	 */

	IMPORT_C static CSupportedLanguages* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedLanguages object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedLanguages representing the data read.		 	 
	 */

	IMPORT_C static CSupportedLanguages* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CSupportedLanguages();

private:

	CSupportedLanguages();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in place constructor.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.	 
	 */

	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:

	/**
	 * This function returns the number of supported languages.
	 */
	inline TInt Count() const;

	/**
	 * This operator returns the language at index n.
	 */
	inline TLanguage operator [](TInt n) const;

private:

	/**
	 * The list of Language objects.
	 */
	RPointerArray<CLanguage> iLanguages;
	};

// inline functions
TInt CSupportedLanguages::Count() const
	{
	return iLanguages.Count();
	}

TLanguage CSupportedLanguages::operator [](TInt n) const
	{
	return iLanguages[n]->Language();
	}

 } // namespace Sis
} //namespace Swi

#endif
