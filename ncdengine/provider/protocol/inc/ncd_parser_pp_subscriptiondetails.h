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
* Description:   CNcdPreminetProtocolSubscriptionDetailsParser declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_PARSER_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_PARSER_H

#include "ncdsubparser.h"

class MNcdPreminetProtocolSubscriptionDetails;
class CNcdPreminetProtocolSubscriptionDetailsImpl;

/**
 * Parser for 'subscriptionDetails' element
 */
class CNcdPreminetProtocolSubscriptionDetailsParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolSubscriptionDetailsParser* NewL( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth,
        const Xml::RTagInfo& aElement,
        const Xml::RAttributeArray& aAttributes,
        const TDesC& aParentId );
    
    virtual ~CNcdPreminetProtocolSubscriptionDetailsParser();


public:

    MNcdPreminetProtocolSubscriptionDetails* SubscriptionDetails();

private:
    CNcdPreminetProtocolSubscriptionDetailsParser( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth );
    
    void ConstructL(const Xml::RTagInfo& aElement,
                    const Xml::RAttributeArray& aAttributes,
                    const TDesC& aParentId );

protected:
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );

    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );

    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

protected:

    CNcdPreminetProtocolSubscriptionDetailsImpl* iSubscriptionDetails;
    
    };

#endif //NCD_PREMINET_PROTOCOL_SUBSCRIPTIONREPORT_PARSER_H
