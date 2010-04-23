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
* Description:   AppInfo represents installed application
*
*/


#ifndef C_APPMNGR2APPINFO_H
#define C_APPMNGR2APPINFO_H

#include "appmngr2infobase.h"           // CAppMngr2InfoBase

/**
 * CAppMngr2AppInfo represents installed application.
 * 
 * Runtime plug-in must define it's own CAppMngr2AppInfo derived class
 * to provide the actual functionality for abstract functions defined in
 * CAppMngr2InfoBase class.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2AppInfo : public CAppMngr2InfoBase
    {
public:     // constructor and destructor
    IMPORT_C void ConstructL();
    IMPORT_C ~CAppMngr2AppInfo();

protected:  // new functions
    /**
     * Exported protected constructor for derived classes
     */
    IMPORT_C CAppMngr2AppInfo( CAppMngr2Runtime& aRuntime, RFs& aFsSession );
    };

#endif // C_APPMNGR2APPINFO_H

