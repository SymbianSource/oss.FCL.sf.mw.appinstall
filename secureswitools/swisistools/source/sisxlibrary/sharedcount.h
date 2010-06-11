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
* counter for smart ponters. See CElement.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SHAREDCOUNT_H__
#define __SHAREDCOUNT_H__


#include <functional>
#include "actualcounterbase.h"





class CSharedCount
	{
    friend inline bool operator == (CSharedCount const& aLeft, CSharedCount const& aRight);
    friend inline bool operator < (CSharedCount const& aLeft, CSharedCount const& aRight);

private:
    CCounterBase* iBase;

public:
    CSharedCount ();
    CSharedCount (CSharedCount const& aInitialiser);
    ~CSharedCount ();

    template <class P> CSharedCount (P aP) : iBase (0)
		{
		iBase = new CActualCounterBase <P> (aP);
		}

    CSharedCount & operator = (CSharedCount const& aInitialiser); 
    void swap (CSharedCount & asc); 
    long UseCount () const; 

	};



inline CSharedCount::CSharedCount () :
		iBase (0)
	{
	}

inline CSharedCount::~CSharedCount () 
	{
    if (iBase != NULL) 
		{
		iBase -> Release ();
		}
	}

inline CSharedCount::CSharedCount (CSharedCount const& aInitialiser) :
		iBase (aInitialiser.iBase) 
	{
    if (iBase != NULL) 
		{
		iBase -> AddRef ();
		}
	}

inline long CSharedCount::UseCount () const 
	{
	return (iBase != NULL) ? iBase -> UseCount (): 0;
	}

inline bool operator == (CSharedCount const& aLeft, CSharedCount const& aRight)
	{
    return aLeft.iBase == aRight.iBase;
	}

inline bool operator < (CSharedCount const& aLeft, CSharedCount const& aRight)
	{
    return std::less <CCounterBase*> () (aLeft.iBase, aRight.iBase);
	}


#endif // __SHAREDCOUNT_H__
