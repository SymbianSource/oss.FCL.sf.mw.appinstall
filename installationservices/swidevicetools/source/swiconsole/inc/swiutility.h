/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* Some utility functions needed by SWI. SwiUtility class is implemented
* as a static class.
* @internalComponent
*
*/



#ifndef __SWI_UTILITY_H_ 
#define __SWI_UTILITY_H_ 

#include <e32base.h>
#include <e32std.h>
#include <e32const.h>

class SwiUtility
	{
	public:
 		static TLanguage GetLanguageType(const TDesC& aLanguage);
 		static void FillArray(RArray<TInt>& aArray, TInt aValue);
 	};
 	
 
 /** 
  * Template class CleanupReset to clean up the array
  * of implementation information from the cleanup stack.
  */

template <class T>
class CleanupReset
	{
public:
	/**
	 * Puts an item on the cleanup stack.
     *
	 * @param  aRef 
	 * The implementation information to be put on the cleanup stack.
	 */
	inline static void PushL(T& aRef);

private:
	static void Reset(TAny *aPtr);
	};


// Global interface function which calls CleanupReset class' PushL function
template <class T>
inline void CleanupResetPushL(T& aRef)
	{
	CleanupReset<T>::PushL(aRef);
	}

// This is the actual implementation of CleanupReset's Reset function
// This function creates object of TCleanupItem and and pushes it onto the
// cleanup stack.
template <class T>
inline void CleanupReset<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&Reset,&aRef));
	}

template <class T>
void CleanupReset<T>::Reset(TAny *aPtr)
	{
	  static_cast<T*>(aPtr)->Reset();
	}

#endif //__SWI_UTILITY_H_ 