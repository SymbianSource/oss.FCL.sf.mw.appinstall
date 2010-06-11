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


#ifndef NcdPREMINETPROTOCOLENTITYREFPARSER_H
#define NcdPREMINETPROTOCOLENTITYREFPARSER_H

#include "ncdsubparser.h"

class CNcdPreminetProtocolEntityRefImpl;
class MNcdPreminetProtocolFolderRef;
class MNcdPreminetProtocolItemRef;

class CNcdPreminetProtocolFolderRefImpl;
class CNcdPreminetProtocolItemRefImpl;

/**
 * Parser for 'entity' element
 */
class CNcdPreminetProtocolEntityRefParser : public CNcdSubParser
    {
public:
    static CNcdPreminetProtocolEntityRefParser* NewL( MNcdParserObserverBundle& aObservers, 
                                                      MNcdSubParserObserver& aSubParserObserver,
                                                      TInt aDepth,
                                                      const Xml::RTagInfo& aElement,
                                                      const Xml::RAttributeArray& aAttributes,
                                                      const TDesC& aParentId,
                                                      const TDesC& aParentNamespace );
    virtual ~CNcdPreminetProtocolEntityRefParser();

public:
    
    /**
     * Retrieves the finished folder ref. Ownership is transferred.
     * Valid when the parser is finished.
     * @return Folder ref data or 0.
     */
    MNcdPreminetProtocolFolderRef* FolderRef();
    /**
     * Retrieves the finished item ref. Ownership is transferred.
     * Valid when the parser is finished.
     * @return Item ref data or 0.
     */
    MNcdPreminetProtocolItemRef* ItemRef();

private:
    CNcdPreminetProtocolEntityRefParser( MNcdParserObserverBundle& aObservers, 
                                         MNcdSubParserObserver& aSubParserObserver,
                                         TInt aDepth );
    void ConstructL(const Xml::RTagInfo& aElement,
                    const Xml::RAttributeArray& aAttributes,
                    const TDesC& aParentId,
                    const TDesC& aParentNamespace );
protected:
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );
    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

private:
    HBufC* iParentNamespace;

    /**
     * The data object being built
     */
//     MNcdPreminetProtocolEntityRefImpl* iEntityRef;
    CNcdPreminetProtocolItemRefImpl* iItemRef;
    CNcdPreminetProtocolFolderRefImpl* iFolderRef;
    

    };

#endif
