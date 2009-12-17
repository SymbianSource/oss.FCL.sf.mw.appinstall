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
* base class for all types found in sis files. Defines all virtual functions.
* fieldroot <- field <- container <- [array, sequence, structure] <- most SIS types
* fieldroot <- field <- [string, compressed, blob]
* fieldroot <- numeric
* fieldroot <- header
*
*/


/**
 @file 
 @internalComponent
 @released
*/


#ifndef __FIELDROOT_H__
#define __FIELDROOT_H__

#include <assert.h>
#include "basefile.h"


#ifdef GENERATE_ERRORS
#ifndef _DEBUG
#error GENERATE_ERRORS requires _DEBUG
			// or change the code to rewrite TLV sizes in array and structure
#endif
#endif

class CSISFieldRoot

	{
public:
	typedef TUint64 TFieldSize;		// in BYTES

	typedef enum
		{
		ESISUndefined,
		ESISString,
		ESISArray,
		ESISCompressed,
		ESISVersion,
		ESISVersionRange, 
		ESISDate,
		ESISTime,
		ESISDateTime,
		ESISUid,
		ESISUnused1,
		ESISLanguage,
		ESISContents,
		ESISController,
		ESISInfo,
		ESISSupportedLanguages,
		ESISSupportedOptions,
		ESISPrerequisites,
		ESISDependency,
		ESISProperties,
		ESISProperty,
		ESISSignatures,
		ESISCertificateChain,
		ESISLogo,
		ESISFileDescription,
		ESISHash,
		ESISIf,
		ESISElseIf,
		ESISInstallBlock,
		ESISExpression,
		ESISData,
		ESISDataUnit,
		ESISFileData,
		ESISSupportedOption,
		ESISControllerChecksum,
		ESISDataChecksum,
		ESISSignature,
		ESISBlob,
		ESISSignatureAlgorithm,
		ESISSignatureCertificateChain,
		ESISDataIndex,
		ESISCapabilities,
			// insert new fields here
		ESISUnknown
		} TFieldType;

	typedef enum
		{
		EDbgDefault				= 0,
		EDbgDataChecksum		= 0x01,
		EDbgControllerChecksum	= 0x02,
		EDbgCompress			= 0x04,
		EDbgNoCompress			= 0x08,
		} TDbgFlag;

#ifdef GENERATE_ERRORS
	typedef enum
		{
		EBugDefault			= 0,
		EBugCRCError		= 0x0001,
		EBugInvalidLength	= 0x0002,
		EBugMissingField  	= 0x0004,
		EBugUnexpectedField	= 0x0008,
		EBugBigEndian	  	= 0x0010,
		EBugDuffFieldType 	= 0x0020,
		EBugInvalidValues  	= 0x0040,
		EBugHashError	 	= 0x0080,
		EBugNegativeLength	= 0x0100,
		EBugInsaneString	= 0x0200,
		EBugInsaneBlob		= 0x0400,
		EBugArrayCount		= 0x0800,
		EBug32As64			= 0x1000,
		EBugUnknownField	= 0x2000,
		EBugEmptyCaps       = 0x4000,
		EBugUnknownData		= 0x8000,
		} TBug;
#endif // GENERATE_ERRORS

	typedef enum
		{
		EOptDefault				= 0,
		EOptIsUnicode			= 1<<0,
		EOptIsDistributable		= 1<<1,
		EOptIsCompareToMajor	= 1<<2,
		EOptNoCompress			= 1<<3,
		EOptShutdownApps		= 1<<4,
		EOptNonRemovable		= 1<<5,
		EOptROMUpgrade			= 1<<6,
		} THeadOpt;


public:
	CSISFieldRoot ();
	virtual ~CSISFieldRoot ();

public:
	/**
	 * Internalize the class
	 * @param aFile File stream from where data needs to be read.
	 * @param aContainerSize size of the content to be read.
	 * @param aArrayType Type of the array 
	 */
	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const TFieldType aArrayType = ESISUndefined) = 0;
	/**
	 * Skip the file reading for this field. Read pointer will be moved to the
	 * next field to be read.
	 * @param aFile stream for which the data read should be skipped.
	 * @param aContainerSize size of the data to be skipped. 
	 */
	virtual void Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const = 0;
	/**
	 * Externalize the class
	 * @param aFile File stream to which the data needs to be written.
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const = 0;
	  
	/**
	 * This function verifies the structure. If verification of object fails, throw an exception
	 * aLanguages >= 1
	 * @param aLanguages - language
	 */ 
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * if object not acceptable, modify it. Assumes, once modified, will not need modifying again
	 */
	virtual void MakeNeat ();
	/**
	 * @return 	if true, don't bother writing this record to disc
	 * 			if false, when reading, it'd better be there
	 */
	virtual bool WasteOfSpace () const;
	/**
	 * Retrieves the size of the structure excluding the header.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual TFieldSize ByteCount (const bool aInsideArray) const = 0;
	/**
	 * Retrieves the in-stream size of data in BYTES of the structure with the header.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual TFieldSize ByteCountWithHeader (const bool aInsideArray) const;	
	/**
	 * Sets the size of the structure. 
	 * either 	a) set variable size field, or
	 *			b) ensure fixed size field is expected size
	 * @param size - size of the structure. 
	 */
	virtual void SetByteCount (const TFieldSize aSize);
	/**
	 * Dump the entire content in hex format into the stream
	 */
	virtual void Dump (std::ostream& aStream, const int aLevel) const = 0;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	virtual void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Calculates CRC of the content
	 * @param aCRC CRC value of the content
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */ 
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const = 0 ;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	virtual void SkipOldWriteNew (TSISStream& aFile) const;
	/**
	 * stream offset before the header
	 */
	virtual TSISStream::pos_type PreHeaderPos () const;
	/**
	 * stream offset after the header
	 */
	virtual TSISStream::pos_type PostHeaderPos () const;
	/**
	 * Calculates CRC of the content
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 * @return CRC value of the content
	 */ 
	TCRC Crc (const bool aIsArrayElement = false) const;
	std::string NoteIfOptional () const;

	/**
	 * Set Debug options
	 */
	static void SetDebugOption (const TDbgFlag aDbgFlag);
	/**
	 * Check if a particular debug option is set or not.
	 */
	static bool IsDebugOptionSet (const TDbgFlag aDbgFlag);
	/**
	 * Set file header option
	 */
	static void SetHeaderOption (const THeadOpt aHeaderOption);
	/**
	 * Check if a particular file header option is set or not.
	 */
	static bool IsHeaderOptionSet (const THeadOpt aHeaderOption);

