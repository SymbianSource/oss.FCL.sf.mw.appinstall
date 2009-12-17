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
* Description:   CNcdRequestPurchase declaration
*
*/


#ifndef C_NCD_REQUEST_MANAGESUBSCRIPTION_HH
#define C_NCD_REQUEST_MANAGESUBSCRIPTION_HH

#include <e32std.h>
#include <e32base.h>
#include "ncdrequestbase.h"
#include "ncdrequestconstants.h"
#include "ncdprotocoltypes.h"

class CNcdSubscriptionManagement : public CBase
    {
public:
    void ConstructL();
    ~CNcdSubscriptionManagement();
    CNcdSubscriptionManagement();
    const TDesC& EntityId() const;
    const TDesC& PurchaseOptionId() const;
    TNcdSubscriptionOperation Operation() const;
public:    
    HBufC* iEntityId;
    HBufC* iPurchaseOptionId;
    TNcdSubscriptionOperation iOperation;
    };


class CNcdRequestManageSubscriptions : public CNcdRequestBase
    {

public:


    HBufC8* CreateRequestL();
    
    
public:
    static CNcdRequestManageSubscriptions* NewL();
    static CNcdRequestManageSubscriptions* NewLC();

    void ConstructL();
    
    ~CNcdRequestManageSubscriptions();


    void AddSubscriptionL( const TDesC& aEntityId, 
        const TDesC& aPurchaseOptionId, 
        const TNcdSubscriptionOperation aOperation );
        
private:
    CNcdRequestManageSubscriptions();

        

private:
    
    TXmlEngString iName;
    TXmlEngString iNamespaceUri;
    TXmlEngString iPrefix;
    
    RPointerArray<CNcdSubscriptionManagement> iSubscriptions;
    };

#endif //C_NCD_REQUEST_MANAGESUBSCRIPTION_HH
    