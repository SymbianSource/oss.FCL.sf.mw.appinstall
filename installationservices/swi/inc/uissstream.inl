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



using namespace StreamHelpers;

template <class T>
void StreamOutArrayL(const RArray<T>& aArray, RWriteStream& aStream)
	{
	StreamHelpers::StreamArrayHelper<T, TClassAllowable<T>::Allow>::StreamOutArrayL(aArray, aStream);
	}

template <class T>
void StreamInArrayL(RArray<T>& aArray, RReadStream& aStream)
	{
	StreamHelpers::StreamArrayHelper<T, TClassAllowable<T>::Allow>::StreamInArrayL(aArray, aStream);
	}

// Helper functions

template <class T>
void StreamArrayHelper<T, ETrue>::StreamOutArrayL(const RArray<T>& aArray, RWriteStream& aStream)
	{
	TInt32 count = aArray.Count();
	aStream.WriteInt32L(count);

	for (TInt32 i = 0; i < count; ++i)
		{
		TPckgC<T> package(aArray[i]);
		aStream.WriteL(package);
		}
	}

template <class T>
void StreamArrayHelper<T, ETrue>::StreamInArrayL(RArray<T>& aArray, RReadStream& aStream)
	{
	aArray.Reset();
	
	TInt32 count = aStream.ReadInt32L();

	for (TInt32 i = 0; i < count; ++i)
		{
		T entry;	// T is always a TType so this is allowed
		TPckg<T> package(entry);
		aStream.ReadL(package);
		User::LeaveIfError(aArray.Append(entry));
		}
	}
