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
#include "ucmp.h"


RResourceFileImpl::TExtra::TExtra():
	iBitArrayOfResourcesContainingCompressedUnicode(NULL),
	iFileOffset(0),
	iFileSize(0)
	{
	}


RResourceFileImpl::TExtra::~TExtra()
	{
	delete iBitArrayOfResourcesContainingCompressedUnicode;
	}


TInt32 RResourceFileImpl::TExtra::ContainsCompressedUnicode(const TInt& aRscIdx) const
	{
	assert(aRscIdx>=0);
	
	if (iBitArrayOfResourcesContainingCompressedUnicode==NULL)
		{
		return EFalse;
		}
	
	TInt index = aRscIdx/8;
	
	return (iBitArrayOfResourcesContainingCompressedUnicode)[index]&(1<<(aRscIdx%8));
	}


RResourceFileImpl::RResourceFileImpl() :
	iResourceContents(NULL),
	iSizeOfLargestResourceWhenCompletelyUncompressed(0),
	iIndex(0),
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


Ptr8* RResourceFileImpl::GetDecompressedResourceDataL(
													const TInt& aResourceIndex,			
													const TUint32& aFlags) 
	{
	const TInt positionOfResourceData= iIndex[aResourceIndex];
	const TInt numberOfBytes= iIndex[aResourceIndex+1]-positionOfResourceData;
	
	assert(numberOfBytes >= 0);
	
	Ptr8* outputResourceData=new Ptr8(numberOfBytes);
	ReadL(aFlags, positionOfResourceData, outputResourceData->GetPtr(), numberOfBytes);
	outputResourceData->UpdateLength(numberOfBytes);
	
	return outputResourceData;
	}


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
	assert(resourceIndex>=0);
	
	Ptr8* decompressedResourceData= 
		GetDecompressedResourceDataL(
								resourceIndex,
								iFlagsAndNumberOfResources & static_cast<TUint32>(EAllFlags));
	
	// Return the resource data if its not unicode compressed.
	if (!iExtra->ContainsCompressedUnicode(resourceIndex))
		{
		return decompressedResourceData;
		}
	
	// Get the decompressed unicode data.
	Ptr8* finalResourceData= DecompressUnicodeL(decompressedResourceData);	
	
	delete decompressedResourceData;	
	return finalResourceData;	
	}


Ptr8* RResourceFileImpl::DecompressUnicodeL(const Ptr8* aInputResourceData) const
	{
	const TInt numberOfBytesInInput= aInputResourceData->GetLength();
	assert(iSizeOfLargestResourceWhenCompletelyUncompressed>0);
							
	Ptr8* outputResourceData= new Ptr8(iSizeOfLargestResourceWhenCompletelyUncompressed);
	
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
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
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
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().

@see Offset()
@see AllocReadL()
@see AllocReadLC() 
@see ReadL()
@internalComponent
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
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
	
	return (resourceIndex >= 0) && (resourceIndex < numberOfResources);
	}


void RResourceFileImpl::ReadL(
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
	}


void RResourceFileImpl::ReadL(TInt aPos, TUint8* aData, const TInt& aLength) 
	{
	ReadL(iFlagsAndNumberOfResources & static_cast<TUint32>(EAllFlags),aPos,aData,aLength);
	}


TInt RResourceFileImpl::LittleEndianTwoByteInteger(
													TUint8* aBuffer,
													const TInt& aIndexOfFirstByte) const
	{
	return aBuffer[aIndexOfFirstByte] | (aBuffer[aIndexOfFirstByte+1]<<8);
	}


