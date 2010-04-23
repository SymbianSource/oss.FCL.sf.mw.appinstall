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
* Description:   AppMngr2 Runtime API STIF test module
*
*/


#include "appmngr2runtimeapitest.h"     // CRuntimeApiTest


// ======== GLOBAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// LibEntryL()
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    return static_cast<CScriptBase*>( CAppMngr2RuntimeApiTest::NewL( aTestModuleIf ) );
    }

