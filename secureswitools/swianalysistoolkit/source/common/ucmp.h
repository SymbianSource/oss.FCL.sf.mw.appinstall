/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Ported from ucmp.h
* Header for the Standard Compression Scheme for Unicode.
* This code is compiled only in the Unicode build.
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __UCMP_H__
#define __UCMP_H__ 1

#include "symbiantypes.h"
#include <iostream>


typedef std::istream Deserialiser;
typedef std::ostream Serialiser;

#define KMaxTInt 0x7FFFFFFF
/**
  * @internalComponent
  */

class TUnicodeCompressionState
	{
	public:
	TUnicodeCompressionState();
	void Reset();
	static TInt32 StaticWindowIndex(TUint16 aCode);
	static TInt32 DynamicWindowOffsetIndex(TUint16 aCode);
	static TUint32 DynamicWindowBase(TInt32 aOffsetIndex);
	static TBool EncodeAsIs(TUint16 aCode);

	enum TPanic
		{
		EUnhandledByte,			// expander code fails to handle all possible byte codes
		ENotUnicode,			// expander can't handle Unicode values outside range 0x0..0x10FFFF;
								// that is, 16-bit codes plus 32-bit codes that can be expressed using
								// 16-bit surrogates
		EOutputBufferOverflow	// output buffer is not big enough
		};

	static void Panic(TPanic aPanic);

	protected:

	enum
		{
		EStaticWindows = 8,
		EDynamicWindows = 8,
		ESpecialBases = 7
		};

	TBool iUnicodeMode;									// TRUE if in Unicode mode as opposed to single-byte mode
	TUint32 iActiveWindowBase;							// base of the active window - bases are 32-bit because they
														// can be set to the surrogate area, which represents codes
														// from 0x00010000 to 0x0010FFFF - planes 1-16 of ISO-10646.
	static const TUint32 iStaticWindow[EStaticWindows];	// bases of the static windows
	static const TUint32 iDynamicWindowDefault[EDynamicWindows];	// default bases of the dynamic windows
	static const TUint16 iSpecialBase[ESpecialBases];	// bases for window offsets F9..FF

	TUint32 iDynamicWindow[EDynamicWindows];			// bases of the dynamic windows
	TInt32 iUnicodeWords;									// Unicode words processed; read by compressor, written by expander
	TInt32 iMaxUnicodeWords;								// maximum number of Unicode words to read or write
	TInt32 iCompressedBytes;								// compressed bytes processed: read by expander, written by compressor
	TInt32 iMaxCompressedBytes;							// maximum number of compressed bytes to read or write
	};

/**
  * @internalComponent
  */

class MUnicodeSource
	{
	public:
	virtual TUint16 ReadUnicodeValue() = 0;
	};

/**
  * @internalComponent
 A class to read Unicode values directly from memory.
 */
class TMemoryUnicodeSource: public MUnicodeSource
	{
	public:
	inline TMemoryUnicodeSource(const TUint16* aPtr);
	inline TUint16 ReadUnicodeValue();

	private:
	const TUint16* iPtr;
	};

/**Deserialiser& Deserialiser::
  * @internalComponent
 A class to read Unicode values from a stream built on a memory object.
 */
class TMemoryStreamUnicodeSource: public MUnicodeSource
	{
	public:
	inline TMemoryStreamUnicodeSource(Deserialiser& aStream);
	inline TUint16 ReadUnicodeValue();

	private:
	Deserialiser& iStream;
	};

/**
  * @internalComponent
 */
class MUnicodeSink
	{
	public:
	virtual void WriteUnicodeValue(TUint16 aValue) = 0;
	};

/**
  * @internalComponent
 A class to write Unicode values directly to memory.
 */
class TMemoryUnicodeSink: public MUnicodeSink
	{
	public:
	inline TMemoryUnicodeSink(TUint16* aPtr);
	inline void WriteUnicodeValue(TUint16 aValue);

	private:
	TUint16* iPtr;
	};

/**
  * @internalComponent
 A class to write Unicode values to a stream built on a memory object.
 */
class TMemoryStreamUnicodeSink: public MUnicodeSink
	{
	public:
	inline TMemoryStreamUnicodeSink(Serialiser& aStream);
	inline void WriteUnicodeValue(TUint16 aValue);

	private:
	Serialiser& iStream;
	};

