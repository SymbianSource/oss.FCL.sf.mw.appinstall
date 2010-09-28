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
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK  
#include <usif/scr/appregentries.h>
#include <usif/scr/screntries_platform.h>
#endif
const TInt KDefaultShutdownTimeout = 10000000;

namespace Swi
{
class CSisRegistryFileDescription;
class CSoftwareTypeRegInfo;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
class TAppUpdateInfo;
#endif

class RSisLauncherSession : public RSessionBase
    {
public:
    /**
     * Connects to the SIS launcher server.
     */
    IMPORT_C TInt Connect();
    
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    /**
     * Closes to the SIS launcher session.
     */
    IMPORT_C void Close();
#endif
    
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
    IMPORT_C void ParseSwTypeRegFileL(RFile& aFile, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray);

    IMPORT_C void RegisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes);

    IMPORT_C void UnregisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes);
            
    /**
    * Populate CApplicationRegistrationData from the specified resource file, for the given languages and pass the data size back to the caller in TRequestStatus
    *
    * @param An already opened file handle.
    * @param aAppLanguages List of the languages
    * @return Return size of buffer needed in TRequestStatus
    */
    IMPORT_C void AsyncParseResourceFileSizeL(const RFile& aRegistrationFile, TRequestStatus& aStatus, TBool aIsForGetCompInfo = EFalse);
    
	/**
    * Return populate CApplicationRegistrationData 
    *
    * @param aDataSize Size of buffer needed for CApplicationRegistrationData.
    * @return Pointer to a CApplicationRegistrationData (caller owns it)
    */
    IMPORT_C Usif::CApplicationRegistrationData*  AsyncParseResourceFileDataL(TInt aDataSize);
    
	/**
    * @param aRegistrationFileName registration file name to be parsed
    * @return Pointer to a CApplicationRegistrationData (caller owns it)
    */    
    IMPORT_C Usif::CApplicationRegistrationData* SyncParseResourceFileL(const RFile& aRegistrationFile, TBool aIsForGetCompInfo = EFalse); 
       
	/**
     * Notifies APPARC of new applications. This call  
     * informs APPARC which apps to treat as registered
     * even though installation is not quite complete.
     *
     * @param aFiles The array of application information to be treated as registered before this current installation completes.
     */
    IMPORT_C void NotifyNewAppsL(const RPointerArray<Usif::CApplicationRegistrationData>& aAppRegData);
    
    /**
     * Notifies APPARC of applications which are removed/upgraded during installation       
     * @param aAppUpdateInfo , array of app uids along with the action(remove/upgrade)
     */
    IMPORT_C void NotifyNewAppsL(const RArray<TAppUpdateInfo>& aAppUpdateInfo);
    
private:
    void RegisterSifLauncherMimeTypesImplL(const RPointerArray<HBufC8>& aMimeTypes, TBool aRegister);   
#endif
    };

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 	
enum InstallActions
       {
       EAppInstalled = 0,
       EAppUninstalled
       };

class TAppUpdateInfo
    {
public:    
    IMPORT_C void InternalizeL(RReadStream& aReadStream);
    IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const; 
    IMPORT_C TAppUpdateInfo(TUid aAppUid, InstallActions aAction);
    IMPORT_C TAppUpdateInfo();    
public:
    TUid iAppUid;
    InstallActions iAction;
    };
#endif

} //namespace

#endif

