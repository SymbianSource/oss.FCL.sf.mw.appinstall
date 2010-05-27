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
* Description:   Widget runtime type for AppMngr2
*
*/


#include "appmngr2widgetruntime.h"      // CAppMngr2WidgetRuntime
#include "appmngr2widgetappinfo.h"      // CAppMngr2WidgetAppInfo
#include "appmngr2widgetpackageinfo.h"  // CAppMngr2WidgetPackageInfo
#include "appmngr2recognizedfile.h"     // CAppMngr2RecognizedFile
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <data_caging_path_literals.hrh> // KDC_APP_BITMAP_DIR
#include <apmrec.h>                     // CDataTypeArray
#include <AknIconArray.h>               // CAknIconArray
#include <AknsUtils.h>                  // AknsUtils
#include <gulicon.h>                    // CGulIcon
#include <eikenv.h>                     // CEikonEnv
#include <appmngr2widget.mbg>           // icon IDs

_LIT( KAppMngr2WidgetIconFileNameMif, "AppMngr2Widget.mif" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetRuntime* CAppMngr2WidgetRuntime::NewL( MAppMngr2RuntimeObserver& aObserver )
    {
    CAppMngr2WidgetRuntime* self = new (ELeave) CAppMngr2WidgetRuntime( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::~CAppMngr2WidgetRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetRuntime::~CAppMngr2WidgetRuntime()
    {
    iWidgetRegistry.Disconnect();
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::LoadIconsL( CAknIconArray& aIconArray )
    {
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    HBufC* iconFilePath = NULL;
    
    iconFilePath = FullBitmapFileNameLC( KAppMngr2WidgetIconFileNameMif );

    // Icon 0: Widget application icon, qgn_menu_am_widget
    CGulIcon* icon = AknsUtils::CreateGulIconL( skinInstance,
            KAknsIIDQgnMenuAmWidget, *iconFilePath,
            EMbmAppmngr2widgetQgn_menu_am_widget,
            EMbmAppmngr2widgetQgn_menu_am_widget_mask );
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    CleanupStack::PopAndDestroy( iconFilePath );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::GetSupportedDataTypesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::GetSupportedDataTypesL(
        CDataTypeArray& aDataTypeArray )
    {
    TDataType zipFile( KDataTypeWidget );
    aDataTypeArray.AppendL( zipFile );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::GetInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::GetInstallationFilesL(
        RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
        const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
        RFs& aFsSession, TRequestStatus& aStatus )
    {
    TInt fileCount = aFileList.Count();
    for( TInt index = 0; index < fileCount; index++ )
        {
        CAppMngr2RecognizedFile* file = aFileList[ index ];
        CAppMngr2WidgetPackageInfo* packageInfo = NULL;
        TRAPD( err, packageInfo = CAppMngr2WidgetPackageInfo::NewL( *this,
                file->FileName(), aFsSession ) );
        if( err == KErrNone )
            {
            CleanupStack::PushL( packageInfo );
            aPackageInfos.AppendL( packageInfo );
            CleanupStack::Pop( packageInfo );
            }
        }

    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::CancelGetInstallationFiles()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::CancelGetInstallationFiles()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::GetInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::GetInstalledAppsL(
        RPointerArray<CAppMngr2AppInfo>& aApps,
        RFs& aFsSession, TRequestStatus& aStatus )
    {
    RWidgetInfoArray widgetInfoArray;
    CleanupResetAndDestroyPushL( widgetInfoArray );
    iWidgetRegistry.InstalledWidgetsL( widgetInfoArray );

    for( TInt index = 0; index < widgetInfoArray.Count(); index++ )
        {
        CAppMngr2WidgetAppInfo* appObj = NULL;
        TRAPD( err, appObj = CAppMngr2WidgetAppInfo::NewL( *this,
                *( widgetInfoArray[ index ] ), aFsSession ) );
        if( err == KErrNone )
            {
            CleanupStack::PushL( appObj ); 
            aApps.Append( appObj );
            CleanupStack::Pop( appObj );
            }
        }
    
    CleanupStack::PopAndDestroy( &widgetInfoArray );
    
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::CancelGetInstalledApps()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::CancelGetInstalledApps()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::CAppMngr2WidgetRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetRuntime::CAppMngr2WidgetRuntime( MAppMngr2RuntimeObserver& aObserver )
        : CAppMngr2Runtime( aObserver ) 
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetRuntime::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetRuntime::ConstructL()
    {
    User::LeaveIfError( iWidgetRegistry.Connect() );
    }

