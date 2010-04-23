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
*
*/


/**
 @file 
 @internalComponent
*/
 
#include <e32property.h>
#include <e32base.h>

// Forward declarations
	
class RApaLsSession;
	
namespace Swi
	{	
	/**
	 * Describes the installation behaviour of any sis files found by the Daemon
	 */
	class MDaemonInstallBehaviour
		{
	public:
		/**
		 * The behavioural states 
		 */
		enum TSisInstallState 
			{
			EStateVerifyAppCacheList,
			EStateVerifySwisProperty,
			EStateVerifySwisIdle,
			EStateInstall
			};

	public:
		/**
		 * Application architecture server has a cached list of applications
		 * and this is required to be ready to use
		 */
		virtual TSisInstallState VerifyAppCacheListL() = 0;
		
		/**
		 * The software installer uses a property to indicate what operation it is in,
		 * this should be defined
		 */
		virtual TSisInstallState VerifySwisPropertyL() = 0;
		
		/**
		 * Before any installation of sisx files can take place, the software installer
		 * must not be busy
		 */
		virtual TSisInstallState VerifySwisIdleL() = 0;

		/**
		 * Request Installation
		 *
		 * If not using the swidaemonplugin, then use apparch to
		 * launch the handler for the SIS file and block until the
		 * handler is done (ie. exits).
		 *
		 * If using the swidaemonplugin, then ask the plugin to do the
		 * install, but do NOT wait for it to complete (and the plugin
		 * RequestInstallL function must not block either).
		 *
		 * @param aFileName The name of the sis filename
		 */
		virtual void DoInstallRequestL(const TDesC& aFileName) = 0;

		/**
		 * Tell daemon we have called DoInstallRequestL for all SIS
		 * files on this removable media.
		 */
		virtual void DoNotifyMediaProcessingComplete() = 0;
		};

	/**
	 * The Daemon SIS installer
	 */
	class CSisInstaller : public CActive
		{
	public:
		/**
		 * Symbian construction (for member variables)
		 */
		static CSisInstaller* NewL(MDaemonInstallBehaviour* aDaemonBehaviour);
		
		/**
		 * C++ destructor
		 */
		~CSisInstaller();
		
		/**
		 Adds a file to the list of files to be installed
		 @param aFileName The full name and path of the file to be installed
		 */
		void AddFileToInstallL(const TDesC& aFileName);
		
		/**
		 * Install the specified sis file
		 */
		void StartInstallingL();
				
	private: // From CActive
	
 		/**
		 * Called by Cancel when a request from this class is outstanding
		 * nb. You should probably call Reset instead of Cancel.
		 */
		void DoCancel();
	
	public:
		/**
		 * Cancel outstanding requests and reset statemachine.
		 * Also calls Cancel.
		 * Called by DoCancel, StartInstallingL and RunError
		 */
		void Reset();

	private:
		/**
		 * Asynchronous completion request processing
		 */
		void RunL();
		
		/**
		 * Handles any Leaves from the RunL
		 */
		TInt RunError(TInt aError);

	private:
		/**
		 * C++ constructor
		 */
		CSisInstaller(MDaemonInstallBehaviour* aDaemonBehaviour);
		
		/**
		 * 2nd phase constructor
		 */
		void ConstructL();
		
		/**
		 * Asynchronous request activate
		 */
		void ReRequestL();
		
		/**
		 * Self request completion
		 */
		void CompleteSelf();
								
	private:
		// The behaviour of the daemon with respect to installing
		MDaemonInstallBehaviour* iDaemonBehaviour;
	
		// The state
		MDaemonInstallBehaviour::TSisInstallState iState;
		
		RPointerArray <HBufC> iFilesToInstall;
		
		// Sis files and location
		TBuf<KMaxFileName> iSisFile;
		TInt iFileIndex;
		
		// Timer and time step
		RTimer iTimer;
		TTimeIntervalMicroSeconds32 iTimeStep;
		
		// Software installer property
		RProperty iProperty;
		};		
	}

