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

#include "iaupdateinterdepxmlparser.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdatenodeversionfloorxmlparser.h"
#include "iaupdatenodeversionroofxmlparser.h"
#include "iaupdatenodeuidxmlparser.h"
#include "iaupdatenodeembeddedxmlparser.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateInterDepXmlParser* CIAUpdateInterDepXmlParser::NewL( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
    {
    CIAUpdateInterDepXmlParser* self =
        CIAUpdateInterDepXmlParser::NewLC( aDependencies );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateInterDepXmlParser* CIAUpdateInterDepXmlParser::NewLC( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
    {
    CIAUpdateInterDepXmlParser* self =
        new( ELeave ) CIAUpdateInterDepXmlParser( aDependencies );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateInterDepXmlParser::CIAUpdateInterDepXmlParser( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
: CIAUpdateXmlSubParser(),
  iDependencies( aDependencies )
    {
    
    }
    
    
void CIAUpdateInterDepXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KNodeDependency() );
    
    // Add sub parsers to the list
    
    // Notice! Because this parser may be used for multiple dependencies, the sub parsers
    // that are created here will take a pointer reference as their parameter. This way,
    // when the member variable changes here when dependency object is created, the sub parsers
    // are able to use the correct object when setting values.
    
    CIAUpdateNodeVersionXmlParser* versionFloorParser(
        CIAUpdateNodeVersionFloorXmlParser::NewLC( iDependency ) );
    SubParsers().AppendL( versionFloorParser );
    CleanupStack::Pop( versionFloorParser );

    CIAUpdateNodeVersionXmlParser* versionRoofParser(
        CIAUpdateNodeVersionRoofXmlParser::NewLC( iDependency ) );
    SubParsers().AppendL( versionRoofParser );
    CleanupStack::Pop( versionRoofParser );

    CIAUpdateNodeUidXmlParser* uidParser(
        CIAUpdateNodeUidXmlParser::NewLC( iDependency ) );
    SubParsers().AppendL( uidParser );
    CleanupStack::Pop( uidParser );

    CIAUpdateNodeEmbeddedXmlParser* embeddedParser(
        CIAUpdateNodeEmbeddedXmlParser::NewLC( iDependency ) );
    SubParsers().AppendL( embeddedParser );
    CleanupStack::Pop( embeddedParser );
    }


CIAUpdateInterDepXmlParser::~CIAUpdateInterDepXmlParser()
    {
    // Delete the dependency if it has been left hanging for some reason.
    delete iDependency;
    }


void CIAUpdateInterDepXmlParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                  const Xml::RAttributeArray& aAttributes, 
                                                  TInt aErrorCode )
    {
    // Let the parent do forwarding to the sub parsers if necessary.
    CIAUpdateXmlSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    // If the element is for this element, then create a new dependency object that will
    // be inserted to the dependency array when this element has been handled.
    if ( AcceptData() )
        {
        // Element parameter is for this class object.
        // If for some reason dependency has been hanging because of some error situation
        delete iDependency;
        iDependency = NULL;
        iDependency = CIAUpdateNodeDependency::NewL();
        }        
    }

void CIAUpdateInterDepXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                TInt aErrorCode )
    {
    // Let the parent do forwarding to the sub parsers if necessary.
    CIAUpdateXmlSubParser::OnEndElementL( aElement, aErrorCode );

    if ( IsElementEnded() 
         && iDependency )
        {
        // Element parameter is for this class object.
        // Append the handled dependency into the dependency array.
        iDependencies.AppendL( iDependency );
        iDependency = NULL;
        }            
    }
