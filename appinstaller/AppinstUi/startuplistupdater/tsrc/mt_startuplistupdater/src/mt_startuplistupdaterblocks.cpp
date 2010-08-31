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

#include "mt_startuplistupdater.h"      // CTestStartupListUpdater
#include "cleanupresetanddestroy.h"     // CleanupResetAndDestroyPushL
#include <StifParser.h>                 // CStifItemParser
#include <usif/sif/sif.h>               // Usif::RSoftwareInstall
#include <usif/scr/scr.h>               // Usif::RSoftwareComponentRegistry
#include <dscstore.h>                   // RDscStore
#include <dscitem.h>                    // CDscItem

const TInt KMaxLineLength = 256;

_LIT( KTestTag, "SLU" );
_LIT( KTestStartFormat, "%S: start" );
_LIT( KTestParamFormatArg, "%S: %S => %d");
_LIT( KTestDoneFormat, "%S: done" );

using namespace Usif;


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// FileNamesEqual()
// ---------------------------------------------------------------------------
//
TBool FileNamesEqual( const HBufC& aFile1, const HBufC& aFile2 )
    {
    return ( aFile1.CompareF( aFile2 ) == 0 );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::Delete
// ---------------------------------------------------------------------------
//
void CTestStartupListUpdater::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CTestStartupListUpdater::Print()
// -----------------------------------------------------------------------------
//
void CTestStartupListUpdater::Print( const TInt aPriority,
        TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KMaxLineLength> buf;
    buf.FormatList( aFmt, list );
    iLog->Log( buf );
    TestModuleIf().Printf( aPriority, KTestTag, buf );
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::RunMethodL
// ---------------------------------------------------------------------------
//
TInt CTestStartupListUpdater::RunMethodL( CStifItemParser& aItem )
    {
    static TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "Install", CTestStartupListUpdater::InstallL ),
        ENTRY( "Remove", CTestStartupListUpdater::RemoveL ),
        ENTRY( "InStartList", CTestStartupListUpdater::CheckStartListContainsL ),
        ENTRY( "NotInStartList", CTestStartupListUpdater::CheckStartListDoesNotContainL )
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::InstallL
// ---------------------------------------------------------------------------
//
TInt CTestStartupListUpdater::InstallL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "Install" );
    Print( 0, KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    RSoftwareInstall installer;
    User::LeaveIfError( installer.Connect() );
    CleanupClosePushL( installer );

    TPtrC param;
    while ( aItem.GetNextString ( param ) == KErrNone )
        {
        COpaqueNamedParams* args = COpaqueNamedParams::NewLC();
        COpaqueNamedParams* results = COpaqueNamedParams::NewLC();
        args->AddIntL( KSifInParam_InstallSilently, 1 );

        TRequestStatus status;
        installer.Install( param, *args, *results, status );
        User::WaitForRequest( status );
        testResult = status.Int();

        CleanupStack::PopAndDestroy( 2, args );

        Print( 1, KTestParamFormatArg, &KTestName, &param, testResult );
        }

    CleanupStack::PopAndDestroy( &installer );

    Print( 0, KTestDoneFormat, &KTestName );
    return testResult;
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::RemoveL()
// ---------------------------------------------------------------------------
//
TInt CTestStartupListUpdater::RemoveL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "Remove" );
    Print( 0, KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    RSoftwareComponentRegistry registry;
    User::LeaveIfError( registry.Connect() );
    CleanupClosePushL( registry );

    // Get installed application TComponentIds and TUids
    RArray<TUid> uidList;
    CleanupClosePushL( uidList );
    RArray<TComponentId> componentIdList;
    CleanupClosePushL( componentIdList );

    registry.GetComponentIdsL( componentIdList );
    for( TInt i = 0; i < componentIdList.Count(); ++i ) {
        TComponentId compId = componentIdList[ i ];
        CComponentEntry *compEntry = CComponentEntry::NewLC();
        if( registry.GetComponentL( compId, *compEntry ) ) {
            if( compEntry->IsRemovable() && compEntry->SoftwareType() == KSoftwareTypeNative )
                {
                _LIT(KCompUid, "CompUid");
                CPropertyEntry *property = registry.GetComponentPropertyL( compId, KCompUid );
                CleanupStack::PushL( property );
                CIntPropertyEntry* intProperty = dynamic_cast<CIntPropertyEntry*>( property );
                uidList.AppendL( TUid::Uid( intProperty->IntValue() ) );
                CleanupStack::PopAndDestroy( property );
            } else {
                uidList.AppendL( KNullUid );
            }

        }
        CleanupStack::PopAndDestroy( compEntry );
    }
    if( uidList.Count() != componentIdList.Count() )
        {
        _LIT( KFailedToGetIds, "Failed to get IDs" );
        Print( 0, KTestStartFormat, &KFailedToGetIds );
        testResult = KErrGeneral;
        }

    // Uninstall TUid apps listed in parameters
    TPtrC param;
    while( aItem.GetNextString ( param ) == KErrNone && testResult == KErrNone )
        {
        const TUint KMaxLimit = 0xFFFFFFFF;
        TLex lex( param );
        TUint32 intVal = 0;

        _LIT( KHexPrefix, "0x" );
        if( param.Left( KHexPrefix().Length() ) == KHexPrefix )
            {
            lex.Inc( KHexPrefix().Length() );
            testResult = lex.BoundedVal( intVal, EHex, KMaxLimit );
            }
        else
            {
            testResult = lex.BoundedVal( intVal, EDecimal, KMaxLimit );
            }

        if( testResult == KErrNone )
            {
            TUid uid( TUid::Uid( intVal ) );
            if( uid != KNullUid )
                {
                TInt index = uidList.Find( uid );
                if( index >= 0 && index < componentIdList.Count() )
                    {
                    TComponentId componentId = componentIdList[ index ];

                    RSoftwareInstall installer;
                    User::LeaveIfError( installer.Connect() );
                    CleanupClosePushL( installer );

                    TRequestStatus status;
                    installer.Uninstall( componentId, status );
                    User::WaitForRequest( status );
                    testResult = status.Int();

                    CleanupStack::PopAndDestroy( &installer );
                    }
                else
                    {
                    testResult = KErrNotFound;
                    }
                }
            else
                {
                testResult = KErrUnknown;
                }
            }
        Print( 1, KTestParamFormatArg, &KTestName, &param, testResult );
        }

    CleanupStack::PopAndDestroy( 3, &registry );    // componentIdList, uidList, registry
    Print( 0, KTestDoneFormat, &KTestName );
    return testResult;
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::CheckStartListContainsL()
// ---------------------------------------------------------------------------
//
TInt CTestStartupListUpdater::CheckStartListContainsL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "InStartList" );
    Print( 0, KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    RPointerArray<HBufC> startupList;
    CleanupResetAndDestroyPushL( startupList );
    GetStartupListL( startupList );

    TIdentityRelation<HBufC> compareFileNames( FileNamesEqual );

    TPtrC param;
    while( aItem.GetNextString ( param ) == KErrNone )
        {
        HBufC* buf = param.AllocLC();
        if( startupList.Find( buf, compareFileNames ) == KErrNotFound )
            {
            testResult = KErrNotFound;
            }
        CleanupStack::PopAndDestroy( buf );
        Print( 1, KTestParamFormatArg, &KTestName, &param, testResult );
        }

    CleanupStack::PopAndDestroy( &startupList );

    Print( 0, KTestDoneFormat, &KTestName );
    return testResult;
    }


// ---------------------------------------------------------------------------
// CTestStartupListUpdater::CheckStartListDoesNotContainL()
// ---------------------------------------------------------------------------
//
TInt CTestStartupListUpdater::CheckStartListDoesNotContainL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "NotInStartList" );
    Print( 0, KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    RPointerArray<HBufC> startupList;
    CleanupResetAndDestroyPushL( startupList );
    GetStartupListL( startupList );

    TIdentityRelation<HBufC> compareFileNames( FileNamesEqual );

    TPtrC param;
    while( aItem.GetNextString ( param ) == KErrNone )
        {
        HBufC* buf = param.AllocLC();
        if( startupList.Find( buf, compareFileNames ) >= 0 )
            {
            testResult = KErrAlreadyExists;
            }
        CleanupStack::PopAndDestroy( buf );
        Print( 1, KTestParamFormatArg, &KTestName, &param, testResult );
        }

    CleanupStack::PopAndDestroy( &startupList );

    Print( 0, KTestDoneFormat, &KTestName );
    return testResult;
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::GetStartupListL()
// ---------------------------------------------------------------------------
//
void CTestStartupListUpdater::GetStartupListL( RPointerArray<HBufC>& aStartupList )
    {
    RDscStore dscStore;
    dscStore.OpenL();
    CleanupClosePushL( dscStore );
    if( !dscStore.DscExistsL() )
        {
        dscStore.CreateDscL();
        }
    dscStore.EnumOpenLC();

    while( CDscItem* item = dscStore.EnumReadNextL() )
        {
        CleanupStack::PushL( item );
        aStartupList.AppendL( item->FileName().AllocL() );
        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy( 2, &dscStore );    // EnumClose, dscStore
    }
