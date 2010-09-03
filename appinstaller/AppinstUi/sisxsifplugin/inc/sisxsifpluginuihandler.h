/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UI Handler for normal (non-silent) install/uninstall operations.
*
*/

#ifndef C_SISXSIFPLUGUIHANDLER_H
#define C_SISXSIFPLUGUIHANDLER_H

#include <e32base.h>                    // CBase
#include "sisxsifpluginuihandlerbase.h" // CSisxSifPluginUiHandlerBase

class CSifUi;
class CSifUiAppInfo;
class CSisxSifUiSelectionCache;
enum CSifUi::TInstallingPhase;


namespace Usif
{
    /**
     * UI Handler for normal (non-silent) install/uninstall operations.
     */
    class CSisxSifPluginUiHandler : public CSisxSifPluginUiHandlerBase
        {
    public:     // constructors and destructor
        static CSisxSifPluginUiHandler* NewL( RFs& aFs,
                CSisxSifPluginErrorHandler& aErrorHandler );
        ~CSisxSifPluginUiHandler();

    public:     // from MUiHandler (via CSisxSifPluginUiHandlerBase)
        // from MCommonDialogs
        TBool DisplayTextL( const Swi::CAppInfo& aAppInfo, Swi::TFileTextOption aOption,
                const TDesC& aText );
        void DisplayErrorL( const Swi::CAppInfo& aAppInfo, Swi::TErrorDialog aType,
                const TDesC& aParam );
        TBool DisplayDependencyBreakL( const Swi::CAppInfo& aAppInfo,
                const RPointerArray<TDesC>& aComponents );
        TBool DisplayApplicationsInUseL( const Swi::CAppInfo& aAppInfo,
                const RPointerArray<TDesC>& aAppNames );
        TBool DisplayQuestionL( const Swi::CAppInfo& aAppInfo, Swi::TQuestionDialog aQuestion,
                const TDesC& aDes );

        // from MInstallerUiHandler
        TBool DisplayInstallL( const Swi::CAppInfo& aAppInfo, const CApaMaskedBitmap* aLogo,
                const RPointerArray<Swi::CCertificateInfo>& aCertificates );
        TBool DisplayGrantCapabilitiesL( const Swi::CAppInfo& aAppInfo,
                const TCapabilitySet& aCapabilitySet );
        TInt DisplayLanguageL( const Swi::CAppInfo& aAppInfo,
                const RArray<TLanguage>& aLanguages );
        TInt DisplayDriveL( const Swi::CAppInfo& aAppInfo, TInt64 aSize,
                const RArray<TChar>& aDriveLetters, const RArray<TInt64>& aDriveSpaces );
        TBool DisplayUpgradeL( const Swi::CAppInfo& aAppInfo,
                const Swi::CAppInfo& aExistingAppInfo );
        TBool DisplayOptionsL( const Swi::CAppInfo& aAppInfo,
                const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections );
        TBool HandleInstallEventL( const Swi::CAppInfo& aAppInfo, Swi::TInstallEvent aEvent,
                TInt aValue, const TDesC& aDes );
        void HandleCancellableInstallEventL( const Swi::CAppInfo& aAppInfo,
                Swi::TInstallCancellableEvent aEvent, Swi::MCancelHandler& aCancelHandler,
                TInt aValue, const TDesC& aDes );
        TBool DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
                Swi::TSignatureValidationResult aSigValidationResult,
                RPointerArray<CPKIXValidationResultBase>& aPkixResults,
                RPointerArray<Swi::CCertificateInfo>& aCertificates, TBool aInstallAnyway );
        TBool DisplayOcspResultL( const Swi::CAppInfo& aAppInfo,
                Swi::TRevocationDialogMessage aMessage,
                RPointerArray<TOCSPOutcome>& aOutcomes,
                RPointerArray<Swi::CCertificateInfo>& aCertificates, TBool aWarningOnly );
        void DisplayCannotOverwriteFileL( const Swi::CAppInfo& aAppInfo,
                const Swi::CAppInfo& aInstalledAppInfo, const TDesC& aFileName );
        TBool DisplayMissingDependencyL( const Swi::CAppInfo& aAppInfo,
                const TDesC& aDependencyName, TVersion aWantedVersionFrom,
                TVersion aWantedVersionTo, TVersion aInstalledVersion );

        // from MUninstallerUiHandler
        TBool DisplayUninstallL( const Swi::CAppInfo& aAppInfo );

    public:     // from CSisxSifPluginUiHandlerBase
        void DisplayPreparingInstallL( const TDesC& aFileName );
        void DisplayCompleteL();
        void DisplayFailedL( const CSisxSifPluginErrorHandler& aError );
        void CancelDialogs();

    public:     // new functions
        void SetDriveSelectionRequired( TBool aIsRequired );

    private:    // new functions
        CSisxSifPluginUiHandler( RFs& aFs, CSisxSifPluginErrorHandler& aErrorHandler );
        void ConstructL();
        void AddMemorySelectionL();
        void AddCertificatesL( RPointerArray<Swi::CCertificateInfo>& aCertificates,
                RPointerArray<CPKIXValidationResultBase>& aPkixResults );
        CSifUiAppInfo* GetAppInfoLC( const Swi::CAppInfo& aAppInfo );
        void ShowProgressL( const Swi::CAppInfo& aAppInfo, TInt aProgressBarFinalValue,
                CSifUi::TInstallingPhase aPhase );
        TBool ShowConfirmationL( const Swi::CAppInfo& aAppInfo );

    private:    // data
        CSifUi* iSifUi;
        CSisxSifUiSelectionCache* iSelectionCache;
        CApaMaskedBitmap* iLogo;
        TBool iQuestionIncompatibleDisplayed;
        RArray<TInt> iSelectableDrives;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGUIHANDLER_H

