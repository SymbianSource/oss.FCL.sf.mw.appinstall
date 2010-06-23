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
* Implementation of SecurityCheckUtil that provides utility functions for Install and restore machine.
*
*/


/**
 @file
 @released
 @internalTechnology 
*/
#include <hal.h> 
#include "securitycheckutil.h"
#include "sisinstallerrors.h"
#include "certchainconstraints.h"
#include "securitymanager.h"
#include "securitypolicy.h"
#include "log.h"
#include "sisregistryfiledescription.h"
#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "cleanuputils.h"
#ifndef __WINS__
#include <f32image.h>
#include <e32rom.h>
#else
#include <windows.h>
#endif
#include "secutils.h"
#include "sistruststatus.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sidcache.h"
#include "securityinfo.h"


using namespace Swi;
using namespace Swi::Sis;

void SecurityCheckUtil::CheckDeviceIdConstraintsL(const CCertChainConstraints* aConstraints, const RPointerArray<HBufC>& aDeviceIds)
	{			
	if (aConstraints->DeviceIDsAreConstrained())
		{		
		TInt devCount=aDeviceIds.Count();
		TBool found=EFalse;
		for (TInt i=0;i<devCount;i++)
			{
			if (aConstraints->DeviceIDIsValid(aDeviceIds[i]))
				{
				found=ETrue;
				break;
				}
			}
		//None of the retrieved Device IDs is in the list, so reject the installation
		if (!found)
			{
			DEBUG_PRINTF(_L8("Install Server - Device ID constraints exceeded"));
			User::Leave(KErrSecurityError);		
			}			
		}
	}

void SecurityCheckUtil::RemoveUserCaps(TCapabilitySet& aCapabilities, const CSecurityManager& aSecurityManager)
	{
	aCapabilities.Remove(aSecurityManager.SecurityPolicy().UserCapabilities());
	}

TBool SecurityCheckUtil::NotEmpty(const TCapabilitySet& aCapabilities)
	{
	TUint32 notEmpty=0;
	for(TInt n=ECapability_Limit-1; n>=0; n--)
		notEmpty |= aCapabilities.HasCapability(static_cast<TCapability>(n));
	return notEmpty;
	}
	
	
TInt SecurityCheckUtil::CompareHeaderAndExeCapabilities(
	CSisRegistryFileDescription& aFileToProcess, 
	const TSecurityInfo& aSecurityInfo)
	{
	TCapabilitySet fileHeaderCaps;
	fileHeaderCaps.SetEmpty();		
	const HBufC8* rawFileHeaderCaps = aFileToProcess.CapabilitiesData();
	if	(rawFileHeaderCaps)
		{
		const TInt KCapSetSize=sizeof(TUint32); // size of a capability bit set
		const TInt KCapSetSizeBits=8*KCapSetSize;
		const TInt KNumCapSets=rawFileHeaderCaps->Size()/KCapSetSize;
		for (TInt set=0; set<KNumCapSets; set++)
			{
			TUint32 capsValue=*(reinterpret_cast<const TUint32*>(rawFileHeaderCaps->Ptr())+set);
			for (TInt capIndex=0; capIndex<KCapSetSizeBits; capIndex++)
				{
				if (capsValue & (0x1<<capIndex))
					{
					TCapability cap=static_cast<TCapability>(capIndex+set*KCapSetSizeBits);
					fileHeaderCaps.AddCapability(cap);
					}
				}
			}			
		}
	if (!aSecurityInfo.iCaps.HasCapabilities(fileHeaderCaps) || 
		!fileHeaderCaps.HasCapabilities(aSecurityInfo.iCaps))
		{
		DEBUG_PRINTF2(_L("Install Server - File '%S', error. Executable capabilities do not match header capabilities"),
			&aFileToProcess.Target());
		return KErrCapabilitiesMismatch;
		}
	return KErrNone;
	}

TInt SecurityCheckUtil::CheckVidConstraints(
	const CCertChainConstraints* aConstraints, 
	const TSecurityInfo& aSecurityInfo)
	{
	//Check the VID in Executables with the constraints
	if (aConstraints->VIDsAreConstrained() && !aConstraints->VIDIsValid(aSecurityInfo.iVendorId))
		{
		DEBUG_PRINTF(_L8("Install Server - VID constraints exceeded"));
		return KErrSecurityError;
		}	
	return KErrNone;
	}
	
