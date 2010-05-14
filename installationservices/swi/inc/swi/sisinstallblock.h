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
* Definition of the Swi::Sis::CInstallBlock
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISINSTALLBLOCK_H__
#define __SISINSTALLBLOCK_H__

#include <e32base.h>

#include "sisfield.h"
#include "sisfiledescription.h"
#include "sisif.h"
#include "siscontroller.h"

namespace Swi
{
namespace Sis
 {

 class TPtrProvider;

/**
 * This class represents a SisInstallBlock. SisInstallBlock is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CInstallBlock : public CField
	{
public:

	/**
	 * This creates a new CInstallBlock object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInstallBlock representing the data read.	 	 	 	 
	 */
	IMPORT_C static CInstallBlock* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CInstallBlock object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInstallBlock representing the data read.	 	 	 	 
	 */
	IMPORT_C static CInstallBlock* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CInstallBlock object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInstallBlock representing the data read.	 	 	 	 
	 */
	
	IMPORT_C static CInstallBlock* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CInstallBlock object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInstallBlock representing the data read.	 	 	 	 
	 */

	IMPORT_C static CInstallBlock* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CInstallBlock();

public: // Accessors

	inline const RPointerArray<CFileDescription>& FileDescriptions() const;

	inline const RPointerArray<CController>& EmbeddedControllers() const;

	inline const RPointerArray<CIf>& IfStatements() const;

private:

	CInstallBlock();

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
	RPointerArray<CFileDescription> iFileDescriptions;
	
	RPointerArray<CController> iEmbeddedControllers;

	RPointerArray<CIf> iIfStatements;
	};


// inline functions from CInstallBlock

const RPointerArray<CFileDescription>& CInstallBlock::FileDescriptions() const
	{
	return iFileDescriptions;
	}

const RPointerArray<CController>& CInstallBlock::EmbeddedControllers() const
	{
	return iEmbeddedControllers;
	}

const RPointerArray<CIf>& CInstallBlock::IfStatements() const
	{
	return iIfStatements;
	}


 } // namespace Sis
} //namespace Swi

#endif
