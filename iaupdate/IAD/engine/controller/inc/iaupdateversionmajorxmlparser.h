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



#ifndef IA_UPDATE_VERSION_MAJOR_XML_PARSER_H
#define IA_UPDATE_VERSION_MAJOR_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"
#include "iaupdateversion.h"


/**
 * CIAUpdateVersionMajorXmlParser handles the dependency XML elements for
 * version major information.
 *
 * @since S60 v3.2
 */
class CIAUpdateVersionMajorXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aVersion The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdateVersionMajorXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionMajorXmlParser* NewL( TIAUpdateVersion& aVersion );

    /**
     * @see CIAUpdateVersionMajorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionMajorXmlParser* NewLC( TIAUpdateVersion& aVersion );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateVersionMajorXmlParser();


public: // CIAUpdateXmlSubParser

    /**
     * @see CIAUpdateXmlSubParser::OnContentL
     * 
     * Sets the version major information.
     *
     * @since S60 v3.2
     */                                
    virtual void OnContentL( const TDesC8& aBytes, 
                             TInt aErrorCode );    
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateVersionMajorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateVersionMajorXmlParser( TIAUpdateVersion& aVersion );
    
    /**
     * 2nd. phase constructor.
     * Adds the node dependency sub parsers to the parent class sub parser list. 
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * @return TIAUpdateVersion& The version minor information that is
     * parsed from the XML data is inserted into this referenced object.
     *
     * @since S60 v3.2
     */
    TIAUpdateVersion& Version();
    

private:
    
    // Prevent these if not implemented
    CIAUpdateVersionMajorXmlParser( const CIAUpdateVersionMajorXmlParser& aObject );
    CIAUpdateVersionMajorXmlParser& operator =( const CIAUpdateVersionMajorXmlParser& aObject );


private: // data

     TIAUpdateVersion& iVersion; 

    };

#endif // IA_UPDATE_VERSION_MAJOR_XML_PARSER_H
