/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

namespace Swi
{

/**
Description:  NewL      
@internalTechnology
@param aFs - File server object  															
@param aDrive  - Drive number 
@param aDemPageSmallArray - SmallArray to be accessed 
@param aObserver - an Observer object 
@param aPriority 
@return CFileWatcher* - pointer to newly created object of CFileWatcher
@test
*/
CFileWatcher* CFileWatcher::NewL(RFs& aFs, CSmallArray& aDemPageSmallArray, 
												MFileObserver& aObserver,
							 					TInt aPriority)
	{
	CFileWatcher* self=NewLC(aFs, aDemPageSmallArray,aObserver, aPriority);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Description:  NewLC      
@internalTechnology
@param aFs - File server object  															
@param aDrive  - Drive number 
@param aDemPageSmallArray - SmallArray to be accessed 
@param aObserver - an Observer object 
@param aPriority - Priority number
@return CFileWatcher* - pointer to newly created object of CFileWatcher
@test
*/
CFileWatcher* CFileWatcher::NewLC(RFs& aFs, 	CSmallArray& aDemPageSmallArray,	
												MFileObserver& aObserver,
												TInt aPriority)
	{
	CFileWatcher* self=new(ELeave) CFileWatcher(aFs, aObserver,aDemPageSmallArray, aPriority);
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
	}


/**
Description:  Constructor
@param aFs    - File server object
@param aDrive - Drive number
@param aObserver - an Observer object 
@param aPriority - Priority number
@internalTechnology
@test
*/
CFileWatcher::CFileWatcher(RFs& aFs, MFileObserver& aObserver,CSmallArray& aDemPageSmallArray,
							 TInt aPriority)
	: CActive(aPriority), iFs(aFs), iObserver(aObserver),iDemPageSmallArray(aDemPageSmallArray)
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
	TBool status = iDemPageSmallArray.ReadSmallArray(KIndexInRange);			
	iObserver.FileChangeL(iFs,(MFileObserver::TChangeType) status);
	}

/**
Description:  WaitForChangeL 
@internalTechnology
@test
*/
void CFileWatcher::WaitForChangeL()
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<32> path (sysDrive.Name());
	path.Append(_L("\\PREQ1110\\"));
	iFs.NotifyChange(ENotifyEntry, iStatus, path);
	SetActive();
	}
	
/**
Description:  NewL
@internalTechnology
@test
*/
CFileObserver* CFileObserver::NewL()	
	{
	CFileObserver* self = new(ELeave) CFileObserver();
	return self;
	}
	
/**
Description:  constructor 
@internalTechnology
@test
*/
CFileObserver::CFileObserver()
	{
	
	} 
	
/**
Description:  Destructor of class CFileObserver
@internalTechnology
@test
*/
CFileObserver::~CFileObserver()
	{
	
	}
	
/**
Description:  FileChangeL
@param aFs    - File server object
@param aDrive - Drive number
@param aChangeType - Type of change  
@param aPriority - Priority number
@internalTechnology
@test
*/
void CFileObserver::FileChangeL(RFs& aFs, TChangeType aChangeType)
	{
	if(!aChangeType)
		{
		//create pass file.
		RFile file;
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TBuf<128> path (sysDrive.Name());
		path.Append(_L("\\PREQ1110\\"));
		file.Create(aFs, path, EFileWrite);
		file.Close();
		//TRAPD(err,file.Create(aFs, _L("C:\\preq1110\\Passed"), EFileWrite)); 
		//err= err;//to remove warning
		}
	}
	
}  

