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
#include <ncdutils.h>

#include "iaupdatenodeembeddedxmlparser.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateNodeEmbeddedXmlParser* CIAUpdateNodeEmbeddedXmlParser::NewL( 
                                      CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeEmbeddedXmlParser* self =
        CIAUpdateNodeEmbeddedXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateNodeEmbeddedXmlParser* CIAUpdateNodeEmbeddedXmlParser::NewLC( 
                                      CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeEmbeddedXmlParser* self =
        new( ELeave ) CIAUpdateNodeEmbeddedXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateNodeEmbeddedXmlParser::CIAUpdateNodeEmbeddedXmlParser( 
                                      CIAUpdateNodeDependency*& aDependency )
: CIAUpdateXmlSubParser(),
  iDependency( aDependency )
    {
    
    }
    
    
void CIAUpdateNodeEmbeddedXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KNodeEmbedded() );    
    }


CIAUpdateNodeEmbeddedXmlParser::~CIAUpdateNodeEmbeddedXmlParser()
    {
    }


void CIAUpdateNodeEmbeddedXmlParser::OnContentL( const TDesC8& aBytes, 
                                                 TInt aErrorCode )
    {
    // If the parent wants to do something with the information.
    CIAUpdateXmlSubParser::OnContentL( aBytes, aErrorCode );

    // Check if this is the leaf parser and no other sub parser should 
    // handle this.
    if ( AcceptData() )
        {
        // We have gotten content for this element.
        // Now, set the node information to the dependency object.
        _LIT8( KTrue, "true" );
        _LIT8( KFalse, "false" );
        _LIT8( KOneTrue, "1" );
        _LIT8( KZeroFalse, "0" );

        TBool embedded( EFalse );    
        if ( aBytes.CompareF( KTrue() ) == 0 
             || aBytes.CompareF( KOneTrue() ) == 0 )
            {
            embedded = ETrue;
            }
        else if ( aBytes.CompareF( KFalse() ) == 0 
                  || aBytes.CompareF( KZeroFalse() ) == 0 )
            {
            embedded = EFalse;
            }
        else
            {
            // The content is corrupted.
            User::Leave( KErrArgument );
            }
            
        DependencyL().SetEmbedded( embedded );
        }
    }


CIAUpdateNodeDependency& CIAUpdateNodeEmbeddedXmlParser::DependencyL()
    {
    if ( !iDependency )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iDependency;
    }

