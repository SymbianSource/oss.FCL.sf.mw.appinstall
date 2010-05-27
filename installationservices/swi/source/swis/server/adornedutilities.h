/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of a collection of utility functions handling adorned filenames
*
*/


/**
 @file
 @internalTechnology
 @released
*/
#ifndef __ADORNED_UTILITIES_H__
#define __ADORNED_UTILITIES_H__

#include <f32file.h>
#include <e32def.h>
#include <e32cmn.h>

namespace Swi
{
/** Checks if the input filename is adorned and cuts off the adorned part 
	@param aAdornedFilename buffer containing the input filename (possibly adorned)
	@param pointer to the output buffer where the unadorned name is returned
	*/
	void GetUnadornedFileName(const TDesC& aAdornedFilename, TDes& aUnadornedFilename);

	/** Checks if the two input filenames  are variants of each other (belong to the same adorned name "family")
	@param aFileName1 first name to compare (full path and name)
	@param aFileName2 second name to compare (full path and name)
	*/
	TBool IsAdornedVariationOfL(const TDesC& aFileName1, const TDesC& aFileName2);
	
	/** Searches through the file system looking for all variants of searchNameWild
	@param aFs handle to a fileserver session
	@param aSearchNameWild represents the filename pattern that is to be searched for (i.e: afile{????????}.ext)
	@param aSearchPath represents the path to be searched through on all drives (i.e: \sys\bin\)
	@param aAdornedFileNamesFound all files matching the pattern are returned in this array (contains fully qualified filenames i.e. full path+name+ext)
	*/
	void FindAllAdornedVariantsL(RFs& aFs, const TDesC& aSearchNameWild, const TDesC& aSearchPath, RPointerArray<HBufC>& aAdornedFileNamesFound);

	/** Generates adorned filename pattern using wild chars for searching
	@param aFileName the filename that is the base of the pattern; can be either unadorned or adorned name (i.e: somename.ext or somename{12345678}.ext) 
	@param aSearchNameWild the calculated pattern output (i.e: somename{????????}.ext)
	*/
	void GenerateSearchNameWildL(const TDesC& aFileName, TDes& aSearchNameWild);

	extern const TInt FileNameUnadornedPartLength;
}
#endif
