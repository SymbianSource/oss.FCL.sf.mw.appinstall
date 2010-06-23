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
* Implementation of the CFileSisDataProvider
*
*/


#include <f32file.h> 

#include "filesisdataprovider.h"

using namespace Swi;

EXPORT_C CFileSisDataProvider* CFileSisDataProvider::NewL(RFs& aFs, const TFileName& aFileName, TInt aFileMode)
	{
	CFileSisDataProvider* self=CFileSisDataProvider::NewLC(aFs, aFileName, aFileMode);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CFileSisDataProvider* CFileSisDataProvider::NewLC(RFs& aFs, const TFileName& aFileName, TInt aFileMode)
	{
	CFileSisDataProvider* self=new(ELeave) CFileSisDataProvider(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aFileMode);
	return self;	
	}

EXPORT_C CFileSisDataProvider* CFileSisDataProvider::NewL(RFile& aFileHandle)
	{
	CFileSisDataProvider* self=CFileSisDataProvider::NewLC(aFileHandle);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CFileSisDataProvider* CFileSisDataProvider::NewLC(RFile& aFileHandle)
	{
	CFileSisDataProvider* self=new(ELeave) CFileSisDataProvider(aFileHandle);
	CleanupStack::PushL(self);
	return self;	
	}

CFileSisDataProvider::CFileSisDataProvider(RFs& aFs)
	: iFs(aFs), iCloseHandle(ETrue)
	{
	}

CFileSisDataProvider::CFileSisDataProvider(RFile& aFileHandle)
	: iFile(aFileHandle), iCloseHandle(EFalse)
	{
	}

void CFileSisDataProvider::ConstructL(const TFileName& aFileName, TInt aFileMode)
	{
	User::LeaveIfError(iFile.Open(iFs, aFileName, aFileMode)); 
	}

EXPORT_C CFileSisDataProvider::~CFileSisDataProvider()
	{
	if (iCloseHandle)
		{
		iFile.Close();
		}
	}

// From MSisDataProvider

EXPORT_C TInt CFileSisDataProvider::Read(TDes8& aDes)
	{
	return iFile.Read(aDes);
	}

EXPORT_C TInt CFileSisDataProvider::Read(TDes8& aDes, TInt aLength)
	{
	return iFile.Read(aDes, aLength);
	}

EXPORT_C TInt CFileSisDataProvider::Seek(TSeek aMode, TInt64& aPos)
	{
	if (aPos < TInt64(KMinTInt32) || TInt64(KMaxTInt32) < aPos)
		{
		return KErrNotSupported;
		}

	TInt pos=I64INT(aPos);
	TInt32 err=iFile.Seek(aMode, pos);
	aPos=pos;
	return err;
	}

