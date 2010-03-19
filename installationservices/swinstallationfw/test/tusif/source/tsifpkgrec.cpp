/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file implements a package file recognizer for the Reference Installer.
*
*/


#include <apmrec.h>
#include <apmstd.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "tsifpkgrec.h"

const TUid KUidTestPkgRecDll = {0x102866F0};
const TInt KUidTestPkgRecImpl = 0x102866F1;
const TInt KTestNumMimeTypes = 1;
const TInt KMaxBufferLength = 0xf;

/**
A file extension and MIME type for reference package files.
*/
_LIT(KTestPkgFileExt, ".siftestpkg");
_LIT8(KDataTestMimeType, "binary/sif-testpkg");

CTestSifPkgRecognizer::CTestSifPkgRecognizer()
	: CApaDataRecognizerType(KUidTestPkgRecDll, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = KTestNumMimeTypes;
	}

TUint CTestSifPkgRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CTestSifPkgRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KDataTestMimeType);
	}

void CTestSifPkgRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& /*aBuffer*/)
	{
	
	// This recognition is based only on the file extension
	const TInt dotPos = aName.LocateReverse('.');
	if (dotPos == KErrNotFound)
		{
		iConfidence = ENotRecognized;
		return;
		}
	TInt extLen = aName.Length() - dotPos;
	if (aName.Right(extLen).CompareF(KTestPkgFileExt) == 0)
		{
		iDataType = TDataType(KDataTestMimeType);
		iConfidence = ECertain;
		}
	}

CApaDataRecognizerType* CTestSifPkgRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CTestSifPkgRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidTestPkgRecImpl, CTestSifPkgRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
