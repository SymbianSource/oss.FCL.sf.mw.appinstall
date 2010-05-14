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
* Description:   List view that shows installation files
*
*/


#ifndef C_APPMNGR2PACKAGESVIEW_H
#define C_APPMNGR2PACKAGESVIEW_H

#include "appmngr2listview.h"           // CAppMngr2ListView
#include <appmngr2common.hrh>           // KAppMngr2PackagesViewIdValue

const TUid KPackagesViewId = { KAppMngr2PackagesViewIdValue };


class CAppMngr2PackagesView : public CAppMngr2ListView
    {
public:     // constructor and destructor
    static CAppMngr2PackagesView* NewL();
    ~CAppMngr2PackagesView();
    
public:     // from CAknView
    TUid Id() const;
    void HandleCommandL( TInt aCommand );
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

protected:  // from CAppMngr2ListView
    CAppMngr2ListContainer* CreateContainerL();
    void SetDefaultMiddleSoftkeyCommandL();
    void SetTitleL( CAknTitlePane& aTitlePane );

private:    // new functions
    CAppMngr2PackagesView();
    void ConstructL();
    };

#endif  // C_APPMNGR2PACKAGESVIEW_H

