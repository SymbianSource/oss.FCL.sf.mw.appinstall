/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of  CNcdSubscriptionManagerListener class
*
*/


#ifndef C_NCDSUBSCRIPTIONMANAGERLISTENER_H
#define C_NCDSUBSCRIPTIONMANAGERLISTENER_H

#include <e32base.h>

class CNcdSubscriptionManagerProxy;

/**
 * Active object used by CNcdSubscriptionManagerProxy. Listens for
 * server side subscription manager and internalizes the proxy side
 * subscription manager as the server side manager gets updated.
 */
class CNcdSubscriptionManagerListener : public CActive 
{
public:
    static CNcdSubscriptionManagerListener* NewL(
        CNcdSubscriptionManagerProxy& aParent );
    virtual ~CNcdSubscriptionManagerListener();
    
    /**
     * Calls SetActive().
     */
    void Activate();
    
protected:
    CNcdSubscriptionManagerListener(
        CNcdSubscriptionManagerProxy& aParent );

protected: // From CActive
    virtual void DoCancel();
    virtual void RunL();
           
private:
    CNcdSubscriptionManagerProxy& iParent;
};
        
  
        


#endif // C_NCDSUBSCRIPTIONMANAGERLISTENER_H