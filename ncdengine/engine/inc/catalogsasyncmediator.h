/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   
*
*/


#ifndef C_CATALOGSASYNCMEDIATOR_H
#define C_CATALOGSASYNCMEDIATOR_H

#include <e32base.h>

/**
 * Used to call a callback after an asynchronous service completes
 * but before the real client of the asynchronous service is notified
 * of completion
 *
 */
class CCatalogsAsyncMediator : public CActive
    {
public:

    /**
     * Constructor
     *
     * @param aStatus Client's status
     * @param aCallback Callback method that is called when this object completes
     */          
    CCatalogsAsyncMediator( 
        TRequestStatus& aStatus, 
        TCallBack aCallback );
    
    /**
     * DO NOT DELETE this object before the real request has been completed
     */
    virtual ~CCatalogsAsyncMediator();

    /**
     * Activates this object
     */
    void Activate();
    
protected:

    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError( TInt aError );    

private:

    TRequestStatus& iClientStatus;    
    TCallBack iCallback;
    };

#endif // C_CATALOGSASYNCMEDIATOR_H
