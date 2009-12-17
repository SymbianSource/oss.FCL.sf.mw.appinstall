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

#include "iaupdateversionbuildxmlparser.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateVersionBuildXmlParser* CIAUpdateVersionBuildXmlParser::NewL( TIAUpdateVersion& aVersion )
    {
    CIAUpdateVersionBuildXmlParser* self =
        CIAUpdateVersionBuildXmlParser::NewLC( aVersion );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateVersionBuildXmlParser* CIAUpdateVersionBuildXmlParser::NewLC( TIAUpdateVersion& aVersion )
    {
    CIAUpdateVersionBuildXmlParser* self =
        new( ELeave ) CIAUpdateVersionBuildXmlParser( aVersion );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateVersionBuildXmlParser::CIAUpdateVersionBuildXmlParser( TIAUpdateVersion& aVersion )
: CIAUpdateXmlSubParser(),
  iVersion( aVersion )
    {
    
    }
    
    
void CIAUpdateVersionBuildXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KVersionBuild() );    
    }


CIAUpdateVersionBuildXmlParser::~CIAUpdateVersionBuildXmlParser()
    {
    }


void CIAUpdateVersionBuildXmlParser::OnContentL( const TDesC8& aBytes, 
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
        TInt value( 0 );
        TLex8 lex( aBytes );
        // If aBytes value was corrputed, 
        // then just set the default value.
        lex.Val( value );
        Version().iBuild = value;
        }
    }


TIAUpdateVersion& CIAUpdateVersionBuildXmlParser::Version()
    {
    return iVersion;
    }

