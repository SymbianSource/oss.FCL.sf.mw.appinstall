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
* SISINSTALLATIONRESULT.H
* SIS Installation Result class
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISREMOVALRESULT_H__
#define __SISREMOVALRESULT_H__

#include <e32base.h>

class RWriteStream;
class RReadStream;

namespace Swi
{
		
/**
 * This class represents the result of a SIS file removal
 */
class CRemovalResult : public CBase
	{
public:
	IMPORT_C static CRemovalResult* NewL();
	virtual ~CRemovalResult();
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

	inline TBool operator==(const CRemovalResult& other) const;
	inline TBool operator!=(const CRemovalResult& other) const;

private:
	CRemovalResult();

public:
	enum TResult
		{
		ESuccess = 0,
		ECancelled,
		ETimedOut,
		EInstallLeft,	// The installer left with an error code
		ENotFound,	
		EFileError
		};

public:
	TResult iResult;
	TInt iLeaveCode; // error code if iResult = EInstallLeft
	};
} // namespace Swi

inline TBool Swi::CRemovalResult::operator==(const CRemovalResult& other) const
	{
	// Only compare result code
	return (iResult == other.iResult);
	}

inline TBool Swi::CRemovalResult::operator!=(const CRemovalResult& other) const
	{
	return (iResult != other.iResult);
	}

#endif
