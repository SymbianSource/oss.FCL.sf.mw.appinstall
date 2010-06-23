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
* Description:   This file contains the implementation of CSWInstApplication 
*                class member functions.
*
*/


// INCLUDE FILES

#include "SWInstApplication.h"
#include "SWInstDocument.h"
#include "SWInstCommon.h"
#include "SWInstServer.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstApplication::AppDllUid
// Creates CSWInstApplication document object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CApaDocument* CSWInstApplication::CreateDocumentL()
    {
    return CSWInstDocument::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CSWInstApplication::AppDllUid
// Returns application's UID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CSWInstApplication::AppDllUid() const
    {
    return KUidSWInstSvr;
    }

// -----------------------------------------------------------------------------
// CSWInstApplication::NewAppServerL
// Creates application server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = CSWInstServer::NewL();    
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CSWInstApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

//  End of File  
