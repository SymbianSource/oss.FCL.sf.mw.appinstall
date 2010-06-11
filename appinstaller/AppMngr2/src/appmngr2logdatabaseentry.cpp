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
* Description:   Implements CAppMngr2LogDatabaseEntry to access installation log
*
*/


#include "appmngr2logdatabaseentry.h"   // CAppMngr2LogDatabaseEntry

// Columns in SQL query KLogReadTableSQL: "SELECT time,name,vendor,version,action FROM log"
const TInt KTimeCol = 1;
const TInt KNameCol = 2;
const TInt KVendorCol = 3;
const TInt KVersionCol = 4;
const TInt KActionCol = 5;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabaseEntry* CAppMngr2LogDatabaseEntry::NewL( RDbView& aView )
    {
    CAppMngr2LogDatabaseEntry* self = new (ELeave) CAppMngr2LogDatabaseEntry;
    CleanupStack::PushL( self );
    self->ConstructL( aView );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::~CAppMngr2LogDatabaseEntry()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabaseEntry::~CAppMngr2LogDatabaseEntry()
    {
    delete iName;
    delete iVersion;
    delete iVendor;
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::CAppMngr2LogDatabaseEntry()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabaseEntry::CAppMngr2LogDatabaseEntry()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2LogDatabaseEntry::ConstructL( RDbView& aView )
    {
    aView.GetL();
    TInt64 time = aView.ColInt64( KTimeCol );
    iTime = TTime( time );
    iName = aView.ColDes( KNameCol ).AllocL();
    iVendor = aView.ColDes( KVendorCol ).AllocL();
    iVersion = aView.ColDes( KVersionCol ).AllocL();
    iAction = (SwiUI::TLogTaskAction)aView.ColUint32( KActionCol );
    }

