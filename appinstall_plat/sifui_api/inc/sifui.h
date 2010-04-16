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
* Description:  CSifUi API can be used to implement UI dialogs for
*               SIF (Software Install Framework) plugins.
*/


#ifndef C_SIFUI_H
#define C_SIFUI_H

#include <e32base.h>                            // CActive
#include <hb/hbcore/hbsymbiandevicedialog.h>    // MHbDeviceDialogObserver



/******************************************************************
 *                                                                *
 *   WARNING - WORK-IN-PROGRESS - THIS API CHANGES ALL THE TIME   *
 *                                                                *
 ******************************************************************/



class MSifUiCertificateDetailsProvider;
class MSifUiDrmDetailsProvider;
class CHbDeviceDialog;
class CHbSymbianVariantMap;
class CActiveSchedulerWait;
class CApaMaskedBitmap;

namespace Swi {
    class CAppInfo;
    class CCertificateInfo;
}

/**
 * CSifUi provides UI dialogs for SW installation. UI dialogs are
 * global and they can be used in a non-UI code (like in SIF plugins).
 * SW install device dialog plugin implements the UI dialogs.
 *
 * @lib SifUi.lib
 * @since 10.1
 */
class CSifUi : public CActive, public MHbDeviceDialogObserver
    {
    public:     // constructor and destructor
        /**
         * Creates new CSifUi object and pushes it into cleanup stack.
         * @returns CSifUi* -- new CSifUi object
         */
        IMPORT_C static CSifUi* NewLC();

        /**
         * Creates new CSifUi object.
         * @returns CSifUi* -- new CSifUi object
         */
        IMPORT_C static CSifUi* NewL();

        /**
         * Destructor.
         */
        CSifUi::~CSifUi();

    public:     // new functions
        /**
         * Defines installation or uninstallation mode.
         */
        enum TMode {
            EUnspecified = 0,
            EInstalling = 1,
            EUninstalling = 2
        };

        /**
         * Defines installing or uninstalling mode. Selected mode defines
         * the dialog titles and buttons.
         * @param aMode - installing or uninstalling mode
         */
        IMPORT_C void SetMode( TMode aMode );

        /**
         * Returns the currently selected mode.
         * @return TMode - installing or uninstalling mode
         */
        IMPORT_C TMode Mode();

        /**
         * Displays main installation/uninstallation confirmation query and waits
         * for user response. Returns ETrue if user accepted the query. Confirmation
         * dialog is left on the screen. Subsequent ShowProgressL() call replaces it's
         * content with main progress note, and ShowFailedL() or ShowCompleteL() calls
         * replace it's content with the error or complete dialogs.
         * @param aAppInfo - application information (name, version, and vendor)
         * @param aAppSize - application size in bytes (not displayed if zero)
         * @param aAppIcon - application icon, default icon is displayed if NULL
         * @param aCertificates - certificate details
         * @return TBools - ETrue if user accepted the query, EFalse otherwise
         */
        IMPORT_C TBool ShowConfirmationL( const Swi::CAppInfo& aAppInfo,
                TInt aAppSize, const CApaMaskedBitmap* aAppIcon,
                const RPointerArray<Swi::CCertificateInfo>& aCertificates );

        /**
         * Displays main installation/uninstallation progress note. If progress note
         * or main confirmation query is already displayed, then updates the dialog
         * content. Use IncreaseProgressBarValueL() to increase the progress bar value.
         * Dialog remains still on the screen after progress bar shows full 100% value.
         * Use ShowFailedL() or ShowCompleteL() to replace the dialog content with the
         * final error or complete note.
         * @param aAppInfo - application information (name, version, and vendor)
         * @param aAppSize - application size in bytes (not displayed if zero)
         * @param aProgressBarFinalValue - final value of the progress bar
         */
        IMPORT_C void ShowProgressL( const Swi::CAppInfo& aAppInfo,
                TInt aAppSize, TInt aProgressBarFinalValue );

        /**
         * Updates the progress bar value displayed in progress note. Initially progress bar
         * shows 0%. Each aIncrement increases the value displayed in progress bar. When all
         * increments reach the final value defined in ShowProgressNoteL() method, then the
         * progress bar shows full 100%.
         * @param aIncrement - progress bar value increment
         */
        IMPORT_C void IncreaseProgressBarValueL( TInt aIncrement );

        /**
         * Displays main installation/uninstallation complete note. Installation complete
         * note contains button to launch application libaray.
         */
        IMPORT_C void ShowCompleteL();

        /**
         * Displays main installation/uninstallation error note. Installation error note
         * contains button to see detailed error message.
         * @param aErrorCode - error code
         */
        IMPORT_C void ShowFailedL( TInt aErrorCode );

    protected:  // from CActive
        void DoCancel();
        void RunL();

    private:    // from MHbDeviceDialogObserver
        void DataReceived( CHbSymbianVariantMap& aData );
        void DeviceDialogClosed( TInt aCompletionCode );

    private:    // new functions
        CSifUi();
        void ConstructL();
        void ClearParamsL();
        void ChangeNoteTypeL( TInt aType );
        void AddParamL( const TDesC& aKey, TInt aValue );
        void AddParamL( const TDesC& aKey, const TDesC& aValue );
        void AddParamsAppInfoAndSizeL( const Swi::CAppInfo& aAppInfo, TInt aAppSize );
        void AddParamsIconL( const CApaMaskedBitmap* aIcon );
        void AddParamsCertificatesL( const RPointerArray<Swi::CCertificateInfo>& aCertificates );
        void DisplayDeviceDialogL();
        TInt WaitForResponse();
        void ResponseReceived( TInt aCompletionCode );

    private:    // data
        TMode iMode;
        CHbDeviceDialog* iDeviceDialog;
        CHbSymbianVariantMap* iVariantMap;
        CActiveSchedulerWait* iWait;
        TBool iIsDisplayingDialog;
        TInt iCompletionCode;
        TInt iReturnValue;
        CApaMaskedBitmap* iBitmap;
    };


#endif  // C_SIFUI_H

