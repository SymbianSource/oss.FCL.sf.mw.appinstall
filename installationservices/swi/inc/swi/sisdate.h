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
* Definition of the Swi::Sis::CDate
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISDATE_H__
#define __SISDATE_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
 
 class TPtrProvider;

/**
 * This class represents a SisDate. SisDate is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CDate : public CField
	{
public:

	/**
	 * This creates a new CDate object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDate representing the data read.		 	 	 
	 */
	IMPORT_C static CDate* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDate object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDate representing the data read.		 	 	 	 
	 */
	IMPORT_C static CDate* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDate object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDate representing the data read.		 	 	 	 
	 */

	IMPORT_C static CDate* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDate object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDate representing the data read.		 	 	 	 
	 */

	IMPORT_C static CDate* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	IMPORT_C virtual ~CDate();

private:

	CDate();

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

public: // Accessor

	inline TUint16 Year() const;

	inline TUint8 Month() const;

	inline TUint8 Day() const;

private:

	TUint16 iYear;
	TUint8 iMonth;
	TUint8 iDay;
	};

// inline functions from CDate

TUint16 CDate::Year() const
	{
	return iYear;
	}

TUint8 CDate::Month() const
	{
	return iMonth;
	}
	
TUint8 CDate::Day() const
	{
	return iDay;
	}

 } // namespace Sis
} //namespace Swi



#endif
