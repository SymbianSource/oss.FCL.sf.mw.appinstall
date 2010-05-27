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



#ifndef IA_UPDATE_NODE_DEPENDENCY_XML_PARSER_H
#define IA_UPDATE_NODE_DEPENDENCY_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"

class CIAUpdateNodeDependency;


/**
 * CIAUpdateNodeDependencyXmlParser handles the dependency XML elements for
 * node dependencies.
 *
 * @since S60 v3.2
 */
class CIAUpdateNodeDependencyXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aDependencies Array where node dependencies that are
     * created according to the XML data are appended.
     * @return CIAUpdateNodeDependencyXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeDependencyXmlParser* NewL( 
                    RPointerArray< CIAUpdateNodeDependency >& aDependencies );

    /**
     * @see CIAUpdateNodeDependencyXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateNodeDependencyXmlParser* NewLC( 
                    RPointerArray< CIAUpdateNodeDependency >& aDependencies );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateNodeDependencyXmlParser();
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateNodeDependencyXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeDependencyXmlParser( 
                   RPointerArray< CIAUpdateNodeDependency >& aDependencies );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * @return RPointerArray< CIAUpdateNodeDependency >& Node dependency array where
     * parser will add all node dependencies that are initialized with values that are
     * parsed from the XML.
     */
    RPointerArray< CIAUpdateNodeDependency >& Dependencies();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateNodeDependencyXmlParser( const CIAUpdateNodeDependencyXmlParser& aObject );
    CIAUpdateNodeDependencyXmlParser& operator =( const CIAUpdateNodeDependencyXmlParser& aObject );


private: // data

    RPointerArray< CIAUpdateNodeDependency >& iDependencies;

    };

#endif // IA_UPDATE_NODE_DEPENDENCY_XML_PARSER_H
