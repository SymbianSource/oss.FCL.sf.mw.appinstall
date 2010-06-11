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
* Description:   This file contains the header file of 
*                CIAUpdateNetworkRegistration class 
*
*/


#ifndef __IAUPDATE_NETWORK_REGISTRATION_H__
#define __IAUPDATE_NETWORK_REGISTRATION_H__

// INCLUDES
#include <e32base.h>
#include <etel3rdparty.h>

// FORWARD DECLARATIONS
class MIAUpdateNetworkRegistrationObserver;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateNetworkRegistration : public CActive
    {
public:

    /**
    * Construct a CIAUpdateNetworkRegistration using two phase construction,
    * and return a pointer to the created object
    * @param aObserver Observer interface to be called when
    *                  network registration status monitoring is completed
    */
    IMPORT_C static CIAUpdateNetworkRegistration* NewL( 
                             MIAUpdateNetworkRegistrationObserver& aObserver );
                                   
    
    /**
    * Destructor
    */
    ~CIAUpdateNetworkRegistration();

public: // new functions
    /**
    * Starts monitoring
    */
    IMPORT_C void StartMonitoringL();
        
private: // from CActive

    /**
     * Cancels async network registration status monitoring 
     * @see CActive::DoCancel
     *
     */
    void DoCancel();

    /**
     * When the server side has finished operation, the CActive object will
     * be informed about it, and as a result RunL will be called. This function
     * well inform the observer that the operation has been completed.
     * @see CActive::RunL
     *
     */
    void RunL();

private:

    /**
    * Perform the second phase construction of a CIAUpdateNetworkRegistration object
    */
    void ConstructL();
  
    /**
    * C++ constructor 
    *
    * @param aObserver Observer interface to be called when
    *                  network registration status monitoring is completed
    */
    CIAUpdateNetworkRegistration( MIAUpdateNetworkRegistrationObserver& aObserver );
  
private: //data
    
    CTelephony* iTelephony;
    
    CTelephony::TNetworkRegistrationV1* iNetReg;
    
    CTelephony::TNetworkRegistrationV1Pckg* iNetRegPkg;
	
	MIAUpdateNetworkRegistrationObserver* iObserver; //not owned
    };


#endif // __IAUPDATE_NETWORK_REGISTRATION_H__
