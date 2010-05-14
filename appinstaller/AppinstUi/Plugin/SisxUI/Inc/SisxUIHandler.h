/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SISXUIHANDLER_H
#define SISXUIHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <swi/msisuihandlers.h>
#include <CUIDetailsDialog.h>
#include <f32file.h>

#include "CUIDialogs.h"
#include "SisxUIResult.h"

namespace SwiUI
{

// FORWARD DECLARATIONS
class CSisxUISelectionCache;
class CSisxUIAppInfo;

/**
* Enumeration for ocsp procedure.
*
* @since 3.0
*/
enum TSisxUIOcspProc
    {
    ESisxUIOcspProcOff = 0,
    ESisxUIOcspProcOn,
    ESisxUIOcspProcMust
    };

/**
* This interface defines an observer for user initiated cancel.
*
* @since 3.0
*/
class MSisxUICancelObserver
    {
    public:
       
        /**
        * Called when user has cancelled the operation. 
        * @since 3.0
        */       
        virtual void UserCancelL( TBool aCancelImmediately ) = 0;        
    };

/**
* This class handles the Ui for native installations.
*
* @lib sisxui.lib
* @since 3.0
*/
class CSisxUIHandler : public CBase,
                       public Swi::MUiHandler,
                       public CommonUI::MCUIWaitDialogCallback
    {
    public:  // Constructors and destructor
    
        /**
        * Enumeration for install mode.
        * @since 3.0
        */
        enum TInstallMode
        {
        EModeInstall,
        EModeUninstall
        };        

        /**
        * Two-phased constructor.
        * @param aMode - Indicates if this is for install or uninstall
        */
        static CSisxUIHandler* NewL( TInstallMode aMode, MSisxUICancelObserver* aCancelObserver );
        
        /**
        * Destructor.
        */
        virtual ~CSisxUIHandler();
        
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

        /**
        * From, MCUIWaitDialogCallback, called when wait/progress dialog is about
        * to be cancelled.
        */
        TBool HandleDialogExitL( TInt aButtonId );
        
    public:  // New functions
        
        /**
        * Process the (un)installation result.
        * @since 3.0
        * @param aResult - Result from the (un)installation engine.
        */
        void ProcessResult( TInt aResult );

        /**
        * Return result of the installation.
        * @since 3.0
        * @return Result of the installation.
        */
        TSisxUIResult GetResult() const;

        /**
        * Show the final note.
        * @since 3.0
        */        
        virtual void ShowFinalNoteL();        

        /**
        * Return the package information about this install/uninstall. Ownership
        * does not transfer.
        * @since 3.0
        * @param aPackages - On return contains the package information.
        */
        void ProcessedPackages( RPointerArray<CSisxUIAppInfo>& aPackages );

        /**
        * Return the install mode of this instance.
        * @since 3.0
        * @return Install mode.
        */
        TInstallMode InstallMode() const;        

        /**
        * Sets the ocsp procedure for the installation.
        * @since 3.0
        * @param aOcspProc - Ocsp procedure.
        */
        void SetOcspProcedure( const TSisxUIOcspProc& aOcspProc );

        /**
        * Indicates if a dialog is shown to user and expecting some user input.
        * @since 3.0
        */
        TBool IsShowingDialog();

        /**
        * Shows a cancelling wait dialog.
        * @since 3.0
        */
        void ShowCancellingL();   

        /**
        * Sets the UI not to show final note.
        * @since 3.0
        */
        void DoNotShowFinalNote();   

        /**
        * Resets the result to no result.
        * @since 3.0
        */
        void ResetResult();        

        /**
        * Sets the file handle for showing drm details.
        * @since 3.0
        * @param aFile - File handle.
        */
        void SetFileForDrm( RFile& aFile );        

        /**
        * Sets the policy for allowing the installation of untrusted packages.
        * @since 3.0
        * @param aAllowUntrusted - True if untrusted is allowed.
        */
        void SetAllowUntrusted( TBool aAllowUntrusted );        

    protected:
        
        /**
        * Constructor.
        * @param aMode - Indicates if this is for install or uninstall
        */  
        CSisxUIHandler( TInstallMode aMode, MSisxUICancelObserver* aCancelObserver );

    private:    

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Convenience function for showing a Sisx specific error.
        * @since 3.0
        * @param aResourceId - Resource id of the error string.
        */
        void ShowErrorL( TInt aResourceId );

        /**
        * Convenience function for showing a Sisx specific error.
        * @since 3.0
        * @param aDes - String to be shown in error note.
        */
        void ShowErrorL( const TDesC& aDes );        
        
        /**
        * Adds the given capability string to the descriptor. 
        * @since 3.0
        * @param aString - The descriptor where the capability string is added.
        *                  This must be on top of the cleanup stack when the function
                           is called.
        * @param aResourceId - Resource id of the capability string.
        */
        void AddCapabilityL( HBufC*& aString, TInt aResourceId ); 

        /**
        * Displays a cancel note.
        * @since 3.0
        */
        void DisplayCancelL();        

        /**
        * Displays a complete note.
        * @since 3.0
        */
        void DisplayCompleteL();

        /**
        * Finalizes the installation by closing all open dialogs. 
        * @since 3.0
        */
        void Finalize();

        /**
        * Callback used in message query. 
        * @since 3.0
        */
        static TInt ShowCapabilitiesHelp( TAny* aPtr );        

    protected:  // Data
             
        CommonUI::CCUIDialogs* iCommonDialogs; // Owned
        RPointerArray<CommonUI::CCUICertificateInfo> iCertificateArray;  

        TInstallMode iMode;  // Indicates whether we have install or uninstall in progress
        TSisxUIOcspProc iOcspProc;
        TBool iShownFinalNote;
        TBool iIntegrityCheckDone; 
        TBool iOcspInProgress;    
        TBool iIncompatibleShown; 
        TBool iShowingDialog;        

        CSisxUISelectionCache* iCache;

        TSisxUIResult iResult;
        RPointerArray<CSisxUIAppInfo> iPackages;  // Owned   

        MSisxUICancelObserver* iCancelObserver;  
        TBool iCancelRequested;           

        TBool iAllowUntrusted;        

    private:

        RFile iDrmFile; 
        TBool iIsDrmFileSet; 
        TBool iShowingWaitDialog;        
    };

}

#endif      // SISXUIHANDLER_H   
            
// End of File
