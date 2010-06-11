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
#include "sifrefpkgrec.h"

const TUid KUidSifRefRecognizer = {0x10286351};
const TInt KSifRefNumMimeTypes = 1;
const TInt KMaxBufferLength = 0xf;

/**
A file extension and MIME type for reference package files.
*/
_LIT(KSifRefBinPkgFileExt, ".sifrefbinpkg");
_LIT8(KDataTestMimeType, "binary/sif-refpkg");

CSifRefPkgRecognizer::CSifRefPkgRecognizer()
	: CApaDataRecognizerType(KUidSifRefRecognizer, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = KSifRefNumMimeTypes;
	}

TUint CSifRefPkgRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CSifRefPkgRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KDataTestMimeType);
	}

void CSifRefPkgRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& /*aBuffer*/)
	{
	// This recognition is based only on the file extension
	const TInt dotPos = aName.LocateReverse('.');
	if (dotPos == KErrNotFound)
		{
		iConfidence = ENotRecognized;
		return;
		}
	TInt extLen = aName.Length() - dotPos;
	if (aName.Right(extLen).CompareF(KSifRefBinPkgFileExt) == 0)
		{
		iDataType = TDataType(KDataTestMimeType);
		iConfidence = ECertain;
		}
	}

CApaDataRecognizerType* CSifRefPkgRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CSifRefPkgRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10286352, CSifRefPkgRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
