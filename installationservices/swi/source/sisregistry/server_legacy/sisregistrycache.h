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
* CSisRegistryCache - class used to store and access registry entry information in RAM 
* serving as a "cache" alternative to reading/seaching from other storage media
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYCACHE_H__
#define __SISREGISTRYCACHE_H__

#include <e32base.h>
#include <f32file.h>
#include <swi/swiobserverclient.h>
#include <s32strm.h>
#include <s32mem.h>
#include "sisregistrylog.h"


class RReadStream;
class RWriteStream;


namespace Swi
{
class CIntegrityServices;
class CSisRegistryPackage;
class CSisRegistryFile;
class CSisRegistryObject;
class CSisRegistryToken;
class CHashContainer;
class CSisRegistryFileDescription;
class TSisTrustStatus;

/**
 * @internalComponent
 * @released
 */

class CSisRegistryCache : public CBase
	{
public:
	static CSisRegistryCache* NewL();
	static CSisRegistryCache* NewLC();
	
	~CSisRegistryCache();
	
	/**
	 * Returns the current list of registered Uids, even those on removable media
	 */
	void UidListL(RArray<TUid>& aUids) const;

	/**
	 * Returns the current list of registered packages, even those on removable media
	 */
	void PackageListL(RPointerArray<CSisRegistryPackage>& aPackages) const;
	
	/**
	 * Returns the current list of registered packages, even those on removable media
	 */
	void PackageAugmentationsL(const TUid aUid, RPointerArray<CSisRegistryPackage>& aPackages) const;
	
	/**
	 * Returns the number of augmentations of a registred package with the passed UID
	 *
	 * @param aUid the UID of the package
	 * @return the augmentations number of the package
	 */
	TInt PackageAugmentationsNumber(const TUid aUid) const;
	
	/**
	 * Returns the Rfs handle, serves as a central point for all RFs calls
	 */
	RFs& RFsHandle();
	
	/**
	 * Returns a number between 1 and 0xFFFFFFFF, which internally identifies  
	 * a subsession and the assocaited registry entry 
	 */
    static TUint AllocateSubsessionId();
	
	/**
	 * Perform search of whether a specific object has been registered
	 */
	TBool IsRegistered(const TUid aUid) const;
	TBool IsRegistered(const CSisRegistryPackage& aPackage) const;
	TBool IsRegistered(const CHashContainer& aHashContainer) const;
	
	/**
	 * Perform search of whether a specific object (package, token) 
	 * has been registered conforming to specific search arguments. If found returns 
	 * reference to the object in question leave if not found, or if error
	 */
    CSisRegistryPackage& PackageL(const TUid aUid) const;
    CSisRegistryPackage& PackageL(const TUid aUid, TInt aIndex) const;
	CSisRegistryPackage& PackageL(const TDesC& aName, const TDesC& aVendor) const;
	CSisRegistryToken& TokenL(const TUid aUid, const TDesC& aName, const TDesC& aVendor) const;
	CSisRegistryToken& TokenL(const TUid aUid, TInt aIndex) const;
	
	/**
	 * create an object from registry file
	 */
	CSisRegistryObject* ObjectL(TUid aUid, const TDesC& aName, const TDesC& aVendor);
	CSisRegistryObject* ObjectL(TUid aUid, TInt aIndex);
	CSisRegistryObject* ObjectL(const CSisRegistryToken& aToken);
	
	CSisRegistryPackage& SidToPackageL(const TUid aSid) const;
	void SidToPackageL(const TUid aSid, RArray<CSisRegistryPackage>& aListMatchingPackages ) const;
	
	/**
	 * Find the given filename from the list of all tokens.
	 * Return whether file is modifiable.
	 */
	TBool ModifiableL(const TDesC& aFileName);
	
	/**
	 * Find the given filename from the list of all tokens.
	 * Return its hash.
	 */
	CHashContainer* HashL(const TDesC& aFileName);	

	void OpenReadHandleL(const CSisRegistryPackage& aPackage, TUint& aId);
    void CloseReadHandleL(TUint aId);
    
    CSisRegistryFile& HandleEntryL(TUint aId);
	CSisRegistryObject& EntryObjectL(TUint aId);
	
    
    void DependentsPackagesL(const CSisRegistryObject& aObject,
						 RPointerArray<CSisRegistryPackage>& aDependents);

