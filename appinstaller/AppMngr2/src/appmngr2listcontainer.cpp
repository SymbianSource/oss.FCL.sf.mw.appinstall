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
* Description:   Base class for AppMngr2 list view containers
*
*/


#include "appmngr2listcontainer.h"      // CAppMngr2ListContainer
#include "appmngr2appui.h"              // CAppMngr2AppUi
#include "appmngr2model.h"              // CAppMngr2Model
#include "appmngr2listview.h"           // CAppMngr2ListView
#include "appmngr2.hrh"                 // Icon IDs
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2infobase.h>           // CAppMngr2InfoBase
#include <appmngr2debugutils.h>         // FLOG macros
#include <aknlists.h>                   // CAknDoubleLargeStyleListBox
#include <eiktxlbm.h>                   // CTextListBoxModel
#include <AknIconArray.h>               // CAknIconArray
#include <StringLoader.h>               // StringLoader
#include <gulicon.h>                    // CGulIcon

const TInt KGranularity = 8;
const TInt KSpaceForTabsAndIconIndexes = 9; // 3 tabs + 2 * 3 digits
_LIT( KItemFormatFull,  "%d\t%S\t%S\t%d" );
_LIT( KItemFormatNoInd, "%d\t%S\t%S" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::CAppMngr2ListContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2ListContainer::CAppMngr2ListContainer( CAppMngr2ListView& aView )
        : iView( aView )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::~CAppMngr2ListContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2ListContainer::~CAppMngr2ListContainer()
    {
    delete iListBox;
    delete iItemArray;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMngr2ListContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = iListBox->OfferKeyEventL( aKeyEvent, aType ); 
    if( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        iView.UpdateMiddleSoftkeyCommandL();
        iView.CurrentItemSelectedByUser( ETrue );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if( aType == KAknsMessageSkinChange )
        {
        TRAP_IGNORE( LoadIconsL() );
        }

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        if( AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                mainPaneRect ) )
            {
            SetRect( mainPaneRect );
            }
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::SizeChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2ListContainer::CountComponentControls() const
    {
    return 1;   // always only iListBox
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CAppMngr2ListContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType )
    {
    if( aEventType == EEventItemClicked || aEventType == EEventItemSingleClicked )
        {
        iView.UpdateMiddleSoftkeyCommandL();
        iView.CurrentItemSelectedByUser( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::IsListEmpty()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2ListContainer::IsListEmpty() const
    {
    if( iItemArray )
        {
        return ( iItemArray->Count() == 0 );
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::HandleGenericCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::HandleGenericCommandL( TInt aCommand )
    {
    if( !IsListEmpty() )
        {
        CAppMngr2InfoBase& currentItem = CurrentItem();
        if( currentItem.SupportsGenericCommand( aCommand ) )
            {
            Model().HandleCommandL( currentItem, aCommand );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::RefreshL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::RefreshL( TBool aPreserveSelectedItem,
        TBool& aSelectedItemChanged, TInt aMoreRefreshesExpected )
    {
    FLOG( "CAppMngr2ListContainer::RefreshL( %d )", aPreserveSelectedItem );
    
    // Record the item text of the current item if selection must be preserved in
    // the current item. The item text is used to identify the item later, so that
    // it can be selected again.
    HBufC* itemText = NULL;
    TInt currentItemIndex = iListBox->CurrentItemIndex();
    if( aPreserveSelectedItem && currentItemIndex >= 0 )
        {
        itemText = (*iItemArray)[ currentItemIndex ].AllocL();
        CleanupStack::PushL( itemText );
        }

    SetEmptyTextL( aMoreRefreshesExpected );    // defines empty text in first refresh
    CreateItemArrayL();                         // resets the item array
    PopulateItemArrayL();                       // fills in new items
    iListBox->HandleItemAdditionL();            // re-calculates size and scrollbar
    
    // If selection must be preserved, search the item in new item array
    // and select it again. Make sure to delete itemText if it was allocated.
    TBool currentItemSet = EFalse;
    TInt itemCount = iItemArray->Count();
    if( itemText )
        {
        for( TInt index = 0; index < itemCount && !currentItemSet; index++ )
            {
            if( itemText->Compare( (*iItemArray)[ index ] ) == 0 )
                {
                iListBox->SetCurrentItemIndex( index );
                currentItemSet = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( itemText );
        }
    
    // If item is not found (it may have been deleted), then tell to the
    // caller that the selected item was changed and select another item
    // from the same row number than the previously selected item.
    aSelectedItemChanged = !currentItemSet;
    if( aSelectedItemChanged )
        {
        if( currentItemIndex >= itemCount )
            {
            currentItemIndex = itemCount - 1;
            }
        if( currentItemIndex < 0 )
            {
            currentItemIndex = 0;
            }
        if( currentItemIndex != iListBox->CurrentItemIndex() )
            {
            iListBox->SetCurrentItemIndex( currentItemIndex );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    iListBox = new (ELeave) CAknDoubleLargeStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->SetListBoxObserver( this );
    
    // Dont display default "(no data)" empty text
    iListBox->View()->SetListEmptyTextL( KNullDesC );

    LoadIconsL();
    CreateItemArrayL();
    PopulateItemArrayL();
    
    iListBox->CreateScrollBarFrameL();
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListBox->SetRect( aRect.Size() );
    
    // Enable marquee effect
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::Model()
// ---------------------------------------------------------------------------
//
CAppMngr2Model& CAppMngr2ListContainer::Model() const
    {
    return reinterpret_cast<CAppMngr2AppUi*>( iEikonEnv->EikAppUi() )->Model();
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::LoadIconsL()
    {
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( iconArray );
    Model().LoadIconsL( *iconArray );
    delete iListBox->ItemDrawer()->ColumnData()->IconArray();
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
    CleanupStack::Pop( iconArray );
    iItemSpecificIcons = 0;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::CreateItemArrayL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::CreateItemArrayL()
    {
    if( iItemArray )
        {
        iListBox->Model()->SetItemTextArray( NULL );
        delete iItemArray;
        iItemArray = NULL;
        }
    iItemArray = new ( ELeave ) CDesCArrayFlat( KGranularity );
    iListBox->Model()->SetItemTextArray( iItemArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::PopulateItemArrayL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::PopulateItemArrayL()
    {
    FLOG( "CAppMngr2ListContainer::PopulateItemArrayL()" );
    
    CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->ColumnData()->IconArray();
    DeleteItemSpecificIcons( *iconArray );
    
    TInt count = ItemCount();
    for( TInt index = 0; index < count; index++ )
        {
        const CAppMngr2InfoBase& appInfo = ItemInfo( index );

        // Get name and details
        TPtrC name = appInfo.Name();
        TPtrC size = appInfo.Details();

        // Get icon indexes that plugin loaded in LoadIconsL()
        TInt iconIndexBase;
        TInt iconIndexMax;
        Model().GetIconIndexesL( appInfo.Runtime().RuntimeUid(),
                iconIndexBase, iconIndexMax );
        
        // Get list icon index from plugin
        TInt iconIndex = appInfo.IconIndex();
        
        // Convert index into the range of 0 .. (icons - 1)
        if( iconIndex == EAppMngr2UseSpecificIcon )
            {
            CGulIcon* icon = NULL;
            TRAP_IGNORE( icon = appInfo.SpecificIconL() );
            if( icon )
                {
                CleanupStack::PushL( icon );
                iconIndex = iconArray->Count();
                iconArray->AppendL( icon );
                CleanupStack::Pop( icon );
                iItemSpecificIcons++;
                }
            else
                {
                iconIndex = EAppMngr2IconIndex_QgnPropUnknown;
                }
            }
        else
            {
            // There are no default list icons that plugins could use, so
            // this must be list icon that plugin has loaded in LoadIconsL().
            // Icons (of this plugin) are indexed from iconIndexBase.
            iconIndex += iconIndexBase;

            // Plugin should use only those icons it has provided. Use
            // default unknown icon if plugin tries to use icons that
            // another plugin has loaded.
            if( iconIndex < iconIndexBase || iconIndex >= iconIndexMax )
                {
                iconIndex = EAppMngr2IconIndex_QgnPropUnknown;
                }
            }
        // Sanity check - index must be in range, otherwise list panics 
        if( iconIndex < 0 || iconIndex >= iconArray->Count() )
            {
            iconIndex = EAppMngr2IconIndex_QgnPropUnknown;
            }

        // Get indicator icon index from plugin
        TInt indIconIndex = appInfo.IndicatorIconIndex();
        
        // Convert indicator icon index into the range of 0 .. (icons-1) or
        // leave special value EAppMngr2NoIndicatorIcon in it
        if( indIconIndex == EAppMngr2UseSpecificIcon )
            {
            CGulIcon* icon = NULL;
            TRAP_IGNORE( icon = appInfo.SpecificIndicatorIconL() );
            if( icon )
                {
                CleanupStack::PushL( icon );
                indIconIndex = iconArray->Count();
                iconArray->AppendL( icon );
                CleanupStack::Pop( icon );
                iItemSpecificIcons++;
                }
            else
                {
                indIconIndex = EAppMngr2NoIndicatorIcon;
                }
            }
        else
            {
            // EAppMngr2NoIndicatorIcon is special value that is handled later
            if( indIconIndex != EAppMngr2NoIndicatorIcon )
                {
                // Default icon indexes (starting from EAppMngr2NoIndicatorIcon) are
                // all above the icon index range reserved for plugins
                if( indIconIndex > EAppMngr2NoIndicatorIcon )
                    {
                    // Use default icons, indexes start from zero
                    indIconIndex -= ( EAppMngr2NoIndicatorIcon + 1 );
                    }
                else
                    {
                    // Use item specific icons, indexes start from iconIndexBase
                    indIconIndex += iconIndexBase;
                    }
                }
            }
        // Sanity check - icon index must be in range, otherwise list panics
        if( ( indIconIndex != EAppMngr2NoIndicatorIcon ) &&
                ( indIconIndex < 0 || indIconIndex >= iconArray->Count() ) )
            {
            indIconIndex = EAppMngr2NoIndicatorIcon;
            }

        // Construct line to be displayed
        HBufC* temp = HBufC::NewLC( name.Length() + size.Length() + KSpaceForTabsAndIconIndexes );
        TPtr line = temp->Des();
        if( indIconIndex == EAppMngr2NoIndicatorIcon )
            {
            line.AppendFormat( KItemFormatNoInd, iconIndex, &name, &size );
            }
        else
            {
            line.AppendFormat( KItemFormatFull, iconIndex, &name, &size, indIconIndex );
            }
        iItemArray->AppendL( line );
        FLOG( "CAppMngr2ListContainer::PopulateItemArrayL(): %S", temp );
        CleanupStack::PopAndDestroy( temp );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::DeleteItemSpecificIcons()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::DeleteItemSpecificIcons( CArrayPtr<CGulIcon>& aIconArray )
    {
    if( iItemSpecificIcons )
        {
        TInt iconArrayCount = aIconArray.Count();
        TInt firstItemSpecificIcon = iconArrayCount - iItemSpecificIcons;
        for( TInt index = firstItemSpecificIcon; index < iconArrayCount; index++ )
            {
            delete aIconArray[ index ];
            }
        aIconArray.Delete( firstItemSpecificIcon, iItemSpecificIcons );
        iItemSpecificIcons = 0;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ListContainer::SetEmptyTextL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ListContainer::SetEmptyTextL( TInt aMoreRefreshesExpected )
    {
    // Set empty text if it is not already set
    CListBoxView* view = iListBox->View();
    if( view )
        {
        const TDesC* text = view->EmptyListText();
        if( text == NULL || text->Length() == 0 )
            {
            // The first refresh may happen too soon. If there are no items
            // to be displayed, and more refreshes are still expected, then
            // do not set the empty text yet -- it would be displayed.
            if( ItemCount() > 0 || !aMoreRefreshesExpected )
                {
                HBufC* emptyText = StringLoader::LoadLC( ListEmptyTextResourceId() );
                view->SetListEmptyTextL( *emptyText );
                CleanupStack::PopAndDestroy( emptyText );
                }
            }
        }
    }

