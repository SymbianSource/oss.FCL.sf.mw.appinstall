// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/** 
* @file barscimpl.h
*
* @internalComponent
* @released
*/

#ifndef __BARSCIMPL_H__
#define __BARSCIMPL_H__

#include <string>
#include "commontypes.h"

//Forward declarations
struct SDictionaryCompressionData;
class RDictionaryCompressionBitStream;
/** 
Accesses a resource file and reads the resource data into a buffer.
It is the implementation class for CResourceFile class.
*/
class RResourceFileImpl
	{

public:
	RResourceFileImpl();
	~RResourceFileImpl();
	/** 
	Opens the resource file reader.
	The resource file reader must be opened before reading resources.
	@param aName name of the RSC file.
	@param aFileOffset RSC file offset 
	@param aFileSize RSC file size
	*/
	void OpenL(const std::string& aName, TUint32 aFileOffset=0, TInt aFileSize=0);
	/** 
	Reads a resource after allocating heap memory for it.
	Ownership of the allocated heap is passes to the caller who must 
	free it.	
	@param aResourceId The numeric id of the resource to be read.
	@return Pointer to the heap containing the resource.
	*/
	Ptr8* AllocReadL(const TInt& aResourceId);

	/** Initialises the offset value from the first resource.

	The function tests to catch cases where the first resource is not an RSS_SIGNATURE.
	It assumes that the first resource in the file consists of
	two 32-bit integers. The first integer contains the version number and
	the second is a self-referencing link whose value is the offset for
	the resources in the file, plus 1.This function must be called before
	calling Offset(), AllocReadL(), AllocReadLC() or ReadL().
	*/
	void ConfirmSignatureL();
	
	/** 
	Tests whether the resource file owns the specified resource id.
	@param aResourceId The resource id to test.
	@return True, if the resource file owns the id, false otherwise.
	*/
	TBool OwnsResourceId(const TInt& aResourceId) const;

	/** 
	Function to read specified bytes from the RSC file
	@param aPos Offset from where to begin reading
	@param aData Buffer to store read values
	@param aLength Length of the bytes to be read.
	@return Length of the bytes to be read.
	*/
	TInt ReadL(TInt aPos,TUint8* aData,const TInt& aLength);

	TUidType UidType() const;


private:
	struct SSigRecord
		{
		TInt signature;
		TInt offset;
		};
  enum
		{
		EFlagIsRomFile																=0x80000000,
		EFlagPotentiallyContainsCompressedUnicode									=0x40000000,
		EFlagDictionaryCompressed													=0x20000000,
		EFlagThirdUidIsOffset														=0x10000000,
		EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource							=0x08000000,
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode	=0x04000000,
		EFlagCalypsoFileFormat														=0x02000000,
		EFlagIsBufferRscFile														=0x01000000,
		EAllFlags																	=0xff000000
		};
	enum
		{
		EOffsetBits	=0xfffff000,
		EIdBits		=0x00000fff
		};

	class TExtra;


private:	
	/** 
	Function to read specified bytes from the RSC file
	@param aFlags Integer conatainig no of resource and status flags.
	@param aPos Offset from where to begin reading
	@param aData Buffer to store read values
	@param aLength Length of the bytes to be read.
	@return Length of the bytes to be read.
	*/
	TInt ReadL(const TUint32& aFlags, TInt aPos, TUint8* aData, const TInt& aLength);

	/** @internalComponent
	@return The first resource record.
	@panic Some BAFL panic codes, if the file is corrupted.
	@leave KErrCorrupt The file is corrupted.
	Some other error codes are possible too.
	The method could panic or leave depending on the state of
	iAssertObj member of RResourceFileImpl::TExtra class. */
	SSigRecord FirstRecordL() const;

	
	/** 
	Function to retrieve the header and 
	resource index information of the RSC file.
	*/
	void ReadHeaderAndResourceIndexL();
	
	/** 
	The method will decomress the unicode data (aCompressedUnicode argument) and append
	the decompressed data to the end of aBuffer (aBuffer argument).

	@param aBuffer Destination buffer.
	@param aCompressedUnicode Pointer to compressed unicode data.
	@param aLengthOfCompressedUnicode Length of compressed unicode data.
	*/
	void AppendDecompressedUnicodeL(
								Ptr8* aBuffer,
								const TUint8*  aCompressedUnicode,
								const TInt& aLengthOfCompressedUnicode) const;
	/** 
	The method will decompress the unicode data (aInputResourceData argument), allocate enough
	memory from the heap for the decompressed data, copy the data there and return a pointer
	to the decompressed data.
	The method doesn't own the allocated heap memory for the decompressed data. It's a caller
	responsibility to deallocate the allocated memory.
	@param aInputResourceData Compressed data.
	@return Pointer to decompressed data 
	*/
	Ptr8* DecompressUnicodeL(const Ptr8* aInputResourceData) const;


	/**
	The method will decompress the dictionary compressed data, allocate enough
	memory from the heap for the decompressed data, copy the data there and return a pointer
	to the decompressed data.
	The method doesn't own the allocated heap memory for the decompressed data. It's a caller
	responsibility to deallocate the allocated memory.
	@pre OpenL() is called.
	@leave KErrCorrupt The file is corrupted.
	@leave KErrNoMemory There is not enough memory for the decompressed data.
	Some other error codes are possible too.
	*/
	Ptr8* DictionaryDecompressedResourceDataL(
								TInt aResourceIndex,
								TUint aFlags,
								const SDictionaryCompressionData& aDictionaryCompressionData,
								const Ptr16* aIndex) const;


	/** 
	The method will decomress the dictionary compressed data (aDictionaryCompressionData argument) and append
	the decompressed data to the end of std::vector<RDictionaryCompressionBitStream> 
	(aStackOfDictionaryCompressionBitStreams argument).
	*/
	void AppendDictionaryCompressionBitStreamL(
								std::vector<RDictionaryCompressionBitStream>& aStackOfDictionaryCompressionBitStreams,
								TUint aFlags,
								const SDictionaryCompressionData& aDictionaryCompressionData,
								TInt aStartOfBitData,
								TInt aStartOfIndex,
								TInt aIndexEntry) const;
	
	/**
	Get the two bytes(in Little Endian format) from the specified buffer
	@param aBuffer Buffer address from which 2-bytes are to be achieved.
	@param aIndexOfFirstByte Offset of the first byte to be retrieved from the
           buffer.
	@return 2-bytes read from the buffer.
	*/
	
	TInt LittleEndianTwoByteInteger(const TUint8* aBuffer,const TInt& aIndexOfFirstByte,TInt aLength) const;


private:
	std::ifstream* iResourceContents;	
	TInt iSizeOfLargestResourceWhenCompletelyUncompressed;
	//basically an array of (unsigned) 16-bit file-positions - 
	//this is only used for non-dictionary-compressed resource files
	Ptr16* iIndex; 

	//the position of this member in the class is exposed because RResourceFile::Offset() is 
	//an inline function accessing "iOffset". RResourceFileImpl is an implementation class for
	//RResourceFile class. The "iOffset" offset from the beginning of the class must be exactly
	//12 bytes.
	TInt iOffset; 
	TExtra* iExtra;
	TUint32 iFlagsAndNumberOfResources;
	};


