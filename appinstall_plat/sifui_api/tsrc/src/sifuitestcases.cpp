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

#include <badesca.h>                    // CDesCArray, StifUnitMacros.h needs this
#include <StifUnitMacros.h>             // STIF_ASSERT_NULL, STIF_ASSERT_NOT_NULL
#include <sifui.h>                      // CSifUi
#include <sifuiappinfo.h>               // CSifUiAppInfo
#include <sifuicertificateinfo.h>       // CSifUiCertificateInfo
#include <s32file.h>                    // RFileReadStream
#include <s32mem.h>                     // RDesReadStream
#include <apgcli.h>                     // RApaLsSession
#include <swi/msisuihandlers.h>         // Swi::CCertificateInfo
#include "sifuitest.h"                  // CSifUiTest
#include "sifuitestcleanuputils.h"      // CleanupResetAndDestroyPushL

_LIT( KX509TestCertFile, "\\testing\\data\\test_x509_cert.cer" );

_LIT( KEnter, "Enter" );
_LIT( KStepFormat, "Step %d" );
_LIT( KExit, "Exit" );

const TInt KBufferGranularity = 1024;


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
        ENTRY( "MemorySelectionTest", CSifUiTest::MemorySelectionTest ),
        ENTRY( "CertificateInfoTest", CSifUiTest::CertificateInfoTest ),
        ENTRY( "AppInfoTest", CSifUiTest::AppInfoTest ),
        ENTRY( "ProgressDialogsTest", CSifUiTest::ProgressDialogsTest )
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
// CSifUiTest::ReadCertificateL()
// -----------------------------------------------------------------------------
//
CX509Certificate* CSifUiTest::ReadCertificateL( const TDesC& aFileName )
    {
    TFindFile findFile( iFs );
    User::LeaveIfError( findFile.FindByDir( aFileName, KNullDesC ) );

    RFile file;
    User::LeaveIfError( file.Open( iFs, findFile.File(), EFileRead ) );
    CleanupClosePushL( file );

    TInt fileSize = 0;
    User::LeaveIfError( file.Size( fileSize ) );

    HBufC8* buffer = HBufC8::NewLC( fileSize );
    TPtr8 ptr( buffer->Des() );
    User::LeaveIfError( file.Read( ptr ) );

    CX509Certificate* x509cert = CX509Certificate::NewL( *buffer );

    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &file );
    return x509cert;
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
    CSifUi* sifUi = NULL;
    while( error == KErrNoMemory )
        {
        User::__DbgSetAllocFail( EFalse, RHeap::EDeterministic, ++count );
        TRAP( error, sifUi = CSifUi::NewL() );
        User::__DbgSetAllocFail( EFalse, RHeap::ENone, count );
        if( !error )
            {
            delete sifUi;
            sifUi = NULL;
            }
        }
    TestModuleIf().Printf( 1, KTestName, _L("count %d, last error %d"), count, error );
    SetResult( aResult, error );

    TestModuleIf().Printf( 0, KTestName, KExit );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::MemorySelectionTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::MemorySelectionTest( TTestResult& aResult )
    {
    TRAPD( result, DoMemorySelectionTestL( aResult ) );
    SetResult( aResult, result );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::DoMemorySelectionTestL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::DoMemorySelectionTestL( TTestResult& aResult )
    {
    _LIT( KTestName, "MemorySelectionTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EGetSelectedWhenNotSet,
        ESetNoDrivesStep,
        ESetThreeDriveStep,
        ESetOneDriveStep,
        EGetSelectedWhenSetButNotAskedStep,
        ELastStep,
        EAllDone
        };

    CSifUi* sifUi = NULL;
    RArray<TInt> driveNumbers;
    CleanupClosePushL( driveNumbers );
    TInt drive = 0;
    TInt errorCode = KErrNone;

    TInt error = KErrNone;
    for( TInt step = EFirstStep; step < EAllDone && !error; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                STIF_ASSERT_NULL( sifUi );
                TRAP( error, sifUi = CSifUi::NewL() );
                if( !error )
                    {
                    CleanupStack::PushL( sifUi );
                    }
                break;

            case EGetSelectedWhenNotSet:
                STIF_ASSERT_NOT_NULL( sifUi );
                TRAP( error, errorCode = sifUi->SelectedDrive( drive ) );
                if( error == KErrNone && errorCode == KErrNotFound )
                    {
                    error = KErrNone;
                    }
                else
                    {
                    error = KErrGeneral;
                    }
                break;

            case ESetNoDrivesStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                TRAP( error, sifUi->SetMemorySelectionL( driveNumbers ) );
                break;

            case ESetThreeDriveStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                driveNumbers.Append( EDriveC );
                driveNumbers.Append( EDriveE );
                driveNumbers.Append( EDriveF );
                TRAP( error, sifUi->SetMemorySelectionL( driveNumbers ) );
                break;

            case ESetOneDriveStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                driveNumbers.Append( EDriveC );
                TRAP( error, sifUi->SetMemorySelectionL( driveNumbers ) );
                break;

            case EGetSelectedWhenSetButNotAskedStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                TRAP( error, errorCode = sifUi->SelectedDrive( drive ) );
                if( error == KErrNone && errorCode == KErrNotFound )
                    {
                    error = KErrNone;
                    }
                else
                    {
                    error = KErrGeneral;
                    }
                break;

            case ELastStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                CleanupStack::PopAndDestroy( sifUi );
                sifUi = NULL;
                break;

            default:
                User::Leave( KErrGeneral );
                break;
            }
        }
    User::LeaveIfError( error );

    CleanupStack::PopAndDestroy( &driveNumbers );
    TestModuleIf().Printf( 0, KTestName, KExit );
    }

