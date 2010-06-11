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
* Definition of the Swi::Sis::CFileData
*
*/


/**
 @file 
 @released
 @internalTechnology
*/
 
#ifndef __SISFILEDATA_H__
#define __SISFILEDATA_H__

#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
 
 class CCompressed;
 
/**
 * This class represents a SisFileData entity. This structure holds
 * the actual data of a file stored into a sisx package. 
 * For a full description see SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CFileData : public CField
	{
public:

	/**
	 * This creates a new CFileData object and leaves it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileData representing the data read.
	 */
	static CFileData* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CFileData object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileData representing the data read.
	 */
	static CFileData* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	virtual ~CFileData();

public: // Business Methods

	/**
	 *
	 * @param aFile
	 */
	void ExtractDataFieldL(RFile& aFile);

	void ExtractDataFieldL(RFile& aFile, TInt64 aLength);

	
private: // Life-cycle methods

	CFileData();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	
	CCompressed* iCompressedFile;	

	};

 } // namespace Sis

} // namespace Swi


#endif // __SISFILEDATA_H__
