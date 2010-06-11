/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Startup list updater test module.
*
*/

#ifndef MT_STARTUPLISTUPDATER_H
#define MT_STARTUPLISTUPDATER_H

#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#define TEST_CLASS_VERSION_MAJOR 1
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

_LIT( KTestStartupListUpdaterLogPath, "\\logs\\testframework\\mt_startuplistupdater\\" );
_LIT( KTestStartupListUpdaterLogFile, "mt_startuplistupdater.txt" );
_LIT( KTestStartupListUpdaterLogFileWithTitle, "mt_startuplistupdater_[%S].txt" );


class CTestStartupListUpdater : public CScriptBase
    {
    public:     // constructors and destructor
        static CTestStartupListUpdater* NewL( CTestModuleIf& aTestModuleIf );
        ~CTestStartupListUpdater();

    private:    // new functions
        CTestStartupListUpdater( CTestModuleIf& aTestModuleIf );
        void ConstructL();
        void SendTestClassVersion();
        void Delete();
        void Print( const TInt aPriority, TRefByValue<const TDesC> aFmt, ... );

    public:     // from CScriptBase
        TInt RunMethodL( CStifItemParser& aItem );

    private:    // test methods
        TInt InstallL( CStifItemParser& aItem );
        TInt RemoveL( CStifItemParser& aItem );
        TInt CheckStartListContainsL( CStifItemParser& aItem );
        TInt CheckStartListDoesNotContainL( CStifItemParser& aItem );

    private:    // new functions
        void GetStartupListL( RPointerArray<HBufC>& aStartupList );
    };

#endif      // MT_STARTUPLISTUPDATER_H

