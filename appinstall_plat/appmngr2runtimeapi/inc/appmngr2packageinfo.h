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
* Description:   PackageInfo represents installable package (sisx, jad, jar, ...)
*
*/


#ifndef C_APPMNGR2PACKAGEINFO_H
#define C_APPMNGR2PACKAGEINFO_H

#include <appmngr2infobase.h>           // CAppMngr2InfoBase

class RFs;

/**
 * CAppMngr2PackageInfo represents installation package (installation file).
 * 
 * Runtime plug-in must define it's own CAppMngr2PackageInfo derived class
 * to provide the actual functionality for abstract functions defined in
 * CAppMngr2InfoBase class.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2PackageInfo : public CAppMngr2InfoBase
    {
public:     // constructor and destructor
    IMPORT_C void ConstructL();
    IMPORT_C ~CAppMngr2PackageInfo();

protected:  // new functions
    IMPORT_C CAppMngr2PackageInfo( CAppMngr2Runtime& aRuntime, RFs& aFsSession );
    
    /**
     * Utility function to show R_APPMNGR2_YES_NO_CONFIRMATION_QUERY
     * confirmation query with R_AM_DELETEQUERY string and item name.
     * 
     * @return TBool  ETrue, if user accepted the query
     */ 
    IMPORT_C TBool ShowDeleteConfirmationQueryL();
    };

#endif // C_APPMNGR2PACKAGEINFO_H

