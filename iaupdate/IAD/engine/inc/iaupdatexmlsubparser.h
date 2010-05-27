/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_XML_SUB_PARSER_H
#define IA_UPDATE_XML_SUB_PARSER_H


#include <e32base.h>
#include <e32cmn.h>
#include <xml/contenthandler.h>
#include <f32file.h>


/**
 * CIAUpdateXmlSubParser is the parent class 
 * for IA Update XML sub parsers and provides the basic functionality
 * that is common for all the IA Update XML sub parsers.
 * This class can not be instantiated itself. So, child classes
 * must be created to finalize the functionality.
 *
 * @since S60 v3.2
 */
class CIAUpdateXmlSubParser : public CBase,
                              public Xml::MContentHandler
    {

public:
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CIAUpdateXmlSubParser();


public: // Xml::MContentHandler

    /**
     * @see Xml::MContentHandler::OnStartDocumentL
     *
     * Initializes this sub parser and its sub parsers. 
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam, 
                                            TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnEndDocumentL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual void OnEndDocumentL( TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnStartElementL
     *
     * If necessary, this function can set the current sub parser for this 
     * sub parser by finding the correct subparser from the sub parser array.
     * ElementTagInfoEquals function is used for comparison. 
     * When the current sub parser is set, then element handling is
     * forwarded to the subparser until end tag of the element is reached.
     * The forwarding makes it possible to create a structural element
     * handling. If forwarding is not required, then child classes
     * can handle elements directly in their implementation of this function.
     * This function will set the AcceptData flag to a correct state.
     *
     * Child classes may provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */    
    IMPORT_C virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                           const Xml::RAttributeArray& aAttributes, 
                                           TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnEndElementL
     *
     * If current sub parser is set for this sub parser, hanling of the elements 
     * is forwarded to the sub parser. ElementTagInfoEquals function is used to 
     * check if the current subparser has finished its job. If sub parser is not
     * needed, then sub parser is set to NULL and the responsibility of 
     * handling elements is given back to this class object.
     * This function will set the AcceptData flag to a correct state.
     *
     * Child classes may provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */                                  
    IMPORT_C virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                         TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnContentL
     *
     * If current sub parser is set for this sub parser, hanling of the elements 
     * is forwarded to the sub parser.
     *
     * Child classes may provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */                                
    IMPORT_C virtual void OnContentL( const TDesC8& aBytes, 
                                      TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnStartPrefixMappingL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */                             
    IMPORT_C virtual void OnStartPrefixMappingL( const RString& aPrefix, 
                                                 const RString& aUri, 
                                                 TInt aErrorCode);
                                   
    /**
     * @see Xml::MContentHandler::OnEndPrefixMappingL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */                                        
    IMPORT_C virtual void OnEndPrefixMappingL( const RString& aPrefix, 
                                               TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnIgnorableWhiteSpaceL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */    
    IMPORT_C virtual void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, 
                                                  TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnSkippedEntityL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */    
    IMPORT_C virtual void OnSkippedEntityL( const RString& aName, 
                                            TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnProcessingInstructionL
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */    
    IMPORT_C virtual void OnProcessingInstructionL( const TDesC8& aTarget, 
                                                    const TDesC8& aData, 
                                                    TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::OnError
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual void OnError( TInt aErrorCode );
                                   
    /**
     * @see Xml::MContentHandler::GetExtendedInterface
     *
     * This contains only an empty implementation. Child classes may
     * provide their own implemenations if necessary.
     *
     * @since S60 v3.2
     */    
    IMPORT_C virtual TAny* GetExtendedInterface( const TInt32 aUid );


protected:
    
    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    IMPORT_C CIAUpdateXmlSubParser();
    
    /**
     * 2nd. phase constructor.
     *
     * @note This may be a good place for the child classes to implement
     * a functionality that adds its sub browsers to the sub browser array
     * that is used to delegate element handling to correct places.
     *
     * @param aElementLocalName This value is the local name of the
     * element that this parser is for. ElementTagInfoEquals function uses
     * this as a default to check if this parser is meant for the element
     * that the platform XML parser gives.
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual void ConstructL( const TDesC8& aElementLocalName );

    
    /**
     * @return const TDesC& Local name of this element.
     *
     * @since S60 v3.2
     */
    IMPORT_C const TDesC8& LocalName() const;


    /**
     * Gives the current sub parser of this sub parser if one has been set.
     * Sub parser is set when the element handling is forwarded to
     * a sub parser in OnStartElementL function. OnStartElementL function
     * of this class does it automatically. So, child classes can
     * use this functionality. NULL is returned if sub parser has
     * handled its job and OnEndElementL has been called for that
     * element.
     *
     * @return CIAUpdateXmlSubParser* Current sub parser or NULL if no
     * sub parser is in use. Ownerhsip is NOT transferred.
     *
     * @since S60 v3.2
     */
    IMPORT_C CIAUpdateXmlSubParser* CurrentSubParser() const;


    /**
     * This method can be used to get the array and
     * insert and remove sub parsers from it. 
     *
     * @return RPointerArray< CIAUpdateXmlSubParser >& Reference to the
     * sub parser array. 
     *
     * @since S60 v3.2
     */
    IMPORT_C RPointerArray< CIAUpdateXmlSubParser >& SubParsers();
    
    
    /**
     * Compares the tag information of the given object to the tag info 
     * of the main element of this parser.
     *
     * @note This default implementation compares only element local names.
     * The local name for this element is given in the ConstructL of 
     * this class. Child classes may implement their own functionality here.
     *
     * @note This value is used by OnStartElementL and OnEndElementL functions
     * when current subparser is checked for the given element.
     *
     * @param aElement Information about the element that has been gotten
     * from the XML parser.
     * @return TBool ETrue if tag infos match. 
     * EFalse if tag infos do not match. 
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool ElementTagInfoEquals( const Xml::RTagInfo& aElement ) const;


    /**
     * This flag is used to check if this sub parser should handle the given element or content.
     * 
     * @return TBool If sub parser has forwarded the task to another sub parser 
     * or if the given element does not match the element of this sub parser, 
     * then this flag is EFalse. If this class object should handle the element and it
     * matches the element of this sub parser, then this flag is ETrue.
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool AcceptData() const;    
    

    /**
     * This flag is used to check if this sub parser has handled its on starting element.
     * This is needed because sub element may have a same name with this element. So,
     * just name comparing is not enough.
     *
     * @return TBool ETrue if this sub parser has already handled its start element.
     * EFalse if this sub parser has not already handled its start element.
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool IsElementStarted() const;


    /**
     * This flag is used to check if this sub parser has handled its on end element.
     * This is needed because sub element may have a same name with this element. So,
     * just name comparing is not enough.
     *
     * @return TBool ETrue if this sub parser has already handled its end element.
     * EFalse if this sub parser has not already handled its end element.
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool IsElementEnded() const;


    /**
     * Informs how many unknown elements are in line with this parser.
     *
     * @return TInt Number of unknown elements that this parser have to handle
     * before new possible known elements can be handled.
     *
     * @since S60 v3.2
     */
    IMPORT_C TInt UnknownElementCounter() const;


private:
    
    // Prevent these if not implemented
    CIAUpdateXmlSubParser( const CIAUpdateXmlSubParser& aObject );
    CIAUpdateXmlSubParser& operator =( const CIAUpdateXmlSubParser& aObject );


private: // data

    // Local name of the element of this sub parser.
    HBufC8* iElementLocalName;

    // This array will contain other instances of the parsers that this
    // parser uses as subparsers. This way element hierarchy can be browsed
    // through.
    RPointerArray< CIAUpdateXmlSubParser > iSubParsers;    
    
    // iCurrentSubParser points to the subparser that this class object
    // forwards element handling to.
    // Notice, that the iSubParsers array has the ownership of the
    // subparsers. So, do not delete this object separately.
    // This is NULL if there is no current subparser in use.
    CIAUpdateXmlSubParser* iCurrentSubParser;
    
    // This flag is used to check if this sub parser should handle the given element or content.
    // If sub parser has given the task to another sub parser or if the given element does not
    // match the element of this sub parser, then this flag is EFalse.
    TBool iAcceptData;

    // This flag is used to check if this sub parser has handled its on starting element.
    // This is needed because sub element may have a same name with this element. So,
    // just name comparing is not enough.
    TBool iIsElementStarted;

    // This flag is used to check if this sub parser has handled its on ending element.
    // This is needed because sub element may have a same name with this element. So,
    // just name comparing is not enough.
    TBool iIsElementEnded;
    
    // Number of unknown elements that this parser have to handle
    // before new possible known elements can be handled.
    TInt iUnknownElementCounter;
    };

#endif // IA_UPDATE_XML_SUB_PARSER_H

