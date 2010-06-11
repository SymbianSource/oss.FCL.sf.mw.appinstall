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
* Definition of the Swi::Sis::CDateTime
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __SISDATETIME_H__
#define __SISDATETIME_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
class CDate;
class CTime;
class TPtrProvider;

/**
 * This class represents a SisDateTime. SisDateTime is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CDateTime : public CField
	{
public:

	/**
	 * This creates a new CDateTime object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDateTime representing the data read.		 	 
	 */
	IMPORT_C static CDateTime* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDateTime object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDateTime representing the data read.		 	 
	 */
	IMPORT_C static CDateTime* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDateTime object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDateTime representing the data read.		 	 
	 */

	IMPORT_C static CDateTime* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDateTime object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDateTime representing the data read.		 	 
	 */

	IMPORT_C static CDateTime* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CDateTime();

private:

	CDateTime();

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
public:

	/**
	 * This function returns the first datetime.
	 */
	inline const CDate& Date() const;

	/**
	 * This function returns the second datetime.
	 */
	inline const CTime& Time() const;

private:
	CDate* iDate;

	CTime* iTime;
	};

// inline functions from CDateTime

const CDate& CDateTime::Date() const
	{
	return *iDate;
	}

const CTime& CDateTime::Time() const
	{
	return *iTime;
	}


 } // namespace Sis
} //namespace Swi



#endif