TInt SecurityCheckUtil::CheckSidConstraints(
	const CCertChainConstraints* aConstraints, 
	const TSecurityInfo& aSecurityInfo)
	{
	//Check the SID in Executables with the constraints
	if (aConstraints->SIDsAreConstrained() && !aConstraints->SIDIsValid(aSecurityInfo.iSecureId))
		{
		DEBUG_PRINTF2(_L8("Install Server - SID constraints exceeded (SID: 0x%08x)"),
			aSecurityInfo.iSecureId.iId);
		return KErrSecurityError;
		}
	return KErrNone;
	}


TBool SecurityCheckUtil::CheckFileName(const TDesC& aFileName, TChar aSystemDriveChar)
	{

	if (aFileName.Length() > KMaxFileName)
		{
		return EFalse;
		}
	
	// Check for full drive and path
	TChar drive(aFileName[0]);
	
	_LIT(KDriveSeparator, ":\\");
	if (!drive.IsAlpha() || aFileName.Find(KDriveSeparator) != 1)
		{
		return EFalse;
		}
		
	// Check for double slashes
	_LIT(KDoubleSlash, "\\\\");
	if (aFileName.Find(KDoubleSlash) != KErrNotFound)
		{
		return EFalse;
		}
	
	// Check for .. in the path
	_LIT(KDoubleDotPath, "..\\");
	if (aFileName.Find(KDoubleDotPath) != KErrNotFound)
		{
		return EFalse;
		}
		
	// Check for references to writable SwiCertstore
	_LIT(KWritableSwiCertStore, ":\\Resource\\SwiCertstore");
	TBuf<32> writableSwiCertStorePath;
	writableSwiCertStorePath.Append(aSystemDriveChar);
	writableSwiCertStorePath.Append(KWritableSwiCertStore);
	
	if (aFileName.FindF(writableSwiCertStorePath) != KErrNotFound)
		{
		return EFalse;
		}
	
		// Check if install-file contain non-ascii characters
	_LIT(KPathName, ":\\sys\\bin");
	if (aFileName.FindF(KPathName) != KErrNotFound)
		{
		int temp = 0;
		for (int x = 0; x< aFileName.Length() ;x++)
			{
			if(aFileName[x] == '\\')
			temp = x;
			}
			
		for(int y = temp; y < aFileName.Length(); y++)
			{
			TInt ch = aFileName[y];
			if ( ch >= 0 && ch <= 127 )
			continue;
			else
			return EFalse;
			}	
		}

	return ETrue;
	}

