/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef T_CATALOGSUTILS_H
#define T_CATALOGSUTILS_H

#include <badesca.h>
#include <s32strm.h>
#include <s32mem.h>
#include <f32file.h>

class RFs;
class RWriteStream;
class RReadStream;
class MCatalogsBaseMessage;
class RFile;

/**
 * TVersion class has size restrictions and is not suitable to represent SIS
 * file versions. TCatalogsVersion has higher limits for the version values.
 */
class TCatalogsVersion
    {
public:
    
    /**
     * Converts a descriptor to TCatalogsVersion
     *
     * @leave KErrArgument if at least major number is not in aVersion
     * @leave KErrGeneral if aVersion doesn't contain valid characters
     */
    static void ConvertL( TCatalogsVersion& aTarget, const TDesC& aVersion );
    
    /**
     * Converts a version string to a descriptor
     */
    static HBufC* ConvertLC( const TCatalogsVersion& aSource );
        
    /**
     * Initializes version to 0.0.0
     */
    TCatalogsVersion();

    /**
     * Initializes version
     * 
     * @param aMajor Major version number
     * @param aMinor Minor version number
     * @param aBuild Build version number
     * @return 
     */
    TCatalogsVersion( TUint16 aMajor, TUint16 aMinor, TUint32 aBuild );
    
    /**
     * Comparison operator
     *
     * @aVersion Version to compare with
     * @param ETrue if the versions are equal
     */
    TBool operator==( const TCatalogsVersion& aVersion ) const;
    
    /**
     * Greater than -operator
     *
     * @param aVersion Version to compare with
     * @return ETrue if the current version is greater than
     * the version given as a parameter
     */
    TBool operator>( const TCatalogsVersion& aVersion ) const;

    /**
     * Equar or greater than -operator
     *
     * @param aVersion Version to compare with
     * @return ETrue if the current version is equal with or greater than
     * the version given as a parameter
     */
    TBool operator>=( const TCatalogsVersion& aVersion ) const;
    
    
    inline TBool operator!=( const TCatalogsVersion& aVersion ) const 
        {
        return !operator==( aVersion );
        }
    
public:
    TUint16 iMajor;
    TUint16 iMinor;
    TUint32 iBuild;
    };


#ifndef NCD_STORAGE_TESTING

/**
 * Utility class for reading data from MCatalogsBaseMessages
 *
 * Use CleanupClosePushL when pushing this object to cleanupstack
 *
 * Basic usage:
 * 
 * @code
 * void FunctionThatUsesAMessage( MCatalogsBaseMessage& aMessage )
 *    {
 *    ...
 *    RCatalogsMessageReader reader;
 *    reader.OpenLC( aMessage );
 *    TInt32 intFromMessage = reader().ReadInt32();
 *    ...
 *    CleanupStack::PopAndDestroy( &reader );
 *    ...
 * @endcode
 */
class RCatalogsMessageReader
    {
public:

    /**
     * Constructor
     */
    RCatalogsMessageReader();
    
    /**
     * Opens the stream for reading. 
     *
     * @param aMessage Message to read from
     * @note This object must be Closed when it's not used anymore
     */
    void OpenL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Opens the stream for reading and pushes the reader to cleanupstack
     *
     * @param aMessage Message to read from
     */
    void OpenLC( MCatalogsBaseMessage& aMessage );
    
    /**
     * Closes the object
     */
    void Close();    
    
    /**
     * Destructor
     */
    virtual ~RCatalogsMessageReader();
        
    /**
     * Stream getter
     */
    RReadStream& operator()()
        {
        return iStream;
        }
    
protected:

    
    RCatalogsMessageReader( const RCatalogsMessageReader& );
    RCatalogsMessageReader& operator=( const RCatalogsMessageReader& );
    
private:
    
    RBuf8 iBuf;
    RDesReadStream iStream;
        
    };
    
#endif

/**
 * Utility class for writing to a buffer
 *
 * Usage:
 *
 * @code
 * RCatalogsBufferWriter writer;
 * writer.OpenLC();
 * writer().WriteInt32L( 64 );
 * ExternalizeDesL( myDescriptor, writer() );
 * ...
 * TPtr8 ptr( writer->PtrL() ); 
 * // use the pointer 
 * ...
 * CleanupStack::PopAndDestroy( &writer );
 * @endcode
 */
