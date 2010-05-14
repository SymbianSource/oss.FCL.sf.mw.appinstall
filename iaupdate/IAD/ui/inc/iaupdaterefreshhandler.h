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
* Description:   This file contains the header file of the CIAUpdateRefreshHandler
*                class 
*
*/


#ifndef __IAUPDATE_REFRESHHANDLER_H__
#define __IAUPDATE_REFRESHHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <eikenv.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MIAUpdateRefreshObserver;
class CIAUpdateInstallationListener;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateRefreshHandler : public CActive
    {
public:

    /**
    * Construct a CIAUpdateRefreshHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateRefreshHandler
    */
    static CIAUpdateRefreshHandler* NewL();
    
    /**
    * Construct a CIAUpdateRefreshHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateRefreshHandler
    */
    static CIAUpdateRefreshHandler* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateRefreshHandler();

public: // new functions
    
    /**
    *
    */ 
    void InformRefreshL();
    
    /**
    *
    */
    void StartListeningL( MIAUpdateRefreshObserver* iObserver );
    
private: // CActive

    /**
     * @see CActive::DoCancel
     */
    void DoCancel();

    /**
     * @see CActive::RunL
     */
    void RunL();
   
    /**
     * @see CActive::RunError
     */ 
    TInt RunError( TInt aError );

private:

    /**
    * C++ constructor 
    */
    CIAUpdateRefreshHandler();
    
    /**
     * Perform the second phase construction of a CIAUpdateRefreshHandler object
     */
    void ConstructL();
    
private: //data

    CEikonEnv* iEikEnv; //not owned

    RProperty iProperty;
    
    MIAUpdateRefreshObserver* iObserver;
    
    CIAUpdateInstallationListener* iInstallationListener;
    
    };


#endif // __IAUPDATE_REFRESHHANDLER_H__
