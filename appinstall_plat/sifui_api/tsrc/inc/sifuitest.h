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
* Description: STIF hardcoded module declaration
*
*/

#ifndef SIFUITEST_H
#define SIFUITEST_H

#include <StifTestModule.h>             // CTestModuleBase
#include <StifLogger.h>                 // CStifLogger
#include <w32std.h>                     // TKeyEvent


// Definitions defined in STIF test module template
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

_LIT( KSifUiTestLogPath, "\\logs\\testframework\\SifUiTest\\" );
_LIT( KSifUiTestLogFile, "SifUiTest.txt" );
_LIT( KSifUiTestLogFileWithTitle, "SifUiTest_[%S].txt" );

#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

class CX509Certificate;
class CSifUiTest;
class CSifUi;

// Pointer to test function (that does the actual testing) type definition
typedef TInt (CSifUiTest::* TestFunction)(TTestResult&);

// Structure containing test case name and pointer to test function
class TCaseInfo
    {
    public:
        TPtrC           iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TDesC& aDes ) : iCaseName( aDes )
        {
        };
    };



/**
 *  SifUiTest class.
 *
 */
NONSHARABLE_CLASS(CSifUiTest) : public CTestModuleBase
    {
    public:     // constructors and destructor
        static CSifUiTest* NewL();
        ~CSifUiTest();

    public:     // from CTestModuleBase
        /**
        * InitL is used to initialize the test module.
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first
        *   created instance of CSifUiTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * GetTestCasesL is used to inquiry test cases.
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile,
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * RunTestCaseL is used to run an individual test case.
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is
        *   not reported in aResult parameter as test case failure).
        */
        TInt RunTestCaseL( const TInt aCaseNumber,
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();

    private:
        CSifUiTest();
        void ConstructL();

        /**
        * Function returning test case name and pointer to test case function.
        * @param aCaseNumber test case number
        * @return TCaseInfo
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

        /**
         * Helper function that returns aErrorCode as string.
         * @param aErrorCode - Symbian OS error code
         * @return name of the error code
         */
        TPtrC ErrorCodeString( TInt aErrorCode ) const;

        /**
         * Helper function that sets aReturnCode result to aResult.
         * @param aResult - test case result to be set
         * @param aReturnCode - Symbian OS error code
         */
        void SetResult( TTestResult& aResult, const TInt aReturnCode ) const;

        /**
         * Helper function that sends key event after given time delay.
         * Note that this function returns immediately, but it still sends
         * the key event after given delay.
         * @param aDelay - time to wait in microseconds
         * @param aKey - key event to send
         */
        void AsyncWaitAndSendKeyEventL( TTimeIntervalMicroSeconds32 aDelay,
                const TKeyEvent& aKeyEvent ) const;

        /**
         * Helper function to create Swi::CCertificateInfo objects.
         */
        CX509Certificate* ReadCertificateL( const TDesC& aFileName );


        // --- TEST CASE FUNCTIONS ---

        /**
         * Creates CSifUi objects in very low-memory until create succeeds.
         * @param aResult - test case result (PASS/FAIL)
         * @return Symbian OS error code (other than test result)
         */
        TInt CreateLowMemTest( TTestResult& aResult );

        /**
         * Run drive selection test case.
         * @param aResult - test case result (PASS/FAIL)
         * @return Symbian OS error code (other than test result)
         */
        TInt MemorySelectionTest( TTestResult& aResult );

        /**
         * Helper function for MemorySelectionTest()
         */
        void DoMemorySelectionTestL( TTestResult& aResult );

        /**
         * Run certificate info test case.
         * @param aResult - test case result (PASS/FAIL)
         * @return Symbian OS error code (other than test result)
         */
        TInt CertificateInfoTest( TTestResult& aResult );

        /**
         * Helper function for CertificateInfoTest()
         */
        void DoCertificateInfoTestL( TTestResult& aResult );

        /**
         * Run application info test case.
         * @param aResult - test case result (PASS/FAIL)
         * @return Symbian OS error code (other than test result)
         */
        TInt AppInfoTest( TTestResult& aResult );

        /**
         * Helper function for AppInfoTest()
         */
        void DoAppInfoTestL( TTestResult& aResult );
        
        /**
         * Run progress dialogs test case.
         * @param aResult - test case result (PASS/FAIL)
         * @return Symbian OS error code (other than test result)
         */
        TInt ProgressDialogsTest( TTestResult& aResult );

        /**
         * Helper function for ProgressDialogsTest()
         */
        void DoProgressDialogsTestL( TTestResult& aResult );

    private:        // data
        TestFunction iMethod;
        CStifLogger* iLog;
        CStifLogger* iStdLog;
        CStifLogger* iTCLog;
        TBool iAddTestCaseTitleToLogName;
        TBool iVersionLogged;
        RFs iFs;
        CActiveScheduler* iScheduler;
    };

#endif      // SIFUITEST_H

