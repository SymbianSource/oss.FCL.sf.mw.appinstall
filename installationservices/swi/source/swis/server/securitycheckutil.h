/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The file contains the declaration of the SecurityCheckUtil class.
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __SECURITYCHECKUTIL_H__
#define __SECURITYCHECKUTIL_H__

#include <f32file.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#else
#include "integrityservices.h"
#endif

#include "sisfieldtypes.h"
#include "siscontroller.h"
#include "sistruststatus.h"
#include "application.h"

namespace Swi
{
class CCertChainConstraints;
class CSecurityManager;
class CSisRegistryFileDescription;

_LIT(KBinPath, "\\sys\\bin\\");
_LIT(KImportDir, "\\import\\");
_LIT(KPrivatePath, "\\private\\");
_LIT(KResourcePath, "\\resource\\");
_LIT(KSysPath, "\\sys\\");
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
_LIT(KSysInstallPath, "\\sys\\install\\integrityservices\\");
#endif
_LIT(KSysInstallTempPath, "\\sys\\install\\temp\\");
_LIT(KHashPath, "\\sys\\hash\\");
_LIT(KHashPathFormat, "%c:%S");

/**
This class is a utility class to check the request DeviceID, SID and VID against the constraints.
To compare capabilities in the header and in the executable.
To check if a TCapabilitySet is empty or not.
To remove user capabilities.
@released
@internalTechnology 
*/
class SecurityCheckUtil
	{
public:

	// Specific errors identified by CheckProtectedDirectoriesL.
	enum TProtectedDirectoryCheckError
		{
		EInvalidFileName,
		ESIDMismatch
		};

	/**
	The function is to compare the capability stored in the header and the executable, leave if they do not
	match.
	
	@param aFileToProcess the CSisRegistryFileDescription of the executable.
	@param aSecurityInfo the security info in the executable header.
	@return KErrCapabilitiesMismatch if the capabilities in the header and exe do not match. 
			KErrNone if the capabilities match.
	*/
	static TInt CompareHeaderAndExeCapabilities(CSisRegistryFileDescription& aFileToProcess, const TSecurityInfo& aSecurityInfo);
	/**
	The function is to check the retrieved Device IDs against the device Id constaints. leave if check fails.
	
	@param aConstraints the constraints in one controller.
	@param aDeviceIds the device id of the Symbian device
	@return None
	*/	
	static void CheckDeviceIdConstraintsL(const CCertChainConstraints* aConstraints, const RPointerArray<HBufC>& aDeviceIds);
	
	/**
	Removes user capabilities from the set, leaving only system 
	capabilities in it.
	
	@param aCapabilities the capability set whose user capabilities need to be removed.
	@param aSecurityManager the security manager.
	@return None
	*/	
	static void RemoveUserCaps(TCapabilitySet& aCapabilities, const CSecurityManager& aSecurityManager);
	
	/**
	The function is to determine if the given capability set is empty.
	 
	@param aCapabilities The capability set to be determined
	@return \b True if the capability set is not empty, \b false if it is
	*/	
	static TBool NotEmpty(const TCapabilitySet& aCapabilities);
	
	/**
	The function is to check the SID in the executable 
	against the constraints in Symbian device.	
	
	@param aConstraints the constraints in one controller.
	@param aSecurityInfo the security info in the executable header.
	@return KErrNone if the SID is valid or KErrSecurityError if the constraint is not met.
	*/	
	static TInt CheckSidConstraints(
		const CCertChainConstraints* aConstraints, 
		const TSecurityInfo& aSecurityInfo);
	/**
	The function is to check the VID in the executable or dll against 
	the constraints in Symbian device.	
	
	@param aConstraints the constraints in one controller.
	@param aSecurityInfo the security info in the executable header.
	@return KErrNone if the VID is valid or KErrSecurityError if the constraint is not met.
	*/	
	static TInt CheckVidConstraints(
		const CCertChainConstraints* aConstraints, 
		const TSecurityInfo& aSecurityInfo);
	
	/**
	This function performs a security check on given file names
	
	@param aFileName the filename to check
	@param aSystemDriveChar the letter of system drive
	@return True if the filename is acceptable, false otherwise
	*/
	static TBool CheckFileName(const TDesC& aFileName, TChar aSystemDriveChar);

	/** Determines whether a package UID is within the protected range
	@param aUid The package Uid to check
	@return ETrue if it is protected
	*/
	static TBool IsPackageUidProtected(const TUint32 aUid);

	/** Determines whether an EXE SID is within the protected range
	@param aSid The SID to check
	@return ETrue if it is protected
	*/
	static TBool IsSidProtected(const TUint32 aSid);

	/** Determines whether an EXE VID is within the protected range
	@param aUid The VID to check
	@return ETrue if it is protected
	*/
	static TBool IsVidProtected(const TUint32 aVid);

	/**
	Checks the supplied controller is trusted to an equal or higher level
	than the validation status required of it. The exception to this is if
	the base package is in ROM, in which case the eclipsing rules apply.
	@param aController The controller to check
	@param aRequiredStatus The required validation level
	@return True if the required level of trust is met, false otherwise
	*/
		
	static TBool IsSufficientlyTrusted(const Sis::CController& aController, TValidationStatus aRequiredStatus);
	
	static TBool IsSufficientlyTrusted(const Sis::CController& aController, const Sis::CController& aBaseController);

	static TInt LookForBlockingFilesL(const RArray<TFileName>& aFileNameArray);	

