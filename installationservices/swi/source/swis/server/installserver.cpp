/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SWIS implementation
*
*/


/**
 @file
*/

#include "installserver.h"
#include "installclientserver.h"
#include "installmachine.h"
#include "uninstallmachine.h"
#include "swispubsubdefs.h"
#include "restoremachine.h"
#include "log.h"
#include "packageremover.h"

#include <connect/sbdefs.h>
#include <swi/pkgremover.h>

namespace Swi
{

/**
 * Local function to panic an offending client
 * @internalTechnology
 * @released
 */
static void PanicClient(const RMessagePtr2& aMessage,
	TInstallServerPanic aPanic)
	{
	aMessage.Panic(KInstallServerName, aPanic);
	}

//
// CInstallSession
//

// 2nd phase construction, called by the server framework
void CInstallSession::CreateL()
	{
	Server().AddSession();

#ifdef __WINSCW__
	// For 2 minutes after initial boot, DLLs are not unloaded. If we are doing an
	// install/uninstall/restore, we need to make sure any pending unloadeds are actioned,
	// otherwise a previously loaded DLL could cause the rollback to fail on windows (on
	// arm it is legal to delete a loaded DLL/EXE, whilst on windows it is not).
	RLoader loader;
	TInt r = loader.Connect();
	if(r == KErrNone)
		{
		(void)loader.CancelLazyDllUnload();
		loader.Close();
		}
#endif
	}

CInstallSession::~CInstallSession()
	{
	delete iMachine;
	delete iRestoreMachine;
	delete iRemover;
	Server().ListRemoveInProgress(EFalse);
	RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisNone);
	Server().DropSession();
	}

CInstallServer& CInstallSession::Server()
	{
	return *static_cast<CInstallServer*>(const_cast<CServer2*>(
		CSession2::Server()));
	}

// Handle a client request. Leaving is handled by ServiceError() which reports 
// the error code to the client
void CInstallSession::ServiceL(const RMessage2& aMessage)
	{
	// Handle the cancel message first
	if (aMessage.Function()==ECancel)
		{
		DEBUG_PRINTF(_L8("CInstallSession::ServiceL() cancel function called"));
		// Set status as aborted
		TInt property;
		if (RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property) == KErrNone)
			{
			RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, property | ESwisStatusAborted);
			}	

		if (iMachine)
			{
			iMachine->CancelInstallation();
			}	
		
		aMessage.Complete(KErrNone);
		return;
		}
	
	TInt property = 0;
	TInt err = RProperty::Get(KUidSystemCategory, conn::KUidBackupRestoreKey, property);

	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	if (iMachine != NULL)
		{
		DEBUG_PRINTF2(_L8("Install Server Message %d refused. Install/Uninstall in progress"), aMessage.Function());
		aMessage.Complete(KErrInUse);
		}

	/*
	 * We must return KErrInUse if:
	 * 1) The Connect P&S property indicates we are doing a backup/restore
	 * 2) We are not servicing that restore operation with this request
	 * 3) A list/remove operation is in progress
	 *
	 */
	else if ((iRemover != NULL) || Server().IsListRemoveInProgress())
		{
		// Remove operation in progress
		DEBUG_PRINTF2(_L8("Install Server Message '%d' refused. List/Remove in progress"), aMessage.Function());
		aMessage.Complete(KErrInUse);
		}
	else if (err != KErrNotFound && (property & (conn::KBURPartTypeMask^conn::EBURNormal)) 
				&& aMessage.Function() != ERestore && !iRestoreMachine)
		{
		DEBUG_PRINTF2(_L8("Install Server Message '%d' refused. Backup/Restore in progress"), aMessage.Function());
		aMessage.Complete(KErrInUse);
		}
	else
		{
		switch (aMessage.Function())
			{
			case EInstall:
				{
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() install function called"));
				
				err = RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisInstall);
				User::LeaveIfError(err);
				
				iMachine=CInstallMachine::NewL(aMessage);
				User::LeaveIfError(iMachine->Start());
				// The state machine will complete the client once the work 
				// is done.
				break;
				}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			case EGetComponentInfo:
				{
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() get component info function called"));
							
				iMachine=CInstallMachine::NewL(aMessage, ETrue);
				// Set the install machine to run on comp info collection mode.
				User::LeaveIfError(iMachine->Start());
				// The state machine will complete the client once the work 
				// is done.
				break;
				}			
			#endif
			case EUninstall:
				{
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() uninstall function called"));
				
				err = RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisUninstall);
				User::LeaveIfError(err);

				iMachine=CUninstallMachine::NewL(aMessage);
				User::LeaveIfError(iMachine->Start());
	
				// The state machine will complete the client once the work 
				// is done.
				break;
				}
				
			case ERestore:
				{
				
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() restore function called"));
				
				err = RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisRestore);
				User::LeaveIfError(err);
				
				if (iRestoreMachine && !iRestoreMachine->IsComplete())
					{
					
					aMessage.Complete(KErrInUse);
					
					}
					
				if (iRestoreMachine)
					{
					
					delete iRestoreMachine;
					iRestoreMachine = NULL;
					
					}
				
				iRestoreMachine = CRestoreMachine::NewL(aMessage);
				break;
				
				}
			case ERestoreFile:
				{
				
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() restore file function called"));
				
				if (iRestoreMachine == NULL)
					{
					
					// Should never be reached
					aMessage.Panic(KInstallServerName, KErrNotReady);
					
					}
				else
					{
					
					iRestoreMachine->ServiceFileRequestL(aMessage);
					
					}
				break;
				
				}
			case ERestoreCommit:
				{
				
				DEBUG_PRINTF(_L8("CInstallSession::ServiceL() commit function called"));
				
				if (iRestoreMachine == NULL)
					{
					
					// Should never be reached
					aMessage.Panic(KInstallServerName, KErrNotReady);
					
					}
				else
					{
					
					iRestoreMachine->ServiceCommitRequestL(aMessage);
					
					}
				break;
				
				}
			case EListUnInstalledPkgs:
				{
				Server().ListRemoveInProgress(ETrue);

				iRemover = CPackageRemover::NewL(aMessage);
				iRemover->ListL();
				// The state will complete the client once the work 
				// is done.
				break;	
				}
			case ERemoveUnInstalledPkg:
				{
				Server().ListRemoveInProgress(ETrue);

				iRemover = CPackageRemover::NewL(aMessage);
				iRemover->RemoveL();
				// The state will complete the client once the work 
				// is done.
				break;	
				}
			default:
				{
				DEBUG_PRINTF2(_L8("CInstallSession::ServiceL() unknown function called (%d)"), aMessage.Function());
				PanicClient(aMessage, Swi::EPanicInstallServerIllegalFunction);
				break;
				}
			}
		}
	}

