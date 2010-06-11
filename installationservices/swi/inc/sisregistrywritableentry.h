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
* RSisRegistryWritableEntry - restricted  client registry entry interface 
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISREGISTRYWRITABLEENTRY_H__
#define __SISREGISTRYWRITABLEENTRY_H__

#include <e32std.h>
#include "swi/installtypes.h"
#include "swi/sisregistryentry.h"

namespace Swi
{
class CSisRegistryPackage;
class CSisRegistryDependency;
class CSisRegistryFileDescription;

class RSisRegistryWritableEntry : public RSisRegistryEntry
	{
public:

	/**
	 * Returns what drive the user selected for files in the Sisx file that
	 * did not specify drive.
	 *
	 * @return TChar The drive selected
	 */
	IMPORT_C TChar SelectedDriveL();
	
	/**
	 * Returns the install type for this package
	 *
	 * @return The install type of Sis::TInstallType
	 */
	IMPORT_C Sis::TInstallType InstallTypeL();

	/**
	 * Return the file descriptions associated with this package
	 *
	 * @param aFileDescriptions The array of file descriptions to be populated.
	 *
	 */
	IMPORT_C void FileDescriptionsL(RPointerArray<CSisRegistryFileDescription>& aFileDescriptions);
	
	/**
	 * Return the language ID's of matching supported languages with this package
	 *
	 * @param aMatchingSupportedLanguages The array of matching supported language ID's to be populated.
	 *
	 */
	IMPORT_C void GetMatchingSupportedLanguagesL(RArray<TLanguage>& aMatchingSupportedLanguages);

	};

} // namespace
#endif
