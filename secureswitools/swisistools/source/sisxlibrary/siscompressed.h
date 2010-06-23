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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISCOMPRESSED_H__
#define __SISCOMPRESSED_H__

#include "field.h"
#include "symbiantypes.h"
#include <string>



template <class T> class CSISCompressed : public CSISField <CSISFieldRoot::ESISCompressed>

	{
public:
	typedef enum {
		ECompressNone,
		ECompressDeflate
	} TCompressionAlgorithm;

private:
	void Dispose () const;
	bool Compress () const;
	bool Decompress ();
	unsigned int DifferenceInSize () const;

public:
	CSISCompressed ();
	CSISCompressed (T& aData);
	CSISCompressed (const CSISCompressed& aInitialiser);
	virtual ~CSISCompressed ();

	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  
	virtual CSISFieldRoot::TFieldSize ByteCount (const bool aInsideArray) const;
	virtual void SetByteCount (const TFieldSize size);
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	virtual bool WasteOfSpace () const;
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	virtual std::string Name () const;
	virtual void Verify (const TUint32 aLanguages) const;
	virtual void MakeNeat ();
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	const T& Content () const;
	T& Content ();
	TUint64 CompressedSize () const;
	TUint64 UncompressedSize () const;
	void SetNoCompression ();

private:
	T					iData;
	mutable TUint8*		iCompressedData;
	mutable TFieldSize	iCompressedDataSize;
	mutable TUint64		iUncompressedSize;
	mutable TUint32		iAlgorithm;					// may be changed if compression not efficient
	};



bool CompressData (	const TUint8* aInputData, const CSISFieldRoot::TFieldSize aInputSize,
					TUint8*& aOutputData, CSISFieldRoot::TFieldSize& aOutputSize,
					const bool aCanAbandon);
void ExpandStream (const TUint8* aInputData, const CSISFieldRoot::TFieldSize aInputSize, TSISStream& stream);






template <class T> inline CSISCompressed <T>::CSISCompressed () :
		iCompressedData (NULL), 
		iCompressedDataSize (0),
		iUncompressedSize (0),
		iAlgorithm (IsDebugOptionSet (CSISFieldRoot::EDbgNoCompress) ? ECompressNone : ECompressDeflate)
	{ 
	}

template <class T> inline CSISCompressed <T>::CSISCompressed (T& aData) :
		iData(aData),
		iCompressedData (NULL), 
		iCompressedDataSize (0),
		iUncompressedSize (0),
		iAlgorithm (IsDebugOptionSet (CSISFieldRoot::EDbgNoCompress) ? ECompressNone : ECompressDeflate)
	{ 
	}


template <class T> CSISCompressed <T>::CSISCompressed (const CSISCompressed& aInitialiser) :
		CSISField <CSISFieldRoot::ESISCompressed> (aInitialiser),
		iAlgorithm (aInitialiser.iAlgorithm),
		iData (aInitialiser.iData)								
	{
	// this assumes, in general, compressed data does not change.
	iCompressedDataSize = aInitialiser.iCompressedDataSize;
	if (iCompressedDataSize != 0)
		{
		try
			{
			iCompressedData = new TUint8 [iCompressedDataSize];
			memcpy (iCompressedData, aInitialiser.iCompressedData, iCompressedDataSize);
			return;
			}
		catch (...)
			{
			}
		}
	iCompressedDataSize = 0;
	iCompressedData = NULL;
	iUncompressedSize = aInitialiser.iUncompressedSize;
	}


template <class T> inline unsigned int CSISCompressed <T>::DifferenceInSize () const
	{
	return sizeof (TUint32) + sizeof (TUint64);
	}


template <class T> inline const T& CSISCompressed <T>::Content () const
	{
	return iData;
	}


template <class T> inline T& CSISCompressed <T>::Content ()
	{
	Dispose ();
	return iData;
	}


template <class T> void CSISCompressed <T>::Dispose () const
	{
	if (iCompressedData != NULL)
		{
		assert (iCompressedDataSize != 0);
		iCompressedDataSize = 0;
		delete [] iCompressedData;
		iCompressedData = NULL;
		}
	}

template <class T> bool CSISCompressed <T>::Compress () const
	{
	if (IsDebugOptionSet (CSISFieldRoot::EDbgNoCompress) || IsHeaderOptionSet (CSISFieldRoot::EOptNoCompress))
		{
		iAlgorithm = ECompressNone;
		}
	if (iAlgorithm == ECompressNone)
		{
		iUncompressedSize = iData.ByteCountWithHeader (false);
		return false;
		}
	if (iCompressedData == NULL)
		{
		assert (iCompressedDataSize == 0);
		TSISStream stream;
		iData.Write (stream, false);
		stream.seek (0);
		iUncompressedSize = stream.length ();
		if (	(iUncompressedSize == 0) ||
				! CompressData (stream.data (), stream.length (), 
								iCompressedData, iCompressedDataSize,
								! IsDebugOptionSet (CSISFieldRoot::EDbgCompress)))
			{
			iAlgorithm = ECompressNone;
			iCompressedDataSize = 0;
			iUncompressedSize = iData.ByteCountWithHeader (false);
			return false;
			}
		}
	return true;
	}

template <class T> bool CSISCompressed <T>::Decompress ()
	{
	if (iAlgorithm == ECompressNone) 
		{
		assert (iUncompressedSize == iData.ByteCountWithHeader (false));
		return false;
		}
	assert (iCompressedData != NULL);
	assert (iCompressedDataSize != 0);
	TSISStream stream;
	ExpandStream (iCompressedData, iCompressedDataSize, stream);
	assert (iUncompressedSize == stream.length ());
	stream.seek (0);
	iData.Read (stream, stream.length ());
	return true;
	}

