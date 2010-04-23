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
* Description:   This file contains the header file of the 
*                MIAUpdateWaitDialogObserver class.
*
*/



#ifndef IAUPDATEDIALOGS_H
#define IAUPDATEDIALOGS_H

//  INCLUDES
#include <e32base.h>


//  CLASS DECLARATIONS

/**
* A callback interface for handling common non modal dialog cancellation.
* 
*/
class MIAUpdateWaitDialogObserver
    {
    public:
        
        /**
        * This is called when the dialog is about to be closed.
        * @param aButtonId - Id of the button, which was used to cancel the dialog.
        * @return ETrue, if it's ok to close the dialog, EFalse otherwise.
        */
        virtual TBool HandleDialogExitL( TInt aButtonId ) = 0;    
    };


#endif // IAUPDATEDIALOGS_H
            
// End of File
