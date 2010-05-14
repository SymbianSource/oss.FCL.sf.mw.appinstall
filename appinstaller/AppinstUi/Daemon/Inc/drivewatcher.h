/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/**
 * @file 
 *
 * @internalComponent
 * @prototype
 */

#ifndef __DRIVEWATCHER_H__
#define __DRIVEWATCHER_H__

#include <e32base.h>
#include <f32file.h>

namespace Swi
{

/**
 * Observer interface called whenever a media change is detected.
 */
class MDriveObserver
	{
public:
	/// Enumeration indicating whether a card was inserted or removed
	enum TChangeType { EMediaInserted, EMediaRemoved };
	
	/**
	 * This function is called to indicate a media change has occurred.
	 *
	 * @param aChangeType	EInserted to indicate media has been inserted,
	 *						or ERemoved to indicate media has been removed.
	 */
	virtual void MediaChangeL(TInt aDrive, TChangeType aChangeType)=0;
	};

/**
 * This active object waits for an indication that the media has changed,
 * and then notifies its observer.
 */
class CDriveWatcher : public CActive
	{
public:
	static CDriveWatcher* NewL(RFs& aFs, TInt aDrive, MDriveObserver& aObserver,TInt aPriority = EPriorityStandard);
	static CDriveWatcher* NewLC(RFs& aFs, TInt aDrive, MDriveObserver& aObserver,TInt aPriority = EPriorityStandard);
	
	~CDriveWatcher();

	TInt Drive() const;
private:
	CDriveWatcher(RFs& aFs, TInt aDrive, MDriveObserver& aObserver, TInt aPriority);

	void ConstructL();

// from CActive
	void DoCancel();
	void RunL();

	/// Set up change notification.
	void WaitForChangeL();

	/**
	 * Notifies the observer of the appropriate media change depending on whether
	 * media is now present on the drive we're currently watching.
	 */
	void NotifyMediaChange();
	
	/** 
	 * Determines if the media is present in the drive we are currently watching.
	 *
	 * @return ETrue if the media is present, EFalse otherwise.
	 */
	TBool IsMediaPresentL();

private:
	/// Not owned by this class
	RFs& iFs;
	
	/// Drive number we are watching
	TInt iDrive;

	/// Observer to notify of changes
	MDriveObserver& iObserver;  
	};

// inline functions from CDriveWatcher

inline TInt CDriveWatcher::Drive() const
	{
	return iDrive;
	}
	
} //namespace Swi

#endif // #ifndef __DRIVEWATCHER_H__

