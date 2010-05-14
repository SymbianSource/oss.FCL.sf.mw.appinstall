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



#ifndef IA_UPDATE_PLATFORM_VERSION_XML_PARSER_H
#define IA_UPDATE_PLATFORM_VERSION_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"
#include "iaupdateversion.h"

class CIAUpdatePlatformDependency;


/**
 * CIAUpdatePlatformVersionXmlParser handles the dependency XML elements for
 * platform version.
 *
 * @since S60 v3.2
 */
class CIAUpdatePlatformVersionXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdatePlatformVersionXmlParser();
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdatePlatformVersionXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformVersionXmlParser( CIAUpdatePlatformDependency& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL( const TDesC8& aElementLocalName );


    /**
     * @return CIAUpdatePlatformDependency& The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformDependency& Dependency();


    /**
     * @return TIAUpdateVersion& Reference to the version variable.
     *
     * @since S60 v3.2
     */    
    TIAUpdateVersion& Version();
    
    
private:
    
    // Prevent these if not implemented
    CIAUpdatePlatformVersionXmlParser( 
                    const CIAUpdatePlatformVersionXmlParser& aObject );
    CIAUpdatePlatformVersionXmlParser& operator =( 
                    const CIAUpdatePlatformVersionXmlParser& aObject );


private: // data

    CIAUpdatePlatformDependency& iDependency;

    // This version will contain the parsed version information after version element
    // is completed.
    TIAUpdateVersion iVersion;

    };

#endif // IA_UPDATE_PLATFORM_VERSION_XML_PARSER_H
