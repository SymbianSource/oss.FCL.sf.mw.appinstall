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
* Description:   CNcdConfigurationProtocolDetailParser declaration
*
*/


#ifndef C_CNCDCONFIGURATIONPROTOCOLDETAILPARSER_H
#define C_CNCDCONFIGURATIONPROTOCOLDETAILPARSER_H

#include "ncdsubparser.h"

class MNcdConfigurationProtocolDetail;
class CNcdConfigurationProtocolDetailImpl;
class CNcdConfigurationProtocolContentImpl;

class CNcdConfigurationProtocolDetailParser : public CNcdSubParser
    {
public:
    static CNcdConfigurationProtocolDetailParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                        MNcdSubParserObserver& aSubParserObserver,
                                                        TInt aDepth,
                                                        const Xml::RTagInfo& aElement,
                                                        const Xml::RAttributeArray& aAttributes );
    virtual ~CNcdConfigurationProtocolDetailParser();
    MNcdConfigurationProtocolDetail* Detail();

private:
    CNcdConfigurationProtocolDetailParser( MNcdParserObserverBundle& aObservers, 
                                           MNcdSubParserObserver& aSubParserObserver,
                                           TInt aDepth );
    
    void ConstructL(const Xml::RTagInfo& aElement,
                    const Xml::RAttributeArray& aAttributes );

protected:
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                          const Xml::RAttributeArray& aAttributes, 
                          TInt aErrorCode );
    
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                        TInt aErrorCode );
    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );
        
private:
    CNcdConfigurationProtocolDetailImpl* iDetail;
    CNcdConfigurationProtocolContentImpl* iContent;    
    };

#endif  //C_CNCDCONFIGURATIONPROTOCOLDETAILPARSER_H
