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
* This file defines a SIF Server task that implements the CSifTransportTask interface.
*
*/


/**
 @file
 @internalComponent
*/

#ifndef SIFSERVERTASK_H
#define SIFSERVERTASK_H

#include <f32file.h>
#include <usif/sif/siftransporttask.h>
#include "sifservercommon.h"

namespace Usif
	{
	class CSifPlugin;

	/**
	A SIF Server task. It implements the functionality of the SIF Server.
	
	This task is instantiated and executed by the @see CSifTransportRequest class
	in response to an incoming software management request.
	*/
	class CSifServerTask : public CSifTransportTask
		{
	public:
		/**
		Instantiates a SIF Server task.
		
		@return Pointer to a @see CSifServerTask object.
		*/
		static CSifServerTask* NewL(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams);

		virtual ~CSifServerTask();

	protected:
		/**
		Implements the functionality of the SIF Server.
		
		@return TBool, ETrue if the processing of a software management request
		is completed and the task can be cleaned up, EFalse if the request requires
		further processing so this method will be called again.
		*/
		virtual TBool ExecuteImplL();

		/**
		Cancels an ongoing asynchronous request.
		*/
		virtual void CancelImpl();

	private:
		CSifServerTask(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams);

		void SelectAndLoadPluginL();

		TransportTaskFactory::TTaskType iTaskType;
		CSifPlugin* iPlugin;
		TUid iEcomKey;
		};

	} // End of namespace Usif


#endif	// #ifndef SIFSERVERTASK_H