#ifdef GENERATE_ERRORS
	static void SetBug (const TBug aBug);
	static bool IsBugSet (const TBug aBug);
	static bool IsAnyBugSet ();
	static void SetBugStart (const unsigned aStart);
	static void SetBugRepeat (const unsigned aRepeat);
	static bool IsBugToBeCreated (const TBug aBug);
	static void AddRawDataValue (TSISStream& aFile,const unsigned aLength);
#endif // GENERATE_ERRORS

	/**
	 * Initialize the instance.
	 */
	static void InitInstance ();

protected:
	// calculates the CRC of the given data
	static void DoTheCrc (TCRC& aCRC, const TUint8* aData, const unsigned aLen);
	
	// calculates CRC on the given data and the padding bytes that will be required
	static void DoPaddedCrc(TCRC& aCRC, const TUint8* aData, const unsigned aLen);
	
	// calculate the CRC of just the padding for the given data length
	static void PaddingCrc(TCRC& aCRC, const unsigned aLength);

	static unsigned SizeOfFieldType ();

protected:
	static TDbgFlag	iDbgFlag;
	static THeadOpt	iOptions;

#ifdef GENERATE_ERRORS
	static TBug		iBug;
	static unsigned iBugStart;
	static unsigned iBugRepeat;
	static unsigned iBugRepeatCountdown;
