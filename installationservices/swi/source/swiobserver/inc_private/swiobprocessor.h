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
* Defines the SWI Observer processor class.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SWIOBPROCESSOR_H
#define SWIOBPROCESSOR_H

#include <e32property.h>
#include <connect/sbdefs.h>
#include "swiobserverplugin.h"
#include "swiobservationreader.h"

namespace Swi
	{
	//Forward declaration
	class CSwiObserver;
	
	enum TProcessorProgress
	/**
		A set of enumerated values which is used to indicate the progress of the SWI
		Observer processor.
	 */
	 	{
	 	EProcessorIdle,
	 	EProcessorSubscribed,
	 	EProcessorBusy
	 	};
	 	
	/** Indicates if the file process requires notification of the ecom plugin. */
	enum TLogFileProcess
		{
		ENoNotification,		///< No notification.
		EHeaderNotification,	///< Package header information should send to the plugin.
		EDataNotification		///< Record information should be send to the plug-in.
		};
			 			
	class CSwiObserverProcessor : public CActive
	/**
		An instance of this class processes all log files with .obs extension in 
		the SWI Observer's private directory and notifies plug-ins to perform their 
		own tasks.	
	 */
		{
		/** Defines the states of the SWI Observer processor. */
		enum TObserverState
			{
			EIdle,				///< The initial state
			ERestoreCompleted,	///< Subscribes to KUidBackupRestoreKey property to be notified when backup or restore has been finished.
			ENextPlugin,		///< Instantiates the next plug-in from the registered plug-ins list.
			ENextLogFile,		///< Opens the next log file from the observer log directory.
			EProcessLogFile,	///< Process the log file and do required notifications.
			ECompleteProcess	///< Completes the processor.
			};
		
	public:
		static CSwiObserverProcessor* NewL(CSwiObserver& aObserver);
		void StartProcessingL();
		TProcessorProgress ProgressStatus();
				
		~CSwiObserverProcessor();
				
	protected:
		CSwiObserverProcessor(CSwiObserver& aObserver);
		//Inherited pure virtual methods of CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
			
	private:
		void ConstructL();
		void RestoreCompletedL();
		void SelfComplete();
		void NextPluginL();
		void NextLogFileL();
		void ProcessLogFileL();
		void CompleteProcess();
		void Reset();
		TLogFileProcess ProcessNextLineL(TInt& aIndex);
		void MoveLogFileL(RFs aFs, const TDesC& aFrom, const TDesC& aTo);
		
	private:
		CSwiObserver& iObserver;	//<< Owning server.
		/** Pointer to the loaded swi observer ecom plug-in implementation. */
		CSwiObserverPlugin* iPlugin;
		/** The list of files in which the currently loaded plug-in is interested.*/
		CObservationFilter* iCurrentFilter;
		/** 
			Indicates the progress status of the processor. 
			The SWI Observer checks whether the processor is currently
			handling a commit operation made by the SWI Installer.
		 */ 
		TProcessorProgress iProgress;
		/** The current state of the SWI Observer processor.*/
		TObserverState iState;
		/** Interface used to subscribe Backup&Restore property.*/
		RProperty iProperty; 
		/** The implementation list of the SWI Observer ECOM plug-in interface. */
		RImplInfoPtrArray iImplList;
		/** Currently loaded ECOM plug-in. */ 
		TInt iCurrentImpl;
		/** The .obs file which is currently being processsed. */ 
		RBuf iCurrentFileName;
		/** The reader object used to read observations from the log file. */
		CSwiObservationReader* iReader;
		/** 
			The header object holding the current header line which has been
			read from the log file. 
		 */	
		CObservationHeader* iHeader;
		/** 
			The data object holding the current data record which has been 
			read from the log file. 
		 */	
		CObservationData* iData;
		};
	} // End of namespace Swi
#endif