/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppMngr2 GS Installed plugin implementation
*
*/


#include "appmngr2gsinstalledplugin.h"  // CAppMngr2GsInstalledPlugin
#include "appmngr2internalpskeys.h"     // KAppManagerApplicationMode
#include "appmngr2.hrh"                 // KAppMngr2AppUidValue
#include <AknNullService.h>             // CAknNullService
#include <e32property.h>                // RProperty
#include <StringLoader.h>               // StringLoader
#include <bautils.h>                    // BaflUtils
#include <gsprivatepluginproviderids.h> // KGSPluginProviderInternal
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2gsinstalledpluginrsc.rsg> // Resource IDs
#include <appmngr2.mbg>                 // Bitmap IDs

_LIT( KAppMngr2InstalledResourceFileName, "z:appmngr2gsinstalledpluginrsc.rsc" );
_LIT( KAppMngrIconFileNameMif, "appmngr2.mif" );
_LIT( KDriveZ, "z:" );

const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstalledPlugin* CAppMngr2GsInstalledPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CAppMngr2GsInstalledPlugin* self = new( ELeave ) CAppMngr2GsInstalledPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::~CAppMngr2GsInstalledPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstalledPlugin::~CAppMngr2GsInstalledPlugin()
    {
    FLOG( "CAppMngr2GsInstalledPlugin::~CAppMngr2GsInstalledPlugin" );
    iResources.Close();
    delete iNullService;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::GetCaptionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* result = StringLoader::LoadL( R_CP_FOLDER_INSTALLED );
    aCaption.Copy( *result );
    delete result;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::ItemType()
// ---------------------------------------------------------------------------
//
TGSListboxItemTypes CAppMngr2GsInstalledPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::CreateIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CAppMngr2GsInstalledPlugin::CreateIconL( const TUid aIconType )
    {
    HBufC* iconFilePath = HBufC::NewLC( KDriveZ().Length() +
            KDC_APP_BITMAP_DIR().Length() + KAppMngrIconFileNameMif().Length() );
    TPtr ptr = iconFilePath->Des();
    ptr.Append( KDriveZ );
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KAppMngrIconFileNameMif );

    CGulIcon* icon;
    if( aIconType == KGSIconTypeLbxItem )
        {       
        icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                KAknsIIDQgnPropCpInstInstalls, *iconFilePath,
                EMbmAppmngr2Qgn_prop_cp_inst_installs,
                EMbmAppmngr2Qgn_prop_cp_inst_installs_mask );
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    CleanupStack::PopAndDestroy( iconFilePath );
    return icon;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::PluginProviderCategory()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2GsInstalledPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::Id()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2GsInstalledPlugin::Id() const
    {
    return KAppMngr2InstalledPluginUid;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::DoActivateL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    FLOG( "CAppMngr2GsInstalledPlugin::DoActivateL" );  // not called
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::DoDeactivate()
    {
    FLOG( "CAppMngr2GsInstalledPlugin::DoDeactivate" ); // not called
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::CAppMngr2GsInstalledPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstalledPlugin::CAppMngr2GsInstalledPlugin() : iResources( *iCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::ConstructL()
    {
    FLOG( "CAppMngr2GsInstalledPlugin::ConstructL" );
    OpenLocalizedResourceFileL( KAppMngr2InstalledResourceFileName, iResources );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::HandleSelection()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAP_IGNORE( LaunchAppManagerL() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::OpenLocalizedResourceFileL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    aResourceLoader.OpenL( fileName );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::LaunchAppManagerL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::LaunchAppManagerL()
    {
    FLOG( "CAppMngr2GsInstalledPlugin::LaunchAppManagerL" );
    
    _LIT_SECURITY_POLICY_PASS( KPropReadPolicy );
    _LIT_SECURITY_POLICY_C1( KPropWritePolicy, ECapabilityWriteDeviceData );
    TInt err = RProperty::Define( KPSUidAppManagerNotification,
            KAppManagerApplicationMode, RProperty::EInt,
            KPropReadPolicy, KPropWritePolicy );	
    if( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    User::LeaveIfError( RProperty::Set( KPSUidAppManagerNotification,
            KAppManagerApplicationMode, EAppMngr2InstalledView ) );

    RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );
    TApaTaskList taskList( ws );
    TApaTask task = taskList.FindApp( KAppMngr2AppUid );

    if( task.Exists() )
        {
        task.BringToForeground();
        }
    else
        {
        EmbedAppL( KAppMngr2AppUid );
        }

    CleanupStack::PopAndDestroy( &ws );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstalledPlugin::EmbedAppL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstalledPlugin::EmbedAppL( const TUid& aAppUid )
    {
    if( iNullService )
        {
        delete iNullService;
        iNullService = NULL;
        }
    iNullService = CAknNullService::NewL( aAppUid, this );
    }

