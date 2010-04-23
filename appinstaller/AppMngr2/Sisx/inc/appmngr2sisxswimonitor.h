/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility class to listen SWI operations.
*
*/


#ifndef C_APPMNGR2SISXSWIMONITOR_H
#define C_APPMNGR2SISXSWIMONITOR_H

#include <e32base.h>                    // CActive
#include <e32property.h>                // RProperty
#include <appmngr2runtimeobserver.h>    // MAppMngr2RuntimeObserver


/**
 * Class for listening SWI operations. When a software install operation
 * (installation, uninstallation or backup restore) completes successfully,
 * this class calls MAppMngr2RuntimeObserver::RefreshInstalledApps() to
 * refresh the displayed list of installed applications.
 */
class CAppMngr2SisxSwiMonitor : public CActive
    {
public:     // constructors and destructor
    static CAppMngr2SisxSwiMonitor* NewL( MAppMngr2RuntimeObserver& aObs );
    ~CAppMngr2SisxSwiMonitor();

protected:  // from CActive
    void DoCancel();
    void RunL();
    
private:    // new functions
    CAppMngr2SisxSwiMonitor( MAppMngr2RuntimeObserver& aObs );
    void ConstructL();

private:     // data
    MAppMngr2RuntimeObserver& iObs;
    RProperty iSwInstallKey;
    };

#endif      // C_APPMNGR2SISXSWIMONITOR_H

