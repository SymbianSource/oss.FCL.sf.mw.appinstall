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
* Description:   This file contains the header file of the CIAUpdateBGNotifyhandler
*                class 
*
*/


#ifndef __IAUPDATEBGNOTIFYHANDLER_H__
#define __IAUPDATEBGNOTIFYHHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

/**
*  Observer
*/
class MIAUpdateBGNotifyObserver
{
public: // Observer API
    /**
    * Called by CIAUpdateBGNotifyHandler when indicator is to
    * be removed 
    */ 
    virtual void HandleIndicatorRemoveL() = 0;
};
/**
*
*/

class CIAUpdateBGNotifyHandler : public CActive
    {
public:

    /**
    * Construct a CIAUpdateBGNotifyHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateBGNotifyHandler
    */
    static CIAUpdateBGNotifyHandler* NewL();
    
    /**
    * Construct a CIAUpdateBGNotifyHandler using two phase construction,
    * and return a pointer to the created object
    * @return A pointer to the created instance of CIAUpdateBGNotifyHandler
    */
    static CIAUpdateBGNotifyHandler* NewLC();
    
    /**
    * Destructor
    */
    ~CIAUpdateBGNotifyHandler();

public: // new functions
    
    /**
    *
    */
    void StartListeningL( MIAUpdateBGNotifyObserver* iObserver );
    
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
    CIAUpdateBGNotifyHandler();
    
    /**
     * Perform the second phase construction of a CIAUpdateBGNotifyHandler object
     */
    void ConstructL();
    
private: //data

    RProperty iProperty;
    
    MIAUpdateBGNotifyObserver* iObserver;
    
    };


#endif // __IAUPDATEBGNOTIFYHANDLER_H__
