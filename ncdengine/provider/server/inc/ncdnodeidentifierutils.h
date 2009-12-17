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
* Description:  
*
*/


#include <e32def.h>
#include <e32cmn.h>

class CNcdNodeIdentifier;

namespace NcdNodeIdentifierUtils 
    {
    /**
     * Tells whether the given array contains an equal identifier
     * to the given one.
     */    
    TBool ContainsIdentifier(
        const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdNodeIdentifier>& aArray );
    
    /**
     * Returns the index of the equal identifier in the array.
     *
     * @return KErrNotFound If there is no equal indentifier in the
     * array.
     */    
    TInt IdentifierIndex(
        const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdNodeIdentifier>& aArray );
    }
