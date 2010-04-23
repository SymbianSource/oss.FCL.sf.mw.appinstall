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


#include "ncdnodeidentifierutils.h"
#include "ncdnodeidentifier.h"


TBool NcdNodeIdentifierUtils::ContainsIdentifier(
    const CNcdNodeIdentifier& aIdentifier,
    const RPointerArray<CNcdNodeIdentifier>& aArray )
    {
    TInt count = aArray.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( aArray[ i ]->Equals( aIdentifier ) ) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
TInt NcdNodeIdentifierUtils::IdentifierIndex(
    const CNcdNodeIdentifier& aIdentifier,
    const RPointerArray<CNcdNodeIdentifier>& aArray )
    {
    TInt count = aArray.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        if ( aArray[ i ]->Equals( aIdentifier ) ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }
