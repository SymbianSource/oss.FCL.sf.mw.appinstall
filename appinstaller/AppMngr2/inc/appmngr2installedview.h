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
* Description:   List view that shows installed applications
*
*/


#ifndef C_APPMNGR2INSTALLEDVIEW_H
#define C_APPMNGR2INSTALLEDVIEW_H

#include "appmngr2listview.h"           // CAppMngr2ListView
#include <appmngr2common.hrh>           // KAppMngr2InstalledViewIdValue

class CAppMngr2ListContainer;

const TUid KInstalledViewId = { KAppMngr2InstalledViewIdValue };


class CAppMngr2InstalledView : public CAppMngr2ListView
    {
public:     // constructor and destructor
    static CAppMngr2InstalledView* NewL();
    ~CAppMngr2InstalledView();
    
public:     // from CAknView
    TUid Id() const;
    void HandleCommandL( TInt aCommand );
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

protected:  // from CAppMngr2ListView
    CAppMngr2ListContainer* CreateContainerL();
    void SetDefaultMiddleSoftkeyCommandL();
    void SetTitleL( CAknTitlePane& aTitlePane );

private:    // new functions
    CAppMngr2InstalledView();
    void ConstructL();
    };

#endif  // C_APPMNGR2INSTALLEDVIEW_H

