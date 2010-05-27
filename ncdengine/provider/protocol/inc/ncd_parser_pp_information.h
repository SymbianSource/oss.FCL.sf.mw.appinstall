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
* Description:   CNcdPreminetProtocolInformationParser declaration
*
*/


#ifndef C_NCDPREMINETPROTOCOLINFORMATIONPARSER_H
#define C_NCDPREMINETPROTOCOLINFORMATIONPARSER_H

#include "ncdsubparser.h"

class MNcdPreminetProtocolInformation;
class CNcdPreminetProtocolInformationImpl;

class CNcdPreminetProtocolInformationParser : public CNcdSubParser                                                           
    {
public:
    static CNcdPreminetProtocolInformationParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                        MNcdSubParserObserver& aSubParserObserver,
                                                        TInt aDepth,
                                                        const Xml::RTagInfo& aElement,
                                                        const Xml::RAttributeArray& aAttributes );
    virtual ~CNcdPreminetProtocolInformationParser();

    MNcdPreminetProtocolInformation* Information();

private:
    CNcdPreminetProtocolInformationParser( MNcdParserObserverBundle& aObservers, 
                                           MNcdSubParserObserver& aSubParserObserver,
                                           TInt aDepth );
    void ConstructL( const Xml::RTagInfo& aElement,
                     const Xml::RAttributeArray& aAttributes );
protected:
    void OnStartElementL( const Xml::RTagInfo& aElement, 
                          const Xml::RAttributeArray& aAttributes, 
                          TInt aErrorCode );

    void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );
    
    void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

private:
    /**
     * The data object being built
     */
    CNcdPreminetProtocolInformationImpl* iInformation;
    };

#endif
