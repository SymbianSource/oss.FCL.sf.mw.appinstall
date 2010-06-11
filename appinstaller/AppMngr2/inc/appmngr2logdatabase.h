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
* Description:   Defines CAppMngr2LogDatabase class to access installation log
*
*/


#ifndef C_APPMNGR2LOGDATABASE_H
#define C_APPMNGR2LOGDATABASE_H

#include <e32base.h>                    // CBase
#include <d32dbms.h>                    // RDbs, RDbNamedDatabase

class CAppMngr2LogDatabaseEntry;


class CAppMngr2LogDatabase : public CBase
    {
public:     // constructors and destructor
    static CAppMngr2LogDatabase* NewL();
    ~CAppMngr2LogDatabase();

public:     // new functions
    const RPointerArray<CAppMngr2LogDatabaseEntry>& Entries() const;

private:    // new functions
    CAppMngr2LogDatabase();
    void ConstructL();

private:    // data
    RPointerArray<CAppMngr2LogDatabaseEntry> iEntries;
    RDbNamedDatabase iLogDb;
    RDbs iDbSession;
    };

#endif  // C_APPMNGR2LOGDATABASE_H

