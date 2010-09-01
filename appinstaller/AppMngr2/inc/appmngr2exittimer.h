/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declares UI class for application
*
*/

#ifndef C_APPMNGR2EXITTIMER_H
#define C_APPMNGR2EXITTIMER_H

#include <e32base.h>

class CAppMngr2AppUi;

class CAppMngr2ExitTimer : public CTimer
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAppMngr2ExitTimer* NewL( CAppMngr2AppUi* aAppUi );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAppMngr2ExitTimer();

    public: // New functions

        /**
        * Starts timer for Exit process.
        * @since 9.2
        */
        IMPORT_C void StartExitTimer();        
       
    protected: // Functions from base classes

        /**
        * From CPeriodic, Called when timer has triggered.
        * Calls Exit for the AppMngr.
        * @since 9.2
        */
        void RunL();

        /**
        * From CActive, Handles a leave occurring in the request 
        * completion event handler RunL().
        * @since 9.2
        */        
        TInt RunError( TInt aError );    
      
    private:

        /**
        * C++ default constructor.       
        */
        CAppMngr2ExitTimer( CAppMngr2AppUi* aAppUi );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
                        
    private:

        CAppMngr2AppUi* iAppUi;                
    };

#endif /* APPMNGR2EXITTIMER_H_ */
