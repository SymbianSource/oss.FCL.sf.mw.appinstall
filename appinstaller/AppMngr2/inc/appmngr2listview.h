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
* Description:   Base class definitions for different AppMngr2 list views
*
*/


#ifndef C_APPMNGR2LISTVIEW_H
#define C_APPMNGR2LISTVIEW_H

#include <aknview.h>                    // CAknView

class CAppMngr2ListContainer;
class CAppMngr2Model;
class CAppMngr2InfoBase;
class CAknTitlePane;
class CAknWaitDialog;
class CAknInfoPopupNoteController;


class CAppMngr2ListView : public CAknView
    {
public:     // constructor and destructor
    void ConstructL( TInt aResourceId );
    ~CAppMngr2ListView();

public:     // new functions
    void RefreshL( TInt aMoreRefreshesExpected );
    void UpdateMiddleSoftkeyCommandL();
    void CurrentItemSelectedByUser( TBool aMaintainFocus );

public:     // from CAknView
    void HandleViewRectChange();
    void HandleCommandL( TInt aCommand );
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

protected:  // from CAknView
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid aCustomMessageId,
                      const TDesC8& aCustomMessage );
    void DoDeactivate();

protected:  // new functions
    virtual CAppMngr2ListContainer* CreateContainerL() = 0;
    virtual void SetDefaultMiddleSoftkeyCommandL() = 0;
    virtual void SetTitleL( CAknTitlePane& aTitlePane ) = 0;

protected:  // new functions
    CAppMngr2Model& Model() const;
    void AddDynamicMenuItemsL( CAppMngr2InfoBase& aCurrentItem,
            CEikMenuPane* aMenuPane );
    void SetMiddleSoftkeyCommandL( TInt aResourceId, TInt aCommandId );
    
protected:  // constructor
    CAppMngr2ListView();

protected:  // data
    CAppMngr2ListContainer* iContainer;

private:    // data
    TInt iMiddleSoftkeyCommandId;
    CAknInfoPopupNoteController* iInfoPopup;
    TBool iMaintainFocus;
    };

#endif  // C_APPMNGR2LISTVIEW_H

