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



#include "iaupdatedependencyxmlsubparser.h"
#include "iaupdatenodedependencyxmlparser.h"
#include "iaupdateplatformdependencyxmlparser.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateDependencyXmlSubParser* CIAUpdateDependencyXmlSubParser::NewL( 
                     RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                     CIAUpdatePlatformDependency& aPlatformDependency )
    {
    CIAUpdateDependencyXmlSubParser* self =
        CIAUpdateDependencyXmlSubParser::NewLC( aNodeDependencies, aPlatformDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateDependencyXmlSubParser* CIAUpdateDependencyXmlSubParser::NewLC( 
                     RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                     CIAUpdatePlatformDependency& aPlatformDependency )
    {
    CIAUpdateDependencyXmlSubParser* self =
        new( ELeave ) CIAUpdateDependencyXmlSubParser();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeDependencies, aPlatformDependency );
    return self;    
    }


CIAUpdateDependencyXmlSubParser::CIAUpdateDependencyXmlSubParser()
: CIAUpdateXmlSubParser()
    {
    
    }
    
    
void CIAUpdateDependencyXmlSubParser::ConstructL( 
                      RPointerArray< CIAUpdateNodeDependency>& aNodeDependencies,
                      CIAUpdatePlatformDependency& aPlatformDependency )
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KDependencies() );
    
    // Add sub parsers to the list
    CIAUpdateNodeDependencyXmlParser* nodeDependencyParser(
        CIAUpdateNodeDependencyXmlParser::NewLC( aNodeDependencies ) );
    SubParsers().AppendL( nodeDependencyParser );
    CleanupStack::Pop( nodeDependencyParser );
    
    CIAUpdatePlatformDependencyXmlParser* platformDependencyParser(
        CIAUpdatePlatformDependencyXmlParser::NewLC( aPlatformDependency ) );
    SubParsers().AppendL( platformDependencyParser );
    CleanupStack::Pop( platformDependencyParser );    
    }


CIAUpdateDependencyXmlSubParser::~CIAUpdateDependencyXmlSubParser()
    {
    }
