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


#include "appmngr2packagesview.h"       // CAppMngr2PackagesView
#include "appmngr2packagescontainer.h"  // CAppMngr2PackagesContainer
#include <appmngr2infobase.h>           // CAppMngr2InfoBase
#include <appmngr2debugutils.h>         // FLOG macros
#include <eikmenub.h>                   // CEikMenuPane
#include <akntitle.h>                   // CAknTitlePane
#include <StringLoader.h>               // StringLoader
#include <appmngr2common.hrh>           // Generic command IDs
#include <avkon.hrh>                    // EAknSoftkeyOk
#include <appmngr2.rsg>                 // Resource IDs


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesView* CAppMngr2PackagesView::NewL()
    {
    CAppMngr2PackagesView* self = new (ELeave) CAppMngr2PackagesView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::~CAppMngr2PackagesView
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesView::~CAppMngr2PackagesView()
    {
    FLOG( "CAppMngr2PackagesView::~CAppMngr2PackagesView" );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::Id()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2PackagesView::Id() const
    {
    return KPackagesViewId;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesView::HandleCommandL( TInt aCommand )
    {
    FLOG( "CAppMngr2PackagesView::HandleCommandL( %d )", aCommand );
    switch( aCommand )
        {
        case EAknSoftkeyOk:
            iContainer->HandleGenericCommandL( EAppMngr2CmdInstall );
            break;

        default:
            CAppMngr2ListView::HandleCommandL( aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesView::DynInitMenuPaneL( TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    CAppMngr2ListView::DynInitMenuPaneL( aResourceId, aMenuPane );
    
    if( aResourceId == R_APPMNGR2_PACKAGES_MENU )
        {
        if( iContainer == NULL || iContainer->IsListEmpty() )
            {
            aMenuPane->SetItemDimmed( EAppMngr2CmdInstall, ETrue );
            aMenuPane->SetItemDimmed( EAppMngr2CmdViewDetails, ETrue );
            aMenuPane->SetItemDimmed( EAppMngr2CmdRemove, ETrue );
            }
        else
            {
            CAppMngr2InfoBase& currentItem = iContainer->CurrentItem();
            if( !currentItem.SupportsGenericCommand( EAppMngr2CmdInstall ) )
                {
                aMenuPane->SetItemDimmed( EAppMngr2CmdInstall, ETrue );
                }
            if( !currentItem.SupportsGenericCommand( EAppMngr2CmdViewDetails ) )
                {
                aMenuPane->SetItemDimmed( EAppMngr2CmdViewDetails, ETrue );
                }
            if( !currentItem.SupportsGenericCommand( EAppMngr2CmdRemove ) )
                {
                aMenuPane->SetItemDimmed( EAppMngr2CmdRemove, ETrue );
                }
            AddDynamicMenuItemsL( currentItem, aMenuPane );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::CreateContainerL()
// ---------------------------------------------------------------------------
//
CAppMngr2ListContainer* CAppMngr2PackagesView::CreateContainerL()
    {
    return CAppMngr2PackagesContainer::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::SetDefaultMiddleSoftkeyCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesView::SetDefaultMiddleSoftkeyCommandL()
    {
    if( iContainer && !iContainer->IsListEmpty() )
        {
        SetMiddleSoftkeyCommandL( R_AM_MSK_INSTALL, EAppMngr2CmdInstall );
        }
    // else the list is empty, and also the MSK is empty
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::SetTitleL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesView::SetTitleL( CAknTitlePane& aTitlePane )
    {
    HBufC* title = StringLoader::LoadLC( R_AM_TITLE_NOT_INSTALLED );
    aTitlePane.SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::CAppMngr2PackagesView()
// ---------------------------------------------------------------------------
//
CAppMngr2PackagesView::CAppMngr2PackagesView() : CAppMngr2ListView()
    {
    FLOG( "CAppMngr2PackagesView::CAppMngr2PackagesView" );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackagesView::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackagesView::ConstructL()
    {
    CAppMngr2ListView::ConstructL( R_APPMNGR2_PACKAGES_VIEW );
    }

