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
* uihandlers.h
*
*/


#ifndef __UIHANDLERS_H__
#define __UIHANDLERS_H__

#include "swi/msisuihandlers.h"
#include <e32test.h>
#include "consoleclientserver.h"

namespace Swi
{
namespace Test
 {
  class CancelAdapter : public MCancelHandler
  {
  public:
    void HandleCancel();    
  };

  class SisUiAdapter : public MUiHandler
  {
  private:
    TBool AskQuestion(TDesC& aQuestion);

  protected:
    SisUiAdapter(RConsoleServerSession& aConsole);
    void ConstructL();

  public:
    static SisUiAdapter* NewLC(RConsoleServerSession& aConsole);
    static SisUiAdapter* NewL(RConsoleServerSession& aConsole);

  public: // From MInstallHandler

    TBool DisplayInstallL(const CAppInfo& aAppInfo,
			  const CApaMaskedBitmap* aLogo,
			  const RPointerArray<CCertificateInfo>& aCertificates); 
		
    TBool DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo, 
				  const TCapabilitySet& aCapabilitySet);
    
    TInt DisplayLanguageL(const CAppInfo& aAppInfo, 
			  const RArray<TLanguage>& aLanguages);
    
    TInt DisplayDriveL(const CAppInfo& aAppInfo, 
			TInt64 aSize,
			const RArray<TChar>& aDriveLetters,
			const RArray<TInt64>& aDriveSpaces);
    	
    TBool DisplayUpgradeL(const CAppInfo& aAppInfo, 
			  const CAppInfo& aExistingAppInfo);
		
    TBool DisplayQuestionL(const CAppInfo& aAppInfo, TQuestionDialog aQuestion, const TDesC& aDes=KNullDesC);
		
    TBool DisplayOptionsL(const CAppInfo& aAppInfo,
			  const RPointerArray<TDesC>& aOptions,
			  RArray<TBool>& aSelections);
    
    TBool HandleInstallEventL(const CAppInfo& aAppInfo,
				 TInstallEvent aEvent, 
			     TInt aValue=0, 
			     const TDesC& aDes=KNullDesC);

    virtual void HandleCancellableInstallEventL(const CAppInfo& aAppInfo,
						TInstallCancellableEvent aEvent, 
						MCancelHandler& aCancelHandler,
						TInt aValue=0, 
						const TDesC& aDes=KNullDesC);
    
    TBool DisplaySecurityWarningL(const CAppInfo& aAppInfo,
				  TSignatureValidationResult aSigValidationResult,
				  RPointerArray<CPKIXValidationResultBase>& aPkixResults,
				  RPointerArray<CCertificateInfo>& aCertificates,
				  TBool aInstallAnyway);
		
//    TBool DisplayRevocationQueryL(TBool& aDoRevocationCheck);
    
    TBool DisplayOcspResultL(const CAppInfo& aAppInfo,
				 const TRevocationDialogMessage aMessage, 
			     RPointerArray<TOCSPOutcome>& aOutcomes, 
				 RPointerArray<CCertificateInfo>& aCertificates,
			     const TBool aWarningOnly);
    
    void DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
				     const CAppInfo& aInstalledAppInfo,
				     const TDesC& aFileName);    


  public: // From MUninstallHandler
    TBool DisplayUninstallL(const CAppInfo& aAppInfo);

  public:  // From MErrorDialogs

    TBool DisplayTextL(const CAppInfo& aAppInfo, 
		       TFileTextOption aOption,
		       const TDesC& aText);
    
    void DisplayErrorL(const CAppInfo& aAppInfo,
		       TErrorDialog aType, 
		       const TDesC& aParam);
    
		
    TBool DisplayDependencyBreakL(const CAppInfo& aAppInfo,
				  const RPointerArray<TDesC>& aComponents);
    
    TBool DisplayApplicationsInUseL(const CAppInfo& aAppInfo, const RPointerArray<TDesC>& aAppNames);  

	TBool DisplayMissingDependencyL(
		const CAppInfo& aAppInfo,
		const TDesC& aDependencyName,
		TVersion aWantedVersionFrom,
		TVersion aWantedVersionTo,
		TVersion aInstalledVersion);
		
  private:
    RConsoleServerSession& iConsole;  // We do not own this!
  };
  
   
 } // namespace Test

} // namespace Swi


#endif

enum KInstallerPanics
	{ 
	ESwiInstallerPanic
	};



