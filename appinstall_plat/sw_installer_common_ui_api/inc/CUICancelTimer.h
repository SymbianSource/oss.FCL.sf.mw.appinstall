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
* Description:   This file contains the header file of the CCCUICancelTimer 
*                class 
*
*/


#ifndef CCUICANCELTIMER_H
#define CCUICANCELTIMER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CEikonEnv;

namespace SwiUI
{
namespace CommonUI
{
    
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* 
*
* @since 3.0
*/
class MCUICancellable
    {
    public:
    
        /**
        * Indicates if a modal dialog is currently shown.
        * @since 3.0
        */
        virtual TBool IsShowingDialog() = 0;

        /**
        * Cancel engine.
        * @since 3.0
        */
        virtual void CancelEngine() = 0;

        /**
        * Perform forced cancel. Called when nothing else has worked.
        * @since 3.0
        */
        virtual void ForceCancel() = 0; 

        /**
        * Called to indicate that the cancelling progress has been started.
        * @since 3.0
        */
        virtual void StartedCancellingL() = 0;        
    };

/**
*
*
* @since 3.0
*/
class CCUICancelTimer : public CTimer
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CCUICancelTimer* NewL( MCUICancellable* aCancellable );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCUICancelTimer();

    public: // New functions

        /**
        * Starts the cancelling process.
        * @since 3.0
        */
        IMPORT_C void StartCancelling();        
       
    protected: // Functions from base classes

        /**
        * From CPeriodic, Called when timer has triggered.
        * @since 3.0
        */
        void RunL();

        /**
        * From CActive, Handles a leave occurring in the request completion 
        * event handler RunL().
        * @since 3.0
        */        
        TInt RunError( TInt aError );    
      
    private: // New functions

        /**
        * C++ default constructor.
        * @param aCancellable - 
        */
        CCUICancelTimer( MCUICancellable* aCancellable );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
                        
    private: // Data

        MCUICancellable* iCancellable;  
        TInt iTryCount;  
        CEikonEnv* iEikEnv;        
    };
}
}

#endif      // CCUIINSTALLER_H   
            
// End of File
