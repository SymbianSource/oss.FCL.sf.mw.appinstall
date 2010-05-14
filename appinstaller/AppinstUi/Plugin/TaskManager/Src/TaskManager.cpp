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
* Description:   This module contains the implementation of CTaskManager class 
*                member functions.
*
*/


// INCLUDE FILES

#include "SWInstTaskManager.h"
#include "SWInstTask.h"
#include "TaskManagerImpl.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTaskManager::CTaskManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTaskManager::CTaskManager()
    {
    }

// -----------------------------------------------------------------------------
// CTaskManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTaskManager::ConstructL()
    {
    if ( !Dll::Tls() )
        {
        Dll::SetTls( CTaskManagerImpl::NewL() );
        }
    
    // Increase the client count
    iImpl = Impl();
    iImpl->IncreaseClientCount();
    }

// -----------------------------------------------------------------------------
// CTaskManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTaskManager* CTaskManager::NewL()
    {
    CTaskManager* self = new( ELeave ) CTaskManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// Destructor
EXPORT_C CTaskManager::~CTaskManager()
    {
    // Decrease the client count
    iImpl->DecreaseClientCount();

    // Check if no more clients so that TLS can be freed.
    if ( iImpl->CanBeFreed() )
        {
        delete iImpl;
        Dll::SetTls( NULL );
        }    
    }

// -----------------------------------------------------------------------------
// TaskManager::AddTaskL
// Adds a task to the task list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTaskManager::AddTaskL( CTask* aTask )
    {
    if ( aTask == NULL  )
        {
        User::Leave( KErrArgument );    
        }    
    
    iImpl->AddTaskL( aTask );
    }

// -----------------------------------------------------------------------------
// TaskManager::RemoveTaskL
// Removes a task from the task list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTaskManager::RemoveTaskL( CTask* aTask )
    {
    if ( aTask == NULL  )
        {
        User::Leave( KErrArgument );    
        }    
    
    iImpl->RemoveTaskL( aTask );
    }
     
// -----------------------------------------------------------------------------
// TaskManager::FlushTasks
// Removes tasks from task list without executing them.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//             
EXPORT_C void CTaskManager::FlushTasks()
    {
    iImpl->FlushTasks();    
    }
          
// -----------------------------------------------------------------------------
// TaskManager::ExecutePendingTasksL
// Executes all pending tasks that are added to the task list within the 
// lifetime of this thread.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTaskManager::ExecutePendingTasksL()
    {
    iImpl->ExecutePendingTasksL();
    }

// -----------------------------------------------------------------------------
// TaskManager::ExecuteRecoveryTasksL
// Executes all tasks from non persistent and persistent memory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTaskManager::ExecuteRecoveryTasksL()
    {
    iImpl->ExecuteRecoveryTasksL();    
    }

// -----------------------------------------------------------------------------
// TaskManager::CommitL
// Writes all persistent tasks from the task list to a persistent storage.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTaskManager::CommitL()
    {
    iImpl->CommitL();    
    }

// -----------------------------------------------------------------------------
// TaskManager::Impl
// Return pointer to TaskManager implementation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTaskManagerImpl* CTaskManager::Impl()
    {
    return static_cast<CTaskManagerImpl*> (Dll::Tls());
    }

//  End of File  
