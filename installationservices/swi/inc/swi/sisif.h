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
* Definition of the Swi::Sis::CIf
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISIF_H__
#define __SISIF_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
class CElseIf;
class CExpression;
class CInstallBlock;
class TPtrProvider;

/**
 * This class represents a SisIf. SisIf is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CIf : public CField
	{
public:

	/**
	 * This creates a new CIf object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CIf representing the data read.	 	 	 	 	 	 
	 */
	IMPORT_C static CIf* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CIf object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CIf representing the data read.		 
	 */
	IMPORT_C static CIf* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CIf object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CIf representing the data read.		 
	 */
	
	IMPORT_C static CIf* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CIf object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CIf representing the data read.		 
	 */

	IMPORT_C static CIf* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CIf();

public: // Accessors

	inline const CExpression& Expression() const;

	inline const CInstallBlock& InstallBlock() const;

	inline const RPointerArray<CElseIf>& ElseIfs() const;
	
private:

	CIf();

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
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	CExpression* iExpression;
	
	CInstallBlock* iInstallBlock;
	
	RPointerArray<CElseIf> iElseIfs;
	};

// inline functions from CIf

const CExpression& CIf::Expression() const
	{
	return *iExpression;		
	}

const CInstallBlock& CIf::InstallBlock() const
	{
	return *iInstallBlock;	
	}

const RPointerArray<CElseIf>& CIf::ElseIfs() const
	{
	return iElseIfs;
	}


 } // namespace Sis
} //namespace Swi



#endif /* __SISIF_H__ */
