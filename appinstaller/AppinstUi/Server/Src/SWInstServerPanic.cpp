/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains implementations of server panic functions. 
*
*/


// INCLUDE FILES

#include "SWInstServerPanic.h"

// ==================== LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// PanicServer
// Panics the server
// Returns: None
// -----------------------------------------------------------------------------
//
GLDEF_C void SwiUI::PanicServer( TSWInstPanic aPanic )
    {
    User::Panic( KSWInstSvrName, aPanic );
    }

// -----------------------------------------------------------------------------
// PanicClient
// Panics the client.
// Returns: None
// -----------------------------------------------------------------------------
//
GLDEF_C void SwiUI::PanicClient( const RMessage2& aMessage, TSWInstPanic aPanic )
    {
    aMessage.Panic( KSWInstSvrName, aPanic );
    }
    
//  End of File  