struct SDictionaryCompressionData
	{
 	inline SDictionaryCompressionData() :
		iStartOfDictionaryData(0),
		iStartOfDictionaryIndex(0),
		iNumberOfDictionaryEntries(0),
		iStartOfResourceData(0),
		iStartOfResourceIndex(0),
		iNumberOfBitsUsedForDictionaryTokens(0), 
		iCachedDictionaryIndex(0),
		iCachedResourceBuffer(0)// = NULL;
		{
		}
	TInt iStartOfDictionaryData;
	TInt iStartOfDictionaryIndex;
	TInt iNumberOfDictionaryEntries;
	TInt iStartOfResourceData;
	TInt iStartOfResourceIndex;
	TInt iNumberOfBitsUsedForDictionaryTokens;
	TUint16* iCachedDictionaryIndex;
	TUint8* iCachedResourceBuffer;
	
	};


/** 
Stores the RSC filesize, offset and the bit-array 
representing resources containing compressed unicode.
*/
class RResourceFileImpl::TExtra
	{

public:
	TExtra();
	~TExtra();
	/**
	Finds whether the specified resource contain compressed unicode
	@param aRscIdx Resource Index
	@return Status of whether the resource is actually present or not.
	*/
	TInt32 ContainsCompressedUnicode(TInt& aRscIdx, TBool aFirstRscIsGen) const;

private:
	// Prevent default copy constructor
	TExtra(const TExtra&);
	// Prevent default "=" operator
	TExtra& operator=(const TExtra&);

public:
	// RSC file UID
	TUidType iUidType;
	// An array of bits, one for each resource in the resource file
	Ptr8* iBitArrayOfResourcesContainingCompressedUnicode;
	SDictionaryCompressionData iDictionaryCompressionData;
	// Offset of RSC chunk
	TInt iFileOffset;	
	// RSC file size 
	TUint32 iFileSize;		
	};


#endif//__BARSCIMPL_H__
