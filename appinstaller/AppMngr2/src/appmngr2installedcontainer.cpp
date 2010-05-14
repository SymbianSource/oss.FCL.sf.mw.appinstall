/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Container for list view that shows installed applications
*
*/


#include "appmngr2installedcontainer.h" // CAppMngr2InstalledContainer
#include "appmngr2installedview.h"      // CAppMngr2InstalledView
#include "appmngr2model.h"              // CAppMngr2Model
#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <appmngr2common.hrh>           // Command IDs
#include <eiktxlbm.h>                   // CTextListBoxModel
#include <aknlists.h>                   // CAknDoubleLargeStyleListBox
#include <appmngr2.rsg>                 // Resource IDs
#include <csxhelp/am.hlp.hrh>           // Help contexts


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledContainer* CAppMngr2InstalledContainer::NewL(
        CAppMngr2InstalledView& aView )
    {
    CAppMngr2InstalledContainer* self = new (ELeave) CAppMngr2InstalledContainer( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aView );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::~CAppMngr2InstalledContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledContainer::~CAppMngr2InstalledContainer()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMngr2InstalledContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    switch( aKeyEvent.iCode )
        {
        case EKeyOK:    // Middle softkey
            HandleGenericCommandL( EAppMngr2CmdViewDetails );
            break;
            
        case EKeyBackspace:
            HandleGenericCommandL( EAppMngr2CmdUninstall );
            break;

        default:
            response = CAppMngr2ListContainer::OfferKeyEventL( aKeyEvent, aType );
            break;
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::GetHelpContext()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };
    aContext.iMajor = KAppMngr2AppUid;
    aContext.iContext = KAM_HLP_INSTALLED;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledContainer::HandleListBoxEventL( CEikListBox* aListBox,
        TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventItemSingleClicked:
        case EEventEnterKeyPressed:
            HandleGenericCommandL( EAppMngr2CmdViewDetails );
            break;

        default:
            CAppMngr2ListContainer::HandleListBoxEventL( aListBox, aEventType );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::CurrentItem()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase& CAppMngr2InstalledContainer::CurrentItem() const
    {
    // panics if list box is empty
    return Model().AppInfo( iListBox->CurrentItemIndex() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::ListEmptyTextResourceId()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2InstalledContainer::ListEmptyTextResourceId() const
    {
    return R_AM_INSTALLED_VIEW_EMPTY;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::ItemCount()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2InstalledContainer::ItemCount() const
    {
    return Model().AppInfoCount();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::ItemInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase& CAppMngr2InstalledContainer::ItemInfo( TInt aIndex )
    {
    return Model().AppInfo( aIndex );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::CAppMngr2InstalledContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledContainer::CAppMngr2InstalledContainer( CAppMngr2InstalledView& aView )
        : CAppMngr2ListContainer( aView )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledContainer::ConstructL( CAppMngr2InstalledView& aView )
    {
    SetMopParent( &aView );
    CAppMngr2ListContainer::ConstructL( aView.ClientRect() );
    }

