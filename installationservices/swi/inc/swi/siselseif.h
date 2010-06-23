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
* Definition of the Swi::Sis::CElseIf
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISELSEIF_H__
#define __SISELSEIF_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
class CElseElseIf;
class CExpression;
class CInstallBlock;
class TPtrProvider;

/**
 * This class represents a SisElseIf. SisElseIf is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CElseIf : public CField
	{
public:

	/**
	 * This creates a new CElseIf object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CIf representing the data read.	 	 	 	 	 	 	 
	 */
	IMPORT_C static CElseIf* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CElseIf object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CElseIf representing the data read.	 
	 */
	IMPORT_C static CElseIf* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CElseIf object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CElseIf representing the data read.	 
	 */

	IMPORT_C static CElseIf* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CElseIf object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CElseIf representing the data read.	 
	 */

	IMPORT_C static CElseIf* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CElseIf();

public: // Accessors

	inline const CExpression& Expression() const;

	inline const CInstallBlock& InstallBlock() const;

private:

	CElseIf();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */

	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	CExpression* iExpression;
	
	CInstallBlock* iInstallBlock;
	};


// inline functions from CElseIf

const CExpression& CElseIf::Expression() const
	{
	return *iExpression;	
	}

const CInstallBlock& CElseIf::InstallBlock() const
	{
	return *iInstallBlock;	
	}

 } // namespace Sis
} //namespace Swi



#endif // __SISELSEIF_H__
