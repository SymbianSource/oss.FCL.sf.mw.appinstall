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
* Description:   Observer interface to indicate when data has been changed
*
*/


#ifndef C_APPMNGR2MODELOBSERVER_H
#define C_APPMNGR2MODELOBSERVER_H


class MAppMngr2ModelObserver
    {
public:
    virtual void InstalledAppsChanged( TInt aMoreRefreshesExpected ) = 0;
    virtual void InstallationFilesChanged( TInt aMoreRefreshesExpected ) = 0;
    virtual TBool InstalledAppsDisplayed() = 0;
    virtual TBool InstallationFilesDisplayed() = 0;
    };


#endif  // C_APPMNGR2MODELOBSERVER_H

