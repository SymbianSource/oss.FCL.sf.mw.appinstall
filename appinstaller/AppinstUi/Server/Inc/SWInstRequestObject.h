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
* Description:   This file contains the header file of the CSWInstRequestObject 
*                class.
*
*                This class is a base class for request handler objects.
*
*/


#ifndef SWINSTREQUESTOBJECT_H
#define SWINSTREQUESTOBJECT_H

//  INCLUDES
#include <e32base.h>

namespace SwiUI
{

// FORWARD DECLARATIONS
class CSWInstUIPluginAPI;
class CStartupItem;
class CTaskManager;

/**
* Callback interface for observing request completion.
*
* @since 3.0
*/
class MRequestCallback
    {
    public:

        /**
        * Called when request is completed.
        * @param aResult - Result of the request.
        */
        virtual void RequestCompleted( TInt aResult ) = 0;
    };

// CLASS DECLARATION

/**
* This class is a base class for request handler objects.
*
* @since 3.0
*/
class CSWInstRequestObject : public CActive
    {
    public:  // Constructors and destructor
     
        /**
        * C++ default constructor.
        * @param aMessage - Message object having this request. The ipc code of the message
        * will be used as the object's id.
        */
        CSWInstRequestObject( const RMessage2& aMessage );

        /**
        * C++ default constructor.
        * @param aObjectId - Id of the request.
        */
        CSWInstRequestObject( TInt aObjectId );        

        /**
        * Destructor.
        */
        virtual ~CSWInstRequestObject();

    public: // New functions

        /**
        * Returns the message object having this request.
        * @since 3.0
        * @return Message object. If no message object has been set, the value is NULL.
        */
        RMessage2* Message();

        /**
        * Returns the request id of this object.
        * @since 3.0
        * @return Handle of the object.
        */
        TInt ObjectId() const;

        /**
        * Completes the request. 
        * If callback has been set, the callback function will be called.
        * If the ipc message object has been set, the message will be completed. 
        * @since 3.0
        * @param aResult - Result of the request.
        */
        void Complete( TInt aResult );

        /**
        * Sets the request callback handler.
        * @since 3.0
        * @param aCallback - Callback handler.
        */
        void SetCallback( MRequestCallback* aCallback );

        /**
        * Cancel current operation. Asynchronous.
        * @since 3.0
        */
        void Cancel();

        /**
        * Cancel current operation. Asynchronous.
        * @since 3.0
        * @param aMessage - Cancel message. Will be completed after the cancel
        *                   is completed.
        */
        void Cancel( const RMessage2& aMessage );  

    protected: // New functions

        /**
        * 2nd phase constructor.
        */
        void BaseConstructL();        

        /**
        * Creates a UI plugin based on the given mime time. Deriving objects must call this
        * function before accessing the UI plugin!
        * @since 3.0
        * @param aMime - Mime type.
        */
        void CreatePluginL( const TDesC8& aMime ); 

        /**
        * Checks if there is another request object in use globally. Leaves with
        * KSWInstErrBusy if there is.
        * @since 3.0
        */
        void LeaveIfInUseL();        
        
        /**
        * Cancel current request to be implemented by derived classes.
        * @since 3.0
        */
        virtual void CancelRequest() = 0;
        
        /**
        * Gets called when the request is completed.
        * @since 3.0
        * @param aResult - Result of the request.
        */
        virtual void RequestCompleteL( TInt aResult ) = 0;

        /**
        * Completes this active object.
        * @since 3.0
        * @param aResult - Result of the request.
        */
        void CompleteSelf( TInt aResult );        

    private: // Functions from base classes

        /**
        * From CActive, Called by framework when request is finished.
        * @since 3.0
        */
        void RunL();

        /**
        * From CActive, Called by framework when request is cancelled.
        * @since 3.0
        */        
        void DoCancel();

        /**
        * From CActive, Handles a leave occurring in the request completion 
        * event handler RunL().
        * @since 3.0
        */        
        TInt RunError( TInt aError );
                
    protected:  // Data

        CSWInstUIPluginAPI* iUIPlugin; // UI plugin, see CreatePluginL
        TBool iCancelling;
        TBool iIsSilent;        
        CTaskManager* iTaskManager; 

    private: // Data

        RMessage2 iMessage;  // Message object
        TBool iMessageSet;        

        TInt iObjectId;
        MRequestCallback* iCallback;        

        RMutex iSingleInstanceMutex;

        RMessage2 iCancelMessage;  // Cancel message object
        TBool iCancelMessageSet;     
    };
}

#endif      // SWINSTREQUESTOBJECT_H   
            
// End of File