// Handle an error from ServiceL(). A bad descriptor error implies a badly 
// programmed client, so panic it. Otherwise use the default handling (report 
// the error to the client).
void CInstallSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	
	// set the pubsub flag to aborted status
	TInt property = 0;
	TInt err = RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property);
	
	if (err == KErrNone)
		{
		RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, property | ESwisStatusAborted);
		}
	
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage, EPanicInstallServerBadDescriptor);
	CSession2::ServiceError(aMessage, aError);
	}

//
// CInstallServerShutdown
//

inline CInstallServerShutdown::CInstallServerShutdown()
:	CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

CInstallServerShutdown::~CInstallServerShutdown()
	{
	Cancel();
	}

inline void CInstallServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CInstallServerShutdown::Start()
	{
	After(KShutdownDelay);
	}

// Initiate server exit when the timer expires
void CInstallServerShutdown::RunL()
	{
    DeleteTempRscFilesL();
	CActiveScheduler::Stop();
	}

void CInstallServerShutdown::DeleteTempRscFilesL()
    {
    DEBUG_PRINTF(_L8("Deleting temporary application registration rsource files"));
    TChar systemDrive = RFs::GetSystemDriveChar();
    TFileName tempResourceFileLocation; 
    _LIT(KTempRscFileLocation, "%c:\\resource\\install\\temp\\");
    tempResourceFileLocation.Format(KTempRscFileLocation, TUint(systemDrive));
    RFs fileSession;    
    User::LeaveIfError(fileSession.Connect());
    CleanupClosePushL(fileSession);   
    CFileMan* fileManager = NULL;
    TRAPD(err, fileManager = CFileMan::NewL(fileSession))
    if (KErrNone != err)
        {
        DEBUG_PRINTF2(_L8("Can not delete temp app rsc files %d"), err);
        if(NULL != fileManager)
            delete fileManager;
        CleanupStack::Pop(&fileSession);
        fileSession.Close();  
        return;
        }        
    
    err = fileManager->RmDir(tempResourceFileLocation);
    if (KErrNone != err)
        DEBUG_PRINTF2(_L8("Deleting temp app rsc file resulted in an error %d"), err);
    
    delete fileManager;
    CleanupStack::Pop(&fileSession);
    fileSession.Close();    
    }

//
// CInstallServer
//

// All functions require TrustedUI capability
const TInt CInstallServer::iRanges[iRangeCount] = 
                {
                0 // All connect attempts
                };

