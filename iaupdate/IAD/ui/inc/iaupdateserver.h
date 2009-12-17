/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateServer class 
*
*/



#ifndef IAUPDATESERVER_H
#define IAUPDATESERVER_H

//  INCLUDES
#include <AknServerApp.h>


// FORWARD DECLARATIONS
class MRequestCallback;

// CLASS DECLARATION

/**
* This class represents the IAUpdate server.
*
* @since 3.2
*/
class CIAUpdateServer : public CAknAppServer
    {
    public:  // Constructors and destructor
  
        /**
        * Constructor.
        * @return Instance of the created server object.
        */  
        static CIAUpdateServer* NewL();

        /**
        * Destructor.
        */
        virtual ~CIAUpdateServer();      
        
    
    public: // Functions from base classes

        /**
        * From CApaAppServiceBase, Service creation function.
        * @since 3.2
        */
        CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
               
        
        void HandleAllClientsClosed();
        
        
    private:

        /**
        * Constructor.
        */
        CIAUpdateServer();

        /**
        * 2nd phase constructor.
        * @param aServerName - Name of the server
        */
        void ConstructL();
        

    };

#endif      // IAUPDATESERVER_H
            
// End of File
