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

#include "iaupdateplatformdependencyxmlparser.h"
#include "iaupdateplatformversionfloorxmlparser.h"
#include "iaupdateplatformversionroofxmlparser.h"
#include "iaupdateplatformdescriptionxmlparser.h"
#include "iaupdateprotocolconsts.h"


CIAUpdatePlatformDependencyXmlParser* CIAUpdatePlatformDependencyXmlParser::NewL( 
                                            CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformDependencyXmlParser* self =
        CIAUpdatePlatformDependencyXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdatePlatformDependencyXmlParser* CIAUpdatePlatformDependencyXmlParser::NewLC( 
                                            CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformDependencyXmlParser* self =
        new( ELeave ) CIAUpdatePlatformDependencyXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdatePlatformDependencyXmlParser::CIAUpdatePlatformDependencyXmlParser( 
                                           CIAUpdatePlatformDependency& aDependency )
: CIAUpdateXmlSubParser(),
  iDependency( aDependency )
    {
    
    }
    
    
void CIAUpdatePlatformDependencyXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KPlatformDependency() );    

    // Add sub parsers to the list
    
    CIAUpdatePlatformVersionXmlParser* versionFloorParser(
        CIAUpdatePlatformVersionFloorXmlParser::NewLC( Dependency() ) );
    SubParsers().AppendL( versionFloorParser );
    CleanupStack::Pop( versionFloorParser );

    CIAUpdatePlatformVersionXmlParser* versionRoofParser(
        CIAUpdatePlatformVersionRoofXmlParser::NewLC( Dependency() ) );
    SubParsers().AppendL( versionRoofParser );
    CleanupStack::Pop( versionRoofParser );

    CIAUpdatePlatformDescriptionXmlParser* platformParser(
        CIAUpdatePlatformDescriptionXmlParser::NewLC( Dependency() ) );
    SubParsers().AppendL( platformParser );
    CleanupStack::Pop( platformParser );
    }


CIAUpdatePlatformDependencyXmlParser::~CIAUpdatePlatformDependencyXmlParser()
    {
    }


CIAUpdatePlatformDependency& CIAUpdatePlatformDependencyXmlParser::Dependency()
    {
    return iDependency;
    }
