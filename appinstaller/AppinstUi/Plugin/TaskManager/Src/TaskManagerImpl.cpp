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
* Description:   This module contains the implementation of CTaskManagerImpl 
*                class member functions.
*
*/


// INCLUDE FILES

#include <s32file.h>
#include <pathinfo.h>

#include "TaskManagerImpl.h"
#include "SWInstTask.h"

using namespace SwiUI;

_LIT( KTaskDir, "Tasks\\" );
_LIT( KTaskListMatch, "*" );

const TUint KMaxNumLength = 20;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTaskManagerImpl::CTaskManagerImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTaskManagerImpl::CTaskManagerImpl()
    {
    }

// -----------------------------------------------------------------------------
// CTaskManagerImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::ConstructL()
    {
    User::LeaveIfError( iRFs.Connect() );    
    }

// -----------------------------------------------------------------------------
// CTaskManagerImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTaskManagerImpl* CTaskManagerImpl::NewL()
    {
    CTaskManagerImpl* self = new( ELeave ) CTaskManagerImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// Destructor
CTaskManagerImpl::~CTaskManagerImpl()
    {
    iTaskList.ResetAndDestroy();    
    iRFs.Close();    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::AddTaskL
// Adds a task to the task list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::AddTaskL( CTask* aTask )
    {
    iTaskList.Append( aTask );    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::RemoveTaskL
// Removes a task from the task list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::RemoveTaskL( CTask* aTask )
    {
    TInt index = iTaskList.Find( aTask );
    
    if ( aTask->IsPersistent() )
        {
        RemovePersistentTask( *aTask );        
        }    

    iTaskList.Remove( index );
    delete aTask;    
    }
     
// -----------------------------------------------------------------------------
// TaskManagerImpl::FlushTasks
// Removes tasks from task list without executing them.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//             
void CTaskManagerImpl::FlushTasks()
    {
    while ( iTaskList.Count() )
        {
        CTask* task = iTaskList[0];        
        iTaskList.Remove( 0 );

        if ( task->IsPersistent() )
            {
            RemovePersistentTask( *task );            
            }
        
        delete task;       
        }
    
    iTaskList.Reset();    
    }
          
// -----------------------------------------------------------------------------
// TaskManagerImpl::ExecutePendingTasksL
// Executes all pending tasks that are added to the task list within the 
// lifetime of this thread.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::ExecutePendingTasksL()
    {
    TInt error( KErrNone );

    while ( iTaskList.Count() )
        {
        CTask* task = iTaskList[0];        
        TRAP( error, task->ExecuteL() );
        iTaskList.Remove( 0 );

        if ( error == KErrNone && task->IsPersistent() )
            {
            RemovePersistentTask( *task );            
            }
        
        delete task;       
        }   

    iTaskList.Reset();    

    if ( error != KErrNone )
        {
        User::Leave( error );        
        }    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::ExecuteRecoveryTasksL
// Executes all tasks from non persistent and persistent memory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::ExecuteRecoveryTasksL()
    {
    TInt error( KErrNone );
 
    PopulatePersistentTasksL();
    
    while ( iTaskList.Count() )
        {
        CTask* task = iTaskList[0];        
        TRAP( error, task->ExecuteL() );
        iTaskList.Remove( 0 );
        
        if ( task->IsPersistent() )
            {
            RemovePersistentTask( *task );            
            }
 
        delete task;       
        }             

    iTaskList.Reset();    
    DeleteTaskFilesL();

    if ( error != KErrNone )
        {
        User::Leave( error );        
        }    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::CommitL
// Writes all persistent tasks from the task list to a persistent storage.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::CommitL()
    {
    CreateTaskDirL();
    
    for ( TInt index = 0; index < iTaskList.Count(); index++ )
        {
        CTask* task = iTaskList[index];
        
        // Write to store only if this task is persistent and it has no id
        // (already written)
        if ( task->IsPersistent() && !task->Id() )
            {
            // Get an id for the task
            TInt id = GetFreeTaskIdL();            
            task->SetId( id );
          
            // Create stream
            RFileWriteStream stream;
            stream.PushL();
            TFileName filePath;                
            GetTaskFilePath( filePath, id );
            User::LeaveIfError( stream.Create( iRFs, filePath, EFileWrite ) );
    
            // Externalize task
            stream.WriteInt32L( task->ImplementationUid().iUid );            
            stream << *task;
            stream.CommitL();
            CleanupStack::PopAndDestroy(); // stream            
            }        
        }
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::IncreaseClientCount
// Increase the count of clients.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::IncreaseClientCount()
    {
    ++iClientCount;
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::DecreaseClientCount
// Decrease the count of clients.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTaskManagerImpl::DecreaseClientCount()
    {
    --iClientCount;
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::CanBeFreed
// Indicates if no more clients have open instances and memory can be freed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CTaskManagerImpl::CanBeFreed() 
    {
    if ( iClientCount > 0 )
	{
        return EFalse;
	}
    else
	{
        return ETrue;
	}
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::RemovePersistentTask
// Removes task from persistent storage.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CTaskManagerImpl::RemovePersistentTask( const CTask& aTask )
    {
    // Remove only if task is committed, i.e. it has an id.
    if ( aTask.Id() )
        {        
        TFileName fileName;    
        GetTaskFilePath( fileName, aTask.Id() );
        iRFs.Delete( fileName );    
        }    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::DeleteTaskFilesL
// Deletes all tasks from persistent storage.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CTaskManagerImpl::DeleteTaskFilesL()
    {
    TFileName fileMatch;
    GetTaskDir( fileMatch );
    fileMatch.Append( KTaskListMatch );

    CFileMan* fileMan = CFileMan::NewL( iRFs );
    fileMan->Delete( fileMatch );
    delete fileMan;    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::CreateTaskDirL
// Creates the directory where persistent tasks are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CTaskManagerImpl::CreateTaskDirL()
    {
    TInt drive = 0;
    iRFs.CharToDrive( TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive()[0], drive );
    iRFs.CreatePrivatePath( drive );
    
    TFileName taskDir;    
    GetTaskDir( taskDir );    
    iRFs.MkDir( taskDir );    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::GetTaskDir
// Gets the directory where persistent tasks are stored.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CTaskManagerImpl::GetTaskDir( TDes& aFilePath )
    {
    iRFs.PrivatePath( aFilePath );
    aFilePath.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
    aFilePath.Append( KTaskDir );    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::GetTaskFilePath
// Gets the full path to the persistent task file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CTaskManagerImpl::GetTaskFilePath( TDes& aFilePath, TInt aTaskId )
    {
    GetTaskDir( aFilePath );    
    aFilePath.AppendNum( aTaskId ); 
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::PopulatePersistentTasksL
// Reads all persistent task from storage and populates the task list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CTaskManagerImpl::PopulatePersistentTasksL()
    {
    // Get task dir matcher
    HBufC* taskDir = HBufC::NewLC( KMaxFileName );
    TPtr dirPtr( taskDir->Des() );    
    GetTaskDir( dirPtr );
    dirPtr.Append( KTaskListMatch );
    
    // Get dir contents
    CDir* dirList;    
    iRFs.GetDir( dirPtr, KEntryAttMaskSupported, ESortByDate, dirList );
    
    if ( dirList )
        {        
        CleanupStack::PushL( dirList );    
    
        HBufC* taskPath = HBufC::NewLC( KMaxFileName );
        TPtr pathPtr( taskPath->Des() );
        GetTaskDir( dirPtr );
        
        // Go through the list and internalize tasks
        for ( TInt index = 0; index < dirList->Count(); index++ )
            {
            pathPtr.Copy( dirPtr );            
            pathPtr.Append( (*dirList)[index].iName );
            
            // Create stream
            RFileReadStream stream;
            stream.PushL();    
            User::LeaveIfError( stream.Open( iRFs, pathPtr, EFileRead ) );

            // Internalize task
            TUid uid;
            uid.iUid = stream.ReadInt32L();            
            CTask* task = CTask::NewL( uid, ETrue );
            stream >> *task;
            iTaskList.Append( task );
            CleanupStack::PopAndDestroy(); // stream        
            }

        CleanupStack::PopAndDestroy( 2, dirList );        
        }
    CleanupStack::PopAndDestroy( taskDir );    
    }

// -----------------------------------------------------------------------------
// TaskManagerImpl::GetFreeTaskIdL
// Gets the next free id that can be assigned to a task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt CTaskManagerImpl::GetFreeTaskIdL()
    {
    TInt result( 0 );
    
    // Get task dir matcher
    HBufC* taskDir = HBufC::NewL( KMaxFileName );
    TPtr dirPtr( taskDir->Des() );    
    GetTaskDir( dirPtr );
    dirPtr.Append( KTaskListMatch );
    
    // Get dir contents
    CDir* dirList;    
    iRFs.GetDir( dirPtr, KEntryAttMaskSupported, ESortByDate, dirList );
    delete taskDir;
    
    if ( dirList )
        {        
        // Find the first free slot
        TInt index( 0 );        
        while ( index < dirList->Count() )
            {
            TBuf<KMaxNumLength> temp;
            // Make sure that first index is 1
            temp.Num( index + 1 );            
            if ( TParsePtrC( (*dirList)[index].iName ).Name() != temp )
                {
                break;
                } 
            ++index;            
            }   

        delete dirList;   
        // Make sure that the first index is 1        
        result = index + 1;        
        }

    return result;    
    }

//  End of File  
