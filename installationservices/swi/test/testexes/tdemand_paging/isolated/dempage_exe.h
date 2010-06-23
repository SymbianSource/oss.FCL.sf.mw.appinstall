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


/**
 @file dempage_exe.h
 @internalComponent
 @released
*/

#ifndef __DEMPAGE_EXE_H__
#define __DEMPAGE_EXE_H__

#include "smallarray.h"

static const TInt KIndexInRange = 1;
namespace Swi
	{

/**
 * Observer File creation in a given path
 */
class MFileObserver
	{
public:
	/** Enumeration indicating whether a file has been changed or not */
	enum TChangeType { EFileChanged, EFileNotChanged };
	
/**
 Description: FileChangeL-This function is called to indicate a file change has occurred     
 @internalTechnology
 @param aChangeType	EFileChanged, to indicate file has changed,
		or EFileNotChanged to indicate file has not changed.
 */
	virtual void FileChangeL(RFs& aFs,TChangeType aChangeType)=0;
	};

class CFileObserver:public MFileObserver
	{
	public:
	static CFileObserver* NewL();
	CFileObserver();
	~CFileObserver();
	protected:
	void FileChangeL(RFs& aFs,TChangeType aChangeType);
	};

/**
 * This active object waits for an indication that the file has changed,
 * and then notifies its observer.
 */
class CFileWatcher : public CActive
	{
public:
	static CFileWatcher * NewL(RFs& aFs,  CSmallArray& aDemPageSmallArray ,MFileObserver& aObserver,TInt aPriority = EPriorityStandard);
	static CFileWatcher * NewLC(RFs& aFs, CSmallArray& aDemPageSmallArray,MFileObserver& aObserver,TInt aPriority = EPriorityStandard);
	
	~CFileWatcher ();

private:
	CFileWatcher (RFs& aFs, MFileObserver& aObserver,CSmallArray& aDemPageSmallArray, TInt aPriority);

	void ConstructL();

	/** from CActive*/
	void DoCancel();
	void RunL();

	/** Set up change notification.*/
	void WaitForChangeL();


private:
	/** Not owned by this class */
	RFs& iFs;
	
	/** Observer to notify of changes */
	MFileObserver& iObserver;

    CSmallArray& iDemPageSmallArray ;
	};


} //namespace Swi
#endif // #ifndef __DEMPAGE_EXE_H__