class RCatalogsBufferWriter
    {
public:

    /**
     * Constructor
     */
    RCatalogsBufferWriter();
    
    /**
     * Opens the stream for writing. 
     *     
     * @note This object must be Closed when it's not used anymore
     */
    void OpenL();
    
    /**
     * Opens the stream for writing and pushes the writer to cleanupstack
     *
     */
    void OpenLC();
    
    /**
     * Closes the object
     */
    void Close();    
    
    /**
     * Destructor
     */
    virtual ~RCatalogsBufferWriter();
        
    /**
     * Stream getter
     */
    RWriteStream& operator()() 
        {
        return iStream;
        }
    

    /**
     * Written buffer
     */
    TPtr8 PtrL()
        {        
        iStream.CommitL();
        return iBuf->Ptr( 0 );
        }
    
protected:

    
    RCatalogsBufferWriter( const RCatalogsBufferWriter& );
    RCatalogsBufferWriter& operator=( const RCatalogsBufferWriter& );
    
private:
    
    CBufBase* iBuf;
    RBufWriteStream iStream;
        
    };

/**
 * Copies a descriptor to a heap descriptor.
 * 
 * @param aDes Destination heap descriptor.
 * @param aSource Source descriptor.
 */
void AssignDesL( HBufC8*& aDes, const TDesC8& aSource );

/**
 * Copies a descriptor to a heap descriptor.
 * 
 * @param aDes Destination heap descriptor.
 * @param aSource Source descriptor.
 */
void AssignDesL( HBufC16*& aDes, const TDesC16& aSource );


/**
 * Copies a UTF-8 descriptor to a unicode heap descriptor.
 * 
 * @param aDes Destination heap descriptor.
 * @param aSource Source descriptor.
 */
void AssignDesL( HBufC16*& aDes, const TDesC8& aSource );

/**
 * Converts a utf8 descriptor into unicode.
 * 
 * @param aUtfText Source data
 * @return HBufC16* Converted data
 */
HBufC16* ConvertUtf8ToUnicodeL( const TDesC8& aUtfText );

/**
 * Converts a unicode descriptor into utf8.
 * 
 * @param aUnicodeText Source data
 * @return HBufC8* Converted data
 */
HBufC8* ConvertUnicodeToUtf8L( const TDesC16& aUnicodeText );

/**
 * Converts a 16-bit des to a 8-bit des by TRUNCATING(!) data.
 * Creates readable text: _L16("abc") -> _L8("abc")
 * 
 * @param aDes Source data
 * @return HBufC8* Dynamically allocated buffer containing the converted data.
 */
HBufC8* Des16ToDes8LC( const TDesC16& aDes );

/**
 * Converts a 16-bit des to a 8-bit des by TRUNCATING(!) data.
 * Creates readable text: _L16("abc") -> _L8("abc")
 * 
 * @param aDes Source data
 * @return HBufC8* Dynamically allocated buffer containing the converted data.
 */
HBufC8* Des16ToDes8L( const TDesC16& aDes );

/**
 * Converts a 8-bit des to a 16-bit des by adding empty 8 bits to every byte.
 * Creates readable text: _L8("abc") -> _L16("abc")
 * 
 * @param aDes Source data
 * @return HBufC16* Dynamically allocated buffer containing the converted data.
 */
HBufC16* Des8ToDes16LC( const TDesC8& aDes );

/**
 * Converts a 8-bit des to a 16-bit des by adding empty 8 bits to every byte.
 * Creates readable text: _L8("abc") -> _L16("abc")
 * 
 * @param aDes Source data
 * @return HBufC16* Dynamically allocated buffer containing the converted data.
 */
HBufC16* Des8ToDes16L( const TDesC8& aDes );

/**
 * Converts descriptor containing decimal number to an integer.
 * 
 * @param aDes Descriptor representation of a decimal.
 * @return TInt Converted integer.
 */
TInt DesDecToIntL( const TDesC& aDes );

/**
 * Converts descriptor containing decimal number to an integer.
 * 
 * @param aDes Descriptor containing the decimal number.
 * @param aValue Integer where the outcome is put.
 * @return Symbian error-code as a conversion outcome.
 */
TInt DesDecToInt( const TDesC8& aDes, TInt& aValue );

/**
 * Converts descriptor containing decimal number to an integer.
 * 
 * @param aDes Descriptor containing the decimal number.
 * @param aValue Integer where the outcome is put.
 * @return Symbian error-code as a conversion outcome.
 */
TInt DesDecToInt( const TDesC16& aDes, TInt& aValue );

