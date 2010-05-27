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
* Description:   This file contains the implementation of CLogTask
*                class member functions.
*
*/


// INCLUDE FILES
#include <d32dbms.h>
#include "LogTask.h"

_LIT( KLogSecureFormat, "SECURE%S" );
const TUint KMaxTimeStringLength = 20;
const TUint KExtraCharsInSql = 16;
const TUint KMaxLogEntryCount = 50;

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLogTask::CLogTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLogTask::CLogTask()
    {
    }

// -----------------------------------------------------------------------------
// CLogTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLogTask* CLogTask::NewL()
    {
    return new(ELeave) CLogTask();    
    }

// Destructor
CLogTask::~CLogTask()
    {
    }

// -----------------------------------------------------------------------------
// CLogTask::ExecuteL
// Executes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLogTask::ExecuteL()
    {
    TRAP_IGNORE( DoExecuteL() );    
    }

// -----------------------------------------------------------------------------
// CLogTask::SetParameterL
// Adds a parameter to the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLogTask::SetParameterL( const TDesC8& aParam, TInt /*aIndex*/ )
    {
    TLogTaskParamPckg pckg( iLogEntry );
    pckg.Copy( aParam );    
    }

// -----------------------------------------------------------------------------
// CLogTask::ExternalizeL
// Externalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLogTask::DoExternalizeL( RWriteStream& /*aStream*/ ) const
    {
    // Task is not persistent
    }

// -----------------------------------------------------------------------------
// CLogTask::DoInternalizeL
// Internalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLogTask::DoInternalizeL( RReadStream& /*aStream*/ )
    {
    // Task is not persistent
    }

// -----------------------------------------------------------------------------
// CLogTask::ConstructVersionString
// Construct a string representation of given version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CLogTask::ConstructVersionStringLC( TInt aMajor, TInt aMinor, TInt aBuild )
    {
    
    // First calculate the number of digits in version numbers
    TInt result( 0 );
    TInt majorDigits( 1 );    
    TInt minorDigits( 1 );    
    TInt buildDigits( 1 );    

    result = aMajor / 10;
    while ( result > 0 )
        {
        result = result / 10;
        majorDigits++;
        }
         
    result = aMinor / 10;
    while ( result > 0 )    
        {
        result = result / 10;
        minorDigits++;
        }    

    result = aBuild / 10;
    while ( result > 0 )    
        {
        result = result / 10;
        buildDigits++;
        }    
        
    // Now we can construct the actual version descriptor

    HBufC* tmp = HBufC::NewLC( majorDigits + minorDigits + buildDigits + 4 ); // . + possible 0 and brackets
    TPtr tmpBuf( tmp->Des() );
    
    //tmpBuf.Append( ' ' );
    tmpBuf.AppendNum( aMajor );
    tmpBuf.Append( '.' );
    if ( aMinor < 10 )
        {        
        tmpBuf.Append('0');
        }
    
    tmpBuf.AppendNum( aMinor );

    tmpBuf.Append( '(' );
    tmpBuf.AppendNum( aBuild );
    tmpBuf.Append( ')' );

    return tmp;    
    }

// -----------------------------------------------------------------------------
// CLogTask::DoExecuteL
// Helper to executes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLogTask::DoExecuteL()
    {
    RDbs dbSession;
    User::LeaveIfError( dbSession.Connect() );
    CleanupClosePushL( dbSession );

    // Construct the db format string
    HBufC* formatString = HBufC::NewLC( KLogSecureFormat().Length() + 
                                        KLogAccessPolicyUid.Name().Length() );
    TPtr ptr( formatString->Des() );
    TUidName uidStr = KLogAccessPolicyUid.Name();    
    ptr.Format( KLogSecureFormat, &uidStr );        

    RDbNamedDatabase dbs;
    // Try to open the db
    TInt err = dbs.Open( dbSession, KLogDatabaseName, *formatString );
    if ( err == KErrNotFound )
        {
        // DB not exist, create it               
        User::LeaveIfError( dbs.Create( dbSession, KLogDatabaseName, *formatString ) );
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );       
        }
    CleanupClosePushL( dbs );    
    
    // See if the log table already exists
    TBool needCompact( EFalse );    
    RDbTable table;
    err = table.Open( dbs, KLogTableName );
    if ( err != KErrNone )
        {
        // Table does not exist
        // Create the table
        User::LeaveIfError( dbs.Execute( KLogCreateTableSQL ) );
        }
    else
        {
        // Check that we don't have too many rows
        CleanupClosePushL( table );        
        if ( table.CountL( RDbTable::EQuick ) >= KMaxLogEntryCount )
            {
            // Delete the first row
            table.FirstL();
            table.DeleteL();    
            needCompact = ETrue;            
            }        
        CleanupStack::PopAndDestroy( &table );        
        }        
    
    // Add entry to the table

    // Time
    HBufC* timeString = HBufC::NewLC( KMaxTimeStringLength );
    TPtr timePtr( timeString->Des() );
    timePtr.Num( iLogEntry.iTime.Int64() );

    // Version
    HBufC* version = ConstructVersionStringLC( iLogEntry.iVersion.iMajor, 
                                               iLogEntry.iVersion.iMinor,
                                               iLogEntry.iVersion.iBuild );   

    // Construct the sql query
    HBufC* sqlQuery = HBufC::NewLC( KLogInsertSQLFormat().Length() + 
                                    KMaxLogNameLength + 
                                    KMaxLogVendorLength + 
                                    version->Length() +
                                    timeString->Length() +
                                    KExtraCharsInSql );
    TPtr sqlPtr( sqlQuery->Des() );
    sqlPtr.Format( KLogInsertSQLFormat, timeString, iLogEntry.iUid.iUid, &(iLogEntry.iName), 
                   &(iLogEntry.iVendor), version, iLogEntry.iAction, iLogEntry.iIsStartup );

    // Execute the query
    User::LeaveIfError( dbs.Execute( *sqlQuery ) );

    // Compact the db if needed
    if ( needCompact )
        {
        dbs.Compact();
        }

    CleanupStack::PopAndDestroy( 6 );  // sqlQuery, version, timeString, dbs, formatString, dbSession
    }


//  End of File  
