/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CStartupTaskAdd
*                class member functions.
*
*/


#include <dscstore.h>                   // RDscStore
#include <dscitem.h>                    // CDscItem
#include "StartupTaskAdd.h"
#include "SWInstDebug.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupTaskAdd::CStartupTaskAdd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStartupTaskAdd::CStartupTaskAdd()
    {
    }

// -----------------------------------------------------------------------------
// CStartupTaskAdd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStartupTaskAdd* CStartupTaskAdd::NewL()
    {
    return new(ELeave) CStartupTaskAdd();
    }

// Destructor
CStartupTaskAdd::~CStartupTaskAdd()
    {
    }

// -----------------------------------------------------------------------------
// CStartupTaskAdd::ExecuteL
// Executes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupTaskAdd::ExecuteL()
    {
    RDscStore dsc;
    dsc.OpenL();
    CleanupClosePushL( dsc );
    CreateIfNotExistL( dsc );

    for( TInt index = 0; index < iItems.Count(); index++ )
        {
        const TDesC& fileName = iItems[index]->ExecutableFile();

        CDscItem* item = CDscItem::NewLC( fileName, KNullDesC );
        // Currently only EStartupItemExPolicyNone is supported,
        // hence CStartupItem::RecoveryPolicy() is not used.

        FLOG_1( _L("StartupTaskAdd: Adding %S"), &fileName );
        dsc.AddItemL( *item );

        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy( &dsc );
    }


//  End of File
