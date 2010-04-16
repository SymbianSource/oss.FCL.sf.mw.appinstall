/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CScrServer providing management of SCR services.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "scrserver.h"
#include "scrrequestimpl.h"
#include "usiflog.h"
#include <scs/cleanuputils.h>
#include "scrhelperclient.h"
#include <scs/securityutils.h>

using namespace Usif;

static const TUint scrRangeCount = 13;

static const TInt scrRanges[scrRangeCount] =
	{
	0,							 // Range-0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	 // Range-1 - EBaseSession to EBaseMustAllow exclusive.
	CScsServer::EBaseSession | EGetSingleComponentSize, // Modification and transaction APIs have custom checks
	CScsServer::EBaseSession | EGetComponentIdListSize, // Component-specific APIs are free for all
	CScsServer::EBaseSession | EGetApplicationLaunchersSize, // Getting all component IDs is restricted to ReadUserData
	CScsServer::EBaseSession | EGetIsMediaPresent, // Only allowed by apparc
	CScsServer::EBaseSession | EAddApplicationEntry, // Component-specific APIs are free for all
	CScsServer::EBaseSession | EGetPluginUidWithMimeType, // Custom checks (as with the rest of modification APIs)
	CScsServer::EBaseSession | EAddSoftwareType, // File filter sub-sessions and plugin-fetching APIs are free for all	
	CScsServer::EBaseSubSession | EOpenComponentsView, // Software Type management APIs are only allowed for SWI 
	CScsServer::EBaseSubSession | EOpenFileList, // Component filter sub-sessions require ReadUserData	
	CScsServer::EBaseSubSession | EOpenApplicationRegistrationInfoView, // Always pass
	CScsServer::EBaseMustAllow // Application Registration view to be only allowed by apparc, the rest of the range is reserved for SCS, and must be allowed( EBaseMustAllow to KMaxTInt inclusive)	
	};

static const TUint8 scrElementsIndex[scrRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 is not supported.
	CPolicyServer::ECustomCheck,
	CPolicyServer::EAlwaysPass,
	0, // Require ReadUserData
	2, // Only Apparc process can invoke	
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ECustomCheck,
	CPolicyServer::EAlwaysPass,	
	1, // Only SWI process can invoke	
	0, // Require ReadUserData
	CPolicyServer::EAlwaysPass,
	2, // Only Apparc process can invoke
	CPolicyServer::EAlwaysPass 
	};							

static const CPolicyServer::TPolicyElement scrElements[] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S0(KSisRegistryServerSid.iId), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S0(KApparcServerSid.iId), CPolicyServer::EFailClient}	
	};

static const CPolicyServer::TPolicy scrPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connections
	scrRangeCount,
	scrRanges,
	scrElementsIndex,
	scrElements,
	};

//
// CScrServer
//

CScrServer::CScrServer()
/**
	Intializes the SCR server object with its version and policy.
 */
	:	CScsServer(ScrServerVersion(), scrPolicy)
		{
		DEBUG_PRINTF(_L8("SCR server construction!"));
		}

CScrServer::~CScrServer()
/**
	Destructor. Cleanup the SCR Server.
 */
	{
	DEBUG_PRINTF(_L8("SCR server destruction!"));
	delete iRequestImpl;
	iDatabaseFile.Close();
	iJournalFile.Close();
	iFs.Close();
	iSubsessionOwners.Close();
	}
	
	
