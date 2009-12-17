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
* Definition of the Swi::Sis::CUid
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISUID_H__
#define __SISUID_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
 
 class TPtrProvider;

/**
 * This class represents a SisUid. SisUid is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CUid : public CField
	{
public:

	/**
	 * This creates a new CUid object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CUid representing the data read.	 
	 */
	IMPORT_C static CUid* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CUid object and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CUid representing the data read.	 
	 */
	IMPORT_C static CUid* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CUid object in place.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CUid representing the data read.	 
	 */

	IMPORT_C static CUid* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CUid object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CUid representing the data read.	 
	 */

	IMPORT_C static CUid* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CUid();

private:

	CUid();

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
	 * This function returns the actual uid.
	 * 
	 * @return The actual underlying Uid value
	 */
	inline TUid Uid() const;

private:
	/**
	 * The first uid.
	 */
	TUid iUid;
	};


// inline functions from CUid

TUid CUid::Uid() const
	{
	return iUid;
	}

 } // namespace Sis
} //namespace Swi



#endif
