/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains the header file of the CSWSisUIHandler
*                class.
*
*                This class handles the Ui for native installations.
*
*/


#ifndef SISXUISILENTHANDLER_H
#define SISXUISILENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <swi/msisuihandlers.h>
//#include <CUIDetailsDialog.h>

//#include "CUIDialogs.h"
#include "SisxUIResult.h"
#include "SisxUIHandler.h"
#include "SWInstPrivateUid.h"

namespace SwiUI
{

/**
* This class handles the Ui for native installations.
*
* @lib sisxui.lib
* @since 3.0
*/
class CSisxUISilentHandler : public CSisxUIHandler 
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aMode - Indicates if this is for install or uninstall
        */
        static CSisxUISilentHandler* NewL( const TInstallOptions &aOptions, MSisxUICancelObserver* aCancelObserver );
        static CSisxUISilentHandler* NewL( const TUninstallOptions &aOptions, MSisxUICancelObserver* aCancelObserver );
        
        /**
        * Destructor.
        */
        virtual ~CSisxUISilentHandler();
        
    public:  // From base classes
        
        /**
        * From MUiHandler, displays a dialog at the beginning of an installation 
        * with details about the SISX package being installed. This dialog must 
        * be shown in TrustedUI mode.
        */
        TBool DisplayInstallL( const Swi::CAppInfo& aAppInfo,
                               const CApaMaskedBitmap* aLogo,
                               const RPointerArray<Swi::CCertificateInfo>& aCertificates ); 
	
        /**
        * From MUiHandler, displays a dialog requesting that the user grants 
        * these capabilities to the SISX package being installed. This dialog 
        * must be shown in TrustedUI mode.
        */
        TBool DisplayGrantCapabilitiesL( const Swi::CAppInfo& aAppInfo, 
                                         const TCapabilitySet& aCapabilitySet );
										
        /**
        * From MUiHandler, requests preferred language and dialect information 
        * from the UI.
        */
        TInt DisplayLanguageL( const Swi::CAppInfo& aAppInfo, 
                               const RArray<TLanguage>& aLanguages );
	
        /**
        * From MUiHandler, requests the drive to install the package on, from 
        * the UI.
        */
        TInt DisplayDriveL( const Swi::CAppInfo& aAppInfo, 
                            TInt64 aSize,
                            const RArray<TChar>& aDriveLetters,
                            const RArray<TInt64>& aDriveSpaces );
	
        /**
        * From MUiHandler, displays a dialog asking whether we want to upgrade an 
        * application present on the device.
        */
        TBool DisplayUpgradeL( const Swi::CAppInfo& aAppInfo, 
                               const Swi::CAppInfo& aExistingAppInfo );
	
        /**
        * From MUiHandler, displays a dialog box asking which options the user 
        * wishes to install.
        */
        TBool DisplayOptionsL( const Swi::CAppInfo& aAppInfo,
                               const RPointerArray<TDesC>& aOptions,
                               RArray<TBool>& aSelections );
        
