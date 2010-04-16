/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SECUTILS_H__
#define __SECUTILS_H__

#include <e32std.h>
#include <f32file.h>
#include <e32uid.h>
#include "swi/swiutils.h"

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
	   
    /**
     * Appends a Uid to the buffer and publishes the same.
     * @param aBuf The buffer containing the list of Uids that is published.
     * @param aUid The UID of the package which has been processed.
     */
    IMPORT_C static TInt PublishPackageUid(TUid aUid, TUid (&aUidList)[KMaxUidCount]);
    
    /**
     * Checks if the given Uid is present in the buffer that contains the list of Uids published.
     * @param aBuf The buffer containing the list of Uids that is published.
     * @param aUid The UID of the package which to be verified.
     */
    IMPORT_C static TBool IsPackageUidPresent(TUid aUid, const TUid (&aUidList)[KMaxUidCount]);
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

#endif
