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

#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x);
#define FLOG_1(x,y)     RDebug::Print(x, y);
#define FLOG_2(x,y,z)   RDebug::Print(x, y, z);
#define FLOG_3(x,y,z,v) RDebug::Print(x, y, z, v);
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#define FLOG_3(x,y,z,v)
#endif

_LIT( KDevTraceDir, "c:\\data\\logs\\swinst\\" );
_LIT( KDevTraceFile, "startuplistmgmt.log" );
const TInt KDevTraceBuffer = 256;


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
    FLOG( _L("CStartupListUpdater::NewL") );
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
    FLOG( _L("CStartupListUpdater::~CStartupListUpdater") );
    iLogFile.Close();
    iFileBuf.Close();
    delete iLogBuf;
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
    FLOG( _L("CStartupListUpdater::UpdateStartupListL") );
    if( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
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
    FLOG( _L("CStartupListUpdater::ConstructL, begin") );
    FeatureManager::InitializeLibL();
    User::LeaveIfError( iFs.Connect() );

    TInt err = iFs.MkDirAll( PrivateImportPathL() );
    if( err != KErrNone && err != KErrAlreadyExists )
        {
        FLOG_1( _L("CStartupListUpdater::ConstructL, err %d"), err );
        User::Leave( err );
        }
    FLOG( _L("CStartupListUpdater::ConstructL, end") );
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
    FLOG_1( _L("CStartupListUpdater::PrivateImportPathL %S"), iPrivateImportPath );
    return *iPrivateImportPath;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ProcessImportsAndUninstallsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ProcessImportsAndUninstallsL()
    {
    FLOG( _L("CStartupListUpdater::ProcessImportsAndUninstallsL, begin") );
    RDscStore dscStore;
    OpenDscStoreLC( dscStore );

    TRAPD( err, ImportNewResourceFilesL( dscStore ) );
    if( err )
        {
        DevTrace( _L("Error: import failed, error code %d"), err );
        User::Leave( err );
        }
    DeregisterUninstalledAppsL( dscStore );

    CleanupStack::PopAndDestroy( &dscStore );
    FLOG( _L("CStartupListUpdater::ProcessImportsAndUninstallsL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::OpenDscStoreLC()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::OpenDscStoreLC( RDscStore& aDscStore )
    {
    FLOG( _L("CStartupListUpdater::OpenDscStoreLC") );
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
        FLOG( _L("CStartupListUpdater::OpenDscStoreLC, KErrAlreadyExists") );
        User::Leave( KErrAlreadyExists );
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ImportNewResourceFilesL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ImportNewResourceFilesL( RDscStore& aDscStore )
    {
    FLOG( _L("CStartupListUpdater::ImportNewResourceFilesL, begin") );
    RPointerArray<HBufC> executableArray;
    CleanupResetAndDestroyPushL( executableArray );
    RPointerArray<HBufC> resourceFileArray;
    CleanupResetAndDestroyPushL( resourceFileArray );

    GetDataToBeImportedL( executableArray, resourceFileArray );
    ImportExecutablesL( aDscStore, executableArray );
    RemoveImportedResourceFiles( resourceFileArray );

    CleanupStack::PopAndDestroy( 2, &executableArray );
    FLOG( _L("CStartupListUpdater::ImportNewResourceFilesL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::DeregisterUninstalledAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::DeregisterUninstalledAppsL( RDscStore& aDscStore )
    {
    FLOG( _L("CStartupListUpdater::DeregisterUninstalledAppsL, begin") );
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
    FLOG( _L("CStartupListUpdater::DeregisterUninstalledAppsL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetDataToBeImportedL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetDataToBeImportedL(
        RPointerArray<HBufC>& aExecutableArray,
        RPointerArray<HBufC>& aResourceFileArray )
    {
    FLOG( _L("CStartupListUpdater::GetDataToBeImportedL, begin") );
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
    FLOG( _L("CStartupListUpdater::GetDataToBeImportedL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ImportExecutablesL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ImportExecutablesL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::ImportExecutablesL, begin") );
    for( TInt index = 0; index < aExecutableArray.Count(); ++index )
        {
        const TDesC& executableName = *( aExecutableArray[ index ] );
        FLOG_1( _L("CStartupListUpdater::ImportExecutablesL, exe %S"), &executableName );
        CDscItem* item = CDscItem::NewLC( executableName, KNullDesC );
        if( !aDscStore.ItemExistsL( *item ) )
            {
            FLOG( _L("CStartupListUpdater::ImportExecutablesL, add item to DscStore") );
            aDscStore.AddItemL( *item );
            DevTrace( _L("Success: '%S' will be started at boot"), &executableName );
            }
        else
            {
            DevTrace( _L("Error: '%S' already in start list"), &executableName );
            }
        CleanupStack::PopAndDestroy( item );
        }
    FLOG( _L("CStartupListUpdater::ImportExecutablesL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::RemoveImportedResourceFiles()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::RemoveImportedResourceFiles(
        RPointerArray<HBufC>& aResourceFileArray )
    {
    FLOG( _L("CStartupListUpdater::RemoveImportedResourceFiles, begin") );
    for( TInt index = 0; index < aResourceFileArray.Count(); ++index )
        {
        const TDesC& fileName = *( aResourceFileArray[ index ] );
        FLOG_1( _L("CStartupListUpdater::RemoveImportedResourceFiles, deleting %S"), &fileName );
        TInt err = iFs.Delete( fileName );
        FLOG_1( _L("CStartupListUpdater::RemoveImportedResourceFiles, err %d"), err );
        if( !err )
            {
            DevTrace( _L("Resource file '%S' processed"), &fileName );
            }
        }
    FLOG( _L("CStartupListUpdater::RemoveImportedResourceFiles, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::AppendExecutablesFromResourceFileL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::AppendExecutablesFromResourceFileL(
        const TDesC& aResourceFile,
        RPointerArray<HBufC>& aExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, begin") );
    OpenDevTraceL();
    DevTrace( _L("Processing resource file: %S"), &aResourceFile );

    RResourceFile resource;
    CleanupClosePushL( resource );
    resource.OpenL( iFs, aResourceFile );
    FLOG_1( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, %S"), &aResourceFile );

    for( TInt id = KFirstStartupItemInfo; resource.OwnsResourceId( id ); ++id )
        {
        FLOG_1( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, resource id %d"), id );
        HBufC8* buffer = resource.AllocReadLC( id );

        TResourceReader reader;
        reader.SetBuffer( buffer );

        // Read STARTUP_ITEM_INFO
        TInt versionInfo = reader.ReadUint8();
        TFileName executableName;
        executableName.Copy( reader.ReadTPtrC() );
        TInt recoveryPolicy = reader.ReadUint16();
        FLOG_1( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, executableName %S"),
                &executableName );
        FLOG_1( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, recoveryPolicy %d"),
                recoveryPolicy );

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

                FLOG_1( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, new name %S"),
                        &executableName );
                }

            DevTrace( _L("Executable file: %S"), &executableName );
            if( IsValidExecutableForStartupL( aResourceFile, executableName ) )
                {
                FLOG( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, adding to array") );
                aExecutableArray.AppendL( executableName.AllocL() );
                }
            }
        else
            {
            if( versionInfo != 0 )
                {
                DevTrace( _L("Error: unsupported STARTUP_ITEM_INFO version: %d"), versionInfo );
                }
            else
                {
                DevTrace( _L("Error: unsupported recovery policy: %d"), recoveryPolicy );
                }
            }
        }

    CleanupStack::PopAndDestroy( &resource );
    FLOG( _L("CStartupListUpdater::AppendExecutablesFromResourceFileL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsValidExecutableForStartupL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsValidExecutableForStartupL( const TDesC& aResourceFile,
        const TDesC& aExecutableName )
    {
    FLOG( _L("CStartupListUpdater::IsValidExecutableForStartupL, begin") );
    FLOG_1( _L("CStartupListUpdater::IsValidExecutableForStartupL, res %S"), &aResourceFile );
    FLOG_1( _L("CStartupListUpdater::IsValidExecutableForStartupL, exe %S"), &aExecutableName );
    TBool isValid = EFalse;

    TEntry entry;
    if( iFs.Entry( aExecutableName, entry ) == KErrNone )   // needs AllFiles capability
        {
        FLOG( _L("CStartupListUpdater::IsValidExecutableForStartupL, exe file exists") );

        TUid packageUid = KNullUid;
        ExtractPackageUidFromResourceFileL( aResourceFile, packageUid );

        if( packageUid != KNullUid )
            {
            Swi::RSisRegistrySession sisRegSession;
            User::LeaveIfError( sisRegSession.Connect() );
            CleanupClosePushL( sisRegSession );

            isValid = IsResourceFileValidForPackageL( sisRegSession, aResourceFile, packageUid ) &&
                    IsExeFileIncludedInPackageL( sisRegSession, aExecutableName, packageUid );

            CleanupStack::PopAndDestroy( &sisRegSession );
            }
        else
            {
            DevTrace( _L("Error: invalid package UID - check resource file name") );
            }
        }
    else
        {
        DevTrace( _L("Error: executable file '%S' not found"), &aExecutableName );
        }

    FLOG_1( _L("CStartupListUpdater::IsValidExecutableForStartupL, return %d"), isValid );
    return isValid;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::ExtractPackageUidFromResourceFileL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::ExtractPackageUidFromResourceFileL( const TDesC& aResourceFile,
        TUid& aPackageUid )
    {
    FLOG( _L("CStartupListUpdater::ExtractPackageUidFromResourceFileL, begin") );

    // Extract package UID from the resource file name
    // - allow both "[1234ABCD]" and "1234ABCD" formats
    // - allow possible "0x" prefix too
    aPackageUid = KNullUid;

    TParse parse;
    parse.Set( aResourceFile, NULL, NULL );
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

        FLOG_1( _L("CStartupListUpdater::ExtractPackageUidFromResourceFileL, fileName %S"),
                &fileName );
        TLex lex( fileName );
        TUint32 uidValue = 0;
        TInt lexError = lex.Val( uidValue, EHex );
        if( !lexError )
            {
            aPackageUid.iUid = uidValue;
            }
        }

    FLOG_1( _L("CStartupListUpdater::ExtractPackageUidFromResourceFileL, end, ret 0x%08x"),
            aPackageUid.iUid );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsResourceFileValidForPackageL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsResourceFileValidForPackageL(
        Swi::RSisRegistrySession& aSisRegSession,
        const TDesC& aResourceFile,
        const TUid& aPackageUid )
    {
    FLOG( _L("CStartupListUpdater::IsResourceFileValidForPackageL, begin") );
    TBool isValidForPackage = EFalse;

    // Get package info from RSisRegistry, and check that
    // - the package contains the resource file, and
    // - the package is properly signed

    Swi::RSisRegistryEntry package;
    CleanupClosePushL( package );
    TInt openError = package.Open( aSisRegSession, aPackageUid );
    FLOG_1( _L("CStartupListUpdater::IsResourceFileValidForPackageL, openError %d"), openError );
    if( !openError )
        {
        Swi::TSisTrustStatus trustStatus = package.TrustStatusL();
        Swi::TValidationStatus validationStatus = trustStatus.ValidationStatus();
        FLOG_1( _L("CStartupListUpdater::IsResourceFileValidForPackageL, validationStatus %d"),
                validationStatus );
        Swi::TRevocationStatus revocationStatus = trustStatus.RevocationStatus();
        FLOG_1( _L("CStartupListUpdater::IsResourceFileValidForPackageL, revocationStatus %d"),
                revocationStatus );
        TBool isTrusted = (validationStatus == Swi::EPackageInRom ||
                validationStatus == Swi::EValidatedToAnchor) &&
                revocationStatus != Swi::EOcspRevoked;
        FLOG_1( _L("CStartupListUpdater::IsResourceFileValidForPackageL, isTrusted %d"),
                isTrusted );

        if( isTrusted )
            {
            isValidForPackage = IsFileIncludedInPackageL( aResourceFile, package );

            if( !isValidForPackage )
                {
                DevTrace( _L("Error: package 0x%08x does not install resource file %S"),
                        aPackageUid.iUid, &aResourceFile );
                }
            }
        else
            {
            DevTrace( _L("Error: package 0x%08x not trusted"), aPackageUid.iUid );
            }
        }
    else
        {
        DevTrace( _L("Error: package 0x%08x open failed, error %d - check resource file name"),
                aPackageUid.iUid, openError );
        }

    CleanupStack::PopAndDestroy( &package );

    FLOG_1( _L("CStartupListUpdater::IsResourceFileValidForPackageL, ret %d"), isValidForPackage );
    return isValidForPackage;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsFileIncludedInPackageL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsFileIncludedInPackageL( const TDesC& aFile,
        Swi::RSisRegistryEntry& aPackage )
    {
    FLOG_1( _L("CStartupListUpdater::IsFileIncludedInPackageL, begin, file %S"), &aFile );
    TBool isIncluded = EFalse;

    RPointerArray<HBufC> files;
    CleanupResetAndDestroyPushL( files );
    aPackage.FilesL( files );
    TInt fileCount = files.Count();
    FLOG_1( _L("CStartupListUpdater::IsFileIncludedInPackageL, pckg fileCount %d"), fileCount );
    for( TInt index = 0; index < fileCount && !isIncluded; ++index )
        {
        TPtrC pckgFile( *files[ index ] );
        FLOG_1( _L("CStartupListUpdater::IsFileIncludedInPackageL, pckgFile %S"), &pckgFile );
        isIncluded = ( aFile.CompareF( pckgFile ) == 0 );
        }
    CleanupStack::PopAndDestroy( &files );

    if( isIncluded )
        {
        DevTrace( _L("File '%S' installed by package 0x%08x" ), &aFile, aPackage.UidL().iUid );
        }
    else
        {
        DevTrace( _L("File '%S' not installed by package 0x%08x" ), &aFile, aPackage.UidL().iUid );
        }
    FLOG_1( _L("CStartupListUpdater::IsFileIncludedInPackageL, ret %d"), isIncluded );
    return isIncluded;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL(
        Swi::RSisRegistrySession& aSisRegSession, const TDesC& aFile,
        Swi::RSisRegistryEntry& aPackage )
    {
    FLOG( _L("CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL, begin") );
    TBool isIncluded = EFalse;

    RPointerArray<Swi::CSisRegistryPackage> embeddedPackages;
    CleanupResetAndDestroyPushL( embeddedPackages );
    aPackage.EmbeddedPackagesL( embeddedPackages );

    TInt embeddedPackagesCount = embeddedPackages.Count();
    FLOG_1( _L("CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL, embeddedPackagesCount %d"),
            embeddedPackagesCount );
    for( TInt index = 0; index < embeddedPackagesCount && !isIncluded; ++index )
        {
        TUid uid = embeddedPackages[index]->Uid();
        FLOG_1( _L("CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL, uid 0x%08x"),
                uid.iUid );

        Swi::RSisRegistryEntry package;
        if( package.Open( aSisRegSession, uid ) == KErrNone )
            {
            CleanupClosePushL( package );
            isIncluded = IsFileIncludedInPackageL( aFile, package );
            if( !isIncluded )
                {
                // Check other embedded packages recursively if necessary
                isIncluded = IsFileIncludedInEmbeddedPackagesL( aSisRegSession, aFile, package );
                }
            CleanupStack::PopAndDestroy( &package );
            }
        }
    CleanupStack::PopAndDestroy( &embeddedPackages );

    FLOG_1( _L("CStartupListUpdater::IsFileIncludedInEmbeddedPackagesL, ret %d"), isIncluded );
    return isIncluded;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::IsExeFileIncludedInPackageL()
// ---------------------------------------------------------------------------
//
TBool CStartupListUpdater::IsExeFileIncludedInPackageL(
        Swi::RSisRegistrySession& aSisRegSession,
        const TDesC& aExeFile, const TUid& aPackageUid )
    {
    FLOG_1( _L("CStartupListUpdater::IsExeFileIncludedInPackageL, begin, file %S"), &aExeFile );
    TBool isIncluded = EFalse;

    Swi::RSisRegistryEntry package;
    CleanupClosePushL( package );
    TInt openError = package.Open( aSisRegSession, aPackageUid );
    if( !openError )
        {
        isIncluded = IsFileIncludedInPackageL( aExeFile, package ) ||
                IsFileIncludedInEmbeddedPackagesL( aSisRegSession, aExeFile, package );
        if( !isIncluded )
            {
            DevTrace( _L("Error: exe not installed by this package or it's subpackages") );
            }
        }
    CleanupStack::PopAndDestroy( &package );

    FLOG_1( _L("CStartupListUpdater::IsExeFileIncludedInPackageL, return %d"), isIncluded );
    return isIncluded;
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetInstalledAppsL(
        RPointerArray<HBufC>& aInstalledExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::GetInstalledAppsL, begin") );
    Swi::RSisRegistrySession sisRegistrySession;
    User::LeaveIfError( sisRegistrySession.Connect() );
    CleanupClosePushL( sisRegistrySession );

    RPointerArray<Swi::CSisRegistryPackage> removablePackages;
    CleanupResetAndDestroyPushL( removablePackages );
    sisRegistrySession.RemovablePackagesL( removablePackages );

    TInt removablePackagesCount = removablePackages.Count();
    FLOG_1( _L("CStartupListUpdater::GetInstalledAppsL, removablePackagesCount %d"),
            removablePackagesCount );
    for( TInt index = 0; index < removablePackagesCount; ++index )
        {
        FLOG_1( _L("CStartupListUpdater::GetInstalledAppsL, index %d"), index );
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
    FLOG( _L("CStartupListUpdater::GetInstalledAppsL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetStartupListAppsL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetStartupListAppsL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aStartedExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::GetStartupListAppsL, begin") );
    aDscStore.EnumOpenLC();

    while( CDscItem* item = aDscStore.EnumReadNextL() )
        {
        CleanupStack::PushL( item );
        TPtrC fileName( item->FileName() );
        FLOG_1( _L("CStartupListUpdater::GetStartupListAppsL, fileName %S"), &fileName );
        aStartedExecutableArray.AppendL( fileName.AllocL() );
        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy();  // runs EnumClose
    FLOG( _L("CStartupListUpdater::GetStartupListAppsL, end") );
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
    FLOG( _L("CStartupListUpdater::StartedButNotInstalledAppsL, begin") );
    aStartedButNotInstalledExecutableArray.ResetAndDestroy();

    TIdentityRelation<HBufC> identityRelation( FileNamesEqual );
    for( TInt index = 0; index < aStartedExecutableArray.Count(); ++index )
        {
        const HBufC* startedAppName = aStartedExecutableArray[ index ];
        FLOG_1( _L("CStartupListUpdater::StartedButNotInstalledAppsL, startedAppName %S"),
                startedAppName );
        if( aInstalledExecutableArray.Find( startedAppName, identityRelation ) == KErrNotFound )
            {
            FLOG( _L("CStartupListUpdater::StartedButNotInstalledAppsL, adding to array") );
            aStartedButNotInstalledExecutableArray.AppendL( startedAppName->AllocL() );
            }
        }
    FLOG( _L("CStartupListUpdater::StartedButNotInstalledAppsL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::RemoveFromStartupListL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::RemoveFromStartupListL( RDscStore& aDscStore,
        RPointerArray<HBufC>& aExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::RemoveFromStartupListL, begin") );
    for( TInt index = 0; index < aExecutableArray.Count(); ++index )
        {
        const TDesC& executableName = *( aExecutableArray[ index ] );
        FLOG_1( _L("CStartupListUpdater::RemoveFromStartupListL, executableName %S"),
                &executableName );
        CDscItem* item = CDscItem::NewLC( executableName, KNullDesC );
        if( aDscStore.ItemExistsL( *item ) )
            {
            FLOG( _L("CStartupListUpdater::RemoveFromStartupListL, removing from DscStore") );
            aDscStore.DeleteItemL( *item );
            }
        CleanupStack::PopAndDestroy( item );
        }
    FLOG( _L("CStartupListUpdater::RemoveFromStartupListL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::GetExecutablesFromEntryL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::GetExecutablesFromEntryL( Swi::RSisRegistryEntry& aEntry,
        RPointerArray<HBufC>& aExecutableArray )
    {
    FLOG( _L("CStartupListUpdater::GetExecutablesFromEntryL, begin") );
    RPointerArray<HBufC> filesList;
    CleanupResetAndDestroyPushL( filesList );
    aEntry.FilesL( filesList );

    for( TInt index = 0; index < filesList.Count(); ++index )
        {
        TParse parse;
        TInt err = parse.SetNoWild( KNullDesC, filesList[ index ], NULL );
        FLOG_3( _L("CStartupListUpdater::GetExecutablesFromEntryL, index %d, parse %S, err %d"),
                index, &(parse.FullName()), err );
        if( !err && ( parse.Ext().CompareF( KExecutableExtension ) == 0 ) )
            {
            FLOG( _L("CStartupListUpdater::GetExecutablesFromEntryL, adding to array") );
            aExecutableArray.AppendL( parse.FullName().AllocL() );
            }
        else
            {
            FLOG( _L("CStartupListUpdater::GetExecutablesFromEntryL, skipped") );
            }
        }

    CleanupStack::PopAndDestroy( &filesList );
    FLOG( _L("CStartupListUpdater::GetExecutablesFromEntryL, end") );
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::OpenDevTraceL()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::OpenDevTraceL()
    {
    if( !iLogBuf )
        {
        TEntry entry;
        if( iFs.Entry( KDevTraceDir, entry ) == KErrNone )
            {
            FLOG( _L("CStartupListUpdater::OpenDevTraceL, log dir exists") );
            TFileName fileName;
            fileName.Copy( KDevTraceDir );
            fileName.Append( KDevTraceFile );
            if( iLogFile.Replace( iFs, fileName, EFileWrite ) == KErrNone )
                {
                iLogBuf = HBufC::NewL( KDevTraceBuffer );
                iFileBuf.CreateL( KDevTraceBuffer );
                FLOG( _L("CStartupListUpdater::OpenDevTraceL, log file opened") );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CStartupListUpdater::DevTrace()
// ---------------------------------------------------------------------------
//
void CStartupListUpdater::DevTrace( TRefByValue<const TDesC> aFmt, ... )
    {
    if( iLogBuf )
        {
        VA_LIST list;
        VA_START( list, aFmt );
        TPtr line( iLogBuf->Des() );
        line.FormatList( aFmt, list );
        FLOG_1( _L("CStartupListUpdater::DevTrace: %S"), iLogBuf );

        const TChar KNewLine( '\n' );
        line.Append( KNewLine );
        iFileBuf.Copy( line );
        iLogFile.Write( iFileBuf );  // return value ignored
        }
    }

