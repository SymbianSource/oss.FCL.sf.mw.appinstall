/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file ADORNEDUTILITIES.H
 @internalComponent
 @released
*/

#ifndef	ADORNED_UTILITIES_H
#define	ADORNED_UTILITIES_H

#pragma warning (disable: 4786)

#include <algorithm>
#include <string>
#include <list>
#include <sstream>
#include <iostream>

#include "configmanager.h"

/** Checks if the input filename is adorned and cuts off the adorned part 
@param aAdornedFilename buffer containing the input filename (possibly adorned)
@param pointer to the output buffer where the unadorned name is returned
*/
void GetUnadornedFileName(const std::wstring& aAdornedFilename, std::wstring& aUnadornedFilename);

/** Checks if the two input filenames  are variants of each other (belong to the same adorned name "family")
@param aFileName1 first name to compare (full path and name)
@param aFileName2 second name to compare (full path and name)
*/
#ifndef __TOOLS2_LINUX__
bool IsAdornedVariationOf(const std::wstring& aFileName1, const std::wstring& aFileName2);
#else
bool IsAdornedVariationOf(const std::wstring& aFileName1, const std::wstring& aFileName2, const std::wstring& aDrivePath);
#endif
	
/** Searches through the file system looking for all variants of searchNameWild
@param aSearchNameWild represents the filename pattern that is to be searched for (i.e: afile{????????}.ext)
@param aSearchPath represents the path to be searched through on all drives (i.e: \sys\bin\)
@param aAdornedFileNamesFound all files matching the pattern are returned in this array (contains fully qualified filenames i.e. full path+name+ext)
*/
void FindAllAdornedVariants(const std::wstring& aSearchNameWild, const std::wstring& aSearchPath, std::list<std::wstring>& aAdornedFileNamesFound, const DrivesMap& aDriveMap);

/** Generates adorned filename pattern using wild chars for searching
@param aFileName the filename that is the base of the pattern; can be either unadorned or adorned name (i.e: somename.ext or somename{12345678}.ext) 
@param aSearchNameWild the calculated pattern output (i.e: somename{????????}.ext)
*/
void GenerateSearchNameWild(const std::wstring& aFileName, std::wstring& aSearchNameWild);

#endif	/* ADORNED_UTILITIES_H */
