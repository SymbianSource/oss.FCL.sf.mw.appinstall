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
* threadmonitor - definition of CThreadMonitor class
*
*/


/**
 @file
 @released
 @internalComponent
*/
#ifndef __THREADMONITOR_H__
#define __THREADMONITOR_H__

#include <e32base.h>

class RWsSession;

namespace Swi
{

class CThreadMonitor
    {
public:
    static CThreadMonitor* NewL(TThreadId aTThreadId, RWsSession* aWsSession);
    
    static CThreadMonitor* NewLC(TThreadId aTThreadId, RWsSession* aWsSession);
    
    CThreadMonitor(TThreadId aTThreadId, RWsSession* aWsSession);
    
    virtual ~CThreadMonitor();
    
    void SyncShutdownL(TInt aDelay, TBool aKillOnTimeout, TInt aApplicationShutdownTimeout);
    
private:
	void ConstructL();
	
	TThreadId iTThreadId;
	RThread iThread;
	RWsSession* iWsSession;
	TBool iThreadAlreadyClosed;
    };

} //namespace

#endif // __THREADMONITOR_H__
