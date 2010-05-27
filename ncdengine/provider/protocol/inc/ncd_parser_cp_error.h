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
* Description:   CNcdConfigurationProtocolErrorParser declaration
*
*/


#ifndef C_NCDCONFIGURATIONPROTOCOLERRORPARSER_H
#define C_NCDCONFIGURATIONPROTOCOLERRORPARSER_H

#include "ncdsubparser.h"

class MNcdConfigurationProtocolError;
class MNcdPreminetProtocolError;
class CNcdConfigurationProtocolErrorImpl;
class CNcdPreminetProtocolErrorImpl;

class CNcdConfigurationProtocolErrorParser : public CNcdSubParser                                                           
    {
public:
    static CNcdConfigurationProtocolErrorParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                       MNcdSubParserObserver& aSubParserObserver,
                                                       TInt aDepth,
                                                       const Xml::RTagInfo& aElement,
                                                       const Xml::RAttributeArray& aAttributes );

    virtual ~CNcdConfigurationProtocolErrorParser();

    MNcdConfigurationProtocolError* ConfigurationProtocolError();
    MNcdPreminetProtocolError* PreminetProtocolError();

private:
    CNcdConfigurationProtocolErrorParser( MNcdParserObserverBundle& aObservers, 
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
    /**
     * The data object being built. This is actually PP error so that we can use the same parser.
     * PP is CP error by inheritance.
     */
    CNcdPreminetProtocolErrorImpl* iError;
    };

#endif
