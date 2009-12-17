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



#ifndef IA_UPDATE_NODE_EMBEDDED_XML_PARSER_H
#define IA_UPDATE_NODE_EMBEDDED_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"

class CIAUpdateNodeDependency;


/**
 * CIAUpdateNodeEmbeddedXmlParser handles the dependency XML elements for
 * node.
 *
 * @since S60 v3.2
 */
class CIAUpdateNodeEmbeddedXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * Because there may be multiple dependencies, the dependency sub parser that uses
     * this sub parser may change the dependency. So, that is why reference to
     * the pointer is used. This way this parser always uses the correct dependency.
     *
     * @param aDependency Reference to the dependency pointer that points to the
     * current dependency. The embedded information that is parsed from the XML 
     * data is inserted into this object.
     * @return CIAUpdateNodeEmbeddedXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeEmbeddedXmlParser* NewL( CIAUpdateNodeDependency*& aDependency );

    /**
     * @see CIAUpdateNodeEmbeddedXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeEmbeddedXmlParser* NewLC( CIAUpdateNodeDependency*& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateNodeEmbeddedXmlParser();


public: // CIAUpdateXmlSubParser

    /**
     * @see CIAUpdateXmlSubParser::OnContentL
     * 
     * Sets the UID information for the dependency.
     *
     * @since S60 v3.2
     */                                
    virtual void OnContentL( const TDesC8& aBytes, 
                             TInt aErrorCode );    
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateNodeEmbeddedXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeEmbeddedXmlParser( CIAUpdateNodeDependency*& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * @return CIAUpdateNodeDependency& The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @exception Leaves with KErrNotFound if dependency is not found. Else system
     * wide error code.
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeDependency& DependencyL();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateNodeEmbeddedXmlParser( const CIAUpdateNodeEmbeddedXmlParser& aObject );
    CIAUpdateNodeEmbeddedXmlParser& operator =( const CIAUpdateNodeEmbeddedXmlParser& aObject );


private: // data

    CIAUpdateNodeDependency*& iDependency;

    };

#endif // IA_UPDATE_NODE_EMBEDDED_XML_PARSER_H
