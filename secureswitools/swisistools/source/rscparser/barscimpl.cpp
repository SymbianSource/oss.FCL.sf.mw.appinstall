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
* @file BaRscImpl.cpp
*
* @internalComponent
* @released
*/
#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <sstream>
#include "barsc2.h"
#include "barscimpl.h"
#include "dictionarycompression.h"
#include "ucmp.h"
#include "util.h"
#include "parse.h"

#define REINTERPRET_CAST(type,exp) (reinterpret_cast<type>(exp))
#define CONST_CAST(type,exp) (const_cast<type>(exp))
#define STATIC_CAST(type,exp) (static_cast<type>(exp))

inline TUint8* MemCopy(TAny* aTrg, const TAny* aSrc, TInt aLength)
{ return (TUint8*)memmove(aTrg, aSrc, aLength) + aLength; }


/** Cleanup function.
@internalComponent
@param aArrayOfDictionaryCompressionBitStreams Pointer to an vector of 
RDictionaryCompressionBitStream objects which have to be closed. */
void CloseArrayOfDictionaryCompressionBitStreams(TAny* aArray)
{
	typedef std::vector<RDictionaryCompressionBitStream> RDictComprBitStream;
	RDictComprBitStream* array = static_cast <RDictComprBitStream*> (aArray);
	for (TInt i=array->size()-1;i>=0;--i)
		{
		array[i].clear();
		}
	array->clear();
}

RResourceFileImpl::TExtra::TExtra():
	iBitArrayOfResourcesContainingCompressedUnicode(NULL),
	iFileOffset(0),
	iFileSize(0)
{
}


RResourceFileImpl::TExtra::~TExtra()
{
	delete [] iDictionaryCompressionData.iCachedResourceBuffer;
	iDictionaryCompressionData.iCachedResourceBuffer = NULL;
	delete [] iDictionaryCompressionData.iCachedDictionaryIndex;
	iDictionaryCompressionData.iCachedDictionaryIndex = 0;

	delete iBitArrayOfResourcesContainingCompressedUnicode;
}


TInt32 RResourceFileImpl::TExtra::ContainsCompressedUnicode(TInt& aRscIdx, TBool aFirstRscIsGen) const
{
	
	if (aFirstRscIsGen)
	{
		// dictionary-compressed resource files can have an automatically generated 
		//resource which is the bit-array of resources containing compressed Unicode 
		//(this automatically generated resource does not have a corresponding bit 
		//for itself in the bit-array as it would be self-referring...)
		--aRscIdx; 
		if (aRscIdx<0)
		{
			//aRscIdx is referring to the automatically generated resource 
			//(which is the bit-array of resources containing compressed Unicode)
			return EFalse; 
			}
	}
	
	assert(aRscIdx>=0);
	
	if (iBitArrayOfResourcesContainingCompressedUnicode==NULL)
		{
		return EFalse;
		}
	
	TInt index = aRscIdx/8;
	assert(index < iBitArrayOfResourcesContainingCompressedUnicode->GetLength());
	return (*iBitArrayOfResourcesContainingCompressedUnicode)[index]&(1<<(aRscIdx%8));
}


RResourceFileImpl::RResourceFileImpl() :
	iResourceContents(NULL),
	iSizeOfLargestResourceWhenCompletelyUncompressed(0),
	iIndex(NULL),
	iOffset(0),
	iExtra(NULL),
	iFlagsAndNumberOfResources(0)
{
	// Fixed class size - because of the BC reasons.
	// RResourceFileImpl size must be the same as CResourceFile size.
	enum
		{
		KRscFileImplSize = 24
		};	
	assert(sizeof(RResourceFileImpl) == KRscFileImplSize);

	//Fixed "iOffset" position - because of the BC reasons.
	assert(offsetof(RResourceFileImpl, iOffset)==12);
}


RResourceFileImpl::~RResourceFileImpl()
{
	if(iResourceContents)
	{
		if (iResourceContents->is_open())
		{
			iResourceContents->close();
		}
		delete iResourceContents;
	}
	iSizeOfLargestResourceWhenCompletelyUncompressed=0;
	delete iIndex;
	iIndex=NULL;
	if (iExtra)
	{
		delete iExtra;
		iExtra=NULL;
	}

	iFlagsAndNumberOfResources=0;
	iOffset=0;
}

/** Opens the resource file reader.
The resource file reader must be opened before reading resources or
checking the signature of the resource file. 
@internalComponent
@param aName File to open as a resource file.
@param aFileOffset The resource file section offset from the beginning of the file.
@param aFileSize The resource file section size.
@leave - The file is corrupted.
*/
void RResourceFileImpl::OpenL(
						  const std::string& aName, 
						  TUint32 aFileOffset, 
						  TInt aFileSize)
{
	iResourceContents= new std::ifstream(aName.c_str(), std::ios::in|std::ios::binary);
		
	if(!iResourceContents->good())
	{
		std::string errMsg= "Unable to open RSC file. " + aName;
		if (iResourceContents->is_open())
			iResourceContents->close();
		if(iResourceContents)
			delete iResourceContents;
		throw CResourceFileException(errMsg);
	}

	iExtra=new TExtra();
	iExtra->iFileOffset = aFileOffset;
	
	TInt fileSize = 0;
	if (aFileSize)
		{
		fileSize = aFileSize;
		assert(fileSize > TInt(aFileOffset));
		}
	else
		{
		// Get the resource file size
		struct stat resourceFileStats;
    
		if (stat(aName.c_str(),&resourceFileStats) == 0)
			{
			// The size of the file in bytes is in
			// resourceFileStats.st_size
			fileSize=resourceFileStats.st_size;
			assert(fileSize > 0);
			}   	
		else
			{
			if (iResourceContents->is_open())
					iResourceContents->close();		
			std::string errMsg="Invalid RSC File";
			throw CResourceFileException(errMsg);
			}		
		}
	
	iExtra->iFileSize = fileSize;	
	
	// Verify the header of the RSC and get the resource index
	ReadHeaderAndResourceIndexL();
}


