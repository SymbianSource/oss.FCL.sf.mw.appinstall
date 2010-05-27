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
* Description:   This file contains the header file of the CSWInstSession class 
*
*/


#ifndef SWINSTSESSION_H
#define SWINSTSESSION_H

//  INCLUDES
#include <AknServerApp.h>
#include <SWInstDefs.h>

#include "SWInstRequestObject.h"
#include "CUICancelTimer.h"

namespace SwiUI
{

// FORWARD DECLARATIONS
class CSWInstRequestStore;
class CSWInstServer;
class MRequestCallback;

// CLASS DECLARATION

/**
* This class represents the main server session.
*
* @since 3.0
*/
class CSWInstSession : public CAknAppServiceBase, public MRequestCallback,
                       public CommonUI::MCUICancellable
    {
    public:  // Constructors and destructor
     
        /**
        * C++ default constructor.
        */
        CSWInstSession();

        /**
        * Destructor.
        */
        virtual ~CSWInstSession();

    public: // New functions

        /**
        * Prepare the session for exit.
        * @since 3.0
        * @param aExitCallback - Will be called when the session is ready for exit.
        */
        void PrepareForExit( MRequestCallback* aExitCallback );       

    public: // Functions from base classes

        /**
        * From CAknAppServiceBase, Handles the received message.
        * The method simply calls HandleRequestL under trap harness. 
        * @since 3.0
        */
        void ServiceL( const RMessage2& aMessage );

        /**
        * From CAknAppServiceBase, Called back by the server framework if this 
        * session's RunL function returns an error.
        * @since 3.0
        */
        void ServiceError( const RMessage2& aMessage, TInt aError );

        /**
        * From CAknAppServiceBase, Completes construction of this server-side 
        * client session object.
        * @since 3.0
        */
        void CreateL(); 

        /**
        * From MRequestCallback, Called when request is completed.
        * @since 3.0
        */
        void RequestCompleted( TInt aResult );
        
    public:
                   
        /**
        * From MCUICancellable.       
        * @since 3.2
        * @return Boolean ETrue.
        */        
        TBool IsShowingDialog(); 
        
        /**
        * From MCUICancellable.      
        * @since 3.2
        */        
        void CancelEngine(); 
        
        /**
        * From MCUICancellable.      
        * @since 3.2
        */        
        void ForceCancel();
        
        /**
        * From MCUICancellable.      
        * @since 3.2
        */        
        void StartedCancellingL();          

    private:

        /**
        * Returns reference to the server.
        * @since 3.0
        * @return Reference to the server.
        */
        CSWInstServer& Server();

        /**
        * Handles the request.
        * @since 3.0
        * @param aMessage - Reference to the message that contains the request.
        */
        void HandleRequestL( const RMessage2& aMessage );

        /**
        * Helper to retrieve an int parameter from message.
        * @since 3.0
        * @param aMessage - Reference to the message that contains the request.
        * @param aIpcSlot - Slot where the parameter is retrieved.
        */
        TInt GetInt( const RMessage2& aMessage, TInt aIpcSlot ); 
        
        /**
        * Sends application to background.
        * @since 3.2        
        */        
        void SendAppToBackgroundL();  
        
        /**
        * Get parameters from descriptor. Adds url to iHeapSourceUrl
        * @since 3.1
        * @param aParamsDes - Descriptor containing inst. params and url.
        * @param aParams - Install params read form given descriptor.
        */        
        void GetReqParamsL( TDesC8& aParamsDes, TInstallReq& aParams );
        	                                                  

    private: // Functions from base classes

        /**
        * From CApaAppServiceBase, Virtual framework function that is called 
        * on receipt of a message from the client. This allows the service 
        * implementation to define a security policy for messages from the client. 
        * @since 3.0
        */
        CPolicyServer::TCustomResult SecurityCheckL( const RMessage2& aMsg, 
                                                     TInt& aAction, 
                                                     TSecurityInfo& aMissing );        

    private:    // Data

        CSWInstRequestStore* iRequestStore;     

        MRequestCallback* iExitCallback;   
        TBool iAppInForeground;
        CommonUI::CCUICancelTimer* iCanceller; 
        HBufC* iHeapSourceUrl;
    };
}

#endif      // SWINSTSESSION_H   
            
// End of File
