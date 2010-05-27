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
* Definition of the Swi::Sis::CCapabilities
*
*/




/**
 @file 
 @internalTechnology
*/

#ifndef __SISCAPABILITIES_H__
#define __SISCAPABILITIES_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

/**
 * This class represents a Siscapabilities. Siscapabilities is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CCapabilities : public CField
	{
public:

	/**
	 * This creates a new CCapabilities object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCapabilities representing the data read.	 
	 */
	IMPORT_C static CCapabilities* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour = EReadType);

	/**
	 * This creates a new CCapabilities object and places it on the cleaup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCapabilities representing the data read.	 	 
	 */
	IMPORT_C static CCapabilities* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour = EReadType);
	
	/**
	 * This creates a new CCapabilities object in place.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCapabilities representing the data read.	 	 
	 */
	
	IMPORT_C static CCapabilities* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour = EReadType);
	
	/**
	 * This creates a new CCapabilities object in place and places it on the cleaup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCapabilities representing the data read.	 	 
	 */
	
	IMPORT_C static CCapabilities* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour = EReadType);

	IMPORT_C virtual ~CCapabilities();

	/**
	 * Accessor for the actual raw data wrapped up by this capabilities.
	 */
	inline const TDesC8& Data() const;

private:

	CCapabilities();

	/**
	 * The second-phase constructor.	 
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour);

	/**
	 * The second-phase constructor.	 
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param acapabilitiesReadBehaviour Whether we need to check the sis type or not.	 
	 */

	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour acapabilitiesReadBehaviour);

private:

	TPtrC8 iDataPtr;
	HBufC8* iData;
	};
	
// inline functions from CCapabilities

const TDesC8& CCapabilities::Data() const
	{
	return iDataPtr;	
	}

 } // namespace Sis
} //namespace Swi



#endif
