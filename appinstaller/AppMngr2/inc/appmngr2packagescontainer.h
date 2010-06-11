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
* Description:   Container for list view that shows installation files
*
*/


#ifndef C_APPMNGR2PACKAGESCONTAINER_H
#define C_APPMNGR2PACKAGESCONTAINER_H

#include "appmngr2listcontainer.h"      // CAppMngr2ListContainer

class CAppMngr2PackagesView;


class CAppMngr2PackagesContainer : public CAppMngr2ListContainer
    {
public:     // constructor and destructor
    static CAppMngr2PackagesContainer* NewL( CAppMngr2PackagesView& aView );
    ~CAppMngr2PackagesContainer();
    
public:     // from CCoeControl (via CAppMngr2ListContainer)
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void GetHelpContext( TCoeHelpContext& aContext ) const;

public:     // from MEikListBoxObserver (via CAppMngr2ListContainer)
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
public:     // from CAppMngr2ListContainer
    CAppMngr2InfoBase& CurrentItem() const;
    TInt ListEmptyTextResourceId() const;

protected:  // from CAppMngr2ListContainer
    TInt ItemCount() const;
    CAppMngr2InfoBase& ItemInfo( TInt aIndex );

private:    // new functions
    CAppMngr2PackagesContainer( CAppMngr2PackagesView& aView );
    void ConstructL( CAppMngr2PackagesView& aView );
    };

#endif  // C_APPMNGR2PACKAGESCONTAINER_H

