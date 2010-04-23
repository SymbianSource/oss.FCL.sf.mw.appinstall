/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
 @internalComponent 
*/

#ifndef	__OSINTERFACE_H__
#define	__OSINTERFACE_H__
#pragma warning(disable: 4786)

#include "symbiantypes.h"

#include <string>
#include <vector>
#include <tchar.h>
using namespace std;

#ifndef UNICODE
#undef _T
#undef _TCHAR
#define _T(S) S
#define _TCHAR char
#endif // UNICODE



/**
  * This class acts as an interface for different OS platform.This is used in order to support
  * portability between linux and windows platform.The current implementation supports windows 
  * platform.
  */

class OSInterface
{
public:

	/**
	  This method extracts all the files present within a specified directory.
	  @param	aRootDir	Reference of the directory from which the files are to be extracted.
	  @param	aExtension	Extension of the file to be extracted.
	  @return	string vector containing the files in the root directory (aRootDir) specified.
	 */

	static const std::vector<STRING> ExtractFilesFromDirectory(const STRING& aRootDir , const STRING& aExtension = _T("")) ;
	
	/**
	  This method which deletes the files of a particular extension present within a specified directory,
	  @param	aRootDir	Reference of the directory from which the files are to be deleted.
	  @param	aExtension	Extension of the file to be deleted.	
	 */

	static void DeleteFilesFromDirectory(const STRING& aRootDir , const STRING& aExtension);	

	/**
	  Checks whether the directory passed is  C: Based or not. Since the files within the C: based 
	  are numbered,check is made to see that the the string passed can be converted into integer .
	  @param	aFileName	Reference of the filename within the specified writable certstore to be verified.	
	  */
	static bool IsInteger(const STRING& aFileName) ;


	/**
	  Checks whether it is a valid directory .	
	  */

	static int CheckIfDirectory(const STRING& aName)  ;

	static int DeleteDirectory(STRING& aDirectory) ;

	/**
	  Reads the value from the key value pair specified in the section from an file.
	  @param	aSectionLabel	section name in the file whose key value pair is to be read.
	  @param	aRootLabel		The key whose value is to be read.
	  @param	aCertstoreFile	The file containing the section and key value pair.
	  @param	aBuf			To be populated with the value of the key.
	  */

	static void ReadPrivateProfile(const STRING& aSectionLabel , const STRING& aRootLabel , const STRING& aCertstoreFile , _TCHAR* aBuf);
};

#endif //__OSINTERFACE_H__


	
	
	
	


	

	
