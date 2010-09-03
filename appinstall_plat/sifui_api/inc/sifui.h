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


/**************************************************************
 *                                                            *
 *   WARNING - WORK-IN-PROGRESS - THIS API MAY STILL CHANGE   *
 *                                                            *
 **************************************************************/

#ifndef C_SIFUI_H
#define C_SIFUI_H

#include <e32base.h>                            // CBase
#include <bamdesca.h>                           // MDesCArray

class CSifUiPrivate;
class CApaMaskedBitmap;
class CSifUiCertificateInfo;
class CSifUiAppInfo;
class CSifUiErrorInfo;

namespace Swi {
    class CAppInfo;
    class CCertificateInfo;
}


/**
 * CSifUi provides UI dialogs for SW installation. UI dialogs are
 * global and they can be used in a non-UI code (like SIF plugins).
 * SW install device dialog plugin implements the UI dialogs.
 *
 * @lib sifui.lib
 * @since 10.1
 */
class CSifUi : public CBase
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
        ~CSifUi();

    public:     // new functions

        //=================================================
        // Preparing note method
        //=================================================

        /**
         * Displays installation dialog with "Preparing" text and indefinite progress bar.
         * No buttons are available. Use ShowConfirmationL() method to update the dialog
         * content to the installation confirmation query.
         */
        IMPORT_C void ShowPreparingL();


        //=================================================
        // Install confirmation query methods
        //=================================================

        /**
         * Defines memory selection alternatives for the ShowConfirmationL() installation
         * confirmation query. Use SelectedDrive() method to get the selected drive number
         * after ShowConfirmationL() call.
         * @param aDriveNumbers - options for memory selection
         */
        IMPORT_C void SetMemorySelectionL( const RArray<TInt>& aDriveNumbers );

        /**
         * Defines certificate details for the ShowConfirmationL() installation
         * confirmation query. If certificates are not set, then application is
         * untrusted.
         * @param aCertificates - certificate details
         */
        IMPORT_C void SetCertificateInfoL(
                const RPointerArray<CSifUiCertificateInfo>& aCertificates );

        /**
         * Displays installation confirmation query and waits for user response.
         * Returns ETrue if the user accepted the query. The next ShowProgressL() call
         * changes the dialog content to installation progress note. And finally,
         * ShowCompleteL() or ShowFailedL() call changes the dialog content to the
         * complete or error note.
         * If preparing installation note needs to be displayed before confirmation query,
         * then call ShowPreparingL() before any other functions.
         * If the installation confirmation query should contain memory selection option,
         * then set the selectable drives with SetMemorySelectionL() first. User selected
         * drive can be retrieved with SelectedDrive() after ShowConfirmationL() returns.
         * If the installation confirmation query should contain certificate details, then
         * set the certificate details using SetCertificateInfoL() first.
         * @param aAppInfo - application information (name, size, version, vendor, icon)
         * @return TBools - ETrue if the user accepted the query, EFalse otherwise
         */
        IMPORT_C TBool ShowConfirmationL( const CSifUiAppInfo& aAppInfo );

        /**
         * Gets the selected drive where new component should be installed.
         * Use RFs::DriveToChar() to convert the drive number to drive letter.
         * @param aDriveNumber - selected drive number
         * @return TInt - KErrNone if successful, otherwise Symbian error code
         */
        IMPORT_C TInt SelectedDrive( TInt& aDriveNumber );


        //=================================================
        // Progress note methods
        //=================================================

        /**
         * Installing phases in progress notes. Indicated in progress note title text.
         */
        enum TInstallingPhase {
            EInstalling,
            EDownloading,
            ECheckingCerts
        };

        /**
         * Displays installation progress note. Changes confirmation query content
         * to progress note. If the progress note is already displayed, then updates
         * the dialog content. Use IncreaseProgressBarValueL() method to increase the
         * progress bar value.
         * Dialog remains on the screen after progress bar shows full 100% value.
         * Use ShowCompleteL() or ShowFailedL() methods to show the result.
         * @param aAppInfo - application information (name, size, version, vendor, icon)
         * @param aProgressBarFinalValue - final value of the progress bar
         * @param aPhase - defines new title text for the progress note dialog
         */
        IMPORT_C void ShowProgressL( const CSifUiAppInfo& aAppInfo, TInt aProgressBarFinalValue,
                TInstallingPhase aPhase = EInstalling );

        /**
         * Updates the progress bar value displayed in progress note. Initially progress bar
         * shows 0%. Each aIncrement increases the value displayed in progress bar. When all
         * increments reach the final value defined in ShowProgressNoteL() method, then the
         * progress bar shows full 100%. Caller should check using IsCancelled() method if
         * the user has cancelled the progress note before updating the progress bar value.
         * @param aIncrement - progress bar value increment
         */
        IMPORT_C void IncreaseProgressBarValueL( TInt aIncrement );

        /**
         * Returns ETrue if the user has cancelled the progress dialog.
         * @return TBool - ETrue if the progress dialog has been cancelled
         */
        IMPORT_C TBool IsCancelled();


        //=================================================
        // Final complete and error notes
        //=================================================

        /**
         * Displays installation complete note and waits for the user to close it.
         * Installation complete note contains "Show" button to show installed
         * applications in application library. Use SetButtonVisible() method to
         * hide the "Show" button when necessary.
         */
        IMPORT_C void ShowCompleteL();

        /**
         * Displays installation error note and waits for user to close it.
         * Installation error note contains "Details" button to show the detailed
         * error message (when available). Also SetButtonVisible() method can be
         * used to hide the "Details" button if necessary.
         * @param aErrorCategory - error category
         * @param aErrorMessage - localized error message to be displayed
         * @param aErrorDetails - localized error message details (if any)
         * @param aErrorCode - error code
         * @param aErrorCode - error code
         */
        IMPORT_C void ShowFailedL( const CSifUiErrorInfo& aErrorInfo );


        //=================================================
        // Buttons in progress/complete/error notes
        //=================================================

        /**
         * Toolbar buttons in progress and complete notes that can be disabled/hidden.
         */
        enum TOptionalButton
            {
            EHideProgressButton,        // "Hide" button in progress note
            ECancelProgressButton,      // "Cancel" button in progress note
            EShowInAppLibButton,        // "Show" button in complete note
            EErrorDetailsButton         // "Details" button in error note
            };

        /**
         * Hides or shows optional toolbar buttons from installation progress note
         * or installation complete note. All optional buttos are visible by default.
         * Buttons visibility becomes effective in the next ShowProgressL(),
         * ShowCompleteL(), or IncreseProgressBarValueL() method calls.
         * @param aButton - button which visibility is changed
         * @param aIsVisible - EFalse if button needs to be hidden
         */
        IMPORT_C void SetButtonVisible( TOptionalButton aButton, TBool aIsVisible );


        //=================================================
        // Other query dialogs
        //=================================================

        /**
         * Displays dialog requesting capabilities for the application being installed
         * and waits for user response. Returns ETrue if the user granted the capabilities.
         * Other dialogs (like progress note) are not affected.
         * @param aCapabilities - requested user capabilities
         * @return TBool - ETrue if the user granted the capabilities, EFalse otherwise
         */
        IMPORT_C TBool ShowGrantCapabilitiesL( const TCapabilitySet& aCapabilities );

        /**
         * Displays pop-up language selection dialog with radio-buttons, and
         * waits for user response. Other displayed installation dialogs (like
         * progress note) are not affected. Returns integer that is the index
         * of the selected language in given langauge array, or error code.
         * Return value is KErrCancel if the user cancelled the query.
         * @param aLanguages - array of selectable languages
         * @return The selected item index to aLangauges array, or error code.
         */
        IMPORT_C TInt ShowSelectLanguageL( const RArray<TLanguage>& aLanguages );

        /**
         * Displays multi-selection pop-up dialog with checkboxes, and waits
         * for user response. Other displayed installation dialogs (like progress
         * note) are not affected. Returns ETrue if the user made the selection,
         * EFalse if the user cancelled the query. Selected indices are returned
         * in aSelectedIndexes array.
         * @param aSelectableItems - array of selectable items displayed
         * @param aSelectedIndexes - returns the selected item indices
         * @return TBool - ETrue if the user accepted the query, EFalse otherwise
         */
        IMPORT_C TBool ShowSelectOptionsL( const MDesCArray& aSelectableItems,
                RArray<TInt>& aSelectedIndexes );

        /**
         * Closes dialogs immediately and resets the CSifUi object.
         */
        IMPORT_C void CancelDialogs();


        //=================================================
        // DEPRECATED METHODS
        //=================================================

        // DEPRECATED, WILL BE REMOVED, DO NOT USE
        IMPORT_C void ShowFailedL( TInt aErrorCode, const TDesC& aErrorMessage,
            const TDesC& aErrorDetails = KNullDesC );
        IMPORT_C TBool ShowSingleSelectionL( const TDesC& aTitle,
            const MDesCArray& aSelectableItems, TInt& aSelectedIndex );
        IMPORT_C TBool ShowMultiSelectionL( const TDesC& aTitle,
            const MDesCArray& aSelectableItems, RArray<TInt>& aSelectedIndexes );


    private:    // new functions
        CSifUi();
        void ConstructL();

    private:    // data
        CSifUiPrivate* iPrivate;
    };


#endif  // C_SIFUI_H

