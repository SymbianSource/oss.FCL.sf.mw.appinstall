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
* Description:   This file contains the header file of the CIAUpdateLauncherManager class 
*
*/



#ifndef IA_UPDATE_LAUNCHER_MANAGER_H
#define IA_UPDATE_LAUNCHER_MANAGER_H


//INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <coemain.h>

#include "iaupdatelauncherclient.h"

/**
 * 
  */
class CIAUpdateLauncherManager : public CActive
    {

public:

    /**
     * Symbian two phased constructor.
     */
    static CIAUpdateLauncherManager* NewL( TBool& aRefreshFromNetworkDenied );
    
    /**
     * Symbian two phased constructor.
     */
    static CIAUpdateLauncherManager* NewLC( TBool& aRefreshFromNetworkDenied );
    

    /**
     * Destructor
     */
    virtual ~CIAUpdateLauncherManager();
    

    

private: // CActive

    /**
     * @see CActive::DoCancel
     */
    void DoCancel();

    /**
     * When the server side has finished operation, the CActive object will
     * be informed about it, and as a result RunL will be called. This function
     * well inform the observer that the operation has been completed.
     * @see CActive::RunL
     */
    void RunL();


private:

    /**
     * private C++ constructor
     */
    CIAUpdateLauncherManager();
    
    /**
     * @see NewL
     */
    void ConstructL( TBool& aRefreshFromNetworkDenied );

  
private:

    // Prevent these if not implemented
    CIAUpdateLauncherManager( const CIAUpdateLauncherManager& aObject );
    CIAUpdateLauncherManager& operator =( const CIAUpdateLauncherManager& aObject );
    

private: // data
       
    // Handles the client server communication.
    RIAUpdateLauncherClient iUpdateClient;
        
    CEikonEnv* iEikEnv; //not owned

    };

#endif // IA_UPDATE_LAUNCHER_MANAGER_H
