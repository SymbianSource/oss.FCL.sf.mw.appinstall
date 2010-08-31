/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// registry file extension
_LIT(KRegistryExt, "reg");
// registry controller file extension
_LIT(KControllerExt, "ctl");
// sis file filter 
_LIT(KSisFileFilter, "*.sis");
// registry file filter 
_LIT(KRegFileFilter, "*.reg");
// controller file filter 
_LIT(KControllerFileFilter, "*.ctl");

// swiDaemon private directory
_LIT(KSwiDaemonPrivateDirectory, "\\private\\10202dce\\");

///////////////Useful Formats ////////
// drive, path, uid, index
_LIT(KEntryFileNameFormat, "%S%S%08x\\%08x.reg");
// drive, path, uid, index, offset
_LIT(KControllerFileNameFormat, "%S%S%08x\\%08x_%04x.ctl");
// drive, uid, index
_LIT(KUninstallLogPathFormat, "%S\\sys\\uninstall\\%08x_%08x\\%c\\");

// the file name 'backup' is used here because as server is transient 
// in the periods when it is down this file serves as a backup of its memory state   
_LIT(KSysBackupFile, "!:\\sys\\install\\sisregistry\\backup.lst");
// backup of the KSysBackupFile
_LIT(KSysBackupFileBackup, "!:\\sys\\install\\sisregistry\\backup.bak");

_LIT(KIntegrityServicesPath, "\\sys\\install\\integrityservices\\");
	
_LIT(KLogDir, "\\sys\\install\\log.txt");
	
}

#endif // __SISREGISTRYSERVERCONST_H__
