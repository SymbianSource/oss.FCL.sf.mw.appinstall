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
* Description:   CNcdSubParser declaration
*
*/


#ifndef NcdSUBPARSER_H
#define NcdSUBPARSER_H

#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

#include "ncdparser.h"

/**
 * Subparser observer.
 */
class MNcdSubParserObserver
    {
public:
    virtual ~MNcdSubParserObserver() {}
    /**
     * This is called when the subparser is finished parsing its data.
     * @param aSubParserTag The tag which was parsed by the parser
     * @param aErrorCode Error code in case of parse errors
     */
    virtual void SubParserFinishedL( const TDesC8& aSubParserTag, TInt aErrorCode ) = 0;
    };

/**
 * Subparser base clas.
 */
class CNcdSubParser : public CBase,
                      public Xml::MContentHandler,
                      public MNcdSubParserObserver
    {
public:
    enum TType
        {
        EParserDefaultType,
        EParserUnknown,
        EParserProtocolResponse,
        EParserEntityRef,
        EParserEntityData
        };

public:
    CNcdSubParser( MNcdParserObserverBundle& aObservers,
                   MNcdSubParserObserver& aSubParserObserver,
                   TInt aDepth,
                   TType aType = EParserDefaultType );
    virtual ~CNcdSubParser();

public:
    TType Type() const;
    const TDesC8& Tag() const;
    TInt Depth() const;
    // return KNullDesC8 if not found
    static const TDesC8& AttributeValue( const TDesC8& aAttributeName,
                                         const Xml::RAttributeArray& aAttributes );
    static const TDesC8& AttributePrefix( const TDesC8& aAttributeName,
                                          const Xml::RAttributeArray& aAttributes );



public: // From Xml::MContentHandler

    /**
     * @see Xml::MContentHandler::OnStartDocumentL
     * @note this parent class implementation of OnStartDocumentL
     *  must be called by overriding child class implementations of the function. 
     * 
     */
    virtual void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode) ;
    virtual void OnEndDocumentL(TInt aErrorCode) ;
    virtual void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, 
                                 TInt aErrorCode) ;
    virtual void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) ;
    virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode) ;
    virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
                                       TInt aErrorCode) ;
    virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode) ;
    virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode) ;
    virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
    virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
                                          TInt aErrorCode) ;
    virtual void OnError(TInt aErrorCode);
    virtual TAny* GetExtendedInterface(const TInt32 aUid);

public: // From MNcdSubParserObserver

    virtual void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

protected:
    void ConstructL( const Xml::RTagInfo& aElement );
protected:
    TType iType;
    TInt iDepth;
    HBufC8* iTag;
    HBufC8* iBuffer;
    MNcdParserObserverBundle* iObservers;
    MNcdSubParserObserver* iSubParserObserver;
    CNcdSubParser* iSubParser;
    };

#endif
