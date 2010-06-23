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


#include "appmngr2installedview.h"      // CAppMngr2InstalledView
#include "appmngr2installedcontainer.h" // CAppMngr2InstalledContainer
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
// CAppMngr2InstalledView::NewL
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledView* CAppMngr2InstalledView::NewL()
    {
    CAppMngr2InstalledView* self = new (ELeave) CAppMngr2InstalledView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::~CAppMngr2InstalledView
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledView::~CAppMngr2InstalledView()
    {
    FLOG( "CAppMngr2InstalledView::~CAppMngr2InstalledView" );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::Id()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2InstalledView::Id() const
    {
    return KInstalledViewId;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledView::HandleCommandL( TInt aCommand )
    {
    FLOG( "CAppMngr2InstalledView::HandleCommandL( %d )", aCommand );
    switch( aCommand )
        {
        case EAknSoftkeyOk:
            iContainer->HandleGenericCommandL( EAppMngr2CmdViewDetails );
            break;

        default:
            CAppMngr2ListView::HandleCommandL( aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledView::DynInitMenuPaneL( TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    CAppMngr2ListView::DynInitMenuPaneL( aResourceId, aMenuPane );
    
    if( aResourceId == R_APPMNGR2_INSTALLED_MENU )
        {
        if( iContainer == NULL || iContainer->IsListEmpty() )
            {
            aMenuPane->SetItemDimmed( EAppMngr2CmdViewDetails, ETrue );
            aMenuPane->SetItemDimmed( EAppMngr2CmdUninstall, ETrue );
            }
        else
            {
            CAppMngr2InfoBase& currentItem = iContainer->CurrentItem();
            if( !currentItem.SupportsGenericCommand( EAppMngr2CmdViewDetails ) )
                {
                aMenuPane->SetItemDimmed( EAppMngr2CmdViewDetails, ETrue );
                }
            if( !currentItem.SupportsGenericCommand( EAppMngr2CmdUninstall ) )
                {
                aMenuPane->SetItemDimmed( EAppMngr2CmdViewDetails, ETrue );
                }
            AddDynamicMenuItemsL( currentItem, aMenuPane );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::CreateContainerL()
// ---------------------------------------------------------------------------
//
CAppMngr2ListContainer* CAppMngr2InstalledView::CreateContainerL()
    {
    return CAppMngr2InstalledContainer::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::SetDefaultMiddleSoftkeyCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledView::SetDefaultMiddleSoftkeyCommandL()
    {
    if( iContainer && !iContainer->IsListEmpty() )
        {
        SetMiddleSoftkeyCommandL( R_AM_MSK_VIEW_DETAILS, EAppMngr2CmdViewDetails );
        }
    // else the list is empty, and also the MSK is empty
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::SetTitleL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledView::SetTitleL( CAknTitlePane& aTitlePane )
    {
    HBufC* title = StringLoader::LoadLC( R_AM_TITLE_INSTALLED );
    aTitlePane.SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::CAppMngr2InstalledView()
// ---------------------------------------------------------------------------
//
CAppMngr2InstalledView::CAppMngr2InstalledView() : CAppMngr2ListView()
    {
    FLOG( "CAppMngr2InstalledView::CAppMngr2InstalledView" );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InstalledView::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InstalledView::ConstructL()
    {
    CAppMngr2ListView::ConstructL( R_APPMNGR2_INSTALLED_VIEW );
    }

