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
* Description:   Defines CAppMngr2LogDatabaseEntry to access installation log
*
*/


#ifndef C_APPMNGR2LOGDATABASEENTRY_H
#define C_APPMNGR2LOGDATABASEENTRY_H

#include <e32base.h>                    // CBase
#include <d32dbms.h>                    // RDbView
#include <SWInstLogTaskParam.h>         // SwiUI::KMaxLogNameLength


class CAppMngr2LogDatabaseEntry : public CBase
    {
public:     // constructors and destructor
    static CAppMngr2LogDatabaseEntry* NewL( RDbView& aView );
    ~CAppMngr2LogDatabaseEntry();

public:     // new functions
    inline const TDesC& Name() const;
    inline const TTime Time() const;
    inline const SwiUI::TLogTaskAction Action() const;
    inline const TDesC& Version() const;
    inline const TDesC& Vendor() const;

private:    // new functions
    CAppMngr2LogDatabaseEntry();
    void ConstructL( RDbView& aView );

private:    // data
    HBufC* iName;
    TTime iTime;
    SwiUI::TLogTaskAction iAction;
    HBufC* iVersion;
    HBufC* iVendor;
    };

#include "appmngr2logdatabaseentry.inl"

#endif  // C_APPMNGR2LOGDATABASEENTRY_H

