/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of MNcdInstallationServiceObserver
*
*/


#ifndef M_NCD_INSTALLATION_SERVICE_OBSERVER_H
#define M_NCD_INSTALLATION_SERVICE_OBSERVER_H

/**
 * 
 */
class MNcdInstallationServiceObserver
    {
public: // New functions

    /**
     * This function is called when Installation Service has completed
     * the installation process.
     * @param aFileName Will contain path and file name, if picture,
     * ringingtone or similar was installed and a theme name if a theme was installed.
     * If SIS application was installed this will contain version number of the
     * installed application.
     * Will contain KNullDesC for java applications.
     * @param aAppUid will contain UID if an application was installed, either sis
     * or java. Contains KNcdThemeSisUid if a theme was installed and
     * KNullUid for non-applications.
     * @param aError KErrNone if everything went ok. KErrAbort if the user cancelled
     * the installation. KNcdThemeReinstalled if a theme was reinstalled. 
     * KNcdThemePossiblyReinstalled if a theme was likely reinstalled.
     * Otherwise some Symbian error code.
     */
    virtual void InstallationCompleteL( const TDesC& aFileName, 
                                        const TUid& aAppUid,
                                        TInt aError ) = 0;

    };

#endif // M_NCD_INSTALLATION_SERVICE_OBSERVER_H
