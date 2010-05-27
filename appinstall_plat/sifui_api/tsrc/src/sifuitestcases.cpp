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
* Description: This file contains hardcoded module implementation.
*
*/

#include <badesca.h>                    // CDesCArray
#include <sifui.h>                      // CSifUi
#include "SifUiTest.h"                  // CSifUiTest

_LIT( KEnter, "Enter" );
_LIT( KStepFormat, "Step %d" );
_LIT( KExit, "Exit" );

// Internal structure containing test case name and pointer to test function
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

const TInt KOneSecond = 1000000;
const TInt KThreeSeconds = 3 * KOneSecond;
const TInt KFiveSeconds = 5 * KOneSecond;
const TInt KHalfSecond = KOneSecond / 2;
const TKeyEvent KRightSoftkey = { EKeyDevice1, 0, 0, EStdKeyDevice1 };  // EKeyCBA2


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CSifUiTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// -----------------------------------------------------------------------------
//
const TCaseInfo CSifUiTest::Case ( const TInt aCaseNumber ) const
     {
    /**
    * When adding new test cases, implement new test case function and add new
    * line to KCases array specifing the name of the case and the function.
    *
    * ENTRY macro takes two parameters: test case name and test case
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    */

    static TCaseInfoInternal const KCases[] =
        {
        ENTRY( "CreateLowMemTest", CSifUiTest::CreateLowMemTest ),
        ENTRY( "InformationNoteTest", CSifUiTest::InformationNoteTest ),
        ENTRY( "WarningNoteTest", CSifUiTest::WarningNoteTest ),
        ENTRY( "ErrorNoteTest", CSifUiTest::ErrorNoteTest ),
        ENTRY( "PermanentNoteTest", CSifUiTest::PermanentNoteTest ),
        ENTRY( "ProgressNoteTest", CSifUiTest::ProgressNoteTest ),
        ENTRY( "WaitNoteTest", CSifUiTest::WaitNoteTest ),
        ENTRY( "LaunchHelpTest", CSifUiTest::LaunchHelpTest )
        };

    if( (TUint) aCaseNumber >= sizeof( KCases ) / sizeof( TCaseInfoInternal ) )
        {
        TCaseInfo emptyObject( KNullDesC );
        emptyObject.iMethod = NULL;
        emptyObject.iIsOOMTest = EFalse;
        emptyObject.iFirstMemoryAllocation = 0;
        emptyObject.iLastMemoryAllocation = 0;
        return emptyObject;
        }

    TPtrC caseName( static_cast<const TUint16*>( KCases[ aCaseNumber ].iCaseName ) );
    TCaseInfo caseInfo( caseName );
    caseInfo.iMethod = KCases[ aCaseNumber ].iMethod;
    caseInfo.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    caseInfo.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    caseInfo.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return caseInfo;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::CreateSifUi()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::CreateSifUi()
    {
    if( iSifUi )
        {
        delete iSifUi;
        iSifUi = NULL;
        }

    TRAPD( err, iSifUi = CSifUi::NewL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::CreateTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::CreateLowMemTest( TTestResult& aResult )
    {
    _LIT( KTestName, "CreateLowMemTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    TInt count = 0;
    TInt error = KErrNoMemory;
    while( error == KErrNoMemory )
        {
        User::__DbgSetAllocFail( EFalse, RHeap::EDeterministic, ++count );
        __UHEAP_MARK;
        error = CreateSifUi();
        __UHEAP_MARKEND;
        User::__DbgSetAllocFail( EFalse, RHeap::ENone, count );
        }
    TestModuleIf().Printf( 1, KTestName, _L("count %d, last error %d"), count, error );
    SetResult( aResult, error );
    delete iSifUi;
    iSifUi = NULL;

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::InformationNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::InformationNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "InformationNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EChangeText,
        EWaitSomeTime,
        EAllDone
        };
    _LIT( KInfoNoteText, "This is information note" );
    _LIT( KChangedInfoNoteText, "Changed information note text" );

    TInt result = CreateSifUi();
    for( TInt step = EFirstStep; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                TRAP( result, iSifUi->DisplayInformationNoteL( KInfoNoteText ) );
                break;
            case EChangeText:
                User::After( KOneSecond );
                TRAP( result, iSifUi->DisplayInformationNoteL( KChangedInfoNoteText ) );
                break;
            case EWaitSomeTime:
                User::After( KOneSecond );
                break;
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::WarningNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::WarningNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "WarningNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EChangeText,
        EWaitSomeTime,
        EAllDone
        };
    _LIT( KWarningNoteText, "This is warning note" );
    _LIT( KChangedWarningNoteText, "Changed warning note text" );

    TInt result = CreateSifUi();
    for( TInt step = EFirstStep; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                TRAP( result, iSifUi->DisplayInformationNoteL( KWarningNoteText ) );
                break;
            case EChangeText:
                User::After( KOneSecond );
                TRAP( result, iSifUi->DisplayInformationNoteL( KChangedWarningNoteText ) );
                break;
            case EWaitSomeTime:
                User::After( KOneSecond );
                break;
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::ErrorNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::ErrorNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "ErrorNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EChangeText,
        EAllDone
        };
    _LIT( KErrorNoteText, "This is error note" );
    _LIT( KChangedErrorNoteText, "Changed error note text" );

    TInt result = CreateSifUi();
    for( TInt step = EFirstStep; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                TRAP( result, iSifUi->DisplayErrorNoteL( KErrorNoteText ) );
                break;
            case EChangeText:
                User::After( KOneSecond );
                TRAP( result, iSifUi->DisplayErrorNoteL( KChangedErrorNoteText ) );
                break;
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::PermanentNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::PermanentNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "PermanentNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EDisplayNote,
        EChangeText,
        ECloseNote,
        EDisplayDelayedNote,
        EChangeDelayedNoteText,
        ECloseSecondNote,
        EAllDone
        };

    _LIT( KPermanentNoteText, "This is permanent note" );
    _LIT( KChangedNoteText, "Changed permanent note text" );
    _LIT( KDelayedNoteText, "This is delayed permanent note" );
    _LIT( KChangedDelayedNoteText, "Changed delayed permanent note text" );

    TInt result = CreateSifUi();
    for( TInt step = EFirstStep; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                TRAP( result, iSifUi->ClosePermanentNote() );
                break;
            case EDisplayNote:
                TRAP( result, iSifUi->DisplayPermanentNoteL( KPermanentNoteText ) );
                break;
            case EChangeText:
                User::After( KThreeSeconds );
                TRAP( result, iSifUi->DisplayPermanentNoteL( KChangedNoteText ) );
                break;
            case ECloseNote:
                TRAP( result, iSifUi->ClosePermanentNote() );
                break;
            case EDisplayDelayedNote:
                User::After( KThreeSeconds );
                TRAP( result, iSifUi->DisplayPermanentNoteL( KDelayedNoteText ) );
                break;
            case EChangeDelayedNoteText:
                User::After( KFiveSeconds );
                TRAP( result, iSifUi->DisplayPermanentNoteL( KChangedDelayedNoteText ) );
                break;
            case ECloseSecondNote:
                User::After( KOneSecond );
                TRAP( result, iSifUi->ClosePermanentNote() );
                break;
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::ProgressNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::ProgressNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "ProgressNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EDisplayProgressNote,
        EIncreaseBar1,
        EIncreaseBar2,
        EChangeProgressText,
        EIncreaseBar3,
        EIncreaseBar4,
        EIncreaseBar5,
        ECloseProgressNote,
        EDelayedProgressNote,
        EAllDone
        };
    _LIT( KProgressNoteText, "This is progress note" );
    _LIT( KChangedProgressNoteText, "Changed progress note text" );

    const TInt KProgressBarFinalValue = 6;
    TInt progressBarValue = -1;
    TRequestStatus reqStatus;       // ignored now, not monitoring if user cancels the note
    TRequestStatus reqUpdate;
    TInt result = CreateSifUi();
    for( TInt step = EDisplayProgressNote; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EDisplayProgressNote:
                TRAP( result, iSifUi->DisplayProgressNoteL( KProgressNoteText, reqStatus ) );
                if( result == KErrNone )
                    {
                    TRAP( result, iSifUi->SetProgressNoteFinalValueL( KProgressBarFinalValue ) );
                    }
                break;

            case EIncreaseBar1:
            case EIncreaseBar2:
            case EIncreaseBar3:
            case EIncreaseBar4:
            case EIncreaseBar5:
                User::After( KOneSecond );
                TRAP( result, iSifUi->UpdateProgressNoteValueL( progressBarValue ) );
                progressBarValue += 2;
                break;

            case EChangeProgressText:
                User::After( KOneSecond );
                TRAP( result, iSifUi->DisplayProgressNoteL( KChangedProgressNoteText, reqUpdate ) );
                if( result == KErrNone )
                    {
                    User::WaitForRequest( reqUpdate );
                    result = reqUpdate.Int();
                    }
                break;

            case ECloseProgressNote:
                User::After( KOneSecond );
                TRAP( result, iSifUi->CloseProgressNoteL() );
                if( result == KErrNone )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;

            case EDelayedProgressNote:
                TRAP( result, iSifUi->DisplayProgressNoteL( KProgressNoteText, reqStatus ) );
                if( result == KErrNone )
                    {
                    User::After( KHalfSecond );
                    TRAP( result, iSifUi->CloseProgressNoteL() );
                    }
                break;

            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::WaitNoteTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::WaitNoteTest( TTestResult& aResult )
    {
    _LIT( KTestName, "WaitNoteTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        ECloseWaitNoteBeforeItsOpened,
        EDisplayWaitNote,
        EChangeDisplayedText,
        ECloseFirstWaitNote,
        EDisplayDelayedWaitNote,
        EChangeDelayedWaitNoteText,
        ECloseWaitNote,
        EAllDone
        };
    _LIT( KWaitNoteText, "This is wait note" );
    _LIT( KChangedWaitNoteText, "Changed wait note text" );
    _LIT( KDelayedWaitNoteText, "This is delayed wait note" );
    _LIT( KChangedDelayedWaitNoteText, "Changed delayed wait note text" );

    TInt result = CreateSifUi();
    TRequestStatus reqStatus;       // ignored now, not monitoring if user cancels the note
    for( TInt step = EDisplayWaitNote; step < EAllDone && result == KErrNone; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case ECloseWaitNoteBeforeItsOpened:
                TRAP( result, iSifUi->CloseWaitNote() );
                break;
            case EDisplayWaitNote:
                TRAP( result, iSifUi->DisplayWaitNoteL( KWaitNoteText, reqStatus ) );
                if( result == KErrNone && reqStatus != KRequestPending )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            case EChangeDisplayedText:
                User::After( KThreeSeconds );
                TRAP( result, iSifUi->DisplayWaitNoteL( KChangedWaitNoteText, reqStatus ) );
                if( result == KErrNone && reqStatus != KRequestPending )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            case ECloseFirstWaitNote:
                TRAP( result, iSifUi->CloseWaitNote() );
                if( result == KErrNone )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            case EDisplayDelayedWaitNote:
                User::After( KOneSecond );
                TRAP( result, iSifUi->DisplayWaitNoteL( KDelayedWaitNoteText, reqStatus ) );
                if( result == KErrNone && reqStatus != KRequestPending )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            case EChangeDelayedWaitNoteText:
                User::After( KThreeSeconds );
                TRAP( result, iSifUi->DisplayWaitNoteL( KChangedDelayedWaitNoteText, reqStatus ) );
                if( result == KErrNone && reqStatus != KRequestPending )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            case ECloseWaitNote:
                User::After( KOneSecond );
                TRAP( result, iSifUi->CloseWaitNote() );
                if( result == KErrNone )
                    {
                    User::WaitForRequest( reqStatus );
                    result = reqStatus.Int();
                    }
                break;
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    delete iSifUi;
    iSifUi = NULL;

    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::LaunchHelpTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::LaunchHelpTest( TTestResult& aResult )
    {
    _LIT( KTestName, "LaunchHelpTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    TInt result = CreateSifUi();
    if( result == KErrNone )
        {
        AsyncWaitAndSendKeyEventL( KFiveSeconds, KRightSoftkey );

        _LIT( KAM_HLP_INSTALL_CAPAB, "AM_HLP_INSTALL_CAPAB" );
        TRAP( result, iSifUi->LaunchHelpL( KAM_HLP_INSTALL_CAPAB ) );

        delete iSifUi;
        iSifUi = NULL;
        }
    SetResult( aResult, result );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