/** 
Retrieve the UID tuple of the opened resource file.

@internalComponent
@pre OpenL() has been called successfully.
@return The UIDs of the loaded resource file.
*/
TUidType RResourceFileImpl::UidType() const
{
	assert(iExtra!=NULL);
	return iExtra->iUidType;
}

/** Reads a resource into a heap buffer, returns a pointer to that
buffer.

A heap buffer of appropriate length is allocated for the resource.
Ownership of the heap buffer passes to the caller who must destroy
it. The search for the resource uses the following algorithm:A
resource id in the range 1 to 4095 is looked up in this resource file.
The function leaves if there is no matching resource.If the resource
id is greater than 4095, then the most significant 20 bits of the
resource id is treated as an offset and the least significant 12 bits
is treated as the real resource id. If the offset matches the offset
value defined for this file, then the resource is looked up in this
resource file using the real resource id (i.e. the least significant
12 bits). If the offset does not match, then the function leaves.Note,
do not call this function until a call to
ConfirmSignatureL() has completed successfully.

@internalComponent
@pre OpenL() is called.
@param aResourceId The numeric id of the resource to be read.
@return Pointer to a heap buffer containing the resource.
@panic Some BAFL panic codes, if the file is corrupted.
@leave - The file is corrupted.
@leave - There is no resource with aResourceId in the file.
*/
Ptr8* RResourceFileImpl::AllocReadL(const TInt& aResourceId) 
{
	// Check if the resource id is present in the RSC file
	if (!OwnsResourceId(aResourceId))
	{
		std::ostringstream errDispStream;
		
		errDispStream<<"Resource ID:";
		errDispStream<<aResourceId;
		errDispStream<<" is not present in the RSC file";
		
		throw CResourceFileException(errDispStream.str());
	}
	
	//"-1" because the first resource has ID 0x*****001 (not 0x*****000)
	TInt resourceIndex= (aResourceId & EIdBits)-1; 
	
	if (iFlagsAndNumberOfResources & EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource)
	{
		assert(iFlagsAndNumberOfResources & EFlagDictionaryCompressed);
		assert(iFlagsAndNumberOfResources & EFlagThirdUidIsOffset);
		
		if (resourceIndex>0)
		{
			--resourceIndex;
		}
		else
		{
			assert(resourceIndex==0);

			Ptr8* resourceDataFor_RSS_SIGNATURE = new Ptr8(8);
			if(NULL==resourceDataFor_RSS_SIGNATURE || NULL==resourceDataFor_RSS_SIGNATURE->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}

			resourceDataFor_RSS_SIGNATURE->SetLength(8);
			TUint* wordPointer=REINTERPRET_CAST(TUint*,CONST_CAST(TUint8*,resourceDataFor_RSS_SIGNATURE->GetPtr()));

			wordPointer[0]=4;
			wordPointer[1]=((iExtra->iUidType[2].iUid << 12) | 1);
			return resourceDataFor_RSS_SIGNATURE;
		}
	}
	

	const TBool firstResourceIsGenerated=
		(iFlagsAndNumberOfResources & 
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode);
	if (firstResourceIsGenerated)
	{
		assert(iFlagsAndNumberOfResources & EFlagDictionaryCompressed);
		//dictionary-compressed resource files can have an automatically generated 
		//resource which is the bit-array of resources containing compressed Unicode 
		//(this automatically generated resource does not have a corresponding bit for 
		//itself in the bit-array as it would be self-referring...)
		++resourceIndex; 
	}
	
	assert(resourceIndex>=0);

	Ptr8* const dictionaryDecompressedResourceData = DictionaryDecompressedResourceDataL(resourceIndex,
						iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),	
						iExtra->iDictionaryCompressionData,	
						iIndex);

	// Return the resource data if its not unicode compressed.
	if (!iExtra->ContainsCompressedUnicode(resourceIndex,firstResourceIsGenerated))
	{
		return dictionaryDecompressedResourceData;
	}
		
	Ptr8* const finalResourceData=DecompressUnicodeL(dictionaryDecompressedResourceData);

	delete dictionaryDecompressedResourceData;
	return finalResourceData;
	
}

