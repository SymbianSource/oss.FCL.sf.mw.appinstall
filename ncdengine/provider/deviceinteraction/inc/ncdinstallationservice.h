/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of MNcdInstallationService
*
*/


#ifndef M_NCD_INSTALLATION_SERVICE_H
#define M_NCD_INSTALLATION_SERVICE_H

#include <usif/sif/sifcommon.h>

#include "ncditempurpose.h"

class MNcdInstallationServiceObserver;
class TCatalogsVersion;
class TDataType;
class RFile;
class RFs;

/**
 * Theme SIS UID
 * This is given to observer as an application UID if a theme
 * was installed
 */
const TInt KNcdThemeSisUid = 0x101f8582;


/**
 * Application installation statuses
 *
 * @note Do not change these 
 */ 
enum TNcdApplicationStatus
    {
    ENcdApplicationOlderVersionInstalled = -1,
    ENcdApplicationNotInstalled = 0,
    ENcdApplicationInstalled,
    ENcdApplicationNewerVersionInstalled
    };
    

/**
 *
 */
class MNcdInstallationService
    {
public: // Destruction

    /**
     * Destructor
     */
    virtual ~MNcdInstallationService() {}
    
public: // New functions

    /**
     * Installs all kinds of files. Will leave with KErrInUse if installation
     * is already in progress.
     * @param aFileName Name of the file to be installed.
     * @param aMimeType Mime type for the file to be installed.
     * @param aPurpose
     */    
    virtual void InstallL( const TDesC& aFileName,
                           const TDesC& aMimeType,
                           const TNcdItemPurpose& aPurpose  ) = 0;
    

    /**
     * Installs all kinds of files. Will leave with KErrInUse if installation
     * is already in progress.
     * @param aFile Handle to source file
     * @param aMimeType Mime type for the file to be installed.
     * @param aPurpose
     */    
    virtual void InstallL( RFile& aFile,
                           const TDesC& aMimeType,
                           const TNcdItemPurpose& aPurpose  ) = 0;
    

    /**
     * Installs java software. Will leave with KErrInUse if installation
     * is already in progress.
     * @param aFileName Name of the JAR file to be installed.
     * @param aMimeType Mime type for the file to be installed.
     * @param aDescriptorData Descriptor (JAD) data, or KNullDesC if not available.
     */    
    virtual void InstallJavaL( const TDesC& aFileName,
                               const TDesC& aMimeType,
                               const TDesC8& aDescriptorData ) = 0;


    /**
     * Installs java software. Will leave with KErrInUse if installation
     * is already in progress.
     * @param aFile Handle to source file
     * @param aMimeType Mime type for the file to be installed.
     * @param aDescriptorData Descriptor (JAD) data, or KNullDesC if not available.
     */    
    virtual void InstallJavaL( RFile& aFile,
                               const TDesC& aMimeType,
                               const TDesC8& aDescriptorData ) = 0;

    /**
     * Installs all kinds of files silently. 
     * @see InstallL
     */    
    virtual void SilentInstallL( RFile& aFile,
                                 const TDesC& aMimeType,
                                 const TNcdItemPurpose& aPurpose ,
                                 const Usif::COpaqueNamedParams* aInstallOptionsPckg ) = 0;


    /**
     * Installs java software silently. 
     * @see InstallJavaL
     */    
    virtual void SilentInstallJavaL( RFile& aFile,
                                     const TDesC& aMimeType,
                                     const TDesC8& aDescriptorData,
                                     const Usif::COpaqueNamedParams* aInstallOptionsPckg ) = 0;
    
    /*
     * Use SWI API to install widget silently. 
     */     
    virtual void SilentInstallWidgetL(RFile& aFile,
                                      const Usif::COpaqueNamedParams* aInstallOptionsPckg ) = 0;

    /**
     * Cancels the silent installation if it is going on.
     *
     * @note If the installation has almost finished, the cancel request
     * may not have any effect and the installation will finish.
     *
     * @param aFileName Will contain path and file name, if picture,
     * ringingtone or similar was installed and a theme name if a theme was installed.
     * Will contain NULL for sis and java applications. Old value is deleted.
     * @param aAppUid will contain UID if an application was installed, either sis
     * or java. Contains KNcdThemeSisUid if a theme was installed and
     * KNullUid for non-applications.
     * @param aError KErrNone if installation was finished even though
     * cancel was requested. KErrCancel if cancellation was success.
     * KNcdThemeReinstalled if a theme was reinstalled. 
     * KNcdThemePossiblyReinstalled if a theme was likely reinstalled.
     * Otherwise some Symbian error code.
     */
    virtual void CancelSilentInstall( HBufC*& aFileName,
                                      TUid& aAppUid,
                                      TInt& aError ) = 0;
                   
                   
    /**
     * Sets observer for installation services.
     * @param aObserver Observer for installation services.
     */
    virtual void SetObserver(
        MNcdInstallationServiceObserver& aObserver ) = 0;

    /**
     * Gets version of installed application by using its UID.
     * @param aUid UID of the application.
     * @param aVersion On return contains version of the application.
     * @return TInt Error code, KErrNotFound if application not found.
     */
    virtual TInt ApplicationVersion( const TUid& aUid,
                                     TCatalogsVersion& aVersion ) = 0;

    /**
     * Checks whether an application is installed on the device.
     * @param aUid UID of the application.
     * @return TBool ETrue if installed.
     */
    virtual TBool IsApplicationInstalledL( const TUid& aUid ) = 0;

    virtual TNcdApplicationStatus IsApplicationInstalledL( 
        const TUid& aUid, const TCatalogsVersion& aVersion ) = 0;
    
    /**
     * Use widget registry API to check if certain widget with given identifier
     * has been installed already.
     */
    virtual TNcdApplicationStatus IsWidgetInstalledL(
        const TDesC& aIdentifier, const TCatalogsVersion& aVersion) = 0;
    
    /**
     * Calling widget registry API to return the Uid of the widget 
     * with given identifier.
     */     
    virtual TUid WidgetUidL( const TDesC& aIdentifier) = 0;
    
    /**
     * Gets the first SID from the SIS registry package
     * by using application UID.
     * @param aUid UID of the application.
     * @return TUid KNullUid if package not found.
     */
    virtual TUid SidFromSisRegistryL( const TUid& aUid ) = 0;

    /**
     * Appends rights into the rights database. May leave with DRM
     * specific error code.
     * @param aRightsObject Buffer containing the rights object.
     * @param aMimeType Type of the rights object.
     */
    virtual void AppendRightsL(
        const TDesC8& aRightsObject,
        const TDataType& aMimeType ) = 0;

    
    /**
     * Checks whether a theme is installed or not
     *
     * @param aThemeName Theme name
     * @return ETrue if the theme is installed
     */
    virtual TBool IsThemeInstalledL( const TDesC& aThemeName ) = 0;    
    
    
    /**
     * Deletes the given file
     *
     * @param aFilename Path and name of the file to delete
     * @return Symbian error code
     */
    virtual TInt DeleteFile( const TDesC& aFilename ) = 0;
    
    
    /**
     * File server session getter
     *
     * @return File server session
     */
    virtual RFs& FileServerSession() = 0; 

    
    /**
     * Writes JAD to disk. 
     * 
     * The file is written to the same directory where the related JAR is.
     * 
     * @param aJarFileName Name of the related JAR. Used for generating the filename for JAD
     * @param aJad JAD data
     * @return JAD filepath
     * @leave Symbian error code
     */
    virtual HBufC* WriteJadL( const TDesC& aJarFileName, const TDesC8& aJad  ) = 0;
    
    /**
     * Recognizes the data from the file itself
     *
     * @param aFileName Path and name of the file 
     * @return Mime type of the file
     */
    virtual HBufC* RecognizeDataL( const TDesC& aFileName ) = 0;

    };

#endif // M_NCD_INSTALLATION_SERVICE_H
