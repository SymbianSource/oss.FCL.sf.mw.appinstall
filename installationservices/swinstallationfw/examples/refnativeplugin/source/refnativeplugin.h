/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This class implements a test SIF plugin for native software
*
*/


/**
 @file
 @internalComponent
*/

#ifndef REFNATIVEPLUGIN_H
#define REFNATIVEPLUGIN_H

#include <usif/sif/sifplugin.h>
#include <usif/scr/screntries.h>
#include <swi/msisuihandlers.h>
#include <e32base.h>
#include <e32std.h>

namespace Swi
{
	class CAsyncLauncher;
	class CInstallPrefs;
}

namespace Usif
	{
	
	_LIT(KDeclineOperationOptionName, "SwiDeclineOperation");
	// ECOM objects and CActive do not interact well - especially since SIFPlugin inherits from CBase
	// and double C-inheritance is impossible. So, a separate class is used to drive the asynchronous interaction
	// to the CAsyncLauncher
	NONSHARABLE_CLASS(CRefNativePluginActiveImpl) : public CActive, public Swi::MUiHandler
	{
	public:
		static CRefNativePluginActiveImpl* NewL();
		~CRefNativePluginActiveImpl();

		// CActive interface
		void RunL();
		void DoCancel();
			
		void Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);

		void Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);
							
		void GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, 
							TRequestStatus& aStatus);

		void Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
							TRequestStatus& aStatus);

		void Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
							TRequestStatus& aStatus);

		// Swi::MUiHandler interface
		TInt DisplayLanguageL(const Swi::CAppInfo& aAppInfo,const RArray<TLanguage>& aLanguages);
		TInt DisplayDriveL(const Swi::CAppInfo& aAppInfo,TInt64 aSize, const RArray<TChar>& aDriveLetters,const RArray<TInt64>& aDriveSpaces);
		TBool DisplayUninstallL(const Swi::CAppInfo& aAppInfo);
		TBool DisplayTextL(const Swi::CAppInfo& aAppInfo,Swi::TFileTextOption aOption,const TDesC& aText);
		TBool DisplayDependencyBreakL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aComponents);
		TBool DisplayApplicationsInUseL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aAppNames);
		TBool DisplayQuestionL(const Swi::CAppInfo& aAppInfo, Swi::TQuestionDialog aQuestion, const TDesC& aDes);
		TBool DisplayInstallL(const Swi::CAppInfo& aAppInfo,const CApaMaskedBitmap* aLogo, const RPointerArray<Swi::CCertificateInfo>& aCertificates); 
		TBool DisplayGrantCapabilitiesL(const Swi::CAppInfo& aAppInfo, const TCapabilitySet& aCapabilitySet);
		TBool DisplayUpgradeL(const Swi::CAppInfo& aAppInfo, const Swi::CAppInfo& aExistingAppInfo);
		TBool DisplayOptionsL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aOptions,RArray<TBool>& aSelections);
		TBool DisplaySecurityWarningL(const Swi::CAppInfo& aAppInfo, Swi::TSignatureValidationResult aSigValidationResult,
									RPointerArray<CPKIXValidationResultBase>& aPkixResults, RPointerArray<Swi::CCertificateInfo>& aCertificates,TBool aInstallAnyway);
		TBool DisplayOcspResultL(const Swi::CAppInfo& aAppInfo, Swi::TRevocationDialogMessage aMessage,RPointerArray<TOCSPOutcome>& aOutcomes, 
									RPointerArray<Swi::CCertificateInfo>& aCertificates,TBool aWarningOnly);
		TBool DisplayMissingDependencyL(const Swi::CAppInfo& aAppInfo, const TDesC& aDependencyName,TVersion aWantedVersionFrom,
									TVersion aWantedVersionTo,TVersion aInstalledVersion);
		TBool HandleInstallEventL(const Swi::CAppInfo& aAppInfo, Swi::TInstallEvent aEvent,TInt aValue = 0,const TDesC& aDes = KNullDesC);
		void HandleCancellableInstallEventL(const Swi::CAppInfo& aAppInfo, Swi::TInstallCancellableEvent aEvent,Swi::MCancelHandler& aCancelHandler,
									TInt aValue = 0,const TDesC& aDes=KNullDesC);
		void DisplayCannotOverwriteFileL(const Swi::CAppInfo& aAppInfo, const Swi::CAppInfo& aInstalledAppInfo,const TDesC& aFileName);
		void DisplayErrorL(const Swi::CAppInfo& aAppInfo,Swi::TErrorDialog aType,const TDesC& aParam);
	private:
		CRefNativePluginActiveImpl() : CActive(EPriorityStandard) {}
		CRefNativePluginActiveImpl(const CRefNativePluginActiveImpl &);
		CRefNativePluginActiveImpl & operator =(const CRefNativePluginActiveImpl &);
		void ConstructL();
		void CommonRequestPreamble(const TSecurityContext& aSecurityContext, const COpaqueNamedParams& aInputParams, 
									COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus);
		void UninstallL(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
						const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, 
						TRequestStatus& aStatus);
						
		TComponentId GetLastInstalledComponentIdL();
		TBool NeedUserCapabilityL();
		void ProcessSilentInstallL();
	private:
		Swi::CAsyncLauncher* iAsyncLauncher;
		Swi::CInstallPrefs* iInstallPrefs;
		TRequestStatus* iClientStatus;
		const COpaqueNamedParams* iInputParams;
		COpaqueNamedParams* iOutputParams;
		CComponentInfo* iComponentInfo;
		TFileName iFileName;
		RFile* iFileHandle; // FileHandle is not owned by the plug-in
		TBool iDeclineOperation; // Used for plugin options - optionally specifies that the operation will not be confirmed at the first callback
		TBool iInstallRequest; // Used to identify the type of the current requst in RunL() so we know if the id of an installed component should be sent
		TBool iSilentInstall; // Used to identify a silent install
		};


	NONSHARABLE_CLASS(CRefNativePlugin) : public CSifPlugin
		{
	public:
		static CRefNativePlugin* NewL();
		~CRefNativePlugin();
	
		// MSIFPlugin interface
		void GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							  CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void GetComponentInfo(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							  CComponentInfo& aComponentInfo, TRequestStatus& aStatus);
		
		void Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);

		void Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);

		virtual void Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus);

		virtual void Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus);

		virtual void Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus);

		void CancelOperation();
	private:
		CRefNativePlugin() {}
		void ConstructL();
		CRefNativePlugin(const CRefNativePlugin &);
		CRefNativePlugin & operator =(const CRefNativePlugin &);
	private:
		CRefNativePluginActiveImpl *iImpl;
		};
	} // end namespace Usif

#endif // REFNATIVEPLUGIN_H
