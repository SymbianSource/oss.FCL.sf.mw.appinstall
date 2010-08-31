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
*
*/


#include "scrhelpercommon.h"
#include "scrhelperserver.h"
#include <e32cmn.h>
#include <e32debug.h>
#include <usif/scr/scrcommon.h>

using namespace Usif;

static const TUint scrHelperRangeCount = 2;

static const TInt scrHelperRanges[scrHelperRangeCount] =
	{
	0,							 // Range-0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	 // Range-1 - EBaseSession to KMaxTInt inclusive.
	};

static const TUint8 scrHelperElementsIndex[scrHelperRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 is not supported.
	0,							  // Range 1 must come from the SCR Server.
	};							

static const CPolicyServer::TPolicyElement scrHelperElements[] =
	{
	{_INIT_SECURITY_POLICY_S0(KUidScrServer.iUid), CPolicyServer::EFailClient}
	};

static const CPolicyServer::TPolicy scrHelperPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connections
	scrHelperRangeCount,
	scrHelperRanges,
	scrHelperElementsIndex,
	scrHelperElements,
	};

/////////////////////
// CScrHelperServer
/////////////////////

CScrHelperServer::CScrHelperServer()
	:	CScsServer(ScrHelperServerVersion(), scrHelperPolicy)
	{
	// empty
	}

CScrHelperServer::~CScrHelperServer()
	{
	iFs.Close();
	}

CScrHelperServer* CScrHelperServer::NewLC()
	{
	CScrHelperServer* self = new(ELeave) CScrHelperServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CScrHelperServer::ConstructL() 
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());

	StartL(KScrHelperServerName);
	CScsServer::ConstructL(KScrHelperServerShutdownPeriod);	
	}

CScsSession* CScrHelperServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CScsServer by allocating a new instance of CScrHelperSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of the SCR Helper session class which is owned by the caller.
 */
	{
	DEBUG_PRINTF(_L8("SCR Helper session creation!"));
	return CScrHelperSession::NewL(*this, aMessage);
	}

inline RFs& CScrHelperServer::FileServer()
	{
	return iFs;
	}

/////////////////////
// CScrHelperSession
/////////////////////

CScrHelperSession::CScrHelperSession(CScrHelperServer& aServer)
	:	CScsSession(aServer)
	{
	// empty
	}

CScrHelperSession::~CScrHelperSession()
	{
	// empty
	}

CScrHelperSession* CScrHelperSession::NewL(CScrHelperServer &aServer, const RMessage2& aMessage)
/**
	Factory function allocates new instance of CScrSession.
	
	@param aServer  SCR Helper Server object.
	@param aMessage	Standard server-side handle to message. Not used.
	@return			Newly created instance of CScrHelperSession which is owned by the caller.
 */
	{
	(void)aMessage; // Make the compiler happy in release mode
	CScrHelperSession* self = new (ELeave) CScrHelperSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CScrHelperSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
	{
	TScrHelperServerMessages f = static_cast<TScrHelperServerMessages>(aFunction);
	RFs& fs =  static_cast<CScrHelperServer*>(&iServer)->FileServer();
	
	switch (f)
		{
		case EGetDatabaseFileHandle:
			GetFileHandleL(fs, aMessage, KScrDatabaseFilePath);
			break;
		case EGetJournalFileHandle:
			GetFileHandleL(fs, aMessage, KScrJournalFilePath);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	//RMessage2 object is closed by both TransferToClient() and SCS framework.
	//return EFalse to prevent SCS to close the message object.
	return EFalse;
	}

void CopyDbFromROMToSystemL(RFs& aFs, const TDesC& aTargetPath)
	{
	CFileMan* fileManager = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileManager);
	User::LeaveIfError(fileManager->Copy(KScrDbRomPath, aTargetPath, 0));

	// Reset the read-only attribute on the copied file
	User::LeaveIfError(aFs.SetAtt(aTargetPath, 0, KEntryAttReadOnly));
	CleanupStack::PopAndDestroy(fileManager);
	}

void CScrHelperSession::GetFileHandleL(RFs& aFs, const RMessage2& aMessage, const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("Returning file handle of %S."), &aFileName);
	RBuf filePath;
	filePath.CreateL(aFileName.Length());
	filePath.CleanupClosePushL();
	filePath.Copy(aFileName);
	filePath[0] = aFs.GetSystemDriveChar();
	TInt err = aFs.MkDirAll(filePath);
	if(KErrNone != err && KErrAlreadyExists != err)
		{
		DEBUG_PRINTF3(_L("An error (%d) occured while making all directories of %S."), err, &filePath);
		User::Leave(err);
		}
	
	RFile file;
	TEntry entry;

	if(KErrNone == aFs.Entry(filePath, entry))
		{ // The file exists, just open it.
		User::LeaveIfError(file.Open(aFs, filePath, EFileShareAny|EFileWrite));
		}    
	else
		{ // The file doesn't exist. First, check if the requested file is database or journal file.
		if(KScrDatabaseFilePath() == aFileName)
			{ // This is the database file. Copy the default one into the requested location.
			DEBUG_PRINTF(_L8("SCR database file doesn't exist. It is being copied from ROM"));
			CopyDbFromROMToSystemL(aFs, filePath);
			 // Then, open the file.
			User::LeaveIfError(file.Open(aFs, filePath, EFileShareAny|EFileWrite));
			}
		else
			{ // This is the journal file, simply create an empty file.
			User::LeaveIfError(file.Create(aFs, filePath, EFileShareAny|EFileWrite));
			}
		}
	CleanupStack::PopAndDestroy(&filePath);
	CleanupClosePushL(file);
	
	// Store the RFile handle into the package buffer in slot 0 and complete the message with the RFs handle
	User::LeaveIfError(file.TransferToClient(aMessage, 0));
	ASSERT(aMessage.IsNull());  // The message should have been completed

	CleanupStack::PopAndDestroy(&file);
	}
