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

#include "iaupdateplatformdescriptionxmlparser.h"
#include "iaupdateplatformdependency.h"
#include "iaupdateprotocolconsts.h"


CIAUpdatePlatformDescriptionXmlParser* CIAUpdatePlatformDescriptionXmlParser::NewL( 
                                               CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformDescriptionXmlParser* self =
        CIAUpdatePlatformDescriptionXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdatePlatformDescriptionXmlParser* CIAUpdatePlatformDescriptionXmlParser::NewLC( 
                                               CIAUpdatePlatformDependency& aDependency )
    {
    CIAUpdatePlatformDescriptionXmlParser* self =
        new( ELeave ) CIAUpdatePlatformDescriptionXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdatePlatformDescriptionXmlParser::CIAUpdatePlatformDescriptionXmlParser( 
                                              CIAUpdatePlatformDependency& aDependency )
: CIAUpdateXmlSubParser(),
  iDependency( aDependency )
    {
    
    }
    
    
void CIAUpdatePlatformDescriptionXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KPlatformDescription() );    
    }


CIAUpdatePlatformDescriptionXmlParser::~CIAUpdatePlatformDescriptionXmlParser()
    {
    }


void CIAUpdatePlatformDescriptionXmlParser::OnContentL( const TDesC8& aBytes, 
                                                        TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdateXmlSubParser::OnContentL( aBytes, aErrorCode );

    if ( AcceptData() )
        {
        // Because there is no current subparser set, this means
        // that this parser is the leaf parser at the moment.

        // We have gotten content for this element.
        // Now, set the platform information to the dependency object.
        Dependency().SetPlatformL( aBytes );
        }
    }


CIAUpdatePlatformDependency& CIAUpdatePlatformDescriptionXmlParser::Dependency()
    {
    return iDependency;
    }

