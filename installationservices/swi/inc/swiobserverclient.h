/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Client-side API via which the SWI Installer uses the SWI Observer service.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SWIOBSERVERCLIENT_H 
#define SWIOBSERVERCLIENT_H

#include <scs/scsclient.h>
#include <swi/swiobcommon.h>
#include <swi/swiobservedinfo.h>

namespace Swi
	{
	class RSwiObserverSession : public RScsClientBase
	/**
		The SWI Installer uses this class to establish a connection with the SWI Observer
		and to transfer the log file handle including changed files list. 
	 */
		{
	public:
		IMPORT_C RSwiObserverSession();
		IMPORT_C TInt Connect();
		IMPORT_C void Close();
		IMPORT_C void ProcessLogsL(RFs& aFs);
		IMPORT_C void Register(TRequestStatus& aStatus);
		IMPORT_C void CancelRegistration();
		IMPORT_C void GetFileHandleL(TDes& aFileName);
		IMPORT_C void CommitL();
		IMPORT_C void AddEventL(const CObservationData& aData);
		IMPORT_C void AddHeaderL(const CObservationHeader& aHeader);
			
	private:
		RFile iFile; 				///< Handle to the log file created by the SWI Observer.
		RFileWriteStream iStream; 	///< The writing stream used to write information into the log file.
		TThreadId iClientTid; 		///< The Id of the thread who connects to the SWI Observer.
		};

	/** Delay in microseconds before the client re-tries to connect to the SWI Observer. */
	const static TInt KSWIObserverRetryDelay = 2 * 1000 * 1000;
	
	} // End of namespace Swi
	
#endif
