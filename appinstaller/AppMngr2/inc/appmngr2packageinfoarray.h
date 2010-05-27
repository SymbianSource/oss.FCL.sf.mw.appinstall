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


#ifndef C_APPMNGR2PACKAGEINFOARRAY_H
#define C_APPMNGR2PACKAGEINFOARRAY_H

#include "appmngr2infoarray.h"          // CAppMngr2InfoArray
#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo


class CAppMngr2PackageInfoArray : public CAppMngr2InfoArray
    {
public:     // constructor and destructor
    static CAppMngr2PackageInfoArray* NewL( MAppMngr2InfoArrayObserver& aObserver ); 
    ~CAppMngr2PackageInfoArray();

public:     // new functions
    void AddItemsInOrderL( RPointerArray<CAppMngr2PackageInfo>& aPackageInfos );

private:    // new functions
    CAppMngr2PackageInfoArray( MAppMngr2InfoArrayObserver& aObserver );
    };

#endif  // C_APPMNGR2PACKAGEINFOARRAY_H

