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
*
*/


#include "secutils.h"
#include <hash.h> 

namespace Swi
{

EXPORT_C TBool SecUtils::IsExeL(const TDesC& aFileName)
	{
	TEntry entry;
	EntryL(aFileName, entry);
	return IsExe(entry);
	}
	
EXPORT_C TBool SecUtils::IsDllL(const TDesC& aFileName)
	{
	TEntry entry;
	EntryL(aFileName, entry);
	return IsDll(entry);
	}

void SecUtils::EntryL(const TFileName& aFileName, TEntry& aEntry)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Entry(aFileName, aEntry));
	CleanupStack::PopAndDestroy(&fs);
	}

EXPORT_C HBufC* SecUtils::HexHashL(const TDesC& aName)
	{
	CMessageDigest* digester = CMD5::NewL();
	CleanupStack::PushL(digester);

	TPtrC8 nameHash = digester->Final(TPtrC8(REINTERPRET_CAST(const TUint8*, aName.Ptr()),
						aName.Size()));

	HBufC* hashBuf = HBufC::NewL(nameHash.Length() * 2);
	TPtr hashPtr = hashBuf->Des();
	
	_LIT(KHexDigit, "%02x");
	for (TInt i = 0; i < nameHash.Length(); i++)
		{
		hashPtr.AppendFormat(KHexDigit, nameHash[i]);
		}

	CleanupStack::PopAndDestroy(digester);
	return hashBuf;
	}
	
} // namespace Swi