	void EmbeddingPackagesL(const CSisRegistryObject& aObject,
						 RPointerArray<CSisRegistryPackage>& aEmbedding);
	
   	void GenerateChainListL(const CSisRegistryObject& aObject, 
   							RPointerArray<HBufC8>& aChainList);	
   							
	 /**
	 Loads a single controller belonging to the registry object and
	 places the controller buffer on the cleanup stack.
	 
	 @param aObject The SIS registry object that owns the controller
	 @param aIndex The index of the controller
	 @return a pointer to the buffer containing the controller.
	 */
	 HBufC8* LoadControllerLC(const CSisRegistryObject& aObject, TUint aIndex);   							

	void GenerateControllersArrayL(const CSisRegistryObject& aObject, 
									RPointerArray<HBufC8>& aControllers);	
	
	// drive oriented functionality											 
	void AddDriveAndRefreshL(TInt aDrive);
	void RemoveDriveAndRefresh(TInt aDrive);

	inline TUint FixedDrives() const;

	void RemoveRegistryEntryL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService);
	void RemoveRegistryFilesL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService);
	void AddRegistryEntryL(CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService, const TDesC8& aController);

 	/// Regenerates the cache to match the files on disk
	// Existing integrity services journal files will be committed.
 	void RegenerateCacheL();

    /** 
     * Updates the trust status settings in the registry for the 
     * package identified by uid. Note that this updates the registry's 
     * file system.
     *
     * @param uid The uid of the registry entry to be updated
     * @param status The new trust status values to assign to the entry
     *
     * @return KErrNone on success
     * @return KErrInUse if another process holds the registry file requiring the update
     */
    TUint UpdateTrustStatusL(const TUid& uid,
                             const TSisTrustStatus& status );

	TBool IsSidPresent(const TUid aSid) const;
	
	/** 
     * Sets the remove with last dependent flag in the registry for the 
     * package identified by uid. Note that this updates the registry's 
     * file system.
     *
     * @param uid The uid of the registry entry to be updated
     * @param status The new trust status values to assign to the entry
     *
     * @return KErrNone on success
     * @return KErrInUse if another process holds the registry file requiring the update
     */
	TUint SetRemoveWithLastDependentL(const TUid& uid);
	
	/**
	 * Returns an array of the currently installed packages which can be removed
	 * (including augmentations).
	 *
	 * @param aPackages The array of installed packages which are removable
	 */
	void RemovablePackageListL(RPointerArray<CSisRegistryPackage>& aPackages);
	
	/**
	 * Performs a roll back and then regenerates the cache to match the registry files on disk
	 */
	void RecoverL();
	
