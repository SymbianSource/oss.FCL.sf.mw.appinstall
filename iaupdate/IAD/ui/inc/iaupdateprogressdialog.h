/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateProgressDialog class.
*
*/



#ifndef IAUPDATEPROGRESSDIALOG_H
#define IAUPDATEPROGRESSDIALOG_H

// INCLUDES
#include <AknWaitDialog.h>


// FORWARD DECLARATIONS
class MIAUpdateWaitDialogObserver;

// CLASS DECLARATION

/**
* Wait dialog for IA Update client
*/
class CIAUpdateProgressDialog : public CAknProgressDialog
    {

    public: // Constructors and destructor

        /**
        * Constructor
        */
        CIAUpdateProgressDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );

        virtual ~CIAUpdateProgressDialog();

    public:
        
        /**
        * From CEikDialog, respond to softkey inputs.
        * @para aButtonId, type of pressed Button or Softkey
        * @return TBool, ETrue if exit the dialog, otherwise EFalse.
        */
        TBool OkToExitL( TInt aButtonId );

    public:  // New functions
        void SetCallback( MIAUpdateWaitDialogObserver* aCallback );  
        
        /**
        * Set final value for the progress bar in progress dialog.        
        * @param aValue - Final value of the progress bar.
        */
        void SetProgressDialogFinalValueL( TInt aValue );

        /**
        * Increments the progress bar in progress dialog.
        * @param aValue - Value to be set as the total progress.
        */
        void UpdateProgressDialogValueL( TInt aValue );      

    private: //data

        MIAUpdateWaitDialogObserver* iCallback;   
        TInt iMaxProgress;     
    };


#endif // IAUPDATEPROGRESSDIALOG_H

// End of file
