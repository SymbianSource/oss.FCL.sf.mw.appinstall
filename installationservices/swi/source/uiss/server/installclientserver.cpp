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
* Utility class implementation for SWIS
*
*/


#include <e32std.h>
#include "installclientserver.h"
#include "sisregistrypackage.h"

//
// Starts SWIS in a new process
//
static TInt StartSwis()
	{
	const TUidType serverUid(KNullUid, KNullUid, Swi::KInstallServerUid3);
	RProcess server;

	TInt err=server.Create(Swi::KInstallServerImage, KNullDesC, serverUid);
	if (err!=KErrNone)
		return err;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0); // abort startup
	else
		server.Resume(); // logon OK, start the server
	User::WaitForRequest(stat); // wait for start or death
	// We can't use the 'exit reason' if the server panicked as this is the
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone.
	err=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}
 
EXPORT_C TInt Swi::RInstallServerSession::Connect()
	{
	TInt retry=2;
	for(;;)
		{
		TInt err=CreateSession(Swi::KInstallServerName, 
		    TVersion(KInstallServerVersionMajor,KInstallServerVersionMinor,KInstallServerVersionBuild));
		if (err!=KErrNotFound && err!=KErrServerTerminated)
			return err;
		if (--retry==0)
			return err;
		err=StartSwis();
		if (err!=KErrNone && err!=KErrAlreadyExists)
			return err;
		}
	}

void Swi::RInstallServerSession::InstallL(const TIpcArgs& aArgs,
	TRequestStatus& aStatus)
	{
	SendReceive(Swi::EInstall, aArgs, aStatus);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
void Swi::RInstallServerSession::GetComponentInfoL(const TIpcArgs& aArgs,
	TRequestStatus& aStatus)
	{	
	SendReceive(Swi::EGetComponentInfo, aArgs, aStatus);
	}
#endif

void Swi::RInstallServerSession::Uninstall(TIpcArgs& args, TRequestStatus& aStatus)
	{
	SendReceive(Swi::EUninstall, args, aStatus);
	}

EXPORT_C TInt Swi::RInstallServerSession::Cancel()
	{
	return Send(Swi::ECancel);
	}

EXPORT_C TInt Swi::RInstallServerSession::Restore(const TIpcArgs& aArgs)
	{
	return SendReceive(Swi::ERestore, aArgs);
	}

EXPORT_C TInt Swi::RInstallServerSession::RestoreFile(const TIpcArgs& aArgs)
	{
	return SendReceive(Swi::ERestoreFile, aArgs);
	}

EXPORT_C TInt Swi::RInstallServerSession::RestoreCommit()
	{
	return SendReceive(Swi::ERestoreCommit);
	}