/**
 * Converts descriptor containing hexadecimal number to an integer.
 * Accepts "ABCD" and "0xABCD" format numbers.
 * Leaves if corrupt parameter.
 * @param aDes Descriptor representation of a hexadecimal.
 * @return TInt Converted integer.
 */
TInt DesHexToIntL( const TDesC& aDes );

/**
 * Converts the raw contents of a 8-bit descriptor into an integer.
 * 
 * @param aDes Descriptor containing a raw number.
 * @return Int Converted number
 */
TInt Des8ToInt( const TDesC8& aDes );

/**
 * Converts the raw contents of a 8-bit descriptor into an unsigned integer.
 * 
 * @param aDes Descriptor containing a raw number.
 * @return Uint Converted number
 */
TUint Des8ToUint( const TDesC8& aDes );

/**
 * Converts an integer into a 8-bit descriptor.
 * NOTE: the number will not be in text format.
 * 
 * @param aInt Number to convert
 * @return Dynamically allocated buffer containing the data
 */
HBufC8* IntToDes8LC( TInt aInt );

/**
 * Converts an integer into a 8-bit descriptor.
 * NOTE: the number will not be in text format.
 * 
 * @param aInt Number to convert
 * @return Dynamically allocated buffer containing the data
 */
HBufC8* IntToDes8L( TInt aInt );

/**
 * Converts an unsigned integer into a 8-bit descriptor.
 * NOTE: the number will not be in text format.
 * 
 * @param aUint Number to convert
 * @return Dynamically allocated buffer containing the data
 */
HBufC8* UintToDes8LC( TUint aUint );

/**
 * Converts an unsigned integer into a 8-bit descriptor.
 * NOTE: the number will not be in text format.
 * 
 * @param aUint Number to convert
 * @return Dynamically allocated buffer containing the data
 */
HBufC8* UintToDes8L( TUint aUint );


/**
 * Converts an integer into a 16-bit descriptor.
 *
 * The required length for the target descriptor depends on the used integer.
 *
 * For 8- and 16-bit integers the length of the descriptor must be at least 1
 * For 32-bit integers the length of the descriptor must be at least 2
 * and for 64-bit integers it must be at least 4.
 *
 * @note The number will not be in text format.
 * @note The number can be unsigned or signed and 8, 16, 32 or 64 bit.
 *
 * 
 * @param aInt Number to convert. 
 * @param aDes Target descriptor. Length must be at least 
 */
template<typename T>
void IntToDes16( const T& aInt, TDes& aDes );


template<>
void IntToDes16<TInt64>( const TInt64& aInt, TDes& aDes );


/**
 * Converts the raw contents of a 16-bit descriptor into an integer.
 * @note The number will not be in text format.
 * @note The number can be unsigned or signed and 8, 16, 32 or 64 bit.
 * 
 
 * @param aDes Source descriptor. Length must be at least 1 for 8 and 16-bit
 * integers, 2 for 32-bit integers and 4 for 64-bit integers.
 * @param aInt Number to convert. 
 */
template<typename T>
void Des16ToInt( const TDesC& aDes, T& aInt );

template<>
void Des16ToInt<TInt64>( const TDesC& aDes, TInt64& aInt );



/**
 * Externalizes the descriptor to the stream with length information
 *
 * @param aDes Descriptor to externalize
 * @param aStream Target stream
 */
void ExternalizeDesL( const TDesC16& aDes, RWriteStream& aStream );


/**
 * Internalizes a descriptor written with ExternalizeDesL() from the stream
 *
 * @param Target descriptor pointer. Old descriptor is deleted if the read is 
 * successful and the length of the data was > 0.
 * @param aStream Source stream
 * @return Length of the read data
 */
TInt InternalizeDesL( HBufC16*& aDes, RReadStream& aStream );


/**
 * Externalizes the descriptor to the stream with length information
 *
 * @param aDes Descriptor to externalize
 * @param aStream Target stream
 */
void ExternalizeDesL( const TDesC8& aDes, RWriteStream& aStream );

/**
 * Internalizes a descriptor written with ExternalizeDesL() from the stream
 *
 * @param Target descriptor pointer. Old descriptor is deleted if the read is 
 * successful and the length of the data was > 0.
 * @param aStream Source stream
 * @return Length of the read data
 */
TInt InternalizeDesL( HBufC8*& aDes, RReadStream& aStream );


/**
 * Returns the path where the Engine data has been installed.
 * Examples:
 * c:\private\12345678\
 * z:\private\DEADBEEF\
 * 
 * @return TPath Engine path.
 */
