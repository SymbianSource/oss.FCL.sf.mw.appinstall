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
* Description:   This file contains the header file of the CStartupTaskRemove 
*                class.
*
*/


#ifndef STARTUPTASKREMOVE_H
#define STARTUPTASKREMOVE_H

//  INCLUDES
#include <e32base.h>
#include "StartupTask.h"

namespace SwiUI
{

/**
* Implementation of CTask ECOM interface for startup list removal task.
*
* @since 3.0
*/
class CStartupTaskRemove : public CStartupTask
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CStartupTaskRemove* NewL();

        /**
        * Destructor.
        */
        virtual ~CStartupTaskRemove();

    public: // New functions
        
        /**
        * From CTask, Executes the task.
        * @since 3.0
        */
        void ExecuteL();       
                
    private: // New functions
        
        /**
        * Constructor.
        */  
        CStartupTaskRemove();
    };
}

#endif      // TASK_H   
            
// End of File
