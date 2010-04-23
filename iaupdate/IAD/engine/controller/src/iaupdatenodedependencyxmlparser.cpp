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

#include "iaupdatenodedependencyxmlparser.h"
#include "iaupdateinterdepxmlparser.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateNodeDependencyXmlParser* CIAUpdateNodeDependencyXmlParser::NewL( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
    {
    CIAUpdateNodeDependencyXmlParser* self =
        CIAUpdateNodeDependencyXmlParser::NewLC( aDependencies );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateNodeDependencyXmlParser* CIAUpdateNodeDependencyXmlParser::NewLC( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
    {
    CIAUpdateNodeDependencyXmlParser* self =
        new( ELeave ) CIAUpdateNodeDependencyXmlParser( aDependencies );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateNodeDependencyXmlParser::CIAUpdateNodeDependencyXmlParser( 
                       RPointerArray< CIAUpdateNodeDependency >& aDependencies )
: CIAUpdateXmlSubParser(),
  iDependencies( aDependencies )
    {
    
    }
    
    
void CIAUpdateNodeDependencyXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KNodeDependencies() );    

    // Add sub parsers to the list
    
    CIAUpdateInterDepXmlParser* parser(
        CIAUpdateInterDepXmlParser::NewLC( Dependencies() ) );
    SubParsers().AppendL( parser );
    CleanupStack::Pop( parser );
    }


CIAUpdateNodeDependencyXmlParser::~CIAUpdateNodeDependencyXmlParser()
    {
    }


RPointerArray< CIAUpdateNodeDependency >& CIAUpdateNodeDependencyXmlParser::Dependencies()
    {
    return iDependencies;
    }
