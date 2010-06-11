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
* Description:   AppMngr2 GS Installation Files plugin implementation
*
*/


#include "appmngr2gsinstfilesplugin.h"  // CAppMngr2GsInstFilesPlugin
#include "appmngr2internalpskeys.h"     // KAppManagerApplicationMode
#include "appmngr2.hrh"                 // KAppMngr2AppUidValue
#include <AknNullService.h>             // CAknNullService
#include <e32property.h>                // RProperty
#include <StringLoader.h>               // StringLoader
#include <bautils.h>                    // BaflUtils
#include <gsprivatepluginproviderids.h> // KGSPluginProviderInternal
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2gsinstfilespluginrsc.rsg> // Resource IDs
#include <appmngr2.mbg>                 // Bitmap IDs

_LIT( KAppMngr2InstFilesResourceFileName, "z:appmngr2gsinstfilespluginrsc.rsc" );
_LIT( KAppMngrIconFileNameMif, "appmngr2.mif" );
_LIT( KDriveZ, "z:" );
const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstFilesPlugin* CAppMngr2GsInstFilesPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CAppMngr2GsInstFilesPlugin* self = new( ELeave ) CAppMngr2GsInstFilesPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::~CAppMngr2GsInstFilesPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstFilesPlugin::~CAppMngr2GsInstFilesPlugin()
    {
    FLOG( "CAppMngr2GsInstFilesPlugin::~CAppMngr2GsInstFilesPlugin" );
    iResources.Close();
    delete iNullService;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::GetCaptionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* result = StringLoader::LoadL( R_CP_FOLDER_NOT_INSTALLED );
    aCaption.Copy( *result );
    delete result;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::HandleSelection()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::HandleSelection( const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAP_IGNORE( LaunchAppManagerL() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::ItemType()
// ---------------------------------------------------------------------------
//
TGSListboxItemTypes CAppMngr2GsInstFilesPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::CreateIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CAppMngr2GsInstFilesPlugin::CreateIconL( const TUid aIconType )
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
                KAknsIIDQgnPropCpInstFiles, *iconFilePath,
                EMbmAppmngr2Qgn_prop_cp_inst_files,
                EMbmAppmngr2Qgn_prop_cp_inst_files_mask );
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    CleanupStack::PopAndDestroy( iconFilePath );
    return icon;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::PluginProviderCategory()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2GsInstFilesPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::Id()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2GsInstFilesPlugin::Id() const
    {
    return KAppMngr2InstFilesPluginUid;
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::DoActivateL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    FLOG( "CAppMngr2GsInstFilesPlugin::DoActivateL" );  // not called
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::DoDeactivate()
    {
    FLOG( "CAppMngr2GsInstFilesPlugin::DoDeactivate" ); // not called
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::CAppMngr2GsInstFilesPlugin()
// ---------------------------------------------------------------------------
//
CAppMngr2GsInstFilesPlugin::CAppMngr2GsInstFilesPlugin() : iResources( *iCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::ConstructL()
    {
    FLOG( "CAppMngr2GsInstFilesPlugin::ConstructL" );
    OpenLocalizedResourceFileL( KAppMngr2InstFilesResourceFileName, iResources );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::OpenLocalizedResourceFileL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::OpenLocalizedResourceFileL(
        const TDesC& aResourceFileName, RConeResourceLoader& aResourceLoader )
    {
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    aResourceLoader.OpenL( fileName );
    }

// ---------------------------------------------------------------------------
// CAppMngr2GsInstFilesPlugin::LaunchAppManagerL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::LaunchAppManagerL()
    {
    FLOG( "CAppMngr2GsInstFilesPlugin::LaunchAppManagerL" );

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
            KAppManagerApplicationMode, EAppMngr2InstallationFilesView ) );

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
// CAppMngr2GsInstFilesPlugin::EmbedAppL()
// ---------------------------------------------------------------------------
//
void CAppMngr2GsInstFilesPlugin::EmbedAppL( const TUid& aAppUid )
    {
    if( iNullService )
        {
        delete iNullService;
        iNullService = NULL;
        }
    iNullService = CAknNullService::NewL( aAppUid, this );
    }

