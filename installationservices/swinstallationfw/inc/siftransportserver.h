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
* The server side of the SIF Transport library
*
*/


/**
 @file
 @internalComponent
*/

#ifndef SIFTRANSPORTSERVER_H
#define SIFTRANSPORTSERVER_H

#include <f32file.h>
#include <scs/scsserver.h>
#include <usif/sif/sifcommon.h>
#include <usif/sif/siftransporttask.h>
#include "siftransportcommon.h"

namespace Usif
	{
	// forward declaration
	class CSifTransportServer;

	/**
	A SIF Transport session. It is instantiated by the @see CSifTransportServer class in response
	to an incoming connection. The session instantiates an asynchronous request (an instance
	of the @see CSifTransportRequest class) and hands over the processing of a request to it.
	*/
	class CSifTransportSession : public CScsSession
		{
	public:
		/** Creates an instance of the CSifTransportSession class. */
		static CSifTransportSession* NewL(CSifTransportServer &aServer, TransportTaskFactory::GenerateTask aTaskFactory);

		/** Destroys an instance of the CSifTransportSession class. */
		virtual ~CSifTransportSession();

		/** Implements the CScsSession interface */
		virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

	private:
		CSifTransportSession(CSifTransportServer &aServer, TransportTaskFactory::GenerateTask aTaskFactory);

		TransportTaskFactory::GenerateTask iTaskFactory;
		TBool iExclusiveOperation;
		};

	/**
	A SIF Transport server. It is instantiated by the system when a client submits a connection request.
	The server instantiates a SIF Transport session object in response to an incoming software management
	request and hands over further processing to it.
	
	This is a transient server which means that it is instantiated in response to an incoming connection
	and shuts down itself, after timeout specified in @see StartTransportServer, if there are no sessions.
	*/
	class CSifTransportServer : public CScsServer
		{
	public:
		/** Creates an instance of the CSifTransportSession class. */
		IMPORT_C static CScsServer* NewSifTransportServerLC();

		/** Destroys an instance of the CSifTransportServer class. */
		virtual ~CSifTransportServer();

		/** Implements the CScsServer interface */
		virtual CScsSession* DoNewSessionL(const RMessage2& aMessage);

	private:
		friend TInt StartTransportServer(const TDesC& aServerName, const TVersion& aVersion, TransportTaskFactory::GenerateTask aTaskFactory, TInt aShutdownPeriodUs);
		static CSifTransportServer* NewLC(const TDesC& aServerName, const TVersion& aVersion, TransportTaskFactory::GenerateTask aTaskFactory, TInt aShutdownPeriodUs);

		CSifTransportServer(const TVersion& aVersion, TransportTaskFactory::GenerateTask aFactory);
		void ConstructL(const TDesC& aServerName, TInt aShutdownPeriodUs);

		TransportTaskFactory::GenerateTask iTaskFactory;
		};

	/**
	A SIF Transport request. An instance of this object is created by @see CSifTransportSession
	class to represent an asynchronous software management request. The request takes a task
	factory in the @see CSifTransportRequest::CreateAndExecuteL() method and uses it to
	instantiate an appropriate task and execute it.
	 */
	class CSifTransportRequest : public CAsyncRequest
		{
	public:
		/** Creates an instance of the CSifTransportRequest class. */
		static void CreateAndExecuteL(TInt aFunction, CSifTransportSession* aSession, TransportTaskFactory::GenerateTask aTaskFactory, const RMessage2& aMessage);
		
		/** Destroys an instance of the CSifTransportRequest class. */
		virtual ~CSifTransportRequest();

	private:
		CSifTransportRequest(TInt aFunction, CSifTransportSession* aSession, const RMessage2& aMessage);
		void PrepareParamsL();
		void SetupRequestL(TransportTaskFactory::GenerateTask aTaskFactory);
		TInt AdoptFileHandle(TInt aFunction, const RMessage2& aMessage);
		
		void ProcessTaskResultsL();
		void ProcessOpaqueResultsL();
		void LaunchTask();

		// Implement CAsyncRequest and CActive
		virtual void DoCleanup();
		virtual void DoCancel();
		virtual void RunL();

	private:
		enum { EInvalidComponentId = -1 };

		RFile iFile;
		TInt iFunction;
		const TSecurityContext iSecurityContext;
		TComponentId iComponentId;

		// Task to be executed by the server and its params
		CSifTransportTask* iTask;
		TTransportTaskParams iParams;
		TBool iTaskComplete;
		};

	// The string below must be up to 16 characters, otherwise it will be truncated
	_LIT(KSifTransportServerRequestError, "SifTransportError");

	} // End of namespace Usif

#endif	// #ifndef SIFTRANSPORTSERVER_H
