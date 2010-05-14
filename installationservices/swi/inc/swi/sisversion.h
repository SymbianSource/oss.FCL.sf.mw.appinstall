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
* Definition of the CVersion
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISVERSION_H__
#define __SISVERSION_H__

#include <e32base.h>
#include "sisfield.h"


namespace Swi 
{
namespace Sis
 {

 class TPtrProvider;

/**
 * This class represents a SISVersion. SISVersion is a basic structure found in SIS files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CVersion : public CField
	{
public:

	/**
	 * This creates a new CVersion object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CVersion representing the data read.		 
	 */
	IMPORT_C static CVersion* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CVersion object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CVersion representing the data read.		 
	 */
	IMPORT_C static CVersion* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CVersion object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CVersion representing the data read.		 
	 */

	IMPORT_C static CVersion* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CVersion object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CVersion representing the data read.		 
	 */
	
	IMPORT_C static CVersion* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CVersion();

private:

	CVersion();

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
	 * This function returns the major version number.
	 */
	inline TInt32 Major() const;
	
	/**
	 * This function returns the minor version number.
	 */
	inline TInt32 Minor() const;

	/**
	 * This function returns the build version number.
	 */
	inline TInt32 Build() const;

private:

	/**
	 * The major version number.
	 */
	TInt32 iMajor;

	/**
	 * The minor version number.
	 */
	TInt32 iMinor;

	/**
	 * The build version number.
	 */
	TInt32 iBuild;
	};

// inline functions from CVersion

TInt32 CVersion::Major() const
	{
	return iMajor;
	}

TInt32 CVersion::Minor() const
	{
	return iMinor; 
	}

TInt32 CVersion::Build() const
	{
	return iBuild;
	}

 } // namespace Sis
} // namespace Swi

#endif
