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


/********************************************************
 *                                                      *
 *   WARNING - WORK-IN-PROGRESS - THIS API MAY CHANGE   *
 *                                                      *
 ********************************************************/

#ifndef C_SIFUI_H
#define C_SIFUI_H

#include <e32base.h>                            // CBase

class CSifUiPrivate;
class CApaMaskedBitmap;
class CSifUiCertificateInfo;
class CSifUiAppInfo;

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
        CSifUi::~CSifUi();

    public:     // new functions
        /**
         * Displays main installation confirmation query and waits for user response.
         * Returns ETrue if user accepted the query. The next ShowProgressL() call
         * changes the dialog to installation progress note. And finally, ShowFailedL()
         * or ShowCompleteL() changes the dialog to the final error or complete note.
         * If the installation confirmation query should contain memory selection option,
         * then set the selectable drives with SetMemorySelectionL() first. User selected
         * drive can be retrieved with SelectedDrive().
         * @param aAppInfo - application information (name, size, version, vendor, icon)
         * @return TBools - ETrue if user accepted the query, EFalse otherwise
         */
        IMPORT_C TBool ShowConfirmationL( const CSifUiAppInfo& aAppInfo );

        /**
         * Defines memory selection alternatives for the main installation
         * confirmation query displayed with ShowConfirmationL() function.
         * @param aDriveNumbers - options for memory selection
         */
        IMPORT_C void SetMemorySelectionL( const RArray<TInt>& aDriveNumbers );

        /**
         * Gets the selected drive where new component should be installed.
         * Use RFs::DriveToChar() to convert the drive number to drive letter.
         * @param aDriveNumber - selected drive number
         * @return TInt - KErrNone if successful, otherwise Symbian error code
         */
        IMPORT_C TInt SelectedDrive( TInt& aDriveNumber );

        /**
         * Defines certificate details for the main installation confirmation
         * query displayed with ShowConfirmationL() function.
         * @param aCertificates - certificate details
         */
        IMPORT_C void SetCertificateInfoL(
                const RPointerArray<CSifUiCertificateInfo>& aCertificates );

        /**
         * Displays main installation progress note. If the progress note or main
         * confirmation query is already displayed, then updates the dialog content.
         * Use IncreaseProgressBarValueL() to increase the progress bar value.
         * Dialog remains on the screen after progress bar shows full 100% value.
         * Use ShowFailedL() or ShowCompleteL() to replace the dialog content
         * with the final error or complete note.
         * @param aAppInfo - application information (name, size, version, vendor, icon)
         * @param aProgressBarFinalValue - final value of the progress bar
         */
        IMPORT_C void ShowProgressL( const CSifUiAppInfo& aAppInfo, TInt aProgressBarFinalValue );

        /**
         * Updates the progress bar value displayed in progress note. Initially progress bar
         * shows 0%. Each aIncrement increases the value displayed in progress bar. When all
         * increments reach the final value defined in ShowProgressNoteL() method, then the
         * progress bar shows full 100%.
         * @param aIncrement - progress bar value increment
         */
        IMPORT_C void IncreaseProgressBarValueL( TInt aIncrement );

        /**
         * Displays main installation complete note. Installation complete note contains
         * button to launch the application libaray to show the recently installed apps.
         */
        IMPORT_C void ShowCompleteL();

        /**
         * Displays main installation error note. Installation error note contains button
         * to see detailed error message when available.
         * @param aErrorCode - error code
         * @param aErrorMessage - localized error message to be displayed
         * @param aErrorDetails - localized error message details (if any)
         */
        IMPORT_C void ShowFailedL( TInt aErrorCode, const TDesC& aErrorMessage,
            const TDesC& aErrorDetails = KNullDesC );



        /** DEPRECATED -- DO NOT USE -- WILL BE REMOVED */
        enum TMode {
            EUnspecified = 0,
            EInstalling = 1,
            EUninstalling = 2
        };
        IMPORT_C void SetMode( TMode aMode );
        IMPORT_C TMode Mode();
        IMPORT_C void ShowFailedL( TInt aErrorCode );
        IMPORT_C void SetCertificateInfoL(
                const RPointerArray<Swi::CCertificateInfo>& aCertificates );
        IMPORT_C TBool ShowConfirmationL( const Swi::CAppInfo& aAppInfo,
                TInt aAppSize = 0, const CApaMaskedBitmap* aAppIcon = NULL );
        IMPORT_C void ShowProgressL( const Swi::CAppInfo& aAppInfo,
                 TInt aAppSize, TInt aProgressBarFinalValue );

    private:    // new functions
        CSifUi();
        void ConstructL();

    private:    // data
        CSifUiPrivate* iPrivate;
    };


#endif  // C_SIFUI_H

