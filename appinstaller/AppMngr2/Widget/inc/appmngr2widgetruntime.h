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


#ifndef C_APPMNGR2WIDGETRUNTIME_H
#define C_APPMNGR2WIDGETRUNTIME_H

#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <WidgetRegistryClient.h>       // RWidgetRegistryClientSession 

class CAppMngr2PackageInfo;
class CAknIconArray;

const TInt KAppMngr2WidgetUidValue = 0x20016BFA;
const TUid KAppMngr2WidgetUid = { KAppMngr2WidgetUidValue };

_LIT8( KDataTypeWidget, "application/x-nokia-widget" );


class CAppMngr2WidgetRuntime : public CAppMngr2Runtime
    {
public:     // constructor and destructor
    static CAppMngr2WidgetRuntime* NewL( MAppMngr2RuntimeObserver& aObserver );
    ~CAppMngr2WidgetRuntime();

public:     // new CAppMngr2Runtime
    void LoadIconsL( CAknIconArray& aIconArray );
    void GetSupportedDataTypesL( CDataTypeArray& aDataTypeArray );
    void GetInstallationFilesL( RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
            const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
            RFs& aFsSession, TRequestStatus& aStatus );
    void CancelGetInstallationFiles();
    void GetInstalledAppsL( RPointerArray<CAppMngr2AppInfo>& aApps,
                RFs& aFsSession, TRequestStatus& aStatus );
    void CancelGetInstalledApps();

private:    // new functions
    CAppMngr2WidgetRuntime( MAppMngr2RuntimeObserver& aObserver );
    void ConstructL();

private:    // data
    TInt iIconCount;
    TInt iIconOffset;
    RWidgetRegistryClientSession iWidgetRegistry;
    };

#endif // C_APPMNGR2WIDGETRUNTIME_H

