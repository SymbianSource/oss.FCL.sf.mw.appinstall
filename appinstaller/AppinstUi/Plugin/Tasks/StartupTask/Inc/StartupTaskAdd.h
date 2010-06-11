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
* Description:   This file contains the header file of the CStartupTaskAdd 
*                class.
*
*/


#ifndef STARTUPTASKADD_H
#define STARTUPTASKADD_H

//  INCLUDES
#include <e32base.h>
#include "StartupTask.h"

namespace SwiUI
{

/**
* Implementation of CTask ECOM interface for startup list addition task.
*
* @since 3.0
*/
class CStartupTaskAdd : public CStartupTask
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CStartupTaskAdd* NewL();

        /**
        * Destructor.
        */
        virtual ~CStartupTaskAdd();

    public: // Functions from base classes
        
        /**
        * From CTask, Executes the task.
        * @since 3.0
        */
        void ExecuteL();           

    private: // New functions
      
        /**
        * Constructor.
        */  
        CStartupTaskAdd();            
    };
}

#endif      // TASK_H   
            
// End of File
