/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

  

#include "backuprestore.h"
#include "installclientserver.h"
#include "log.h"

namespace Swi 
	{	
	EXPORT_C TInt RRestoreSession::Connect()
		{
		DEBUG_PRINTF(_L8("RRestoreSession::Connect"));
		iSession = new RInstallServerSession;
		if (iSession == NULL)
			{
			return KErrNoMemory;
			}			
		return iSession->Connect();		
		}
		
	EXPORT_C void RRestoreSession::StartPackageL(TUid aPackageUid, const HBufC8& aPackageMetadata)
		{
		DEBUG_PRINTF2(_L8("Start PackageL 0x%08x"), aPackageUid);
		TPckgC<TUid> pkg(aPackageUid);
		User::LeaveIfError(iSession->Restore(TIpcArgs(&pkg, &aPackageMetadata)));		
		}
		
	EXPORT_C void RRestoreSession::RestoreFileL(RFile& aFile, TDesC& aDestinationFileName)
		{	
		TIpcArgs args;
		User::LeaveIfError(aFile.TransferToServer(args, EMessageSlotRestoreFs, EMessageSlotRestoreFile));
		args.Set(EMessageSlotRestorePath, &aDestinationFileName);
		
		User::LeaveIfError(iSession->RestoreFile(args));		
		}
		
	EXPORT_C void RRestoreSession::CommitPackageL()
		{
		DEBUG_PRINTF(_L8("RRestoreSession::CommitPackageL"));
		User::LeaveIfError(iSession->RestoreCommit());		
		}
		
	EXPORT_C void RRestoreSession::Close()
		{
		DEBUG_PRINTF(_L8("RRestoreSession::Close"));
		if (iSession != NULL)
			{
			iSession->Close();
			delete iSession;
			iSession = NULL;
			}		
		}
	}
