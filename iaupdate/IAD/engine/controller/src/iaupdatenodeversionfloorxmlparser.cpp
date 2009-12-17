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

#include "iaupdatenodeversionfloorxmlparser.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateNodeVersionFloorXmlParser* CIAUpdateNodeVersionFloorXmlParser::NewL( 
                                                 CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeVersionFloorXmlParser* self =
        CIAUpdateNodeVersionFloorXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateNodeVersionFloorXmlParser* CIAUpdateNodeVersionFloorXmlParser::NewLC( 
                                                 CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeVersionFloorXmlParser* self =
        new( ELeave ) CIAUpdateNodeVersionFloorXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateNodeVersionFloorXmlParser::CIAUpdateNodeVersionFloorXmlParser( 
                                                 CIAUpdateNodeDependency*& aDependency )
: CIAUpdateNodeVersionXmlParser( aDependency )
    {
    Version().SetToFloor();
    }
    
    
void CIAUpdateNodeVersionFloorXmlParser::ConstructL()
    {
    CIAUpdateNodeVersionXmlParser::ConstructL( IAUpdateProtocolConsts::KNodeVersionFrom() );    
    }


CIAUpdateNodeVersionFloorXmlParser::~CIAUpdateNodeVersionFloorXmlParser()
    {
    }


void CIAUpdateNodeVersionFloorXmlParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                          const Xml::RAttributeArray& aAttributes, 
                                                          TInt aErrorCode )
    {
    // Let the parent do necessary initializing.
    CIAUpdateNodeVersionXmlParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    // If the element is for this element, then set the version information to floor
    // as default because this parser may be used multiple times. We do not want old
    // values be lef hanging. 
    if ( AcceptData() )
        {
        Version().SetToFloor();
        }            
    }
    

void CIAUpdateNodeVersionFloorXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                        TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdateNodeVersionXmlParser::OnEndElementL( aElement, aErrorCode );

    if ( IsElementEnded() )
        {
        // We have finished the parsing of this element.
        // Now, set the version information to the dependency object.
        DependencyL().SetVersionFloor( Version() );
        }
    }
