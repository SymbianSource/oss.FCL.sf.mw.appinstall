/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// This file defines the API for adornedfilenametestingdll.dll

#ifndef __ADORNEDFILENAMETESTINGDLL_H__
#define __ADORNEDFILENAMETESTINGDLL_H__


//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf


//  Constants

const TInt KadornedfilenametestingdllBufferLength = 15;
typedef TBuf<KadornedfilenametestingdllBufferLength> TadornedfilenametestingdllExampleString;


//  Class Definitions

class Cadornedfilenametestingdll : public CBase
	{
	public:	 // new functions
		IMPORT_C static Cadornedfilenametestingdll* NewL();
		IMPORT_C static Cadornedfilenametestingdll* NewLC();
		IMPORT_C ~Cadornedfilenametestingdll();

	public:	 // new functions, example API
		IMPORT_C TVersion Version() const;
		IMPORT_C void ExampleFuncAddCharL(const TChar& aChar);
		IMPORT_C void ExampleFuncRemoveLast();
		IMPORT_C const TPtrC ExampleFuncString() const;

	private:	// new functions
		Cadornedfilenametestingdll();
		void ConstructL();

	private:	// data
		TadornedfilenametestingdllExampleString* iString;
	};


#endif  // __ADORNEDFILENAMETESTINGDLL_H__


