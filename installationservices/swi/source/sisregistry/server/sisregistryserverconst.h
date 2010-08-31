/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* sisregistry server constants
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYSERVERCONST_H__
#define __SISREGISTRYSERVERCONST_H__

namespace Swi
{
// registry main path
_LIT(KRegistryPath, "\\sys\\install\\sisregistry\\");
// preinstalled components location
_LIT(KPreInstalledPath, "\\system\\install\\");
// controller file filter 
_LIT(KControllerFileFilter, "*.ctl");

// Private path
_LIT(KPrivatePathFormat, "?:\\private\\");

// swiDaemon private directory
_LIT(KSwiDaemonPrivateDirectory, "\\private\\10202dce\\");

///////////////Useful Formats ////////
// drive, path, uid, index, offset
_LIT(KControllerFileNameFormat, "%S%S%08x\\%08x_%04x.ctl");
// drive, uid, index
_LIT(KUninstallLogPathFormat, "%c:\\sys\\uninstall\\%08x_%08x\\%c\\uninstall.log");

_LIT(KLogDir, "\\sys\\install\\log.txt");

//Location of application registration resource files
_LIT(KApparcRegFilePath, "\\private\\10003a3f\\apps\\*.rsc");
_LIT(KApparcRegFileImportPath, "\\private\\10003a3f\\import\\apps\\*.rsc");

//Registration resource file uid 
const TUid KUidAppRegistrationFile = {0x101F8021};
_LIT(KApparcRegImportDir, "\\private\\10003a3f\\import\\apps\\");
_LIT(KApparcRegDir, "\\private\\10003a3f\\apps\\");
}

#endif // __SISREGISTRYSERVERCONST_H__
