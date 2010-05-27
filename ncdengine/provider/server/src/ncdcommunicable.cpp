/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Contains CNcdCommunicable class implementation
*
*/


#include "ncdcommunicable.h"
#include "catalogsdebug.h"


CNcdCommunicable::CNcdCommunicable()
: CCatalogsCommunicable()
    {
    DLTRACEIN((""));
    }
    
CNcdCommunicable::~CNcdCommunicable()
    {
    DLTRACEIN((""));
    }


void CNcdCommunicable::SetAsObsolete( TBool aObsolete )
    {
    DLTRACEIN(("aObsolete = %d, aObsolete"));
    iObsolete = aObsolete;
    }
 
TBool CNcdCommunicable::IsObsolete() const
    {
    DLTRACEIN(("IsObsolete = %d", iObsolete ));
    return iObsolete;
    }