TPath EnginePathL( RFs& aFs );


/**
 * Returns the path where the Engine may write data.
 * Examples:
 * c:\private\12345678\
 * 
 * @return TPath Path where the engine may write data.
 */
TPath WritableEnginePathL( RFs& aFs );

/**
 * Searches for the given file from engine's private paths
 *
 * @param aFs File server session
 * @param aFilename File to search for
 * @return Path to the file
 * @leave KErrNotFound if the file was not found
 */
HBufC* FindEngineFileL( RFs& aFs, const TDesC& aFilename );

/**
 * Checks whether a drive is a rom drive, and thus not writable.
 * 
 * @param aFs File server session
 * @param aDriveChar Drive to check, ie. C,D,E,..,Z
 * @return ETrue if drive is ROM.
 */
TBool IsRomDriveL( RFs& aFs, TChar aDriveChar );


/**
 * Converts a drive number into a drive letter.
 * 
 * @param aDrive Drive number
 * @return TChar Drive letter
 */
TChar DriveToCharL( TInt aDrive );


/**
 * Gives the root path of a given drive.
 *
 * @param aFs File server session
 * @param aDriveChar Drive to check, ie. C,D,E,..,Z
 * @return HBufC* Root path of the drive. 
 * (Notice, this is not necessarily C:\ but can be, for example, 
 * C:\data.)
 */
HBufC* DriveRootPathLC( RFs& aFs, const TChar& aDriveLetter );


/**
 * Converts symbian language codes to java form (e.g. ELangEnglish -> "en-EN")
 * 
 * @param aLang Language code
 * @return HBufC* Language string in java form.
 */
HBufC* LangCodeToDescLC( TLanguage aLang );


/**
 * Removes [ and ] from the UID name
 *
 * @param aUid UID
 * @return UID in a descriptor without [ and ]
 */
TUidName CleanUidName( const TUid& aUid );


/**
 * Encodes a filename so that it can safely be used within 
 * the filesystem by replacing certain characters.
 * @param aFileName Filename to be encoded
 * @return HBufC* Encoded filename
 */
HBufC* EncodeFilenameLC( const TDesC& aFileName, RFs& aFs );


/**
 * Creates a private directory on the designated drive
 * @param aFs File server session
 * @param aDriveAndColon Drive letter and a colon, eg. "C:"
 * @param aPath Created directory path including drive
 */
void CreatePrivatePathL( RFs& aFs, const TDesC& aDriveAndColon, TDes& aPath );
 

/**
 * Returns maximum amount of free space on devices drive (flash or hd).
 * 
 * @param aFs File server session
 * @param aDriveNumber Drive number.
 * @return TInt64 Amount of free space.
 */
TInt64 FreeDiskSpaceL( RFs& aFs, TInt aDriveNumber );    


#ifndef USE_MMF_RESET_AND_DESTROY_PUSHL
/**
 * A utility class used by the templated function CleanupResetAndDestroyPushL() 
 * to create a TCleanupItem item that will perform a ResetAndDestroy type 
 * operation on the class T type object.
 *
 * @see CleanupResetAndDestroyPushL()
 */ 
template <typename T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};
	

/**
 * Pushes the given item to cleanupstack and calls ResetAndDestroy() to it
 * in PopAndDestroy or in case a leave occurs.
 */	
template <typename T>
inline void CleanupResetAndDestroyPushL(T& aRef);

#endif // USE_MMF_RESET_AND_DESTROY_PUSHL

/**
 * A utility class used by the templated function CleanupInternalReleasePushL()
 * to create a TCleanupItem item that will perform a InternalRelease operation
 * on th eclass T type object.
 *
 * @see CleanupInternalReleasePushL()
 */
template <typename T>
class CleanupInternalRelease
    {
public:
    inline static void PushL( T& aRef );
private:
    static void InternalRelease( TAny* aPtr );
    };
    
/**
 * Pushes the given item to cleanupstack and calls InternalRelease() to it
 * in PopAndDestroy or in case a leave occurs.
 */
template <typename T>
inline void CleanupInternalReleasePushL( T& aRef );


/**
 * Checks whether the given uri uses https shceme.
 *
 * @return ETrue if uri is https, EFalse if not.
 */
TBool IsHttpsUri( const TDesC& aUri );


/**
 * Retrieves the bytes allocated by the directory in the filesystem.
 * @param aFs File server session
 * @param aDir Directory
 * @return TInt Allocation in bytes.
 */
