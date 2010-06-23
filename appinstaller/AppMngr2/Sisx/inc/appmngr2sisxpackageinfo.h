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
* Description:   Sisx PackageInfo represents installable SISX file
*
*/


#ifndef C_APPMNGR2SISXPACKAGEINFO_H
#define C_APPMNGR2SISXPACKAGEINFO_H

#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo
#include <SWInstApi.h>                  // SwiUI::RSWInstLauncher


class CAppMngr2SisxPackageInfo : public CAppMngr2PackageInfo
    {
public:     // constructor and destructor
    static CAppMngr2SisxPackageInfo* NewL( CAppMngr2Runtime& aRuntime,
            const TDesC& aFileName, RFs& aFs );
    ~CAppMngr2SisxPackageInfo();

public:     // from CAppMngr2InfoBase
    TInt IconIndex() const;
    const TDesC& Name() const;
    const TDesC& Details() const;
    TInt IndicatorIconIndex() const;
    TBool SupportsGenericCommand( TInt aCmdId );
    void HandleCommandL( TInt aCommandId, TRequestStatus& aStatus );
    void HandleCommandResultL( TInt aStatus );
    void CancelCommand();

private:    // new functions
    CAppMngr2SisxPackageInfo( CAppMngr2Runtime& aRuntime, RFs& aFs );
    void ConstructL( const TDesC& aFileName );
    void HandleDeleteL();
    void ShowDetailsL();

private:    // data
    HBufC* iName;
    HBufC* iDetails;
    HBufC* iFileName;
    SwiUI::RSWInstLauncher* iSWInstLauncher;
    };

#endif // C_APPMNGR2SISXPACKAGEINFO_H

