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

#include "iaupdatenodeuidxmlparser.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateutils.h"
#include "iaupdateprotocolconsts.h"


CIAUpdateNodeUidXmlParser* CIAUpdateNodeUidXmlParser::NewL( 
                                     CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeUidXmlParser* self =
        CIAUpdateNodeUidXmlParser::NewLC( aDependency );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateNodeUidXmlParser* CIAUpdateNodeUidXmlParser::NewLC( 
                                    CIAUpdateNodeDependency*& aDependency )
    {
    CIAUpdateNodeUidXmlParser* self =
        new( ELeave ) CIAUpdateNodeUidXmlParser( aDependency );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateNodeUidXmlParser::CIAUpdateNodeUidXmlParser( 
                                    CIAUpdateNodeDependency*& aDependency )
: CIAUpdateXmlSubParser(),
  iDependency( aDependency )
    {
    
    }
    
    
void CIAUpdateNodeUidXmlParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateProtocolConsts::KNodeUid() );    
    }


CIAUpdateNodeUidXmlParser::~CIAUpdateNodeUidXmlParser()
    {
    }


void CIAUpdateNodeUidXmlParser::OnContentL( const TDesC8& aBytes, 
                                            TInt aErrorCode )
    {
    // If the parent class wants to do something with the information.
    CIAUpdateXmlSubParser::OnContentL( aBytes, aErrorCode );

    if ( AcceptData() )
        {
        // Because there is no current subparser set, this means
        // that this parser is the leaf parser at the moment.

        // We have gotten content for this element.
        // Now, set the node information to the dependency object.
        HBufC* tmp( HBufC::NewLC( aBytes.Length() * 2 ) );
        TPtr ptr( tmp->Des() );
        ptr.Copy( aBytes );
        // Get the UID value for the dependency
        TUid uid( TUid::Uid( IAUpdateUtils::DesHexToIntL( ptr ) ) );
        DependencyL().SetUid( uid );
        CleanupStack::PopAndDestroy( tmp );
        }
    }


CIAUpdateNodeDependency& CIAUpdateNodeUidXmlParser::DependencyL()
    {
    if ( !iDependency )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iDependency;
    }

