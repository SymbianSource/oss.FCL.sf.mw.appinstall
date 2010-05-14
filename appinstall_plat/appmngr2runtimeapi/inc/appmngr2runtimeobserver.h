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
* Description:   AppMngr2 Runtime plugin observer interface
*
*/


#ifndef C_APPMNGR2RUNTIMEOBSERVER_H
#define C_APPMNGR2RUNTIMEOBSERVER_H

/**
 * Application Manager's change notification interface.
 *
 * Application Manager monitors changes in application list and scanned
 * directories. There may be some plug-in specific changes that are not
 * visible in monitored interfaces, and that require refreshing displayed
 * installed application (or installation packages) list. Runtime plug-in
 * can notify the Application Manager via this interface when needed.
 *
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class MAppMngr2RuntimeObserver
    {
public:
    /**
     * Runtime plugin can use RefreshInstalledApps() function to
     * refresh the installed applications list. Refreshing the list
     * makes AppMngr2 to refetch all installed application data.
     * Note that AppMngr2 listens EAppListChanged events via
     * MApaAppListServObserver, so plugins should use this
     * function only if AppMngr2 is not notified otherwise.
     */
    virtual void RefreshInstalledApps() = 0;
    
    /**
     * Runtime plugin can use RefreshInstallationFiles() function
     * to refresh the installation files list. Refreshing the list
     * makes AppMngr2 to rescan install directories and refetch
     * all installation package data. Note that AppMngr2 listens
     * RFs::NotifyChange() events to handle file additions and
     * deletions in defined install directories. Hence, plugins
     * should use this function only if AppMngr2 is not notified
     * otherwise.
     */
    virtual void RefreshInstallationFiles() = 0;
    };

#endif  // C_APPMNGR2RUNTIMEOBSERVER_H

