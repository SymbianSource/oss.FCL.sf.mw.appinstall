/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisRegistrySubSession class definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYSERVERSUBSESSION_H__
#define __SISREGISTRYSERVERSUBSESSION_H__

#include <e32base.h>
#include <e32cmn.h>
#include <usif/scr/scr.h>
#include "sisregistryserver.h"

namespace Swi
{
	
// Swi: Forward Declarations 
class CSisRegistryPackage;
class CSisRegistryFileDescription;
class CSisRevocationManager;
class CSisControllerVerifier;

class CSisRegistrySubSession : public CBase
{
public:
	~CSisRegistrySubSession();
	CSisRegistrySubSession(CSisRegistrySession& aSession, Usif::TComponentId& aCompId, TUid aCompUid);
	void ResetFileAPIData();

public:
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
	void RequestDependenciesRegistryEntryL(const RMessage2& aMessage);
	void RequestDependentPackagesRegistryEntryL(const RMessage2& aMessage);
	void DeletablePreInstalledRegistryEntryL(const RMessage2& aMessage);
	void TrustTimeStampL(const RMessage2& aMessage);
	void TrustStatusEntryL(const RMessage2& aMessage);
	void RevocationCheckEntryL(const RMessage2& aMessage);
	void CancelRevocationCheckEntryL(const RMessage2& aMessage);
	void ShutdownAllAppsL(const RMessage2& aMessage);

	//Request to get the value of RemoveWithLastDependent flag
	void RemoveWithLastDependentL(const RMessage2& aMessage);
	//Request to set the value of RemoveWithLastDependent flag
	void SetRemoveWithLastDependentL(const RMessage2& aMessage);

	void IsRemovableL(const RMessage2& aMessage);
	void InRomRegistryEntryL(const RMessage2& aMessage);
	void AugmentationRegistryEntryL(const RMessage2& aMessage);
	void RequestControllersL(const RMessage2& aMessage);
	void PreInstalledRegistryEntryL(const RMessage2& aMessage);
	void VerifyControllerL(const RMessage2& aMessage);
	void SignedBySuCertRegistryEntryL(const RMessage2& aMessage);
	void RequestMatchingSupportedLanguagesL(const RMessage2& aMessage);

private:
	// Helper methods
	Usif::CComponentEntry* ComponentL();
	void FileListL(RPointerArray<HBufC>& aFiles);
	void FileDescriptionListL(RPointerArray<CSisRegistryFileDescription>& aFileDescriptions);
	TBool IsCompletelyPresentL();
	void GenerateControllersArrayL(const Usif::TComponentId aCompId, RPointerArray<HBufC8>& aControllers);
	TUint InstalledDrivesL();
	void EmbeddingPackagesL(const Usif::TComponentId aCompId, TUid aCompUid, RPointerArray<CSisRegistryPackage>& aEmbeddingPackages);
	void GenerateChainListL(const Usif::TComponentId aCompId, RPointerArray<HBufC8>& aChainList);	
	TBool UidPresentInStubL(const TUid& aUid, TInt aDrive);
	TBool UidPresentInFlaggingControllerL(Usif::TComponentId aComponentId, const TUid& aUid, TInt aDrive, TInt aAugmentationIndex, TInt aComponentControllerCount);

private:
	// Handle to session object
	CSisRegistrySession& iSession;

	// The component id of the opened component
	Usif::TComponentId iComponentId;
	// The UID of the opened component
	TUid iCompUid;
	
	// Information about the FilesL and FileDescriptionListL API since these return entries in chunks
	// spread over more than one call
	TInt iNumFilesSent;
	TInt iWildcardFileCount;
	TInt iRegularFileCount;

	// Saved file description in case we did not manage to fit into the buffer. Used in order to return it on the next corresponding call to SWI Registry.
	CSisRegistryFileDescription* iSavedFileDesc;
	Usif::RSoftwareComponentRegistryFilesList* iScrFileList;

	CSisRevocationManager* iRevocationManager;
	CSisControllerVerifier* iSisControllerVerifier;
	RPointerArray<HBufC8> 	iControllerArray;
	Usif::CComponentEntry* iComponentEntry;
};

} //namespace

#endif // __SISREGISTRYSERVERSUBSESSION_H__


