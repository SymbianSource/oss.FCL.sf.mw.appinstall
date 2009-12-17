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
* Definition of the Swi::Sis::CSupportedOptions
*
*/


/**
 @file  
 @released
 @internalTechnology
*/


#ifndef __SISSUPPORTEDOPTIONS_H__
#define __SISSUPPORTEDOPTIONS_H__

#include <e32base.h>
#include "sisfield.h"
#include "sislanguage.h"

namespace Swi
{
namespace Sis
 {

class CLanguage;
class CSupportedOption;
class TPtrProvider;

/**
 * This class represents a SupportedOptions. SupportedOptions is a 
 * basic structure found in  files. It is described in SGL.GT0140.045.
 *
 * @released
 * @internalTechnology
 */
class CSupportedOptions : public CField
	{
public:

	/**
	 * This creates a new CSupportedOptions object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOptions representing the data read.		 	 	 	 	 
	 */
	IMPORT_C static CSupportedOptions* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOptions object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOptions representing the data read.		 	 	 	 	 
	 */
	IMPORT_C static CSupportedOptions* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOptions object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOptions representing the data read.		 	 	 	 	 
	 */

	IMPORT_C static CSupportedOptions* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOptions object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOptions representing the data read.		 	 	 	 	 
	 */

	IMPORT_C static CSupportedOptions* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CSupportedOptions();

private:

	CSupportedOptions();

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
	 * This function returns the number of supported options. Note that it can be 0.
	 *
	 * @return The number of supported options. Zero if no options are supported.
	 */
	inline TInt Count() const;

	/**
	 * This operator returns the language at index n.
	 *
	 * @return The n-th supported option.
	 */
	inline const CSupportedOption& operator [](TInt n) const;

private:

	/**
	 * The list of Language objects.
	 */
	RPointerArray<CSupportedOption> iOptions;
	};

// inline functions
TInt CSupportedOptions::Count() const
	{
	return iOptions.Count();
	}

const CSupportedOption& CSupportedOptions::operator [](TInt n) const
	{
	return *iOptions[n];
	}

 } // namespace Sis
} //namespace Swi

#endif