// -----------------------------------------------------------------------------
// CSifUiTest::CertificateInfoTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::CertificateInfoTest( TTestResult& aResult )
    {
    TRAPD( result, DoCertificateInfoTestL( aResult ) );
    SetResult( aResult, result );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::DoCertificateInfoTestL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::DoCertificateInfoTestL( TTestResult& aResult )
    {
    _LIT( KTestName, "CertificateInfoTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        ESetNoCertificates,
        EReadAndSetCertificate,
        ELastStep,
        EAllDone
        };

    CSifUi* sifUi = NULL;
    RPointerArray<CSifUiCertificateInfo> certificates;
    CleanupResetAndDestroyPushL( certificates );

    TInt error = KErrNone;
    for( TInt step = EFirstStep; step < EAllDone && !error; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                STIF_ASSERT_NULL( sifUi );
                TRAP( error, sifUi = CSifUi::NewL() );
                if( !error )
                    {
                    CleanupStack::PushL( sifUi );
                    }
                break;

            case ESetNoCertificates:
                STIF_ASSERT_NOT_NULL( sifUi );
                TRAP( error, sifUi->SetCertificateInfoL( certificates ) );
                break;

            case EReadAndSetCertificate:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                CX509Certificate* x509cert = ReadCertificateL( KX509TestCertFile );
                CleanupStack::PushL( x509cert );

                Swi::CCertificateInfo* swiCert = Swi::CCertificateInfo::NewLC( *x509cert );
                CSifUiCertificateInfo* testCert = CSifUiCertificateInfo::NewLC( *swiCert );

                CBufBase* buf = CBufFlat::NewL( KBufferGranularity );
                CleanupStack::PushL( buf );
                RBufWriteStream writeStream( *buf );
                CleanupClosePushL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() == 0 );
                testCert->ExternalizeL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() > 0 );
                CleanupStack::PopAndDestroy( 2, buf );      // writeStream, buf

                CleanupStack::PopAndDestroy( testCert );
                testCert = NULL;

                testCert = CSifUiCertificateInfo::NewL( *swiCert );
                CleanupStack::PushL( testCert );
                certificates.AppendL( testCert );
                CleanupStack::Pop( testCert );
                TRAP( error, sifUi->SetCertificateInfoL( certificates ) );

                CleanupStack::PopAndDestroy( 2, x509cert );     // swiCert, x509cert
                }
                break;

            case ELastStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                CleanupStack::PopAndDestroy( sifUi );
                sifUi = NULL;
                break;

            default:
                User::Leave( KErrGeneral );
                break;
            }
        }
    User::LeaveIfError( error );

    CleanupStack::PopAndDestroy( &certificates );
    TestModuleIf().Printf( 0, KTestName, KExit );
    }

