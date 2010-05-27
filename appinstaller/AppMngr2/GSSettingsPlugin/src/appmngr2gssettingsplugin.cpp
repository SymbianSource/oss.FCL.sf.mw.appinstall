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
* Description:   AppMngr2 Settings plug-in for GS (General Settings)
*
*/


#include "appmngr2gssettingsplugin.h"   // CAppMngrGSSettingsPlugin
#include "appmngr2gssettingscontainer.h" // CAppMngr2GSSettingsContainer
#include "appmngr2.hrh"                 // KAppMngr2AppUidValue
#include <gsprivatepluginproviderids.h> // KGSPluginProviderInternal
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <aknradiobuttonsettingpage.h>  // CAknRadioButtonSettingPage
#include <akntextsettingpage.h>         // CAknTextSettingPage
#include <SWInstallerInternalCRKeys.h>  // KCRUidSWInstallerSettings
#include <centralrepository.h>          // CRepository
#include <StringLoader.h>               // StringLoader
#include <akntitle.h>                   // CAknTitlePane
#include <aknnotewrappers.h>            // CAknInformationNote
#include <hlplch.h>                     // HlpLauncher
#include <featmgr.h>                    // FeatureManager
#include <csxhelp/am.hlp.hrh>           // Help IDs
#include <appmngr2gssettingspluginrsc.rsg> // Resource IDs
#include <appmngr2.mbg>                 // Bitmap IDs

