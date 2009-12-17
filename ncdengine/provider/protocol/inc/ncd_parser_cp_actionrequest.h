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
* Description:   CNcdConfigurationProtocolClientConfigurationParser declaration
*
*/


#ifndef C_NCDCONFIGURATIONPROTOCOLACTIONREQUESTPARSER_H
#define C_NCDCONFIGURATIONPROTOCOLACTIONREQUESTPARSER_H

#include "ncdsubparser.h"

class MNcdConfigurationProtocolActionRequest;
class CNcdConfigurationProtocolActionRequestImpl;

_LIT8( KNcdPreminetProtocolTypeAdd, "add" );
_LIT8( KNcdPreminetProtocolTypeRemove, "remove" );
_LIT8( KNcdPreminetProtocolTypeClear, "clear" );
_LIT8( KNcdPreminetProtocolTypeUpdate, "update" );

class CNcdConfigurationProtocolActionRequestParser : public CNcdSubParser                                                           
    {
public:
    static CNcdConfigurationProtocolActionRequestParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                               MNcdSubParserObserver& aSubParserObserver,
                                                               TInt aDepth,
                                                               const Xml::RTagInfo& aElement,
                                                               const Xml::RAttributeArray& aAttributes );
    ~CNcdConfigurationProtocolActionRequestParser();

    MNcdConfigurationProtocolActionRequest* ActionRequest();

private:
    CNcdConfigurationProtocolActionRequestParser( MNcdParserObserverBundle& aObservers, 
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
     * The data object being built
     */
    CNcdConfigurationProtocolActionRequestImpl* iActionRequest;
    };

#endif