const TUint8 CInstallServer::iElementsIndex[iRangeCount] = 
                {
                CPolicyServer::ECustomCheck // Custom check polices for TrustedUI or SID from connect.
                };

const CPolicyServer::TPolicy CInstallServer::iPolicy =
                {
                CPolicyServer::ECustomCheck,					// specifies all connect attempts
                iRangeCount,
                iRanges,
                iElementsIndex,
                NULL,
                };

CInstallServer* CInstallServer::NewLC()
	{
	CInstallServer* self = new(ELeave) CInstallServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CInstallServer::~CInstallServer()
	{
	DEBUG_PRINTF(_L8("Install Server Shutdown"));
	CSecurityPolicy::ReleaseResource();
	delete iShutdown;
	iShutdown = NULL; //required in case the server dies before the session
	}

// This server does not offer sharable sessions
CInstallServer::CInstallServer()
: 	CPolicyServer(EPriorityMore, iPolicy, EUnsharableSessions),
	iListRemoveInProgress(EFalse)
	{
	}

// Ensure the timer and server objects are running
void CInstallServer::ConstructL()
	{
	DEBUG_PRINTF(_L8("Install Server Created"));
	StartL(KInstallServerName);
	iShutdown = new(ELeave) CInstallServerShutdown;
	iShutdown->ConstructL();
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Create the property used to publish SWI Progress Bar Value. Since this property is not needed
	// if the SWI component doesn't exist, it is created here.
	TInt err = RProperty::Define(KUidInstallServerCategory, KUidSwiProgressBarValueKey, RProperty::EInt);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		DEBUG_PRINTF2(_L8("Progress Bar Property couldn't be created. Error=%d."), err);
		User::Leave(err);
		}
#endif	
	}
	
	
CPolicyServer::TCustomResult CInstallServer::CustomSecurityCheckL(const RMessage2& aMsg, 
	TInt& /*aAction*/, TSecurityInfo& aMissing)
	{
	
	_LIT_SECURE_ID(secureBackupClientSID, 0x10202D56);
	
	if (aMsg.SecureId().iId == secureBackupClientSID.iId)
		{
		return CPolicyServer::EPass;
		}
	else if (aMsg.HasCapability(ECapabilityTrustedUI, 
		__PLATSEC_DIAGNOSTIC_STRING("CInstallServer::CustomSecurityTest client process does not have Trusted UI Capability")))
		{
		// Check additional capability for listing and removing "uninstalled" packages
		if ((aMsg.Function() == EListUnInstalledPkgs) && !aMsg.HasCapability(ECapabilityReadDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("CInstallServer::CustomSecurityTest client process does not have ReadDeviceData Capability")))
			{
			aMissing.iCaps.Set(ECapabilityReadDeviceData);
			return CPolicyServer::EFail;
			}

		if ((aMsg.Function() == ERemoveUnInstalledPkg) && !aMsg.HasCapability(ECapabilityWriteDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("CInstallServer::CustomSecurityTest client process does not have WriteDeviceData Capability")))
			{
			aMissing.iCaps.Set(ECapabilityWriteDeviceData);
			return CPolicyServer::EFail;
			}

		return CPolicyServer::EPass;
		}
	else
		{
		// Missing connect SID or TrustedUI capability, cannot complete connection
		aMissing.iCaps.Set(ECapabilityTrustedUI);
		return CPolicyServer::EFail;
		}
	}

CSession2* CInstallServer::NewSessionL(const TVersion& aClientVersion, 
	const RMessage2&) const
	{
 
	// Currently there is only one version of the InstallServer		
	const TVersion serverVersion(KInstallServerVersionMajor,KInstallServerVersionMinor,KInstallServerVersionBuild);  

	// Check that the version of the server requested is not greater than 
	// that available. 	
	if (!User::QueryVersionSupported(serverVersion, aClientVersion))
		{
		User::Leave(KErrNotSupported);
		}
		
	return new(ELeave) CInstallSession();	
	}

void CInstallServer::AddSession()
	{
	++iSessionCount;
	DEBUG_PRINTF2(_L8("Install Server Session Created (%d sessions active)"), iSessionCount);
	iShutdown->Cancel();
	}

void CInstallServer::DropSession()
	{
	// if last session is dropped and server still alive start shutdown timer
	if (--iSessionCount==0 && iShutdown)
		{
		iShutdown->Start();
		DEBUG_PRINTF(_L8("Install Server Starting Shutdown Timer"));
		}
	DEBUG_PRINTF2(_L8("Install Server Session Destroyed (%d sessions active)"), iSessionCount);
	}

} // namespace Swi
