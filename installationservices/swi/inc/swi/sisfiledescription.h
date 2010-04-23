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
* Definition of the Swi::Sis::CFileDescription
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISFILEDESCRIPTION_H__
#define __SISFILEDESCRIPTION_H__

#include <e32base.h>

#include "sisfield.h"

namespace Swi
{
namespace Sis
 {

class CHash;
class CString;
class CCapabilities;
class TPtrProvider;

/**
 * This class represents a SisFileDescription. SisFileDescription is a basic structure found in  files.
 * For a full description see SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CFileDescription : public CField
	{
public:

	/**
	 * This creates a new CFileDescription object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileDescription representing the data read.	 	 	 	 	 	 	 	 	 
	 */
	IMPORT_C static CFileDescription* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CFileDescription object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileDescription representing the data read.	 	 	 	 	 	 	 	 	 	 
	 */
	IMPORT_C static CFileDescription* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CFileDescription object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileDescription representing the data read.	 	 	 	 	 	 	 	 	 	 
	 */

	IMPORT_C static CFileDescription* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CFileDescription object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CFileDescription representing the data read.	 	 	 	 	 	 	 	 	 	 
	 */

	IMPORT_C static CFileDescription* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CFileDescription();

public: // Accessors
	
	inline const CString& Target() const;

	inline const CString& MimeType() const;

	inline const CHash& Hash() const;

	inline const CCapabilities* Capabilities() const;
	
	inline TSISFileOperation Operation() const;

	inline TSISFileOperationOptions OperationOptions() const;

	inline TFieldLength DataLength() const;

	inline TFieldLength UncompressedLength() const;
	
	inline TUint32 Index() const;

private:

	CFileDescription();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in place constructor.
	 *
	 * @param aDataProvider		 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	CString* iTarget;
	
	CString* iMimeType;
	
	CCapabilities *iCapabilities;

	CHash* iHash;
	
	TSISFileOperation iOperation;
	
	TSISFileOperationOptions iOperationOptions;
	
	TFieldLength iDataLength;
	
	TFieldLength iUncompressedLength;
	
	TUint32 iIndex;
	};

// inline functions from CFileDescription

const CString& CFileDescription::Target() const
	{
	return *iTarget;	
	}

const CString& CFileDescription::MimeType() const
	{
	return *iMimeType;	
	}
	
const CCapabilities* CFileDescription::Capabilities() const
	{
	return iCapabilities;
	}

const CHash& CFileDescription::Hash() const
	{
	return *iHash;	
	}

TSISFileOperation CFileDescription::Operation() const
	{
	return iOperation;	
	}

TSISFileOperationOptions CFileDescription::OperationOptions() const
	{
	return iOperationOptions;	
	}

TInt64 CFileDescription::DataLength() const
	{
	return iDataLength;
	}

TInt64 CFileDescription::UncompressedLength() const
	{
	return iUncompressedLength;
	}
	
TUint32 CFileDescription::Index() const
	{
	return iIndex;
	}


 } // namespace Sis
} //namespace Swi

#endif
