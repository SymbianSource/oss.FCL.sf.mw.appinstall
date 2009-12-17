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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* used for smart pointers. See CElement.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __ACTUALCOUNTERBASE_H__
#define __ACTUALCOUNTERBASE_H__


#include <memory>

#include "counterbase.h"




template <class P> class CActualCounterBase: public CCounterBase
	{
private:
    P iPtr; // copy constructor must not throw

    CActualCounterBase (CActualCounterBase const&);
    CActualCounterBase & operator= (CActualCounterBase const&);
    typedef CActualCounterBase<P> this_type;

public:
    CActualCounterBase (P p);
    virtual void Dispose ();
    void* operator new (size_t);
    void operator delete (void* ap);
	};




template <class P> inline
		CActualCounterBase <P>::CActualCounterBase (P p) :
			iPtr (p)		
	{
	}

template <class P> void CActualCounterBase <P>::Dispose ()					
	{ 
	delete iPtr; 
	}

template <class P> void* CActualCounterBase <P>::operator new (size_t)
	{
	return std::allocator<this_type> ().allocate (1, static_cast <this_type*> (0));
	}

template <class P> void CActualCounterBase <P>::operator delete (void* ap)
	{
	std::allocator<this_type> ().deallocate (static_cast <this_type*> (ap), 1);
	}

#endif // __ACTUALCOUNTERBASE_H__
