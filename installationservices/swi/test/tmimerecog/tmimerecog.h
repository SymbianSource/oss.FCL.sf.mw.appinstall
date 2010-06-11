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


#if !defined(__TMIMERECOG_H__)
#define __TMIMERECOG_H__

#include <apmrec.h>


/**
 * A recogniser that recognises the following MIME types:
 * 
 */
class CApaTMimeRecognizer : public CApaDataRecognizerType
	{
public:
	CApaTMimeRecognizer();

public: // from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL( TInt aIndex ) const;
	static CApaDataRecognizerType* CreateRecognizerL();

private: // from CApaDataRecognizerType
	void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );

	};



#endif
