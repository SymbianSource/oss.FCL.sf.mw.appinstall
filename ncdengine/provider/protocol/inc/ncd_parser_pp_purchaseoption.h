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
* Description:   CNcdPreminetProtocolPurchaseOptionParser declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_PURCHASEOPTION_PARSER_H
#define NCD_PREMINET_PROTOCOL_PURCHASEOPTION_PARSER_H

#include "ncdsubparser.h"

class CNcdPreminetProtocolPurchaseOptionImpl;
class MNcdPreminetProtocolPurchaseOption;

/**
 * Parser for 'purchase' element
 */
class CNcdPreminetProtocolPurchaseOptionParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolPurchaseOptionParser* NewL( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth,
        const Xml::RTagInfo& aElement,
        const Xml::RAttributeArray& aAttributes,
        const TDesC& aParentId );
    
    virtual ~CNcdPreminetProtocolPurchaseOptionParser();

    /**
     * @param aParentId Parent id for the entity being generated. 
     *                  Set to KNullDesC8 if no parent.
     */
//     virtual void StartL( const Xml::RTagInfo& aElement, 
//                          const Xml::RAttributeArray& aAttributes, 
//                          TInt aErrorCode,
//                          const TDesC8& aParentId );

public:

    /**
     * Retrieves the finished purchase option object. Ownership is transferred.
     * Valid when the parser is finished.
     * @return Purchase option data or 0.
     */
    MNcdPreminetProtocolPurchaseOption* PurchaseOption();



private:
    CNcdPreminetProtocolPurchaseOptionParser( 
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
    /**
     * The data object being built
     */
    CNcdPreminetProtocolPurchaseOptionImpl* iPurchaseOption;
    
    //HBufC8* iParentTag;
    };

#endif //NCD_PREMINET_PROTOCOL_PURCHASE_PARSER_H
