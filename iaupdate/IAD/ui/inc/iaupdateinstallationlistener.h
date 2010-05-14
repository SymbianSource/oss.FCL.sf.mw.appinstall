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
* Description:   This file contains the header file of the CIAUpdateInstallationListener
*                class 
*
*/


#ifndef __IAUPDATE_INSTALLATIONLISTENER_H__
#define __IAUPDATE_INSTALLATIONLISTENER_H__

// INCLUDES
#include <e32base.h>
#include <eikenv.h>

// FORWARD DECLARATIONS
class MIAUpdateRefreshObserver;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateInstallationListener : public CActive
    {
public:

    /**
    * Construct a CIAUpdateInstallationListener using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateInstallationListener
    */
    static CIAUpdateInstallationListener* NewL();
    
    /**
    * Construct a CIAUpdateInstallationListener using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateInstallationListener
    */
    static CIAUpdateInstallationListener* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateInstallationListener();

public: // new functions
        
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
    CIAUpdateInstallationListener();
    
    /**
     * Perform the second phase construction of a CIAUpdateInstallationListener object
     */
    void ConstructL();
    
private: //data

    RProperty iProperty;
    
    MIAUpdateRefreshObserver* iObserver;
    
    };


#endif // __IAUPDATE_REFRESHHANDLER_H__