// -----------------------------------------------------------------------------
// CSifUiTest::AppInfoTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::AppInfoTest( TTestResult& aResult )
    {
    TRAPD( result, DoAppInfoTestL( aResult ) );
    SetResult( aResult, result );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::DoAppInfoTestL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::DoAppInfoTestL( TTestResult& aResult )
    {
    _LIT( KTestName, "AppInfoTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EAppInfoTests,
        EAppInfoIconTest,
        EShowConfirmation,
        EShowError,
        ELastStep,
        EAllDone
        };

    CSifUi* sifUi = NULL;

    _LIT( KAppName, "TestApplication" );
    _LIT( KAppVendor, "TestSupplier" );
    const TVersion KAppVersion( 1, 2, 3 );
    const TInt KAppSize = 0x1234;

    TInt error = KErrNone;
    for( TInt step = EFirstStep; step < EAllDone && !error; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                STIF_ASSERT_NULL( sifUi );
                TRAP( error, sifUi = CSifUi::NewL() );
                if( !error )
                    {
                    CleanupStack::PushL( sifUi );
                    }
                break;

            case EAppInfoTests:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                CSifUiAppInfo* appInfo = NULL;
                appInfo = CSifUiAppInfo::NewLC( KAppName, KAppVendor, KAppVersion, KAppSize, NULL );

                STIF_ASSERT_TRUE( appInfo->Name().Compare( KAppName ) == 0 );
                STIF_ASSERT_TRUE( appInfo->Vendor().Compare( KAppVendor ) == 0 );

                STIF_ASSERT_TRUE( appInfo->Version().iMajor == KAppVersion.iMajor );
                STIF_ASSERT_TRUE( appInfo->Version().iMinor == KAppVersion.iMinor );
                STIF_ASSERT_TRUE( appInfo->Version().iBuild == KAppVersion.iBuild );

                STIF_ASSERT_TRUE( appInfo->Size() == KAppSize );
                STIF_ASSERT_TRUE( appInfo->Bitmaps() == NULL );

                CBufBase* buf = CBufFlat::NewL( KBufferGranularity );
                CleanupStack::PushL( buf );
                RBufWriteStream writeStream( *buf );
                CleanupClosePushL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() == 0 );
                appInfo->ExternalizeL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() > 0 );
                CleanupStack::PopAndDestroy( &writeStream );
                CleanupStack::PopAndDestroy( buf );

                CleanupStack::PopAndDestroy( appInfo );
                }
                break;

            case EAppInfoIconTest:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                CSifUiAppInfo* appInfo = NULL;

                // TODO: proper icon test needed
                CApaMaskedBitmap* appBitmap = NULL;
                appInfo = CSifUiAppInfo::NewLC( KAppName, KAppVendor, KAppVersion, KAppSize, appBitmap );

                CBufBase* buf = CBufFlat::NewL( KBufferGranularity );
                CleanupStack::PushL( buf );
                RBufWriteStream writeStream( *buf );
                CleanupClosePushL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() == 0 );
                appInfo->ExternalizeL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() > 0 );
                CleanupStack::PopAndDestroy( &writeStream );
                CleanupStack::PopAndDestroy( buf );

                CleanupStack::PopAndDestroy( appInfo );
                }
                break;

            case EShowConfirmation:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                CSifUiAppInfo* appInfo = NULL;
                appInfo = CSifUiAppInfo::NewL( KAppName, KAppVendor, KAppVersion, KAppSize, NULL );
                CleanupStack::PushL( appInfo );

                CBufBase* buf = CBufFlat::NewL( KBufferGranularity );
                CleanupStack::PushL( buf );
                RBufWriteStream writeStream( *buf );
                CleanupClosePushL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() == 0 );
                appInfo->ExternalizeL( writeStream );
                STIF_ASSERT_TRUE( buf->Size() > 0 );
                CleanupStack::PopAndDestroy( &writeStream );
                CleanupStack::PopAndDestroy( buf );

                TBool result = EFalse;
                // TODO: how to close opened dialog automatically?
                TRAP( error, result = sifUi->ShowConfirmationL( *appInfo ) );
                if( result )
                    {
                    _LIT( KAccepted, "Accepted" );
                    TestModuleIf().Printf( 1, KTestName, KAccepted );
                    }
                else
                    {
                    _LIT( KCancelled, "Cancelled" );
                    TestModuleIf().Printf( 1, KTestName, KCancelled );
                    }
                CleanupStack::PopAndDestroy( appInfo );
                }
                break;

            case EShowError:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                _LIT( KErrorMessage, "Test error" );
                _LIT( KErrorDetails, "Test error details" );
                // TODO: how to close opened dialog automatically?
                TRAP( error, sifUi->ShowFailedL( KErrNotFound, KErrorMessage, KErrorDetails ) );
                }
                break;

            case ELastStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                CleanupStack::PopAndDestroy( sifUi );
                sifUi = NULL;
                break;

            default:
                User::Leave( KErrGeneral );
                break;
            }
        }
    User::LeaveIfError( error );

    TestModuleIf().Printf( 0, KTestName, KExit );
    }

