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
* Description:   This file contains the implementation of CStartupTaskRemove
*                class member functions.
*
*/


#include <dscstore.h>                   // RDscStore
#include <dscitem.h>                    // CDscItem
#include "StartupTaskRemove.h"
#include "SWInstDebug.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupTaskRemove::CStartupTaskRemove
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStartupTaskRemove::CStartupTaskRemove()
    {
    }

// -----------------------------------------------------------------------------
// CStartupTaskRemove::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStartupTaskRemove* CStartupTaskRemove::NewL()
    {
    return new(ELeave) CStartupTaskRemove();
    }

// Destructor
CStartupTaskRemove::~CStartupTaskRemove()
    {
    }

// -----------------------------------------------------------------------------
// CStartupTaskRemove::ExecuteL
// Executes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupTaskRemove::ExecuteL()
    {
    RDscStore dsc;
    dsc.OpenL();
    CleanupClosePushL( dsc );
    CreateIfNotExistL( dsc );

    for( TInt index = 0; index < iItems.Count(); index++ )
        {
        const TDesC& fileName = iItems[index]->ExecutableFile();
        CDscItem* item = CDscItem::NewLC( fileName, KNullDesC );

        FLOG_1( _L("StartupTaskRemove: Searching for %S"), &fileName );
        if( dsc.ItemExistsL( *item ) )
            {
            FLOG_1( _L("StartupTaskRemove: Removing %S"), &fileName );
            dsc.DeleteItemL( *item );
            }

        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy( &dsc );
    }


//  End of File
