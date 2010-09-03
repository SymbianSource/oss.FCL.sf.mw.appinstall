/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IAUPDATEUTILS_H
#define IAUPDATEUTILS_H

#include <e32base.h>
#include <driveinfo.h>

// For silent installation
#include <usif/sif/sif.h>
#include <swi/sisregistryentry.h>

class TIAUpdateVersion;
class MIAUpdateNode;
class CIAUpdateBaseNode;
class RWidgetRegistryClientSession;

namespace IAUpdateUtils
{

/**
 * Converts descriptor containing hexadecimal number to an integer.
 * Accepts "ABCD" and "0xABCD" format numbers.
 * Leaves if corrupt parameter.
 * @param aDes Descriptor representation of a hexadecimal.
 * @return TInt Converted integer.
 */
IMPORT_C TInt DesHexToIntL( const TDesC& aDes );

/**
 * Converts a version string of format <major>.<minor>.<build> to version components.
 * 
 * @note If part of the version information is not included in the aVersion,
 * then missing version parts are not set and those version part references will 
 * contain their original values. So, caller needs to initialize them correctly before
 * calling this function.
 *
 * @param aStr Version string
 * @param aMajor On return contains the major version.
 * @param aMinor On return contains the minor version.
 * @param aBuild On return contains the build number.
*/
IMPORT_C void DesToVersionL( const TDesC& aVersion, 
                             TInt8& aMajor, TInt8& aMinor, TInt16& aBuild );

/**
 * Checks if application is installed.
 * Function is able to check if the given UID is SID or pUID. If SID is given then
 * the package where it is included is returned. If pUID is given, then the pUID is
 * returned.
 * @param aUid SID or pUID of the application
 * @return TUid pUID of the application. If app not found TUid::Null() returned.
 **/
IMPORT_C TUid AppPackageUidL( const TUid& aUid );

/**
 * Checks if application is installed, and if so, returns its version and the pUid.
 * Function is able to check if the given UID is SID or pUID. If SID is given then
 * the package where it is included is returned. If pUID is given, then the pUID is
 * returned.
 * @param aUid SID or pUID of the application
 * @param aVersion Returned version if app is found. If app not found, unchanged
 * @return TUid pUID of the application. If app not found TUid::Null() returned.
 **/
IMPORT_C TUid AppPackageUidL( const TUid& aUid, TIAUpdateVersion &aVersion );


/**
 * Checks if application is installed.
 * Function is able to check if the given UID is SID or pUID.
 * @param aUid SID or pUID of the application
 * @return ETrue if application is installed on the device
 **/
IMPORT_C TBool IsAppInstalledL( const TUid& aUid );    

/**
 * Checks if application is installed, and if so, returns its version.
 * Function is able to check if the given UID is SID or pUID.
 * @param aUid SID or pUID of the application
 * @param aVersion Returned version if app is found. If app not found, unchanged
 * @return ETrue if application is installed on the device
 **/
IMPORT_C TBool IsAppInstalledL( const TUid& aUid, TIAUpdateVersion &aVersion );

/**
 * This function is added to check whether certain widget with given identifier are installed
 * the function calls widget registry API
 **/
IMPORT_C TBool IsWidgetInstalledL( const TDesC& aIdentifier, TIAUpdateVersion &aVersion );

IMPORT_C TBool SpaceAvailableInInternalDrivesL( 
                                         RPointerArray<MIAUpdateNode>& aNodes );

/**
 * 
 * Reads from sis registry existence of exe in installation
 * 
 * @param aPUid Package UID of installation
 * @param aExecutable Exe file name 
 *
 * @return ETrue if exe is found in gvben installation
 **/
IMPORT_C TBool IsInstalledL( const TUid& aPUid, const TDesC& aExecutable );  


/**
 * Creates options for silent install.
 * Uses DriveToInstallL to determine target drive to install.
 * 
 * @param aOptions  Silent install options
 */
void  UsifSilentInstallOptionsL( 
        const CIAUpdateBaseNode& aNode,Usif::COpaqueNamedParams * aOptions );

/**
 * Finds drive where a package is currently installed
 * 
 * @param aLocationDrive Drive where a package is currently installed
 * @return ETrue if a package previously installed to an available drive
 **/
TBool InstalledDriveL( RFs& aFs, const TUid& aUid, TDriveUnit& aLocationDrive );

void InstalledDriveWidgetL( TDriveUnit& aLocationDrive );

TBool NextInternalDriveL( RFs& aFs, 
                          TDriveUnit aCurrentDrive, 
                          TDriveUnit& aNextDrive );

/**
 * Determines target drive to install
 * 
 * @param aUid PUID of the application
 * @param aSize Estimated size of installation
 * @return Drive to install
 **/
TDriveUnit DriveToInstallL( const TUid& aUid, TInt aSize );

TDriveUnit DriveToInstallWidgetL( const TDesC& aIdentifier );

TDriveUnit BiggestInternalDriveL();

TBool InternalDriveWithSpaceL( TInt aSize, 
                               TDriveUnit aPreferredDriveUnit, 
                               TDriveUnit& aTargetDriveUnit );

void SaveCurrentFwVersionIfNeededL();

TBool IsFirmwareChangedL(); 

TInt64 FreeDiskSpace( RFs& aFs, TInt aDriveNumber );

void DrivesWithBinariesL( Swi::RSisRegistryEntry& aEntry, RArray<TInt>& aDrives );

}


#endif  //  IAUPDATEUTILS_H
