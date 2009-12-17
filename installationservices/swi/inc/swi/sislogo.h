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
* Definition of the Swi::Sis::CLogo
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISLOGO_H__
#define __SISLOGO_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

class CFileDescription;
class TPtrProvider;

/**
 * This class represents a SisLogo. SisLogo is a basic structure found in SIS files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CLogo : public CField
	{
public:

	/**
	 * This creates a new CLogo object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CLogo representing the data read.		 
	 */
	IMPORT_C static CLogo* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CLogo object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CLogo representing the data read.		 
	 */
	IMPORT_C static CLogo* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CLogo object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CLogo representing the data read.		 
	 */

	IMPORT_C static CLogo* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CLogo object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CLogo representing the data read.		 
	 */

	IMPORT_C static CLogo* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CLogo();

	/**
	 * Accessor to the SISFileDescription containing the logo binary data
	 */
	inline const CFileDescription& FileDescription() const;
	
private:

	CLogo();

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
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	CFileDescription* iFileDescription;
	};



// inline functions from CLogo

const CFileDescription& CLogo::FileDescription() const
	{
	return *iFileDescription;
	}

 } // namespace Sis
} //namespace Swi



#endif
