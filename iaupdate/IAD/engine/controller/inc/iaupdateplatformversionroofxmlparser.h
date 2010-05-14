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



#ifndef IA_UPDATE_PLATFORM_VERSION_ROOF_XML_PARSER_H
#define IA_UPDATE_PLATFORM_VERSION_ROOF_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdateplatformversionxmlparser.h"

/**
 * CIAUpdatePlatformVersionRoofXmlParser handles the dependency XML elements for
 * platform roof version.
 *
 * @since S60 v3.2
 */
class CIAUpdatePlatformVersionRoofXmlParser : public CIAUpdatePlatformVersionXmlParser
    {

public:
    
    /**
     * @param aDependency The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdatePlatformVersionRoofXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformVersionRoofXmlParser* NewL( 
                                           CIAUpdatePlatformDependency& aDependency );

    /**
     * @see CIAUpdatePlatformVersionRoofXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformVersionRoofXmlParser* NewLC( 
                                          CIAUpdatePlatformDependency& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdatePlatformVersionRoofXmlParser();


public: // CIAUpdatePlatformVersionXmlParser

    /**
     * @see CIAUpdatePlatformVersionXmlParser::OnEndElementL
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
     * @see CIAUpdatePlatformVersionXmlParser::OnEndElementL
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
     * @see CIAUpdatePlatformVersionRoofXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformVersionRoofXmlParser( CIAUpdatePlatformDependency& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();
    

private:
    
    // Prevent these if not implemented
    CIAUpdatePlatformVersionRoofXmlParser( 
                      const CIAUpdatePlatformVersionRoofXmlParser& aObject );
    CIAUpdatePlatformVersionRoofXmlParser& operator =( 
                      const CIAUpdatePlatformVersionRoofXmlParser& aObject );


private: // data

    };

#endif // IA_UPDATE_PLATFORM_VERSION_ROOF_XML_PARSER_H
