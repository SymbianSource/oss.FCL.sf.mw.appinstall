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


#ifndef C_APPMNGR2APPINFOARRAY_H
#define C_APPMNGR2APPINFOARRAY_H

#include "appmngr2infoarray.h"          // CAppMngr2InfoArray
#include <appmngr2appinfo.h>            // CAppMngr2AppInfo


class CAppMngr2AppInfoArray : public CAppMngr2InfoArray
    {
public:     // constructor and destructor
    static CAppMngr2AppInfoArray* NewL( MAppMngr2InfoArrayObserver& aObserver );
    ~CAppMngr2AppInfoArray();

public:     // new functions
    void AddItemsInOrderL( RPointerArray<CAppMngr2AppInfo>& aAppInfos );

private:    // new functions
    CAppMngr2AppInfoArray( MAppMngr2InfoArrayObserver& aObserver );
    };

#endif  // C_APPMNGR2APPINFOARRAY_H

