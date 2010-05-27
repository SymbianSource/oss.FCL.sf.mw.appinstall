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
* Description:   CNcdConfigurationProtocolDetailsParser declaration
*
*/


#ifndef C_NCDCONFIGURATIONPROTOCOLCAPABILITIESPARSER_H
#define C_NCDCONFIGURATIONPROTOCOLCAPABILITIESPARSER_H

#include "ncdsubparser.h"

class CNcdConfigurationProtocolCapabilitiesParser : public CNcdSubParser
    {
public:
    static CNcdConfigurationProtocolCapabilitiesParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                              MNcdSubParserObserver& aSubParserObserver,
                                                              TInt aDepth,
                                                              const Xml::RTagInfo& aElement,
                                                              const Xml::RAttributeArray& aAttributes );
    virtual ~CNcdConfigurationProtocolCapabilitiesParser();
    CArrayPtr<HBufC>* Capabilities();

private:
    CNcdConfigurationProtocolCapabilitiesParser( MNcdParserObserverBundle& aObservers, 
                                                 MNcdSubParserObserver& aSubParserObserver,
                                                 TInt aDepth );
    
    void ConstructL( const Xml::RTagInfo& aElement,
                     const Xml::RAttributeArray& aAttributes );

protected:
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                          const Xml::RAttributeArray& aAttributes, 
                          TInt aErrorCode );
    
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                        TInt aErrorCode );
    
    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );
        
private:
    CArrayPtr<HBufC>* iCapabilities;
    
    };

#endif  //C_NCDCONFIGURATIONPROTOCOLCAPABILITIESPARSER_H
