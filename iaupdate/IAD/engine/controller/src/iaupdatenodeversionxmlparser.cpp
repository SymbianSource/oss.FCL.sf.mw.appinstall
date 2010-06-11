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

#include "iaupdatenodeversionxmlparser.h"
#include "iaupdateversionmajorxmlparser.h"
#include "iaupdateversionminorxmlparser.h"
#include "iaupdateversionbuildxmlparser.h"
#include "iaupdateprotocolconsts.h"



CIAUpdateNodeVersionXmlParser::CIAUpdateNodeVersionXmlParser( 
                                         CIAUpdateNodeDependency*& aDependency )
: CIAUpdateXmlSubParser(),
  iDependency( aDependency )
    {
    
    }
    
    
void CIAUpdateNodeVersionXmlParser::ConstructL( const TDesC8& aElementLocalName )
    {
    CIAUpdateXmlSubParser::ConstructL( aElementLocalName );    

    // Create sub parsers for the version element handling

    CIAUpdateVersionMajorXmlParser* majorParser(
        CIAUpdateVersionMajorXmlParser::NewLC( Version() ) );
    SubParsers().AppendL( majorParser );
    CleanupStack::Pop( majorParser );

    CIAUpdateVersionMinorXmlParser* minorParser(
        CIAUpdateVersionMinorXmlParser::NewLC( Version() ) );
    SubParsers().AppendL( minorParser );
    CleanupStack::Pop( minorParser );

    CIAUpdateVersionBuildXmlParser* buildParser(
        CIAUpdateVersionBuildXmlParser::NewLC( Version() ) );
    SubParsers().AppendL( buildParser );
    CleanupStack::Pop( buildParser );    
    }


CIAUpdateNodeVersionXmlParser::~CIAUpdateNodeVersionXmlParser()
    {
    }


CIAUpdateNodeDependency& CIAUpdateNodeVersionXmlParser::DependencyL()
    {
    if ( !iDependency )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iDependency;
    }


TIAUpdateVersion& CIAUpdateNodeVersionXmlParser::Version()
    {
    return iVersion;
    }
    
