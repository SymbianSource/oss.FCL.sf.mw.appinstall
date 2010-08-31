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
* CSisRegistryServer implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>

#include "sisregistryserver.h"
#include "sisregistrycache.h"
#include "sisregistryservershutdown.h"
#include "sisregistryserversession.h"
#include "log.h"
#include "versionutil.h"
#include "securitypolicy.h"

using namespace Swi;

const TInt CSisRegistryServer::iRanges[KPolicyRanges] = 
	{
	0, 								// accessible by all clients
	EAddDrive,						// Only Daemon should access these 
	EControllers,					// accessible by all clients
	EInitRecovery,					// Only Daemon should access these 
	EPackageExistsInRom,		    // accessible by all clients
	ESeparatorMinimumReadUserData, 	// accessible by ReadUserData clients only
	ESidToPackage,          			// accessible by all clients
	ESeparatorMinimumWriteUserData, // accessible by NetworkServices + WriteUserData clients only
	ESeparatorMinimumTCB, 			// accessible by TCB clients only
	ERegistryFiles,                    // accessible by all clients
	ESeparatorMinimumReadUserData2, 	// accessible by ReadUserData clients only
	ESeparatorEndAll,     			// not supported from there on
	};

const TUint8 CSisRegistryServer::iElementsIndex[iRangeCount] = 
	{
	0,  // all clients can have access 
	3,  // policed on SID only
	0,  // all clients can have access                        
	3,  // policed on SID only
	0,  // all clients can have access                        
	1,	// ReadUserData clients only
	0,  // all clients can have access	
	4,  // WriteUserData clients only
	2,	// Used by SWIS only, so TCB is needed - check on cap and SID.
	0,  // all clients can have access
	1,  // ReadUserData clients only
	CPolicyServer::ENotSupported,  
	};

const CPolicyServer::TPolicyElement CSisRegistryServer::iPolicyElements[] = 
	{
	{_INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S1(KSwisSecureId, ECapabilityTCB), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S0(KDaemonSecureId), CPolicyServer::EFailClient}, 
	{_INIT_SECURITY_POLICY_C2(ECapabilityWriteUserData, ECapabilityNetworkServices), CPolicyServer::EFailClient},
	};

const CPolicyServer::TPolicy CSisRegistryServer::iPolicy =
	{
	CPolicyServer::EAlwaysPass, // so that any client can connect	
	iRangeCount,                                   
	iRanges,
	iElementsIndex,
	iPolicyElements,
	};
 
inline CSisRegistryServer::CSisRegistryServer(CActive::TPriority aPriority)
	:CPolicyServer(aPriority, iPolicy, ESharableSessions)
	{}
	
CSisRegistryServer* CSisRegistryServer::NewLC(CActive::TPriority aPriority)
	{
	CSisRegistryServer* self = new(ELeave) CSisRegistryServer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSisRegistryServer::ConstructL()
//
// 2nd phase construction - ensure the timer and server objects are running
//
	{
	DEBUG_PRINTF(_L8("SIS Registry Server - Starting Server"));
	StartL(KSisRegistryName);
	// create the cache
	iCache = CSisRegistryCache::NewL();
	// create the shutdown 
	iShutdown = new(ELeave) CShutdown;
	iShutdown->ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown->Start();
	}
	
CSisRegistryServer::~CSisRegistryServer()
	{
	DEBUG_PRINTF(_L8("SIS Registry Server - Server Shutting Down"));
	CSecurityPolicy::ReleaseResource();
	delete iCache;
	iCache = NULL;
	delete iShutdown;
	iShutdown = NULL; //required in case the server dies before the session
	}

CSession2* CSisRegistryServer::NewSessionL(const TVersion& aClientVersion, const RMessage2&) const
//
// Create a new client session. This should really check the version number.
//
	{
	if (aClientVersion == TVersion(1,0,0))
		{
		return new(ELeave) CSisRegistrySession();	
		}
	else
		{
		User::Leave(KErrCouldNotConnect);
		return NULL;
		}
	}

void CSisRegistryServer::AddSession()
//
// A new session is being created
// Cancel the shutdown timer if it was running
//
	{
	++iSessionCount;
	DEBUG_PRINTF2(_L8("SIS Registry Server - New Session added (%d active)"), iSessionCount);
	iShutdown->Cancel();
	}

void CSisRegistryServer::DropSession()
//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
	{
	if (--iSessionCount == 0 && iShutdown)
		{
		iShutdown->Start();
		}
	DEBUG_PRINTF2(_L8("SIS Registry Server - Session close (%d active)"), iSessionCount);
	}



