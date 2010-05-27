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
* Description:   PackageInfo array used in Model
*
*/


#include "appmngr2packageinfoarray.h"   // CAppMngr2PackageInfoArray


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoArray::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoArray* CAppMngr2PackageInfoArray::NewL(
        MAppMngr2InfoArrayObserver& aObserver )
    {
    CAppMngr2PackageInfoArray* self = new (ELeave) CAppMngr2PackageInfoArray( aObserver );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoArray::~CAppMngr2PackageInfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoArray::~CAppMngr2PackageInfoArray()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoArray::AddItemsInOrderL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoArray::AddItemsInOrderL(
        RPointerArray<CAppMngr2PackageInfo>& aPackageInfos )
    {
    // move items from aAppInfos to iArray using alphabetical order
    TInt count = aPackageInfos.Count();
    for( TInt index = count - 1; index >= 0; index-- )
        {
        AddItemInOrderL( aPackageInfos[ index ] );  // takes ownership
        aPackageInfos.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoArray::CAppMngr2PackageInfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoArray::CAppMngr2PackageInfoArray(
        MAppMngr2InfoArrayObserver& aObserver ) :  CAppMngr2InfoArray( aObserver )
    {
    }

