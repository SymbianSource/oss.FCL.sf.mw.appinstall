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
* Static class containing Security Manager's utility functions
*
*/


/**
 @file
 @released
 @internalTechnology
*/

#include <e32std.h>
#include <f32file.h>
#include <e32uid.h>

namespace Swi
{

/**
 * Static class containing Security Manager's utility functions
 * @released
 * @internalTechnology
 */
class SecUtils
	{
public:
	/**
	 * Checks if the file is an EXE
	 * @param aFileName Name of the file to check
	 * @return          True if it is an EXE, false otherwise
	 */
	IMPORT_C static TBool IsExeL(const TDesC& aFileName);
	
	/**
	 * Checks if the file is an DLL
	 * @param aFileName Name of the file to check
	 * @return          True if it is an DLL, false otherwise
	 */
	IMPORT_C static TBool IsDllL(const TDesC& aFileName);
	
	/**
	 * Checks if the file is an EXE
	 * @param aEntry Dir entry of the file to check
	 * @return       True if it is an EXE, false otherwise
	 */
	static TBool IsExe(const TEntry& aEntry);
	
	/**
	 * Checks if the file is an DLL
	 * @param aEntry Dir entry of the file to check
	 * @return       True if it is an DLL, false otherwise
	 */
	static TBool IsDll(const TEntry& aEntry);
	
	/**
	 * Retrieves file entry information
	 * @param aFileName Name of the file to get entry for
	 * @param aEntry    Filled with file information
	 */
	static void EntryL(const TFileName& aFileName, TEntry& aEntry);
	
	/**
	 * Returns buffer containing hex representation of a hash of the provided
	 * descriptor.  Intended for use where a "unique" name is needed where the
	 * actual name may be too long.
	 *
	 * @param aName Descriptor containing data to hash
	 * @return      Pointer to allocated buffer, which will be owned by the
	 *              caller.
	 */	
	IMPORT_C static HBufC* HexHashL(const TDesC& aName);
	};

inline TBool SecUtils::IsExe(const TEntry& aEntry)
	{
	return (aEntry[0].iUid == KExecutableImageUidValue) ? ETrue : EFalse;
	}

inline TBool SecUtils::IsDll(const TEntry& aEntry)
	{
	return (aEntry[0].iUid == KDynamicLibraryUidValue) ? ETrue : EFalse;
	}
} // namespace Swi
