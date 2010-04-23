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
* Description:   CNcdRequestManageSubscription implementation
*
*/


#include "ncdrequestmanagesubscriptions.h"
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolwords.h"

#include "catalogsdebug.h"

CNcdRequestManageSubscriptions* CNcdRequestManageSubscriptions::NewL()
    {
    CNcdRequestManageSubscriptions* self = CNcdRequestManageSubscriptions::NewLC( );
    CleanupStack::Pop();
    return self;
    }

CNcdRequestManageSubscriptions* CNcdRequestManageSubscriptions::NewLC()
    {
    CNcdRequestManageSubscriptions* self = new (ELeave) CNcdRequestManageSubscriptions();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CNcdRequestManageSubscriptions::ConstructL()
    {
    CNcdRequestBase::ConstructL( KTagPreminetRequest );
    iName.SetL(KTagManageSubscriptions);
    iNamespaceUri.SetL(KDefaultNamespaceUri);
    iPrefix = TXmlEngString();
    iVersion.SetL(KAttrPreminetVersion);
    }
    
CNcdRequestManageSubscriptions::CNcdRequestManageSubscriptions()
: CNcdRequestBase()
    {
    }

CNcdRequestManageSubscriptions::~CNcdRequestManageSubscriptions()
    {
    DLTRACEIN((""));
    iName.Free();
    iNamespaceUri.Free();
    iPrefix.Free();
    iSubscriptions.ResetAndDestroy();
    
    DLTRACEOUT((""));
    }

void CNcdRequestManageSubscriptions::AddSubscriptionL( 
    const TDesC& aEntityId, const TDesC& aPurchaseOptionId, 
    const TNcdSubscriptionOperation aOperation ) 
    {
    DLTRACEIN((""));
    CNcdSubscriptionManagement* sub = new (ELeave) CNcdSubscriptionManagement;
    CleanupStack::PushL(sub);
    sub->ConstructL();
    NcdProtocolUtils::AssignDesL(sub->iEntityId, aEntityId);
    NcdProtocolUtils::AssignDesL(sub->iPurchaseOptionId, aPurchaseOptionId);
    sub->iOperation = aOperation;
    iSubscriptions.Append(sub);
    CleanupStack::Pop();
    DLTRACEOUT((""));
    }
    

// generates the dom nodes
HBufC8* CNcdRequestManageSubscriptions::CreateRequestL()
    {
    DLTRACEIN((""));
    // generate browse part of the request
    NcdProtocolUtils::NewAttributeL( iRoot, KAttrVersion, iVersion);
    iRequestElement = NcdProtocolUtils::NewElementL(
        iDocument, KTagManageSubscriptions);
    if (iConfiguration)
        iConfiguration->SetNamespacePrefixL( KAttrCdpNamespacePrefix );
    
    for ( TInt i = 0; i < iSubscriptions.Count(); ++i )
        {
        CNcdSubscriptionManagement* sub = iSubscriptions[i];
        TXmlEngElement subscription  = 
            NcdProtocolUtils::NewElementL(iDocument, iRequestElement, KTagSubscription);
        NcdProtocolUtils::NewAttributeL(subscription, KAttrEntityId, sub->EntityId());
        NcdProtocolUtils::NewAttributeL(subscription, KAttrPurchaseOptionId, sub->PurchaseOptionId());
        if ( sub->Operation() == EStatus ) 
            {
            NcdProtocolUtils::NewAttributeL(
                subscription, KAttrOperation, KValueStatus );
            }
        else if ( sub->Operation() == EUnsubscribe ) 
            {
            NcdProtocolUtils::NewAttributeL(
                subscription, KAttrOperation, KValueUnsubscribe );
            }
        else 
            {
            DASSERT( 0 );
            }
        }
        
    DLTRACEOUT((""));
    
    // base class generates the complete request
    return CNcdRequestBase::CreateRequestL();
    }

    
void CNcdSubscriptionManagement::ConstructL() 
    {
    NcdProtocolUtils::AssignEmptyDesL(iEntityId);
    NcdProtocolUtils::AssignEmptyDesL(iPurchaseOptionId);
    }
    
CNcdSubscriptionManagement::~CNcdSubscriptionManagement() 
    {
    delete iEntityId;
    delete iPurchaseOptionId;
    }

CNcdSubscriptionManagement::CNcdSubscriptionManagement() 
: CBase()
    {
    }
    
const TDesC& CNcdSubscriptionManagement::EntityId() const
    {
    return *iEntityId;
    }

const TDesC& CNcdSubscriptionManagement::PurchaseOptionId() const
    {
    return *iPurchaseOptionId;
    }

TNcdSubscriptionOperation CNcdSubscriptionManagement::Operation() const 
    {
    return iOperation;
    }
