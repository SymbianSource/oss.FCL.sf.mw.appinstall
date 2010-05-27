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



#include "iaupdateenginexmlparser.h"
#include "iaupdateenginexmlsubparser.h"
#include "iaupdateengineconfigdata.h"
#include "iaupdateengineconfigconsts.h"


CIAUpdateEngineXmlParser* CIAUpdateEngineXmlParser::NewL()
    {
    CIAUpdateEngineXmlParser* self =
        CIAUpdateEngineXmlParser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateEngineXmlParser* CIAUpdateEngineXmlParser::NewLC()
    {
    // Create config data that will contain all the parsed data.
    CIAUpdateEngineConfigData* configData = 
        CIAUpdateEngineConfigData::NewLC();
    
    // Create sub parser that will handle the given elements.
    CIAUpdateEngineXmlSubParser* subParser = 
        CIAUpdateEngineXmlSubParser::NewLC( *configData );

    // Create the actual XML parser that uses configData and subParser.
    // Notice, that the ownership of the subParser and configData are
    // transferred here to the parser.
    CIAUpdateEngineXmlParser* self =
        new( ELeave ) CIAUpdateEngineXmlParser( subParser, configData );

    CleanupStack::Pop( subParser );
    CleanupStack::Pop( configData );    

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;    
    }


CIAUpdateEngineXmlParser::CIAUpdateEngineXmlParser( CIAUpdateEngineXmlSubParser* aSubParser,
                                                    CIAUpdateEngineConfigData* aConfigData )
: CIAUpdateXmlParser( aSubParser ),
  iConfigData( aConfigData )
    {
    
    }
    
    
void CIAUpdateEngineXmlParser::ConstructL()
    {
    CIAUpdateXmlParser::ConstructL();
    }


CIAUpdateEngineXmlParser::~CIAUpdateEngineXmlParser()
    {
    delete iConfigData;
    }


void CIAUpdateEngineXmlParser::ParseL()
    {
    // Reset the config data pefore starting to set new values there.
    // For example if the parsing has been done before and the config file 
    // content has changed, then old values that are not parsed now, will not
    // be left hanging.
    iConfigData->ResetL();
    ParsePrivateFileL( IAUpdateEngineConfigConsts::KConfigFile );        
    }


const CIAUpdateEngineConfigData& CIAUpdateEngineXmlParser::ConfigData() const
    {
    return *iConfigData;
    }
