/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Detailed error codes for KErrPermissionDenined errors.
*
*/

#ifndef SISXSIFPLUGINERRORS_H
#define SISXSIFPLUGINERRORS_H

enum TSisxSifPluginErrors
    {
    ESifUiTrustedUICapabilityRequired = 1,      // TrustedUI capability required.
    ESifUIAllFilesCapabilityRequired = 2,       // AllFiles capability required.
    ESifUiNeedsAllowUntrustedParameter = 3,     // Cannot install untrusted package without AllowUntrusted parameter.
    ESifUiNeedsPackageInfoParameter = 4,        // Cannot install package displaying infos without PackageInfo parameter.
    ESifUiNeedsAllowAppBreakDependencyParameter = 5, // Uninstall breaks dependencies. Cannot uninstall without AllowAppBreakDependency parameter.
    ESifUiNeedsAllowAppShutdownParameter = 6,   // Application is running. Cannot uninstall without AllowAppShutdown parameter.
    ESifUiNeedsAllowIncompatibleParameter = 7,  // Cannot install incompatible package without AllowIncompatible parameter.
    ESifUiNeedsAllowOverwriteParameter = 8,     // Cannot overwrite existing file which is not part of any package without AllowOverwrite parameter.
    ESifUiNeedsGrantCapabilitiesParameter = 9,  // Cannot install package requesting user capabilities without GrantCapabilities parameter.
    ESifUiCannotOverwriteFile = 99              // Cannot overwrite file
    };

#endif // SISXSIFPLUGINERRORS_H

