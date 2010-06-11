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
* Description:   Implements CAppMngr2LogDatabase class to access installation log
*
*/


#include "appmngr2logdatabase.h"        // CAppMngr2LogDatabase
#include "appmngr2logdatabaseentry.h"   // CAppMngr2LogDatabaseEntry
#include <SWInstLogTaskParam.h>         // KLogReadTableSQL

_LIT( KLogSecureFormat, "SECURE%S" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabase::CAppMngr2LogDatabase()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabase::CAppMngr2LogDatabase()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabase::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabase* CAppMngr2LogDatabase::NewL()
    {
    CAppMngr2LogDatabase* self = new (ELeave) CAppMngr2LogDatabase;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAppMngr2LogDatabase::Entries()
// -----------------------------------------------------------------------------
//
const RPointerArray<CAppMngr2LogDatabaseEntry>& CAppMngr2LogDatabase::Entries() const
    {
    return iEntries;
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabase::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2LogDatabase::ConstructL()
    {
    User::LeaveIfError( iDbSession.Connect() );

    HBufC* formatString = HBufC::NewLC( KLogSecureFormat().Length() +
            SwiUI::KLogAccessPolicyUid.Name().Length() );
    TPtr ptr( formatString->Des() );
    TUidName uidStr = SwiUI::KLogAccessPolicyUid.Name();
    ptr.Format( KLogSecureFormat, &uidStr );

    TInt err = iLogDb.Open( iDbSession, SwiUI::KLogDatabaseName, *formatString );
    CleanupStack::PopAndDestroy( formatString );
    if( err == KErrNone )
        {
        RDbView view;
        User::LeaveIfError( view.Prepare( iLogDb, TDbQuery( SwiUI::KLogReadTableSQL ),
                view.EReadOnly ) );
        CleanupClosePushL( view );
        
        view.EvaluateAll();
        for( view.FirstL(); view.AtRow(); view.NextL() )
            {
            CAppMngr2LogDatabaseEntry* entry = CAppMngr2LogDatabaseEntry::NewL( view );
            CleanupStack::PushL( entry );
            iEntries.AppendL( entry );      // takes ownership
            CleanupStack::Pop( entry );
            }
    
        CleanupStack::PopAndDestroy( &view );
        }
    else
        {
        // handle "database not found" like "database is empty"
        if( err != KErrNotFound && err != KErrPathNotFound )
            {
            User::Leave( err );
            }
        }
    
    iLogDb.Close();
    iDbSession.Close();
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogDatabase::~CAppMngr2LogDatabase()
// ---------------------------------------------------------------------------
//
CAppMngr2LogDatabase::~CAppMngr2LogDatabase()
    {
    iEntries.ResetAndDestroy();
    iLogDb.Close();
    iDbSession.Close();
    }

