/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppMngr2 GS (General Settings) Settings plug-in Container
*
*/


#include "appmngr2gssettingscontainer.h" // CAppMngr2GSSettingsContainer
#include <aknlists.h>                   // CAknSettingStyleListBox
#include <centralrepository.h>          // CRepository
#include <AknIconArray.h>               // CAknIconArray
#include <featmgr.h>                    // FeatureManager
#include <SWInstallerInternalCRKeys.h>  // KCRUidSWInstallerSettings
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <appmngr2gssettingspluginrsc.rsg> // Resource IDs
#include <appmngr2.mbg>                 // Bitmap IDs
#include <csxhelp/am.hlp.hrh>           // Help IDs

const TInt KNumStrSize = 1;             // Buffer size for string which has one number
const TInt KGranularity = 1;
const TInt KMaxSettingsItemLength = 128 + NCentralRepositoryConstants::KMaxUnicodeStringLength;

_LIT( KAppMngrTab, "\t");
_LIT( KAppMngr2IconFileNameMif, "appmngr2.mif" );
_LIT( KDriveZ, "z:" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsContainer* CAppMngr2GSSettingsContainer::NewL( const TRect& aRect )
    {
    CAppMngr2GSSettingsContainer* self = new ( ELeave ) CAppMngr2GSSettingsContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::~CAppMngr2GSSettingsContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsContainer::~CAppMngr2GSSettingsContainer()
    {
    delete iListbox;
    delete iItems;
    delete iOcspCheckValueArray;
    delete iAllowUntrustedValueArray;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::ListBox()
// ---------------------------------------------------------------------------
//
CAknSettingStyleListBox* CAppMngr2GSSettingsContainer::ListBox() const
    {
    return iListbox;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::UpdateListBoxContentL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::UpdateListBoxContentL()
    {
    TInt ocsp_check_value = 0;
    HBufC* ocsp_url = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr ocsp_url_value( ocsp_url->Des() );
    ocsp_url_value.Zero();
    TInt allowUntrustedValue( 0 );

    // Must save listbox indexes because content will be deleted
    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = static_cast<CDesCArray*>( itemList );
    itemArray->Reset();

    // Get data from Central repository
    CRepository* repDB = CRepository::NewLC( KCRUidSWInstallerSettings );
    repDB->Get( KSWInstallerOcspProcedure, ocsp_check_value );
    repDB->Get( KSWInstallerOcspDefaultURL, ocsp_url_value );
    if( ocsp_url_value.Length() == 0 )
        {
        // If OCSP URL is empty string add the default text "none"
        iCoeEnv->ReadResourceAsDes16L( ocsp_url_value, R_AM_SET_OCSP_NONE );
        }
    repDB->Get( KSWInstallerAllowUntrusted, allowUntrustedValue );
    if( allowUntrustedValue )
        {
        allowUntrustedValue = 1;
        }
    CleanupStack::PopAndDestroy( repDB );

    switch( ocsp_check_value )
        {
        case ESWInstallerOcspProcedureOff:
            ocsp_check_value = EAppMngr2OcspCheckValueArrayOff;
            break;
        case ESWInstallerOcspProcedureOn:
            ocsp_check_value = EAppMngr2OcspCheckValueArrayOn;
            break;
        case ESWInstallerOcspProcedureMust:
            ocsp_check_value = EAppMngr2OcspCheckValueArrayMustPass;
            break;
        default:
            break;
        }

    HBufC* itemValue = HBufC::NewLC( KMaxSettingsItemLength );
    TPtr itemPtr = itemValue->Des();
    itemPtr = ( *iItems )[ EAppMngr2SettingSwInstall ];

    // Check variation
    TInt hideAllowUntrusted( 1 );
    CRepository* variationDB = CRepository::NewLC( KCRUidSWInstallerLV );
    variationDB->Get( KSWInstallerHideUntrustedIns, hideAllowUntrusted );
    CleanupStack::PopAndDestroy( variationDB );

    if( !hideAllowUntrusted )
        {
        itemPtr.Zero();
        itemPtr = ( *iItems )[ EAppMngr2SettingSwInstall ];
        itemPtr += ( *iAllowUntrustedValueArray )[ allowUntrustedValue ];
        itemArray->AppendL( itemPtr );
        }

    if( !FeatureManager::FeatureSupported( KFeatureIdOCSP ) )
        {
        // If OCSP feature is not supported, OCSP settings can not be changed
        // --> lock icon is added
        TBuf<KNumStrSize> strNum;
        strNum.Num( 0 );

        // If lock icon is wanted to be shown, the string format must be:
        // " \tSetting label\t\tSetting value\t\t\t0"

        itemPtr.Zero();
        itemPtr = ( *iItems )[ EAppMngr2SettingOcsp ];
        itemPtr += ( *iOcspCheckValueArray )[ ocsp_check_value ];
        itemPtr += KAppMngrTab;
        itemPtr += KAppMngrTab;
        itemPtr += KAppMngrTab;
        itemPtr.Append( strNum );
        itemArray->AppendL( itemPtr );

        itemPtr.Zero();
        itemPtr = ( *iItems )[ EAppMngr2SettingOcspUrl ];
        itemPtr += ocsp_url_value;
        itemPtr += KAppMngrTab;
        itemPtr += KAppMngrTab;
        itemPtr += KAppMngrTab;
        itemPtr.Append( strNum );
        itemArray->AppendL( itemPtr );
        }
    else
        {
        itemPtr.Zero();
        itemPtr = ( *iItems )[ EAppMngr2SettingOcsp ];
        itemPtr += ( *iOcspCheckValueArray )[ ocsp_check_value ];
        itemArray->AppendL( itemPtr );

        itemPtr.Zero();
        itemPtr = ( *iItems )[ EAppMngr2SettingOcspUrl ];
        itemPtr += ocsp_url_value;
        itemArray->AppendL( itemPtr );
        }

    CleanupStack::PopAndDestroy( itemValue );
    // Handle content changes
    iListbox->Reset();
    // Listbox might not have index
    if( lastListBoxPos != -1 )
        {
        iListbox->SetCurrentItemIndex( lastListBoxPos );
        iListbox->SetTopItemIndex( lastListBoxTopPos );
        }

    CleanupStack::PopAndDestroy( ocsp_url );
    
    // Call after addition to ensure that the list dialog is updated correctly.
    iListbox->HandleItemAdditionL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMngr2GSSettingsContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iListbox->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::CAppMngr2GSSettingsContainer()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsContainer::CAppMngr2GSSettingsContainer()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    iListbox = new( ELeave ) CAknSettingStyleListBox;

    iListbox->SetContainerWindowL( *this );
    iListbox->ConstructL( this, EAknListBoxSelectionList );

    // Read the setting items array (contains first lines of the setting items)
    iItems = iCoeEnv->ReadDesC16ArrayResourceL( R_APPMNGR2_SETTING_ARRAY );
    iOcspCheckValueArray = iCoeEnv->ReadDesC16ArrayResourceL(
            R_APPMNGR2_OCSP_CHECK_VALUE_ARRAY );
    iAllowUntrustedValueArray = iCoeEnv->ReadDesC16ArrayResourceL(
            R_APPMNGR2_ALLOW_UNTRUSTED_VALUE_ARRAY );

    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn,
            CEikScrollBarFrame::EAuto );
    iListbox->SetRect( aRect.Size() );
    iListbox->ActivateL();
    SetRect( aRect );
    ActivateL();
    
    iListbox->SetCurrentItemIndex( EAppMngr2SettingSwInstall );
    if( !FeatureManager::FeatureSupported( KFeatureIdOCSP ) )
        {
        LoadIconsL();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::LoadIconsL()
    {
    HBufC* iconFilePath = HBufC::NewL( KDriveZ().Length() +
            KDC_APP_BITMAP_DIR().Length() + KAppMngr2IconFileNameMif().Length() );
    CleanupStack::PushL( iconFilePath );

    TPtr ptr = iconFilePath->Des();
    ptr.Append( KDriveZ );
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KAppMngr2IconFileNameMif );

    CArrayPtr<CGulIcon>* icons = new ( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    CGulIcon* icon = AknsUtils::CreateGulIconL( skinInstance,
            KAknsIIDQgnIndiSettProtectedAdd, *iconFilePath,
            EMbmAppmngr2Qgn_indi_sett_protected_add,
            EMbmAppmngr2Qgn_indi_sett_protected_add_mask );
    CleanupStack::PushL( icon );
    icons->AppendL( icon );

    iListbox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );

    CleanupStack::Pop( icon );
    CleanupStack::Pop( icons );
    CleanupStack::PopAndDestroy( iconFilePath );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::SizeChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::SizeChanged()
    {
    iListbox->SetRect( Rect() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::FocusChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    if( iListbox )
        {
        iListbox->SetFocus( IsFocused(), aDrawNow );
        }
    }
// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2GSSettingsContainer::CountComponentControls() const
    {
    return iListbox ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CAppMngr2GSSettingsContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListbox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::GetHelpContext()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppMngr2AppUid;
    aContext.iContext = KAM_HLP_GENERAL_SETTINGS;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsContainer::HandleResourceChange()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
        DrawNow();
        }
    }

