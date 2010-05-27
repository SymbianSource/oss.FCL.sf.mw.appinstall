/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppInfo array used in Model
*
*/


#include "appmngr2appinfoarray.h"       // CAppMngr2AppInfoArray


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoArray::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoArray* CAppMngr2AppInfoArray::NewL( MAppMngr2InfoArrayObserver& aObserver )
    {
    CAppMngr2AppInfoArray* self = new (ELeave) CAppMngr2AppInfoArray( aObserver );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoArray::~CAppMngr2AppInfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoArray::~CAppMngr2AppInfoArray()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoArray::AddItemsInOrderL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppInfoArray::AddItemsInOrderL(
        RPointerArray<CAppMngr2AppInfo>& aAppInfos )
    {
    // move items from aAppInfos to iArray using alphabetical order
    TInt count = aAppInfos.Count();
    for( TInt index = count - 1; index >= 0; index-- )
        {
        AddItemInOrderL( aAppInfos[ index ] );  // takes ownership
        aAppInfos.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoArray::CAppMngr2AppInfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoArray::CAppMngr2AppInfoArray(
        MAppMngr2InfoArrayObserver& aObserver ) : CAppMngr2InfoArray( aObserver )
    {
    }

