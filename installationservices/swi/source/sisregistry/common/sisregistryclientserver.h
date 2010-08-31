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
* SisRegistry - shared client/server definitions
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISREGISTRYCLIENTSERVER_H__
#define __SISREGISTRYCLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

namespace Swi
{
	
_LIT(KSisRegistryName,"!SisRegistryServer");    // name to connect to
_LIT(KSisRegistryImg, "SisRegistryServer");		// DLL/EXE name
_LIT(KSisRegistryShortName, "SisRegistry");	    // name used for identication when panicking 
                                                // clients  - has to be less than 16 bytes

const TUid KServerUid3				={0x10202DCA};

const TInt KMaxSisRegistryMessage	=100;
const TInt KDefaultBufferSize		=2048;

// ENum for stub file extraction operation switch mode.
enum TStubExtractionMode
	{
	EGetCount,
	EGetFiles
	};

enum TSisRegistryMessages
	{
	// RegistryEntry ReadOnly - All Clients
	EOpenRegistryUid,
	EOpenRegistryPackage,
	EOpenRegistryNames,
	ECloseRegistryEntry,
	ECertificateChains,
	EVersion,
	ELanguage,
	EUniqueVendorName,
	ELocalizedVendorName,
	EPackageName,
	EInRom,
	ESigned,
	EProperty,
	ESelectedDrive,
	EInstalledDrives,			 
	ERemoveWithLastDependent,
	ESetRemoveWithLastDependent,
	EGetMatchingSupportedLanguages,		
	// daemon interface
	EAddDrive,
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	ERemoveDrive,
#else
	ESetComponentPresence,
#endif

	// all clients
	EControllers, 

	EGetTrust,
	ETrustTimeStamp,
	ETrustStatus,
	
	EIsSidPresent,
	ESidToFileName,
	
	EShutdownAllApps,

	EVerifyController,
	EInitRecovery, // daemon interface
	EPackageExistsInRom,
	EStubFileEntries,		
	ESeparatorMinimumReadUserData	=32,
	// RegistrySession ReadOnly ReadUserData Clients
	EInstalledUids,	 
	EInstalledPackages,
	// New ReadUserData calls should be added in the ESeparatorMinimumReadUserData2 range
	// All clients. 
	ESidToPackage,
	EUidInstalled,
	EControllerInstalled,
	EModifiable,			 
	EHash,
	EHashEntry,
	EUid,			 
	EGetEntry,	
	EUidPresent,          
	EAugmentation,			 
	ESids,					 
	EFiles,					 
	EFileDescriptions, 
	EPackageAugmentations,	 
	EPackageExecutables,	 
	EPreInstalled,     		 
	ESize,
	EPackage,
	EPackageAugmentationsNumber,
	ENonRemovable,	
	EloggingFile,	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	EIsFileRegistered, 
	EComponentIdForUid,	
#endif
	// NOTE : The security policy range ESeparatorMinimumWriteUserData-
	// ESeparatorMinimumTCB also requires NetworkServices as well as
	// WriteUserData
	ESeparatorMinimumWriteUserData	=58,
	// RegistrySession Writable WriteUserData Clients
	ERevocationStatus,
	ECancelRevocationStatus,
	ESeparatorMinimumTCB			=64,
	// RegistrySession Writable TCB Clients
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	ECreateTransaction,
	ECommitTransaction,
	ERollbackTransaction,
#endif
	EAddEntry,
	EUpdateEntry,
	EDeleteEntry,
	EInstallType,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	EAddSoftwareTypeEntry,
	EUpdateSoftwareTypeEntry,
    EAddAppRegEntry,
    EUpdateAppRegEntry,
#endif

	// Additional RegistryWritableSession clients 

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	ERegenerateCache,
#endif
	// All clients 
	ERegistryFiles = 128,
	
	// Additional ReadUserData calls
	ESeparatorMinimumReadUserData2 = 256,
	ERemovablePackages,	
	// RegistryEntry  ReadUserData
	EDeletablePreInstalled,
	EDependentPackages,		 
	EEmbeddedPackages,
	EEmbeddingPackages,
	EDependencies,
	ESignedBySuCert,
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	EComponentIdForPackage,
	EAppUidsForComponent,  
	EComponentIdsForPackageUid,
	ESetComponentState = 300, //SIF only
	EAddAppRegInfo = 320,    //requires TCB
	ERemoveAppRegInfo,       //requires TCB 
#endif
	ESeparatorEndAll
	
    };
	
} //namespace
#endif
