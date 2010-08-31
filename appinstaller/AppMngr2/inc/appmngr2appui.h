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
* Description:   Declares UI class for application
*
*/


#ifndef C_APPMNGR2APPUI_H
#define C_APPMNGR2APPUI_H

#include <aknViewAppUi.h>               // CAknViewAppUi
#include "appmngr2modelobserver.h"      // MAppMngr2ModelObserver

class CAppMngr2Model;
class CIdle;


class CAppMngr2AppUi : public CAknViewAppUi, public MAppMngr2ModelObserver
    {
public:     // constructor and destructor
    void ConstructL();
    ~CAppMngr2AppUi();

public:     // new functions
    CAppMngr2Model& Model() const;

public:     // from MAppMngr2ModelObserver
    void InstalledAppsChanged( TInt aMoreRefreshesExpected );
    void InstallationFilesChanged( TInt aMoreRefreshesExpected );
    TBool InstalledAppsDisplayed();
    TBool InstallationFilesDisplayed();
    
private:    // new functions
    static TInt DelayedConstructL( TAny* aSelf );
    
private:    // from CAknViewAppUi
    void HandleCommandL( TInt aCommand );

private:    // data
    CAppMngr2Model* iModel;
    TInt iResourceFileOffset;
    CIdle* iIdle;
    enum TDelayedConstructionStep
        {
        EFirstStep,
        ESecondStep,
        EAllDone
        } iDelayedConstructionStep;
    TBool iConstructInstallationFilesFirst;
    };

#endif  // C_APPMNGR2APPUI_H

