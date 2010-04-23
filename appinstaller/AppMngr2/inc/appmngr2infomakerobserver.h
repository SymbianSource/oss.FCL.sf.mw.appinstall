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
* Description:   Creates info objects asynchronously
*
*/


#ifndef C_APPMNGR2INFOMAKEROBSERVER_H
#define C_APPMNGR2INFOMAKEROBSERVER_H

class CAppMngr2InfoMaker;
class CAppMngr2AppInfo;
class CAppMngr2PackageInfo;


class MAppMngr2InfoMakerObserver
    {
public:
    virtual void NewAppsCreatedL( const CAppMngr2InfoMaker& aMaker,
            RPointerArray<CAppMngr2AppInfo>& aAppInfos ) = 0;
    virtual void ErrorInCreatingAppsL( const CAppMngr2InfoMaker& aMaker,
            TInt aError ) = 0;
    virtual void NewPackagesCreatedL( const CAppMngr2InfoMaker& aMaker,
            RPointerArray<CAppMngr2PackageInfo>& aPackageInfos ) = 0;
    virtual void ErrorInCreatingPackagesL( const CAppMngr2InfoMaker& aMaker,
            TInt aError ) = 0;
    };

#endif  // C_APPMNGR2INFOMAKEROBSERVER_H

