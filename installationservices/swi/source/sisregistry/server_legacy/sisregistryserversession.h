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
* CSisRegistrySession class definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYSERVERSESSION_H__
#define __SISREGISTRYSERVERSESSION_H__

#include <e32base.h>
#include <e32cmn.h>
#include <swi/msisuihandlers.h>
#include "sisregistryclientserver.h"
#include "sisregistryserver.h"
#include "sisregistrycache.h"

// Forward Declarations
class CX509Certificate;

namespace Swi
{
/**
 * This enum is used to make IPC indexes used as arguments in function calls less obscure.
 * @internalComponent
 * @released
 */

enum TIpcIndexes
	{
	EIpcArgument0 = 0,  ///< IPC indexes arguments 0
	EIpcArgument1,      ///< IPC indexes arguments 1
	EIpcArgument2,      ///< IPC indexes arguments 2
	EIpcArgument3       ///< IPC indexes arguments 3
	};
	

// Swi: Forward Declarations 
class CIntegrityServices;
class CSisRegistryPackage;
class CSisRegistryCache;
class CSisRevocationManager;
class CSecurityManager;
class CSisControllerVerifier;


/**
 * @internalComponent
 * @released
 */

class CSisRegistrySession : public CSession2
	{
public:
	void CreateL();

private:
	~CSisRegistrySession();
	
	CSisRegistryServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	
 	void PanicClient(const RMessagePtr2& aMessage, TSisRegistryPanic aPanic);
	
	//////// Requests processing ////////
	// general 
	void RequestInstalledUidsL(const RMessage2& aMessage);
	void RequestInstalledPackagesL(const RMessage2& aMessage);
	void UidInstalledL(const RMessage2& aMessage);
	void ControllerInstalledL(const RMessage2& aMessage);
	void RequestRegistryEntryL(const RMessage2& aMessage);
	void RequestSidToPackageL(const RMessage2& aMessage);
	void RequestSidToFileNameL(const RMessage2& aMessage);
 	void RequestModifiableL(const RMessage2& aMessage);
 	void RequestHashL(const RMessage2& aMessage);	
	void IsSidPresentL(const RMessage2& aMessage);
	void ShutdownAllAppsL(const RMessage2& aMessage);
	void PackageExistsInRomL(const RMessage2& aMessage);
	void RequestStubFileEntriesL(const RMessage2& aMessage);
	
	// subsession oriented 
	void OpenRegistryUidEntryL(const RMessage2& aMessage);
	void OpenRegistryPackageEntryL(const RMessage2& aMessage);
	void OpenRegistryNamesEntryL(const RMessage2& aMessage);
	void CloseRegistryEntryL(const RMessage2& aMessage);
	void RequestVersionRegistryEntryL(const RMessage2& aMessage);
	void RequestPackageNameRegistryEntryL(const RMessage2& aMessage);
	void RequestLocalizedVendorNameRegistryEntryL(const RMessage2& aMessage);
	void RequestUniqueVendorNameRegistryEntryL(const RMessage2& aMessage);
	void RequestUidRegistryEntryL(const RMessage2& aMessage);
	void RequestLanguageRegistryEntryL(const RMessage2& aMessage);
	void RequestPropertyRegistryEntryL(const RMessage2& aMessage);
	void RequestFileNamesRegistryEntryL(const RMessage2& aMessage);
	
	void RequestFileDescriptionsRegistryEntryL(const RMessage2& aMessage);
	
	void RequestSidsRegistryEntryL(const RMessage2& aMessage);
	void RequestSelectedDriveRegistryEntryL(const RMessage2& aMessage);
	void RequestInstalledDrivesRegistryEntryL(const RMessage2& aMessage);
	void RequestInstallTypeRegistryEntryL(const RMessage2& aMessage);
	void RequestCertificateChainsRegistryEntryL(const RMessage2& aMessage);
	void RequestHashRegistryEntryL(const RMessage2& aMessage);
    void RequestEmbeddedPackageRegistryEntryL(const RMessage2& aMessage);
    void RequestEmbeddingPackagesRegistryEntryL(const RMessage2& aMessage);
    void RequestPackageAugmentationsRegistryEntryL(const RMessage2& aMessage);
    void RequestPackageAugmentationsNumberL(const RMessage2& aMessage);
	void RequestSizeRegistryEntryL(const RMessage2& aMessage);
	void RequestPackageRegistryEntryL(const RMessage2& aMessage);

	void UidPresentRegistryEntryL(const RMessage2& aMessage);
	void SignedRegistryEntryL(const RMessage2& aMessage);
	void RegistryEntryTrustL(const RMessage2& aMessage);
	void TrustTimeStampL(const RMessage2& aMessage);
	void TrustStatusEntryL(const RMessage2& aMessage);
	void RevocationCheckEntryL(const RMessage2& aMessage);
	void CancelRevocationCheckEntryL(const RMessage2& aMessage);
	void InRomRegistryEntryL(const RMessage2& aMessage);
	void PreInstalledRegistryEntryL(const RMessage2& aMessage);
	void AugmentationRegistryEntryL(const RMessage2& aMessage);
	void RequestControllersL(const RMessage2& aMessage);

	// support for the writeable interface
	void RegisterEntryL(const RMessage2& aMessage, TBool aNewEntry);	
	void DeleteEntryL(const RMessage2& aMessage);
	void RequestDependenciesRegistryEntryL(const RMessage2& aMessage);
	void RequestDependentPackagesRegistryEntryL(const RMessage2& aMessage);
	void AddDriveL(const RMessage2& aMessage);
	void RemoveDriveL(const RMessage2& aMessage);
	void DeletablePreInstalledRegistryEntryL(const RMessage2& aMessage);
	void SignedBySuCertRegistryEntryL(const RMessage2& aMessage);

	// Regenerate the registry cache from the disk
	void RegenerateCacheL(const RMessage2& aMessage);
	
	/// Tells the server cache to update the trust status of the entry with
	/// the specified uid. 
	void UpdateTrustStatusL(const TUid& uid, 
	                        const TSisTrustStatus& status );
	
	void VerifyControllerL(const RMessage2& aMessage);
	
	//Request to get the value of RemoveWithLastDependent flag
	void RemoveWithLastDependentL(const RMessage2& aMessage);
	//Request to set the value of RemoveWithLastDependent flag
	void SetRemoveWithLastDependentL(const RMessage2& aMessage);

	// Non-removable
	void RequestRemovablePackagesL(const RMessage2& aMessage);
	void IsRemovableL(const RMessage2& aMessage);
	
	void RecoverL(const RMessage2& aMessage);	
	void LoggingFileInfoL(const RMessage2& aMessage);
	void RequestMatchingSupportedLanguagesL(const RMessage2& aMessage);
	void RequestInternalRegistryFilesL(const RMessage2& aMessage);
private:
    friend class CSisRevocationManager;
	RFs iFs;

    CSisRevocationManager* iRevocationManager;
    CSisControllerVerifier* iSisControllerVerifier;
    RPointerArray<HBufC8> 	iControllerArray;
	};
	
inline CSisRegistryServer& CSisRegistrySession::Server()
	{
	// cast to remove const-ness
	return *static_cast<CSisRegistryServer*>(const_cast<CServer2*>(CSession2::Server()));
	}
	
} //namespace

#endif // __SISREGISTRYSERVERSESSION_H__
