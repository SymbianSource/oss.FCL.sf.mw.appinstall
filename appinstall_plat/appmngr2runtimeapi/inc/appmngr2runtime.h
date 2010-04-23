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
* Description:   Base class for AppMngr2 plug-ins that define runtime types
*
*/


#ifndef C_APPMNGR2RUNTIME_H
#define C_APPMNGR2RUNTIME_H

#include <e32base.h>                    // CBase
#include <apmrec.h>                     // CDataTypeArray

class CAppMngr2AppInfo;
class CAppMngr2PackageInfo;
class CAppMngr2RecognizedFile;
class MAppMngr2RuntimeObserver;
class CAknIconArray;
class CEikonEnv;


/**
 *  ECom interface UID for AppMngr2 Runtime plugins
 */
const TUid KAppMngr2PluginInterface = { 0x20016BF4 };


/**
 * Base class for Application Manager Runtime plug-ins.
 * 
 * CAppMngr2Runtime class represents one Runtime plug-in. As it is abstract
 * class plug-in must provide the actual implementation using derived class.
 * 
 * Application Manager lists ECom plug-ins implementing KAppMngr2PluginInterface
 * and creates new CAppMngr2Runtime objects. Creating new CAppMngr2Runtime object
 * loads the Runtime plug-in's DLL and instantiates it.
 * 
 * After all Runtime plug-ins are loaded, Application Manager calls LoadIconsL()
 * to load plug-in specific icons, GetSupportedDataTypesL() to identify installation
 * files that this plug-in support, and GetAdditionalDirsToScanL() to get plug-in
 * specific directories that may contain installation files. Then Application Manager
 * scans directories and recognizes files in them, and proceeds to get installed
 * applications and installation packages. Installed applications are prompted from
 * each plug-in via GetInstalledAppsL() method. Each plug-in creates CAppMngr2AppInfo
 * derived objects to represent currently installed applications. Installation packages
 * are based on recognized files in scanned directories. Application Manager provides
 * list of files that match the supported data types via GetInstallationFilesL()
 * method, and the plug-in creates CAppMngr2PackageInfo objects representing
 * installation packages.
 * 
 * Application Manager monitors changes in scanned directories and known application
 * registeries. When a change is notified, Application Manager gets the latest data
 * using GetInstalledAppsL() and GetInstallationFilesL() methods again. If Application
 * Manager does not listen some specific registry, plug-in can notify change using
 * MAppMngr2RuntimeObserver interface.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2Runtime : public CBase
    {
public:     // constructor and destructor

    /**
     * ECom object instantiation.
     * 
     * Loads the ECom plug-in DLL and instantiates new Runtime plug-in object.
     * 
     * @param aUid Specifices the concrete implementation
     * @param aObserver Observer implementing MAppMngr2RuntimeObserver functions
     * @return CAppMngr2Runtime  New Runtime plug-in object
     */
    IMPORT_C static CAppMngr2Runtime* NewL( TUid aImplementationUid,
            MAppMngr2RuntimeObserver &aObserver );
    
    /**
     * Destructs the ECom object and unloads the plug-in DLL.
     */
    IMPORT_C ~CAppMngr2Runtime();


public:     // new functions

    /**
     * Runtime plug-in UID.
     * 
     * Returns UID that identifies the runtime plug-in
     * 
     * $return TUid  Runtime plug-in UID
     */
    IMPORT_C const TUid RuntimeUid() const;

    /**
     * Utility function to open resource file.
     * 
     * Adds nearest localized resource file to the list maintained by CCoeEnv.
     * Uses DriveInfo::EDefaultRom drive and KDC_RESOURCE_FILES_DIR directory
     * by default, if not defined in aFileName. Nearest extension language code
     * is obtained from BaflUtils::NearestLanguageFile(). Added resource files
     * must be deleted using CCoeEnv::DeleteResourceFile() function.
     * 
     * @param aFileName  Resource file name
     * @return TInt  Offset value for this resource file
     */
    IMPORT_C TInt AddNearestResourceFileL( const TDesC& aFileName );

    /**
     * Utility function to construct full bitmap file name.
     * 
     * Constructs the file name using DriveInfo::EDefaultRom drive,
     * KDC_APP_BITMAP_DIR directory, and given MBM/MIF file name.
     * 
     * The caller of this method is responsible to delete the retuned string.
     * 
     * @param aBitmapFile  MBM or MIF file name
     * @return HBufC*  Full file name for aBitmapFile
     */
    IMPORT_C HBufC* FullBitmapFileNameLC( const TDesC& aBitmapFile );
    
    /**
     * Utility function to return cached CEikonEnv reference.
     * 
     * @return CEikonEnv&  Reference to cached CEikonEnv::Static() instance
     */
    IMPORT_C CEikonEnv& EikonEnv();
    
    /**
     * Runtime observer.
     * 
     * Returns reference to object implementing MAppMngr2RuntimeObserver
     * interface. See appmngr2runtimeobserver.h for more info.
     * 
     * @return MAppMngr2RuntimeObserver&  Reference to observer
     */ 
    IMPORT_C MAppMngr2RuntimeObserver& Observer();

