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



#ifndef IA_UPDATE_PLATFORM_DESCRIPTION_XML_PARSER_H
#define IA_UPDATE_PLATFORM_DESCRIPTION_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"

class CIAUpdatePlatformDependency;


/**
 * CIAUpdatePlatformDescriptionXmlParser handles the dependency XML elements for
 * platform description.
 *
 * @since S60 v3.2
 */
class CIAUpdatePlatformDescriptionXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aDependency The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdatePlatformDescriptionXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDescriptionXmlParser* NewL( 
                                     CIAUpdatePlatformDependency& aDependency );

    /**
     * @see CIAUpdatePlatformDescriptionXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDescriptionXmlParser* NewLC( 
                                     CIAUpdatePlatformDependency& aDependency );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdatePlatformDescriptionXmlParser();


public: // CIAUpdateXmlSubParser

    /**
     * @see CIAUpdateXmlSubParser::OnContentL
     * 
     * Sets the platform description information for the dependency.
     *
     * @since S60 v3.2
     */                                
    virtual void OnContentL( const TDesC8& aBytes, 
                             TInt aErrorCode );    
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdatePlatformDescriptionXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformDescriptionXmlParser( CIAUpdatePlatformDependency& aDependency );
    
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
    CIAUpdatePlatformDescriptionXmlParser( 
                         const CIAUpdatePlatformDescriptionXmlParser& aObject );
    CIAUpdatePlatformDescriptionXmlParser& operator =( 
                         const CIAUpdatePlatformDescriptionXmlParser& aObject );


private: // data

    CIAUpdatePlatformDependency& iDependency;

    };

#endif // IA_UPDATE_PLATFORM_DESCRIPTION_XML_PARSER_H
