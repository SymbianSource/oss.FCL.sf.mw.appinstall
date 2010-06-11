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
* Description:   Base class definitions for AppMng2 list view containers
*
*/


#ifndef C_APPMNGR2LISTCONTAINER_H
#define C_APPMNGR2LISTCONTAINER_H

#include <coecntrl.h>                   // CCoeControl
#include <eiklbo.h>                     // MEikListBoxObserver

class CAppMngr2Model;
class CAppMngr2InfoBase;
class CAppMngr2ListView;
class CAknDoubleLargeStyleListBox;
class CTextListBoxModel;
class CGulIcon;


class CAppMngr2ListContainer : public CCoeControl, public MEikListBoxObserver
    {
public:     // constructor and destructor
    CAppMngr2ListContainer( CAppMngr2ListView& aView );
    ~CAppMngr2ListContainer();
    
public:     // from CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandleResourceChange( TInt aType );
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

public:     // from MEikListBoxObserver
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

public:     // new functions
    TBool IsListEmpty() const;
    void HandleGenericCommandL( TInt aCommand );
    void RefreshL( TBool aPreserveSelectedItem, TBool& aSelectedItemChanged,
            TInt aMoreRefreshesExpected );
    virtual CAppMngr2InfoBase& CurrentItem() const = 0;
    virtual TInt ListEmptyTextResourceId() const = 0;
    
protected:  // new functions
    virtual TInt ItemCount() const = 0;
    virtual CAppMngr2InfoBase& ItemInfo( TInt aIndex ) = 0;
    void ConstructL( const TRect& aRect );
    CAppMngr2Model& Model() const;

private:    // new functions
    void LoadIconsL();
    void CreateItemArrayL();
    void PopulateItemArrayL();
    void DeleteItemSpecificIcons( CArrayPtr<CGulIcon>& aIconArray );
    void SetEmptyTextL( TInt aMoreRefreshesExpected );

protected:  // data
    CAppMngr2ListView& iView;
    CAknDoubleLargeStyleListBox* iListBox;
    CDesCArrayFlat* iItemArray;
    TInt iItemSpecificIcons;
    };

#endif  // C_APPMNGR2LISTCONTAINER_H

