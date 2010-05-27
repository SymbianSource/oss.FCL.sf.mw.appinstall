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


#include <s32mem.h>

#include "textdialog.h"
#include "msisuihandlers.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"
#include <utf.h>

enum TTextEncodingType
	{
	KEncUnicodeUCS2LE		= 1, 
	KEncUnicodeUCS2BE,
	KEncUnicodeUTF8,
	KEncUnknown
	};

namespace Swi
{

CTextDialogCmdHandler::CTextDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CTextDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf,
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Text Display dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// TFileTextOption
	TFileTextOption textOption;
	TPckg<TFileTextOption> textOptionPackage(textOption);
	readStream >> textOptionPackage;
	
	// TDesC
	HBufC8* text = HBufC8::NewLC(readStream, KMaxTInt);
	TPtr8 textPtr(text->Des());

	// Convert from other unicode encoding to UCS-2 Little Endian
	// Detect the type of encoding
	TTextEncodingType encodingType = KEncUnknown;
	if ((textPtr.Size() > 2) && (textPtr[0] == 0xFF) && (textPtr[1] == 0xFE))
		{
		encodingType = KEncUnicodeUCS2LE;
		}
	else if ((textPtr.Size()) > 2 && (textPtr[0] == 0xFE) && (textPtr[1] == 0xFF))
		{
		encodingType = KEncUnicodeUCS2BE;
		}
	else if ((textPtr.Size()) > 3 && (textPtr[0] == 0xEF) && (textPtr[1] == 0xBB) && (textPtr[2] == 0xBF))
		{
		encodingType = KEncUnicodeUTF8;
		}

	RBuf16 convertedText;
	CleanupClosePushL(convertedText);
	switch (encodingType)
		{
		case KEncUnicodeUCS2LE:
		case KEncUnicodeUCS2BE:		// Fall through case
			// Strip off the markers
			textPtr.Delete(0, 2);
			// Convert 8 to 16 bit
			convertedText.CreateL(textPtr.Size() / 2);
			convertedText.Copy(
				reinterpret_cast<const TUint16*> (textPtr.Ptr()), textPtr.Size() / 2);
			if (encodingType == KEncUnicodeUCS2BE)
				{
				// For unicodeUCS2BE convert from UCS-2 Big Endian to UCS-2 Little Endian
				// Only the Endian-ness needs to be changed
				for (TInt index = 0; index < convertedText.Length(); ++index)
					{
					TUint16 word = convertedText[index];
					convertedText[index] = (word >> 8) | (word << 8);
					}
				}
			break;

		case KEncUnicodeUTF8:
			// Strip off the markers
			textPtr.Delete(0, 3);
			// Convert from UTF-8 to UCS-2 Little Endian
			convertedText.Assign(CnvUtfConverter::ConvertToUnicodeFromUtf8L(textPtr));
			break;

		default:		// Unknown encoding
			convertedText.CreateL(textPtr.Size());
			convertedText.Copy(textPtr);
			break;
		}

	// Call UI Handler
	TBool result=iUiHandler.DisplayTextL(*appInfo, textOption, convertedText);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	CleanupStack::PopAndDestroy(5, &readStream); // readStream,appInfo,text,convertedText,os
	}

} // namespace Swi