private:	
	void ConstructL();

    /**
	 *  Used to locate the index of a registry file index in iFiles, which corresponds to 
	 *  aId argument value
	 */    	
	TInt FindReadHandleIndx(TUint aSubsessionId);	

    /**
	 * Used to cleanup the object as a part of a reset. 
	 */    		
    void CleanUp();
    	
	/**
	 * Builds the array of installed packages by scanning for UID directories
	 * within the SwiRegistry path.
	 *                                                   |-UID(hex)-|
	 * The path is in the form ...\sys\install\swiregistry\uuuuuuuu\
	 */
	void BuildUidListL();
	
	/**
	 * Builds the list with underlying files in a specific package directory
	 */
	void BuildFileListL(TUid aUid, const TDesC& aUidPath);
	
	void BuildRomListL();
	void RegisterInRomControllerL(const TDesC& aFileName);
	void DiscoverUidsL(TInt aDrive);
	void DiscoverStubsL(TInt aDrive);
	void RegisterControllerL(const TDesC& aFileName, CFileMan& aFileMan);
	void DiscoverControllersL(const TDesC& aRegistryPath, const TDesC& aDirectoryName);
	TBool IdentifyControllerL(TUid aUid, TInt aAugmentationIndex, const TDesC& aFileName);
	inline TBool UseIntegrityServices() const; // Integrity services is not needed during ROM stub processing

	/** 
	 * Cache internal state management
	 */
	 
	// check whether last time SisRegistryServer closed down correctly
	TBool BackupExists();
	
	// use backup file to initialise internal information;
	// files are committed at the end of the process 
	void InitFromBackupL();
	
	// used after a SisRegistryServer crashed / at first start up
	// files are committed at the end of the process 
	void InitStartUpL();
	
	// creates a copy of registry internal state in a file 
	void StoreBackupL();
    
    /**
	 * Drive management oriented
	 */    		
    void InitDrivesL();
    void UpdatePackagePresentStateL();
    
    // create an array of the drives used by a specific installed package 
	void ControllerDriveListL(const CSisRegistryObject& aObject, RArray<TInt>& aDriveList);
							  
	void AddEntryL(CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService);				
	void RemoveEntryL(const CSisRegistryPackage& aPackage, CIntegrityServices& aIntegrityService);
	
 	void AddControllerL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService, const TDesC8& aBuffer, const TInt aDrive);
 	TBool RemoveControllerL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityService, TInt aDrive);
 
 	void AddCleanupInfrastructureL(CSisRegistryObject& aObject, CIntegrityServices& aIntegrityServices, const TDesC8& aControllerBuffer);
 	void RemoveCleanupInfrastructureL(const CSisRegistryObject& aObject, CIntegrityServices& aIntegrityServices);

    /**
     * Persists the provided registry object into the specified file. Note all
     * other CSisRegistryFile objects refering to the specified file must have
     * their streams closed before calling this method.
     *
     * @param filename The file to write
     * @param obj The object to persist
     **/
    void UpdateRegistryFileL(const TFileName& filename,
                             const CSisRegistryObject& obj);

	void ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3);

	TUid IdentifyUidFromSisFileL(TDesC& aFilename);
	
	/**
	 * Resolves and sets the index value on embedded packages
	 */
	void ResolveEmbeddedPackage(CSisRegistryPackage* aPackage);
	 
	/**
	 * Updates the localizedvendorname and packagenames as per the current
	 * language of the device
	 */
	void UpdateLocalizedNamesL();
	 
	void CloseAllHandlesForUid(const TUid& aUId);
	 
	void ReloadAllHandlesForUidL(const TUid& aUId, const HBufC *registryFilename);	

	TBool IsFirmwareUpdatedL();
	 
	void  UpdateRecentFWVersionL(); 
		
private:
	// File system session which is used to make all requests to file server
	RFs iFs;
	
	// An array of 
	RPointerArray<CSisRegistryToken> iTokens;
	
	// Subsession id reference counter for currently open subsessions
	// it starts from 1 and increments with every subsession Open request
	// the counter rolls over when it reaches max value. It is assumed that 
	// this method reallistically would not suffer from id clash 
	static TUint iSubsessionId;	
	
	// files corresponding to subsessions
	// array of pointers to curenltly open registry objects 
	// the array index does not bear any relation to the id reference counter	
	RPointerArray<CSisRegistryFile> iFiles;

	// internal (sisregistryserver specific) object used to maintain the registry integrity 	
	CIntegrityServices* iIntegrityService;
	
	// Bitmap members which store currently present drives, one for all drives and one for 
	// permanent(fixed) drives.   
	TUint iPermDrives;  // stores just the fixed drives
	
	// Backup file
	HBufC* iBackupFile;
	
	// The registry is stored on the system drive
	TDriveNumber iSystemDrive;
	
	// ETrue if integrity services is needed for registry consistency
	TBool iUseIntegServices;
	
	//inner class of CSisRegistryCache
	class CSisLangChangeMonitor; 
	
	// Active Object used for language change monitoring.
	CSisLangChangeMonitor* iSisRegLangMonitor; 
	
	TBool isFwUpdated;
private:
	friend class CSisLangChangeMonitor;
	
public:
 	/**
 	 * Adds an entry to SWI Log for aObject.
 	 * 
 	 * @param aObject CSisRegistryObject 
     * @param Installinfo descriptor containing installation type
     */
    
  	void AddLogEntryL(const CSisRegistryObject& aObject, TSwiLogTypes aInstallinfo);

	};
	
inline TUint CSisRegistryCache::FixedDrives() const
	{
	return iPermDrives;
	}

inline TUint CSisRegistryCache::AllocateSubsessionId()
	{
	return (iSubsessionId < 0xFFFFFFFF)?(++iSubsessionId):(iSubsessionId = 1);	
	}
inline TBool CSisRegistryCache::UseIntegrityServices() const
	{
	return iUseIntegServices;
	}

} //namespace

#endif //__SISREGISTRYCACHE_H__
