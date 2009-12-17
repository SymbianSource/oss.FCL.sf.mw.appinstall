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
* Description:   This file contains the header file of the CUIProgressDialog 
*                class.
*
*/


#ifndef CUIPROGRESSDIALOG_H
#define CUIPROGRESSDIALOG_H

// INCLUDES
#include <AknProgressDialog.h>

namespace SwiUI
{
namespace CommonUI
{

// FORWARD DECLARATIONS
class MCUIWaitDialogCallback;

// CLASS DECLARATION

/**
* Progress dialog for installer.
* @lib SWInstCommonUI.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CCUIProgressDialog) : public CAknProgressDialog
    {

    public: // Constructors and destructor

        /**
        * Constructor, parameter aAppUi is used in function OkToExitL()
        */
        CCUIProgressDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );

        virtual ~CCUIProgressDialog();

    public:
        
        /**
        * From CEikDialog, respond to softkey inputs.
        * @para aButtonId, type of pressed Button or Softkey
        * @return TBool, ETrue if exit the dialog, otherwise EFalse.
        */
        TBool OkToExitL( TInt aButtonId );

    public:  // New functions
        void SetCallback( MCUIWaitDialogCallback* aCallback );        

    protected:  // from CAknNoteDialog
        void PreLayoutDynInitL();

    private: //data

        MCUIWaitDialogCallback* iCallback;        
    };
}
}

#endif // CUIPROGRESSDIALOG_H

// End of file
