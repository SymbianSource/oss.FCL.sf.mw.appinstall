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
* Description:   Container for list view that shows installation files
*
*/


#include "appmngr2packagescontainer.h"  // CAppMngr2PackagesContainer
#include "appmngr2packagesview.h"       // CAppMngr2PackagesView
#include "appmngr2model.h"              // CAppMngr2Model
#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo
#include <appmngr2common.hrh>           // Command IDs
#include <eiktxlbm.h>                   // CTextListBoxModel
#include <aknlists.h>                   // CAknDoubleLargeStyleListBox
#include <appmngr2.rsg>                 // Resource IDs
#include <csxhelp/am.hlp.hrh>           // Help contexts


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesContainer* CAppMngr2PackagesContainer::NewL( CAppMngr2PackagesView& aView )
    {
    CAppMngr2PackagesContainer* self = new (ELeave) CAppMngr2PackagesContainer( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aView );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::~CAppMngr2PackagesContainer
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesContainer::~CAppMngr2PackagesContainer()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMngr2PackagesContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    switch( aKeyEvent.iCode )
        {
        case EKeyOK:    // Middle softkey
            HandleGenericCommandL( EAppMngr2CmdInstall );
            break;
            
        case EKeyBackspace:
            HandleGenericCommandL( EAppMngr2CmdRemove );
            break;

        default:
            response = CAppMngr2ListContainer::OfferKeyEventL( aKeyEvent, aType );
            break;
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::GetHelpContext()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };
    aContext.iMajor = KAppMngr2AppUid;
    aContext.iContext = KAM_HLP_NOT_INSTALLED;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesContainer::HandleListBoxEventL( CEikListBox* aListBox,
        TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventItemSingleClicked:
        case EEventEnterKeyPressed:
            HandleGenericCommandL( EAppMngr2CmdInstall );
            break;

        default:
            CAppMngr2ListContainer::HandleListBoxEventL( aListBox, aEventType );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::CurrentItem()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase& CAppMngr2PackagesContainer::CurrentItem() const
    {
    // panics if list box is empty
    return Model().PackageInfo( iListBox->CurrentItemIndex() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::ListEmptyTextResourceId()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2PackagesContainer::ListEmptyTextResourceId() const
    {
    return R_AM_PACKAGES_VIEW_EMPTY;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::ItemCount()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2PackagesContainer::ItemCount() const
    {
    return Model().PackageInfoCount();
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::ItemInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase& CAppMngr2PackagesContainer::ItemInfo( TInt aIndex )
    {
    return Model().PackageInfo( aIndex );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::CAppMngr2PackagesContainer
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesContainer::CAppMngr2PackagesContainer( CAppMngr2PackagesView& aView )
        : CAppMngr2ListContainer( aView )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesContainer::ConstructL( CAppMngr2PackagesView& aView )
    {
    SetMopParent( &aView );
    CAppMngr2ListContainer::ConstructL( aView.ClientRect() );
    }

