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
* Description:   Creates AppInfo objects asynchronously
*
*/


#ifndef C_APPMNGR2APPINFOMAKER_H
#define C_APPMNGR2APPINFOMAKER_H

#include "appmngr2infomaker.h"          // CAppMngr2InfoMaker

class CAppMngr2Runtime;
class CAppMngr2AppInfo;


class CAppMngr2AppInfoMaker : public CAppMngr2InfoMaker
    {
public:     // constructor and destructor
    static CAppMngr2AppInfoMaker* NewL( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    static CAppMngr2AppInfoMaker* NewLC( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    ~CAppMngr2AppInfoMaker();
    
public:     // from CActive, via CAppMngr2InfoMaker
    void DoCancel();
    void RunL();

public:     // new functions
    void StartGettingInstalledAppsL();

private:    // new functions
    CAppMngr2AppInfoMaker( CAppMngr2Runtime& aPlugin,
            MAppMngr2InfoMakerObserver& aObserver, RFs& aFs );
    void ConstructL();

private:    // data
    RPointerArray<CAppMngr2AppInfo> iAppInfos;
    };

#endif  // C_APPMNGR2APPINFOMAKER_H

