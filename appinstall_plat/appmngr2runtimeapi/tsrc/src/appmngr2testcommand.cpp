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


#include "appmngr2testcommand.h"        // CAppMngr2TestCommand
#include "appmngr2testcommandobserver.h" // MAppMngr2TestCommandObserver
#include <appmngr2infobase.h>           // CAppMngr2InfoBase


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::NewL()
// -----------------------------------------------------------------------------
//
CAppMngr2TestCommand* CAppMngr2TestCommand::NewL( CAppMngr2InfoBase& aInfo,
        MAppMngr2TestCommandObserver& aObs )
    {
    CAppMngr2TestCommand* self = new (ELeave) CAppMngr2TestCommand( aInfo, aObs );
    return self;
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::~CAppMngr2TestCommand()
// -----------------------------------------------------------------------------
//
CAppMngr2TestCommand::~CAppMngr2TestCommand()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::DoCancel()
// -----------------------------------------------------------------------------
//
void CAppMngr2TestCommand::DoCancel()
    {
    iInfo.CancelCommand();
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::RunL()
// -----------------------------------------------------------------------------
//
void CAppMngr2TestCommand::RunL()
    {
    // no need to trap, as RunError calls CommandResult if RunL leaves
    iInfo.HandleCommandResultL( iStatus.Int() );
    iObs.CommandResult( *this, iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::RunError()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2TestCommand::RunError( TInt aError )
    {
    iObs.CommandResult( *this, aError );
    return aError;
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::HandleCommandL()
// -----------------------------------------------------------------------------
//
void CAppMngr2TestCommand::HandleCommandL( TInt aCommand )
    {
    if( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iInfo.HandleCommandL( aCommand, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CAppMngr2TestCommand::CAppMngr2TestOperation()
// -----------------------------------------------------------------------------
//
CAppMngr2TestCommand::CAppMngr2TestCommand( CAppMngr2InfoBase& aInfo,
        MAppMngr2TestCommandObserver& aObs ) :
        CActive( CActive::EPriorityStandard ), iInfo( aInfo ), iObs( aObs )
    {
    CActiveScheduler::Add( this );
    }

