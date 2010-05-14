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
* Description:   Native SISX runtime type for AppMngr2
*
*/


#ifndef C_APPMNGR2SISXRUNTIME_H
#define C_APPMNGR2SISXRUNTIME_H

#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <swi/sisregistrysession.h>     // RSisRegistrySession

class MAppMngr2RuntimeObserver;
class CAppMngr2PackageInfo;
class CAppMngr2SisxSwiMonitor;
class CAknIconArray;

const TInt KAppMngr2SisxUidValue = 0x20016BF6;
const TUid KAppMngr2SisxUid = { KAppMngr2SisxUidValue }; 

// Private folder of installer daemon, contains SIS files for untrusted apps
_LIT( KAppMngr2DaemonPrivateFolder, "\\private\\10202dce\\" );


class CAppMngr2SisxRuntime : public CAppMngr2Runtime
    {
public:     // constructor and destructor
    static CAppMngr2SisxRuntime* NewL( MAppMngr2RuntimeObserver& aObserver );
    ~CAppMngr2SisxRuntime();

public:     // from CAppMngr2Runtime
    void LoadIconsL( CAknIconArray& aIconArray );
    void GetSupportedDataTypesL( CDataTypeArray& aDataTypeArray );
    void GetAdditionalDirsToScanL( RFs& aFsSession, RPointerArray<HBufC>& aDirs );
    void GetInstallationFilesL( RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
            const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
            RFs& aFsSession, TRequestStatus& aStatus );
    void CancelGetInstallationFiles();
    void GetInstalledAppsL( RPointerArray<CAppMngr2AppInfo>& aApps,
                RFs& aFsSession, TRequestStatus& aStatus );
    void CancelGetInstalledApps();

private:    // new functions
    CAppMngr2SisxRuntime( MAppMngr2RuntimeObserver& aObserver );
    void ConstructL( MAppMngr2RuntimeObserver& aObserver );
    void CreateNewPackageL( const TDesC& aFileName,
            RPointerArray<CAppMngr2PackageInfo>& aPackageInfos, RFs& aFs );

private:    // data
    Swi::RSisRegistrySession iSisRegistrySession;
    CAppMngr2SisxSwiMonitor* iSwiMonitor;
    };

#endif // C_APPMNGR2SISXRUNTIME_H

