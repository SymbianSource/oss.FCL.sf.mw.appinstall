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
* Definition of the CSISString
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISSTRING_H__
#define __SISSTRING_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

 class TPtrProvider;
/**
 * This class represents a SISString. SISString is a basic structure found in SIS files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CString : public CField
	{
public:
	/**
	 * This creates a new CString object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CString representing the data read.
	 */
	IMPORT_C static CString* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CString object and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CString representing the data read.	 
	 */
	IMPORT_C static CString* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CString object in place.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CString representing the data read.	 
	 */

	IMPORT_C static CString* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CString object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CString representing the data read.	 
	 */

	IMPORT_C static CString* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CString();

	/**
	 * Accessor to the actual string data.
	 * 
	 * @return A Descriptor representation of the string.
	 */
	inline const TDesC& Data() const;
	
	/**
	 *
	 * Returns a writeable descriptor to this buffer, for use in SIS
	 * stubs where we are technically rewriting the SIS file
	 *
	 * @return A writeable descriptor for this data
	 *
	 */
	 
	 inline TPtr Des() const;

private:

	CString();

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

private:
	
	/**
	 * The core data pointer.
	 */
	TPtrC iDataPtr;


	/**
	 * If required, a copy of the string data.
	 */
	HBufC* iData;
	};


// inline functions

const TDesC& CString::Data() const
	{
	return iDataPtr;	
	}
	
TPtr CString::Des() const
	{
	return iData->Des();
	}

 } // namespace Sis
} // namespace Swi

#endif
