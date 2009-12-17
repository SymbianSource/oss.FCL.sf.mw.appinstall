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
* Description:   Async command handling for AppMngr2 Runtime API STIF tests
*
*/


#ifndef C_APPMNGR2TESTCOMMAND_H
#define C_APPMNGR2TESTCOMMAND_H

#include <e32base.h>                    // CActive

class CAppMngr2InfoBase;
class MAppMngr2TestCommandObserver;

class CAppMngr2TestCommand : public CActive
    {
public:     // constructor and destructor
    static CAppMngr2TestCommand* NewL( CAppMngr2InfoBase& aInfo,
            MAppMngr2TestCommandObserver& aObs );
    ~CAppMngr2TestCommand();

public:     // from CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

public:     // new functions
    void HandleCommandL( TInt aCommand );
    
private:    // new functions
    CAppMngr2TestCommand( CAppMngr2InfoBase& aInfo,
            MAppMngr2TestCommandObserver& aObs );

private:    // data
    CAppMngr2InfoBase& iInfo;
    MAppMngr2TestCommandObserver& iObs;
    };

#endif  // C_APPMNGR2TESTCOMMAND_H

