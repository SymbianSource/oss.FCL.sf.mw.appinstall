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
* Description:   This file contains the header file of the CTaskManagerImpl class.
*
*/


#ifndef SWINSTTASKMANAGERIMPL_H
#define SWINSTTASKMANAGERIMPL_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

namespace SwiUI
{

class CTask;

/**
* TaskManagerImpl handles persistent and nonpersistent tasks in the subsystem. It
* can be used to make sure that critical tasks are executed even in case of
* fatal error such as power failure.
*
* @since 3.0
* @lib SWInstTaskManagerImpl.lib
*/
NONSHARABLE_CLASS( CTaskManagerImpl ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTaskManagerImpl* NewL();

        /**
        * Destructor.
        */
        virtual ~CTaskManagerImpl();

    public: // New functions
        
        /**
        * Adds a task to the task list.
        * @since 3.0
        * @param aTask - Pointer to the CTask object. Transfers ownership to TaskManagerImpl.
        */
        void AddTaskL( CTask* aTask );

        /**
        * Removes a task from the task list.
        * @since 3.0
        * @param aTask - Pointer to the CTask object.
        */
        void RemoveTaskL( CTask* aTask );

        /**
        * Removes tasks from task list without executing them.
        * @since 3.0
        */                
        void FlushTasks();

        /**
        * Executes all pending tasks that are added to the task list within 
        * the lifetime of this thread.
        * In order to recover from fatal error, please use ExecutelRecoveryTasksL.
        * @since 3.0
        */        
        void ExecutePendingTasksL();

        /**
        * Executes all tasks from non persistent and persistent memory. This
        * should be used to recover from fatal error e.g. power loss.
        * @since 3.0
        */        
        void ExecuteRecoveryTasksL();

        /**
        * Writes all persistent tasks from the task list to a persistent storage.
        * @since 3.0
        */                
        void CommitL();        

        /**
        * Increase the count of clients.
        * @since 3.0       
        */  
        void IncreaseClientCount();

        /**
        * Decrease the count of clients.
        * @since 3.0       
        */
        void DecreaseClientCount();

        /**
        * Indicates if no more clients have open instances and memory can be freed.
        * @since 3.0       
        */
        TBool CanBeFreed();

    private: // New functions
        
        /**
        * Constructor.
        */  
        CTaskManagerImpl();  

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Removes task from persistent storage.
        * @since 3.0       
        * @param aTask - Task to be removed.
        */
        void RemovePersistentTask( const CTask& aTask );

        /**
        * Deletes all tasks from persistent storage.
        * @since 3.0             
        */
        void DeleteTaskFilesL();

        /**
        * Creates the directory where persistent tasks are stored.
        * @since 3.0       
        */
        void CreateTaskDirL();

        /**
        * Gets the directory where persistent tasks are stored.
        * @since 3.0       
        * @param aFilePath - On return will contain the path.
        */        
        void GetTaskDir( TDes& aFilePath );

        /**
        * Gets the full path to the persistent task file.
        * @since 3.0       
        * @param aFilePath - On return will contain the path.
        * @param aTaskId - Id of the task.
        */        
        void GetTaskFilePath( TDes& aFilePath, TInt aTaskId );

        /**
        * Reads all persistent task from storage and populates the task list.
        * @since 3.0       
        */        
        void PopulatePersistentTasksL();

        /**
        * Gets the next free id that can be assigned to a task.
        * @since 3.0       
        * return Task id.
        */        
        TInt GetFreeTaskIdL();

    private:    // Data
        
        RPointerArray<CTask> iTaskList;
        TUint iClientCount;
        RFs iRFs;        
    };
}

#endif      // SWINSTTASKMANAGER_H   
            
// End of File