public:     // new pure virtual functions

    /**
     * Load icons for this plug-in.
     * 
     * Plug-in specific icons are used when CAppMngr2InfoBase::IconIndex()
     * or CAppMngr2InfoBase::IndicatorIconIndex() return indexes to the
     * returned aIconArray.
     *
     * There are no default icons, so each plug-in must provide implementation
     * for LoadIconsL() method. 
     * 
     * @param aIconArray  Array where to append the loaded icons 
     */
    virtual void LoadIconsL( CAknIconArray& aIconArray ) = 0;

    /**
     * Supported data types for this plug-in.
     * 
     * Return MIME types that this Runtime plug-in supports for creating
     * installation packages (CAppMngr2PackageInfo objects). Application
     * Manager scans installation files and recognizes file types. Files
     * that match to the supported MIME types are provided to the plug-in
     * via GetInstallationFilesL() method.
     * 
     * Scanning and recognizing is implemented in Application Manager for
     * performance reasons. Plug-ins should not scan or recognize files as
     * Application Manager has done it already once and it provides list 
     * of files and their MIME types to the plug-ins.
     * 
     * @param aDataTypeArray  Array where to append supported data types   
     */
    virtual void GetSupportedDataTypesL( CDataTypeArray& aDataTypeArray ) = 0;
    
    /**
     * Define additional directories for installation file scanning.
     * 
     * By default all PathInfo::EInstallsPath directories in all non-remote
     * drives are scanned. Also KSWInstallerPackageFolder directory defined
     * in CenRep is scanned. Use this function to add more directories for
     * scanning. All directory names are checked with RFs::IsValidName() and
     * invalid names are ignored. Wild-cards (like '*') are not allowed in
     * directory names.
     * 
     * @param aFsSession  File server session 
     * @param aDirs  Array where to append additional directories to scan
     */
    IMPORT_C virtual void GetAdditionalDirsToScanL( RFs& aFsSession,
            RPointerArray<HBufC>& aDirs );
    
    /**
     * Create package info objects.
     * 
     * Package info objects represent installation files in "Installation
     * Files" view. Each package info object is displayed as a separate item.
     * Package info objects should be based on recognized installation files,
     * provided in aFileList array.
     * 
     * GetInstallationFilesL() may be called several times. It is called once
     * for each scanned directory that contains recognized files for this Runtime
     * plugin.
     *
     * This asynchronous request must be completed properly using the method
     * User::RequestComplete() even if the GetInstallationFilesL() itself has
     * been implemented in synchronous manner. Outstanding request may be
     * cancelled by calling CancelGetInstallationFiles().
     * 
     * @param aPackageInfos  Array where to append package info objects
     * @param aFileList  List of file names and corresponding MIME types
     * @param aFsSession  File server session
     * @param aStatus  Request status for the asynchronous request
     */
    virtual void GetInstallationFilesL(
            RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
            const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
            RFs& aFsSession,
            TRequestStatus& aStatus ) = 0;

    /**
     * Cancel pending asynchronous GetInstallationFilesL() request.
     */
    virtual void CancelGetInstallationFiles() = 0;
    
    /**
     * Create application info objects.
     * 
     * Application info objects represent installed applications in "Installed"
     * view. Each application info object is displayed as a separate item.
     *
     * This asynchronous request must be completed properly using the method
     * User::RequestComplete() even if the GetInstalledAppsL() itself has
     * been implemented in synchronous manner. Outstanding request may be
     * cancelled by calling CancelGetInstalledApps().
     * 
     * @param aApps  Array where to add application info objects
     * @param aFsSession  File server session
     * @param aStatus  Request status for the asynchronous request
     */
    virtual void GetInstalledAppsL(
            RPointerArray<CAppMngr2AppInfo>& aApps,
            RFs& aFsSession,
            TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancel pending asynchronous GetInstalledAppsL() request.
     */
    virtual void CancelGetInstalledApps() = 0;

protected:  // new functions
    /**
     * Protected constructor exported for derived classes.
     */
    IMPORT_C CAppMngr2Runtime( MAppMngr2RuntimeObserver &aObserver );

private:    // new functions
    void ConstructL( TUid aImplementationUid );

private:    // data
    /**
     * ECom instance identifier key.
     */
    TUid iDtorIDKey;

    /**
     * Implementation UID that identifies the plugin.
     */
    TUid iRuntimeUid;

    /**
     * Application implementing observer interface.
     */
    MAppMngr2RuntimeObserver& iObserver;

    /**
     * CEikonEnv pointer, cached from CEikonEnv::Static().
     */
    CEikonEnv* iCachedEikonEnv;   // not owned
    };

#endif // C_APPMNGR2RUNTIME_H