void RResourceFileImpl::ReadHeaderAndResourceIndexL()
	{
	
	// Unicode compressed RSC file will have 19 bytes header.
	const TUint8 kHeaderSize= 19;
	TUint8 header[kHeaderSize];
			
	sTUid uid;
	
	// Verify the header of the RSC file.
	if(iExtra->iFileSize >= kHeaderSize)
		{
		
		// Get the RSC header
		ReadL(0,0,header,kHeaderSize);
		// Get the first UID
		memcpy((TUint8*)&uid.iUid1,header,4);
		
		// First uid of unicode compressed RSC is "0x101f4a6b"
		TUint32 unicodeCompressedFirstUid = 0x101f4a6b;
		
		if (uid.iUid1 == unicodeCompressedFirstUid)
			{
			iFlagsAndNumberOfResources |= EFlagPotentiallyContainsCompressedUnicode;
			iSizeOfLargestResourceWhenCompletelyUncompressed=	LittleEndianTwoByteInteger(header,16+1);
			}
		else
			{
				if (iResourceContents->is_open())
						iResourceContents->close();
			std::string errMsg="Failed : Unsupported RSC file type";
			throw CResourceFileException(errMsg);
			}
		}
		
		TInt numberOfResources= 0;
		TUint8* bitArrayOfResourcesContainingCompressedUnicode= NULL;
		
		if (iFlagsAndNumberOfResources & EFlagPotentiallyContainsCompressedUnicode)
		{
			// Cache the resource-index (in iIndex) to minimize disk access.		
			const TInt KMaximumNumberOfBytesCached= 256;
			TUint8 cache[KMaximumNumberOfBytesCached];
			const TInt numberOfBytesCached=
				((iExtra->iFileSize>KMaximumNumberOfBytesCached) ? KMaximumNumberOfBytesCached : iExtra->iFileSize);
			
			ReadL(iExtra->iFileSize-numberOfBytesCached, cache, numberOfBytesCached);
			
			const TInt positionOfStartOfIndex=
				((cache[numberOfBytesCached-1]<<8) | cache[numberOfBytesCached-2]);
			const TInt numberOfBytesOfIndex=iExtra->iFileSize-positionOfStartOfIndex;
			
			assert(numberOfBytesOfIndex%2==0);
			assert(numberOfBytesOfIndex>=0);
			
			const TInt numberOfBytesOfIndexStillToRetrieve = numberOfBytesOfIndex-numberOfBytesCached;
			
			if (numberOfBytesOfIndexStillToRetrieve<=0)
				{
				iIndex= new TUint16[numberOfBytesOfIndex/2];
				memcpy(iIndex, cache+(numberOfBytesCached-numberOfBytesOfIndex), numberOfBytesOfIndex);
				}
			else
				{
				TUint16* index= new TUint16(numberOfBytesOfIndex/2);			
				ReadL(positionOfStartOfIndex, reinterpret_cast<TUint8*>(index), numberOfBytesOfIndexStillToRetrieve);
				memcpy((index+numberOfBytesOfIndexStillToRetrieve),cache,numberOfBytesCached); 		

				iIndex=index;			
				}

			//"-1" because the last thing in the index (which is in fact the last thing in the 
			//file itself) is the position of the start of the index which is therefore not 
			//pointing to a resource
			numberOfResources=(numberOfBytesOfIndex/2) - 1; 
			
			if (numberOfResources <= 0)
				{
				if (iResourceContents->is_open())
						iResourceContents->close();
				std::string errMsg="Failed : Invalid RSC file.";
				throw CResourceFileException(errMsg);
				}
				
			const TInt lengthOfBitArrayInBytes=(numberOfResources+7)/8;
			bitArrayOfResourcesContainingCompressedUnicode=
								new TUint8(lengthOfBitArrayInBytes);
			
			//"16+1+2": 16 bytes of checked-UID + 1 byte of flags (these flags are for a 
			//dictionary-compressing program's use rather than directly for Bafl's use, 
			//so we ignore them) + 2 bytes containing the size of the largest resource when 
			//uncompressed
			ReadL(16+1+2,bitArrayOfResourcesContainingCompressedUnicode,lengthOfBitArrayInBytes); 
		
		}
		
		assert((numberOfResources & EAllFlags)==0);
		assert((iFlagsAndNumberOfResources & ~EAllFlags)==0);
		
		iFlagsAndNumberOfResources |= (numberOfResources & ~EAllFlags);
		iExtra->iUid = uid;
		iExtra->iBitArrayOfResourcesContainingCompressedUnicode = bitArrayOfResourcesContainingCompressedUnicode;
			
	}
