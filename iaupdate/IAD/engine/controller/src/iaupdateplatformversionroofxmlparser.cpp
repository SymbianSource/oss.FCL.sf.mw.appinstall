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



#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

#include "iaupdateplatformversionroofxmlparser.h"
#include "iaupdateplatformdependency.h"
#include "iaupdateprotocolconsts.h"


CIAUpdatePlatformVersionRoofXmlParser* CIAUpdatePlatformVersionRoofXmlParser::NewL( 
                                              CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformVersionRoofXmlParser* self =
        CIAUpdatePlatformVersionRoofXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdatePlatformVersionRoofXmlParser* CIAUpdatePlatformVersionRoofXmlParser::NewLC( 
                                              CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformVersionRoofXmlParser* self =
        new( ELeave ) CIAUpdatePlatformVersionRoofXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdatePlatformVersionRoofXmlParser::CIAUpdatePlatformVersionRoofXmlParser( 
                                              CIAUpdatePlatformDependency& aDependency )
: CIAUpdatePlatformVersionXmlParser( aDependency )
    {
    Version().SetToRoof();
    }
    
    
void CIAUpdatePlatformVersionRoofXmlParser::ConstructL()
    {
    CIAUpdatePlatformVersionXmlParser::ConstructL( 
                                        IAUpdateProtocolConsts::KPlatformVersionTo() );    
    }


CIAUpdatePlatformVersionRoofXmlParser::~CIAUpdatePlatformVersionRoofXmlParser()
    {
    }


void CIAUpdatePlatformVersionRoofXmlParser::OnStartElementL( 
                                              const Xml::RTagInfo& aElement, 
                                              const Xml::RAttributeArray& aAttributes, 
                                              TInt aErrorCode )
    {
    // Let the parent do necessary initializations.
    CIAUpdatePlatformVersionXmlParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    // If the element is for this element, then set the version information to roof
    // as default because this parser may be used multiple times. We do not want old
    // values be lef hanging.
    if ( AcceptData() )
        {
        Version().SetToRoof();
        }            
    }


void CIAUpdatePlatformVersionRoofXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                           TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdatePlatformVersionXmlParser::OnEndElementL( aElement, aErrorCode );

    if ( IsElementEnded() )
        {
        // We have finished the parsing of this element.
        // Now, set the version information to the dependency object.
        Dependency().SetVersionRoof( Version() );
        }
    }
