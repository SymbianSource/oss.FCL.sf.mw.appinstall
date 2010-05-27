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


#include <apmrec.h>
#include <apmstd.h>
#include <asn1cons.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "tmimerecog.h"


const TInt KTMimeRecognizerValue = 0x1028191c;
const TUid KUidTMimeRecognizer = { KTMimeRecognizerValue };

// test mime type
_LIT8( KDataTestMimeType, "x-epoc/x-dummymime-app" );



// ----------------------------------------------------------------------------
// CApaTMimeRecognizer
//

CApaTMimeRecognizer::CApaTMimeRecognizer()
	: CApaDataRecognizerType( KUidTMimeRecognizer, CApaDataRecognizerType::ENormal )
	{
	iCountDataTypes = 1;
	}



TUint CApaTMimeRecognizer::PreferredBufSize()
	{
	return 0;
	}



TDataType CApaTMimeRecognizer::SupportedDataTypeL( TInt aIndex ) const
	{
	__ASSERT_DEBUG( aIndex >= 0 && aIndex < 1,
					User::Panic( _L("TMIMEREC"), 0 ) );
	switch ( aIndex )
		{
		case 0:
			return TDataType( KDataTestMimeType );
		default:
			return TDataType( KDataTestMimeType );
		}
	}

void CApaTMimeRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& /*aBuffer*/ )
	{
	TInt pos=aName.Locate('.');
	TInt elen=aName.Length()-pos;
	if (aName.Right(elen).CompareF(_L(".tsym"))==0)
		{
		iDataType = TDataType( KDataTestMimeType );
		iConfidence = EProbable;
		}
	}

CApaDataRecognizerType* CApaTMimeRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaTMimeRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x1028191b, CApaTMimeRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	

