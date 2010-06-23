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
* Description:   This file contains the header file of the CIAUpdateGlobalLockHandler
*                class 
*
*/


#ifndef __IAUPDATE_GLOBALLOCKHANDLER_H__
#define __IAUPDATE_GLOBALLOCKHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <eikenv.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
*
*/

class CIAUpdateGlobalLockHandler : public CBase
    {
public:

    /**
    * Construct a CIAUpdateGlobalLockHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateGlobalLockHandler
    */
    static CIAUpdateGlobalLockHandler* NewL();
    
    /**
    * Construct a CIAUpdateGlobalLockHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateGlobalLockHandler
    */
    static CIAUpdateGlobalLockHandler* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateGlobalLockHandler();

public: // new functions
    
    /**
     * Check if another IAD instance is just performing operation
     */ 
    TBool InUseByAnotherInstanceL();
    
    /**
     * Set in use flag to another instances on/off
     *
     * @param aInUse True value when operation (update,refresh) is ongoing
     */ 
    void SetToInUseForAnotherInstancesL( TBool aInUse );

private:

    /**
    * C++ constructor 
    */
    CIAUpdateGlobalLockHandler();
    
    /**
     * Perform the second phase construction of a CIAUpdateGlobalLockHandler object
     */
    void ConstructL();
    
    private: //data

    CEikonEnv* iEikEnv; //not owned
    
    TBool iSetAsLocked;
    
    };


#endif // __IAUPDATE_GLOBALLOCKHANDLER_H__
