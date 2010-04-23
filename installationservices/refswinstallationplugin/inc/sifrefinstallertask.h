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
* This file defines tasks for the Reference Installer. These tasks implement the CSifTransportTask
*
*/


/**
 @file
 @internalComponent
 exampleCode
 interface. Please @see siftransporttask.h for details of this interface.
*/

#ifndef SIFREFINSTALLERTASK_H
#define SIFREFINSTALLERTASK_H

#include <usif/scr/scr.h>
#include <usif/sts/sts.h>
#include <usif/sif/sif.h>
#include <usif/sif/siftransporttask.h>
#include "sifrefbinpkgextractor.h"
#include "sifrefpkgparser.h"

namespace Usif
	{

	/**
	The MInstallerUIHandler class defines an interface for UI callbacks. The Reference
	Installer tasks uses a concrete implementation of this interface to interact with the user.
	
	This interface defines four arbitrary chosen callbacks. A real installer may define
	its own interface when these callbacks aren't sufficient.
	*/
	class MInstallerUIHandler
		{
	public:
		/**
		Gets called when an installer needs a confirmation dialog to be displayed to the user.
		*/
		virtual TBool ConfirmationUIHandler(const TDesC& aQuestion) = 0;

		/**
		Gets called when an installer needs an error description to be displayed to the user.
		*/
		virtual void ErrorDescriptionUIHandler(const TDesC& aDescription) = 0;
		};

	/**
	A factory function. The Reference Installer Server uses this function to instantiate a UI handler object.
	*/
	typedef MInstallerUIHandler* (*TInstallerUIHandlerFactory)();

// ===========================================================================================================

	/**
	An auxiliary data structure used by the Reference Installer tasks for SCR search results.
	*/
	struct TComponentSearchData
		{
		const TDesC* iName;
		const TDesC* iVendor;
		TVersionName iVersion;
		TScomoState iScomoState;
		TInstallStatus iInstallStatus;
		TComponentId iComponentId;
		};

// ===========================================================================================================

	/**
	A Reference Installer task that implements the 'GetComponentInfo' SIF request.
	*/
	class CSifRefGetComponentInfoTask : public CSifTransportTask
		{
	public:
		static CSifRefGetComponentInfoTask* NewL(TTransportTaskParams& aParams);
		virtual ~CSifRefGetComponentInfoTask();

	protected:
		virtual TBool ExecuteImplL();

	private:
		CSifRefGetComponentInfoTask(TTransportTaskParams& aParams);

		// GetComponentInfo steps
		void ExtractEmbeddedPkgsL();
		TInt ParsePkgFileL();
		void CheckAndSetComponentInfoL();
		TInt CreateComponentInfoNodeL();
		void SetComponentInfoL();

		// Installer's tools
		RSoftwareComponentRegistry iScr;
		RStsSession iSts; // used for temporary files
		CSifRefPkgParser* iParser;

		// Interstate data
		TComponentSearchData iCompSearchData;

		// Embedded components extracted from a compound package
		RCPointerArray<SifRefBinPkgExtractor::CAuxNode> iEmbeddedComponents;
		TInt iCurrentComponent;

		// Stuff needed for processing packages of different types
		RSoftwareInstall iSif;
		TBool iSifRequestInProgress;
		CComponentInfo* iComponentInfo;
		RFs iFs;
		RFile iFile;

		// The GetComponentInfo task consists of the following steps
		enum TGetComponentInfoSteps
			{
			EExtractEmbeddedPkgs = 1,
			EParsePkgFile,
			EFindComponent,
			ECreateComponentInfoNode,
			ESetComponentInfo
			};
		TInt iStep;
		};

// ===========================================================================================================

	/**
	A Reference Installer task that implements the 'Install' SIF request.
	*/
	class CSifRefInstallTask : public CSifTransportTask
		{
	public:
		static CSifRefInstallTask* NewL(TTransportTaskParams& aParams, TInstallerUIHandlerFactory aUiHandlerFactory);
		virtual ~CSifRefInstallTask();

	protected:
		virtual TBool ExecuteImplL();

	private:
		CSifRefInstallTask(TTransportTaskParams& aParams);
		void ConstructL(TInstallerUIHandlerFactory aUiHandlerFactory);

		// Installation steps
		void ExtractEmbeddedPkgsL();
		TInt ParsePkgFileImplL();
		TInt ParsePkgFileL();
		void LaunchForeignInstallL();
		void FinishForeignInstallL();
		TInt FindAndCheckComponentL();
		void GetInstalledFileListL();
		TBool UnregisterAndDeleteFileL();
		void RegisterComponentL();
		TBool CopyFileL();
		TInt SetScomoStateL();
		void CommitL();

		// Installer's tools
		RSoftwareComponentRegistry iScr;
		RStsSession iSts;
		CSifRefPkgParser* iParser;
		TBool iScrTransaction;

		// UI interaction
		MInstallerUIHandler* iUiHandler;

		// Interstate data
		TComponentSearchData iCompSearchData;
		RSoftwareComponentRegistryFilesList iFileList;

		// Embedded components extracted from a compound package
		RCPointerArray<SifRefBinPkgExtractor::CAuxNode> iEmbeddedComponents;
		TInt iCurrentComponent;

		// Stuff needed for processing packages of different types
		RSoftwareInstall iSif;
		COpaqueNamedParams* iOpaqueArguments;
		COpaqueNamedParams* iOpaqueResults;
		TBool iSifRequestInProgress;
		RFs iFs;
		RFile iFile;

		// The Install task consists of the following steps
		enum TInstallSteps
			{
			EExtractEmbeddedPkgs = 1,
			EParsePkgFile,
			ELaunchForeignInstall,
			EFinishForeignInstall,
			EFindAndCheckComponent,
			EGetInstalledFileList,
			EUnregisterAndDeleteFile,
			ERegisterComponent,
			ECopyFile,
			ESetScomoState,
			ECommit
			};
		TInt iStep;

		// File index for the state machine in the ECopyFile step
		TInt iCopyFileIndex;
		// Component size will be calculated and set in the SCR
		TInt64 iComponentSize; 
		};

// ===========================================================================================================

	/**
	A Reference Installer task that implements the 'Uninstall' SIF request.
	*/
	class CSifRefUninstallTask : public CSifTransportTask
		{
	public:
		static CSifRefUninstallTask* NewL(TTransportTaskParams& aParams, TInstallerUIHandlerFactory aUiHandlerFactory);
		virtual ~CSifRefUninstallTask();

	protected:
		virtual TBool ExecuteImplL();

	private:
		CSifRefUninstallTask(TTransportTaskParams& aParams);
		void ConstructL(TInstallerUIHandlerFactory aUiHandlerFactory);

		void GetFileListL();
		TBool UnregisterAndDeleteFileL();
		void CommitL();

		// UI interaction
		MInstallerUIHandler* iUiHandler;

		// Installer's tools
		RSoftwareComponentRegistry iScr;
		RStsSession iSts;
		
		// Interstate data
		RSoftwareComponentRegistryFilesList iFileList;

		// The Uninstall task consists of the following steps
		enum TUninstallSteps
			{
			EGetFileList = 1,
			EUnregisterAndDeleteFile,
			ECommit
			};
		TInt iStep;
		};

// ===========================================================================================================

	/**
	A Reference Installer task that implements the 'Activate' and 'Deactivate' SIF requests.
	*/
	class CSifRefActivateDeactivateTask : public CSifTransportTask
		{
	public:
		CSifRefActivateDeactivateTask(TTransportTaskParams& aParams, TScomoState aScomoState);
		virtual ~CSifRefActivateDeactivateTask();

	protected:
		virtual TBool ExecuteImplL();

	private:
		// Installer's tools
		RSoftwareComponentRegistry iScr;

		TScomoState iScomoState;
		};

	} // namespace Usif

#endif // SIFREFINSTALLERTASK_H