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
* Description:   CNcdTopLevelParser declaration
*
*/


#ifndef NcdTOPLEVELPARSER_H
#define NcdTOPLEVELPARSER_H

#include "ncdsubparser.h"

/**
 * Top level parser. It is the first one to receive xml data and deletages
 * it to subparsers.
 */
class CNcdTopLevelParser : public CNcdSubParser
    {
public:
    static CNcdTopLevelParser* NewL( MNcdParserObserverBundle& aObservers, 
                                     MNcdSubParserObserver& aSubParserObserver,
                                     TInt aDepth );
    virtual ~CNcdTopLevelParser();
private:
    CNcdTopLevelParser( MNcdParserObserverBundle& aObservers, 
                        MNcdSubParserObserver& aSubParserObserver,
                        TInt aDepth );
    void ConstructL();
private:
    virtual void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, 
                                 TInt aErrorCode) ;
    virtual void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) ;
    virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);

    virtual void StartL( const Xml::RTagInfo& aElement, 
                         const Xml::RAttributeArray& aAttributes, 
                         TInt aErrorCode );
private:

    };

#endif
