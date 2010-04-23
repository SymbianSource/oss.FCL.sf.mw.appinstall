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


//  Include Files  

#include "adornedfilenametestingdll.h"	// Cadornedfilenametestingdll
#include "adornedfilenametestingdll.pan"	  	// panic codes


//  Member Functions

EXPORT_C Cadornedfilenametestingdll* Cadornedfilenametestingdll::NewLC()
	{
	Cadornedfilenametestingdll* self = new (ELeave) Cadornedfilenametestingdll;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


EXPORT_C Cadornedfilenametestingdll* Cadornedfilenametestingdll::NewL()
	{
	Cadornedfilenametestingdll* self = Cadornedfilenametestingdll::NewLC();
	CleanupStack::Pop(self);
	return self;
	}


Cadornedfilenametestingdll::Cadornedfilenametestingdll()
// note, CBase initialises all member variables to zero
	{
	}


void Cadornedfilenametestingdll::ConstructL()
	{
	// second phase constructor, anything that may leave must be constructed here
	iString = new (ELeave) TadornedfilenametestingdllExampleString;
	}


EXPORT_C Cadornedfilenametestingdll::~Cadornedfilenametestingdll()
	{
	delete iString;
	}


EXPORT_C TVersion Cadornedfilenametestingdll::Version() const
	{
	// Version number of example API
	const TInt KMajor = 1;
	const TInt KMinor = 0;
	const TInt KBuild = 1;
	return TVersion(KMajor, KMinor, KBuild);
	}


EXPORT_C void Cadornedfilenametestingdll::ExampleFuncAddCharL(const TChar& aChar)
	{
	__ASSERT_ALWAYS(iString != NULL, Panic(EadornedfilenametestingdllNullPointer));

	if (iString->Length() >= KadornedfilenametestingdllBufferLength)
		{
		User::Leave(KErrTooBig);
		}

	iString->Append(aChar);
	}


EXPORT_C void Cadornedfilenametestingdll::ExampleFuncRemoveLast()
	{
	__ASSERT_ALWAYS(iString != NULL, Panic(EadornedfilenametestingdllNullPointer));

	if (iString->Length() > 0)
		{
		iString->SetLength(iString->Length() - 1);
		}
	}


EXPORT_C const TPtrC Cadornedfilenametestingdll::ExampleFuncString() const
	{
	__ASSERT_ALWAYS(iString != NULL, Panic(EadornedfilenametestingdllNullPointer));
	return *iString;
	}



