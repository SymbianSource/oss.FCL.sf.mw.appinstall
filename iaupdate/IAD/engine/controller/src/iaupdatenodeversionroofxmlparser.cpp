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

#include "iaupdatenodeversionroofxmlparser.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateNodeVersionRoofXmlParser* CIAUpdateNodeVersionRoofXmlParser::NewL( 
                                                   CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeVersionRoofXmlParser* self =
        CIAUpdateNodeVersionRoofXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateNodeVersionRoofXmlParser* CIAUpdateNodeVersionRoofXmlParser::NewLC( 
                                                   CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeVersionRoofXmlParser* self =
        new( ELeave ) CIAUpdateNodeVersionRoofXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateNodeVersionRoofXmlParser::CIAUpdateNodeVersionRoofXmlParser( 
                                                   CIAUpdateNodeDependency*& aDependency )
: CIAUpdateNodeVersionXmlParser( aDependency )
    {
    Version().SetToRoof();
    }
    
    
void CIAUpdateNodeVersionRoofXmlParser::ConstructL()
    {
    CIAUpdateNodeVersionXmlParser::ConstructL( IAUpdateProtocolConsts::KNodeVersionTo() );    
    }


CIAUpdateNodeVersionRoofXmlParser::~CIAUpdateNodeVersionRoofXmlParser()
    {
    }


void CIAUpdateNodeVersionRoofXmlParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                         const Xml::RAttributeArray& aAttributes, 
                                                         TInt aErrorCode )
    {
    // Let the parent do necessary initializations.
    CIAUpdateNodeVersionXmlParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    // If the element is for this element, then set the version information to roof
    // as default because this parser may be used multiple times. We do not want old
    // values be lef hanging. 
    if ( AcceptData() )
        {
        Version().SetToRoof();
        }            
    }


void CIAUpdateNodeVersionRoofXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                       TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdateNodeVersionXmlParser::OnEndElementL( aElement, aErrorCode );

    if ( IsElementEnded() )
        {
        // We have finished the parsing of this element.
        // Now, set the version information to the dependency object.
        DependencyL().SetVersionRoof( Version() );
        }
    }
