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
 SmallArray.h
*/

// System Include
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

	
static const TInt KSmallArrayMaxSize = 100;
static const TUint32 SmallArray[KSmallArrayMaxSize]={
	0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,0x10
};
/**
Contains Methods to Initialise and Read the SmallArray
*/

class CSmallArray : public CBase
  	{
public:
	/** Construction */	
	IMPORT_C static CSmallArray* NewLC(const TDesC& aString);
	/** Destructor - virtual and class not intended	 for derivation, so not exported	*/
	~CSmallArray();
	IMPORT_C TBool ReadSmallArray(TUint32 aNumber);
private:
	
	static TUint32 varr;
	/** C++ constructor - not exported;   implicitly called from NewLC() */
	CSmallArray();
	/** 2nd phase construction, called by NewLC() */
	void ConstructL(const TDesC& aString); 

private:
	/** The member variable is for Allocating container for string data (destructor destroys) */
	HBufC*        iString;  
	};
