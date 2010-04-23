/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Shutdown watcher header file
*
*/


#ifndef _SWIDAEMON_SHUTDOWNWATHCER_H
#define _SWIDAEMON_SHUTDOWNWATHCER_H

#include <e32base.h>
#include <e32property.h>

/**
 *  Shutdown observer
 *  Gets notification when the system is going down
 *
 *  @since S60 v5.0
 */
class MShutdownObserver
    {
public:
    /**
     * Notification when system is shutting down
     */
    virtual void NotifyShuttingDown()=0;
    };

/**
 *  Shutdown watcher
 *  Notifies MShutdownObserver when the system is going down
 *
 *  @since S60 v5.0
 */
class CShutdownWatcher : public CActive
    {
public:
    static CShutdownWatcher* NewL( MShutdownObserver& aObserver );
    ~CShutdownWatcher();

    /**
     * Starts to watch shutdown status
     * @return none
     */
    void Start();

private:
    CShutdownWatcher( MShutdownObserver& aObserver );
    void ConstructL();
    void IssueRequest();

    // from CActive
    void DoCancel();
    void RunL();

private:
    MShutdownObserver&  iObserver;
    RProperty           iProperty;
    };

#endif //_SWIDAEMON_SHUTDOWNWATHCER_H
