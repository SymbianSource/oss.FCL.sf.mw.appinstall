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
* part of smart pointers. See CElement.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __COUNTERBASE_H__
#define __COUNTERBASE_H__

#include "utility_interface.h"
#include <assert.h>
#include "basetype.h"



class CCounterBase
	{
public:
    CCounterBase ();
    virtual ~CCounterBase ();
    virtual void Dispose () = 0; 
    virtual void Destruct ();
    void AddRef ();
    void Release (); 
    long UseCount () const;
private:
    CCounterBase (CCounterBase const&);
    CCounterBase& operator = (CCounterBase const&);
private:
    long iUseCount;
	};



inline CCounterBase::CCounterBase () : 
		iUseCount (1)
	{
	}

inline CCounterBase::~CCounterBase ()
	{ 
	}

inline void CCounterBase::Destruct ()
	{ 
	delete this; 
	}

inline void CCounterBase::AddRef ()
	{
    assert (iUseCount != 0);
	++iUseCount;
	}

inline void CCounterBase::Release () 
	{
	if (--iUseCount) 
		{
		return;
		}
    Dispose ();
    Destruct ();
	}

inline long CCounterBase::UseCount () const
	{ 
	return iUseCount; 
	}

#endif // __COUNTERBASE_H__
