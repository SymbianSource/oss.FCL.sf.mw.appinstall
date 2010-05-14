/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32cmn.h>
#include <e32debug.h>
#include <bautils.h>
#include <s32mem.h>
#include "sisregistryhelperservercommon.h"
#include "sisregistryhelperserver.h"
#include "sisregistryclientserver.h"

using namespace Swi;

static const TUint sisRegistryHelperRangeCount = 2;

static const TInt sisRegistryHelperRanges[sisRegistryHelperRangeCount] =
	{
	0,							 // Range-0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	 // Range-1 - EBaseSession to KMaxTInt inclusive.
	};

static const TUint8 sisRegistryHelperElementsIndex[sisRegistryHelperRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 is not supported.
	0,							  // Range 1 must come from the SIS Registry.
	};							

static const CPolicyServer::TPolicyElement sisRegistryHelperElements[] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient}
	};

static const CPolicyServer::TPolicy sisRegistryHelperPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connections
	sisRegistryHelperRangeCount,
	sisRegistryHelperRanges,
	sisRegistryHelperElementsIndex,
	sisRegistryHelperElements,
	};

/////////////////////
// CSisRegistryHelperServer
/////////////////////

CSisRegistryHelperServer::CSisRegistryHelperServer()
	:	CScsServer(SisRegistryHelperServerVersion(), sisRegistryHelperPolicy)
	{
	// empty
	}

CSisRegistryHelperServer::~CSisRegistryHelperServer()
	{
	}

CSisRegistryHelperServer* CSisRegistryHelperServer::NewLC()
	{
	CSisRegistryHelperServer* self = new(ELeave) CSisRegistryHelperServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSisRegistryHelperServer::ConstructL() 
	{
	StartL(KSisRegistryHelperServerName);
	CScsServer::ConstructL(KSisRegistryHelperServerShutdownPeriod);	
	}

CScsSession* CSisRegistryHelperServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CScsServer by allocating a new instance of CScrHelperSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of the SISRegistry Helper session class which is owned by the caller.
 */
	{
	return CSisRegistryHelperSession::NewL(*this, aMessage);
	}


/////////////////////
// CScrHelperSession
/////////////////////

CSisRegistryHelperSession::CSisRegistryHelperSession(CSisRegistryHelperServer& aServer)
	:	CScsSession(aServer)
	{
	// empty
	}

CSisRegistryHelperSession::~CSisRegistryHelperSession()
	{
	// empty
	}

CSisRegistryHelperSession* CSisRegistryHelperSession::NewL(CSisRegistryHelperServer &aServer, const RMessage2& aMessage)
/**
	Factory function allocates new instance of CSISRegistryHelperSession.
	
	@param aServer  SIS Registry Helper Server object.
	@param aMessage	Standard server-side handle to message. Not used.
	@return			Newly created instance of CSISRegistryHelperSession which is owned by the caller.
 */
	{
	(void)aMessage; // Make the compiler happy in release mode
	CSisRegistryHelperSession* self = new (ELeave) CSisRegistryHelperSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CSisRegistryHelperSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
	{
	TSisRegistryHelperServerMessages f = static_cast<TSisRegistryHelperServerMessages>(aFunction);
	TInt err = KErrNone;
	
	switch (f)
		{
		case EGetEquivalentLanguages:
			TRAP(err, GetEquivalentLanguagesL(aMessage));
			aMessage.Complete(err);
			break;

		default:
			User::Leave(KErrNotSupported);
		}
	//RMessage2 object is closed by both TransferToClient() and SCS framework.
	//return EFalse to prevent SCS to close the message object.
	return EFalse;
	}

void CSisRegistryHelperSession::GetEquivalentLanguagesL(const RMessage2& aMessage)
	{
	TLanguage srcLangID = (TLanguage)aMessage.Int1();
	
	TLanguagePath equivalentLangs;
	BaflUtils::GetEquivalentLanguageList(srcLangID,equivalentLangs);
	
	// calculate the likely size of the data transfer buffer
	const TInt KMaxBufSize=
		sizeof(TInt)+                 // number of entries
		KMaxEquivalentLanguages*sizeof(TLanguage);  // Languages IDs stored as TLanguage
	
	// allocate buffer for the array
	HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
	
	TInt size = 1;
	while (equivalentLangs[size++] != ELangNone){}
		
	TPtr8 pBuf=buf->Des();
	RDesWriteStream outs(pBuf);
	CleanupClosePushL(outs);

	outs.WriteInt32L(size-2);
	TInt i;
	for (i = 1; i < size-1; ++i)
		{
		outs.WriteInt32L(static_cast<TInt>(equivalentLangs[i]));
		}
	
	outs.CommitL();
	aMessage.WriteL(0, *buf);
	
	CleanupStack::PopAndDestroy(2,buf); //outs,buf
	}