CScrServer* CScrServer::NewLC()
/**
	Factory function allocates a new instance of CScrServer.

	@return		The newly created instance of CScrServer which is left on the cleanup stack.
 */
	{
	CScrServer *self = new (ELeave) CScrServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CScrServer::ConstructL()
/**
	Second phase constructor starts the SCR server.
 */
	{
	// StartL() must be called first in order to avoid concurrency issue when starting the server from two or more processes
	StartL(KSoftwareComponentRegistryName);
	CScsServer::ConstructL(KScrServerShutdownPeriod);
	SetupL();
	}

void CheckFilePathL(RFs& aFs, RFile& aFile, const TDesC& aExpectedPath)
	{
	RBuf fileName;
	fileName.CreateL(KMaxFileName);
	fileName.CleanupClosePushL();
	User::LeaveIfError(aFile.FullName(fileName));
	
	RBuf expectedFileName;
	expectedFileName.CreateL(aExpectedPath.Length());
	expectedFileName.CleanupClosePushL();
	expectedFileName.Copy(aExpectedPath);
	expectedFileName[0] = aFs.GetSystemDriveChar();
	
	__ASSERT_ALWAYS(!fileName.CompareF(expectedFileName), User::Leave(KErrBadHandle));
		
	CleanupStack::PopAndDestroy(2, &fileName); // fileName, expectedFileName
	}

void CScrServer::SetupL()
	{
	// Connect to the file server
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	
	// Connect to the SCR database
	RScrHelper sh;
	User::LeaveIfError(sh.Connect());
	CleanupClosePushL(sh);
	
	// Retrieve handles for database and journal files
	sh.RetrieveFileHandlesL(iDatabaseFile, iJournalFile);
	CleanupStack::PopAndDestroy(&sh);
	
	// Check if the retrieved file handles belong to the expected files (KScrDatabaseFilePath/KScrJournalFilePath).
	// This is an extra security measure in the SCR to check that the file handle transferred from SCR Helper Server  
	// really corresponds to scr.db/scr.db-journal in the expected location.
	CheckFilePathL(iFs, iDatabaseFile, KScrDatabaseFilePath);
	CheckFilePathL(iFs, iJournalFile, KScrJournalFilePath);
	
	ReconnectL();
	}

CScsSession* CScrServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CScsServer by allocating a new instance of CScrSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of the SCR session class which is owned by the caller.
 */
	{
	DEBUG_PRINTF(_L8("SCR session creation!"));
	return CScrSession::NewL(*this, aMessage);
	}

CPolicyServer::TCustomResult CScrServer::CheckComponentIdMatchingEnvironmentL(const RMessage2& aMsg, TBool aCheckForSingleApp)
	{	
	TComponentId componentId = 0;
	if(aCheckForSingleApp)
	    {
        // Get the application uid from RMessage2
        TInt applicationUid = aMsg.Int0();        
        // Get the component id for the application            
        if(!iRequestImpl->GetComponentIdForAppInternalL(TUid::Uid(applicationUid), componentId))
            return EFail;
	    }
	else
	    {
	    componentId = CScrRequestImpl::GetComponentIdFromMsgL(aMsg);
	    }
	
	TSecureId clientSid = aMsg.SecureId();

    if (componentId == 0)
        {
        if (clientSid == KSisRegistryServerSid)
            return EPass;
        }   
    
    TBool vaildSid = EFalse;
	RArray<TSecureId> installerSids;
	CleanupClosePushL(installerSids);
	if (iRequestImpl->GetInstallerOrExecutionEnvSidsForComponentL(componentId, installerSids))
	    {
        TInt count = installerSids.Count();
        for (TInt i = 0; i < count; i++)
            {
            if (clientSid == installerSids[i])
                {
                vaildSid = ETrue;
                break;
                }
            }
	    }
	CleanupStack::PopAndDestroy(&installerSids);
	
	if (vaildSid)
		return EPass;
			
	DEBUG_PRINTF3(_L("Neither installer nor execution environment matched the client while checking for component-matching environment. Client SID %d, Component ID %d"), 
			TUint32(clientSid), componentId);
	return EFail;
	}
	
CPolicyServer::TCustomResult CScrServer::CheckDeleteComponentAllowedL(const RMessage2& aMsg)	
	{
	const TUint KSifServerSid = 0x10285BCB;	
	
	if (CheckComponentIdMatchingEnvironmentL(aMsg) == EPass)
		return EPass;
		
	TSecureId clientSid = aMsg.SecureId();		
	TComponentId componentId = CScrRequestImpl::GetComponentIdFromMsgL(aMsg);
	// If a component does not have corresponding environments, it means that it is orphaned. In this case, SIF is allowed to forcefully remove it
	if (iRequestImpl->GetIsComponentOrphanedL(componentId) && (TUint32(clientSid) == KSifServerSid))
		{
		DEBUG_PRINTF2(_L("Allowing SIF to uninstall orphaned component %d"), componentId);
		return EPass;	
		}
		
	return EFail;
	}

CPolicyServer::TCustomResult CScrServer::CheckSoftwareNameMatchingEnvironmentL(const RMessage2& aMsg)
	{
	HBufC* softwareTypeName = CScrRequestImpl::GetSoftwareTypeNameFromMsgLC(aMsg);
	TSecureId clientSid = aMsg.SecureId();
	
	TBool vaildSid = EFalse;
    RArray<TSecureId> installerSids;
    CleanupClosePushL(installerSids);
    if (iRequestImpl->GetSidsForSoftwareTypeL(softwareTypeName, installerSids))
        {
        TInt count = installerSids.Count();
        for (TInt i = 0; i < count; i++)
            {
            if (clientSid == installerSids[i])
                {
                vaildSid = ETrue;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy(2, softwareTypeName);
	
	if (vaildSid)	
		return EPass;	

	DEBUG_PRINTF(_L("Client Sid is not a valid one software type!"));	
	return EFail;
	}

CPolicyServer::TCustomResult CScrServer::CheckAllowedFilePathL(const RMessage2& aMsg)
	{
	HBufC* fileName = CScrRequestImpl::ReadFileNameFromMsgLC(aMsg);
	
	// Retrieve the required capabilities for write access to this path
	TCapabilitySet requiredCapabilities = SecCommonUtils::FileModificationRequiredCapabilitiesL(*fileName, aMsg.SecureId());
	
	TBool result = EFalse;
	TBool allFilesRequired = requiredCapabilities.HasCapability(ECapabilityAllFiles);
	TBool tcbRequired = requiredCapabilities.HasCapability(ECapabilityTCB);
	
	// Test whether the client has at least one of the required capabilities
	if (allFilesRequired)
		result = aMsg.HasCapability(ECapabilityAllFiles);
	if (!result && tcbRequired)
		result = aMsg.HasCapability(ECapabilityTCB);
	if (!allFilesRequired && !tcbRequired)
		result = ETrue;
	
	CleanupStack::PopAndDestroy(fileName);
	return result ? EPass : EFail;
	}

CPolicyServer::TCustomResult CScrServer::CheckClientIsInstallerL(const RMessage2& aMsg)
	{
	TSecureId clientSid = aMsg.SecureId();
	DEBUG_PRINTF2(_L("The client SID is 0x%X"), clientSid.iId);
	TBool isInstallerSid = iRequestImpl->IsInstallerOrExecutionEnvSidL(clientSid);
	return isInstallerSid ? EPass : EFail;
	}

CPolicyServer::TCustomResult CScrServer::CheckCommonComponentPropertySettableL(const RMessage2& aMsg, TCapability aRequiredCapability)
	{
	// Setting a common component property is allowed either for the owning installer or for a process with corresponding capability
	if (CheckComponentIdMatchingEnvironmentL(aMsg) == EPass)
		return EPass;
	return aMsg.HasCapability(aRequiredCapability)? EPass : EFail;
	}

CPolicyServer::TCustomResult CScrServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{	
	// SCS framework adds its own bitmask to the function id - we need to strip it in order to detect the actual function invoked
	TInt functionId = StripScsFunctionMask(aMsg.Function());

	switch (functionId)
		{
		case ECreateTransaction:
		case ERollbackTransaction:
		case ECommitTransaction:
			return CheckClientIsInstallerL(aMsg);
		case EAddComponent:
		case EAddComponentDependency:
		case EDeleteComponentDependency:
			return CheckSoftwareNameMatchingEnvironmentL(aMsg);
		case ERegisterComponentFile:
		case ESetFileStringProperty:
		case ESetFileNumericProperty:
		case EDeleteFileProperty:
		case EUnregisterComponentFile:
			// For file registration we check that the client is actually allowed to modify files under this location
			// We do not check for the installer SID, since if the client can write to this location, then registering this location for other components
			// does not pose additional risk			
			return CheckAllowedFilePathL(aMsg);	
		case ESetComponentLocalizableProperty:
		case ESetComponentNumericProperty:
		case ESetComponentBinaryProperty:
		case ESetComponentName:
		case ESetComponentVendor:
		case ESetComponentVersion:
		case ESetComponentSize:
		case EDeleteComponentProperty:
		case ESetScomoState:
		case ESetIsComponentPresent:
			return CheckComponentIdMatchingEnvironmentL(aMsg);
		case EDeleteComponent:			
			return CheckDeleteComponentAllowedL(aMsg);
		case ESetIsComponentRemovable:
			return CheckCommonComponentPropertySettableL(aMsg, ECapabilityAllFiles);
		case ESetIsComponentDrmProtected:
		case ESetIsComponentHidden:
		case ESetIsComponentKnownRevoked:
		case ESetIsComponentOriginVerified:		
			return CheckCommonComponentPropertySettableL(aMsg, ECapabilityWriteDeviceData);
		case EAddApplicationEntry:
		case EDeleteApplicationEntries:
			return CheckComponentIdMatchingEnvironmentL(aMsg);
		case EDeleteApplicationEntry:
		    return CheckComponentIdMatchingEnvironmentL(aMsg, ETrue);
		default:
			DEBUG_PRINTF2(_L("Unknown function was invoked in CustomSecurityCheck - %d"), functionId);							
			__ASSERT_DEBUG(0, User::Invariant());
			return EFail;
		}
	}

void CScrServer::ReconnectL()
	{
	DeleteObjectZ(iRequestImpl);	
	iRequestImpl = CScrRequestImpl::NewL(iFs, iDatabaseFile, iJournalFile);	
	}

void CScrServer::AddSubsessionOwnerL(CScrSession* aSession)
	{
	iSubsessionOwners.InsertInAddressOrderAllowRepeatsL(aSession);
	}

void CScrServer::RemoveSubsessionOwner(CScrSession* aSession)
	{
	TInt index = iSubsessionOwners.FindInAddressOrder(aSession);
	if(KErrNotFound != index)
		iSubsessionOwners.Remove(index);
	}

TBool CScrServer::IsTheOnlySubsessionOwner(CScrSession* aSession)
	{
	TInt firstElementIdx = 0;
	TInt lastElementIdx = iSubsessionOwners.Count();
	// if both first element and the last element in the array is the provided session
	// it means that all active subsessions is owned by this session. 
	if(firstElementIdx == iSubsessionOwners.SpecificFindInAddressOrder(aSession, EArrayFindMode_First) &&
	   lastElementIdx == iSubsessionOwners.SpecificFindInAddressOrder(aSession, EArrayFindMode_Last))
		{
		return ETrue;
		}
	return EFalse; 
	}

TInt CScrServer::SubsessionCount()
	{
	return iSubsessionOwners.Count();
	}

void CScrServer::SetTransactionOwner(CScrSession* aSession)
	{
	iTransactionOwningSession = aSession;
	}


TBool CScrServer::IsTransactionInProgress()
	{
	if(iTransactionOwningSession)
		return ETrue;
	else 
		return EFalse;
	}


TBool CScrServer::IsTransactionOwner(CScrSession* aSession)
	{	
	__ASSERT_ALWAYS(aSession, User::Invariant());
	if(aSession == iTransactionOwningSession) 
		return ETrue;
	else 
		return EFalse;
	}

void CScrServer::DoPreHeapMarkOrCheckL()
/**
   This function is called by the framework just before settingchecking a heap mark. We need to compress/free memory
   down to a state which should be the same both before and after the test operations.
*/
{
#ifdef _DEBUG
iSubsessionOwners.Compress();
DeleteObjectZ(iRequestImpl);
#endif
}

void CScrServer::DoPostHeapMarkOrCheckL()
/**
   This function is called by the framework just before settingchecking a heap mark. We need to compress/free memory
   down to a state which should be the same both before and after the test operations.
*/
{
#ifdef _DEBUG
this->ReconnectL();
#endif
}
