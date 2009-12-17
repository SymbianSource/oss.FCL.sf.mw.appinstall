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



#ifndef IA_UPDATE_NODE_VERSION_ROOF_XML_PARSER_H
#define IA_UPDATE_NODE_VERSION_ROOF_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatenodeversionxmlparser.h"

/**
 * CIAUpdateNodeVersionRoofXmlParser handles the dependency XML data for
 * node roof version.
 *
 * @since S60 v3.2
 */
class CIAUpdateNodeVersionRoofXmlParser : public CIAUpdateNodeVersionXmlParser
    {

public:
    
    /**
     * @param aDependency The node dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdateNodeVersionRoofXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeVersionRoofXmlParser* NewL( CIAUpdateNodeDependency*& aDependency );

    /**
     * @see CIAUpdateNodeVersionRoofXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeVersionRoofXmlParser* NewLC( CIAUpdateNodeDependency*& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateNodeVersionRoofXmlParser();


public: // CIAUpdateNodeVersionRoofXmlParser

    /**
     * @see CIAUpdateNodeVersionXmlParser::OnEndElementL
     *
     * Sets the version information to its roof value and
     * delegates the job to the parent class after that.
     *
     * @since S60 v3.2
     */    
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );

    /**
     * @see CIAUpdateNodeVersionRoofXmlParser::OnEndElementL
     *
     * After this element has been parsed, it sets the roof version information
     * to the dependency object.
     *
     * @since S60 v3.2
     */                                  
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, 
                                TInt aErrorCode );    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateNodeVersionRoofXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeVersionRoofXmlParser( CIAUpdateNodeDependency*& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateNodeVersionRoofXmlParser( 
                              const CIAUpdateNodeVersionRoofXmlParser& aObject );
    CIAUpdateNodeVersionRoofXmlParser& operator =( 
                              const CIAUpdateNodeVersionRoofXmlParser& aObject );


private: // data

    };

#endif // IA_UPDATE_NODE_VERSION_ROOF_XML_PARSER_H
