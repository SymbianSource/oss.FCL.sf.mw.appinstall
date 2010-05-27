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


/**
 @file
 @internalComponent
*/

#if !defined(__RECSISX_H__)
#define __RECSISX_H__

#include <apmrec.h>

namespace Swi
{

/** 
 * A recognizer of SISX files 
 *
 */
class CApaSisxRecognizer : public CApaDataRecognizerType
	{
public:
	CApaSisxRecognizer();

public: // from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL (TInt aIndex) const;

	static CApaDataRecognizerType* CreateRecognizerL();

private: // from CApaDataRecognizerType
	void DoRecognizeL (const TDesC& aName, const TDesC8& aBuffer);
	};

 // Recognizer housekeeping
const TInt KSupportedDataTypesTotal = 1;

const TInt KUidApaSisx = 0x101FD0EF;
const TUid KApaSisx = { KUidApaSisx };


// MIME type
_LIT8(KDataTypeSisx, "x-epoc/x-sisx-app");
_LIT(KFileExtensionSisx, ".sis");
_LIT(KFileExtensionLegacySis, ".sisx");

} //namespace Swi


#endif	/* __RECSISX_H__ */
