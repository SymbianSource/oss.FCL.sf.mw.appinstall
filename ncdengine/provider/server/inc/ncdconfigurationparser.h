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
* Description:   CNcdConfigurationParser declaration
*
*/


#ifndef C_NCDCONFIGURATIONPARSER_H
#define C_NCDCONFIGURATIONPARSER_H

#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

/**
 * Observer interface for a configuration observer
 */ 
class MNcdConfigurationParserObserver
    {
public:
   
    /**
     * Called when starting to parse a new element
     *
     * @param aElement Name of the element
     */ 
    virtual void ConfigurationElementStartL(
        const TDesC8& aElement ) 
        {
        (void) aElement;
        }


    /**
     * Called after an element has been parsed
     *
     * @param aElement Name of the parsed element
     * @param aData Data included in the element
     */    
    virtual void ConfigurationElementEndL( 
        const TDesC8& aElement, 
        const TDesC8& aData ) = 0;

    /**
     * Called for each attribute in an element
     *
     * @param aElement Element that is being parsed
     * @param aAttribute Name of the attribute
     * @param aValue Value of the attribute
     */
    virtual void ConfigurationAttributeL( 
        const TDesC8& aElement, 
        const TDesC8& aAttribute, 
        const TDesC8& aValue ) = 0;
    
    /**
     * Called when a parse error occurs
     */    
    virtual void ConfigurationError( TInt aError ) = 0;

protected:

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationParserObserver()
        {
        }
    };


/**
 * Provider configuration parser
 */
class CNcdConfigurationParser : public CBase,
                                public Xml::MContentHandler
    {
public:

    /**
     * NewL
     * @param aObserver Observer for parsing events
     */
    static CNcdConfigurationParser* NewL( 
        MNcdConfigurationParserObserver& aObserver );


    /**
     * NewLC
     * @param aObserver Observer for parsing events
     */
    static CNcdConfigurationParser* NewLC( 
        MNcdConfigurationParserObserver& aObserver );
    
    /**
     * Destructor
     */
    virtual ~CNcdConfigurationParser();
    
public: // New methods

    /**
     * Parses given data
     *
     * @param aData Data to parse
     */
    void ParseL( const TDesC16& aData );    


    /**
     * Parses given data
     *
     * @param aData Data to parse
     */
    void ParseL( const TDesC8& aData );    
        
public: // From Xml::MContentHandler

    void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, 
        TInt aErrorCode);
        
    void OnEndDocumentL(TInt aErrorCode);
    
    void OnStartElementL(const Xml::RTagInfo& aElement, 
        const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
        
    void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    
    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    
    void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
        TInt aErrorCode) ;
    
    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    
    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    
    void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    
    void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
        TInt aErrorCode);
    
    void OnError(TInt aErrorCode);
    
    TAny* GetExtendedInterface(const TInt32 aUid);

    
private:
    CNcdConfigurationParser( MNcdConfigurationParserObserver& aObserver );                        
                        
    void ConstructL();

private:

    MNcdConfigurationParserObserver& iObserver;
    Xml::CParser* iXmlParser;
    HBufC8* iBuffer;
    };

#endif // C_NCDCONFIGURATIONPARSER_H
