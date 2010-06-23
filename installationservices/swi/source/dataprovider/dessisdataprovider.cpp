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
* Implementation of the CDesSisDataProvider
*
*/


#include <e32std.h>

#include "dessisdataprovider.h"
#include "sisinstallerrors.h"

using namespace Swi;

EXPORT_C CDesDataProvider* CDesDataProvider::NewL(const TDesC8& aDes)
	{
	CDesDataProvider* self=CDesDataProvider::NewLC(aDes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDesDataProvider* CDesDataProvider::NewLC(const TDesC8& aDes)
	{
	CDesDataProvider* self=new(ELeave) CDesDataProvider(aDes);
	CleanupStack::PushL(self);
	return self;	
	}

CDesDataProvider::CDesDataProvider(const TDesC8& aDes)
	: iDes(aDes)
	{
	}

EXPORT_C CDesDataProvider::~CDesDataProvider()
	{
	}

// From MSisDataProvider

EXPORT_C TInt CDesDataProvider::Read(TDes8& aDes)
	{
	return Read(aDes, aDes.MaxSize());
	}

EXPORT_C TInt CDesDataProvider::Read(TDes8& aDes, TInt aLength)
	{
	if (aLength > aDes.MaxSize())
		{		
		return KErrOverflow;
		}

	TInt availableDataLength = 	iDes.Length() - iPos;
	TInt bytesToRead = aLength > availableDataLength ? availableDataLength : aLength;

	aDes.Copy(iDes.Mid(iPos, bytesToRead));
	iPos += bytesToRead;
	return KErrNone;
	}

EXPORT_C TInt CDesDataProvider::Seek(TSeek aMode, TInt64& aPos)
	{
	if (aPos < TInt64(KMinTInt32) || TInt64(KMaxTInt32) < aPos)
		{
		return KErrNotSupported;
		}

	TInt offset = I64INT(aPos);
	switch (aMode)
		{
		case ESeekStart:
			if ((offset < 0) || (offset >= iDes.Length()))
				{
				// Don't truncate offset as ESeekStart doesn't modify aPos
				return KErrArgument;
				}
			iPos = offset;
			break;
		case ESeekCurrent:
			if (offset >= 0)
				{	
				if (offset > (iDes.Length()-iPos))
					{
					iPos = iDes.Length();	
					}
				else
					{
					iPos += offset;
					}
				}
			else
				{
				if ((iPos+offset) < 0)
					{
					iPos = 0;
					}
				else
					{
					iPos += offset;
					}
				}
			// TInt64 has no assignment or construction from TInt32
			aPos = TInt(iPos);
			break;
		case ESeekEnd:
			if (offset > 0)
				{
				return KErrArgument;	
				}
			iPos = iDes.Length()+offset;
			if (iPos < 0)
				{
				iPos = 0;
				}
			aPos = TInt(iPos);
			break;
		case ESeekAddress:
			return KErrNotSupported;
			
		//Default Clause added as part of DEF100584			
		default:
    		return KErrArgument;
		}
	return KErrNone;
	}