        /**
        * From MUiHandler, informs the UI of events occurring during installation.  
        */
        TBool HandleInstallEventL( const Swi::CAppInfo& aAppInfo,
                                  Swi::TInstallEvent aEvent, 
                                  TInt aValue = 0, 
                                  const TDesC& aDes = KNullDesC );
        /**
        * From MUiHandler, informs the UI of cancellable events occurring during 
        * installation.  
        */
        void HandleCancellableInstallEventL( const Swi::CAppInfo& aAppInfo,
                                             Swi::TInstallCancellableEvent aEvent, 
                                             Swi::MCancelHandler& aCancelHandler,
                                             TInt aValue = 0, 
                                             const TDesC& aDes = KNullDesC );
        /**
        * From MUiHandler, displays a dialog indicating that the package is unsigned, 
        * or that signature validation has failed.
        */
        TBool DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
                                       Swi::TSignatureValidationResult aSigValidationResult,
                                       RPointerArray<CPKIXValidationResultBase>& aPkixResults,
                                       RPointerArray<Swi::CCertificateInfo>& aCertificates,
                                       TBool aInstallAnyway );
	
        /**
        * From MUiHandler, displays a dialog indicating that there was an error 
        * carrying out online certificate status checking.
        */
        TBool DisplayOcspResultL( const Swi::CAppInfo& aAppInfo,
                                  Swi::TRevocationDialogMessage aMessage,
                                  RPointerArray<TOCSPOutcome>& aOutcomes, 
                                  RPointerArray<Swi::CCertificateInfo>& aCertificates,
                                  TBool aWarningOnly );
	
        /**
        * From MUiHandler, used to inform the user the installation they are attempting 
        * is trying to overwrite a file that does not belong to it. Following this 
        * message installation is aborted.
        */
        void DisplayCannotOverwriteFileL( const Swi::CAppInfo& aAppInfo,
                                          const Swi::CAppInfo& aInstalledAppInfo,
                                          const TDesC& aFileName );
       
        /**
        * Used to inform the user of a missing dependency needed to install the 
        * current package. It is up to the UI implementors and/or the user whether
        * or not this warning can be overridden.
        */
		TBool DisplayMissingDependencyL( const Swi::CAppInfo& aAppInfo,
                                         const TDesC& aDependencyName,
                                         TVersion aWantedVersionFrom,
                                         TVersion aWantedVersionTo,
                                         TVersion aInstalledVersion );
        
        /**
        * From MUiHandler, displays a dialog at the start of the uninstallation with 
        * information about the application to be uninstalled. The user may cancel the 
        * uninstallation at this stage. This dialog must be shown in TrustedUI mode.
        */
        TBool DisplayUninstallL( const Swi::CAppInfo& aAppInfo );

        /**
        * From MUiHandler, displays a dialog containing text. This dialog has only a 
        * Continue button if aOption is EInstFileTextOptionContinue, or Yes and No buttons 
        * otherwise.
        */
        TBool DisplayTextL( const Swi::CAppInfo& aAppInfo, 
                            Swi::TFileTextOption aOption,
                            const TDesC& aText );
        
        /**
        * From MUiHandler, displays a dialog showing an error message. 
        */
        void DisplayErrorL( const Swi::CAppInfo& aAppInfo,
                            Swi::TErrorDialog aType, 
                            const TDesC& aParam );
        
        /**
        * From MUiHandler, displays a dialog indicating that a component which is being 
        * uninstalled is a dependency of another component installed.
        */
        TBool DisplayDependencyBreakL( const Swi::CAppInfo& aAppInfo,
                                       const RPointerArray<TDesC>& aComponents );
		
        /**
        * From MUiHandler, informs user that the applications are in use during an 
        * uninstallation. Return ETrue if the user wishes to continue the removal process and 
        * close the applications anyway.
        */
        TBool DisplayApplicationsInUseL( const Swi::CAppInfo& aAppInfo, 
                                         const RPointerArray<TDesC>& aAppNames );
	
        /**
        * From MUiHandler, displays a generic question dialog with a Yes/No answer.
        */
        TBool DisplayQuestionL( const Swi::CAppInfo& aAppInfo, 
                                Swi::TQuestionDialog aQuestion, 
                                const TDesC& aDes = KNullDesC );

        
    public:  // From CSisxUIHandler
        
        
        /**
        * Show the final note.
        * @since 3.0
        */        
        virtual void ShowFinalNoteL();        

         
    private:
        
        /**
        * Constructor.
        * @param aMode - Indicates if this is for install or uninstall
        */  
        CSisxUISilentHandler( const TInstallOptions &aOptions, MSisxUICancelObserver* aCancelObserver );
        
        CSisxUISilentHandler( const TUninstallOptions &aOptions, MSisxUICancelObserver* aCancelObserver );
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Constructor.
        * @aForeground - ETrue value brings Installer UI to foreground. EFalse sends it
        * to background. 
        */ 
        void BringToForeground( TBool aForeground );

    private:  // Data
             
        TInstallOptions iOptions;    
        TUninstallOptions iUninstallOptions;
    };

}

#endif      // SISXUIHANDLER_H   
            
// End of File
