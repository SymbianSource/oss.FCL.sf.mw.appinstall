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
* Definition of the Swi::Sis::CDependency
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __SISDEPENDENCY_H__
#define __SISDEPENDENCY_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

class CUid;
class CVersionRange;
class CString;
class TPtrProvider;

/**
 * This class represents a SisDependency. SisDependency is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CDependency : public CField
	{
public:

	/**
	 * This creates a new CDependency object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDependency representing the data read.	 	 
	 */
	IMPORT_C static CDependency* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDependency object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDependency representing the data read.	 	 	 
	 */
	IMPORT_C static CDependency* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDependency object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDependency representing the data read.	 	 	 
	 */

	IMPORT_C static CDependency* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDependency object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDependency representing the data read.	 	 	 
	 */

	IMPORT_C static CDependency* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CDependency();

public: // Accessors

	inline const CUid& Uid() const;

	inline const CVersionRange* VersionRange() const;

	inline const RPointerArray<CString>& Names() const;

private:

	CDependency();

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

private:

	CUid* iUid;
	
	CVersionRange* iVersionRange;
	
	RPointerArray<CString> iNames;
	};

// inline functions from CDependency

const CUid& CDependency::Uid() const
	{
	return *iUid;
	}

const CVersionRange* CDependency::VersionRange() const
	{
	return iVersionRange;
	}

const RPointerArray<CString>& CDependency::Names() const
	{
	return iNames;
	}


 } // namespace Sis
} //namespace Swi



#endif
