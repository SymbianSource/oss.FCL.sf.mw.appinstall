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



#ifndef IA_UPDATE_INTER_DEP_XML_PARSER_H
#define IA_UPDATE_INTER_DEP_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"

class CIAUpdateNodeDependency;


/**
 * CIAUpdateInterDepXmlParser handles the node dependency elements and
 * inserts information into the node dependency object. Also, inserts the dependency
 * object to the given node dependency array.
 *
 * @since S60 v3.2
 */
class CIAUpdateInterDepXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aDependencies Array where node dependencies that are
     * created according to the XML data are appended.
     * @return CIAUpdateInterDepXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateInterDepXmlParser* NewL( 
                      RPointerArray< CIAUpdateNodeDependency >& aDependencies );

    /**
     * @see CIAUpdateInterDepXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateInterDepXmlParser* NewLC( 
                      RPointerArray< CIAUpdateNodeDependency >& aDependencies );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateInterDepXmlParser();


public: // CIAUpdateXmlSubParser 

    /**
     * @see CIAUpdateXmlSubParser::OnStartElementL
     *
     * @since S60 v3.2
     */    
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );

    /**
     * @see CIAUpdateXmlSubParser::OnEndElementL
     *
     * @since S60 v3.2
     */    
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );
                                  

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateInterDepXmlParser::NewL
     *
     *
     * @since S60 v3.2
     */
    CIAUpdateInterDepXmlParser( RPointerArray< CIAUpdateNodeDependency >& aDependencies );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


private:
    
    // Prevent these if not implemented
    CIAUpdateInterDepXmlParser( const CIAUpdateInterDepXmlParser& aObject );
    CIAUpdateInterDepXmlParser& operator =( const CIAUpdateInterDepXmlParser& aObject );


private: // data

    // Current dependency object. That will be inserted into the dependencies array
    // when parsing is done for the object.
    CIAUpdateNodeDependency* iDependency; 

    // Reference to the dependency array where parsed dependencies will be inserted.
    RPointerArray< CIAUpdateNodeDependency >& iDependencies;

    };

#endif // IA_UPDATE_INTER_DEP_XML_PARSER_H
