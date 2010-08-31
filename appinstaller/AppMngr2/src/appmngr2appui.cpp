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
* Description:   CAppMngr2AppUi implementation
*
*/


#include "appmngr2appui.h"              // CAppMngr2AppUi
#include "appmngr2model.h"              // CAppMngr2Model
#include "appmngr2internalpskeys.h"     // KAppManagerApplicationMode
#include "appmngr2installedview.h"      // CAppMngr2InstalledView
#include "appmngr2packagesview.h"       // CAppMngr2PackagesView
#include "appmngr2.hrh"                 // Command IDs
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2debugutils.h>         // FLOG macros
#include <eikdoc.h>                     // CEikDocument
#include <appmngr2.rsg>                 // Resource IDs
#include <featmgr.h>                    // FeatureManager
#include <e32property.h>                // RProperty
#include <hlplch.h>                     // HlpLauncher

_LIT( KSWInstCommonUIResourceFileName, "SWInstCommonUI.rsc" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppUi::ConstructL()
    {
    FLOG( "CAppMngr2AppUi::ConstructL" );
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    RWsSession& wsSession = iEikonEnv->WsSession();
    wsSession.ComputeMode( RWsSession::EPriorityControlDisabled );

    FeatureManager::InitializeLibL();
    
    TFileName* fullName = TAppMngr2DriveUtils::NearestResourceFileLC(
            KSWInstCommonUIResourceFileName, iEikonEnv->FsSession() );
    FLOG( "CAppMngr2AppUi::ConstructL, opening %S", fullName );
    iResourceFileOffset = iEikonEnv->AddResourceFileL( *fullName );
    CleanupStack::PopAndDestroy( fullName );

    FLOG( "CAppMngr2AppUi::ConstructL, creting model" );
    iModel = CAppMngr2Model::NewL( iEikonEnv->FsSession(), *this );
   
    FLOG( "CAppMngr2AppUi::ConstructL, creting views" );
    CAppMngr2InstalledView* installedView = CAppMngr2InstalledView::NewL();
    AddViewL( installedView );  // takes ownership
    CAppMngr2PackagesView* packagesView = CAppMngr2PackagesView::NewL();
    AddViewL( packagesView );   // takes ownership

    if( iEikonEnv->StartedAsServerApp() )
        {
        TInt appMode;
        TInt err = RProperty::Get( KPSUidAppManagerNotification,
                KAppManagerApplicationMode, appMode );
        FLOG( "CAppMngr2AppUi::ConstructL, appMode = %d, err = %d", appMode, err );
        if( err == KErrNone )
            {
            if( appMode == EInstalled )
                {
                ActivateLocalViewL( KInstalledViewId );
                }
            else
                {
                ActivateLocalViewL( KPackagesViewId );
                iConstructInstallationFilesFirst = ETrue;
                }
            }
        else
            {
            ActivateLocalViewL( KInstalledViewId );
            }
        }
    else
        {
        ActivateLocalViewL( KInstalledViewId );
        }
    
    FLOG( "CAppMngr2AppUi::ConstructL, starting delayed construct" );
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    iIdle->Start( TCallBack( &CAppMngr2AppUi::DelayedConstructL, this ) );
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::~CAppMngr2AppUi()
// ---------------------------------------------------------------------------
//
CAppMngr2AppUi::~CAppMngr2AppUi()
    {
    FLOG( "CAppMngr2AppUi::~CAppMngr2AppUi" );
    delete iIdle;
    delete iModel;
    if( iResourceFileOffset > 0 )
        {
        iEikonEnv->DeleteResourceFile( iResourceFileOffset );
        }
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::Model()
// ---------------------------------------------------------------------------
//
CAppMngr2Model& CAppMngr2AppUi::Model() const
    {
    return *iModel;
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::InstalledAppsChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppUi::InstalledAppsChanged( TInt aMoreRefreshesExpected )
    {
    FLOG( "CAppMngr2AppUi::InstalledAppsChanged( %d )", aMoreRefreshesExpected );
    if( iView == View( KInstalledViewId ) )
        {
        CAppMngr2ListView* view = static_cast<CAppMngr2ListView*>( iView );
        TRAP_IGNORE( view->RefreshL( aMoreRefreshesExpected ) );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::InstallationFilesChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppUi::InstallationFilesChanged( TInt aMoreRefreshesExpected )
    {
    FLOG( "CAppMngr2AppUi::InstallationFilesChanged( %d )", aMoreRefreshesExpected );
    if( iView == View( KPackagesViewId ) )
        {
        CAppMngr2ListView* view = static_cast<CAppMngr2ListView*>( iView );
        TRAP_IGNORE( view->RefreshL( aMoreRefreshesExpected ) );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::InstalledAppsDisplayed()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2AppUi::InstalledAppsDisplayed()
    {
    return ( iView == View( KInstalledViewId ) );
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::InstallationFilesDisplayed()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2AppUi::InstallationFilesDisplayed()
    {
    return ( iView == View( KPackagesViewId ) );
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::DelayedConstructL()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2AppUi::DelayedConstructL( TAny* aSelf )
    {
    if( aSelf )
        {
        CAppMngr2AppUi* self = static_cast<CAppMngr2AppUi*>( aSelf );
        FLOG( "CAppMngr2AppUi::DelayedConstructL, step %d",
                self->iDelayedConstructionStep );
        switch( self->iDelayedConstructionStep )
            {
            case EFirstStep:
                if( self->iConstructInstallationFilesFirst )
                    {
                    self->iModel->StartFetchingInstallationFilesL();
                    }
                else
                    {
                    self->iModel->StartFetchingInstalledAppsL();
                    }
                self->iDelayedConstructionStep = ESecondStep;
                return ETrue; // call DelayedConstruct again

            case ESecondStep:
                if( self->iConstructInstallationFilesFirst )
                    {
                    self->iModel->StartFetchingInstalledAppsL();
                    }
                else
                    {
                    self->iModel->StartFetchingInstallationFilesL();
                    }
                self->iDelayedConstructionStep = EAllDone;
                break;
                
            default:
                break;
            }
        }
    return EFalse; // all done
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppUi::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppUi::HandleCommandL( TInt aCommand )
    {
    FLOG( "CAppMngr2AppUi::HandleCommandL( %d )", aCommand );
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EAknCmdHelp:
        case EEikCmdHelpContents:
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppHelpContextL() );
                }
            break;

        default:
            break;
        }
    }

