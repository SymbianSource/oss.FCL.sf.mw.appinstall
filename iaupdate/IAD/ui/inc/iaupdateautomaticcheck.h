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
* Description:   This file contains the header file of the CIAUpdateAutomaticCheck
*                class 
*
*/


#ifndef __IAUPDATE_AUTOMATICCHECK_H__
#define __IAUPDATE_AUTOMATICCHECK_H__

// INCLUDES
#include <e32base.h>
#include "iaupdatedialogobserver.h"

class HbAction;
class IAUpdateDialogUtil;
// CLASS DECLARATION
/**
*
*/

class CIAUpdateAutomaticCheck : public CBase,
                                public IAUpdateDialogObserver
    {
public:

    /**
    * Construct a CIAUpdateAutomaticCheck using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateAutomaticCheck
    */
    static CIAUpdateAutomaticCheck* NewL();
    
    /**
    * Construct a CIAUpdateAutomaticCheck using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateAutomaticCheck
    */
    static CIAUpdateAutomaticCheck* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateAutomaticCheck();

public: // new functions
    
    /**
    * Checks that automatic update checks using network connetion is  accepted by an user
    *
    * @return True value if automatic update connections are allowed, EFalse if not allowed.
    *         Returns always true if this function is called after first time
    */
    void AcceptAutomaticCheckL();
    
    TBool AutoUpdateCheckEnabledL();
        
private:

    /**
    * C++ constructor 
    */
    CIAUpdateAutomaticCheck();
    
    /**
     * Perform the second phase construction of a CIAUpdateAutomaticCheck object
     */
    void ConstructL();
    
    /**
    * Enables automatic updates cheks
    */
    void EnableAutoUpdateCheckL( TBool aEnable );

private: // From IAUpdateDialogObserver     
         
     void dialogFinished(HbAction *action);       
    
        
private: //data
    IAUpdateDialogUtil *mDialogUtil;
    HbAction *mPrimaryAction;
    };


#endif // __IAUPDATE_AUTOMATICCHECK_H__