_LIT( KAppMngr2GsSettingsPluginResourceFileName, "appmngr2gssettingspluginrsc.rsc" );  
_LIT( KAppMngr2IconFileNameMif, "appmngr2.mif" );
_LIT( KDriveZ, "z:" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsPlugin* CAppMngr2GSSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CAppMngr2GSSettingsPlugin* self = new( ELeave ) CAppMngr2GSSettingsPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::~CAppMngr2GSSettingsPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsPlugin::~CAppMngr2GSSettingsPlugin()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    iResources.Close();
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::GetCaptionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_SET_FOLDER_APPLICATION_MANAGER );
    aCaption.Copy( *result );
    delete result;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::CreateIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CAppMngr2GSSettingsPlugin::CreateIconL( const TUid aIconType )
    {
    HBufC* iconFilePath = HBufC::NewLC( KDriveZ().Length() +
            KDC_APP_BITMAP_DIR().Length() + KAppMngr2IconFileNameMif().Length() );
    TPtr ptr = iconFilePath->Des();
    ptr.Append( KDriveZ );
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KAppMngr2IconFileNameMif );

    CGulIcon* icon;
    TInt iconId = EMbmAppmngr2Qgn_prop_cp_inst_settings;
    TInt maskId = EMbmAppmngr2Qgn_prop_cp_inst_settings_mask;
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                KAknsIIDQgnPropCpInstSettings, *iconFilePath,
                iconId, maskId );
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    CleanupStack::PopAndDestroy( iconFilePath );
    return icon;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::PluginProviderCategory()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2GSSettingsPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::Id()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2GSSettingsPlugin::Id() const
    {
    return KAppMngr2GSSettingsUid;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAppMngr2CmdChange:
        case EAknSoftkeyChange:
            {
            HandleListBoxSelectionL();
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
            break;
            }
        case EAknSoftkeyExit:
            {
            static_cast<MEikCommandObserver*>( AppUi() )->ProcessCommandL( EAknCmdExit );
            break;
            }
        case EAknCmdHelp:
        case EEikCmdHelpContents:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                CArrayFix<TCoeHelpContext>* contexts =
                    new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
                CleanupStack::PushL( contexts );
                TCoeHelpContext context;
                context.iMajor = KAppMngr2AppUid;
                context.iContext = KAM_HLP_GENERAL_SETTINGS;
                contexts->AppendL( context );
                CleanupStack::Pop( contexts );
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), contexts );
                }
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::HandleViewRectChange()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::HandleViewRectChange()
    {
    if( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_APPMNGR2_SETTINGS_MENU:
            if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::HandleListBoxEventL( CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::DoActivateL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    iPrevViewId = aPrevViewId;
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    HBufC* jamSetTitle = StringLoader::LoadLC( R_AM_INSTALLATION_SETTINGS_TITLE );
    CEikStatusPane* statusPane = StatusPane();
    CAknTitlePane* titlePane;
    titlePane = ( CAknTitlePane* ) statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
    titlePane->SetTextL( jamSetTitle->Des() );
    CleanupStack::PopAndDestroy( jamSetTitle );
    jamSetTitle = NULL;

    iContainer = CAppMngr2GSSettingsContainer::NewL( ClientRect() );
    iContainer->SetMopParent( this );
    iContainer->MakeVisible( ETrue );
    iContainer->SetRect( ClientRect() );
    iContainer->ActivateL();
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->ListBox()->SetListBoxObserver( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::DoDeactivate()
    {
    if( iContainer )
        {
        iContainer->MakeVisible( EFalse );
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::HandleForegroundEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::HandleForegroundEventL( TBool aForeground )
    {
    if( aForeground )
        {
        iContainer->UpdateListBoxContentL();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::CAppMngr2GSSettingsPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GSSettingsPlugin::CAppMngr2GSSettingsPlugin() : iResources( *CCoeEnv::Static() )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::ConstructL()
    {
    TFileName* fileName = TAppMngr2DriveUtils::NearestResourceFileLC(
            KAppMngr2GsSettingsPluginResourceFileName, iCoeEnv->FsSession() );
    
    iResources.OpenL( *fileName );
    BaseConstructL( R_APPMNGR2_SETTINGS_VIEW );

    CleanupStack::PopAndDestroy( fileName );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::HandleListBoxSelectionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::HandleListBoxSelectionL()
    {
    TInt currentIndex = iContainer->ListBox()->CurrentItemIndex();
    ShowSettingPageL( currentIndex );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::ShowSettingPageL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::ShowSettingPageL( TInt aSettingIndex )
    {
    FLOG( "CAppMngr2GSSettingsPlugin: ShowSettingPageL( %d )", aSettingIndex );

    // Variation flag may be used to hide "allow untrusted" option 
    TInt hideAllowUntrusted = 1;
    CRepository* variationDB = CRepository::NewLC( KCRUidSWInstallerLV );
    variationDB->Get( KSWInstallerHideUntrustedIns, hideAllowUntrusted );
    CleanupStack::PopAndDestroy( variationDB );
    if( hideAllowUntrusted )
        {
        // Increase the setting index so that allow untrusted is not taken into account
        aSettingIndex++;
        }

    if( aSettingIndex == EAppMngr2SettingOcsp || aSettingIndex == EAppMngr2SettingOcspUrl )
        {
        if( !FeatureManager::FeatureSupported( KFeatureIdOCSP ) )
            {
            // If OCSP feature is not supported, it's not allowed to change values
            HBufC* p = StringLoader::LoadLC( R_AM_NOTE_LOCKED_SETTIGS );
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( p->Des() );
            CleanupStack::PopAndDestroy( p );
            return;
            }
        }

    HBufC* urlTextStr = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr urlText( urlTextStr->Des() );
    urlText.Zero();
    TInt newIndex = 0;

    if( aSettingIndex == EAppMngr2SettingSwInstall || aSettingIndex == EAppMngr2SettingOcsp )
        {
        CRepository* repDB = CRepository::NewLC( KCRUidSWInstallerSettings );
        CAknSettingPage* dlg = NULL;
        CDesCArrayFlat* itemArray = NULL;

        if( aSettingIndex == EAppMngr2SettingSwInstall )
            {
            // Install software
            itemArray = iCoeEnv->ReadDesC16ArrayResourceL(
                    R_APPMNGR2_ALLOW_UNTRUSTED_VALUE_ARRAY );
            repDB->Get( KSWInstallerAllowUntrusted, newIndex );
            if( newIndex )
                {
                newIndex = 1;
                }
            dlg = new ( ELeave ) CAknRadioButtonSettingPage(
                    R_APPMNGR2_SET_PAGE_ALLOW_UNTRUSTED,
                    newIndex, itemArray );
            }
        else
            {
            // OCSP check
            repDB->Get( KSWInstallerOcspProcedure, newIndex );
            newIndex = MapOCSPIndex( newIndex );
            itemArray = iCoeEnv->ReadDesC16ArrayResourceL(
                    R_APPMNGR2_OCSP_CHECK_VALUE_ARRAY );
            dlg = new ( ELeave ) CAknRadioButtonSettingPage(
                    R_APPMNGR2_OCSP_CHECK_PAGE, newIndex, itemArray );
            }
        CleanupStack::PopAndDestroy( repDB );
        CleanupStack::PushL( itemArray );

        if( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            SetPermissionL( aSettingIndex, newIndex, urlText );
            }
        CleanupStack::PopAndDestroy( itemArray );
        }
    else if( aSettingIndex == EAppMngr2SettingOcspUrl )
        {
        CRepository* repDB = CRepository::NewLC( KCRUidSWInstallerSettings );
        repDB->Get( KSWInstallerOcspDefaultURL, urlText );
        CAknTextSettingPage* textDlg = new ( ELeave ) CAknTextSettingPage(
                R_APPMNGR2_OCSP_URL_SETTING_PAGE, urlText,
                CAknTextSettingPage::EZeroLengthAllowed );
        CleanupStack::PushL( textDlg );
        textDlg->ConstructL();
        textDlg->TextControl()->SetTextLimit( KAppMngr2MaxOcspUrlLength );

        CleanupStack::Pop( textDlg );
        CleanupStack::PopAndDestroy( repDB );

        if( textDlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            // Remove spaces from URL text
            for( TInt pos = urlText.Length() - 1; pos >= 0; --pos ) {
                if( TChar( urlText[pos] ).IsSpace() ) {
                    urlText.Delete( pos, 1 );
                }
            }
            SetPermissionL( aSettingIndex, newIndex, urlText );
            }
        }
    else
        {
        ASSERT( EFalse );
        }
    
    CleanupStack::PopAndDestroy( urlTextStr );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::SetPermissionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GSSettingsPlugin::SetPermissionL( TInt aSettingIndex,
                                       TInt aNewValueIndex,
                                       TPtrC16 aUrlText )
    {
    FLOG( "CAppMngr2GSSettingsPlugin::SetPermissionL( %d, %d, %S )",
            aSettingIndex, aNewValueIndex, &aUrlText );

    // Write the setting value to Central repository
    CRepository* repDB = CRepository::NewLC( KCRUidSWInstallerSettings );
    switch( aSettingIndex )
        {
        case EAppMngr2SettingSwInstall:
            repDB->Set( KSWInstallerAllowUntrusted, aNewValueIndex );
            break;
        case EAppMngr2SettingOcsp:
            aNewValueIndex = MapOCSPIndex( aNewValueIndex );
            repDB->Set( KSWInstallerOcspProcedure, aNewValueIndex );
            break;
        case EAppMngr2SettingOcspUrl:
            repDB->Set( KSWInstallerOcspDefaultURL, aUrlText );
            break;
        default:
            ASSERT( EFalse );
            break;
        }
    CleanupStack::PopAndDestroy( repDB );

    iContainer->UpdateListBoxContentL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2GSSettingsPlugin::MapOCSPIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2GSSettingsPlugin::MapOCSPIndex( TInt aValue )
    {
    TInt result = 0;

    switch( aValue )
        {
        case ESWInstallerOcspProcedureOff:
            result = EAppMngr2OcspCheckValueArrayOff;
            break;
        case ESWInstallerOcspProcedureOn:
            result = EAppMngr2OcspCheckValueArrayOn;
            break;
        case ESWInstallerOcspProcedureMust:
            result = EAppMngr2OcspCheckValueArrayMustPass;
            break;
        default:
            break;
        }

    return result;
    }

