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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "sharedcount.h"



CSharedCount & CSharedCount::operator = (CSharedCount const& aInitialiser) 
	{
    CCounterBase* tmp = aInitialiser.iBase;
    if (tmp != 0) 
		{
		tmp -> AddRef ();
		}
    if (iBase != 0) 
		{
		iBase -> Release ();
		}
    iBase = tmp;
    return *this;
	}


void CSharedCount::swap (CSharedCount & aSwap) 
	{
    CCounterBase* tmp = aSwap.iBase;
    aSwap.iBase = iBase;
    iBase = tmp;
	}

