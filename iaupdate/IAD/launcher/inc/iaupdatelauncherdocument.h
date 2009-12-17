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
* Description:   This file contains the header file of the CIAUpdateLauncherDocument
*                class 
*
*/




#ifndef __IAUPDATELAUNCHERDOCUMENT_H__
#define __IAUPDATELAUNCHERDOCUMENT_H__

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CIAUpdateLauncherAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CLauncherDocument application class.
* An instance of class CLauncherDocument is the Document part of the
* AVKON application framework for the Launcher example application.
*/
class CIAUpdateLauncherDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CIAUpdateLauncherDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CIAUpdateLauncherDocument
        */
        static CIAUpdateLauncherDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CIAUpdateLauncherDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CIAUpdateLauncherDocument.
        */
        static CIAUpdateLauncherDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CLauncherDocument
        * Virtual Destructor.
        */
        virtual ~CIAUpdateLauncherDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CIAUpdateLauncherAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CLauncherDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CIAUpdateLauncherDocument( CEikApplication& aApp );

    };

#endif // __IAUPDATELAUNCHERDOCUMENT_H__

// End of File