/** The method will decompress the unicode data (aInputResourceData argument), allocate enough
memory from the heap for the decompressed data, copy the data there and return a buffer
to the decompressed data.

The method doesn't own the allocated heap memory for the decompressed data. It's a caller
responsibility to deallocate the allocated memory.

@internalComponent
@param aInputResourceData Compressed data.
@pre OpenL() is called.
@leave - The file is corrupted.
@leave - There is not enough memory for the decompressed data.
*/
Ptr8* RResourceFileImpl::DecompressUnicodeL(const Ptr8* aInputResourceData) const
{
	const TInt numberOfBytesInInput= aInputResourceData->GetLength();
	assert(iSizeOfLargestResourceWhenCompletelyUncompressed>0);
							
	Ptr8* outputResourceData= new Ptr8(iSizeOfLargestResourceWhenCompletelyUncompressed);
	if(NULL==outputResourceData || NULL==outputResourceData->GetPtr())
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	
	const TUint8* input= aInputResourceData->GetPtr();
	TInt index=0;
	
	TBool decompressRun=ETrue;
	while (1)
	{
		assert(index<numberOfBytesInInput);
		
		TInt runLength=input[index];
		
		// The run-length occupies a single byte if it is less than 128, 
		// otherwise it occupies two bytes (in little-endian byte order), 
		// with the most significant bit of the first byte set to non-zero 
		//to indicate that the run-length occupies two bytes.
		if (runLength & 0x80)
		{
			++index;
			if (index>=numberOfBytesInInput)
			{
				std::string errMsg="Invalid Rsc File";
				throw CResourceFileException(errMsg);
			}
			runLength &= ~0x80;
			runLength <<= 8;
			runLength |= input[index];
		}
		++index;
		if (runLength>0)
		{			
			if (decompressRun)
			{
				AppendDecompressedUnicodeL(
										outputResourceData,				
										const_cast<unsigned char *>(input+index),
										runLength);
			}
			else
			{
				assert(
					(outputResourceData->GetLength() + runLength) <= 
					iSizeOfLargestResourceWhenCompletelyUncompressed);
				
				memcpy((char*)(outputResourceData->GetPtr()+outputResourceData->GetLength()),(char*)(input+index),runLength);				
				outputResourceData->UpdateLength(runLength);
			}
			index+=runLength;
		}
		if (index>numberOfBytesInInput)
		{
			std::string errMsg="Invalid Rsc File";
			throw CResourceFileException(errMsg);
		}
		if (index>=numberOfBytesInInput)
		{
			break;
		}
			decompressRun=!decompressRun;
	}	
	return outputResourceData;
}


/** @internalComponent
@return The first resource record.
@leave - The file is corrupted.
*/
RResourceFileImpl::SSigRecord RResourceFileImpl::FirstRecordL() const
{
	// Added to support reading of rel 6.x resource files.
	// rel 6.x files do not have signatures!
	Ptr8* const firstResource=AllocReadL(1);

	// Basic check to test if the signature is of the correct size.
	if (firstResource->GetLength()!= sizeof(SSigRecord))
	{
		std::string errMsg="Invalid RSS Signature";
		throw CResourceFileException(errMsg);
	}
	SSigRecord sigRecord = *reinterpret_cast<const SSigRecord*>(firstResource->GetPtr());
	delete firstResource;
	return sigRecord;
}

/** Initialises the offset value from the first resource.

The function tests to catch cases where the first resource is not an RSS_SIGNATURE.
It assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL() or ReadL().

@internalComponent
@pre OpenL() is called.
@leave if the file is corrupted.
Some other error codes are possible too.
*/
void RResourceFileImpl::ConfirmSignatureL()
{
	// Added to support reading of rel 6.x resource files.
	SSigRecord firstRecord=FirstRecordL();

	// If the resource offset does not correspond to the first resource
	// this is not a resource signature.
	if ((firstRecord.offset & EIdBits) != 1)
	{
		std::string errMsg="Failed : Invalid RSS Signature";
		throw CResourceFileException(errMsg);
	}
	iOffset=(firstRecord.offset & EOffsetBits);
}

/** The method will decomress the unicode data (aCompressedUnicode argument) and append
the decompressed data to the end of aBuffer (aBuffer argument).

@internalComponent
@pre OpenL() is called.
@param aBuffer Destination buffer.
@param aCompressedUnicode Compressed unicode buffer.
@leave - The file is corrupted.
*/

void RResourceFileImpl::AppendDecompressedUnicodeL(
												   Ptr8* aBuffer,
												   const TUint8*  aCompressedUnicode,
												   const TInt& aLengthOfCompressedUnicode) const
{

	if (aLengthOfCompressedUnicode>0)
	{
		TUint8* startOfDecompressedUnicode= aBuffer->GetPtr() + aBuffer->GetLength();
		
		if (reinterpret_cast<TUint32>(startOfDecompressedUnicode) & 0x01)
		{			
			TUint8 padChar = 0xab;
			memcpy(startOfDecompressedUnicode,&padChar,1);
			++startOfDecompressedUnicode;
			aBuffer->UpdateLength(1);
		}
		
		const TInt maximumOutputLength= (
			iSizeOfLargestResourceWhenCompletelyUncompressed - (aBuffer->GetLength()))/2; 
		
		TMemoryUnicodeSink decompressedUnicode(reinterpret_cast<TUint16*>(startOfDecompressedUnicode));
		
		TInt lengthOfDecompressedUnicode;
		TInt numberOfInputBytesConsumed;
		TUnicodeExpander unicodeExpander;
		
		unicodeExpander.ExpandL(decompressedUnicode,
								aCompressedUnicode,
								maximumOutputLength,
								aLengthOfCompressedUnicode,
								&lengthOfDecompressedUnicode,
								&numberOfInputBytesConsumed);
		TInt temp;
		unicodeExpander.FlushL(decompressedUnicode,maximumOutputLength,temp);
		lengthOfDecompressedUnicode+=temp;
		aBuffer->UpdateLength(lengthOfDecompressedUnicode*2);
		
		assert(numberOfInputBytesConsumed == aLengthOfCompressedUnicode);		
	}
}

