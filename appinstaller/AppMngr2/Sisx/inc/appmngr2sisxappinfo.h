/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   SisxAppInfo represents installed native/SISX applications
*
*/


#ifndef C_APPMNGR2SISXAPPINFO_H
#define C_APPMNGR2SISXAPPINFO_H

#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <swi/sisregistryentry.h>       // RSisRegistryEntry
#include <SWInstApi.h>                  // SwiUI::RSWInstLauncher
#include <CUIDetailsDialog.h>           // SwiUI::CommonUI::CCUICertificateInfo
#include <f32file.h>                    // TDriveUnit


class CAppMngr2SisxAppInfo : public CAppMngr2AppInfo
    {
public:     // constructor and destructor
    static CAppMngr2SisxAppInfo* NewL( CAppMngr2Runtime& aRuntime,
            Swi::RSisRegistryEntry& aEntry, RFs& aFsSession );
    ~CAppMngr2SisxAppInfo();

public:     // from CAppMngr2InfoBase
    TInt IconIndex() const;
    const TDesC& Name() const;
    const TDesC& Details() const;
    TBool SupportsGenericCommand( TInt aCmdId );
    void HandleCommandL( TInt aCommandId, TRequestStatus& aStatus );
    void HandleCommandResultL( TInt aStatus );
    void CancelCommand();

public:     // new functions
    TVersion Version() const;
    const TDesC& Vendor() const;
    TBool IsTrusted() const;

protected:  // new functions
    CAppMngr2SisxAppInfo( CAppMngr2Runtime& aRuntime, RFs& aFsSession );
    void ConstructL( Swi::RSisRegistryEntry& aEntry );

private:    // new functions
    void ShowDetailsL();
    void ReadCertificatesL();
    void HandleUninstallL( TRequestStatus& aStatus );
    
private:    // data
    TUid iAppUid;
    HBufC* iName;
    HBufC* iDetails;
    TVersion iVersion;
    HBufC* iVendor;
    TBool iIsAugmentation;
    TInt iAugmentationIndex;
    TBool iIsTrusted;
    TBool iIsDRMProtected;
    TBool iIsRightsObjectMissingOrExpired;
    HBufC* iProtectedFile;
    SwiUI::RSWInstLauncher* iSWInstLauncher;
    HBufC8* iSWInstLauncherCustomUninstallParams;
    RPointerArray<SwiUI::CommonUI::CCUICertificateInfo> iCertificates;
    TBool iCertsRead;
    TInt iCommandId;
    };

#endif // C_APPMNGR2SISXAPPINFO_H

