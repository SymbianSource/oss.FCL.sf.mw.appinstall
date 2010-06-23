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



#include "iaupdatedependencyxmlparser.h"
#include "iaupdatedependencyxmlsubparser.h"


CIAUpdateDependencyXmlParser* CIAUpdateDependencyXmlParser::NewL( 
                  RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                  CIAUpdatePlatformDependency& aPlatformDependency )
    {
    CIAUpdateDependencyXmlParser* self =
        CIAUpdateDependencyXmlParser::NewLC( aNodeDependencies, 
                                             aPlatformDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateDependencyXmlParser* CIAUpdateDependencyXmlParser::NewLC( 
                  RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                  CIAUpdatePlatformDependency& aPlatformDependency )
    {
    // Create sub parser that will handle the given elements.
    CIAUpdateDependencyXmlSubParser* subParser =
        CIAUpdateDependencyXmlSubParser::NewLC( aNodeDependencies, aPlatformDependency );

    // Create the actual XML parser that uses subParser.
    // Notice, that the ownership of the subParser is
    // transferred here to the parser.
    CIAUpdateDependencyXmlParser* self =
        new( ELeave ) CIAUpdateDependencyXmlParser( subParser );

    CleanupStack::Pop( subParser );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateDependencyXmlParser::CIAUpdateDependencyXmlParser( 
                                   CIAUpdateDependencyXmlSubParser* aSubParser )
: CIAUpdateXmlParser( aSubParser )
    {
    
    }
    
    
void CIAUpdateDependencyXmlParser::ConstructL()
    {
    CIAUpdateXmlParser::ConstructL();
    }


CIAUpdateDependencyXmlParser::~CIAUpdateDependencyXmlParser()
    {
    }
