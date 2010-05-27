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
* Description:   This file contains the header file of the CDialogWrapper class.
*
*/


#ifndef DIALOGWRAPPER_H
#define DIALOGWRAPPER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "dialogwatcher.h"

class CHbDeviceProgressDialogSymbian;
class CHbDeviceNotificationDialogSymbian;
class CHbIndicatorSymbian;

namespace Swi
{
// Define uninstaller mode for universal indicator.
const TInt KSWIDaemonUninstallerMode = -1; 
// Define installer mode for universal indicator.
const TInt KSWIDaemonInstallerMode = 0;

class CDialogTimer;
/**
* This class wraps functions for showing some global dialogs.
* 
* @since 3.0 
*/
class CDialogWrapper : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDialogWrapper* NewL( RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CDialogWrapper();

    public:  // New functions

        /**
        * Show global result dialog (untrusted case).
        * @since 3.0
        */
        void ShowUntrustedResultL();

        /**
        * Show global result dialog (error case).
        * @since 3.0
        */
        void ShowErrorResultL();
        /**
        * Show global waiting note (during installing).
        * @since 3.0
        */
        void ShowWaitingNoteL();
        
        /**
        * Cancel global waiting note (after installing).
        * @since 3.0
        */
        void CancelWaitingNote();
        
        /**
         * Sets dialog control flag.
         * @since 3.2
         * @parm TInt Flag for dialog control.
         */       
        void SetUIFlag( TInt aUIFlag );
        
        /**
          * Show global waiting note (during uninstalling).
          * @since 3.2         
          */              
        void ShowWaitingNoteForUninstallerL();
        
        /**
         * Cancel global waiting note (after installing).
         * @since 10.1
         */        
        void CancelWaitingNoteForUninstaller();
 
        /**
         * Show universal indicator for install or uninstall process.
         * @since 10.1          
         */ 
        void ActivateIndicatorL( TReal aProcessValue );
        
        /**
         * Set mode for universal indicator plugin. 
         * @since 10.1 
         * @parm aMode Flag for switchig to uninstaller mode. By default
         * plugin works as installer mode.       
         */        
        void SetModeToIndicatorL( TInt aMode );
        
        /**
         * Close universal indicator for install or uninstall process.
         * @since 10.1         
         */        
        void CancelIndicatorL();
 
//TODO: remove, it seems that this is no needed after wk18.        
        /**
         * Check system state. This is needed to know, can Daemon show UI
         * dialogs.
         * @since 10.1         
         */       
        //void CheckSystemState();

    private:

        /**
        * Constructor.
        */  
        CDialogWrapper( RFs& aFs );
         
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Read resource string.
        * @since 3.0
        * @param aResourceId - Id of the resource.
        * @return Read resource buffer.
        */
        HBufC* ReadResourceLC( TInt aResourceId );        

    private: //  Data
          
        RFs& iFs;        
        TInt iNoteId;  
        TInt iDisableAllNotes;
        CDialogWatcher* iWatcher;
        
        TBool iIsProgressDialog; // Defines that dialog is constructed.  
        CHbDeviceProgressDialogSymbian* iHbProgressDialog;
        TBool iIsUninstallerProgressDialog; // Defines that dialog is constructed.                   
        CHbDeviceProgressDialogSymbian* iHbProgressDialogForUninstaller;
        TBool iIsIndicator; // Defines that indicator is constructed.
        CHbIndicatorSymbian* iHbIndicator;  // Univeral indicator.
        CDialogTimer* iTimer;   // Timer for closing procress dialog.
        // Defines that dialog has used the time interval and can not be shown.
        TBool iTimeOffDisableProgress;         
    };

/**
* Helper class for closing the progress dialogs shown by 
* the DialogWrapper class.
* 
* @since 10.1 
*/
class CDialogTimer : public CActive
    {
    public:
    
        /**
        * Destructor.
        */
        virtual ~CDialogTimer();
        
        /**
        * Two-phased constructor.
        */
        static CDialogTimer* NewL( CDialogWrapper* aDialog );
        
        /**
         * Function sets time interva for progress dialog.
         * After this time interval the progress dialog is closed.
         * @param aRefresTime Time in microseconds.
         */          
         void StartDialogTimer( TUint32 aRefreshTime );
        
    private:

        /**
        * C++ default constructor.
        */
        CDialogTimer();

        /**
        * 2nd phase constructor. 
        * @param 
        */
        void ConstructL( CDialogWrapper* aDialog );
        
        /**
         * Function sets time.
         * @param aRefresTime Time in microseconds.
         */          
         void TimerSet( TUint32 aRefreshTime );
      
        /**
        * 
        */        
        void RunL();

        /**
        * Cancel timer.
        */        
        void DoCancel();
        
        /**
        * Handles a leave occurring in the request completion 
        * event handler RunL.
        */             
        TInt RunError( TInt aError );

    private:

        RTimer          iRTimer;     // Timer.   
        CDialogWrapper* iDialog;     // Dialog class.                     
    };
}

#endif      // DIALOGWRAPPER_H   
            
// End of File
