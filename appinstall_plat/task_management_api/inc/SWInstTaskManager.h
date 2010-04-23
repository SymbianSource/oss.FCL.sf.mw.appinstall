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
* Description:   This file contains the header file of the CTaskManager class.
*
*/


#ifndef SWINSTTASKMANAGER_H
#define SWINSTTASKMANAGER_H

//  INCLUDES
#include <e32base.h>

namespace SwiUI
{

class CTask;
class CTaskManagerImpl;

/**
* TaskManager handles persistent and nonpersistent tasks in the subsystem. It
* can be used to make sure that critical tasks are executed even in case of
* fatal error such as power failure.
*
* @since 3.0
* @lib SWInstTaskManager.lib
*/
class CTaskManager : public CBase
    {
    public:  // Constructors and destructor
       
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CTaskManager* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CTaskManager();

    public: // New functions
        
        /**
        * Adds a task to the task list.
        * @since 3.0
        * @param aTask - Pointer to the CTask object. Transfers ownership to TaskManager.
        */
        IMPORT_C void AddTaskL( CTask* aTask );

        /**
        * Removes a task from the task list.
        * @since 3.0
        * @param aTask - Pointer to the CTask object.
        */
        IMPORT_C void RemoveTaskL( CTask* aTask );

        /**
        * Removes tasks from task list without executing them.
        * @since 3.0
        */                
        IMPORT_C void FlushTasks();

        /**
        * Executes all pending tasks that are added to the task list within 
        * the lifetime of this thread.
        * In order to recover from fatal error, please use ExecutelRecoveryTasksL.
        * @since 3.0
        */        
        IMPORT_C void ExecutePendingTasksL();

        /**
        * Executes all tasks from non persistent and persistent memory. This
        * should be used to recover from fatal error e.g. power loss.
        * @since 3.0
        */        
        IMPORT_C void ExecuteRecoveryTasksL();

        /**
        * Writes all persistent tasks from the task list to a persistent storage.
        * @since 3.0
        */                
        IMPORT_C void CommitL();

    private: // New functions
        
        /**
        * Constructor.
        */  
        CTaskManager();
       
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Return pointer to TaskManager implementation.
        * @since 3.0       
        * @return Pointer to CTaskManager instance.
        */
        static CTaskManagerImpl* Impl(); 

    private: // Data
        
        CTaskManagerImpl* iImpl;        
    };
}

#endif      // SWINSTTASKMANAGER_H   
            
// End of File
