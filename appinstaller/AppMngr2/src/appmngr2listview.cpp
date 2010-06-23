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
* Description:   Base class for different AppMngr2 list views
*
*/


#include "appmngr2listview.h"           // CAppMngr2ListView
#include "appmngr2listcontainer.h"      // CAppMngr2ListContainer
#include "appmngr2appui.h"              // CAppMngr2AppUi
#include "appmngr2model.h"              // CAppMngr2Model
#include "appmngr2log.h"                // CAppMngr2Log
#include "appmngr2.hrh"                 // Command IDs
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2infobase.h>           // CAppMngr2InfoBase
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2common.hrh>           // generic command ids
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <featmgr.h>                    // FeatureManager
#include <bautils.h>                    // BaflUtils
#include <eikenv.h>                     // CEikonEnv
#include <aknViewAppUi.h>               // CAknViewAppUi
#include <eikmenub.h>                   // CEikMenuBar, CEikMenuPane
#include <eikmenup.h>                   // CEikMenuPaneItem
#include <akntitle.h>                   // CAknTitlePane
#include <StringLoader.h>               // StringLoader
#include <AknInfoPopupNoteController.h> // CAknInfoPopupNoteController
#include <appmngr2.rsg>                 // Resource IDs

const TUid KStatusPaneUid = { EEikStatusPaneUidTitle };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2ListView::ConstructL() 
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::ConstructL( TInt aResourceId )
    {
    FLOG( "CAppMngr2ListView::ConstructL( 0x%08x )", aResourceId );
    
    BaseConstructL( aResourceId );
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::~CAppMngr2ListView() 
// ---------------------------------------------------------------------------
//
CAppMngr2ListView::~CAppMngr2ListView()
    {
    FLOG( "CAppMngr2ListView::~CAppMngr2ListView" );
    
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    delete iInfoPopup;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::RefreshL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::RefreshL( TInt aMoreRefreshesExpected )
    {
    if( iContainer )
        {
        TBool selectedItemChanged = EFalse;
        iContainer->RefreshL( iMaintainFocus, selectedItemChanged, aMoreRefreshesExpected );
        if( selectedItemChanged )
            {
            UpdateMiddleSoftkeyCommandL();
            StopDisplayingMenuBar();
            iMaintainFocus = EFalse;
            }

        // Close progress note used in application startup
        if( iInfoPopup && !aMoreRefreshesExpected )
            {
            iInfoPopup->HideInfoPopupNote();
            delete iInfoPopup;
            iInfoPopup = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::UpdateMiddleSoftkeyCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::UpdateMiddleSoftkeyCommandL()
    {
    FLOG_PERF_STATIC_BEGIN( UpdateMiddleSoftkeyCommandL );
    
    if( iContainer && !iContainer->IsListEmpty() )
        {
        // add item-specific MSK if the current item has one
        CAppMngr2InfoBase& currentItem = iContainer->CurrentItem();
        TInt resourceId = 0;
        TInt commandId = 0;
        currentItem.GetMiddleSoftkeyCommandL( resourceId, commandId );
        if( resourceId && commandId )
            {
            SetMiddleSoftkeyCommandL( resourceId, commandId );
            }
        else
            {
            SetDefaultMiddleSoftkeyCommandL();
            }
        }
    else
        {
        // list is empty, remove old MSK if it has been added and set the default
        CEikButtonGroupContainer* cba = Cba();
        if( iMiddleSoftkeyCommandId && cba )
            {
            cba->RemoveCommandFromStack( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    iMiddleSoftkeyCommandId );
            }
        SetDefaultMiddleSoftkeyCommandL();
        }
    
    FLOG_PERF_STATIC_END( UpdateMiddleSoftkeyCommandL )
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::CurrentItemSelectedByUser()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::CurrentItemSelectedByUser( TBool aMaintainFocus )
    {
    iMaintainFocus = aMaintainFocus;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::HandleViewRectChange()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::HandleViewRectChange()
    {
    if( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::HandleCommandL( TInt aCommand )
    {
    FLOG( "CAppMngr2ListView::HandleCommandL( %d )", aCommand );
    
    switch( aCommand )
        {
        case EAknSoftkeyBack:
            AppUi()->HandleCommandL( EAknSoftkeyExit );
            break;

        case EAppMngr2CmdLog:
            {
            CAppMngr2Log* log = CAppMngr2Log::NewLC();
            log->ShowInstallationLogL();
            CleanupStack::PopAndDestroy( log );
            }
            break;

        case EAknCmdHelp:
        case EAknCmdExit:
            AppUi()->HandleCommandL( aCommand );
            break;

        default:
            Model().HandleCommandL( iContainer->CurrentItem(), aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::DynInitMenuPaneL( TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    if( aResourceId == R_APPMNGR2_INSTALLED_MENU
            || aResourceId == R_APPMNGR2_PACKAGES_MENU )
        {
        if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) ) 
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }

        // Keep the currently selected item focused in forthcoming list refreshes. 
        iMaintainFocus = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::DoActivateL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    FLOG( "CAppMngr2ListView::DoActivateL, id 0x%08x", Id().iUid );
    
    CAknTitlePane* titlePane = NULL;
    titlePane = static_cast<CAknTitlePane*>( StatusPane()->ControlL( KStatusPaneUid ) );
    SetTitleL( *titlePane );
    
    if( iContainer == NULL )
        {
        iContainer = CreateContainerL();
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        iContainer->ActivateL();
        }
    UpdateMiddleSoftkeyCommandL();

    if( iInfoPopup == NULL )
        {
        iInfoPopup = CAknInfoPopupNoteController::NewL();
        iInfoPopup->SetTimePopupInView( 0 );
        iInfoPopup->HideWhenAppFaded( EFalse );
        HBufC* noteText = StringLoader::LoadLC( R_QTN_AM_SCANNING_MEMORY );
        iInfoPopup->SetTextL( *noteText );
        CleanupStack::PopAndDestroy( noteText );
        iInfoPopup->ShowInfoPopupNote();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::DoDeactivate()
    {
    FLOG( "CAppMngr2ListView::DoDeactivate, id 0x%08x", Id().iUid );
    
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    if( iInfoPopup )
        {
        delete iInfoPopup;
        iInfoPopup = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::Model()
// ---------------------------------------------------------------------------
//
CAppMngr2Model& CAppMngr2ListView::Model() const
    {
    return reinterpret_cast<CAppMngr2AppUi*>( iEikonEnv->EikAppUi() )->Model();
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::AddDynamicMenuItemsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::AddDynamicMenuItemsL( CAppMngr2InfoBase& aCurrentItem,
        CEikMenuPane* aMenuPane )
    {
    if( aMenuPane )
        {
        FLOG_PERF_STATIC_BEGIN( AddDynamicMenuItemsL );
        
        TInt position = 0;
        if( aMenuPane->MenuItemExists( EAppMngr2PlaceForPluginSpecificCmds, position ) )
            {
            aMenuPane->DeleteMenuItem( EAppMngr2PlaceForPluginSpecificCmds );
    
            RPointerArray<CEikMenuPaneItem::SData> menuItems;
            CleanupResetAndDestroyPushL( menuItems );

            TRAPD( err, aCurrentItem.GetMenuItemsL( menuItems ) );
            if( !err )
                {
                TInt count = menuItems.Count();
                for( TInt index = 0; index < count; index++ )
                    {
                    aMenuPane->InsertMenuItemL( *( menuItems[ index ] ), position++ );
                    aMenuPane->SetItemSpecific( menuItems[ index ]->iCommandId, ETrue );
                    }
                }

            CleanupStack::PopAndDestroy( &menuItems );
            }
        
        FLOG_PERF_STATIC_END( AddDynamicMenuItemsL )
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::SetMiddleSoftkeyCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListView::SetMiddleSoftkeyCommandL( TInt aResourceId, TInt aCommandId )
    {
    CEikButtonGroupContainer* cba = Cba();
    if( cba )
        {
        // remove old command from the stack if it is already added
        if( iMiddleSoftkeyCommandId )
            {
            cba->RemoveCommandFromStack( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    iMiddleSoftkeyCommandId );
            }
        HBufC* middleSoftkeyLabel = StringLoader::LoadLC( aResourceId );
        cba->AddCommandToStackL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                aCommandId, *middleSoftkeyLabel );
        CleanupStack::PopAndDestroy( middleSoftkeyLabel );
        iMiddleSoftkeyCommandId = aCommandId;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListView::CAppMngr2ListView()
// ---------------------------------------------------------------------------
//
CAppMngr2ListView::CAppMngr2ListView()
    {
    }

