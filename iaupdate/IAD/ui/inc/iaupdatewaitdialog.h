/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateWaitDialog class.
*
*/



#ifndef IAUPDATEWAITDIALOG_H
#define IAUPDATEWAITDIALOG_H

// INCLUDES
#include <AknWaitDialog.h>


// FORWARD DECLARATIONS
class MIAUpdateWaitDialogObserver;

// CLASS DECLARATION

/**
* Wait dialog for IA Update client
*/
class CIAUpdateWaitDialog : public CAknWaitDialog
    {

    public: // Constructors and destructor

        /**
        * Constructor
        */
        CIAUpdateWaitDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );

        virtual ~CIAUpdateWaitDialog();

    public:
        
        /**
        * From CEikDialog, respond to softkey inputs.
        * @para aButtonId, type of pressed Button or Softkey
        * @return TBool, ETrue if exit the dialog, otherwise EFalse.
        */
        TBool OkToExitL( TInt aButtonId );

    public:  // New functions
        void SetCallback( MIAUpdateWaitDialogObserver* aCallback );        

    private: //data

        MIAUpdateWaitDialogObserver* iCallback;        
    };


#endif // IAUPDATEWAITDIALOG_H

// End of file
