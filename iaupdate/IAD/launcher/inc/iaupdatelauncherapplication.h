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
* Description:   This file contains the header file of the CIaupdateLauncherApplication class 
*
*/



#ifndef __IAUPDATELAUNCHERAPPLICATION_H__
#define __IAUPDATELAUNCHERAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CIaupdateLauncherApplication application class.
* Provides factory to create concrete document object.
* An instance of CIaupdateLauncherApplication is the application part of the
* AVKON application framework for the IAupdate client launcher application.
*/
class CIAUpdateLauncherApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidHelloWorldBasicApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CIaupdateLauncherDocument document object. The returned
        * pointer in not owned by the CIaupdateLauncherApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __IAUPDATELAUNCHERAPPLICATION_H__

// End of File

