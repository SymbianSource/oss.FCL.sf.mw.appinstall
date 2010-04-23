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
* Description:   CNcdPreminetProtocolSubscriptionParser declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTION_PARSER_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTION_PARSER_H

#include "ncdsubparser.h"

class CNcdPreminetProtocolSubscriptionImpl;
class MNcdPreminetProtocolSubscription;

/**
 * Parser for 'subscription' element
 */
class CNcdPreminetProtocolSubscriptionParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolSubscriptionParser* NewL( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth,
        const Xml::RTagInfo& aElement,
        const Xml::RAttributeArray& aAttributes,
        const TDesC& aParentId,
        const TDesC& aResponseNamespace );
    
    virtual ~CNcdPreminetProtocolSubscriptionParser();


public:

    MNcdPreminetProtocolSubscription* Subscription();

private:
    CNcdPreminetProtocolSubscriptionParser( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth );
    
    void ConstructL(const Xml::RTagInfo& aElement,
                    const Xml::RAttributeArray& aAttributes,
                    const TDesC& aParentId,
                    const TDesC& aResponseNamespace );

protected:
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );

    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );

    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

protected:

    HBufC8* iParentTag;
    CNcdPreminetProtocolSubscriptionImpl* iSubscription;
    
    };

#endif //NCD_PREMINET_PROTOCOL_SUBSCRIPTION_PARSER_H
