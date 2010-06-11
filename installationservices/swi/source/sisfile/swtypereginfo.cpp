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


#include "swtypereginfo.h"
#include "arrayutils.h"
#include "ipcutil.h"
using namespace Swi;

EXPORT_C void SoftwareTypeRegInfoUtils::SerializeArrayL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf8& aSerializedArray)
	{
	TInt bufLen = sizeof(TInt);
	const TInt count = aSwTypeRegInfoArray.Count();
	for (TInt i=0; i<count; ++i)
	    {
	    bufLen += GetObjectSizeL(aSwTypeRegInfoArray[i]);
	    }

	aSerializedArray.ReAllocL(aSerializedArray.Length()+bufLen);
	RDesWriteStream ws(aSerializedArray);
	CleanupClosePushL(ws);

	ExternalizePointerArrayL(aSwTypeRegInfoArray, ws);

	ws.CommitL();
	CleanupStack::PopAndDestroy(&ws);
	}

EXPORT_C void SoftwareTypeRegInfoUtils::UnserializeArrayL(RReadStream& aStream, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray)
	{
	const TInt numElems = aStream.ReadInt32L();
	for (TInt i=0; i<numElems; ++i)
		{
		Usif::CSoftwareTypeRegInfo* info = Usif::CSoftwareTypeRegInfo::NewL(aStream);
		CleanupStack::PushL(info);
		aSwTypeRegInfoArray.AppendL(info);
		CleanupStack::Pop(info);
		}
	}

EXPORT_C void SoftwareTypeRegInfoUtils::SerializeUniqueSwTypeNamesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf& aSerializedNames)
	{
	const TInt numNames = aSwTypeRegInfoArray.Count();
	
	TInt bufLen = 0;
	for (TInt i=0; i<numNames; ++i)
		{
		const Usif::CSoftwareTypeRegInfo& info = *aSwTypeRegInfoArray[i];
		bufLen += info.UniqueSoftwareTypeName().Length() + sizeof(TChar);
		}
	
	aSerializedNames.ReAllocL(aSerializedNames.Length()+bufLen);
	
	for (TInt i=0; i<numNames; ++i)
		{
		const Usif::CSoftwareTypeRegInfo& info = *aSwTypeRegInfoArray[i];
		aSerializedNames.Append(info.UniqueSoftwareTypeName());
		aSerializedNames.Append(static_cast<TChar>(KUniqueNameSeparator));
		}
	}

EXPORT_C void SoftwareTypeRegInfoUtils::UnserializeUniqueSwTypeNamesL(const TDesC& aSerializedNames, RArray<TPtrC>& aUniqueSwTypeNames)
	{
	TPtrC buf(aSerializedNames);
	for (;;)
		{
		const TInt sep = buf.Locate(static_cast<TChar>(KUniqueNameSeparator));
		if (sep != KErrNotFound)
			{
			aUniqueSwTypeNames.AppendL(buf.Left(sep));
			buf.Set(buf.Mid(sep+1));
			}
		else
			{
			if (buf.Length() > 0)
				{
				User::Leave(KErrGeneral);
				}
			break;
			}
		}
	}

EXPORT_C void SoftwareTypeRegInfoUtils::ExtractMimeTypesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RPointerArray<HBufC8>& aMimeTypes)
	{
	for (TInt i=0; i<aSwTypeRegInfoArray.Count(); ++i)
		{
		const RPointerArray<HBufC>& mimeTypes = aSwTypeRegInfoArray[i]->MimeTypes();
		for (TInt m=0; m<mimeTypes.Count(); ++m)
			{
			const TDesC& mimeType = *mimeTypes[m];
			HBufC8* mimeType8 = HBufC8::NewLC(mimeType.Length());
			mimeType8->Des().Copy(mimeType);

			aMimeTypes.AppendL(mimeType8);

			CleanupStack::Pop(mimeType8);
			}
		}
	}
