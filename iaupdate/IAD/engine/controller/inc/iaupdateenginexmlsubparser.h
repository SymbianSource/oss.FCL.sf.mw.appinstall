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



#ifndef IA_UPDATE_ENGINE_XML_SUB_PARSER_H
#define IA_UPDATE_ENGINE_XML_SUB_PARSER_H


#include "iaupdatexmlsubparser.h"

class CIAUpdateEngineConfigData;


/**
 * CIAUpdateEngineXmlSubParser parses the engine configuration data.
 *
 * @since S60 v3.2
 */
class CIAUpdateEngineXmlSubParser : public CIAUpdateXmlSubParser
    {

public:
    
    /**
     * @param aConfigData This config data will contain engine information that is
     * parsed from the elements that are handled by this sub parser.
     * @return CIAUpdateEngineXmlSubParser* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineXmlSubParser* NewL( CIAUpdateEngineConfigData& aConfigData );

    /**
     * @see CIAUpdateEngineXmlSubParser::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateEngineXmlSubParser* NewLC( CIAUpdateEngineConfigData& aConfigData );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateEngineXmlSubParser();


    /**
     * @note If parsing has not been done before calling this function,
     * the returned object contains only default values.
     *
     * @return const CIAUpdateEngineConfigData& Config data.
     *
     * @since S60 v3.2
     */
    const CIAUpdateEngineConfigData& ConfigData() const;
    

public: // CIAUpdateXmlSubParser 

    /**
     * @see CIAUpdateXmlSubParser::OnStartElementL
     *
     * @since S60 v3.2
     */    
    virtual void OnStartElementL( const Xml::RTagInfo& aElement, 
                                  const Xml::RAttributeArray& aAttributes, 
                                  TInt aErrorCode );


protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateEngineXmlSubParser::NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateEngineXmlSubParser( CIAUpdateEngineConfigData& aConfigData );
    
    /**
     * 2nd. phase constructor.
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


private:
    
    // Prevent these if not implemented
    CIAUpdateEngineXmlSubParser( const CIAUpdateEngineXmlSubParser& aObject );
    CIAUpdateEngineXmlSubParser& operator =( const CIAUpdateEngineXmlSubParser& aObject );

    // Funtions that hande the attributes of different elements and insert the
    // values into the configuration data object.

    void HandleStorageAttributesL( const Xml::RAttributeArray& aAttributes );
    void HandleMasterServerAttributesL( const Xml::RAttributeArray& aAttributes );
    void HandleProvisioningAttributesL( const Xml::RAttributeArray& aAttributes );
    void HandleClientRoleAttributesL( const Xml::RAttributeArray& aAttributes );


private: // data

    CIAUpdateEngineConfigData& iConfigData;
    
    };

#endif // IA_UPDATE_ENGINE_XML_SUB_PARSER_H
