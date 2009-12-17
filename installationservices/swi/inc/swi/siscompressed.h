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
* Definition of the Swi::Sis::CCompressed
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISCOMPRESSED_H__
#define __SISCOMPRESSED_H__

#include "sisfield.h"

namespace Swi
{
namespace Sis
 {
 
/**
 * This class represents a SisCompressed entity. This is an <i>envelope</i> which
 * wraps up data encoded with a given compression scheme. Currently we support
 * two compression schemes: 
 * 
 * <ul> 
 *      <li> <b>ECompressDeflate.</b> Data compressed using the zip algorithm
 *      <li> <b>ECompressNone.</b> Denotes uncompressed data
 * </ul>
 *
 * A full description can be found in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CCompressed : public CField
	{
public:

	/**
	 * This creates a new CCompressed object.
	 *
	 * @param aDataProvider 	  An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour  Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCompressed representing the data read.	 	 
	 */
	static CCompressed* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CController object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CCompressed representing the data read.
	 */
	static CCompressed* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	virtual ~CCompressed();

public: // Business methods

	/**
	 * This function reads and returns the <b>uncompressed</b> SISX controller
	 * data. This raw data can be fed into Swi::Sis::CController for parsing.
	 *
	 * @return A buffer containing the raw, uncompressed, SISX controller data.
	 */	
	HBufC8* ReadControllerDataL();
	
	/**
	 * Extracts the enveloped data into the given file.
	 *
	 * @param aFile A file handle open for writing. It will store the
	 *              enveloped data into this file.
	 */
	void ExtractDataFieldL(RFile& aFile);
	

	/**
	 * Extracts some of the enveloped data into the given file. This may be called
	 * multiple times in order to extract all of the data
	 *
	 * @param aFile	A file handle open for writing. It will store the
	 *              	enveloped data into this file.
	 * @param aLength The length of the data to extract.
	 */
	void ExtractDataFieldL(RFile& aFile, TInt64 aLength);

	void ExtractDataFieldL(MSisDataProvider& aDataProvider, RFile& aFile, TInt64 aLength);

	/** the Crc of this CCompressed field
	*/
	TUint16 Crc() const;
private: // Helper methods

	/**
	 * This method instantiate a proper MSisDataProvider instance to handle the
	 * enveloped data. This abstracts us away from the compressed/uncompressed 
	 * storage of data.
	 *
	 * @return An instance of MSisDataProvider which can be used to 
	 *     extract the enveloped file data.
	 */
	MSisDataProvider& DataProviderL();

	
private: // Life cycle methods

	/**
	 * The constructor.
	 *
	 * @param aDataProvider A instance of MSisDataProvider used to access the raw data.
	 */
	CCompressed(MSisDataProvider& aDataProvider);

	/**
	 * The second-phase constructor.
	 *
	 * @param aTypeReadBehaviour Whether to check the field code or take it for granted.
	 */
	void ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);
	
private:
	MSisDataProvider& iDataProvider;
	MSisDataProvider* iCompressedReader;
	TFieldLength iUncompressedLength;
	TFieldLength iOffset;
	
	MSisDataProvider* iCurrentDataProvider; ///< Used to keep current data provider pointer across calls to ExtractDataFieldL
	TInt32 iBytesExtracted;
	
	TUint16 iCrc;
	};

inline TUint16 CCompressed::Crc() const
	{
	return iCrc;
	};
	
 } // namespace Sis

} // namespace Swi

#endif // __SISCOMPRESSED_H__
