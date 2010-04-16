/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#if (!defined TSIFOPERATIONSTEP_H)
#define TSIFOPERATIONSTEP_H
#include <test/testexecutestepbase.h>
#include "tsifsuitestepbase.h"
#include <usif/sif/sif.h>

namespace Usif
	{
	class CSifOperationStep : public CSifSuiteStepBase
		{
	public:
		CSifOperationStep();
		~CSifOperationStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
#ifdef _DEBUG
		virtual RScsClientBase* ClientHandle() { return &(iSif.iTransport);}
#endif
		// Used by a non-member function
		void ClearOpaqueParams();
		
	protected:
		TBool CompareOpaqueResultsL();
		TBool CheckOpaqueResultsL();
		void PrintOpaqueParamsL(const COpaqueNamedParams& aOpaqueParams);
		void CancelableWait();

		void LoadExclusiveOperationFlagFromConfigL();
		void LoadFileNameFromConfigL();
		void LoadComponentIdFromConfigL();
		void LoadComponentInfoFromConfigL();
		void LoadPluginOpaqueParamsFromConfigL(const TDesC& aNamePattern, const TDesC& aTypePattern, const TDesC& aValuePattern, Usif::COpaqueNamedParams& aOpaqueParams);
		void LoadOpaqueParamsL();

		TBool iUseEnhancedApi;
		TBool iExclusiveOperation;
		TPtrC iFileName;
		Usif::TComponentId iComponentId;
		Usif::CComponentInfo* iComponentInfo;
		Usif::COpaqueNamedParams* iPluginOpaqueArguments;
		Usif::COpaqueNamedParams* iPluginOpaqueResults;
		Usif::COpaqueNamedParams* iPluginRefOpaqueResults;

		Usif::RSoftwareInstall iSif;
		TRequestStatus iStatus;

	private:
		TInt iCancelAfter;
		};

	class CSifGetComponentInfoStep : public CSifOperationStep
		{
	public:
		CSifGetComponentInfoStep();
		~CSifGetComponentInfoStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();

	private:
		void LoadComponentInfoL();
		TPtrC LoadCompInfoNodeStringParamL(const TDesC& aNodePrefix, const TDesC& aParamName);
		TInt LoadCompInfoNodeIntParamL(const TDesC& aNodePrefix, const TDesC& aParamName, TBool aMandatory = ETrue);
		TBool LoadCompInfoNodeBoolParamL(const TDesC& aNodePrefix, const TDesC& aParamName, TBool aMandatory = ETrue);
		CComponentInfo::CNode* LoadCompInfoNodeLC(const TDesC& aNodeName);
		TBool CompareCompInfoNodeL(const CComponentInfo::CNode& aExpectedNode, const CComponentInfo::CNode& aObtainedNode);
		TBool CompareAppInfoL(const CComponentInfo::CNode& aExpectedNode, const CComponentInfo::CNode& aObtainedNode);
		TBool iOperationByFileHandle;
		TBool iCompareMaxInstalledSize;
		TBool iCompareIconFileSize;
		RArray<TInt> iIconFileSizes;
		TInt iconIndex;
		};

	class CSifInstallStep : public CSifOperationStep
		{
	public:
		CSifInstallStep();
		~CSifInstallStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();

	private:
		TBool iInstallByFileHandle;
		};

	class CSifUninstallStep : public CSifOperationStep
		{
	public:
		CSifUninstallStep();
		~CSifUninstallStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();
		};

	class CSifActivateStep : public CSifOperationStep
		{
	public:
		CSifActivateStep(TBool aActivate);
		~CSifActivateStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();
		
	private:
		TBool iActivate;
		};
		
	class CSifMultipleInstallStep : public CSifOperationStep
		{
	public:
		CSifMultipleInstallStep();
		~CSifMultipleInstallStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();
		};

	_LIT(KSifGetComponentInfoStep,"SifGetComponentInfoStep");
	_LIT(KSifInstallStep,"SifInstallStep");
	_LIT(KSifUninstallStep,"SifUninstallStep");
	_LIT(KSifActivateStep,"SifActivateStep");
	_LIT(KSifDeactivateStep,"SifDeactivateStep");
	_LIT(KSifMultipleInstallStep,"SifMultipleInstallStep");

	} // namespace Sif
	
#endif
