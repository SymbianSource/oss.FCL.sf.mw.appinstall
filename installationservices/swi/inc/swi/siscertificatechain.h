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
* Definition of the Swi::Sis::CCertificateChain
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __SISCERTIFICATECHAIN_H__
#define __SISCERTIFICATECHAIN_H__

#include <e32base.h>
#include "sisfield.h"
#include "sisblob.h"

namespace Swi
{
namespace Sis
 {

 class TPtrProvider;

/**
 * This class represents a SisCertificateChain. SisCertificateChain is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CCertificateChain : public CField
	{
public:

	/**
	 * This creates a new CCertificateChain object.
	 */
	IMPORT_C static CCertificateChain* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CCertificateChain object and places it on the cleanup stack.
	 *
	 * @param aDataProvider      An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCertificateChain representing the data read.	 	 	 
	 */
	IMPORT_C static CCertificateChain* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CCertificateChain object in place.
	 *
	 * @param aDataProvider      An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCertificateChain representing the data read.	 	 	 
	 */
	
	IMPORT_C static CCertificateChain* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CCertificateChain object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider      An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCertificateChain representing the data read.	 	 	 
	 */

	IMPORT_C static CCertificateChain* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CCertificateChain();

public: // Accessors

	TPtrC8 Data() const;

private:

	CCertificateChain();

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
	CBlob* iData; ///< Certificate chain data
	};

// inline functions from CCertificateChain

inline TPtrC8 CCertificateChain::Data() const
	{
	return iData->Data();
	}

 } // namespace Sis
} //namespace Swi


#endif // #ifndef __SISCERTIFICATECHAIN_H__
