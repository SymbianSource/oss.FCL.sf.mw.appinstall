/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSWInstAppUi class 
*
*                This class is a part of the standard application framework.
*                It instantiates the application views.
*                It also acts as the default command handler for the application.
*
*/


#ifndef SWINSTAPPUI_H
#define SWINSTAPPUI_H

//  INCLUDES
#include <aknappui.h>
#include "SWInstRequestObject.h"

namespace SwiUI
{

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* This class is a part of the standard application framework.
* It instantiates the application views.
* It also acts as the default command handler for the application.
*/
class CSWInstAppUi : public CAknAppUi, public MRequestCallback
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */ 
        CSWInstAppUi();
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
	
        /**
        * Destructor.
        */
	virtual ~CSWInstAppUi();   

    public: // New functions

        /**
        * Perform installation.
        * @since 3.0
        * @param aFileName - Full path to the package to be installed.
        */
        void InstallL( const TDesC& aFileName );   

        /**
        * Perform installation.
        * @since 3.0
        * @param aFile - Handle to the file to be installed.
        */
        void InstallL( RFile& aFile );    

    public: // Functions from base classes
        
        /**
        * From MRequestCallback, Called when request is completed.
        * @param aResult - Result of the request.
        */
        void RequestCompleted( TInt aResult );

    private: // Functions from base classes

        /**
        * From CEikAppUi, Handles user commands.
        * @since 3.0
        * @param aCommand - Command ID
        */
	virtual void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, Processes shell commands.
        * @since 3.0
        * @param aCommand - The shell command sent to the application
        * @param aDocumentName - The document name that will be given to 
        * OpenFileL()
        * @param aTail - The rest of the command line
        * @return Whether the final document name represents an existing file
        */
	virtual TBool ProcessCommandParametersL( TApaCommand aCommand,
                                                 TFileName& aDocumentName,
                                                 const TDesC8& aTail );
        
        /**
        * From CEikAppUi, Handles system events.
        * @since 3.0
        * @param aEvent - Event to handle.
        */
        virtual void HandleSystemEventL( const TWsEvent &aEvent );        

    private:    // Data

        CSWInstRequestObject* iReqObj;
        TBool iCancelInProgress; 
        TBool iOkToExit;        
    };

}

#endif      // SWINSTAPPUI_H   
            
// End of File
