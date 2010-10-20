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
* Description:  Header file for startup list updater.
*
*/

#ifndef STARTUPLISTUPDATER_H
#define STARTUPLISTUPDATER_H

#include <e32base.h>                    // CBase
#include <f32file.h>                    // RFs, RFile
#include <dscstore.h>                   // RDscStore

namespace Swi
    {
    class RSisRegistrySession;
    class RSisRegistryEntry;
    }

class CStartupListUpdater : public CBase
    {
public:     // constructor and destructor
    static CStartupListUpdater* NewL();
    ~CStartupListUpdater();

public:     // new functions
    void UpdateStartupListL();

private:    // new functions
    CStartupListUpdater();
    void ConstructL();
    const TDesC& PrivateImportPathL();
    void ProcessImportsAndUninstallsL();
    void OpenDscStoreLC( RDscStore& aDscStore );
    void ImportNewResourceFilesL( RDscStore& aDscStore );
    void DeregisterUninstalledAppsL( RDscStore& aDscStore );
    void GetDataToBeImportedL( RPointerArray<HBufC>& aExecutableArray,
            RPointerArray<HBufC>& aResourceFileArray );
    void ImportExecutablesL( RDscStore& aDscStore, RPointerArray<HBufC>& aExecutableArray );
    void RemoveImportedResourceFiles( RPointerArray<HBufC>& aResourceFileArray );
    void AppendExecutablesFromResourceFileL( const TDesC& aResourceFile,
            RPointerArray<HBufC>& aExecutableArray );
    TBool IsValidExecutableForStartupL( const TDesC& aResourceFile,
            const TDesC& aExecutableName );
    void ExtractPackageUidFromResourceFileL( const TDesC& aResourceFile,
            TUid& aPackageUid );
    TBool IsResourceFileValidForPackageL( Swi::RSisRegistrySession& aSisRegSession,
            const TDesC& aResourceFile, const TUid& aPackageUid );
    TBool IsFileIncludedInPackageL( const TDesC& aFile, Swi::RSisRegistryEntry& aPackage );
    TBool IsFileIncludedInEmbeddedPackagesL( Swi::RSisRegistrySession& aSisRegSession,
            const TDesC& aFile, Swi::RSisRegistryEntry& aPackage );
    TBool IsExeFileIncludedInPackageL( Swi::RSisRegistrySession& aSisRegSession,
            const TDesC& aExeFile, const TUid& aPackageUid );
    void GetInstalledAppsL( RPointerArray<HBufC>& aInstalledExecutableArray );
    void GetStartupListAppsL( RDscStore& aDscStore,
            RPointerArray<HBufC>& aStartedExecutableArray );
    void StartedButNotInstalledAppsL( RPointerArray<HBufC>& aStartedExecutableArray,
            RPointerArray<HBufC>& aInstalledExecutableArray,
            RPointerArray<HBufC>& aStartedButNotInstalledExecutableArray );
    void RemoveFromStartupListL( RDscStore& aDscStore, RPointerArray<HBufC>& aExecutableArray );
    void GetExecutablesFromEntryL( Swi::RSisRegistryEntry& aEntry,
            RPointerArray<HBufC>& aExecutableArray );
    void OpenDevTraceL();
    void DevTrace( TRefByValue<const TDesC> aFmt, ... );

private:    // new data
    RFs iFs;
    HBufC* iPrivateImportPath;
    RFile iLogFile;
    RBuf8 iFileBuf;
    HBufC* iLogBuf;  // NULL if logging disabled
    };

#endif  // STARTUPLISTUPDATER_H

