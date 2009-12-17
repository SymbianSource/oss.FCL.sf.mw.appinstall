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
* Definition of the Swi::Sis::CSupportedOption
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISSUPPORTEDOPTION_H__
#define __SISSUPPORTEDOPTION_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
class CString;
class TPtrProvider;

/**
This class represents a SisSupportedOption. SisSupportedOption is a basic structure found
in SIS files.  It is described in SGL.GT0188.251.

@released
@internalTechnology
*/
class CSupportedOption : public CField
	{
public:

	/**
	 * This creates a new CSupportedOption object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOption representing the data read.		 	 	 	 	 	 
	 */
	IMPORT_C static CSupportedOption* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOption object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOption representing the data read.		 	 	 	 	 	 
	 */
	IMPORT_C static CSupportedOption* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOption object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOption representing the data read.		 	 	 	 	 	 
	 */

	IMPORT_C static CSupportedOption* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CSupportedOption object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CSupportedOption representing the data read.		 	 	 	 	 	 
	 */

	IMPORT_C static CSupportedOption* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	IMPORT_C virtual ~CSupportedOption();

	inline const RPointerArray<CString>& Names() const;

private:

	CSupportedOption();

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
	 * The supported option names.
	 */
	RPointerArray<CString> iOptionNames;
	};


// inline functions from CSupportedOption

const RPointerArray<CString>& CSupportedOption::Names() const
	{
	return iOptionNames;		
	}




 } // namespace Sis
} //namespace Swi



#endif
