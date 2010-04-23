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
* Description:   Shows installation log in a popup list dialog
*
*/


#ifndef C_APPMNGR2LOG_H
#define C_APPMNGR2LOG_H

#include <e32base.h>                    // CBase
#include <badesca.h>                    // CDesCArray
#include <SWInstLogTaskParam.h>         // SwiUI::TLogTaskAction

class CEikonEnv;


class CAppMngr2Log : public CBase
    {
public:     // constructor and destructor
    static CAppMngr2Log* NewLC();
    ~CAppMngr2Log();

public:     // new functions
    void ShowInstallationLogL();

private:    // new functions
    CAppMngr2Log( CEikonEnv& aEikEnv );
    void ConstructLogArrayL( CDesCArray* aItemArray );
    void SetLogActionTextColumnL( TDes& aListboxLine, const SwiUI::TLogTaskAction& aAction );
    void SetLogDateTimeColumnL( TDes& aListboxLine, const TTime& aDateTime );
    
private:    // data
    CEikonEnv& iEikEnv;
    };

#endif  // C_APPMNGR2LOG_H

