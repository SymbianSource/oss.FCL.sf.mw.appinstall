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
* The file defines a base class for SIF Transport tasks and a factory function for them.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFTRANSPORTTASK_H
#define SIFTRANSPORTTASK_H

#include <f32file.h>
#include <usif/sif/sifcommon.h>

namespace Usif
	{

	/** An auxiliary data structure for passing task's parameters across the SIF Transport library. */
	struct TTransportTaskParams
		{
		IMPORT_C TTransportTaskParams();
		CComponentInfo* iComponentInfo;
		TComponentId iComponentId;
		const TDesC* iFileName;
		RFile* iFileHandle;
		const COpaqueNamedParams* iCustomArguments;
		COpaqueNamedParams* iCustomResults;
		const TSecurityContext* iSecurityContext;
		TRequestStatus* iRequestStatus;
		enum { EInvalidComponentId = -1 };
		};

// ===========================================================================================================

	/**
	The core class of the SIF Transport library.
	
	It defines an abstract interface for SIF Transport tasks. These tasks are instantiated
	and executed to handle processing of incoming software management requests.
	
	Each Transport task is executed in the context of the active object. If a task issues
	a request to an asynchronous service provider, it should use a @see TRequestStatus
	object provided in @see TTransportTaskParams. Otherwise, tasks that don't issue
	asynchronous requests must complete this request status themselves before they leave
	the @see CSifTransportTask::ExecuteImplL method. In order to simplify tasks that don't
	deal with asynchronous service providers the AutoCompletion mode has been implementd. In this
	mode the request status is completed automatically by the base class. The AutoCompletion
	mode is enabled by default and should be disabled in constructors of tasks that issue
	asynchronous requests.
	
	An implementation of the Transport Task Factory defined below is responsible for
	instantiating Transport tasks.
	*/
	class CSifTransportTask : public CBase
		{
	protected:
		/**
		Constructs the object.
		
		@param aParams Parameters needed to execute the task. Please
		see @see TTransportTaskParams for details.
		@param aAutoCompletion Enables the AutoCompletion mode. 
		*/
		IMPORT_C CSifTransportTask(TTransportTaskParams& aParams, TBool aAutoCompletion = ETrue);

		/** Destroys the object. */
		IMPORT_C virtual ~CSifTransportTask();

		/**
		Implementation of this abstract method is supposed to process an incoming software
		management request.
		
		If this method leaves with any error code the execution of the task terminates
		immediately. The leave code is passed to the user as a request completion code.
		
		@return TBool, ETrue it the processing of a software management request has been
		completed and the task can be cleaned up, EFalse if the method must be called again
		for further processing.
		*/
		IMPORT_C virtual TBool ExecuteImplL() = 0;
		
		/**
		Cancels the execution of an asynchronous task.
		
		The default implementation is empty. Tasks that issue requests to asynchronous
		service providers must implement this method in order to cancel them.
		*/
		IMPORT_C virtual void CancelImpl();

		/** Gets the id of a component */
		IMPORT_C TComponentId ComponentId() const;
		/** Gets the file name of a package */
		IMPORT_C const TDesC* FileName() const;
		/** Gets the file handle of a package */
		IMPORT_C RFile* FileHandle();
		/** Gets custom arguments */
		IMPORT_C const COpaqueNamedParams* CustomArguments() const;
		/** Gets custom results */
		IMPORT_C COpaqueNamedParams* CustomResults();
		/** Gets security context */
		IMPORT_C const TSecurityContext* SecurityContext() const;
		/** Gets request status associated with the task */
		IMPORT_C TRequestStatus* RequestStatus();
		/** Gets component info */
		IMPORT_C CComponentInfo* ComponentInfo();

	private:
		// These methods are used only by the CSifTransportRequest class to execute and cancel SIF Transport tasks
		friend class CSifTransportRequest;
		TBool Execute();
		void Cancel();

	private:
		CSifTransportTask(const CSifTransportTask&);
		CSifTransportTask& operator=(const CSifTransportTask&);
		
		TTransportTaskParams& iParams;
		TBool iAutoCompletion;
		};

// ===========================================================================================================

	/**
	Definition of a factory function for the SIF Transport library. The @see SifTransportRequest class uses implementations
	of this function to instantiate and execute appropriate SIF Transport tasks. It happens in response to incoming software
	management requests.
	*/
	namespace TransportTaskFactory
		{
		enum TTaskType
			{
			EGetComponentInfo,
			EInstall,
			EUninstall,
			EActivate,
			EDeactivate
			};

		typedef CSifTransportTask* (*GenerateTask)(TTaskType aTaskType, TTransportTaskParams& aParams);
		}

	} // namespace Usif

#endif // SIFTRANSPORTTASK_H
