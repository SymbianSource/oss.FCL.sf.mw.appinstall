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
* Description:  Startup list updater implementation.
*
*/

#include "startuplistupdater.h"         // CStartupListUpdater
#include "cleanupresetanddestroy.h"     // CleanupResetAndDestroyPushL
#include <featmgr.h>                    // FeatureManager
#include <driveinfo.h>                  // DriveInfo
#include <barsc.h>                      // RResourceFile
#include <barsread.h>                   // TResourceReader
#include <StartupItem.hrh>              // EStartupItemExPolicyNone
#include <dscitem.h>                    // CDscItem
#include <swi/sisregistrysession.h>     // Swi::RSisRegistrySession
#include <swi/sisregistrypackage.h>     // Swi::CSisRegistryPackage
#include <swi/sisregistryentry.h>       // Swi::RSisRegistryEntry
#include <swi/sistruststatus.h>         // Swi::TSisTrustStatus::IsTrusted

_LIT( KImport, "import\\" );
_LIT( KDriveAndPathFormat, "%c:%S" );
_LIT( KResourceFileSpec, "c:*.rsc" );
_LIT( KExecutableExtension, ".exe" );
_LIT( KAsterisk, "*" );

const TInt KDriveSpecLength = 2;
const TInt KFirstStartupItemInfo = 1;


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
// CStartupListUpdater::NewL()
// ---------------------------------------------------------------------------
//
CStartupListUpdater* CStartupListUpdater::NewL()
    {
    CStartupListUpdater* self = new( ELeave ) CStartupListUpdater;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::~CStartupListUpdater()
// ---------------------------------------------------------------------------
//
CStartupListUpdater::~CStartupListUpdater()
    {
    delete iPrivateImportPath;
    iFs.Close();

    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::UpdateStartupListL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::UpdateStartupListL()
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {
        ProcessImportsAndUninstallsL();
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::CStartupListUpdater()
// ---------------------------------------------------------------------------
//
CStartupListUpdater::CStartupListUpdater()
    {
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ConstructL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ConstructL()
    {
    FeatureManager::InitializeLibL();
    User::LeaveIfError( iFs.Connect() );

    TInt err = iFs.MkDirAll( PrivateImportPathL() );
    if( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::PrivateImportPathL()
// ---------------------------------------------------------------------------
//
const TDesC& CStartupListUpdater::PrivateImportPathL()
    {
    if( !iPrivateImportPath )
        {
        TFileName privateImportPath;
        User::LeaveIfError( iFs.PrivatePath( privateImportPath ) );
        privateImportPath.Append( KImport );

        TInt driveNumber;
        TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
        User::LeaveIfError( err );
        TChar driveLetter;
        User::LeaveIfError( RFs::DriveToChar( driveNumber, driveLetter ) );

        iPrivateImportPath = HBufC::NewL( KDriveSpecLength + privateImportPath.Length() );
        TPtr ptr( iPrivateImportPath->Des() );
        ptr.Format( KDriveAndPathFormat, static_cast< TUint >( driveLetter ), &privateImportPath );
        }
    return *iPrivateImportPath;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ProcessImportsAndUninstallsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ProcessImportsAndUninstallsL()
    {
    RDscStore dscStore;
    OpenDscStoreLC( dscStore );

    ImportNewResourceFilesL( dscStore );
    DeregisterUninstalledAppsL( dscStore );

    CleanupStack::PopAndDestroy( &dscStore );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::OpenDscStoreLC()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::OpenDscStoreLC( RDscStore& aDscStore )
    {
    if( !aDscStore.IsOpened() )
        {
        aDscStore.OpenL();
        CleanupClosePushL( aDscStore );
        if( !aDscStore.DscExistsL() )
            {
            aDscStore.CreateDscL();
            }
        }
    else
        {
        User::Leave( KErrAlreadyExists );
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ImportNewResourceFilesL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ImportNewResourceFilesL( RDscStore& aDscStore )
    {
    RPointerArray<HBufC> executableArray;
    CleanupResetAndDestroyPushL( executableArray );
    RPointerArray<HBufC> resourceFileArray;
    CleanupResetAndDestroyPushL( resourceFileArray );

    GetDataToBeImportedL( executableArray, resourceFileArray );
    ImportExecutablesL( aDscStore, executableArray );
    RemoveImportedResourceFiles( resourceFileArray );

    CleanupStack::PopAndDestroy( 2, &executableArray );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::DeregisterUninstalledAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::DeregisterUninstalledAppsL( RDscStore& aDscStore )
    {
    RPointerArray<HBufC> startedAtBootArray;
    CleanupResetAndDestroyPushL( startedAtBootArray );
    RPointerArray<HBufC> installedArray;
    CleanupResetAndDestroyPushL( installedArray );
    RPointerArray<HBufC> startedNotInstalled;
    CleanupResetAndDestroyPushL( startedNotInstalled );

    GetStartupListAppsL( aDscStore, startedAtBootArray );
    GetInstalledAppsL( installedArray );
    StartedButNotInstalledAppsL( startedAtBootArray, installedArray, startedNotInstalled );
    RemoveFromStartupListL( aDscStore, startedNotInstalled );

    CleanupStack::PopAndDestroy( 3, &startedAtBootArray );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetDataToBeImportedL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetDataToBeImportedL(
        RPointerArray<HBufC>& aExecutableArray,
        RPointerArray<HBufC>& aResourceFileArray )
    {
    TParse resourceFiles;
    User::LeaveIfError( resourceFiles.Set( KResourceFileSpec, &PrivateImportPathL(), NULL ) );
    const TPtrC importDir( resourceFiles.FullName() );

    CDir* dir;
    User::LeaveIfError( iFs.GetDir( importDir, KEntryAttMaskSupported, ESortNone, dir ) );
    CleanupStack::PushL( dir );
    for( TInt index = 0; index < dir->Count(); ++index )
        {
        TFileName resourceFile;
        TInt lastSeparator = importDir.LocateReverse( '\\' );
        if( lastSeparator > 0 )
            {
            resourceFile.Copy( importDir.Mid( 0, lastSeparator + 1 ) );
            resourceFile.Append( ( *dir )[ index ].iName );
            aResourceFileArray.AppendL( resourceFile.AllocL() );

            AppendExecutablesFromResourceFileL( resourceFile, aExecutableArray );
            }
        }
    CleanupStack::PopAndDestroy( dir );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ImportExecutablesL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ImportExecutablesL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aExecutableArray )
    {
    for( TInt index = 0; index < aExecutableArray.Count(); ++index )
        {
        const TDesC& executableName = *( aExecutableArray[ index ] );
        CDscItem* item = CDscItem::NewLC( executableName, KNullDesC );
        if( !aDscStore.ItemExistsL( *item ) )
            {
            aDscStore.AddItemL( *item );
            }
        CleanupStack::PopAndDestroy( item );
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::RemoveImportedResourceFiles()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::RemoveImportedResourceFiles(
        RPointerArray<HBufC>& aResourceFileArray )
    {
    for( TInt index = 0; index < aResourceFileArray.Count(); ++index )
        {
        const TDesC& fileName = *( aResourceFileArray[ index ] );
        TInt err = iFs.Delete( fileName );
        if( err )
            {
#ifdef _DEBUG
            RDebug::Print( _L("CStartupListUpdater: cannot delete %S, error %d"),
                    &fileName, err );
#endif
            }
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::AppendExecutablesFromResourceFileL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::AppendExecutablesFromResourceFileL(
        const TDesC& aResourceFile,
        RPointerArray<HBufC>& aExecutableArray )
    {
    RResourceFile resource;
    CleanupClosePushL( resource );
    resource.OpenL( iFs, aResourceFile );

    for( TInt id = KFirstStartupItemInfo; resource.OwnsResourceId( id ); ++id )
        {
        HBufC8* buffer = resource.AllocReadLC( id );

        TResourceReader reader;
        reader.SetBuffer( buffer );

        // Read STARTUP_ITEM_INFO
        TInt versionInfo = reader.ReadUint8();
        TFileName executableName;
        executableName.Copy( reader.ReadTPtrC() );
        TInt recoveryPolicy = reader.ReadUint16();

        CleanupStack::PopAndDestroy( buffer );

        if( versionInfo == 0 && recoveryPolicy == EStartupItemExPolicyNone )
            {
            // PKG files use '!' for drive letters that user can decide at installation time
            if( executableName.Length() > 0 && executableName[ 0 ] == '!' )
                {
                executableName.Replace( 0, 1, KAsterisk );

                TFindFile fileFinder( iFs );
                CDir* executableDir;            // next FindWildByDir needs AllFiles capability
                TInt err = fileFinder.FindWildByDir( executableName, KNullDesC, executableDir );
                if( !err )
                    {
                    CleanupStack::PushL( executableDir );
                    executableName.Replace( 0, 1, fileFinder.File() );
                    CleanupStack::PopAndDestroy( executableDir );
                    }
                }

            if( IsValidExecutableForStartupL( aResourceFile, executableName ) )
                {
                aExecutableArray.AppendL( executableName.AllocL() );
                }
            }
        }

    CleanupStack::PopAndDestroy( &resource );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsValidExecutableForStartupL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsValidExecutableForStartupL( const TDesC& aResourceFile,
        const TDesC& aExecutableName )
    {
    TBool isValid = EFalse;

    TEntry entry;
    if( iFs.Entry( aExecutableName, entry ) == KErrNone )   // needs AllFiles capability
        {
        isValid = ETrue;

        // Extract package UID from the resource file name
        // - allow both "[1234ABCD]" and "1234ABCD" formats
        // - allow possible "0x" prefix too
        TUid packageUid = KNullUid;
        TParsePtrC parse( aResourceFile );
        TPtrC parseName = parse.Name();
        TInt fileNameLength = parseName.Length();
        if( !parse.IsNameWild() && fileNameLength > 0 )
            {
            TPtr fileName( const_cast<TUint16*>( parseName.Ptr() ),
                fileNameLength, fileNameLength );

            if( fileName[ 0 ] == '[' && fileName[ fileNameLength - 1 ] == ']' )
                {
                const TInt KTwoCharsLength = 2;
                fileNameLength -= KTwoCharsLength;
                fileName = fileName.Mid( 1, fileNameLength );
                }

            _LIT( KHexPrefix, "0x" );
            const TInt KHexPrefixLength = 2;
            if( fileName.Left( KHexPrefixLength ) == KHexPrefix )
                {
                fileNameLength -= KHexPrefixLength;
                fileName = fileName.Mid( KHexPrefixLength, fileNameLength );
                }

            TLex lex( fileName );
            TUint32 uidValue = 0;
            TInt lexError = lex.Val( uidValue, EHex );
            if( !lexError )
                {
                packageUid.iUid = uidValue;
                }
            }

        // Get package info from RSisRegistry, and check that
        // - the package contains the resource file
        // - the package is properly signed
        if( packageUid != KNullUid )
            {
            Swi::RSisRegistrySession sisRegSession;
            User::LeaveIfError( sisRegSession.Connect() );
            CleanupClosePushL( sisRegSession );

            Swi::RSisRegistryEntry package;
            CleanupClosePushL( package );
            TInt openError = package.Open( sisRegSession, packageUid );
            if( !openError )
                {
                TBool hasResourceFile = EFalse;

                RPointerArray<HBufC> files;
                CleanupResetAndDestroyPushL( files );
                package.FilesL( files );
                for( TInt index = 0; index < files.Count() && !hasResourceFile; ++index )
                    {
                    hasResourceFile = ( aResourceFile.CompareF( *files[ index ] ) == 0 );
                    }

                if( hasResourceFile && package.TrustStatusL().IsTrusted() )
                    {
                    isValid = ETrue;
                    }

                CleanupStack::PopAndDestroy( &files );
                }

            CleanupStack::PopAndDestroy( 2, &sisRegSession );  // package, sisRegSession
            }
        }

    return isValid;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetInstalledAppsL(
        RPointerArray<HBufC>& aInstalledExecutableArray )
    {
    Swi::RSisRegistrySession sisRegistrySession;
    User::LeaveIfError( sisRegistrySession.Connect() );
    CleanupClosePushL( sisRegistrySession );

    RPointerArray<Swi::CSisRegistryPackage> removablePackages;
    CleanupResetAndDestroyPushL( removablePackages );
    sisRegistrySession.RemovablePackagesL( removablePackages );

    for( TInt index = 0; index < removablePackages.Count(); ++index )
        {
        Swi::RSisRegistryEntry entry;
        CleanupClosePushL( entry );
        entry.OpenL( sisRegistrySession, *( removablePackages[ index ] ) );
        if( entry.RemovableL() )
            {
            GetExecutablesFromEntryL( entry, aInstalledExecutableArray );
            }
        CleanupStack::PopAndDestroy( &entry );
        }

    CleanupStack::PopAndDestroy( 2, &sisRegistrySession );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetStartupListAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetStartupListAppsL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aStartedExecutableArray )
    {
    aDscStore.EnumOpenLC();

    while( CDscItem* item = aDscStore.EnumReadNextL() )
        {
        CleanupStack::PushL( item );
        aStartedExecutableArray.AppendL( item->FileName().AllocL() );
        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy();  // runs EnumClose
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::StartedButNotInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::StartedButNotInstalledAppsL(
        RPointerArray<HBufC>& aStartedExecutableArray,                  // in
        RPointerArray<HBufC>& aInstalledExecutableArray,                // in
        RPointerArray<HBufC>& aStartedButNotInstalledExecutableArray )  // out
    {
    aStartedButNotInstalledExecutableArray.ResetAndDestroy();

    TIdentityRelation<HBufC> identityRelation( FileNamesEqual );
    for( TInt index = 0; index < aStartedExecutableArray.Count(); ++index )
        {
        const HBufC* startedAppName = aStartedExecutableArray[ index ];
        if( aInstalledExecutableArray.Find( startedAppName, identityRelation ) == KErrNotFound )
            {
            aStartedButNotInstalledExecutableArray.AppendL( startedAppName->AllocL() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::RemoveFromStartupListL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::RemoveFromStartupListL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aExecutableArray )
    {
    for( TInt index = 0; index < aExecutableArray.Count(); ++index )
        {
        const TDesC& executableName = *( aExecutableArray[ index ] );
        CDscItem* item = CDscItem::NewLC( executableName, KNullDesC );
        if( aDscStore.ItemExistsL( *item ) )
            {
            aDscStore.DeleteItemL( *item );
            }
        CleanupStack::PopAndDestroy( item );
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetExecutablesFromEntryL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetExecutablesFromEntryL( Swi::RSisRegistryEntry& aEntry,
        RPointerArray<HBufC>& aExecutableArray )
    {
    RPointerArray<HBufC> filesList;
    CleanupResetAndDestroyPushL( filesList );
    aEntry.FilesL( filesList );

    for( TInt index = 0; index < filesList.Count(); ++index )
        {
        TParse parse;
        TInt err = parse.SetNoWild( KNullDesC, filesList[ index ], NULL );
        if( !err && ( parse.Ext().CompareF( KExecutableExtension ) == 0 ) )
            {
            aExecutableArray.AppendL( parse.FullName().AllocL() );
            }
        }

    CleanupStack::PopAndDestroy( &filesList );
    }