// -----------------------------------------------------------------------------
// CSifUiTest::ProgressDialogsTest()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::ProgressDialogsTest( TTestResult& aResult )
    {
    TRAPD( result, DoProgressDialogsTestL( aResult ) );
    SetResult( aResult, result );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::DoProgressDialogsTestL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::DoProgressDialogsTestL( TTestResult& aResult )
    {
    _LIT( KTestName, "ProgressDlgsTest" );
    TestModuleIf().Printf( 0, KTestName, KEnter );

    enum TTestSteps
        {
        EFirstStep,
        EShowProgress,
        EUpdateProgress,
        EShowComplete,
        ELastStep,
        EAllDone
        };

    CSifUi* sifUi = NULL;

    _LIT( KAppName, "NoitaCilppa" );
    _LIT( KAppVendor, "Rodnev" );
    const TVersion KAppVersion( 3, 2, 1 );
    const TInt KAppSize = 0x4321;

    const TInt KMaxProgress = 150;
    const TInt KUpdateStarts = -10;
    const TInt KUpdateEnds = KMaxProgress + 10;
    const TInt KUpdateStep = 4;

    TInt error = KErrNone;
    for( TInt step = EFirstStep; step < EAllDone && !error; ++step )
        {
        TestModuleIf().Printf( 1, KTestName, KStepFormat, step );
        switch( step )
            {
            case EFirstStep:
                STIF_ASSERT_NULL( sifUi );
                TRAP( error, sifUi = CSifUi::NewL() );
                if( !error )
                    {
                    CleanupStack::PushL( sifUi );
                    }
                break;

            case EShowProgress:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                CSifUiAppInfo* appInfo = NULL;
                appInfo = CSifUiAppInfo::NewL( KAppName, KAppVendor, KAppVersion, KAppSize, NULL );
                CleanupStack::PushL( appInfo );
                TRAP( error, sifUi->ShowProgressL( *appInfo, KMaxProgress ) );
                CleanupStack::PopAndDestroy( appInfo );
                }
                break;

            case EUpdateProgress:
                {
                STIF_ASSERT_NOT_NULL( sifUi );
                for( TInt i = KUpdateStarts; i < KUpdateEnds; i += KUpdateStep )
                    {
                    TRAP( error, sifUi->IncreaseProgressBarValueL( KUpdateStep ) );
                    }
                }
                break;

            case EShowComplete:
                STIF_ASSERT_NOT_NULL( sifUi );
                TRAP( error, sifUi->ShowCompleteL() );
                break;

            case ELastStep:
                STIF_ASSERT_NOT_NULL( sifUi );
                CleanupStack::PopAndDestroy( sifUi );
                sifUi = NULL;
                break;

            default:
                User::Leave( KErrGeneral );
                break;
            }
        }
    User::LeaveIfError( error );

    TestModuleIf().Printf( 0, KTestName, KExit );
    }

