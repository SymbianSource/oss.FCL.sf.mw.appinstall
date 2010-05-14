/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains declarations of installer parameter classes.
*
*/


#ifndef SWINSTDEFS_H
#define SWINSTDEFS_H

//  INCLUDES
#include <e32std.h>
#include <apmstd.h>
#include <AknServerApp.h>

namespace SwiUI
{

// Errors
const TInt KSWInstErrUserCancel = -30471; // User cancelled the operation
const TInt KSWInstErrFileCorrupted = -30472; // File is corrupted
const TInt KSWInstErrInsufficientMemory = -30473; // Insufficient free memory in the drive to perform the operation
const TInt KSWInstErrPackageNotSupported = -30474; // Installation of the package is not supported
const TInt KSWInstErrSecurityFailure = -30475; // Package cannot be installed due to security error
const TInt KSWInstErrMissingDependency = -30476; // Package cannot be installed due to missing dependency
const TInt KSWInstErrFileInUse = -30477; // Mandatory file is in use and prevents the operation
const TInt KSWInstErrGeneralError = -30478; // Unknown error
const TInt KSWInstErrNoRights = -30479; // The package has no rights to perform the operation
const TInt KSWInstErrNetworkFailure = -30480; // Indicates that network failure aborted the operation
const TInt KSWInstErrBusy = -30481; // Installer is busy doing some other operation
const TInt KSWInstErrAccessDenied = -30482;  // Target location of package is not accessible
const TInt KSWInstUpgradeError = -30483;  // The package is an invalid upgrade

// Handeled mime types

_LIT8( KSisxMimeType, "x-epoc/x-sisx-app" );
_LIT8( KSisMimeType, "application/vnd.symbian.install" );
_LIT8( KPipMimeType, "application/x-pip" );
_LIT8( KJadMIMEType, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KJarMIMEType, "application/java-archive" );
_LIT8( KJavaMIMEType, "application/java");
_LIT8( KJarxMIMEType, "application/x-java-archive");

const TInt KSWInstMaxUserNameLength = 32;
const TInt KSWInstMaxPasswordLength = 32;

/**
* Operation codes.
*
* @since 3.0
*/
enum TServerRequest
    {
    ERequestInstall = RApaAppServiceBase::KServiceCmdBase,
    ERequestInstallHandle,
    ERequestInstallParams,
    ERequestInstallParamsHandle,
    ERequestSilentInstall,
    ERequestSilentInstallHandle,
    ERequestSilentInstallParams,
    ERequestSilentInstallParamsHandle,
    ERequestUninstall,
    ERequestSilentUninstall,
    ERequestCancelRequest,
    ERequestCustomUninstall,
    ERequestSilentCustomUninstall
    };

/**
* Policy for install / uninstall.
* EPolicyUserConfirm is not supported in 3.0,
* and will be interpreted as EPolicyNotAllowed.
* @since 3.0
*/
enum TPolicy
    {
    EPolicyAllowed,
    EPolicyNotAllowed,
    EPolicyUserConfirm
    };

/**
* Code for operation performed by installer
*
* @since 3.0
*/
enum TOperation
    {
    EOperationUninstallIndex
    };
 
/**
* Represents installation request.
*
* @since 3.0
*/
class TInstallReq
    {
    public:
        
        /**
        * Constructor.
        */
        inline TInstallReq();  

    public:  // Data

        TBuf8<KMaxDataTypeLength> iMIME;  // Mime type
        TUint iCharsetId;                 // Id of char set
        TFileName iSourceURL;             // Source URL / source dir
        TUint iIAP;                       // Access point
    };

//A typedef'd packaged for passing TInstallReq objects.
typedef TPckgBuf<TInstallReq> TInstallReqPckg;

/**
* Represents installation options.
*
* @since 3.0
*/
class TInstallOptions
    {
    public:   

        /**
        * Constructor.
        */
        inline TInstallOptions();    

    public:  // Data

        /**
        * Is it ok to upgrade.
        */
        TPolicy iUpgrade;   
        
        /**
        * Sometimes there are optional items in deployment packages.
        */
        TPolicy iOptionalItems;
        
        /**
        * Tells whether OCSP should be performed or not.
        */
        TPolicy iOCSP;
        
        /**
        * If OCSP is performed, but warnings found, what should
        * be done then? -> Allow = Go ahead.
        */
        TPolicy iIgnoreOCSPWarnings;
        
        /**
        * Is installation of untrusted (uncertified) sw ok.
        */
         TPolicy iUntrusted;
        
        /**
        * Skip infos.
        */
        TPolicy iPackageInfo;
        
        /**
        * Automatically grant user capabilities.
        */
        TPolicy iCapabilities;
        
        /**
        * Silently kill an application if needed.
        */
        TPolicy iKillApp;
        
        /**
        * Can files be overwritten.
        */
        TPolicy iOverwrite;
        
        /**
        * Is it ok to download.
        */
        TPolicy iDownload;
        
        /**
        * Download username.
        */
        TBuf<KSWInstMaxUserNameLength> iLogin;
        
        /**
        * Download password.
        */
        TBuf<KSWInstMaxPasswordLength> iPassword;        
		
        /**
        * Install target drive. (E.g. C, D, etc)
        */
        TChar iDrive;
        
        /**
        * Which language should be used
        */
        TLanguage iLang;
        
        /**
        * Force phone default language to be used
        */
        TBool iUsePhoneLang;

        /**
        * In case of upgrade, upgrade the data as well.
        */
        TPolicy iUpgradeData;        
    };

//A typedef'd packaged for passing TInstallOptions objects.
typedef TPckgBuf<TInstallOptions> TInstallOptionsPckg;

/**
* Represents uninstallation options.
*
* @since 3.0
*/
class TUninstallOptions
    {
    public:

        /**
        * Constructor.
        */
        inline TUninstallOptions();   

    public: //  Data
        
        TPolicy iKillApp;    
        TPolicy iBreakDependency;
    };

//A typedef'd packaged for passing TUninstallOptions objects.
typedef TPckgBuf<TUninstallOptions> TUninstallOptionsPckg;

// In silent install this means "any drive"
const TUint KUserConfirmDrive = '!';

/**
* Parameters for EOperationUninstallIndex
*
* @since 3.0
*/
struct TOpUninstallIndexParam
    {
        TUid iUid;
        TInt iIndex;
    };
//A typedef'd packaged for passing TOpUninstallIndexParam objects.
typedef TPckgBuf<TOpUninstallIndexParam> TOpUninstallIndexParamPckg;


/**
* Heap pointer for URL source. Sometimes URL do not fit in the TFileName
* so we need to have haep descriptor for the data. This class is passed to
* plugins in the TInstallReq.iSourceURL buffer if needed. 
*
* @since 3.1
*/
class TInstallReqHeapURL
    {
    public:
                
        inline TInstallReqHeapURL();  

    public:  
                
        TDesC*  iSourcePtr;         
    };

// A typedef'd packaged for passing TInstallReqHeapURL class. 
typedef TPckgBuf<TInstallReqHeapURL> TInstallReqURLPckg;

const TInt KSWInstHeapPointerLength = 4;

#include <SWInstDefs.inl>
}

#endif // SWINSTDEFS_H
