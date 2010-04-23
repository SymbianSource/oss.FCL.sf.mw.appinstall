/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CCUIDialogs class.
*
*                This class provides a wrapper for some common dialogs for the 
*                installer ui plugins.
*
*/


#ifndef CUIDIALOGS_H
#define CUIDIALOGS_H

//  INCLUDES
#include <e32base.h>
#include <avkon.rsg>
#include <AknIconArray.h>
#include <bamdesca.h>

//  FORWARD DECLARATIONS
class CCoeEnv;
class CX509Certificate;
class CAknInputBlock;

namespace SwiUI
{
namespace CommonUI
{

//  FORWARD DECLARATIONS
class CCUIWaitDialog;
class CCUIProgressDialog;
class MCUIInfoIterator;
class CCUICertificateInfo;

//  CLASS DECLARATIONS

/**
* A callback interface for handling common non modal dialog cancellation.
* 
* @since 3.0
*/
class MCUIWaitDialogCallback
    {
    public:
        
        /**
        * This is called when the dialog is about to be closed.
        * @since 3.0
        * @param aButtonId - Id of the button, which was used to cancel the dialog.
        * @return ETrue, if it's ok to close the dialog, EFalse otherwise.
        */
        virtual TBool HandleDialogExitL( TInt aButtonId ) = 0;    
    };

/**
* This class provides a wrapper for some common dialogs for the installer ui plugins.
*
* @lib swinstui.lib
* @since 3.0
*/
class CCUIDialogs : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CCUIDialogs* NewL();
    
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCUIDialogs();


    public: // New functions

        /**
        * Show a wait dialog. If any wait dialog is active, only the
        * dialog text will be changed. No events will be triggered.
        * @since 3.0
        * @param aResourceId - Resource id of the string to be displayed.
        * @param aCallback - Class to be informed about dialog dismissal.        
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        */
        IMPORT_C void ShowWaitDialogL( TInt aResourceId,
                                       MCUIWaitDialogCallback* aCallback,
                                       TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_CANCEL );

        /**
        * Closes the wait dialog. 
        * @since 3.0
        */
        IMPORT_C void CloseWaitDialogL();

        /**
        * Show a progress dialog. If any progress is active, only the
        * dialog text will be changed. No events will be triggered.
        * @since 3.0
        * @param aResourceId - Resource id of the string to be displayed.
        * @param aCallback - Class to be informed about dialog dismissal.
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        */
        IMPORT_C void ShowProgressDialogL( TInt aResourceId, 
                                           MCUIWaitDialogCallback* aCallback,
                                           TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_CANCEL );


        /**
        * Show a progress dialog. If any progress is active, only the
        * dialog text will be changed. No events will be triggered.
        * @since 3.0
        * @param aText - String to be displayed in the dialog.
        * @param aCallback - Class to be informed about dialog dismissal.
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        */
        IMPORT_C void ShowProgressDialogL( const TDesC& aText, 
                                           MCUIWaitDialogCallback* aCallback,
                                           TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_CANCEL );
        
        /**
        * Set final value for the progress bar in progress dialog.        
        * @since 3.0
        * @param aValue - Final value of the progress bar.
        */
        IMPORT_C void SetProgressDialogFinalValueL( TInt aValue );

        /**
        * Increments the progress bar in progress dialog.
        * @since 3.0
        * @param aValue - Value to be added to the total progress.
        */
        IMPORT_C void UpdateProgressDialogValueL( TInt aValue );

        /**
        * Closes the progress dialog. 
        * @since 3.0
        */
        IMPORT_C void CloseProgressDialogL();

        /**
        * Show a confirmation dialog. 
        * @since 3.0
        * @param aResourceId - Resource id of the string to be displayed.
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        * @return ETrue is user wishes to continue, otherwise EFalse.
        */
        IMPORT_C TBool ShowConfirmationQueryL( TInt aResourceId,
                                               TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_OK_CANCEL__OK ) const;

        /**
        * Show a confirmation dialog. 
        * @since 3.0
        * @param aDisplayString - The string to be displayed.
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        * @return ETrue is user wishes to continue, otherwise EFalse.
        */
        IMPORT_C TBool ShowConfirmationQueryL( const TDesC& aDisplayString,
                                               TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_OK_CANCEL__OK ) const;

