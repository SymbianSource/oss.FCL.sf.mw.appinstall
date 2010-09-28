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
#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>
#include <usif/sts/sts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <swi/swiobserverclient.h>
#include "sisregistryclientserver.h"
#include "sisregistryserver.h"
#include "sisregistrylog.h"
#include "sisregistrypackage.h"
#include "sisregistryserversubsession.h"

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
class CSisRegistryPackage;
class CSisRegistryToken;
class CSisRegistryObject;
class CSisRevocationManager;
class CSecurityManager;
class CSisControllerVerifier;
class CHashContainer;
class CSisRegistryFileDescription;
class TSisTrustStatus;

class CSisRegistrySession : public CSession2
	{
public:
	void CreateL();

	// Methods accessed from the subsession objects
	RFs& Fs();
	CSisRegistryServer& Server();
	Usif::RSoftwareComponentRegistry& ScrSession();
 	void PanicClient(const RMessagePtr2& aMessage, TSisRegistryPanic aPanic);
	void GetAugmentationDetailsL(const TUid& aUid, RPointerArray<CSisRegistryPackage>& aPackagesArray, Usif::TComponentId aBaseComponentId);
	TInt GetAugmentationCountL(const TUid& aUid);
	TBool IdentifyControllerL(Usif::TComponentId aComponentId, const TDesC& aFileName); // Used by CSisRegistrySubSession

	CSisRegistrySession(TSecureId aClientSid);
	
private:
	~CSisRegistrySession();
	
	// Returns ETrue if this is the first run of SWIRegistry server ever (or after firmware upgrade)
	TBool IsFirstInvocationL();

	// Does initialization required when run after very first boot of phone (or after firmware upgrade)
	// Method is only invoked when such a state is detected
	// Leaves behind a file in <systemdrive>:\sys to mark a successful initialization 
	void FirstInvocationInitL();

	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	
	//////// Requests processing ////////
	// general 
	void RequestInstalledUidsL(const RMessage2& aMessage);
	void RequestInstalledPackagesL(const RMessage2& aMessage);
	void UidInstalledL(const RMessage2& aMessage);
	void ControllerInstalledL(const RMessage2& aMessage);
	void RequestSidToPackageL(const RMessage2& aMessage);
	void RequestSidToFileNameL(const RMessage2& aMessage);
 	void RequestModifiableL(const RMessage2& aMessage);
 	void RequestHashL(const RMessage2& aMessage);	
	void IsSidPresentL(const RMessage2& aMessage);
	void PackageExistsInRomL(const RMessage2& aMessage);
	void RequestStubFileEntriesL(const RMessage2& aMessage);
	void RequestRegistryEntryL(const RMessage2& aMessage);
	void IsFileRegisteredL(const RMessage2& aMessage);
	void GetComponentIdForUidL(const RMessage2& aMessage);
	void AddAppRegInfoL(const RMessage2& aMessage);
    void RemoveAppRegInfoL(const RMessage2& aMessage);
    void ValidateAndParseAppRegFileL(const TDesC& aRegFileName, Usif::CApplicationRegistrationData*& aAppRegData, TBool aInternal);
    void ReRegisterAppRegFileL(const TDesC& aRegFileDrivePath, const TUid& aAppUid);
	
	// Subsession handling
	void OpenRegistryUidEntryL(const RMessage2& aMessage);
	void OpenRegistryPackageEntryL(const RMessage2& aMessage);
	void OpenRegistryNamesEntryL(const RMessage2& aMessage);
	void CloseRegistryEntryL(const RMessage2& aMessage);

	// support for the writeable interface

	/* 
	 * Transaction support APIs. 
	 * Note that these cover only the mutable APIs (RegistryEntryL & DeleteEntryL)
	 */
	void CreateTransactionL(const RMessage2& aMessage);
	void CommitTransactionL(const RMessage2& aMessage);
	void RollbackTransactionL(const RMessage2& aMessage);

	void RegisterEntryL(const RMessage2& aMessage, TBool aNewEntry, TBool aRegisterSoftwareTypes);
	void AppRegInfoEntryL(const RMessage2& aMessage);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void RegisterAllAppL(RBuf& aApparcRegFolder);
    void GetComponentIdForPackageL(const RMessage2& aMessage);
    void GetAppUidsForComponentL(const RMessage2& aMessage);
    void GetComponentIdsForUidL(const RMessage2& aMessage);
#endif

	void DeleteEntryL(const RMessage2& aMessage);
	void DeleteEntryL(const CSisRegistryObject& aObject, TInt64 aTransactionId, TBool aCleanupRequired = ETrue);
	void AddSoftwareTypeL(const RMessage2& aMessage);
	void AddDriveL(const RMessage2& aMessage);

	/// Updates the trust status of the base package identified by uid
	void UpdateTrustStatusL(const TUid& aUid, const TSisTrustStatus& aTrustStatus);
	
	// Non-removable
	void RequestRemovablePackagesL(const RMessage2& aMessage);
	
	void RecoverL(const RMessage2& aMessage);
	
	void LoggingFileInfoL(const RMessage2& aMessage);

	//sets a particular component's state to the requested value
	void SetComponentStateL(const RMessage2& aMessage);
	void SetComponentPresenceL(const RMessage2& aMessage);

	// Helper methods
	TBool IsRegisteredL(const TUid& aUid);
	TBool IsRegisteredL(const CHashContainer& aHashContainer);
	TBool IsRegisteredL(const TUid& aUid, const TDesC& aPackageName);

	void RemoveEntryL(const Usif::TComponentId aCompId);
	HBufC* RemoveEntryLC(const CSisRegistryPackage& aPackage);
	void RemoveCleanupInfrastructureL(const CSisRegistryObject& aObject, Usif::RStsSession& aStsSession);
	TBool RemoveControllerL(const CSisRegistryObject& aObject, Usif::RStsSession& aStsSession, TInt aDrive);
	void ControllerDriveListL(const CSisRegistryObject& aObject, RArray<TInt>& aDriveList);
	TUint FixedDrivesL() const;
	Usif::TComponentId AddRegistryEntryL(CSisRegistryObject& aObject, Usif::RStsSession& aStsSession, const TDesC8& aController, Usif::TScrComponentOperationType aOpType);
	Usif::TComponentId AddEntryL(CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType);
	Usif::TComponentId AddInRomControllerEntryL(CSisRegistryObject& aObject, const TDesC& aStubFileName);
	TBool RomBasedPackageL(const TUid& aUid);
	void AddAppsFromStubL(Usif::TComponentId aCompId, const TDesC& aFileName);
	void AddControllerL(const CSisRegistryObject& aObject, Usif::RStsSession& aStsSession, const TDesC8& aBuffer, const TInt aDrive);
	void AddCleanupInfrastructureL(CSisRegistryObject& aObject, Usif::RStsSession& aStsSession, const TDesC8& aControllerBuffer);
	TUint CreateSubsessionHandleL(const TUid& aPackageUid);
	TUint CreateSubsessionHandleL(const CSisRegistryPackage& aPackage);
	TUint CreateSubsessionHandleL(const TDesC& aPackageName, const TDesC& aVendorName);
	void CloseSubsessionHandleL(TUint aId);
	Usif::CComponentEntry* ComponentLC(TUint aSubsessionId);
	CSisRegistrySubSession& SubsessionL(TUint aSubsessionId);
	TUint OpenedPackageIndexL(TUint aSubsessionId);
	void ProcessRemovableDriveL(TInt aDrive);
	void DiscoverControllersL(const TDesC& aRegistryPath, const TDesC& aDirectoryName);
	void ExecuteUninstallLogL(const TDesC& aUninstallLogFile, const TDesC& aControllerFile);
	void GetComponentIdsForSidL(TUid aSid, RArray<Usif::TComponentId>& aComponentIds);
	HBufC* SidToFileNameL(TUid aSid, Usif::TComponentId& aComponentId, TInt aExpectedDrive);
	TBool ModifiableL(const TDesC& aFileName);
	CHashContainer* HashL(const TDesC& aFileName);
	void RemovablePackageListL(RPointerArray<CSisRegistryPackage>& aPackages);
	void ProcessRomDriveL();
	void RegisterInRomControllerL(const TDesC& aFileName);
	void RegisterSoftwareTypesL(Usif::TComponentId aComponentId, const RMessage2& aMessage);
	void UnregisterSoftwareTypesL(Usif::TComponentId aComponentId);
	
	void ProcessRomStubsL();
	void ProcessApplicationsL();
	TBool IsFirmwareUpdatedL();	 
	void  UpdateRecentFWVersionL(); 
	void GetStubFileInfoL(TUid aUid, TStubExtractionMode aMode, TInt aStartingFileNo, TInt& aFileCount, RPointerArray<HBufC>& aFileNames);
	void GetStubFilesL(const TDesC& aFileName, RPointerArray<HBufC>& aFileNames);
	void DriveFormatDetectedL(TDriveUnit aDrive);
	
private:
    friend class CSisRevocationManager;

    // File system session which is used to make all requests to file server
	RFs iFs;

	// Used to hold infomation about opened packages (as subsessions)
	// The index value (+ 1) is returned as the subsession ID
	RPointerArray<CSisRegistrySubSession> iOpenedPackages;

	// The registry is stored on the system drive
	TDriveNumber iSystemDrive;
	
	// Session handle to Software Component Registry
	Usif::RSoftwareComponentRegistry iScrSession;

	TSecureId iClientSid;
	TBool iIsFwUpdated;
	TBool iIsFirstInit;
	};

inline RFs& CSisRegistrySession::Fs()
	{
	return iFs;
	}

inline CSisRegistryServer& CSisRegistrySession::Server()
	{
	// cast to remove const-ness
	return *static_cast<CSisRegistryServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

inline Usif::RSoftwareComponentRegistry& CSisRegistrySession::ScrSession()
	{
	return iScrSession;
	}

inline CSisRegistrySession::CSisRegistrySession(TSecureId aClientSid)
    :CSession2()
    {
    iClientSid = aClientSid;
    }

} //namespace

#endif // __SISREGISTRYSERVERSESSION_H__


