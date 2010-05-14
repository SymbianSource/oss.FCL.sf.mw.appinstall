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
* Definition of the Swi::Sis::CData
*
*/


/**
 @file 
 @released
 @internalTechnology
*/
 
#ifndef __SISDATA_H__
#define __SISDATA_H__

#include "sisfield.h"

class RWriteStream;

namespace Swi
{
namespace Sis
 {
 class CDataUnit;
 
/**
 * This class represents a SisData. This is the structure found in SISX files
 * wrapping up the actual file data. It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CData : public CField
	{
public:

	/**
	 * This creates a new CData object and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CData representing the data read.	 
	 */
	static CData* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CData object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CData representing the data read.	 	 
	 *
	 */
	static CData* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	virtual ~CData();


public: // Business methods

	/**
	 * Extracts the data from a given data unit and file index to the given file.
	 *
	 * @param aFile		 A file handle open for writing. The extracted data will be stored there.
	 * @param aFileIndex The index of the file data to extract. Must be greater than or equal to 0 
	 *                   and less than the maximum number of available SISFileData entities. 
	 *                   If this is not the case this funciton will panic.
	 * @param aDataUnit  The index of the data unit to use. Must be greater than or equal to 0 and
	 *                   
	 *
	 */	
	void ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit);

	/**
	 * Extracts some of the data from a given data unit and file index to the given file.
	 *
	 * @param aFile		 A file handle open for writing. The extracted data will be stored there.
	 * @param aFileIndex The index of the file data to extract. Must be greater than or equal to 0 
	 *                   and less than the maximum number of available SISFileData entities. 
	 *                   If this is not the case this funciton will panic.
	 * @param aDataUnit  The index of the data unit to use. Must be greater than or equal to 0 and
	 * @param aLength		The length of data to extract from the file.                  
	 *
	 */	
	void ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit, TInt64 aLength);
	
	/** Append a stub (empty) data object to a stream
	This method is used by the CContents to create a stub sis file based upon a real sis file
	@param aWriteStream The stream to append the stub CData field 
	*/
	static void AppendStubDataFieldL(RWriteStream& aWriteStream);
	
	/** Does this data represent data in a stub SIS file
	@return ETrue if the data is part of a stub sis file
	*/
	TBool IsStub();
	
	/** Accessor for the Crc of the Data field
	*/
	TUint16 Crc() const;

private: // Life-cycle methods

	/**
	 * The constructor.
	 *
	 * @param aDataProvider An instance of a MSisDataProvider to read the entity from.	 
	 */
	CData(MSisDataProvider& aDataProvider);

	/**
	 * The second-phase constructor.
	 * 
	 * @param aTypeReadBehaviour Whether to check the field code or take it for granted.
	 *
	 */
	void ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);
	
private:

	RPointerArray<CDataUnit> iDataUnits;
	MSisDataProvider& iDataProvider;	
	TFieldLength iOffset;

	TUint16 iCrc;
	};

inline TUint16 CData::Crc() const
	{
	return iCrc;
	};

inline TBool CData::IsStub()
	{
	return iDataUnits.Count() == 0 ? ETrue : EFalse;
	};

	} // namespace Sis

} // namespace Swi


#endif // __SISDATA_H__
