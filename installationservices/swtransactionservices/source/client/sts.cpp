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
* sts.h
*
*/


/**
 @file
 @publishedAll
 @released
*/
#include <usif/sts/sts.h>
#include "stscommon.h"
#include <e32cmn.h>
#include <e32uid.h>

using namespace Usif;


void RStsSession::ConnectL()
	{
	__ASSERT_DEBUG(iHandle == NULL, User::Invariant());
	TInt err = RScsClientBase::Connect(KStsServerName, TVersion(KStsVerMajor,
			KStsVerMinor, KStsVerBuild), KStsServerImgName, TUidType(
			KExecutableImageUid, KNullUid, TUid::Uid(KUidStsServer)));
	User::LeaveIfError(err);
	}

EXPORT_C RStsSession::RStsSession()
: RScsClientBase()
	{
	// empty
	}

EXPORT_C void RStsSession::Close()
	{
	RScsClientBase::Close();
	}

EXPORT_C TStsTransactionId RStsSession::CreateTransactionL()
	{
	// It is possible to create several transactions on the same session, assuming that they are all sequential
	if (iHandle == NULL)
		ConnectL();
	TStsTransactionId trID;
	TPckg<TStsTransactionId> idPkg(trID);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KIdIPCSlot, &idPkg);
	User::LeaveIfError(CallSessionFunction(ECreateTransaction, ipcArgs));
	return trID;
	}

EXPORT_C void RStsSession::OpenTransactionL(TStsTransactionId aTransactionID)
	{
	if (iHandle == NULL)
		ConnectL();
	TPckgC<TStsTransactionId> idPkg(aTransactionID);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KIdIPCSlot, &idPkg);
	User::LeaveIfError(CallSessionFunction(EOpenTransaction, ipcArgs));
	}

EXPORT_C void RStsSession::RegisterNewL(const TDesC& aFileName)
	{
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	User::LeaveIfError(CallSessionFunction(ERegisterNew, ipcArgs));
	}

EXPORT_C void RStsSession::CreateNewL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode)
	{
	__ASSERT_DEBUG(((KFilePathIPCSlot!=KFileHandleIPCSlot) &&
					(KFilePathIPCSlot!=KFileModeIPCSlot)   &&
					(KFileModeIPCSlot!=KFileHandleIPCSlot)), User::Invariant());

	TPckgBuf<TInt> fileHandlePkg;
	TPckgC<TUint> pkgFileMode(aCreateMode);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	ipcArgs.Set(KFileHandleIPCSlot, &fileHandlePkg);
	ipcArgs.Set(KFileModeIPCSlot, &pkgFileMode);
	TInt fileServerHandle = User::LeaveIfError(CallSessionFunction(ECreateNew, ipcArgs));
	User::LeaveIfError(aFile.AdoptFromServer(fileServerHandle, fileHandlePkg()));
	}

EXPORT_C void RStsSession::RemoveL(const TDesC& aFileName)
	{
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	User::LeaveIfError(CallSessionFunction(ERemove, ipcArgs));
	}

EXPORT_C void RStsSession::RegisterTemporaryL(const TDesC& aFileName)
	{
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	User::LeaveIfError(CallSessionFunction(ERegisterTemporary, ipcArgs));
	}

EXPORT_C void RStsSession::CreateTemporaryL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode)
	{
	__ASSERT_DEBUG(((KFilePathIPCSlot!=KFileHandleIPCSlot) &&
					(KFilePathIPCSlot!=KFileModeIPCSlot)   &&
					(KFileModeIPCSlot!=KFileHandleIPCSlot)), User::Invariant());

	TPckgBuf<TInt> fileHandlePkg;
	TPckgC<TUint> pkgFileMode(aCreateMode);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	ipcArgs.Set(KFileHandleIPCSlot, &fileHandlePkg);
	ipcArgs.Set(KFileModeIPCSlot, &pkgFileMode);
	TInt fileServerHandle = User::LeaveIfError(CallSessionFunction(ECreateTemporary, ipcArgs));
	User::LeaveIfError(aFile.AdoptFromServer(fileServerHandle, fileHandlePkg()));
	}

EXPORT_C void RStsSession::OverwriteL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode)
	{
	__ASSERT_DEBUG(((KFilePathIPCSlot!=KFileHandleIPCSlot) &&
					(KFilePathIPCSlot!=KFileModeIPCSlot)   &&
					(KFileModeIPCSlot!=KFileHandleIPCSlot)), User::Invariant());

	TPckgBuf<TInt> fileHandlePkg;
	TPckgC<TUint> pkgFileMode(aCreateMode);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KFilePathIPCSlot, &aFileName);
	ipcArgs.Set(KFileHandleIPCSlot, &fileHandlePkg);
	ipcArgs.Set(KFileModeIPCSlot, &pkgFileMode);
	TInt fileServerHandle = User::LeaveIfError(CallSessionFunction(EOverwrite, ipcArgs));
	User::LeaveIfError(aFile.AdoptFromServer(fileServerHandle, fileHandlePkg()));
	}

EXPORT_C void RStsSession::CommitL()
	{
	User::LeaveIfError(CallSessionFunction(ECommit));
	Close();
	}

EXPORT_C void RStsSession::RollBackL()
	{
	User::LeaveIfError(CallSessionFunction(ERollBack));
	Close();
	}

EXPORT_C TStsTransactionId RStsSession::TransactionIdL()
	{
	TStsTransactionId id;
	TPckg<TStsTransactionId> idPkg(id);
	TIpcArgs ipcArgs;
	ipcArgs.Set(KIdIPCSlot, &idPkg);
	User::LeaveIfError(CallSessionFunction(EGetId, ipcArgs));
	return id;
	}

