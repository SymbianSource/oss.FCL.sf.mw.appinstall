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
* Description:   Utility functions for drive and file handling
*
*/


#include "appmngr2driveutils.h"         // TAppMngr2DriveUtils
#include "appmngr2debugutils.h"         // FLOG macros
#include <driveinfo.h>                  // DriveInfo
#include <bautils.h>                    // BaflUtils
#include <data_caging_path_literals.hrh> // KDC_* constant strings

_LIT( KDriveFormat, "%c:" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TAppMngr2DriveUtils::LocationFromFileName()
// ---------------------------------------------------------------------------
//
EXPORT_C TAppMngr2Location TAppMngr2DriveUtils::LocationFromFileNameL(
        const TDesC& aFileName, RFs& aFs )
    {
    TParsePtrC fileNameParser( aFileName );
    const TChar driveLetter( fileNameParser.Drive()[ 0 ] );
    TInt driveNumber;
    User::LeaveIfError( aFs.CharToDrive( driveLetter, driveNumber ) );
    return LocationFromDriveL( driveNumber, aFs );
    }

// ---------------------------------------------------------------------------
// TAppMngr2DriveUtils::LocationFromDrive()
// ---------------------------------------------------------------------------
//
EXPORT_C TAppMngr2Location TAppMngr2DriveUtils::LocationFromDriveL(
        TInt aDrive, RFs& aFs )
    {
    TUint driveStatus = 0;
    User::LeaveIfError( DriveInfo::GetDriveStatus( aFs, aDrive, driveStatus ) );
    if( driveStatus & DriveInfo::EDriveRemovable )
        {
        return EAppMngr2LocationMemoryCard;
        }
    if( driveStatus & DriveInfo::EDriveExternallyMountable )
        {
        return EAppMngr2LocationMassStorage;
        }
    return EAppMngr2LocationPhone;
    }

// ---------------------------------------------------------------------------
// TAppMngr2DriveUtils::NearestResourceFileLC()
// ---------------------------------------------------------------------------
//
EXPORT_C TFileName* TAppMngr2DriveUtils::NearestResourceFileLC(
        const TDesC& aFileName, RFs& aFs )
    {
    FLOG( "NearestResourceFileLC( %S )", &aFileName );
    
    TInt romDriveNumber = KErrNotFound;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, romDriveNumber );
    TChar romDriveLetter;
    User::LeaveIfError( aFs.DriveToChar( romDriveNumber, romDriveLetter ) );

    TFileName* fullName = new (ELeave) TFileName;
    CleanupStack::PushL( fullName );

    fullName->Format( KDriveFormat, static_cast<TUint>( romDriveLetter ) );
    fullName->Append( KDC_RESOURCE_FILES_DIR );
    fullName->Append( aFileName );
    
    BaflUtils::NearestLanguageFile( aFs, *fullName );
    FLOG( "NearestResourceFileLC, returns %S", fullName );
    return fullName;
    }

// ---------------------------------------------------------------------------
// TAppMngr2DriveUtils::FullBitmapFileNameLC()
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* TAppMngr2DriveUtils::FullBitmapFileNameLC(
        const TDesC& aBitmapFile, RFs& aFs )
    {
    FLOG( "FullBitmapFileNameLC( %S )", &aBitmapFile );
    TInt romDriveNumber = KErrNotFound;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, romDriveNumber );
    TChar romDriveLetter;
    User::LeaveIfError( aFs.DriveToChar( romDriveNumber, romDriveLetter ) );

    HBufC* fullName = HBufC::NewLC( KMaxFileName );
    TPtr fullNameDes = fullName->Des();

    fullNameDes.Format( KDriveFormat, static_cast<TUint>( romDriveLetter ) );
    fullNameDes.Append( KDC_APP_BITMAP_DIR );
    fullNameDes.Append( aBitmapFile );

    FLOG( "FullBitmapFileNameLC, returns %S", fullName );
    return fullName;
    }