/**
  * @internalComponent
A class to hold functions to compress text using the Standard Compression Scheme for Unicode.

A note on error handling and leaving.

Although all the public functions except the constructor can leave, it is possible to guarantee success: that is,
guarantee that a call will not leave, and that compression will be completed. To do this, (i) supply a MUnicodeSource
object with a non-leaving ReadUnicodeValue function, such as a TMemoryUnicodeSource; (ii) write output to a
Serialiser with a non-leaving Write function, or to a buffer that you already know to be big enough, which can be
found out using CompressedSize.

This guarantee of success is particularly useful when compressing from one memory buffer to another.
*/
class TUnicodeCompressor: public TUnicodeCompressionState
	{
	public:
	TUnicodeCompressor();
	void Compress(Serialiser& aOutput,MUnicodeSource& aInput,
							TInt32 aMaxOutputBytes = KMaxTInt,TInt32 aMaxInputWords = KMaxTInt,
							TInt32* aOutputBytes = 0,TInt32* aInputWords = 0);
	void Compress(TUint8* aOutput,MUnicodeSource& aInput,
							TInt32 aMaxOutputBytes = KMaxTInt,TInt32 aMaxInputWords = KMaxTInt,
							TInt32* aOutputBytes = 0,TInt32* aInputWords = 0);
	TInt32 Flush(Serialiser& aOutput,TInt32 aMaxOutputBytes,TInt32& aOutputBytes);
	TInt32 Flush(TUint8* aOutput,TInt32 aMaxOutputBytes,TInt32& aOutputBytes);
	static TInt32 CompressedSize(MUnicodeSource& aInput,TInt32 aInputWords);

	private:

	 // A structure to store a character and its treatment code
	struct TAction
		{
		// Treatment codes: static and dynamic window numbers, plain ASCII or plain Unicode
		enum
			{
			EPlainUnicode = -2,	// character cannot be expressed as ASCII or using static or dynamic windows
			EPlainASCII = -1,	// character can be emitted as an ASCII code
			EFirstDynamic = 0,	// values 0..255 are for dynamic windows with offsets at these places in the offset table
			ELastDynamic = 255,
			EFirstStatic = 256,	// values 256..263 are for static windows 0..7
			ELastStatic = 263
			};

		inline TAction();
		TAction(TUint16 aCode);

		TUint16 iCode;		// Unicode value of the character
		TInt32 iTreatment;	// treatment code: see above
		};

	void DoCompress(Serialiser* aOutputStream,TUint8* aOutputPointer,MUnicodeSource* aInput,
					 TInt32 aMaxCompressedBytes,TInt32 aMaxUnicodeWords,
					 TInt32* aCompressedBytes,TInt32* aUnicodeWords);
	void FlushInputBuffer();
	void FlushOutputBuffer();
	void WriteRun();
	void WriteCharacter(const TAction& aAction);
	void WriteSCharacter( const TAction& aAction);
	void WriteUCharacter(TUint16 aCode);
	void WriteByte(TUint32 aByte);
	void WriteCharacterFromBuffer();
	void SelectTreatment(TInt32 aTreatment);

	enum
		{
		EMaxInputBufferSize = 4,
		EMaxOutputBufferSize = EMaxInputBufferSize * 3	// no Unicode character can be encoded as more than three bytes
		};
	TAction iInputBuffer[EMaxInputBufferSize];			// circular buffer; queue of Unicode characters to be processed
	TInt32 iInputBufferStart;								// position of first Unicode character to be processed
	TInt32 iInputBufferSize;								// characters in the input buffer
	TUint8 iOutputBuffer[EMaxOutputBufferSize];			// circular buffer; queue of compressed bytes to be output
	TInt32 iOutputBufferStart;							// position of first compressed byte to be output
	TInt32 iOutputBufferSize;								// characters in the output buffer
	TInt32 iDynamicWindowIndex;							// index of the current dynamic window
	Serialiser* iOutputStream;						// if non-null, output is to this stream
	TUint8* iOutputPointer;								// if non-null, output is to memory
	MUnicodeSource* iInput;								// input object
	};

