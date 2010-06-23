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
* Definition of the Swi::Sis::CDataUnit
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISDATAUNIT_H__
#define __SISDATAUNIT_H__


#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
 class CFileData;
 
/**
 * This class represents a SisDataUnit. This is the structure found in SISX files
 * wrapping up a list of SisFileData entities. It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CDataUnit : public CField
	{
public:

	/**
	 * This creates a new CDataUnit object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDataUnit representing the data read.	 
	 */
	static CDataUnit* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CDataUnit object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CDataUnit representing the data read.	 	 
	 */
	static CDataUnit* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	virtual ~CDataUnit();

public: // Business Methods

	/**
	 * Extracts the file data identified by aFileIndex into aFile.
	 * The data will be uncompressed if necessary.
	 *
	 * @param aFile		 A file, open for writing, where the data will be stored.
	 * @param aFileIndex The index identifying the file data to extract.
	 *
	 */
	void ExtractDataFileL(RFile& aFile, TInt aFileIndex);
	
	/**
	 * Extracts the next aLength bytes of file data identified by aFileIndex 
	 * into aFile. The data will be uncompressed if necessary.
	 *
	 * @param aFile		A file, open for writing, where the data will be stored.
	 * @param aFileIndex The index identifying the file data to extract.
	 * @param aLength		The length of datat to extract.
	 */
	void ExtractDataFileL(RFile& aFile, TInt aFileIndex, TInt64 aLength);
	
private: // Life cycle methods

	/**
	 * The constructor.
	 */
	CDataUnit();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	RPointerArray<CFileData> iCompressedFiles;

	};

 } // namespace Sis

} // namespace Swi


#endif // __SISDATAUNIT_H__
