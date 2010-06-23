/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IAUPDATE_WAITDIALOG_OBSERVER_H
#define IAUPDATE_WAITDIALOG_OBSERVER_H

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
    * This is called when the dialog is cancelled
    */
    virtual void HandleWaitDialogCancel() = 0;    
    };


#endif // IAUPDATE_WAITDIALOG_OBSERVER_H
            
// End of File
