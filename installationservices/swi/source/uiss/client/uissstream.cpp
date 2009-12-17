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
 @file
 @publishedAll
 @released
*/

#include <e32std.h>

#include "uissstream.h"

namespace Swi
{

//	
// Streaming helpers form TDesC
//
	
EXPORT_C void StreamOutArrayL(const RPointerArray<TDesC>& aArray, 
                              RWriteStream& aStream)
	{
	TInt32 count = aArray.Count();
	aStream.WriteInt32L(count);

	for (TInt32 i = 0; i < count; ++i)
		{
		aStream << *aArray[i];
		}
	}

EXPORT_C void StreamInArrayL(RPointerArray<TDesC>& aArray, RReadStream& aStream)
	{
	TInt32 count = aStream.ReadInt32L();
	
	for (TInt32 i = 0;i < count; ++i)
		{
		HBufC* descriptor=HBufC::NewL(aStream, KMaxTInt);
		User::LeaveIfError(aArray.Append(descriptor));		
		}
	}

//	
// Streaming helpers form CCertificateInfo
//
	
EXPORT_C void StreamOutArrayL(const RPointerArray<CCertificateInfo>& aArray, 
                              RWriteStream& aStream)
	{
	TInt32 count = aArray.Count();
	aStream.WriteInt32L(count);

	for (TInt32 i = 0; i < count; ++i)
		{
		aArray[i]->ExternalizeL(aStream);
		}
	}

EXPORT_C void StreamInArrayL(RPointerArray<CCertificateInfo>& aArray, 
                             RReadStream& aStream)
	{
	aArray.ResetAndDestroy();
	
	TInt32 count = aStream.ReadInt32L();

	for (TInt32 i = 0; i < count; ++i)
		{
		CCertificateInfo* certInfo=CCertificateInfo::NewL(aStream);
		User::LeaveIfError(aArray.Append(certInfo));
		}
	}

//	
// Streaming helpers form TOCSPOutcome
//
	
EXPORT_C void StreamOutArrayL(const RPointerArray<TOCSPOutcome>& aArray, 
                              RWriteStream& aStream)
	{
	TInt32 count = aArray.Count();
	aStream.WriteInt32L(count);

	for (TInt32 i = 0; i < count; ++i)
		{
		// An outcome may be NULL
		if (aArray[i])
			{
			TPckg<TBool> present(ETrue);
			aStream << present;
			aStream.WriteL(reinterpret_cast<const TUint8*>(aArray[i]), 
				sizeof(TOCSPOutcome));
			}
		else
			{
			TPckg<TBool> notPresent(EFalse);
			aStream << notPresent;
			}
		}
	}

EXPORT_C void StreamInArrayL(RPointerArray<TOCSPOutcome>& aArray, 
                             RReadStream& aStream)
	{
	aArray.ResetAndDestroy();
	
	TInt32 count = aStream.ReadInt32L();

	for (TInt32 i = 0; i < count; ++i)
		{
		// read 'present' flag
		TBool present=EFalse;
		TPckg<TBool> presentPckg(present);
		aStream >> presentPckg;
		if (present)
			{
			TOCSPOutcome* o=new(ELeave) TOCSPOutcome;
			// read the outcome object as raw data from the stream
			aStream.ReadL(reinterpret_cast<TUint8*>(o), sizeof(TOCSPOutcome));
			User::LeaveIfError(aArray.Append(o));
			}
		}
	}
	
//	
// Streaming helpers form CPKIXValidationResultBase
//
	
EXPORT_C void StreamOutArrayL(
	const RPointerArray<CPKIXValidationResultBase>& aArray, 
	RWriteStream& aStream)
	{
	TInt32 count = aArray.Count();
	aStream.WriteInt32L(count);

	for (TInt32 i = 0; i < count; ++i)
		{
		aArray[i]->ExternalizeL(aStream);
		}
	}

EXPORT_C void StreamInArrayL(
	RPointerArray<CPKIXValidationResultBase>& aArray, 
	RReadStream& aStream)
	{
	aArray.ResetAndDestroy();
	
	TInt32 count = aStream.ReadInt32L();

	for (TInt32 i = 0; i < count; ++i)
		{
		CPKIXValidationResultBase* o=CPKIXValidationResultBase::NewLC();
		o->InternalizeL(aStream);
		User::LeaveIfError(aArray.Append(o));
		CleanupStack::Pop(o);
		}
	}

} // namespace Swi
