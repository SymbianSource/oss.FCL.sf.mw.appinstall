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
* Description:   This file contains the header file of the CIAUpdateSession class 
*
*/



#ifndef IAUPDATESESSION_H
#define IAUPDATESESSION_H

//  INCLUDES
#include <AknServerApp.h>

#include "iaupdaterequestobserver.h"

// FORWARD DECLARATIONS
class CIAUpdateServer;
class RMessage2;
class CEikonEnv;

// CLASS DECLARATION

/**
* This class represents the main server session.
*
*/
class CIAUpdateSession : public CAknAppServiceBase,
                         public MIAUpdateRequestObserver
    {
    public:  // Constructors and destructor
     
        /**
        * C++ default constructor.
        */
        CIAUpdateSession();

        /**
        * Destructor.
        */
        virtual ~CIAUpdateSession();

    public: // From CAknAppServiceBase

        /**
        * Handles the received message.
        * The method simply calls HandleRequestL under trap harness. 
        * @param aMessage - Reference to the message that contains the request
        */
        void ServiceL( const RMessage2& aMessage );

        /**
        * Called back by the server framework if this 
        * session's RunL function returns an error.
        * @param aMessage - Reference to the message that contains the request
        * @param aError - Error code.
        */
        void ServiceError( const RMessage2& aMessage, TInt aError );

        /**
        * Completes construction of this server-side 
        * client session object.
        */
        void CreateL(); 


    public: // from MIAUpdateRequestObserver
    
        /**
         * @see MIAUpdateRequestObserver::RequestCompleted
         */
        void RequestCompleted( CIAUpdateResult* aResult, 
                               TInt aCountOfAvailableUpdates,
                               TBool aUpdateNow,            
                               TInt aError );


    private:

        /**
        * Returns reference of the server.
        * @return Reference of the server.
        */
        CIAUpdateServer& Server();

        /**
        * Handles the request.
        * @param aMessage - Reference to the message that contains the request
        */
        void HandleRequestL( const RMessage2& aMessage );


    private: // From CApaAppServiceBase

        /**
        * Virtual framework function that is called 
        * on receipt of a message from the client. This allows the service 
        * implementation to define a security policy for messages from the client. 
        * @param aMessage - Reference to the message that contains the request
        */
        CPolicyServer::TCustomResult SecurityCheckL( const RMessage2& aMsg, 
                                                     TInt& aAction, 
                                                     TSecurityInfo& aMissing );        

    private:    // Data

        RMessage2* iMessage;
        
        CEikonEnv* iEikEnv; //not owned
        
                   
    };


#endif      // IAUPDATESESSION_H  
            
// End of File
