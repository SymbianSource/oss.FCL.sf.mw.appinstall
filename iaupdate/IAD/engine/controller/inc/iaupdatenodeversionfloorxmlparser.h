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



#ifndef IA_UPDATE_NODE_VERSION_FLOOR_XML_PARSER_H
#define IA_UPDATE_NODE_VERSION_FLOOR_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatenodeversionxmlparser.h"

/**
 * CIAUpdateNodeVersionFloorXmlParser handles the dependency XML data for
 * node floor version.
 *
 * @since S60 v3.2
 */
class CIAUpdateNodeVersionFloorXmlParser : public CIAUpdateNodeVersionXmlParser
    {

public:
    
    /**
     * @param aDependency The node dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdateNodeVersionFloorXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeVersionFloorXmlParser* NewL( 
                                        CIAUpdateNodeDependency*& aDependency );

    /**
     * @see CIAUpdateNodeVersionFloorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeVersionFloorXmlParser* NewLC( 
                                       CIAUpdateNodeDependency*& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateNodeVersionFloorXmlParser();
    

public: // CIAUpdateNodeVersionXmlParser

    /**
     * @see CIAUpdateNodeVersionXmlParser::OnEndElementL
     *
     * Sets the version information to its floor value and
     * delegates the job to the parent class after that.
     *
     * @since S60 v3.2
     */    
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );

    /**
     * @see CIAUpdateNodeVersionXmlParser::OnEndElementL
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
     * @see CIAUpdateNodeVersionFloorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeVersionFloorXmlParser( CIAUpdateNodeDependency*& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateNodeVersionFloorXmlParser( 
                              const CIAUpdateNodeVersionFloorXmlParser& aObject );
    CIAUpdateNodeVersionFloorXmlParser& operator =( 
                              const CIAUpdateNodeVersionFloorXmlParser& aObject );


private: // data

    };

#endif // IA_UPDATE_NODE_VERSION_FLOOR_XML_PARSER_H
