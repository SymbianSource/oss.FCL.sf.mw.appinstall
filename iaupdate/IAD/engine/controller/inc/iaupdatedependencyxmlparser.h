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



#ifndef IA_UPDATE_DEPENDENCY_XML_PARSER_H
#define IA_UPDATE_DEPENDENCY_XML_PARSER_H


#include "iaupdatexmlparser.h"

class CIAUpdateNodeDependency;
class CIAUpdatePlatformDependency;
class CIAUpdateDependencyXmlSubParser;


/**
 * CIAUpdateDependencyXmlParser handles the dependency XML data for both
 * platform dependency and software dependency. All the work is mainly delegated
 * to the corresponding sub parsers that are created in this class object.
 *
 * @since S60 v3.2
 */
class CIAUpdateDependencyXmlParser : public CIAUpdateXmlParser
    {

public:
    
    /**
     * @param aNodeDependencies Node dependency array that will contain nodes that are
     * initialized with the parsed dependency data.
     * @param aPlatformDependency Platform dependency object that will be initialized
     * with the parsed dependency data.
     * @return CIAUpdateDependencyXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateDependencyXmlParser* NewL( 
                              RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                              CIAUpdatePlatformDependency& aPlatformDependency );

    /**
     * @see CIAUpdateDependencyXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateDependencyXmlParser* NewLC( 
                              RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                              CIAUpdatePlatformDependency& aPlatformDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateDependencyXmlParser();
    

protected:
    
    /**
     * Constructor
     *
     * @param aSubParser Sub parser that will handle the XML elements.
     * NULL value is NOT accepted here. Ownership is transferred.
     *
     * @since S60 v3.2
     */
    CIAUpdateDependencyXmlParser( CIAUpdateDependencyXmlSubParser* aSubParser );
    
    /**
     * 2nd. phase constructor.
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


private:
    
    // Prevent these if not implemented
    CIAUpdateDependencyXmlParser( const CIAUpdateDependencyXmlParser& aObject );
    CIAUpdateDependencyXmlParser& operator =( const CIAUpdateDependencyXmlParser& aObject );

private: // data

    };

#endif // IA_UPDATE_DEPENDENCY_XML_PARSER_H
