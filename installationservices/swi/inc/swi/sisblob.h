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
* Definition of the Swi::Sis::CBlob
*
*/




/**
 @file 
 @internalTechnology
*/

#ifndef __SISBLOB_H__
#define __SISBLOB_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

 class TPtrProvider;

/**
 * This class represents a Sisblob. Sisblob is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CBlob : public CField
	{
public:

	/**
	 * This creates a new CBlob object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CBlob representing the data read.	 
	 */
	IMPORT_C static CBlob* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CBlob object and places it on the cleaup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CBlob representing the data read.	 	 
	 */
	IMPORT_C static CBlob* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CBlob object using in place data.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CBlob representing the data read.	 	 
	 */

	IMPORT_C static CBlob* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CBlob object using in place data and places it on the cleaup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CBlob representing the data read.	 	 
	 */

	IMPORT_C static CBlob* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);


	IMPORT_C virtual ~CBlob();

	/**
	 * Accessor for the actual raw data wrapped up by this blob.
	 */
	inline const TDesC8& Data() const;

private:

	CBlob();

	/**
	 * The second-phase constructor.	 
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.	 
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);
	
	/**
	 * The second-phase in-place constructor.	 
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param ablobReadBehaviour Whether we need to check the sis type or not.	 
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	TPtrC8 iDataPtr;
	HBufC8* iData;
	};
	
// inline functions from CBlob

const TDesC8& CBlob::Data() const
	{
	return iDataPtr;	
	}

 } // namespace Sis
} //namespace Swi



#endif
