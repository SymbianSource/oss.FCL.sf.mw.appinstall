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
* Defines server-side classes which are used to implement the SWI Observer.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 

#ifndef SWIOBSERVER_H
#define SWIOBSERVER_H

#include <scs/scsserver.h>
#include <swi/swiobcommon.h>
#include "swiobprocessor.h"

namespace Swi
	{
	//forward declaration
	class CSwiObserver;
		 	
	class CSwiObserverSession : public CScsSession
	/**
		This session object is created for each of the SWI Observer's clients 
		(at the moment just SWI Installer). It is used to perform client requests.
	 */
		{
	public:
		static CSwiObserverSession* NewL(CSwiObserver &aServer, const RMessage2& aMessage);
		~CSwiObserverSession();
				
		// implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

	private:
		CSwiObserverSession(CSwiObserver& aServer);
				
	private:
		/**
			Pointer to the asynchronous waiter object. A session object can only 
			have one waiter object. After the waiter object is created, its ownership
			is passed to the SCS framework. Therefore, it IS NOT deleted in the destructor
			of the session.
		 */
		//CSwiObserverWaiter* iWaiter; 
		/** The fully qualified name of the log file created in the current session. */
		RBuf iFullFileName;
		
		RFile iFile; 		///<File handle to the log file which possesses to this session.
		
		/** 
			The status of a SWI Observer session. A client must first register
			to the server. After registration, the client gets a file handle,
			writes its log and then commits the session.
		 */
		enum TObserverSessionProgress
			{
			ERegistered = 0x01,   ///< The session has been registered to the SWI Observer.
			ECommitted  = 0x02,   ///< The session has been committed the log file.
			};
			
		TUint8 iSessionProgress;   ///< The current progress status of the session object.
		};
	
	class CSwiObserver : public CScsServer
	/**
		The CScsServer derived object which is used to generate SWI Observer
		sessions for the SWI Installer.
	 */
		{
		friend class CSwiObserverSession;
		
	public:
		static CSwiObserver* NewLC();
				
		~CSwiObserver();
				
		void ProcessL();
		void AppendFileNameL(RBuf& aFileName);
		void ProcessorCompleted();
		HBufC* GetLogDirPathLC(const TDesC& aLogDir);
		HBufC* GetLogFilePathLC(const TDesC& aLogFile, const TDesC& aLogDir);
		TProcessorProgress ProcessorProgressStatus();
		
		//Implement from CScsServer
		void DoPreHeapMarkOrCheckL();
		void DoPostHeapMarkOrCheckL();
		CScsSession* DoNewSessionL(const RMessage2& aMessage);
		
	private:
		CSwiObserver();
		void ConstructL();	
		TBool InitializeLogDirsL();
		HBufC* MakeLogDirectoryLC(const TDesC& aLogDir);
		static HBufC* CreateFlagFileNameLC();
		void ManipulateFlagFileL(TFlagFileOperation operation);
		
	private:
		/**
			The highest file name number existing under the SWI Observer's private folder.
	 	 */
		TUint iHighestFileNum;
				
		/** Pointer to the SWI Observer processor object. */
		CSwiObserverProcessor* iProcessor;

	public:
		/** File server handle. Public, since it is accessed by other classes. */
		RFs iFs;
				
		/** The private folder of the SWI Observer process. */
		HBufC* iPrivateFolder;
		};
	
	} // End of namespace Swi				
#endif
