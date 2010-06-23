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
* Description:   WidgetAppInfo represents installed Java applications
*
*/


#ifndef C_APPMNGR2WIDGETAPPINFO_H
#define C_APPMNGR2WIDGETAPPINFO_H

#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <SWInstApi.h>                  // SwiUI::RSWInstLauncher

class CWidgetInfo;


class CAppMngr2WidgetAppInfo : public CAppMngr2AppInfo
    {
public:     // constructor and destructor
    static CAppMngr2WidgetAppInfo* NewL( CAppMngr2Runtime& aRuntime,
            const CWidgetInfo& aWidget, RFs& aFsSession );
    ~CAppMngr2WidgetAppInfo();

public:     // from CAppMngr2InfoBase
    TInt IconIndex() const;
    const TDesC& Name() const;
    const TDesC& Details() const;
    TBool SupportsGenericCommand( TInt aCmdId );
    void HandleCommandL( TInt aCommandId, TRequestStatus& aStatus );
    void HandleCommandResultL( TInt aStatus );
    void CancelCommand();

protected:  // new functions
    CAppMngr2WidgetAppInfo( CAppMngr2Runtime& aRuntime, RFs& aFsSession );
    void ConstructL( const CWidgetInfo& aWidget );

private:    // new functions
    void ShowDetailsL();
    
private:    // data
    TUid iWidgetUid;
    HBufC* iName;
    HBufC* iDetails;
    SwiUI::RSWInstLauncher* iSWInstLauncher;
    };

#endif // C_APPMNGR2WIDGETAPPINFO_H

