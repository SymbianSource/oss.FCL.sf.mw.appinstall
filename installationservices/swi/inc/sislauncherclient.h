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
* SisLauncher - client interface
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISLAUNCHERCLIENT_H__
#define __SISLAUNCHERCLIENT_H__

#include <e32base.h>
#include <f32file.h>
#include "sislauncherdefs.h"

const TInt KDefaultShutdownTimeout = 10000000;

namespace Swi
{
class CSisRegistryFileDescription;
class CSoftwareTypeRegInfo;

class RSisLauncherSession : public RSessionBase
	{
public:
	/**
	 * Connects to the SIS launcher server.
	 */
	IMPORT_C TInt Connect();
	
	/**
	 * Launches an executable.
	 *
	 * @param aFileName The file name of the executable.
	 * @param aWait     If true, then this call waits for the executable to terminate.
	 */
	IMPORT_C void RunExecutableL(const TDesC& aFileName, TBool aWait);
	
	/**
	 * Opens a document.
	 * @param aFileName The file name of the document to open.
	 * @param aWait     If true, then this call waits for the document to close.
	 */
	IMPORT_C void StartDocumentL(const TDesC& aFileName, TBool aWait);
	
	/**
	 * Opens a document using the mime-type to determine the helper application.
	 * @param aFileName The file name of the document to open.
	 * @param aMimeType The mime type of the document.
	 * @param aWait     If true, then this call waits for the document to close.
	 */
	IMPORT_C void StartByMimeL(const TDesC& aFileName, const TDesC8& aMimeType, TBool aWait);
	

	/**
	 * Opens a document.
	 * @param aFile The file handle of the document to open.
	 * @param aWait     If true, then this call waits for the document to close.
	 */
	IMPORT_C void StartDocumentL(RFile& aFile, TBool aWait);
	
	/**
	 * Opens a document using the mime-type to determine the helper application.
	 * @param aFile The file handle of the document to open.
	 * @param aMimeType The mime type of the document.
	 * @param aWait     If true, then this call waits for the document to close.
	 */
	IMPORT_C void StartByMimeL(RFile& aFile, const TDesC8& aMimeType, TBool aWait);
	
	IMPORT_C void ShutdownAllL();
	
	/**
 	 * Checks if the application is running. This function will leave with error KErrInUse if an 
 	 * application is busy. Else it will leave with error KErrNone
 	 *
 	 * @param aUid The UID of the application to check.
	 */
	
	IMPORT_C void CheckApplicationInUseL(RArray<TAppInUse>& aUidList);

	/**
 	 * Shuts down an application. If the aUid parameter is null then all non system/hidden
 	 * applications are closed. This function will leave with error KErrInUse if an 
 	 * application is busy.
 	 *
 	 * @param aUid The UID of the application to close.
 	 * @param aTimeout Timeout value in microseconds used when shutting down individual exe.
	 */
	IMPORT_C void ShutdownL(const RArray<TUid>& aUidList, TInt aTimeout);

	/**
	 * Notifies APPARC of new registration files. Causes APPARC to rescan its
	 * import/apps directory, so avoid calling unnecessarily. This call
	 * supports DEF084847 by telling APPARC which apps to treat as registered
	 * even though installation is not quite complete.
	 *
	 * @param aFiles The array of filenames that have appeared during this installation.
	 */
	IMPORT_C void NotifyNewAppsL(const RPointerArray<TDesC>& aFiles);

	/**
	 * Waits for ECOM to notify of changes before attempting to launch the files.
	 * Files are opened client side before sending them to the server to avoid capability issues. 
	 *  
	 * As SWI may shutdown before ECOM will process delivered plug-ins 
	 * the launcher will not terminate the usual 2secs after the last client. Instead a 
	 * longer running timeout is used for two reasons.
	 * a) to allow ECOM a 'reasonable' amount of time to do its work - we don't want to 
	 *    kill the launcher while ECOM is still busy.
	 * b) to shutdown correctly where SWI has incorrectly identified a plug-in resource file -
	 *    we don't want to hang around forever waiting for an event that will never come.  
	 * */
	IMPORT_C void RunAfterEcomNotificationL(const RPointerArray<CSisRegistryFileDescription>& aFileList);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	IMPORT_C void ParseSwTypeRegFileL(RFile& aFile, RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray);

	IMPORT_C void RegisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes);

	IMPORT_C void UnregisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes);

private:
	void RegisterSifLauncherMimeTypesImplL(const RPointerArray<HBufC8>& aMimeTypes, TBool aRegister);
#endif
    };
} //namespace

#endif
