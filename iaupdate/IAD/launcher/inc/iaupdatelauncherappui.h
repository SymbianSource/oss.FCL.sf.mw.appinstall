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
* Description:   This file contains the header file of the CIAUpdateLauncherAppUi class 
*
*/




#ifndef __IAUPDATELAUNCHERAPPUI_H__
#define __IAUPDATELAUNCHERAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CIAUpdateLauncherManager;

// CLASS DECLARATION
/**
* CLauncherAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CIAUpdateLauncherAppUi : public CAknAppUi
                          
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CIAUpdateLauncherAppUi();

        /**
        * Virtual Destructor.
        */
        virtual ~CIAUpdateLauncherAppUi();

    private: // from CAknViewAppUi
    
         /*!
          * @function HandleCommandL  
          * @discussion Handle user menu selections
          * @param aCommand the enumerated code for the option selected
          */
        void HandleCommandL( TInt aCommand );

        
     private: // from CEikAppUi
        /**
        * @param aCommand - The shell command sent to the application
        * @param aDocumentName - The document name that will be given to 
        * OpenFileL()
        * @param aTail - The rest of the command line
        * @return Whether the final document name represents an existing file
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );

        
    private: // Data
    
        CIAUpdateLauncherManager* iLauncherManager; 
        TBool iRefreshFromNetworkDenied;
    };

#endif // __IAUPDATELAUNCHERAPPUI_H__

// End of File

