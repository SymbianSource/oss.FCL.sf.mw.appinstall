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
* Information shared between the SWI Observer client and server implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SWIOBCOMMON_H
#define SWIOBCOMMON_H
 
#include <e32ver.h>
#include <e32uid.h>
#include <f32file.h>
#include <e32std.h>
#include <s32file.h>

namespace Swi
	{
	_LIT(KSwiObserverName, "!SwiObserver");		///< Identifies SWI Observer Symbian OS server.

	const TInt KSwiObserverVerMajor = 1;		///< SWI Observer version major component.
	const TInt KSwiObserverVerMinor = 0;		///< SWI Observer version minor component.
	const TInt KSwiObserverVerBuild = 0;		///< SWI Observer version build component.

	inline TVersion SwiObserverVersion();
		
	/**
		Executable which hosts SWI Observer.  Required to launch the process.

		@see KUidSwiObserver
	*/
	_LIT(KSwiObserverImageName, "swiobserver.exe");
		
	/**
		Hosting the executable's secure ID.  Required to launch the process.

		@see KSwiObserverImageName
	 */
	const TUid KUidSwiObserver = {0x102836C3};
		
	inline TUidType SwiObserverImageFullUid();
	 	
	enum TSwiObserverFunction
	/**
		Functions supported by a SWI Observer session.
	 */
		{
			ERegisterToObserver=0,     ///< Registers to the SWI Observer. 
			ETransferFileHandle,	   ///< Transfer the observation file handle
			ESwiCommit				   ///< Start processing the observation file
		};
			
	/**
		Delay in microseconds before the SWI Observer is shut down, after
		the last remaining session has been closed.
	 */
	const TInt KSwiObserverShutdownPeriod = 2 * 1000 * 1000;
		
	enum TOperationType
	/**
		A set of enumerated values is used to identify which 
		SWI operation is in progress.
	 */
		{
		EOpInstall=0,
		EOpUninstall,
		EOpRestore
		};
		
	enum TPackageType
	/**
		A set of enumerated values which is used to indicate the type of the package
		which has influenced from the current SWI operation.
	 */
		{
		EStandardApplication=0,
		EStandardPatch,
		EPartialUpgrade,
		EPreinstalledApplication,
		EPreinstalledPatch,
		EUnknownPackage				//During uninstallation, the package type is not needed.
		};
	
	enum TFileFlag
	/**
		Describes various information about a file which has been written 
		into the log file as a record.
	 */
		{
		EFileAdded	 = 0x01,	///< The file was copied to the system.
		EFileDeleted = 0x02,	///< The file was deleted from the system.
		EFileDll	 = 0x10,	///< The file is a DLL.
		EFileExe	 = 0x20		///< The file is an EXE.
		};
							  	
	/** Indicates that the following line is in header format */
	const static TUint8 KHeaderField = 0x01;
		
	/** Indicates that the following line is in data format */
	const static TUint8 KDataField   = 0x02;
	
	/** Maximum length of a log file name. */
	const static TInt KMaxLogFileName = 8; //A log file name consists of 8 hexadecimal chars.
	
	_LIT(KSwiLogDir,     "log_swi\\");	///< A log file in this directory, if it has not started to be processed yet.
	_LIT(KObserverLogDir,"log_obs\\");	///< A log file in this directory, when it is being processed.
	
	_LIT(KObserverFlagFileNameFormat,":\\tmp\\%08xlog.flag");	///A flag file whose presence indicates to the client that there are logfiles left in the observer's private dir, so the client have to start the observer to get the files processed. 

	enum TFlagFileOperation
		{
		EDeleteFlagFile,   //Instructs the SWI Observer to remove the flag file that shows if there is any log is the log dir.
		ECreateFlagFile    //Instructs the SWI Observer to create the flag file that shows if there is any log is the log dir.
		};

	} // End of namespace Swi
 
 #include <swi/swiobcommon.inl>
 
#endif
