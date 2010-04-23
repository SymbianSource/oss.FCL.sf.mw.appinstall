/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This file provides the class declaration of CConsoleUiHandler.
* @internalComponent
*
*/

 

#ifndef _CONSOLE_UIHANDLER_H_
#define _CONSOLE_UIHANDLER_H_

#include <swi/msisuihandlers.h>
#include "ciohandler.h"

// Forward Declaration
class CPreferenceHandler;

class CConsoleUiHandler:public CBase, public Swi::MUiHandler
	{
public:
	static CConsoleUiHandler* NewL(const CPreferenceHandler&);
	static CConsoleUiHandler* NewLC(const CPreferenceHandler&);
	
public:
	// Members from MCommonDialogs
	virtual TBool DisplayTextL(
	 				const Swi::CAppInfo& aAppInfo, 
					Swi::TFileTextOption aOption,
					const TDesC& aText);
	 
	virtual void DisplayErrorL(
					const Swi::CAppInfo& aAppInfo,
					Swi::TErrorDialog aType, 
					const TDesC& aParam);

	virtual TBool DisplayDependencyBreakL(
					const Swi::CAppInfo& aAppInfo,
					const RPointerArray<TDesC>& aComponents);
		
	virtual TBool DisplayApplicationsInUseL(
					const Swi::CAppInfo& aAppInfo, 
					const RPointerArray<TDesC>& aAppNames);
	
	virtual TBool DisplayQuestionL(
					const Swi::CAppInfo& aAppInfo, 
					Swi::TQuestionDialog aQuestion, 
					const TDesC& aDes=KNullDesC);
	
	//Members from MInstallerUiHandler
	virtual TBool DisplayInstallL(
					const Swi::CAppInfo& aAppInfo,
					const CApaMaskedBitmap* aLogo,
					const RPointerArray<Swi::CCertificateInfo>& aCertificates); 
	
	virtual TBool DisplayGrantCapabilitiesL(
					const Swi::CAppInfo& aAppInfo, 
					const TCapabilitySet& aCapabilitySet);
										
	virtual TInt DisplayLanguageL(
					const Swi::CAppInfo& aAppInfo, 
					const RArray<TLanguage>& aLanguages);
	
	virtual TInt DisplayDriveL(
					const Swi::CAppInfo& aAppInfo,				   
					TInt64 aSize,
					const RArray<TChar>& aDriveLetters,
					const RArray<TInt64>& aDriveSpaces);
	
	virtual TBool DisplayUpgradeL(
					const Swi::CAppInfo& aAppInfo,
					const Swi::CAppInfo& aExistingAppInfo);
	
	virtual TBool DisplayOptionsL(
					const Swi::CAppInfo& aAppInfo,
					const RPointerArray<TDesC>& aOptions,
					RArray<TBool>& aSelections);
	
	virtual TBool HandleInstallEventL(
					const Swi::CAppInfo& aAppInfo,
					Swi::TInstallEvent aEvent, 
					TInt aValue=0, 
					const TDesC& aDes=KNullDesC);

	virtual void HandleCancellableInstallEventL(
					const Swi::CAppInfo& aAppInfo,
					Swi::TInstallCancellableEvent aEvent,
					Swi::MCancelHandler& aCancelHandler,
					TInt aValue=0,
					const TDesC& aDes=KNullDesC);

	virtual TBool DisplaySecurityWarningL(
					const Swi::CAppInfo& aAppInfo,
					Swi::TSignatureValidationResult aSigValidationResult,
					RPointerArray<CPKIXValidationResultBase>& aPkixResults,
					RPointerArray<Swi::CCertificateInfo>& aCertificates,
					TBool aInstallAnyway);
	
	virtual TBool DisplayOcspResultL(
					const Swi::CAppInfo& aAppInfo,
					Swi::TRevocationDialogMessage aMessage,
					RPointerArray<TOCSPOutcome>& aOutcomes,
					RPointerArray<Swi::CCertificateInfo>& aCertificates,
					TBool aWarningOnly);
	
	virtual void DisplayCannotOverwriteFileL(
					const Swi::CAppInfo& aAppInfo,
					const Swi::CAppInfo& aInstalledAppInfo,
					const TDesC& aFileName);
	
	virtual TBool DisplayMissingDependencyL(
					const Swi::CAppInfo& aAppInfo,
					const TDesC& aDependencyName,
					TVersion aWantedVersionFrom,
					TVersion aWantedVersionTo,
					TVersion aInstalledVersion);

	//Members from MUninstallerUiHandler
	virtual TBool DisplayUninstallL(
					const Swi::CAppInfo& aAppInfo);
					
private: // Private Member Functions
	// This function parses the option list and fills the boolean array
	TInt ProcessOptionList(
					const RArray<TInt>&, 
					RArray<TBool>&) const;
	void DisplayAppInfoL(
					const Swi::CAppInfo& aAppInfo,
					TIoType aIoType) const;
	void DisplayCapbilitiesL(
					const TCapabilitySet& aCapabilitySet,
					TIoType aIoType) const;
	void DisplayLanguageListL(
					const RArray<TLanguage>& aLanguages,
					TIoType aIoType) const;
	void DisplayCertificateListL(
					const RPointerArray<Swi::CCertificateInfo>& aCertificates,
					TIoType aIoType) const;
	void DisplayCertificateL(
					const Swi::CCertificateInfo& aCertificates,
					TIoType aIoType) const;
	void DisplayPkixResultsL(
					RPointerArray<CPKIXValidationResultBase>&, 
					RPointerArray<Swi::CCertificateInfo>&,
					TIoType aIoType) const;
	void DisplayOcspDetailsL(
					RPointerArray<TOCSPOutcome>&, 
					RPointerArray<Swi::CCertificateInfo>&,
					TIoType aIoType) const;

private: // Private Constructors
	CConsoleUiHandler(const CPreferenceHandler&);

private: // Private Member Variables
	const CPreferenceHandler&		iPreferenceHandler;
	};


#endif //_CONSOLE_UIHANDLER_H_