template <class T> inline CSISCompressed <T>::~CSISCompressed ()
	{
	delete [] iCompressedData;
	}

template <class T> inline bool CSISCompressed <T>::WasteOfSpace () const
	{
	return ! Required () && iData.WasteOfSpace ();
	}

template <class T> void CSISCompressed <T>::Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	Dispose ();
	CSISHeader header (aArrayType);
	header.Read (aFile, aContainerSize, aArrayType);
	CSISException::ThrowIf (header.DataType () != CSISFieldRoot::ESISCompressed,
							CSISException::ECompress,
							"compressed data expected");
	iCompressedDataSize = header.DataSize () - DifferenceInSize ();
	aFile >> iAlgorithm;
	aFile >> iUncompressedSize;
	if (iCompressedDataSize != 0)
		{
		if (iAlgorithm == ECompressNone) 
			{
			iData.Read (aFile, iCompressedDataSize, CSISFieldRoot::ESISUndefined);
			iCompressedDataSize = 0;
			}
		else
			{
			iCompressedData = new TUint8 [iCompressedDataSize];
			try
				{
				aFile.read (iCompressedData, iCompressedDataSize);
				Decompress ();
				}
			catch (...)
				{
				Dispose ();
				throw CSISException (CSISException::ECompress, L"inflation");
				}
			Dispose ();
			}
		}
	} 

template <class T> void CSISCompressed <T>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	if (iAlgorithm == ECompressNone)
		{
		iCompressedDataSize = iData.ByteCountWithHeader (false) + DifferenceInSize ();
		CSISHeader (CSISFieldRoot::ESISCompressed, ByteCount (false)).Write (aFile, aIsArrayElement);
		aFile << iAlgorithm;
		aFile << iUncompressedSize;
		iData.Write (aFile, false);
		}
	else
		{
		if (! Compress ())
			{
			Write (aFile, aIsArrayElement);
			}
		else
			{
			CSISHeader (CSISFieldRoot::ESISCompressed, ByteCount (false)).Write (aFile, aIsArrayElement);
			aFile << iAlgorithm;
			aFile << iUncompressedSize;
			aFile.write (iCompressedData, iCompressedDataSize);
			}
		}
	} 

template <class T> inline CSISFieldRoot::TFieldSize CSISCompressed <T>::ByteCount (const bool aInsideArray) const
	{
	if (Compress ()) 
		{
		return DifferenceInSize () + iCompressedDataSize;
		}
	return DifferenceInSize () + iData.ByteCountWithHeader (false);
	}

template <class T> void CSISCompressed <T>::SetByteCount (const TFieldSize aSize)
	{
	if (iAlgorithm == ECompressNone) 
		{
		iData.SetByteCount (aSize - DifferenceInSize ());
		}
	else
	if (aSize - sizeof (TUint32) != iCompressedDataSize)
		{
		iCompressedDataSize = aSize - DifferenceInSize ();
		Dispose ();
		}
	}

template <class T> void CSISCompressed <T>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	CSISHeader (CSISFieldRoot::ESISCompressed, ByteCount (false)).CalculateCrc (aCRC, aIsArrayElement); 
	CSISUInt32(iAlgorithm).CalculateCrc (aCRC, false);
	CSISUInt64 (iUncompressedSize).CalculateCrc (aCRC, false);
	if (Compress ()) 
		{
		CSISFieldRoot::DoPaddedCrc (aCRC, iCompressedData, iCompressedDataSize);
		}
	else 
		{
		iData.CalculateCrc (aCRC, false);
		}
	}


template <class T> void CSISCompressed <T>::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream	<< std::string ((iAlgorithm == ECompressNone) ? "off " : "on ");
	aStream	<< std::string ("{") << iData.Name () << std::string (" into ") << iCompressedDataSize; 
	aStream << std::string (" from ") << iUncompressedSize << std::string ("}") << std::endl;
	aStream << std::string (aLevel + 2, ' ');
	iData.Dump (aStream, aLevel + 2);
	}


template <class T> inline std::string CSISCompressed <T>::Name () const
	{
	return "Compressed";
	}


template <class T> TUint64 CSISCompressed <T>::CompressedSize () const
	{
	Compress ();
		// do compress first in case it fails and switches algorithm to ECompressNone
	if (iAlgorithm == ECompressNone)
		{
		return UncompressedSize ();
		}
	return iCompressedDataSize;
	}


template <class T> inline TUint64 CSISCompressed <T>::UncompressedSize () const
	{
	return iUncompressedSize;
	}


template <class T> inline void CSISCompressed <T>::Verify (const TUint32 aLanguages) const
	{
	iData.Verify (aLanguages);
	}


template <class T> inline void CSISCompressed <T>::MakeNeat ()
	{
	iData.MakeNeat ();
	}

#ifdef GENERATE_ERRORS
template <class T> inline void CSISCompressed <T>::CreateDefects ()
	{
	iData.CreateDefects ();
	}
#endif // GENERATE_ERRORS


template <class T> inline void CSISCompressed <T>::SetNoCompression ()
	{
	if (iAlgorithm != ECompressNone)
		{
		iAlgorithm = ECompressNone;
		Dispose ();
		}
	}

#endif // __SISCOMPRESSED_H__

