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
* Description:   Engine part of the Application Manager
*
*/


#ifndef C_APPMNGR2MODEL_H
#define C_APPMNGR2MODEL_H

#include <e32base.h>                    // CActive
#include <bamdesca.h>                   // MDesCArray
#include <apgnotif.h>                   // MApaAppListServObserver
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2runtimeobserver.h>    // MAppMngr2RuntimeObserver
#include "appmngr2scannerobserver.h"    // MAppMngr2ScannerObserver
#include "appmngr2infomakerobserver.h"  // MAppMngr2InfoMakerObserver
#include "appmngr2infoarrayobserver.h"  // MAppMngr2InfoArrayObserver

class CAppMngr2Runtime;
class CAppMngr2InfoBase;
class CAppMngr2AppInfo;
class CAppMngr2PackageInfo;
class CAppMngr2Scanner;
class MAppMngr2ModelObserver;
class CAppMngr2InfoMaker;
class CAppMngr2AppInfoArray;
class CAppMngr2PackageInfoArray;
class CAppMngr2PluginHolder;
class CAknIconArray;
class RFs;


class CAppMngr2Model : public CActive, public MAppMngr2RuntimeObserver,
        public MAppMngr2ScannerObserver, public MApaAppListServObserver,
        public MAppMngr2InfoMakerObserver, public MAppMngr2InfoArrayObserver
    {
public:     // constructor and destructor
    static CAppMngr2Model* NewL( RFs& aFsSession, MAppMngr2ModelObserver& aObserver );
    ~CAppMngr2Model();

public:     // new functions
    TInt AppInfoCount() const;
    CAppMngr2AppInfo& AppInfo( TInt aIndex ) const; 
    TInt PackageInfoCount() const;
    CAppMngr2PackageInfo& PackageInfo( TInt aIndex ) const; 
    void LoadIconsL( CAknIconArray& aIconArray );
    void GetIconIndexesL( TUid aUid, TInt& aIconIndexBase, TInt& aIconIndexMax ) const;
    void HandleCommandL( CAppMngr2InfoBase& aInfo, TInt aCommand );
    void StartFetchingInstallationFilesL();
    void StartFetchingInstalledAppsL();
    
protected:  // from CActive
    void DoCancel();
    void RunL();

public:     // from MAppMngr2RuntimeObserver
    void RefreshInstalledApps();
    void RefreshInstallationFiles();
    
public:     // from MAppMngr2ScannerObserver
    void ScanningResultL( RPointerArray<CAppMngr2RecognizedFile>& aResult );
    void ScanningComplete();
    void DirectoryChangedL( const TDesC& aChangedDir );

public:     // from MApaAppListServObserver
    void HandleAppListEvent( TInt aEvent );

public:     // from MAppMngr2InfoMakerObserver
    void NewAppsCreatedL( const CAppMngr2InfoMaker& aMaker,
            RPointerArray<CAppMngr2AppInfo>& aAppInfos );
    void ErrorInCreatingAppsL( const CAppMngr2InfoMaker& aMaker, TInt aError );
    void NewPackagesCreatedL( const CAppMngr2InfoMaker& aMaker,
                RPointerArray<CAppMngr2PackageInfo>& aPackageInfos );
    void ErrorInCreatingPackagesL( const CAppMngr2InfoMaker& aMaker, TInt aError );

public:     // from MAppMngr2InfoArrayObserver
    void ArrayContentChanged( CAppMngr2InfoArray* aArray, TInt aMoreRefreshesExpected );
    
private:    // new functions
    CAppMngr2Model( RFs& aFsSession, MAppMngr2ModelObserver& aObserver );
    void ConstructL();
    void LoadDefaultIconsL( CAknIconArray& aIconArray );
    void LoadPluginsL();
    CAppMngr2PluginHolder* DoLoadPluginL( TUid aUid );
    void CreateScannerL();
    void FetchDataTypesL();
    void CloseInfoMaker( const CAppMngr2InfoMaker& aMaker );

private:    // data
    RFs& iFs;
    MAppMngr2ModelObserver& iObs;
    RPointerArray<CAppMngr2PluginHolder> iPlugins;
    CAppMngr2Scanner* iScanner;
    CApaAppListNotifier* iApaAppListNotifier;
    RPointerArray<CAppMngr2InfoMaker> iInfoMakers;
    CAppMngr2AppInfoArray* iInstalledApps;
    CAppMngr2PackageInfoArray* iInstallationFiles;
    CAppMngr2InfoBase* iActiveItem; // not owned
    TInt iActiveCommand;
    TBool iClosing;
    
    FLOG_PERF_DEFINE( FetchInstallationFiles )
    FLOG_PERF_DEFINE( FetchInstalledApps )
    };

#endif // C_APPMNGR2MODEL_H

