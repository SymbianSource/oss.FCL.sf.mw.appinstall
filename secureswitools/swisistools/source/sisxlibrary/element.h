/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* smart pointers; used as elements in SIS arrays and elsewhere
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <string>
#include <iostream>

#include "sharedcount.h"





template<class T> struct shared_ptr_traits
	{
	typedef T& reference;
	};


template<> struct shared_ptr_traits<void>
	{
	typedef void reference;
	};




template <class T> class CElement

	{
public:
	typedef T*											TPtr;
	typedef typename shared_ptr_traits<T>::reference	TRef;

	CElement ();

	template <class Y> explicit CElement (Y* ay) :
			iPtr (ay), 
			iCount (ay)									{ } 

	template <class Y>
	CElement& operator = (CElement<Y> const& r)
		{
		iPtr = r.iPtr;
		iCount = r.iCount;
		return *this;
		}

	template <class Y> void reset (Y* ay)
		{
		assert (ay == 0 || ay != iPtr);	// can't self reset
		this_type (ay).swap (*this);
		}

	template <class Y>
	bool _internal_less (CElement<Y> const& rhs) const
		{
		return iCount < rhs.iCount;
		}
	void reset ();
	TRef operator* () const
		{
		assert (iPtr != NULL);
		return *iPtr;
		}
	T* operator -> () const;
	T* get () const;
	bool operator! () const;
	long UseCount () const;
	void swap (CElement<T>& aOther);

	void Verify (const TUint32 aLanguages) const;
	TUint32 ByteCount (const bool aInArray) const;
	bool WasteOfSpace () const;
	void MakeNeat ();
	std::string Name () const;
	void Dump (std::ostream& aStream, const int aLevel) const;
	TUint32 ByteCountWithHeader (const bool aInArray) const;
	void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	std::string NoteIfOptional () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	void SkipOldWriteNew (TSISStream& aFile) const;

private:
	T*				iPtr;
	CSharedCount	iCount;
	};



template <class T> inline CElement <T>::CElement () :
		iPtr (NULL) 
	{ 
	}


template<class T> inline void CElement <T>::reset ()
	{ 
	this_type ().swap (*this);
	}


template<class T> T* CElement <T>::operator -> () const
	{
	assert (iPtr != NULL);
	return iPtr;
	}
			
template<class T> inline T* CElement <T>::get () const
	{ 
	return iPtr; 
	}

template<class T> inline bool CElement <T>::operator! () const
	{
	return iPtr == 0;
	}

template<class T> inline long CElement <T>::UseCount () const
	{ 
	return iCount.UseCount (); 
	}

template<class T> inline void CElement <T>::swap (CElement<T>& aOther)
	{
	std::swap (iPtr, aOther.iPtr);
	iCount.swap (aOther.iCount);
	}	

template<class T> inline
		void CElement <T>::Verify (const TUint32 aLanguages) const
	{ 
	iPtr -> Verify (aLanguages); 
	}

template<class T> inline TUint32 CElement <T>::ByteCount (const bool aInArray) const
	{ 
	return iPtr -> ByteCount (aInArray);
	}

template<class T> inline bool CElement <T>::WasteOfSpace () const
	{
	return iPtr -> WasteOfSpace ();
	}

template<class T> inline void CElement <T>::MakeNeat ()
	{
	iPtr -> MakeNeat (); 
	}

template<class T> inline std::string CElement <T>::Name () const
	{
	return iPtr -> Name (); 
	}

template<class T> inline
		void CElement <T>::Dump (std::ostream& aStream, const int aLevel) const
	{
	iPtr -> Dump (aStream, aLevel);
	}

template<class T> inline
		TUint32 CElement <T>::ByteCountWithHeader (const bool aInArray) const
	{
	return iPtr -> ByteCountWithHeader (aInArray);
	}

template <class T> inline
		void CElement <T>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	iPtr -> CalculateCrc (aCRC, aIsArrayElement);
	}

template <class T> inline std::string CElement <T>::NoteIfOptional () const
	{
	return iPtr -> NoteIfOptional ();
	}

#ifdef GENERATE_ERRORS
template <class T> inline void CElement <T>::CreateDefects ()
	{
	iPtr -> CreateDefects ();
	}
#endif // GENERATE_ERRORS

template <class T> inline void CElement <T>::SkipOldWriteNew (TSISStream& aFile) const
	{
	iPtr -> SkipOldWriteNew (aFile);
	}

template <class T, class U> inline bool operator == (CElement<T> const& a, CElement<U> const& b)
	{
	return a.get () == b.get ();
	}

template<class T, class U> inline bool operator != (CElement<T> const& a, CElement<U> const& b)
	{
	return a.get () != b.get ();
	}

template<class T, class U> inline bool operator < (CElement<T> const& a, CElement<U> const& b)
	{
	return a._internal_less (b);
	}

template<class T> inline void swap (CElement<T>& a, CElement<T>& b)
	{
	a.swap (b);
	}

template<class T> inline T* get_pointer (CElement<T> const& p)
	{
	return p.get ();
	}



#endif // __ELEMENT_H__
