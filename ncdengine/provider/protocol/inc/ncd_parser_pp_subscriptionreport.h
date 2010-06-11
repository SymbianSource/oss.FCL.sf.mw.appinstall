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
* Description:   CNcdPreminetProtocolPurchaseParser declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTIONREPORT_PARSER_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTIONREPORT_PARSER_H

#include "ncdsubparser.h"


/**
 * Parser for 'subscriptionReport' element
 */
class CNcdPreminetProtocolSubscriptionReportParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolSubscriptionReportParser* NewL( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth,
        const Xml::RTagInfo& aElement,
        const Xml::RAttributeArray& aAttributes,
        const TDesC& aParentId,
        const TDesC& aResponseNamespace );
    
    virtual ~CNcdPreminetProtocolSubscriptionReportParser();

    /**
     * @param aParentId Parent id for the entity being generated. 
     *                  Set to KNullDesC8 if no parent.
     */
//     virtual void StartL( const Xml::RTagInfo& aElement, 
//                          const Xml::RAttributeArray& aAttributes, 
//                          TInt aErrorCode,
//                          const TDesC8& aParentId );

public:



private:
    CNcdPreminetProtocolSubscriptionReportParser( 
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

    // temporary placeholders for parent tag
    HBufC8* iParentTag;
    HBufC* iResponseNamespace;
    
    };

#endif //NCD_PREMINET_PROTOCOL_SUBSCRIPTIONREPORT_PARSER_H
