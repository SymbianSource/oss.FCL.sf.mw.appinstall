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



#ifndef IA_UPDATE_NODE_VERSION_XML_PARSER_H
#define IA_UPDATE_NODE_VERSION_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"
#include "iaupdateversion.h"

class CIAUpdateNodeDependency;


/**
 * CIAUpdateNodeVersionXmlParser handles the dependency XML elements for
 * node version.
 *
 * @since S60 v3.2
 */
class CIAUpdateNodeVersionXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateNodeVersionXmlParser();
    

protected:
    
    /**
     * Constructor
     *
     * Because there may be multiple dependencies, the dependency parser that uses
     * this version parser may change the dependency. So, that is why reference to
     * the pointer is used. This way this parser always uses the correct dependency.
     *
     * @param aDependency Reference to the dependency pointer that points to the
     * current dependency.
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeVersionXmlParser( CIAUpdateNodeDependency*& aDependency );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL( const TDesC8& aElementLocalName );


    /**
     * @return CIAUpdateNodeDependency& The node dependency information that is
     * parsed from the XML data is inserted into this object.
     * @exception Leaves with KErrNotFound if dependency is not found. Else system
     * wide error code.
     *
     * @since S60 v3.2
     */
    CIAUpdateNodeDependency& DependencyL();


    /**
     * @return TIAUpdateVersion& Reference to the version variable.
     *
     * @since S60 v3.2
     */    
    TIAUpdateVersion& Version();
    
    
private:
    
    // Prevent these if not implemented
    CIAUpdateNodeVersionXmlParser( const CIAUpdateNodeVersionXmlParser& aObject );
    CIAUpdateNodeVersionXmlParser& operator =( const CIAUpdateNodeVersionXmlParser& aObject );


private: // data

    CIAUpdateNodeDependency*& iDependency;

    // This version will contain the parsed version information after version element
    // is completed.
    TIAUpdateVersion iVersion;

    };

#endif // IA_UPDATE_NODE_VERSION_XML_PARSER_H