/**
  * @internalComponent

A class to hold functions to expand text using the Standard Compression Scheme for Unicode.

A note on error handling and leaving.

Although all the puTUnicodeExpanderblic functions except the constructor can leave, it is possible to guarantee success: that is,
guarantee that a call will not leave, and that expansion will be completed. To do this, (i) supply a MUnicodeSink
object with a non-leaving WriteUnicodeValue function, such as a TMemoryUnicodeSink; (ii) read input from a Deserialiser
with a non-leaving Read function; (iii) supply a big enough buffer to write the ouput; you can find out how big by
calling ExpandedSize, using methods (i) and (ii) to guarantee success.

This guarantee of success is particularly useful when expanding from one memory buffer to another.
*/
class TUnicodeExpander: public TUnicodeCompressionState
	{
	public:
	TUnicodeExpander();
	void Expand(MUnicodeSink& aOutput,Deserialiser& aInput,
						  TInt32 aMaxOutputWords = KMaxTInt,TInt32 aMaxInputBytes = KMaxTInt,
						  TInt32* aOutputWords = 0,TInt32* aInputBytes = 0);
	void Expand(MUnicodeSink& aOutput,const TUint8* aInput,
						  TInt32 aMaxOutputWords = KMaxTInt,TInt32 aMaxInputBytes = KMaxTInt,
						  TInt32* aOutputWords = 0,TInt32* aInputBytes = 0);
	TInt32 Flush(MUnicodeSink& aOutput,TInt32 aMaxOutputWords,TInt32& aOutputWords);
	static TInt32 ExpandedSize(Deserialiser& aInput,TInt32 aInputBytes);
	static TInt32 ExpandedSize(const TUint8* aInput,TInt32 aInputBytes);

	private:
	void DoExpand(MUnicodeSink* aOutput,Deserialiser* aInputStream,const TUint8* aInputPointer,
				   TInt32 aMaxOutputWords,TInt32 aMaxInputBytes,
				   TInt32* aOutputWords,TInt32* aInputBytes);
	void HandleByte();
	void FlushOutputBuffer();
	TBool HandleSByte(TUint8 aByte);
	TBool HandleUByte(TUint8 aByte);
	TBool ReadByte(TUint8& aByte);
	TBool QuoteUnicode();
	TBool DefineWindow(TInt32 aIndex);
	TBool DefineExpansionWindow();
	void WriteChar(TUint16 aChar);
	void WriteChar32(TUint32 aChar);

	enum
		{
		EMaxInputBufferSize = 3,		 				// no Unicode character can be encoded as more than 3 bytes
		EMaxOutputBufferSize = 2						// no byte can be expanded into more than 2 Unicode characters
		};
	TUint8 iInputBuffer[EMaxInputBufferSize];			// buffer containing a group of compressed bytes representing
														// a single operation; when an input source ends in the
														// middle of an operation, this buffer enables the next
														// expansion to start in the correct state
	TInt32 iInputBufferStart;								// next read position in the input buffer
	TInt32 iInputBufferSize;								// bytes in the input buffer
	TUint16 iOutputBuffer[EMaxOutputBufferSize];		// circular buffer; queue of Unicode characters to be output
	TInt32 iOutputBufferStart;							// position of first Unicode character to be output
	TInt32 iOutputBufferSize;								// characters in the output buffer
	MUnicodeSink* iOutput;								// output object
	Deserialiser* iInputStream;							// if non-null, input is from this stream
	const TUint8* iInputPointer;						// if non-null, input is from memory
	};

// inline functions start here

inline TMemoryUnicodeSource::TMemoryUnicodeSource(const TUint16* aPtr):
	iPtr(aPtr)
	{
	}

inline TUint16 TMemoryUnicodeSource::ReadUnicodeValue()
	{
	return *iPtr++;
	}

inline TMemoryStreamUnicodeSource::TMemoryStreamUnicodeSource(Deserialiser& aStream):
	iStream(aStream)
	{
	}

inline TMemoryUnicodeSink::TMemoryUnicodeSink(TUint16* aPtr):
	iPtr(aPtr)
	{
	}

inline void TMemoryUnicodeSink::WriteUnicodeValue(TUint16 aValue)
	{
	*iPtr++ = aValue;
	}

inline TMemoryStreamUnicodeSink::TMemoryStreamUnicodeSink(Serialiser& aStream):
	iStream(aStream)
	{
	}

inline TUnicodeCompressor::TAction::TAction():
	iCode(0),
	iTreatment(EPlainUnicode)
	{
	}

#endif // __UCMP_H__

