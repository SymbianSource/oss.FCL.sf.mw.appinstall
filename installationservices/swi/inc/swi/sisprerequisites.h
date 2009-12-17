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
* Definition of the Swi::Sis::CPrerequisites
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISPREREQUISITES_H__
#define __SISPREREQUISITES_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

class CDependency;
class TPtrProvider;

/**
 * This class represents a CPrerequisites. This is a structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CPrerequisites : public CField
	{
public:

	/**
	 * This creates a new CPrerequisites object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CPrerequisites representing the data read.	 
	 */
	IMPORT_C static CPrerequisites* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CPrerequisites object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CPrerequisites representing the data read.		 	 	 	 	 	 
	 */
	IMPORT_C static CPrerequisites* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CPrerequisites object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CPrerequisites representing the data read.		 	 	 	 	 	 
	 */

	IMPORT_C static CPrerequisites* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CPrerequisites object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CPrerequisites representing the data read.		 	 	 	 	 	 
	 */

	IMPORT_C static CPrerequisites* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CPrerequisites();

	/**
	 * This function returns the dependencies.
	 */
	inline  const RPointerArray<CDependency>& Dependencies() const;

	/**
	 * This function returns the list of target devices for this SIS file.
	 */	
	inline  const RPointerArray<CDependency>& TargetDevices() const;

private:

	CPrerequisites();

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

	/**
	 * List of dependencies
	 */
	RPointerArray<CDependency> iDependencies;

	/**
	 * List of target devices
	 */
	RPointerArray<CDependency> iTargetDevices;
	};
	
// inline functinos

const RPointerArray<CDependency>& CPrerequisites::Dependencies() const
	{
	return iDependencies;		
	}
	
const RPointerArray<CDependency>& CPrerequisites::TargetDevices() const
	{
	return iTargetDevices;
	}

 } // namespace Sis
} // namespace Swi

#endif
