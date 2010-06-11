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



#ifndef IA_UPDATE_VERSION_BUILD_XML_PARSER_H
#define IA_UPDATE_VERSION_BUILD_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"
#include "iaupdateversion.h"


/**
 * CIAUpdateVersionBuildXmlParser handles the dependency XML elements for
 * version build information.
 *
 * @since S60 v3.2
 */
class CIAUpdateVersionBuildXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aVersion The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdateVersionBuildXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionBuildXmlParser* NewL( TIAUpdateVersion& aVersion );

    /**
     * @see CIAUpdateVersionBuildXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionBuildXmlParser* NewLC( TIAUpdateVersion& aVersion );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateVersionBuildXmlParser();


public: // CIAUpdateXmlSubParser

    /**
     * @see CIAUpdateXmlSubParser::OnContentL
     * 
     * Sets the version build information.
     *
     * @since S60 v3.2
     */                                
    virtual void OnContentL( const TDesC8& aBytes, 
                             TInt aErrorCode );    


protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateVersionBuildXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateVersionBuildXmlParser( TIAUpdateVersion& aVersion );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * @return TIAUpdateVersion& The version build information that is
     * parsed from the XML data is inserted into this referenced object.
     *
     * @since S60 v3.2
     */
    TIAUpdateVersion& Version();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateVersionBuildXmlParser( const CIAUpdateVersionBuildXmlParser& aObject );
    CIAUpdateVersionBuildXmlParser& operator =( const CIAUpdateVersionBuildXmlParser& aObject );


private: // data

     TIAUpdateVersion& iVersion; 

    };

#endif // IA_UPDATE_VERSION_BUILD_XML_PARSER_H
