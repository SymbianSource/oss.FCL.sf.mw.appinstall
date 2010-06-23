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

#include <e32base.h>
#include <f32file.h>
#include "largearraydll.h"

/**
 * This active object waits for an indication that the file has changed,
 * and then notifies its observer.
 */
class CFileWatcher : public CActive
	{
public:
	static CFileWatcher * NewLC();
	
	~CFileWatcher ();

private:
	CFileWatcher ();

	void ConstructL();

	/** from CActive*/
	void DoCancel();
	void RunL();

	/** Set up change notification.*/
	void WaitForChangeL();


private:
	RFs iFs;
	};



#endif // #ifndef __DEMPAGE_EXE_H__

