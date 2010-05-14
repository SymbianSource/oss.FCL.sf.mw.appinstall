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


#include "recsisx.h"
#include "dessisdataprovider.h"
#include "swicommon.h"
#include <apadef.h>
#include "patchdata.h"
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <u32hal.h> 
#include <e32svr.h>

using namespace Swi;

CApaSisxRecognizer::CApaSisxRecognizer() : CApaDataRecognizerType (KApaSisx, 
																   CApaDataRecognizerType::ENormal)
	{
	iCountDataTypes = KSupportedDataTypesTotal;
	}

TUint CApaSisxRecognizer::PreferredBufSize ()
	{
	return sizeof(TCheckedUid);
	}

TDataType CApaSisxRecognizer::SupportedDataTypeL (TInt /*aIndex*/) const
	{
	return TDataType(KDataTypeSisx);
	}

void CApaSisxRecognizer::DoRecognizeL (const TDesC& aName, const TDesC8& aBuffer)
	{
    iDataType = TDataType(KDataTypeSisx);
    iConfidence = ENotRecognized;
	if (aBuffer.Size() < sizeof(TCheckedUid))
		{
		return;
		}	
	TCheckedUid checkedUID(aBuffer.Mid(0, sizeof(TCheckedUid)) );
	if (checkedUID.UidType()[0] != KUidSisxFile) 
		return;

	const TInt positionOfExtDelimiter=aName.LocateReverse(TChar(KExtDelimiter));
	TUint8 patchableConst = KSisxRecognizerConst;
	
#ifdef __WINS__
// For the emulator allow the constant to be patched via epoc.ini
UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
	(TAny*)"KSisxRecognizerConst", &patchableConst); // read emulator property (if present)
#endif

	if (patchableConst)
		{
		if (positionOfExtDelimiter >= 0)
	    	{
	    	if ((aName.Mid(positionOfExtDelimiter).CompareF(KFileExtensionSisx) == 0) ||
	    		(aName.Mid(positionOfExtDelimiter).CompareF(KFileExtensionLegacySis) == 0))
	        	{
	        	iConfidence = ECertain;
	        	}
			// All other extentions will be set to ENotRecognized
	    	}
		}
    else
	    {
   		// When KSisxRecognizerConst is zero, use the following method to recognise file extentions
	    if ((positionOfExtDelimiter >= 0) &&
        	(aName.Mid(positionOfExtDelimiter).CompareF(KFileExtensionSisx) == 0))
	        {
	        iConfidence = ECertain;
	        }
	    else
	    	{
	    	iConfidence = EPossible;
	    	}
	    }
	}

CApaDataRecognizerType* CApaSisxRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaSisxRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10204FC7, CApaSisxRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	
	
