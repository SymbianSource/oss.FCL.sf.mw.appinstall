/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Error codes defined by the Universal Software Install Framework.
*
*/


/**
 @file
 @publishedAll
 @released 
*/

#ifndef USIFERROR_H
#define USIFERROR_H


namespace Usif
	{

		enum TErrorCategory {
			ENone						= 0, // No Error
			ELowMemory					= 1, // Low on RAM
			ELowDiskSpace				= 2, // Low diskspace
			ENetworkUnavailable			= 3, // Network not available
			EInstallerBusy				= 4, // Installer is in use
			ECorruptedPackage			= 5, // Package corrupt
			EApplicationNotCompatible	= 6, // Not compatible
			ESecurityError				= 7, // Security Error
			EUnexpectedError			= 8, // Unexpected Error
			EUserCancelled				= 9, // Install Cancelled
			EUninstallationBlocked		= 10,// Uninstallation blocked
			EUnknown					= 11 // Unknown error
		};

	}


/** A general error in one of the Unified Installer Framework components. */
const TInt KErrSifUnknown = -10300;

/** Installation of a software upgrade could not complete because the package being upgraded was not installed on the device. */
const TInt KErrSifMissingBasePackage = -10301;

/** Installation of a software component could not complete because one or more of the packages it depends on are not present on the device. */
const TInt KErrSifMissingDependencies = -10302;

/** Installation of a software component could not complete because there was no matching installer for the package. */
const TInt KErrSifUnsupportedSoftwareType = -10303;

/** Installation of a software component failed because the delivery package was corrupt. */
const TInt KErrSifCorruptedPackage = -10304;

/** Installation of a software component failed since the parameters passed via the Software Install Framework were too large. */
const TInt KErrSifOverflow = -10307;

/** Installation of a software component failed because the same version of the component is already installed on the system. */
const TInt KErrSifSameVersionAlreadyInstalled = -10309;

/** Installation of a software component failed because a newer version of the same component is already installed on the system. */
const TInt KErrSifNewerVersionAlreadyInstalled = -10310;

/** Activation of a component failed because it was already activated. */
const TInt KErrSifAlreadyActivated = -10311;

/** Deactivation of a component failed because it was already inactive. */
const TInt KErrSifAlreadyDeactivated = -10312;

/** The component id specified is not installed on the system. */
const TInt KErrSifBadComponentId = -10313;

/** The component was not installed due to lack of free space on the target drive. */
const TInt KErrSifNotEnoughSpace = -10314;

/** The component was not installed due to an internal problem with the corresponding installer. */
const TInt KErrSifBadInstallerConfiguration = -10315;

/** Installation of a component failed because the package is not targeted for this device. */
const TInt KErrSifPackageCannotBeInstalledOnThisDevice = -10316;

/** Installation of a component failed because the component's language is not supported this device. */
const TInt KErrSifUnsupportedLanguage = -10317;

/** At least one writing operation is in progress on the SCR server. A new transaction or subsession cannot be created.*/
const TInt KErrScrWriteOperationInProgress = -10320;

/** At least one reading operation exists on the SCR server. A new transaction cannot be created. */
const TInt KErrScrReadOperationInProgress = -10321;

/** There is no active transaction on the SCR Server owned by the calling session. */
const TInt KErrScrNoActiveTransaction = -10322;

/** The requested value couldn't be found for the specified locale. */
const TInt KErrScrUnsupportedLocale = -10323;

#endif // USIFERROR_H
