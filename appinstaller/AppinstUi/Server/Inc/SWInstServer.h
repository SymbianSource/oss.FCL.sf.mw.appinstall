/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSWInstServer class 
*
*/


#ifndef SWINSTSERVER_H
#define SWINSTSERVER_H

//  INCLUDES
#include <AknServerApp.h>

namespace SwiUI
{

// FORWARD DECLARATIONS
class MRequestCallback;

// CLASS DECLARATION

/**
* This class represents the SWInstaller server.
*
* @since 3.0
*/
class CSWInstServer : public CAknAppServer
    {
    public:  // Constructors and destructor
  
        /**
        * Constructor.
        * @return Instance of the created server object.
        */  
        static CSWInstServer* NewL();

        /**
        * Destructor.
        */
        virtual ~CSWInstServer();      
        
    public: // New functions

        /**
        * Prepare the server for exit.
        * @since 3.0
        * @param aExitCallback - Will be called when the server is ready for exit.
        */
        void PrepareForExit( MRequestCallback* aExitCallback );        

    public: // Functions from base classes

        /**
        * From CApaAppServiceBase, Service creation function.
        * @since 3.0
        */
        CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;

        /**
        * From CEikAppServer, Security check for services.
        * @since 3.0
        */
        CPolicyServer::TCustomResult CreateServiceSecurityCheckL( TUid aServiceType, 
                                                                  const RMessage2& aMsg, 
                                                                  TInt& aAction, 
                                                                  TSecurityInfo& aMissing );        
        
    private:

        /**
        * Constructor.
        */
        CSWInstServer();

        /**
        * 2nd phase constructor.
        * @param aServerName - Name of the server
        */
        void ConstructL();
    };
}

#endif      // SWINSTSERVER_H   
            
// End of File