/** Tests whether the resource file owns the specified resource id.

The resource file owns the resource id if the most significant 20 bits
of the resource id are zero or match the offset value as returned from
a call to the Offset() member function or if the resource id is not out of range.

@internalComponent
@pre OpenL() is called.
@param aResourceId The resource id to test.
@return True, if the resource file owns the id, false otherwise.
@leave - The file is corrupted.
*/

TBool RResourceFileImpl::OwnsResourceId(const TInt& aResourceId) const
{ 
	// Checks whether Rsc file owns the resource:
	// does so if offset is 0, or matches that given, 
	// and id is in index.

	const TInt offset=(aResourceId & EOffsetBits);
//	if ((offset!=0) && (offset!=iOffset))
//		{
//		return EFalse;
//		}
	
	const TInt resourceIndex=(aResourceId & EIdBits)-1;
	TInt numberOfResources=(iFlagsAndNumberOfResources & ~EAllFlags);
	if (iFlagsAndNumberOfResources & EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource)
	{
		assert(iFlagsAndNumberOfResources & EFlagDictionaryCompressed);
		assert(iFlagsAndNumberOfResources & EFlagThirdUidIsOffset);
		++numberOfResources;
	}
	if (iFlagsAndNumberOfResources & 
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode)
	{
		assert(iFlagsAndNumberOfResources & EFlagDictionaryCompressed);
		--numberOfResources;
	}
	return (resourceIndex >= 0) && (resourceIndex < numberOfResources);
}

TInt RResourceFileImpl::ReadL(
							const TUint32& aFlags, 
							TInt aPos,
							TUint8* aData,
							const TInt& aLength) 
{
	aPos += iExtra->iFileOffset;
	
	assert(aPos >= iExtra->iFileOffset);
	assert(aLength >= 0);
	assert((aPos + aLength) <= (iExtra->iFileOffset + iExtra->iFileSize));	
	
	// Seek to the offset specified by "aPos"
	iResourceContents->seekg(aPos, std::ios_base::beg);
	iResourceContents->read((char*)aData, aLength);	
	return iResourceContents->gcount();
}


TInt RResourceFileImpl::ReadL(TInt aPos, TUint8* aData, const TInt& aLength) 
{
	return ReadL(iFlagsAndNumberOfResources & static_cast<TUint32>(EAllFlags),aPos,aData,aLength);
}


TInt RResourceFileImpl::LittleEndianTwoByteInteger(
													const TUint8* aBuffer,
													const TInt& aIndexOfFirstByte, TInt aLength) const
{
	assert((aIndexOfFirstByte + 1) < aLength);
	return aBuffer[aIndexOfFirstByte] | (aBuffer[aIndexOfFirstByte+1]<<8);
}


/** Function to retrieve the header information of the rsc file and all the
	resource index information in the rsc file. This function is created to
	handle the common functionality in the two OpenL() method.
@internalComponent
@pre OpenL() is called.
*/
	
