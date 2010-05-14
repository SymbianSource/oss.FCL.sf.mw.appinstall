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
 @internalComponent
 @released
*/


#ifndef __STSCOMMON_H__
#define __STSCOMMON_H__

namespace Usif
	{
	_LIT(KStsServerName, "!SoftwareTransactionServicesServer");
	_LIT(KStsServerImgName, "softwaretransactionservices.exe");
	const TInt KStsVerMajor = 1;		
	const TInt KStsVerMinor = 0;		
	const TInt KStsVerBuild = 0;		
		
    const TInt KUidStsClient         = 0x10285BCE;
    const TInt KUidStsRecoveryClient = 0x10285BD0;
    const TInt KUidStsServer         = 0x10285BCD;
    const TInt KUidDaemon 			 = 0x10202DCE;
    
    enum TStsClientServerFunctionType
		{
		//client to server
		ECreateTransaction,  //creates a new transaction
		EOpenTransaction,    //opens an existing transaction
		ECommit,             //instructs the server to commit the transaction that is currently associated with the session
		ERollBack,           //instructs the server to roll back the transaction that is currently associated with the session
		EGetId,              //instructs the server to retrieve the id of the transaction that is currently associated with the session

		ERegisterNew,        //notifies the server that a new file was added by client
		ECreateNew,          //instructs the server to create a new file and return a handle to it to the client
		ERemove,             //instructs the server to remove a file
		ERegisterTemporary,  //notifies the server that a new temporary file was added by client
		ECreateTemporary,    //instructs the server to create a new temporary file and return a handle to it to the client
		EOverwrite,          //instructs the server to create a new file (owervriting the existing one)
		
		//recovery of all transactions
		ERollBackAllPending
		};
		/**
			Delay in microseconds before the STS Server is shut down, after
			the last remaining session has been closed.
		 */
		const TInt KStsServerShutdownPeriod = 2 * 1000 * 1000;

		//IPC argument slot in client-server messaging
		const TInt KFilePathIPCSlot 	= 0;
		const TInt KIdIPCSlot       	= 1;
		const TInt KFileHandleIPCSlot	= 2;
		const TInt KFileModeIPCSlot	    = 3;
		
	}//endof namespace Usif
#endif




