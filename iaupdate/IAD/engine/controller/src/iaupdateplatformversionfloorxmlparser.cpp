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

#include "iaupdateplatformversionfloorxmlparser.h"
#include "iaupdateplatformdependency.h"
#include "iaupdateprotocolconsts.h"


CIAUpdatePlatformVersionFloorXmlParser* CIAUpdatePlatformVersionFloorXmlParser::NewL( 
                                                  CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformVersionFloorXmlParser* self =
        CIAUpdatePlatformVersionFloorXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdatePlatformVersionFloorXmlParser* CIAUpdatePlatformVersionFloorXmlParser::NewLC( 
                                                 CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformVersionFloorXmlParser* self =
        new( ELeave ) CIAUpdatePlatformVersionFloorXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdatePlatformVersionFloorXmlParser::CIAUpdatePlatformVersionFloorXmlParser( 
                                                 CIAUpdatePlatformDependency& aDependency )
: CIAUpdatePlatformVersionXmlParser( aDependency )
    {
    Version().SetToFloor();
    }
    
    
void CIAUpdatePlatformVersionFloorXmlParser::ConstructL()
    {
    CIAUpdatePlatformVersionXmlParser::ConstructL( 
                                          IAUpdateProtocolConsts::KPlatformVersionFrom() );    
    }


CIAUpdatePlatformVersionFloorXmlParser::~CIAUpdatePlatformVersionFloorXmlParser()
    {
    }


void CIAUpdatePlatformVersionFloorXmlParser::OnStartElementL( 
                                       const Xml::RTagInfo& aElement, 
                                       const Xml::RAttributeArray& aAttributes, 
                                       TInt aErrorCode )
    {
    // Let the parent do necessary initializations.
    CIAUpdatePlatformVersionXmlParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    // If the element is for this element, then set the version information to floor
    // as default because this parser may be used multiple times. We do not want old
    // values be lef hanging.
    if ( AcceptData() )
        {
        Version().SetToFloor();
        }            
    }


void CIAUpdatePlatformVersionFloorXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                            TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdatePlatformVersionXmlParser::OnEndElementL( aElement, aErrorCode );

    if ( IsElementEnded() )
        {
        // We have finished the parsing of this element.
        // Now, set the version information to the dependency object.
        Dependency().SetVersionFloor( Version() );
        }
    }
