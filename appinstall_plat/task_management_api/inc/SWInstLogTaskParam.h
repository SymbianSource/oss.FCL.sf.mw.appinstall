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
* Description:   This file contains declarations of log task parameters.
*
*/


#ifndef SWINSTLOGTASKPARAM_H
#define SWINSTLOGTASKPARAM_H

//  INCLUDES
#include <e32std.h>
#include <SWInstUid.h>

namespace SwiUI
{

// CONSTANTS
const TInt KMaxLogNameLength = 128;
const TInt KMaxLogVendorLength = 128;

/**
* Actions to indicate the operation conducted by the installer.
*
* @since 3.0
*/
enum TLogTaskAction
    {
    ELogTaskActionInstall,
    ELogTaskActionUninstall
    };

/**
* Parameters for log task.
*
* @since 3.0
*/
class TLogTaskParam
    {
    public:    
        
        TBuf<KMaxLogNameLength> iName;
        TBuf<KMaxLogVendorLength> iVendor;
        TVersion iVersion;
        TUid iUid;  // Package uid
        TTime iTime;  // Time of the operation
        TLogTaskAction iAction;
        TBool iIsStartup; // Indicates did the operation modify startup list
    };

// Package buffer for log task parameters
typedef TPckg<TLogTaskParam> TLogTaskParamPckg;

// Uid for startup list addition implementation
const TUid KLogTaskImplUid = { KSWInstLogTaskImplUid };


// Constants for accessing the DB


// Name of the log db
_LIT( KLogDatabaseName, "c:SWInstLog.db" );

// Name of the log table
_LIT( KLogTableName, "log" );

// SQL query to retrieve data from log table
_LIT( KLogReadTableSQL, "SELECT time,name,vendor,version,action FROM log" );

// SQL query to create the log table
_LIT( KLogCreateTableSQL, 
"CREATE TABLE log (time BIGINT NOT NULL,uid UNSIGNED INTEGER NOT NULL,\
name VARCHAR(128) NOT NULL,vendor VARCHAR(128) NOT NULL,\
version VARCHAR(16) NOT NULL,action UNSIGNED INTEGER,startup UNSIGNED INTEGER)" );

// SQL query to insert row into the log table
_LIT( KLogInsertSQLFormat, 
"INSERT INTO log (time, uid, name, vendor, version, action, startup) \
VALUES (%S, %u, '%S', '%S', '%S', %u, %u)" );

// UID of the SWInstLog db access policy
const TUid KLogAccessPolicyUid = { KSWInstLogAccessPolicyUid };
}

#endif // SWINSTLOGPARAM_H
