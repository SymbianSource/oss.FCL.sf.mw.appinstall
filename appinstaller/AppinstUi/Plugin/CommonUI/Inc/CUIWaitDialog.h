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
* Description:   This file contains the header file of the CCUIWaitDialog class.
*
*/


#ifndef CUIWAITDIALOG_H
#define CUIWAITDIALOG_H

// INCLUDES
#include <AknWaitDialog.h>

namespace SwiUI
{
namespace CommonUI
{

// FORWARD DECLARATIONS
class MCUIWaitDialogCallback;

// CLASS DECLARATION

/**
* Wait dialog for installer
* @lib SWInstCommonUI.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CCUIWaitDialog) : public CAknWaitDialog
    {

    public: // Constructors and destructor

        /**
        * Constructor, parameter aAppUi is used in function OkToExitL()
        */
        CCUIWaitDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );

        virtual ~CCUIWaitDialog();

    public:
        
        /**
        * From CEikDialog, respond to softkey inputs.
        * @para aButtonId, type of pressed Button or Softkey
        * @return TBool, ETrue if exit the dialog, otherwise EFalse.
        */
        TBool OkToExitL( TInt aButtonId );

    public:  // New functions
        void SetCallback( MCUIWaitDialogCallback* aCallback );        

    private: //data

        MCUIWaitDialogCallback* iCallback;        
    };
}
}

#endif // CUIWAITDIALOG_H

// End of file
