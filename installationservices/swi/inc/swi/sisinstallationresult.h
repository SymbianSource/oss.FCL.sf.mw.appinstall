/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SIS Installation Result class
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISINSTALLATIONRESULT_H__
#define __SISINSTALLATIONRESULT_H__

#include <e32base.h>

class RWriteStream;
class RReadStream;

namespace Swi
{
		
/**
 * This class represents the result of a SIS file installation
 */
class CInstallationResult : public CBase
	{
public:
	IMPORT_C static CInstallationResult* NewL();
	virtual ~CInstallationResult();
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

	inline TBool operator==(const CInstallationResult& other) const;
	inline TBool operator!=(const CInstallationResult& other) const;

private:
	CInstallationResult();

public:
	enum TResult
		{
		ESuccess = 0,
		ECancelled,
		ETimedOut,
		EInstallLeft,	// The installer left with an error code
		EInvalidPartialUpgradeVersion,
		ECannotOverwriteFile,
		EInvalidSISFile,
		EFileNotFound,
		EMissingDependency,
		EWrongDependency,
		EFileError
		};

public:
	TResult iResult;
	TInt iLeaveCode; // error code if iResult = EInstallLeft

	/**
	 * If there is an error while doing an operation on a file, this contains
	 * the name of the file.
	 */
	TFileName iProblematicFileName;
	};
} // namespace Swi

inline TBool Swi::CInstallationResult::operator==(const CInstallationResult& other) const
	{
	// Only compare result code
	return (iResult == other.iResult);
	}

inline TBool Swi::CInstallationResult::operator!=(const CInstallationResult& other) const
	{
	return (iResult != other.iResult);
	}

#endif
