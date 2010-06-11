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
* Definition of the CSISInfo
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISINFO_H__
#define __SISINFO_H__

#include <e32base.h>
#include "sisfield.h"
#include "sisstring.h"

#include "installtypes.h"

namespace Swi
{
namespace Sis
 {

class CVersion;
class CUid;
class CString;
class CDateTime;
class TPtrProvider;

/**
 * This class represents a SISInfo. SISInfo is a basic structure found in SIS files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CInfo : public CField
	{
public:
	/**
	 * This creates a new CInfo object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInfo representing the data read.	 
	 */
	IMPORT_C static CInfo* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CInfo object.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInfo representing the data read.	 	 
	 */
	IMPORT_C static CInfo* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CInfo object in place.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInfo representing the data read.	 	 
	 */

	IMPORT_C static CInfo* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	/**
	 * This creates a new CInfo object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CInfo representing the data read.	 	 
	 */

	IMPORT_C static CInfo* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);


	IMPORT_C virtual ~CInfo();

private:

	CInfo();

	/**
	 * The second-phase constructor.
	 *	 
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *	 
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:

	/**
	 * Accessor for the uid field.
	 */
	inline const CUid& Uid() const;

	/**
	 * This function returns the Names structure of the  file. This structure contains a name for each of
	 * the languages the  file supports.
	 */
	inline const RPointerArray<CString>& Names() const;

	/**
	 * This function returns the VendorNames structure of the  file. This structure 
	 * contains a name for each of the languages the  file supports.
	 */
	inline const RPointerArray<CString>& VendorNames() const;

	/**
	 * This function returns the UniqueVendorName string of the  file. This structure 
	 * contains a single name.
	 */
	inline const CString& UniqueVendorName() const;
	
	/**
	 * This function returns the version of the  file.
	 */
	inline const CVersion& Version() const;

	/**
	 * This function returns the install type of the  file.
	 */
	inline TInstallType InstallType() const;

	/**
	 * This function returns the install flags of the  file.
	 */
	inline TInstallFlags InstallFlags() const;


private:
	/**
	 * The uid of the  file.
	 */
	CUid* iUid;
	
	/**
	 * The Names structure of the  file. This structure contains a name for each of
	 * the languages the  file supports.
	 */
	RPointerArray<CString> iNames;
	
	/**
	 * The names of the provider of  file.
	 */
	RPointerArray<CString> iVendorNames;
	
	/**
	 * The version of the  file.
	 */
	CVersion* iVersion;

	/**
	 * Installation type i.e. New, partial upgrade etc
	 */
	TInstallType iInstallType;
	
	/**
	 * Installation flags i.e. Shutdown all applications etc
	 */
	TInstallFlags iInstallFlags;
	
	CString* iUniqueVendorName;

	CDateTime* iDateTime;
	};

// inline functions from CInfo

const CUid& CInfo::Uid() const
	{
	return *iUid;
	}


const RPointerArray<CString>& CInfo::Names() const
	{
	return iNames;
	}

const RPointerArray<CString>& CInfo::VendorNames() const
	{
	return iVendorNames;
	}

const CVersion& CInfo::Version() const
	{
	return *iVersion;
	}

TInstallType CInfo::InstallType() const
	{
	return iInstallType;
	}

TInstallFlags CInfo::InstallFlags() const
	{
	return iInstallFlags;
	}
	
const CString& CInfo::UniqueVendorName() const
	{
	return *iUniqueVendorName;
	}

 } // namespace Sis
} // namespace Swi

#endif