        /**
        * Show a drive / memory selection dialog. 
        * @since 3.0
        * @param aSize - Size of the application to be installed.
        * @param aDriveUnits - Array of available drives as drive units.
        * @param aDriveSpaces - Array of free space on each of the drives in the
        *                       aDriveLetters array.
        * @param aSelectionResult - Selected index of the aDriveUnits array.        
        * @return KErrNone if everything is ok, otherwise KErrCancel if dialog was cancelled 
        *         or KErrNoMemory if no memory is available on the device.
        */
        IMPORT_C TInt ShowDriveSelectionDialogL( TInt64 aSize, 
                                                 const RArray<TDriveUnit>& aDriveUnits, 
                                                 const RArray<TInt64>& aDriveSpaces,
                                                 TInt& aSelectionResult );
        
        /**
        * Show a security warning dialog.
        * @since 3.0
        * @return ETrue is user to continue, EFalse if cancel.
        */
        IMPORT_C TBool ShowSecurityWarningDialogL();

        /**
        * Show an error note.
        * @since 3.0
        * @param aResourceId - Resource id of the error text.
        */
        IMPORT_C void ShowErrorNoteL( TInt aResourceId ) const;

        /**
        * Show an error note.
        * @since 3.0
        * @param aErrorText - Error text.
        */
        IMPORT_C void ShowErrorNoteL( const TDesC& aErrorText ) const;       

        /**
        * Resets the progress bar in the progress dialog.
        * @since 3.0
        */
        IMPORT_C void ResetProgressDialogL();        
        
        /**
        * Launches a help with the given context.
        * @since 3.0
        * @param aContext - The help text to show.
        */
        IMPORT_C void LaunchHelpL( const TDesC& aContext );   

        /**
        * Show a delayed wait dialog. If any wait dialog is active, only the
        * dialog text will be changed. No events will be triggered.
        * @since 3.0
        * @param aResourceId - Resource id of the string to be displayed.
        * @param aCallback - Class to be informed about dialog dismissal.        
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        */
        IMPORT_C void ShowDelayedWaitDialogL( TInt aResourceId,
                                              MCUIWaitDialogCallback* aCallback,
                                              TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_CANCEL );
                                              
        /**
        * Show an installation complete query. 
        * @since 3.2
        * @param aApplicationArray - An array for application names
        * @param aInstalledFolderArray - An array for installed folders. (default folder is used if
        *                                array is empty, folder names for zero length items are not shown)
        * @param aIconArray - An array for application icons
        * @param aIndexOfAppToBeStarted  - Returns index (starting from zero) of application 
        *                                  chosen to be started after installation
        * @return ETrue is user wants to start installed application, otherwise EFalse.
        */
        IMPORT_C TBool ShowInstallationCompleteQueryL( const MDesCArray& aApplicationArray,
                                                       const MDesCArray& aInstalledFolderArray,
                                                       const CAknIconArray& aIconArray,
                                                       TInt& aIndexOfAppToBeStarted ) const;
                                                       
        /**
        * Show an uninstall confirmation query when several 
        * applications (Java MIDLets) are uninstalled. 
        * @since 3.2
        * @param aApplicationArray - An array for application names
        * @param aApplicationGroup - Name application group (Java MIDlet suite)
        * @param aSoftkeyResourceId - Resource id of the softkeys.
        *
        * @return ETrue if an user continues uninstalling, EFalse if cancelled.
        */
        IMPORT_C TBool ShowUninstallConfirmationForSeveralAppsL(
                       const TDesC& aApplicationGroup,   
                       const MDesCArray& aApplicationArray, 
                       TInt aSoftkeyResourceId = R_AVKON_SOFTKEYS_YES_NO__YES) const;                                               
                                      

    private:
        
        /**
        * Constructor.
        */
        CCUIDialogs();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Sets the callback for wait dialog events. 
        * @since 3.0
        * @param aCallback - The callback class.
        */
        void SetWaitDialogCallback( MCUIWaitDialogCallback* aCallback );  

        /**
        * Sets the callback for progress dialog events. 
        * @since 3.0
        * @param aCallback - The callback class.
        */
        void SetProgressDialogCallback( MCUIWaitDialogCallback* aCallback );  

        /**
        * Used as a callback function in message query. Launches help for security
        * warning.
        * @since 3.0
        */
        static TInt ShowSecurityWarningHelp( TAny* aPtr );

    private: // Data

        TInt iResourceFileOffset;
        CCUIWaitDialog* iWaitDialog;
        CCUIProgressDialog* iProgressDialog;        
        CCoeEnv* iCoeEnv;  // not owned
        CAknInputBlock* iAbsorber;
        };
}
}

#endif      // SWCOMMONUI_H
            
// End of File
