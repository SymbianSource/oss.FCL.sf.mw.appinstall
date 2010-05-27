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



#ifndef IA_UPDATE_VERSION_MINOR_XML_PARSER_H
#define IA_UPDATE_VERSION_MINOR_XML_PARSER_H


#include <e32cmn.h>

#include "iaupdatexmlsubparser.h"
#include "iaupdateversion.h"


/**
 * CIAUpdateVersionMinorXmlParser handles the dependency XML elements for
 * version minor information.
 *
 * @since S60 v3.2
 */
class CIAUpdateVersionMinorXmlParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aVersion The platform dependency information that is
     * parsed from the XML data is inserted into this object.
     * @return CIAUpdateVersionMinorXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionMinorXmlParser* NewL( TIAUpdateVersion& aVersion );

    /**
     * @see CIAUpdateVersionMinorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateVersionMinorXmlParser* NewLC( TIAUpdateVersion& aVersion );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateVersionMinorXmlParser();
    
    
public: // CIAUpdateXmlSubParser

    /**
     * @see CIAUpdateXmlSubParser::OnContentL
     * 
     * Sets the version minor information.
     *
     * @since S60 v3.2
     */                                
    virtual void OnContentL( const TDesC8& aBytes, 
                             TInt aErrorCode );    


protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateVersionMinorXmlParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateVersionMinorXmlParser( TIAUpdateVersion& aVersion );
    
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
    CIAUpdateVersionMinorXmlParser( const CIAUpdateVersionMinorXmlParser& aObject );
    CIAUpdateVersionMinorXmlParser& operator =( const CIAUpdateVersionMinorXmlParser& aObject );


private: // data

     TIAUpdateVersion& iVersion; 

    };

#endif // IA_UPDATE_VERSION_MINOR_XML_PARSER_H