#endif // GENERATE_ERRORS
	};


inline void CSISFieldRoot::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	(void)aStream;
	(void)aVerbose;
	}

inline void CSISFieldRoot::InitInstance ()
	{
	srand (1);
		// Please consult the VC6 documentation to understand the supplied parameter value.
	}


inline void CSISFieldRoot::SetDebugOption (const TDbgFlag aDbgFlag)
	{
	iDbgFlag = static_cast <TDbgFlag> (static_cast <unsigned int> (iDbgFlag) + static_cast <unsigned int> (aDbgFlag));
	}

inline bool CSISFieldRoot::IsDebugOptionSet (const TDbgFlag aDbgFlag)
	{
	return (static_cast <unsigned int> (iDbgFlag) & static_cast <unsigned int> (aDbgFlag)) != 0;
	}

inline void CSISFieldRoot::SetHeaderOption (const THeadOpt aOptions)
	{
	iOptions = static_cast <THeadOpt> (static_cast <unsigned int> (iOptions) + static_cast <unsigned int> (aOptions));
	}

inline bool CSISFieldRoot::IsHeaderOptionSet (const THeadOpt aOptions)
	{
	return (static_cast <unsigned int> (iOptions) & static_cast <unsigned int> (aOptions)) != 0;
	}

#ifdef GENERATE_ERRORS
inline void CSISFieldRoot::SetBug (const TBug aBug)
	{
	iBug = static_cast <TBug> (static_cast <unsigned int> (iBug) + static_cast <unsigned int> (aBug));
	}

inline bool CSISFieldRoot::IsBugSet (const TBug aBug)
	{
	return (static_cast <unsigned int> (iBug) & static_cast <unsigned int> (aBug)) != 0;
	}

inline bool CSISFieldRoot::IsAnyBugSet ()
	{
	return iBug != EBugDefault;
	}

inline void CSISFieldRoot::SetBugStart (const unsigned aStart)
	{
	iBugStart = aStart;
	}

inline void CSISFieldRoot::SetBugRepeat (const unsigned aRepeat)
	{
	iBugRepeat = aRepeat;
	iBugRepeatCountdown = aRepeat;
	}

inline void CSISFieldRoot::CreateDefects ()
	{
	}
#endif // GENERATE_ERRORS

inline std::string CSISFieldRoot::NoteIfOptional () const
	{
	return WasteOfSpace () ? " (Dropped)" : std::string ();
	}


inline void CSISFieldRoot::Verify (const TUint32 aLanguages) const
	{ 
	}


inline bool CSISFieldRoot::WasteOfSpace () const
	{ 
	return false; 
	}


inline CSISFieldRoot::TFieldSize CSISFieldRoot::ByteCountWithHeader (const bool aInsideArray) const
	{ 
	return ByteCount (aInsideArray); 
	}
	
		
inline void CSISFieldRoot::SetByteCount (const TFieldSize aSize)
	{
	assert (aSize == ByteCount (false));
	}


inline void CSISFieldRoot::MakeNeat ()
	{ 
	}


inline std::string CSISFieldRoot::Name () const
	{
	return std::string ();
	}


inline unsigned CSISFieldRoot::SizeOfFieldType ()
	{
	return sizeof (TUint32);
	}

inline void CSISFieldRoot::SkipOldWriteNew (TSISStream& aFile) const
	{
	}

inline TSISStream::pos_type CSISFieldRoot::PreHeaderPos () const
	{
	return 0;
	}

inline TSISStream::pos_type CSISFieldRoot::PostHeaderPos () const
	{
	return 0;
	}

TSISStream& operator >> (TSISStream& aFile, CSISFieldRoot::TFieldType& aType);
TSISStream& operator << (TSISStream& aFile, const CSISFieldRoot::TFieldType aType);

#endif // __FIELDROOT_H__