TInt FileSystemAllocationL( RFs& aFs, const TDesC& aDir );

/**
 * Reads a file to the 8-bit descriptor.
 * 
 * @param aFs Reference to the fileserver session.
 * @param aFileName File name.
 * @return HBufC8* Dynamically allocated descriptor containing the file data.
 */
HBufC8* ReadFileL( RFs& aFs, const TDesC& aFileName );


/**
 * Reads a file to the 8-bit descriptor.
 * 
 * @param aFile An opened file handle. Must be closed by the caller
 * @return HBufC8* Dynamically allocated descriptor containing the file data.
 */
HBufC8* ReadFileL( RFile& aFile );


/**
 * Checks if the disk space would go below critical if the given amount
 * of data was written to disk
 *
 * @param aPath Path to check (only drive letter is mandatory)
 * @param aFs File server session
 * @param aSpaceNeeded Needed space
 * @leave KErrDiskFull if there is not enough space
 */
void WouldDiskSpaceGoBelowCriticalLevelL( const TDesC& aPath,
                                          RFs& aFs,
                                          TInt aSpaceNeeded );


/**
 * Converts a version string of format "1.2.3" or "1.2" or "1" to version components.
 * 
 * @param aStr Version string
 * @param aMajor On return contains the major version
 * @param aMinor On return contains the minor version or 0 it not available
 * @param aBuild On return contains the build number or 0 if not available.
 */
void DesToVersionL( const TDesC& aVersion, 
    TUint16& aMajor, TUint16& aMinor, TUint32& aBuild );

/**
 * Function that calls close for each element of given array once.
 * In the end this function calls reset for the array.
 *
 */
template <typename T>
void ResetAndCloseArray( RPointerArray<T>& aArray );

template <typename T>
void ResetAndCloseArray( RArray<T>& aArray );


/**
 * Function that casts each element as T and then calls delete for it.
 * In the end this function calls reset for the array.
 */
template <typename Casted, typename T >
void ResetAndDestroyWithCast( RPointerArray<T>& aArray );


template <typename T>
void DeleteFromArray( RPointerArray<T>& aArray, TInt aIndex );

template <typename Casted, typename T>
void DeleteFromArray( RPointerArray<T>& aArray, TInt aIndex );


/**
 * Internalizes an enumeration from the given stream
 *
 * @note Enumeration is read as a TInt32
 */
template <typename EnumType>
void InternalizeEnumL( EnumType& aTarget, RReadStream& aReadStream );

/**
 * Externalizes an enumeration to the given stream
 *
 * @note Enumeration is written as a TInt32
 */
template <typename EnumType>
void ExternalizeEnumL( const EnumType& aSource, RWriteStream& aWriteStream );


/**
 * Appends aRoot with aAppendPath and puts them to aPath
 *
 * @param aPath RBuf without any allocated memory. This is put to CleanupStack
 * @param aRoot First path. Should contain the path delimiter at the end.
 * @param aAppendPath Second path. Appended to aRoot
 */
void AppendPathsLC( 
    RBuf& aPath, 
    const TDesC& aRoot,
    const TDesC& aAppendPath );

/**
 * Helper methods that leave if aError != KErrNone and aError != aAcceptErr
 */
void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr );
void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr, TInt aAcceptErr2 );
void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr, TInt aAcceptErr2, 
    TInt aAcceptErr3 );


/**
 * Opens a file or creates it if it doesn't already exist
 */
void OpenOrCreateFileL( 
    RFs& aFs, RFile& aFile, const TDesC& aFilePath, TUint aFileMode );


/**
 * Checks if aData == aComp1 or aData == aComp2
 * Can be used for simplifying horrendous if-clauses
 */
template <typename T>
TBool IsOneOf( const T& aData, const T& aComp1, const T& aComp2 );

template <typename T>
TBool IsOneOf( const T& aData, const T& aComp1, const T& aComp2, const T& aComp3 );

template <typename T>
TBool IsOneOf( const T& aData, const T& aComp1, const T& aComp2, 
    const T& aComp3, const T& aComp4 );


/**
 * Releases the object and sets the pointer as NULL
 */
template <typename T>
void ReleasePtr( T*& aObject );


/**
 * Closes the object and sets the pointer as NULL
 */
template <typename T>
void ClosePtr( T*& aObject );


/**
 * Deletes the object and sets the pointer as NULL
 */
template <typename T>
void DeletePtr( T*& aObject );


#include "catalogsutils.inl"
	
#endif // T_CATALOGSUTILS_H
