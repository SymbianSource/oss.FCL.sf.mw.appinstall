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



#ifndef IA_UPDATE_ENGINE_XML_PARSER_H
#define IA_UPDATE_ENGINE_XML_PARSER_H


#include "iaupdatexmlparser.h"

class CIAUpdateEngineXmlSubParser;
class CIAUpdateEngineConfigData;


/**
 * CIAUpdateEngineXmlParser parses the configuration data from
 * engine config file.
 *
 * @since S60 v3.2
 */
class CIAUpdateEngineXmlParser : public CIAUpdateXmlParser
    {

public:
    
    /**
     * @return CIAUpdateEngineXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineXmlParser* NewL();

    /**
     * @see CIAUpdateEngineXmlParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineXmlParser* NewLC();


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateEngineXmlParser();


    /**
     * Parses the data from the configuration file and inserts the values
     * into the configuration data object. This done synchronously.
     *
     * @since S60 v3.2
     */
    virtual void ParseL();


    /**
     * @note If parsing has not been done before calling this function,
     * the returned object contains only default values.
     *
     * @return const CIAUpdateEngineConfigData& Config data.
     *
     * @since S60 v3.2
     */
    const CIAUpdateEngineConfigData& ConfigData() const;
    

protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateEngineXmlParser::NewL
     *
     * @param aSubParser Sub parser that will handle the XML elements.
     * NULL value is NOT accepted here. Ownership is transferred.
     * @param aConfigData Config data that will contain all the parsed data.
     * NULL value is NOT accepted here. Ownership is transferred.
     *
     * @since S60 v3.2
     */
    CIAUpdateEngineXmlParser( CIAUpdateEngineXmlSubParser* aSubParser,
                              CIAUpdateEngineConfigData* aConfigData );
    
    /**
     * 2nd. phase constructor.
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


private:
    
    // Prevent these if not implemented
    CIAUpdateEngineXmlParser( const CIAUpdateEngineXmlParser& aObject );
    CIAUpdateEngineXmlParser& operator =( const CIAUpdateEngineXmlParser& aObject );


private: // data

    CIAUpdateEngineConfigData* iConfigData;
    
    };

#endif // IA_UPDATE_ENGINE_XML_SUB_PARSER_H