	/** Checks the list of files for those that are owned by a certain package and deletes them from the list
	@param aFileNameArray array of TFileName objects containing the fully qualified names of files to check
	@param aFilename the filename that the check is being made for
	@param aInstallPkgUid UID of package whose files will be removed from the list (as it is the same as the one trying to install aFilename)
	*/
	static void FilterNonBlockingFilesOfFilenameL(RArray<TFileName>& aFileNameArray, const TDesC& aFilename, const TUid& aInstallPkgUid);

	/** Checks the list of files for those that are not orphaned and deletes them from the list
	@param aFileNameArray Pointer array containing the fully qualified names of files to check
	*/
	static void FilterOrphanedFilesL(RPointerArray<HBufC>& aFileNameArray);

	/**
	Checks the target of the install/restore against the list of protected directories.

	@param aTarget			The target of the file to install/restore.
	@param aOperation		The operation to perform on the file (FILENULL operations MUST be checked)
	@param aPackageSids		The set of SIDs for executables belonging to the package.
	@param aCheckError		The specific error if the path was not valid.
	@return true if the target is valid.
	*/
	static TBool CheckProtectedDirectoriesL(
		const TFileName& aTarget,
		Sis::TSISFileOperation aOperation,
		const RArray<TUid>& aPackageSids,
		TProtectedDirectoryCheckError& aCheckError);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	The function process the UID verification from given RegistryFileDescription
	 
	@param aApplication			CApplication object
	@param aFs 					A RFs reference
	@param aSidsAdded			array of uid
	@param aStsSession			transactionservice used to construct sidcache.
	@param aFileToProcess		Description of the file which needs to be process	
	@param aCurrentFileName		The file name to check for
	@return	KErrNone if completes successfully otherwise return the TInt of TErrorDialog
	*/
	static TInt ProcessFileL(
		const CApplication& aApplication,
		RFs& aFs,
		RArray<TUid>& aSidsAdded,
		Usif::RStsSession& aStsSession,
		CSisRegistryFileDescription& aFileToProcess,
		const TDesC& aCurrentFileName);
#else
	/**
	The function process the UID verification from given RegistryFileDescription
	 
	@param aApplication			CApplication object
	@param aFs 					A RFs reference
	@param aSidsAdded			array of uid
	@param aIntegrityServices	integrityservices used to construct sidcache.
	@param aFileToProcess		Description of the file which needs to be process	
	@param aCurrentFileName		The file name to check for
	@return	KErrNone if completes successfully otherwise return the TInt of TErrorDialog
	*/
	static TInt ProcessFileL(
		const CApplication& aApplication,
		RFs& aFs,
		RArray<TUid>& aSidsAdded,
		CIntegrityServices& aIntegrityServices,
		CSisRegistryFileDescription& aFileToProcess,
		const TDesC& aCurrentFileName);
#endif

	/**
	Checks if the file is to be uninstalled as part of the specified application.
	@param aFileName The name of the file to check against the removal list.
	@param The application with the list of files to remove.
	@return ETrue if the file is to be removed, EFalse otherwise.
	*/
	static TBool IsToBeUninstalledL(const TDesC& aFileName, const CApplication& aApplication);

	/**
	Checks whether the file is in a private directory.
	@return ETrue if the file is in a private directory; otherwise, EFalse is returned. 
	*/
	static TBool IsPrivateDirectory(const TDesC& aFileName);	
	
	/**
	Checks whether a file is in a protected directory. This checks
    import directories, /resource and /sys
    @param aFileName - The file name to check
    @return ETrue if the file is in a private import directory
	*/
	static TBool IsProtectedDirectoryL(const TDesC& aFileName);
	
	/**
	Checks whether the drive of a file path has been subst'd
	@param aFileName The path of the file 
	@return ETrue if the file path's drive has been subst'd, EFalse otherwise
	*/
	static TBool IsSubstedDriveL(RFs& aFs, const TDesC& aFileName);
	
	/**
	Checks if target location's write access is TCB protected.
	@param aFileName The filename to be checked.
	@return ETrue if write access to the directory requires the TCB capability.
	*/
	static TBool IsTargetTcbWriteProtected(const TDesC& aFileName);

#ifdef __WINS__
	/**
	Clean up the memory which is allocated from windows directly

	@param aBuffer			The buffer to be released
	*/
	static void CleanupWinMemory(TAny* aBuffer);
#endif
	};


inline TBool SecurityCheckUtil::IsPackageUidProtected(const TUint32 aPackageUid)
	{
	// If bit 31 is set then Uid is unprotected
	return (aPackageUid & 0x80000000) ? EFalse : ETrue;
	}
	
inline TBool SecurityCheckUtil::IsSidProtected(const TUint32 aSid)
	{
	// If bit 31 is set then Sid is unprotected
	return (aSid & 0x80000000) ? EFalse : ETrue;
	}
	
inline TBool SecurityCheckUtil::IsVidProtected(const TUint32 aVid)
	{
	// If Uid is zero then it is unprotected
	return (aVid) ? ETrue : EFalse;
	}

inline TBool SecurityCheckUtil::IsSufficientlyTrusted(const Sis::CController& aController, TValidationStatus aRequiredStatus)
	{
	return (aController.TrustStatus().ValidationStatus() >= aRequiredStatus) ||
		(aRequiredStatus == EPackageInRom);
	}
	
inline TBool SecurityCheckUtil::IsSufficientlyTrusted(const Sis::CController& aController, const Sis::CController& aBaseController)
	{
	return (aController.TrustStatus().ValidationStatus() >= aBaseController.TrustStatus().ValidationStatus());
	}

} // namespace Swi

#endif