void RResourceFileImpl::ReadHeaderAndResourceIndexL()
{
	SDictionaryCompressionData dictionaryCompressionData;
		
	TUidType uidType;
	TInt length =0;
	//dictionary-compressed resource files have a 21-byte header, 
	//16 bytes of checked UIDs followed by a 1-byte field and two 2-byte fields
	TUint8 header[21];
	if(iExtra->iFileSize >= 16)
	{
		length = ReadL(0,header,Min((sizeof(header)/sizeof(header[0])),iExtra->iFileSize));
		uidType=TCheckedUid(header, 16).UidType();

		if (uidType[0].iUid==0x101f4a6b)
		{
			iFlagsAndNumberOfResources |= EFlagPotentiallyContainsCompressedUnicode;
			assert(length >= 18);
			iSizeOfLargestResourceWhenCompletelyUncompressed = LittleEndianTwoByteInteger(header,16+1,length);
		}
		else if (uidType[0].iUid==0x101f5010)
		{
			iFlagsAndNumberOfResources |=
					EFlagPotentiallyContainsCompressedUnicode | EFlagDictionaryCompressed;
			assert(length >= 18);
			iSizeOfLargestResourceWhenCompletelyUncompressed = LittleEndianTwoByteInteger(header,16+1,length);
		}
		else if (uidType[0]!=TUid::Null())
		{
			std::string errMsg="Failed : Not Supported. Invalid Registration File.";
			throw CResourceFileException(errMsg);
		}
		//the "signature" of Calypso's resource files
		else if (LittleEndianTwoByteInteger(header,0,length)==4) 
		{
			iFlagsAndNumberOfResources |= EFlagDictionaryCompressed | EFlagCalypsoFileFormat;
			iSizeOfLargestResourceWhenCompletelyUncompressed = LittleEndianTwoByteInteger(header,8,length);
		}
	}
	
	//It seems that the following AssertDebL() call never fails, 
	//because LittleEndianTwoByteIntegerL always 
	//returns zero or positive value.
	assert(iSizeOfLargestResourceWhenCompletelyUncompressed>=0);
	TInt numberOfResources=0;
	Ptr8* bitArrayOfResourcesContainingCompressedUnicode=NULL;
	if (iFlagsAndNumberOfResources & EFlagDictionaryCompressed)
	{
		if (iFlagsAndNumberOfResources & EFlagCalypsoFileFormat)
		{
			assert(length > 10);
			numberOfResources=LittleEndianTwoByteInteger(header,2,length);
			const TInt numberOfBitsUsedForDictionaryTokens = header[10];
			const TInt numberOfDictionaryEntries =
					(1 << numberOfBitsUsedForDictionaryTokens) - header[5];
			assert(numberOfDictionaryEntries >= 0);
			// "+2" because the first entry in the dictionary-index in this file format 
			//is the number of bits from the start of the dictionary data to the start 
			//of the first dictionary entry which is always zero, and thus unnecessary
			const TInt startOfDictionaryData=4+7+2; 
			// "+2" because the first entry in the resource-index in this file format is 
			//the number of bits from the start of the resource data to the start of the 
			//first resource which is always zero, and thus unnecessary
			const TInt startOfResourceIndex=LittleEndianTwoByteInteger(header,6,length)+2; 
			assert(startOfResourceIndex >= 0);
			dictionaryCompressionData.iStartOfDictionaryData=
							startOfDictionaryData+(numberOfDictionaryEntries*2);
			dictionaryCompressionData.iStartOfDictionaryIndex=startOfDictionaryData;
			dictionaryCompressionData.iNumberOfDictionaryEntries=numberOfDictionaryEntries;
			dictionaryCompressionData.iStartOfResourceData=
							startOfResourceIndex+(numberOfResources*2);
			dictionaryCompressionData.iStartOfResourceIndex=startOfResourceIndex;
			dictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens=
							numberOfBitsUsedForDictionaryTokens;
			
			if ((iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsRomFile)) == 0)
			{
				// attempt to cache dictionary index
				// allocate and populate the dictionary index buffer
				dictionaryCompressionData.iCachedDictionaryIndex = new TUint16[numberOfDictionaryEntries];
				if (dictionaryCompressionData.iCachedDictionaryIndex != 0)
				{
					TInt len = numberOfDictionaryEntries * 2;

					Ptr8* ptr8 = new Ptr8(numberOfDictionaryEntries * 2);
					if(NULL==ptr8 || NULL==ptr8->GetPtr())
					{
						std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
						throw CResourceFileException(errMsg);
					}
					ptr8->UpdateLength(numberOfDictionaryEntries * 2);
					ReadL(
						iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags), 	// aFlags
						startOfDictionaryData,											// aPos
						(TUint8*)ptr8->GetPtr(),
						len);															// aLength

					memcpy((TUint8*)dictionaryCompressionData.iCachedDictionaryIndex, ptr8->GetPtr(), len);
					if(NULL != ptr8)
					{
						delete ptr8;
					}
				}
			}	// if (iFlagsAndNumberOfResources & EFlagIsRomFile)
		}
		else
		{
			assert(length==16+1+2+2);
			const TUint firstByteAfterUids=header[16];
			if (firstByteAfterUids & 0x80)
			{
				// this flag is only set if the resource file is dictionary-compressed
				iFlagsAndNumberOfResources |= EFlagThirdUidIsOffset; 
			}
			if (firstByteAfterUids & 0x40)
			{
				// this flag is only set if the resource file is dictionary-compressed
				iFlagsAndNumberOfResources |= EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource; 
			}
			if (firstByteAfterUids & 0x20)
			{
				iFlagsAndNumberOfResources |=
					EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode;
			}
			dictionaryCompressionData.iStartOfResourceData =	LittleEndianTwoByteInteger(header,16+1+2,length);
			TUint8 temp[2];
			length = ReadL((iExtra->iFileSize)-2,temp,2);
	
			const TInt numberOfBitsOfResourceData = LittleEndianTwoByteInteger(temp,0,length);
			dictionaryCompressionData.iStartOfResourceIndex=
						dictionaryCompressionData.iStartOfResourceData+
						((numberOfBitsOfResourceData+7)/8);
			numberOfResources=(iExtra->iFileSize-dictionaryCompressionData.iStartOfResourceIndex)/2;
			dictionaryCompressionData.iStartOfDictionaryData=16+5;
			if ((numberOfResources>0) && 
				!(iFlagsAndNumberOfResources &
				EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode))
			{
				const TInt lengthOfBitArrayInBytes=(numberOfResources+7)/8;
				bitArrayOfResourcesContainingCompressedUnicode=	new Ptr8(lengthOfBitArrayInBytes);
				if(NULL==bitArrayOfResourcesContainingCompressedUnicode || NULL==bitArrayOfResourcesContainingCompressedUnicode->GetPtr())
				{
					std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
					throw CResourceFileException(errMsg);
				}
				bitArrayOfResourcesContainingCompressedUnicode->UpdateLength(lengthOfBitArrayInBytes);
				TUint8* asWritable = bitArrayOfResourcesContainingCompressedUnicode->GetPtr();
				ReadL(16+5,asWritable,lengthOfBitArrayInBytes);
				dictionaryCompressionData.iStartOfDictionaryData+=lengthOfBitArrayInBytes;
			}
			length = ReadL(dictionaryCompressionData.iStartOfResourceData-2,temp,2);
			const TInt numberOfBitsOfDictionaryData=LittleEndianTwoByteInteger(temp,0,length);
			dictionaryCompressionData.iStartOfDictionaryIndex=
						dictionaryCompressionData.iStartOfDictionaryData+
						((numberOfBitsOfDictionaryData+7)/8);
			dictionaryCompressionData.iNumberOfDictionaryEntries=
						(dictionaryCompressionData.iStartOfResourceData-
						dictionaryCompressionData.iStartOfDictionaryIndex)/2;
			//the bottom 3 bits of firstByteAfterUids stores the number of bits used for 
			//dictionary tokens as an offset from 3, e.g. if 2 is stored in these three bits 
			//then the number of bits per dictionary token would be 3+2=5 - this allows a 
			//range of 3-11 bits per dictionary token (the maximum number of dictionary 
			//tokens therefore ranging from 8-2048) - the spec currently only supports 5-9
			//bits per dictionary token, however
			dictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens=
						3 + (firstByteAfterUids & 0x07); 
			if ((numberOfResources>0) && 
				(iFlagsAndNumberOfResources &
				EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode))
			{
				Ptr16* nulldesc = new Ptr16(1);
				if(NULL==nulldesc || NULL==nulldesc->GetPtr())
				{
					std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
					throw CResourceFileException(errMsg);
				}
				*(nulldesc->GetPtr()) = 0;
				nulldesc->UpdateLength(0);
				
				bitArrayOfResourcesContainingCompressedUnicode=
				DictionaryDecompressedResourceDataL(
											0,
											iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),
											dictionaryCompressionData,
											nulldesc);
				if(NULL != nulldesc)
				{
					delete nulldesc;
				}
			}
		}
	}
	else
	{
		assert((iExtra->iFileSize + iExtra->iFileOffset) > 2);
		// This format of resource file is likely to be used for non-ROM resource files, 
		//so cache the resource-index (in iIndex) to minimize disk access.
		// Ignore the flags in non-dictionary-compressed resource files - they are to 
		//be used only by a dictionary-compressing program. 
		const TInt KMaximumNumberOfBytesCached=256;
		TUint8 cache[KMaximumNumberOfBytesCached];
		const TInt numberOfBytesCached=Min(iExtra->iFileSize,KMaximumNumberOfBytesCached);
		TInt len = ReadL(iExtra->iFileSize-numberOfBytesCached,cache,numberOfBytesCached);
		assert(len==numberOfBytesCached);
		const TInt positionOfStartOfIndex=
					((cache[numberOfBytesCached-1]<<8) | cache[numberOfBytesCached-2]);
		const TInt numberOfBytesOfIndex=iExtra->iFileSize-positionOfStartOfIndex;
		assert(numberOfBytesOfIndex%2==0);
		assert(numberOfBytesOfIndex>=0);
		const TInt numberOfBytesOfIndexStillToRetrieve=
					numberOfBytesOfIndex-numberOfBytesCached;
		if (numberOfBytesOfIndexStillToRetrieve<=0)
		{
			Ptr8* indexAsBinaryBuffer = new Ptr8(numberOfBytesOfIndex); 
			if(NULL==indexAsBinaryBuffer || NULL==indexAsBinaryBuffer->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}

			indexAsBinaryBuffer->UpdateLength(numberOfBytesOfIndex);
			BufCpy8(indexAsBinaryBuffer->GetPtr(), cache+(numberOfBytesCached - numberOfBytesOfIndex) , numberOfBytesOfIndex);

			iIndex = new Ptr16(numberOfBytesOfIndex/2); 
			if(NULL==iIndex || NULL==iIndex->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}

			MemCopy(CONST_CAST(TUint16*,(TUint16*)iIndex->GetPtr()),indexAsBinaryBuffer->GetPtr(),numberOfBytesOfIndex); 
			iIndex->UpdateLength(numberOfBytesOfIndex/2);

			if(NULL != indexAsBinaryBuffer)
			{
				delete indexAsBinaryBuffer;
			}
		}
		else
		{
			Ptr16* const index=new Ptr16(numberOfBytesOfIndex/2);
			if(NULL==index || NULL==index->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			index->UpdateLength(numberOfBytesOfIndex/2);
				
			Ptr8* indexAsWritableBinaryBuffer = new Ptr8(numberOfBytesOfIndex);
			if(NULL==indexAsWritableBinaryBuffer || NULL==indexAsWritableBinaryBuffer->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			indexAsWritableBinaryBuffer->UpdateLength(numberOfBytesOfIndexStillToRetrieve);
				
			ReadL(positionOfStartOfIndex,indexAsWritableBinaryBuffer->GetPtr(),
											numberOfBytesOfIndexStillToRetrieve);
			assert(indexAsWritableBinaryBuffer->GetLength()==numberOfBytesOfIndexStillToRetrieve);
			indexAsWritableBinaryBuffer->Append(cache, len);
			indexAsWritableBinaryBuffer->UpdateLength(len);
			assert(indexAsWritableBinaryBuffer->GetLength()==numberOfBytesOfIndex);
			assert(indexAsWritableBinaryBuffer->GetLength()==index->GetLength()*2);
			memcpy((TUint8*)index->GetPtr(), indexAsWritableBinaryBuffer->GetPtr() , numberOfBytesOfIndex);
	
			iIndex=index;
			if(NULL != indexAsWritableBinaryBuffer)
			{
				delete indexAsWritableBinaryBuffer;
			}
		}
			
		//"-1" because the last thing in the index (which is in fact the last thing in the 
		//file itself) is the position of the start of the index which is therefore not 
		//pointing to a resource
		numberOfResources=(numberOfBytesOfIndex/2) - 1; 
		if ((numberOfResources>0) && 
			(iFlagsAndNumberOfResources & EFlagPotentiallyContainsCompressedUnicode))
		{
			const TInt lengthOfBitArrayInBytes=(numberOfResources+7)/8;
			bitArrayOfResourcesContainingCompressedUnicode= new Ptr8(lengthOfBitArrayInBytes);
			if(NULL==bitArrayOfResourcesContainingCompressedUnicode || NULL==bitArrayOfResourcesContainingCompressedUnicode->GetPtr())
			{
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			bitArrayOfResourcesContainingCompressedUnicode->UpdateLength(lengthOfBitArrayInBytes);
			TUint8* bitArray = bitArrayOfResourcesContainingCompressedUnicode->GetPtr();
			//"16+1+2": 16 bytes of checked-UID + 1 byte of flags (these flags are for a 
			//dictionary-compressing program's use rather than directly for Bafl's use, 
			//so we ignore them) + 2 bytes containing the size of the largest resource when 
			//uncompressed
			ReadL(16+1+2,bitArray,lengthOfBitArrayInBytes); 
		}
	}
	assert((numberOfResources & EAllFlags)==0);
	assert((iFlagsAndNumberOfResources & ~EAllFlags)==0);
	iFlagsAndNumberOfResources |= (numberOfResources & ~EAllFlags);
	iExtra->iUidType = uidType;
	iExtra->iBitArrayOfResourcesContainingCompressedUnicode = bitArrayOfResourcesContainingCompressedUnicode;
	iExtra->iBitArrayOfResourcesContainingCompressedUnicode->SetLength(bitArrayOfResourcesContainingCompressedUnicode->GetLength());
	iExtra->iDictionaryCompressionData = dictionaryCompressionData;
	//iOffset is set by calling ConfirmSignatureL
	assert(iOffset==0);
}


/** @internalComponent
@pre OpenL() is called.
@leave KErrCorrupt The file is corrupted.
@leave KErrNoMemory There is not enough memory for the decompressed data.
Some other error codes are possible too.
*/
Ptr8* RResourceFileImpl::DictionaryDecompressedResourceDataL(
			TInt aResourceIndex,
			TUint aFlags,
			const SDictionaryCompressionData& aDictionaryCompressionData,
			const Ptr16* aIndex) const
{
	if (aFlags & EFlagDictionaryCompressed)
	{
		assert(iSizeOfLargestResourceWhenCompletelyUncompressed>0);
		Ptr8* const outputResourceData = new Ptr8(iSizeOfLargestResourceWhenCompletelyUncompressed);
		if(NULL==outputResourceData || NULL==outputResourceData->GetPtr())
		{
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}

		Ptr8* asWritable = outputResourceData;
		std::vector<RDictionaryCompressionBitStream> stackOfDictionaryCompressionBitStreams;
		AppendDictionaryCompressionBitStreamL(
						stackOfDictionaryCompressionBitStreams,
						aFlags,
						aDictionaryCompressionData,
						aDictionaryCompressionData.iStartOfResourceData,
						aDictionaryCompressionData.iStartOfResourceIndex,
						aResourceIndex);
		const TBool calypsoFileFormat=(aFlags & EFlagCalypsoFileFormat);
		while(1)
		{
			const TInt indexOfTopBitStream=stackOfDictionaryCompressionBitStreams.size()-1;
			assert(indexOfTopBitStream>=-1);
			if (indexOfTopBitStream<0)
			{
				break;
			}
			RDictionaryCompressionBitStream& dictionaryCompressionBitStream=
							stackOfDictionaryCompressionBitStreams[indexOfTopBitStream];

			while(1)
			{
				if (dictionaryCompressionBitStream.EndOfStreamL())
				{
					dictionaryCompressionBitStream.Close();
					stackOfDictionaryCompressionBitStreams.erase(indexOfTopBitStream);
					break;
				}
				const TInt indexOfDictionaryEntry=
								dictionaryCompressionBitStream.IndexOfDictionaryEntryL();
				if (indexOfDictionaryEntry<0)
				{
					dictionaryCompressionBitStream.ReadL(asWritable,calypsoFileFormat);
				}
				else
				{
					AppendDictionaryCompressionBitStreamL(
											stackOfDictionaryCompressionBitStreams,
											aFlags,
											aDictionaryCompressionData,
											aDictionaryCompressionData.iStartOfDictionaryData,
											aDictionaryCompressionData.iStartOfDictionaryIndex,
											indexOfDictionaryEntry);
					break;
				}
			}
		}
		stackOfDictionaryCompressionBitStreams.clear();
		return outputResourceData;
	}

	assert(aResourceIndex < aIndex->GetLength());

	const TInt positionOfResourceData=(*aIndex)[aResourceIndex];
	const TInt numberOfBytes=(*aIndex)[aResourceIndex+1]-positionOfResourceData;

	assert(numberOfBytes >= 0);
	Ptr8* const outputResourceData= new Ptr8(numberOfBytes);
	if(NULL==outputResourceData || NULL==outputResourceData->GetPtr())
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	TUint8* asWritable = outputResourceData->GetPtr();
	ReadL(aFlags,positionOfResourceData,asWritable,numberOfBytes);
	outputResourceData->UpdateLength(numberOfBytes);
		
	return outputResourceData;
		
}
	

void RResourceFileImpl::AppendDictionaryCompressionBitStreamL(
			std::vector<RDictionaryCompressionBitStream>& aStackOfDictionaryCompressionBitStreams,
			TUint aFlags,
			const SDictionaryCompressionData& aDictionaryCompressionData,
			TInt aStartOfBitData,
			TInt aStartOfIndex,
			TInt aIndexEntry) const
{
	const TBool isRomFile=(aFlags & static_cast<TUint>(EFlagIsRomFile));
	TUint8 temp[4];
	TInt length = 0;
	assert(aIndexEntry>=0);
	TInt offsetToFirstBit;
	TInt offsetOnePastLastBit;
	if (	aDictionaryCompressionData.iStartOfDictionaryIndex == aStartOfIndex
		&&	aDictionaryCompressionData.iCachedDictionaryIndex != 0)
	{
		assert(!isRomFile);
		// indices start at 1
		offsetToFirstBit = (aIndexEntry <= 0)
			?	0
			:	aDictionaryCompressionData.iCachedDictionaryIndex[aIndexEntry-1];
		offsetOnePastLastBit = aDictionaryCompressionData.iCachedDictionaryIndex[aIndexEntry];
	}
	else
	{
		TInt len = ReadL(aFlags,aStartOfIndex+((aIndexEntry-1)*2),temp,4);
		offsetToFirstBit=(aIndexEntry > 0) ? LittleEndianTwoByteInteger(temp,0,len) : 0;
		offsetOnePastLastBit=LittleEndianTwoByteInteger(temp,2,len);
	}
	TInt rsc_file_size = iExtra->iFileOffset + iExtra->iFileSize;
	TInt offset_first = offsetToFirstBit / 8 + iExtra->iFileOffset;
	assert(offset_first < rsc_file_size);
	TInt offset_last = offsetOnePastLastBit / 8 + iExtra->iFileOffset;
	assert(offset_last <= rsc_file_size);
	TUint8* buffer = NULL;
	TInt start_pos = 0;
	if (isRomFile)
	{
		TInt startOfBitData = aStartOfBitData + iExtra->iFileOffset;
		assert(startOfBitData < rsc_file_size);
		buffer = startOfBitData; 
	}
	else
	{
		const TInt offsetToByteContainingFirstBit=offsetToFirstBit/8;
		const TInt offsetToOnePastByteContainingLastBit=((offsetOnePastLastBit-1)/8)+1;
		const TInt numberOfBytesToLoad=
							offsetToOnePastByteContainingLastBit-offsetToByteContainingFirstBit;
		assert(numberOfBytesToLoad >= 0);
		buffer=new TUint8[numberOfBytesToLoad];
		if(NULL==buffer)
		{	
			std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
		}

		if( iExtra->iDictionaryCompressionData.iCachedResourceBuffer == 0)
		{
			iExtra->iDictionaryCompressionData.iCachedResourceBuffer=new TUint8[rsc_file_size]; // reserver buffer for whole file
			if(NULL==iExtra->iDictionaryCompressionData.iCachedResourceBuffer)
			{	
			 	delete buffer; // buffer deleted in RDictionaryCompressionBitStream::close
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			
			Ptr8* JKasWritable = new Ptr8(rsc_file_size); 
			if(NULL==JKasWritable || NULL == JKasWritable->GetPtr())
			{	
			 	delete buffer; // buffer deleted in RDictionaryCompressionBitStream::close
				std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
				throw CResourceFileException(errMsg);
			}
			JKasWritable->UpdateLength(rsc_file_size);

			try {
				length = ReadL(0,(TUint8*)JKasWritable->GetPtr(),	rsc_file_size);
			}
			catch(...)
			{
			 	delete buffer; // buffer deleted in RDictionaryCompressionBitStream::close
				std::string errMsg= "Failed : Error in Reading File.";
				throw CResourceFileException(errMsg);
			}

			BufCpy8(iExtra->iDictionaryCompressionData.iCachedResourceBuffer, JKasWritable->GetPtr(), length);
			if(NULL != JKasWritable)
			{
				delete JKasWritable;
			}
		}
		start_pos = aStartOfBitData + offsetToByteContainingFirstBit + iExtra->iFileOffset;
		assert(start_pos < rsc_file_size);
		assert((start_pos + numberOfBytesToLoad) <= rsc_file_size);
		const TInt numberOfBitsFromStartOfBitDataToFirstLoadedByte=
											offsetToByteContainingFirstBit*8;
		offsetToFirstBit-=numberOfBitsFromStartOfBitDataToFirstLoadedByte;
		offsetOnePastLastBit-=numberOfBitsFromStartOfBitDataToFirstLoadedByte;
	
		MemCopy( buffer, iExtra->iDictionaryCompressionData.iCachedResourceBuffer + start_pos,  numberOfBytesToLoad);
	}
	RDictionaryCompressionBitStream stream;
	stream.OpenL(
				aDictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens,
				offsetToFirstBit,
				offsetOnePastLastBit,
				!isRomFile,
				buffer);
	try {
		aStackOfDictionaryCompressionBitStreams.push_back(stream);
	}
	catch(...)
	{
	 	delete buffer; // buffer deleted in RDictionaryCompressionBitStream::close
		std::string errMsg= "Failed : Error in Reading File.";
		throw CResourceFileException(errMsg);
	}
	if (!isRomFile)
	{
		 delete buffer; // buffer deleted in RDictionaryCompressionBitStream::close
	}
}