TBool SecurityCheckUtil::CheckProtectedDirectoriesL(
	const TFileName& aTarget,
	Sis::TSISFileOperation aOperation,
	const RArray<TUid>& aPackageSids,
	TProtectedDirectoryCheckError& aCheckError)
	{
	TBool validPath = ETrue;
	TPtrC privatePath(KPrivatePath);
	TPtrC resourcePath(KResourcePath);
	TPtrC sysPath(KSysPath);
	TPtrC binPath(KBinPath);

	// if installing to a private directory, check that it is owned by
	// this package, or this is an import directory of another package
	TFileName directory = TParsePtrC(aTarget).Path();

	if (directory.Left(privatePath.Length()).CompareF(privatePath) == 0)
		{
		// check if the private directory corresponds to one of these
		// applications
		TLex lex(directory.Mid(privatePath.Length()));
		TUint sid;
		// Check whether the package UID specified larger than the valid for a TInt.
		TInt uidErr = lex.Val(sid, EHex);
		if ( uidErr == KErrNone )
			{
			TInt errPrivate = aPackageSids.Find(TUid::Uid(sid));
			if (errPrivate==KErrNotFound)
				{
				// the SID is not ours, check if it was another app's import
				// directory
				const TInt KUidStringLength = 8;
				DEBUG_PRINTF4(_L("Install Server - SecurityCheckUtil::CheckProtectedDirectoriesL - check for private path failed. sid checked is %08x, file target is %S, number of allowed SIDs is %d"), sid, &aTarget, aPackageSids.Count());
				if (directory.Length() < privatePath.Length() + KUidStringLength + KImportDir().Length())
					{
					validPath = EFalse;
					aCheckError = ESIDMismatch;
					}
				else
					{
					TPtrC imp = directory.Mid(privatePath.Length()+KUidStringLength, KImportDir().Length());
					if (imp.CompareF(KImportDir) != 0)
						{
						validPath = EFalse;
						aCheckError = ESIDMismatch;
						}
					}
				}
			}
		else
			{
			validPath = EFalse;
			aCheckError = EInvalidFileName;
			}
		}
	else if (directory.Left(sysPath.Length()).CompareF(sysPath) == 0)
		{
		// The only directory under sys that can be written to is sys\bin,
		// and no subdirectories of sys\bin may be created.
		// FN may never be used to remove a file from anywhere under \sys
		if ((aOperation == Sis::EOpNull) ||
			(directory.CompareF(binPath) != 0))
			{
			validPath = EFalse;
			aCheckError = EInvalidFileName;
			}
		}
	else if ((directory.Left(resourcePath.Length()).CompareF(resourcePath) == 0) &&
		(aOperation == Sis::EOpNull))
		{
		// FN may not be used to remove files from \resource
		validPath = EFalse;
		aCheckError = EInvalidFileName;
		}

	return validPath;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
TInt SecurityCheckUtil::LookForBlockingFilesL(const RArray<TFileName>& aFileNameArray)
   	{ 	
    // Start a sisregistry session
 	RSisRegistrySession registrySession;
 	User::LeaveIfError(registrySession.Connect());
 	CleanupClosePushL(registrySession);
 
 	//Retrieve the number of the files
   	TInt fileNameCount=aFileNameArray.Count();  
    		
	TInt result = KErrNotFound;
 	//Go through each name
   	for (TInt k=0;k<fileNameCount;k++)
    	{
 	 	if (registrySession.IsFileRegisteredL(aFileNameArray[k]))
 		  	{
			result = k;
			break;
   			}
   		}
    		
   	CleanupStack::PopAndDestroy(&registrySession);
   	
   	return result; 
   	}

// Checks the list of files for those that are not orphaned and deletes them from the list
void SecurityCheckUtil::FilterOrphanedFilesL(RPointerArray<HBufC>& aFileNameArray)
   	{
    // Start a sisregistry session
 	RSisRegistrySession registrySession;
 	User::LeaveIfError(registrySession.Connect());
 	CleanupClosePushL(registrySession);

	TInt k = 0;
  	while (k < aFileNameArray.Count())
    	{
 	 	if (registrySession.IsFileRegisteredL(*aFileNameArray[k]))
 		  	{
 		   	// If found then delete the file from the list
 		   	delete aFileNameArray[k];
 		   	aFileNameArray.Remove(k);
 		   	}
		else
			{
			++k;
			}
   		}
   	CleanupStack::PopAndDestroy(&registrySession);
   	}
#else //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
TInt SecurityCheckUtil::LookForBlockingFilesL(const RArray<TFileName>& aFileNameArray)
   	{ 	
    	// Start a sisregistry session
 	RSisRegistrySession registrySession;
 	User::LeaveIfError(registrySession.Connect());
 	CleanupClosePushL(registrySession);
 
   	// check all registry entries for the file
   	RPointerArray<CSisRegistryPackage> packages;		
   	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
   	registrySession.InstalledPackagesL(packages);
 	
 	//Retrieve the number of the files
   	TInt fileNameCount=aFileNameArray.Count();  
    		
   	// Go through each package.
   	TInt packageCount=packages.Count();
   	
   	for (TInt i=0; i<packageCount; ++i)
   		{
   		RSisRegistryEntry entry;
   		entry.OpenL(registrySession, *packages[i]);
   		CleanupClosePushL(entry);
    		
   		RPointerArray<HBufC> files;		
   		CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
   		entry.FilesL(files);
    		
   		// Go through each file in the current package
   		for (TInt j=0; j< files.Count(); ++j)
   			{
   			//Go through each name
   			for (TInt k=0;k<fileNameCount;k++)
    			{
 	   			if (files[j]->CompareF(aFileNameArray[k])==0)
 		   			{
 		   			// If found then, return the index in the array
 		   			CleanupStack::PopAndDestroy(2, &entry);
 				   	CleanupStack::PopAndDestroy(&packages);
 				   	CleanupStack::PopAndDestroy(&registrySession);		   			
 		   			return k;
 		   			}
 	   			}   			
   			}
   		CleanupStack::PopAndDestroy(2, &entry);		
   		}
    		
   	CleanupStack::PopAndDestroy(&packages);
   	CleanupStack::PopAndDestroy(&registrySession);
   	
   	// No block file, so they all orphaned files
   	return KErrNotFound; 
   	}

// Checks the list of files for those that are not orphaned and deletes them from the list
void SecurityCheckUtil::FilterOrphanedFilesL(RPointerArray<HBufC>& aFileNameArray)
   	{
    // Start a sisregistry session
 	RSisRegistrySession registrySession;
 	User::LeaveIfError(registrySession.Connect());
 	CleanupClosePushL(registrySession);

   	// Check all registry entries for the file
   	RPointerArray<CSisRegistryPackage> packages;		
   	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
   	registrySession.InstalledPackagesL(packages);

   	// Go through each package.
   	TInt packageCount = packages.Count();
   	for (TInt i = 0; i < packageCount; ++i)
   		{
   		RSisRegistryEntry entry;
   		entry.OpenL(registrySession, *packages[i]);
   		CleanupClosePushL(entry);

   		RPointerArray<HBufC> files;		
   		CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
   		entry.FilesL(files);

   		// Go through each file in the current package
   		for (TInt j = 0; j < files.Count(); ++j)
   			{
   			// Go through each name
   			TInt k = -1;
   			while (++k < aFileNameArray.Count())
    			{
 	   			if (files[j]->CompareF(*aFileNameArray[k]) == 0)
 		   			{
 		   			// If found then delete the file from the list
 		   			delete aFileNameArray[k];
 		   			aFileNameArray.Remove(k);
					break;
 		   			}
 	   			}
   			}
   		CleanupStack::PopAndDestroy(2, &entry);
   		}

   	CleanupStack::PopAndDestroy(&packages);
   	CleanupStack::PopAndDestroy(&registrySession);
   	}
#endif	

TInt SecurityCheckUtil::ProcessFileL(const CApplication& aApplication,
		RFs& aFs, RArray<TUid>& aSidsAdded, 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		Usif::RStsSession& aStsSession,
#else
		CIntegrityServices& aIntegrityServices,
#endif
		CSisRegistryFileDescription& aFileToProcess, const TDesC& aCurrentFileName)
	{
	const Sis::CController& controller = aApplication.ControllerL();
	TEntry entry;
	User::LeaveIfError(aFs.Entry(aCurrentFileName, entry));
	// Checks that apply to EXEs and DLLs 
	if (entry.IsTypeValid() && (SecUtils::IsExe(entry) || SecUtils::IsDll(entry)))
		{
		// diagnostics
		DEBUG_PRINTF2(_L("Install Server - Security Checking File '%S'"),
			&aFileToProcess.Target());

		TSecurityInfo securityInfo;
		SecurityInfo::RetrieveExecutableSecurityInfoL(aFs, aCurrentFileName, securityInfo);
		if (CompareHeaderAndExeCapabilities(aFileToProcess, securityInfo))
			{
			return EUiCapabilitiesCannotBeGranted;
			}			
		
		const CCertChainConstraints* certChainConstraints = controller.CertChainConstraints();		

		if (CheckVidConstraints(certChainConstraints, securityInfo))
			{
			return EUiConstraintsExceeded;
			}		
		
		if(controller.TrustStatus().ValidationStatus() < EValidatedToAnchor)
			{
			// Application is untrusted
			if(IsVidProtected(securityInfo.iVendorId))
				{
				// Installation of an exe with a protected vendor Id 
				// cannot proceed when the application is not trusted
				return EUiVIDViolation;
				}
			}			
		
		// Checks that only apply to EXEs
		if (SecUtils::IsExe(entry))
			{
			if (CheckSidConstraints(certChainConstraints, securityInfo))
				{
				return EUiConstraintsExceeded;
				}		
						
			if(controller.TrustStatus().ValidationStatus() < EValidatedToAnchor)
				{
				if(IsSidProtected(securityInfo.iSecureId))
					{
					// Installation of an exe with a protected Secure Id 
					// cannot proceed when the application is not trusted
					return EUiSIDViolation;
					}
				}
		
			// if this is an executable check that its SID is not already used by
			// another package
			TUid sid;
			TBuf<KMaxFileName> fileName;
			sid.iUid = securityInfo.iSecureId;
			RSisRegistrySession registrySession;
			User::LeaveIfError(registrySession.Connect());
			CleanupClosePushL(registrySession);
			CSisRegistryPackage* package = NULL;
			TRAPD(err, package = registrySession.SidToPackageL(sid));
			// Non-SP SU (System Upgrade) packages are allowed to overwrite
			// all files regardless of their package UID-s
			TBool spInstall = (aApplication.ControllerL().Info().InstallType() == Sis::EInstAugmentation);	
			TBool isTrustedToEclipse = aApplication.IsInstallSuCertBased();	
			TBool isTrustedToOverwrite = isTrustedToEclipse && !spInstall;
			if(err == KErrNone && package)
				{
				CleanupStack::PushL(package);
		
				if(package->Uid() != controller.Info().Uid().Uid() && !isTrustedToOverwrite)
					{
					User::Leave(KErrAlreadyExists);
					}		
				else  							
					{
					registrySession.SidToFileNameL(sid,fileName);
					// We can replace or eclipse an existing exe with the same
					// name, but not add a new exe with the same sid and
					// different name unless we are removing the old one.
 					if(aFileToProcess.Target().Mid(1).CompareF(fileName.Mid(1)) != 0 &&
 						!IsToBeUninstalledL(fileName, aApplication))
						{
						User::Leave(KErrAlreadyExists);
						}
					}
				CleanupStack::PopAndDestroy(package);
				}
			else if (err == KErrNotFound)
				{				
				// No package has claimed this SID so it can't be a valid
				// eclipsing upgrade. In this case, we need to check the
				// OS SID cache doesn't have this SID in it.
				CSidCache* sidCache;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				sidCache = CSidCache::NewL(aStsSession);
#else
				sidCache = CSidCache::NewL(aIntegrityServices);
#endif
				CleanupStack::PushL(sidCache);
				
				if (sidCache->IsCachedL(sid) && !isTrustedToEclipse)
					{
					// An exe in ROM has the same SID as this file and not trusted to Eclipse the files in the other drives.
					User::Leave(KErrAlreadyExists);
					}
				
				// Check the same SID hasn't already been added as 
				// part of this package.
				if (aSidsAdded.Find(sid) != KErrNotFound)
					{
					User::LeaveIfError(KErrAlreadyExists);
					}	
				CleanupStack::PopAndDestroy(sidCache);
				}
			else
				{
				User::Leave(err);
				}							
			CleanupStack::PopAndDestroy(&registrySession);
			aFileToProcess.SetSid(sid);
			User::LeaveIfError(aSidsAdded.Append(sid));
			}
		}
	else
		{
		TParsePtrC targetPath(aFileToProcess.Target());
		// if this is not an EXE or DLL it cannot be in sys\bin, 
		if (targetPath.Path().CompareF(KBinPath) == 0)
			{
			User::Leave(KErrSISInvalidTargetFile);
			}
		}
	return KErrNone;
	}

#ifdef __WINS__
void SecurityCheckUtil::CleanupWinMemory(TAny* aBuffer)
	{
	LocalFree(LocalHandle ((LPSTR)aBuffer));
	}
#endif

TBool SecurityCheckUtil::IsToBeUninstalledL(const TDesC& aFileName, const CApplication& aApplication)
	{
	const RPointerArray<CSisRegistryFileDescription>& files = aApplication.FilesToRemove();
	TInt count = files.Count();
	for (TInt i = 0; i < count; i++)
		{
		if ((files[i])->Target().CompareF(aFileName) == 0)
			{
			return ETrue;
			}
		}
	return EFalse;
	}


TBool SecurityCheckUtil::IsPrivateDirectory(const TDesC& aFileName)
	{
	TBool result = EFalse;
	TPtrC directory = TParsePtrC(aFileName).Path();
	if (directory.Left(KPrivatePath().Length()).CompareF(KPrivatePath()) == 0)
		{
		result = ETrue;
		}	
	return result;
	}

TBool SecurityCheckUtil::IsProtectedDirectoryL(const TDesC& aFileName)
	{
	TBool result = EFalse;
	if (IsPrivateDirectory(aFileName))
		{
		// Only import directories are considered protected within private directories by this function
		TPtrC directory = TParsePtrC(aFileName).Path();
		const TInt KUidStringLength = 8;
		if (directory.Length() >= KPrivatePath().Length() + KUidStringLength + KImportDir().Length())
			{
			TPtrC imp = directory.Mid(KPrivatePath().Length() + KUidStringLength, KImportDir().Length());
			if (imp.CompareF(KImportDir) == 0)
				{
				result = ETrue;
				}
			}
		}
	else if (IsTargetTcbWriteProtected(aFileName))
		{
		// resource or sys
		result = ETrue;
		}
	return result; 
	}


TBool SecurityCheckUtil::IsSubstedDriveL(RFs& aFs, const TDesC& aFileName)
	{
	TChar drvCh = aFileName[0];
	TInt drvNum;
	User::LeaveIfError(RFs::CharToDrive(drvCh, drvNum));
	TDriveInfo drvInfo;
	User::LeaveIfError(aFs.Drive(drvInfo, drvNum));
	//if the drive is subst'd return true
	if(drvInfo.iDriveAtt & KDriveAttSubsted)
		{
		return ETrue;
		}
	return EFalse;	
	}
	
	
TBool SecurityCheckUtil::IsTargetTcbWriteProtected(const TDesC& aFileName)
	{
	TBool result = EFalse;
	TPtrC resourcePath(KResourcePath);
	TPtrC sysPath(KSysPath);

	TParsePtrC parsedFileName(aFileName);
	TPtrC strippedPath(parsedFileName.Path());

	if (strippedPath.Left(resourcePath.Length()).CompareF(resourcePath) == 0 || strippedPath.Left(sysPath.Length()).CompareF(sysPath) == 0)
		{
		result = ETrue;
		}
	return result; 
	}

void SecurityCheckUtil::FilterNonBlockingFilesOfFilenameL(RArray<TFileName>& aFileNameArray, const TDesC& aFilename, const TUid& aInstallPkgUid)
	{
	DEBUG_PRINTF(_L("Install Server - SecurityCheckUtil::FilterNonBlockingFilesOfFilenameL() called"));

	RBuf fileName; 
	fileName.CreateL(KMaxFileName);
	CleanupClosePushL(fileName);
	
	fileName = aFilename;
	TParsePtr parsedFileName(fileName);

	// Start a sisregistry session
 	RSisRegistrySession registrySession;
 	User::LeaveIfError(registrySession.Connect());
 	CleanupClosePushL(registrySession);

 	RSisRegistryEntry entry;
 	TInt err=entry.Open(registrySession, aInstallPkgUid);
 	if(err==KErrNone)
 		{
		CleanupClosePushL(entry);
	   	RPointerArray<HBufC> files;		
		CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
		entry.FilesL(files);
	
		RBuf entryFileName; 
		entryFileName.CreateL(KMaxFileName);
		CleanupClosePushL(entryFileName);
		
	   	TInt entryFileNameCount=files.Count();  
	
		// Go through each file in the  package
		for (TInt j=0; j<entryFileNameCount; ++j)
			{
			entryFileName = *(files[j]);
			TParsePtr parsedEntryFileName(entryFileName);
			//Go through each name
			TInt k=0;
			while(k<aFileNameArray.Count())
				{
				//if a filename found that is owned by the requested package then remove file from aFileNameArray
				if(files[j]->CompareF(aFileNameArray[k])==0)
		   			{
		   			if(parsedFileName.NameAndExt() == parsedEntryFileName.NameAndExt())
		   				{
		   				//if the actual filename that's being checked is the same as aFilename then we should not take the
		   				//filename out of the array as it is a blocking filename as only one hash par filename can be calculated
		   				++k;
		   				}
		   			else
		   				{
		   				aFileNameArray.Remove(k);
		   				}
		   			}
				else
					{
					++k;
					}
				}
			}
	   	CleanupStack::PopAndDestroy(3, &entry);
   		}
   	else
   		{
   			if(err != KErrNotFound)
   				{
   				User::Leave(err);
   				}
   		}
   	CleanupStack::PopAndDestroy(2, &fileName);
	}


