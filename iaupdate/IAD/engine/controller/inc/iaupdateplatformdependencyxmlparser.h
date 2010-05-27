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



#ifndef IA_UPDATE_PLATFORM_DEPENDENCY_XML_PARSER_H
#define IA_UPDATE_PLATFORM_DEPENDENCY_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"

class CIAUpdatePlatformDependency;


/**
 * CIAUpdatePlatformDependencyXmlParser handles the dependency XML elements for
 * platform dependency.
 *
 * @since S60 v3.2
 */
class CIAUpdatePlatformDependencyXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aDependency The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdatePlatformDependencyXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDependencyXmlParser* NewL( CIAUpdatePlatformDependency& aDependency );

    /**
     * @see CIAUpdatePlatformDependencyXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDependencyXmlParser* NewLC( CIAUpdatePlatformDependency& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdatePlatformDependencyXmlParser();
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdatePlatformDependencyXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformDependencyXmlParser( CIAUpdatePlatformDependency& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * @return CIAUpdatePlatformDependency& The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformDependency& Dependency();
    

private:
    
    // Prevent these if not implemented
    CIAUpdatePlatformDependencyXmlParser( 
                           const CIAUpdatePlatformDependencyXmlParser& aObject );
    CIAUpdatePlatformDependencyXmlParser& operator =( 
                           const CIAUpdatePlatformDependencyXmlParser& aObject );


private: // data

    CIAUpdatePlatformDependency& iDependency;

    };

#endif // IA_UPDATE_PLATFORM_DEPENDENCY_XML_PARSER_H
