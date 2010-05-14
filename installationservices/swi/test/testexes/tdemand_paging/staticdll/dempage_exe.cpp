/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "dempage_exe.h"
#include "largearraydll.h"

#include <f32file.h>

/**
Description:  NewLC      
*/
CFileWatcher* CFileWatcher::NewLC()
	{
	CFileWatcher* self=new(ELeave) CFileWatcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
/**
Description:  Destructor     
@internalTechnology
@test
*/
CFileWatcher::~CFileWatcher()
	{
	Cancel();
	iFs.Close();
	}


/**
Description:  Constructor
*/
CFileWatcher::CFileWatcher()
	: CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
Description:  ConstructL     
@internalTechnology
@test
*/
void CFileWatcher::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());

	// Notify observer of change in folder contents
	// Start watching for changes
	WaitForChangeL();
	}
/**
Description:  DoCancel
@internalTechnology
@test
*/
void CFileWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	}

/**
Description:  RunL
@internalTechnology
@test
*/
void CFileWatcher::RunL()
	{
	TUint32 version;
	const TUint32 *array=GetArray();
	
	switch(array[0])
		{
		case 0x1: // V1 DLL
			version=1;
			break;
		case 0x2001: // V2 DLL
			version=2;
			break;
		default:
			// bad data just return
			return;
		}
	

	TUint32 first = array[0];
	// Check we can read all array values and that they follow on in sequence.
	for(TUint32 i=0; i < EArrayMaxSize; ++i)
		{
		if(array[i] != first+i)
			{
			// Just return without creating a passed dir.
			return;
			}
		}
	
	// Array data is ok.
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> dirOnSysDrive (sysDrive.Name());
	
	if(version==1)
		{
		dirOnSysDrive.Append(_L("\\preq1110\\passed_dll_v1\\"));
		User::LeaveIfError(iFs.MkDirAll(dirOnSysDrive));
		}
	else
		{
		dirOnSysDrive.Append(_L("\\preq1110\\passed_dll_v2\\"));
		User::LeaveIfError(iFs.MkDirAll(dirOnSysDrive));
		}
	return;
	}

/**
Description:  WaitForChangeL 
@internalTechnology
@test
*/
void CFileWatcher::WaitForChangeL()
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> path(sysDrive.Name());
	path.Append(_L("\\preq1110\\wakeup"));
	iFs.NotifyChange(ENotifyEntry, iStatus, path);
	SetActive();
	}
	

// End of file
