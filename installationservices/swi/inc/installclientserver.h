/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common header for the server and the client.
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __INSTALLCLIENTSERVER_H__
#define __INSTALLCLIENTSERVER_H__

#include <e32std.h>

namespace Swi
{
class CSisRegistryPackage;

_LIT(KInstallServerName, "!InstallServer");
_LIT(KInstallServerImage, "InstallServer");

const TUid KInstallServerUid3 = { 0x101F7295 };

/// The current version of the Install Server
const TInt KInstallServerVersionMajor=0;
const TInt KInstallServerVersionMinor=0;
const TInt KInstallServerVersionBuild=0;

/**
 * Messages Launcher sends to SWIS
 * @released
 * @internalTechnology
 */
enum TInstallServerMessage
	{
	EInstall,
	EUninstall,
	ECancel,
	ERestore,
	ERestoreFile,
	ERestoreCommit,
	EListUnInstalledPkgs,
	ERemoveUnInstalledPkg
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	,EGetComponentInfo
	#endif	
	};
	
/**
 * Restore messages pass parameters in these message slots
 * @released
 * @internalTechnology
 */	

enum TRestoreMessage 
	{
	EMessageSlotRestoreUid,
	EMessageSlotRestoreController
	};
	
/**
 * Restore file messages pass parameters in these message slots
 * @released
 * @internalTechnology
 */	

enum TRestoreFileMessage 
	{
	EMessageSlotRestoreFs,
	EMessageSlotRestoreFile,
	EMessageSlotRestorePath
	};


/**
 * A bad client is panicked by SWIS with these codes
 * @released
 * @internalTechnology
 */
enum TInstallServerPanic
	{
	EPanicInstallServerBadDescriptor,
	EPanicInstallServerIllegalFunction
	};

/**
 * Session to communicate with SWIS in order to initiate installation or
 * uninstallation.
 *
 * @released
 * @internalTechnology
 */
class RInstallServerSession : public RSessionBase
	{
public:
	/**
	 * Connect to the server, attempt to start it if it is not yet running
	 *
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Connect();
	
	/**
	 * Start installation. SWIS will ask SISHelper for package data so it is 
	 * not necessary to pass any additional arguments.
	 *
	 * @param aStatus	A request status which indicates the completion status of the asynchronous request.
	 */
	void InstallL(const TIpcArgs& aArgs,TRequestStatus& aStatus);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Get component information for a SIS file. This operation does not install the file, only parses information inside it.
	 *
	 * @param aStatus	A request status which indicates the completion status of the asynchronous request.
	 */
	void GetComponentInfoL(const TIpcArgs& aArgs,TRequestStatus& aStatus);
	#endif
	
	/**
	 * Start uninstallation
	 *
	 * @param aUid UID of the package to uninstall
	 * @param aStatus	A request status which indicates the completion status of the asynchronous request.
	 */
	void Uninstall(TIpcArgs& args, TRequestStatus& aStatus);
	
	/**
	 * Cancel current activity of SWIS. This is meaningful during file 
	 * installation phase only because to cancel installation planning it is
	 * only necessary to cancel a dialog box.
	 *
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Cancel();
	
	/**
	 * Start restoration of a package
	 *
	 * @param aArgs		Contains the UID of the package being restored and the associated metadata
	 * @return			KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Restore(const TIpcArgs& aArgs);
	
	/**
	 * Restores a single system file
	 *
	 * @param aArgs		Contains the destination filename of the file to be restored and a file handle to its data
	 * @return			KErrNone if successful, or an error code
	 */
	IMPORT_C TInt RestoreFile(const TIpcArgs& aArgs);
		
	/**
	 * Commits the files submitted in this package
	 *
	 * @return			KErrNone if successful, or an error code
	 */
	IMPORT_C TInt RestoreCommit();

	};


/**
 * Session to communicate with SWIS in order to request and remove
 * Uninstalled packages from a removable drive
 *
 * @released
 * @internalTechnology
 */
class RUninstalledPkgsSession : public RSessionBase
	{
public:
	/**
	 * Connect to the server, attempt to start it if it is not yet running
	 *
	 * @return KErrNone if successful, or an error code
	 */
	TInt Connect();
	
	/**
	 * Close the connection to the server
	 *
	 * @return KErrNone if successful, or an error code
	 */
	void Close();
	
	/**
	 * Request Uninstalled apps list from SWIS.
	 *
	 * @param aStatus	A request status which indicates the completion status of the asynchronous request.
	 */
	TInt RequestUnInstalledPkgsList(const TIpcArgs& aArgs);

	/**
	 * Request Uninstalled apps list from SWIS.
	 *
	 * @param aStatus	A request status which indicates the completion status of the asynchronous request.
	 */
	TInt RemoveUnInstalledPkg(const TIpcArgs& aArgs);

	};
	
} // namespace Swi

#endif __INSTALLCLIENTSERVER_H__
