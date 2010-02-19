/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*                CIAUpdateRoamingHandler class 
*
*/


#ifndef __IAUPDATE_ROAMING_HANDLER_H__
#define __IAUPDATE_ROAMING_HANDLER_H__

// INCLUDES
#include <e32base.h>

#include "iaupdatenetworkregistrationobserver.h"

// FORWARD DECLARATIONS
class MIAUpdateRoamingHandlerObserver;
class CIAUpdateNetworkRegistration;

// CLASS DECLARATION
/**
*
*/

class CIAUpdateRoamingHandler : public CBase, 
                                public MIAUpdateNetworkRegistrationObserver
    {
public:

    /**
    * Construct a CIAUpdateRoamingHandler using two phase construction,
    * and return a pointer to the created object
    */
    static CIAUpdateRoamingHandler* NewL();

    /**
    * Destructor
    */
    ~CIAUpdateRoamingHandler();

public: // from MIAUpdateNetworkRegistrationObserver    

    /**
    * Called when status is monitored asynchronously
    *
    * @param aRoaming True value when roaming network 
    */
    void StatusMonitored( TBool aRoaming );

    

public: // new functions
    /**
    * Prepare handler asyncronously
    *
    * @param aObserver Observer interface to be called when async 
    *                  preparing is completed
    */
    void PrepareL( MIAUpdateRoamingHandlerObserver& aObserver );
    
    /**
    * Cancel async PrepareL() request
    */
    void CancelPreparing(); 
    
    /**
    * 
    */
    TBool Prepared() const;
        
    /**
    * Is roaming network
    * @return True value if in roaming network
    */
    TBool IsRoaming();
    

private:

    /**
    * Perform the second phase construction of a CIAUpdateRoamingHandler object
    */
    void ConstructL();
  
    /**
    * C++ constructor 
    */
    CIAUpdateRoamingHandler();
  
private: //data
    
    CIAUpdateNetworkRegistration* iNwReg;     
	
	MIAUpdateRoamingHandlerObserver* iObserver; //not owned
	
	TBool iRoaming;
	
	TBool iPreparing;
	
	TBool iPrepared;

    };


#endif // __IAUPDATE_ROAMING_HANDLER_H__
