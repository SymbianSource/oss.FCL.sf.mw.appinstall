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



#ifndef IA_UPDATE_DEPENDENCY_XML_SUB_PARSER_H
#define IA_UPDATE_DEPENDENCY_XML_SUB_PARSER_H


#include "iaupdatexmlsubparser.h"

class CIAUpdateNodeDependency;
class CIAUpdatePlatformDependency;


/**
 * CIAUpdateDependencyXmlSubParser handles the dependency XML elements for both
 * platform dependency and software dependency. All the work is mainly delegated
 * to the corresponding sub parsers that are created in this class object.
 *
 * @since S60 v3.2
 */
class CIAUpdateDependencyXmlSubParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aNodeDependencies Node dependency array that will contain nodes that are
     * initialized with the parsed dependency data.
     * @param aPlatformDependency Platform dependency object that will be initialized
     * with the parsed dependency data.
     * @return CIAUpdateDependencyXmlSubParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateDependencyXmlSubParser* NewL( 
                              RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                              CIAUpdatePlatformDependency& aPlatformDependency );

    /**
     * @see CIAUpdateDependencyXmlSubParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateDependencyXmlSubParser* NewLC( 
                              RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                              CIAUpdatePlatformDependency& aPlatformDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateDependencyXmlSubParser();
    

protected:
    
    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    CIAUpdateDependencyXmlSubParser();
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @see CIAUpdateDependencyXmlSubParser::NewL
     *
     * @since S60 v3.2
     */
    virtual void ConstructL( RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                             CIAUpdatePlatformDependency& aPlatformDependency );


private:
    
    // Prevent these if not implemented
    CIAUpdateDependencyXmlSubParser( const CIAUpdateDependencyXmlSubParser& aObject );
    CIAUpdateDependencyXmlSubParser& operator =( const CIAUpdateDependencyXmlSubParser& aObject );

private: // data

    };

#endif // IA_UPDATE_DEPENDENCY_XML_SUB_PARSER_H
