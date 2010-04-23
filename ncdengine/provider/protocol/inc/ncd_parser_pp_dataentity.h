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
* Description:   CNcdEntityRefParser declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_DATAENTITY_PARSER_H
#define NCD_PREMINET_PROTOCOL_DATAENTITY_PARSER_H

#include "ncdsubparser.h"

class MNcdPreminetProtocolDataEntity;
class CNcdPreminetProtocolDataEntityImpl;
class MNcdPreminetProtocolPurchaseOption;

/**
 * Parser for 'dataEntity' element
 */
class CNcdPreminetProtocolDataEntityParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolDataEntityParser* NewL( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth,
        const Xml::RTagInfo& aElement,
        const Xml::RAttributeArray& aAttributes,
        const TDesC& aNamespace );
    
    virtual ~CNcdPreminetProtocolDataEntityParser();

    MNcdPreminetProtocolDataEntity* DataEntity();

private:
    CNcdPreminetProtocolDataEntityParser( 
        MNcdParserObserverBundle& aObservers, 
        MNcdSubParserObserver& aSubParserObserver,
        TInt aDepth );
    
    void ConstructL(const Xml::RTagInfo& aElement,
                    const Xml::RAttributeArray& aAttributes,
                    const TDesC& aNamespace );

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
    CNcdPreminetProtocolDataEntityImpl* iDataEntity;

    /**
     * Namespace as defined by the response, to be used as default.
     */
    HBufC* iNamespace;
    };

#endif //NCD_PREMINET_PROTOCOL_DATAENTITY_PARSER_H
