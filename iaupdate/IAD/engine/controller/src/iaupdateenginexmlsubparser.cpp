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

#include "iaupdateenginexmlsubparser.h"
#include "iaupdateengineconfigdata.h"
#include "iaupdateengineconfigconsts.h"


CIAUpdateEngineXmlSubParser* CIAUpdateEngineXmlSubParser::NewL( 
                                         CIAUpdateEngineConfigData& aConfigData )
    {
    CIAUpdateEngineXmlSubParser* self =
        CIAUpdateEngineXmlSubParser::NewLC( aConfigData );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateEngineXmlSubParser* CIAUpdateEngineXmlSubParser::NewLC( 
                                         CIAUpdateEngineConfigData& aConfigData )
    {
    CIAUpdateEngineXmlSubParser* self =
        new( ELeave ) CIAUpdateEngineXmlSubParser( aConfigData );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateEngineXmlSubParser::CIAUpdateEngineXmlSubParser( 
                                         CIAUpdateEngineConfigData& aConfigData )
: CIAUpdateXmlSubParser(),
  iConfigData( aConfigData )
    {
    
    }
    
    
void CIAUpdateEngineXmlSubParser::ConstructL()
    {
    CIAUpdateXmlSubParser::ConstructL( IAUpdateEngineConfigConsts::KIAElementLocalName );
    }


CIAUpdateEngineXmlSubParser::~CIAUpdateEngineXmlSubParser()
    {
    }


const CIAUpdateEngineConfigData& CIAUpdateEngineXmlSubParser::ConfigData() const
    {
    return iConfigData;
    }


void CIAUpdateEngineXmlSubParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                   const Xml::RAttributeArray& aAttributes, 
                                                   TInt aErrorCode )
    {
    CIAUpdateXmlSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    if ( aErrorCode == KErrNone )
        {
        const TDesC8& element = aElement.LocalName().DesC();

        // KIAUpdateEngineConfig starts the whole configuration info.
        // But, we are not so picky here. So, just let it go through
        // and forget checkings. Also, if unknown element is given here,
        // just ignore it.

        if ( element == IAUpdateEngineConfigConsts::KIAStorage )
            {
            HandleStorageAttributesL( aAttributes );            
            }
        else if ( element == IAUpdateEngineConfigConsts::KIAMasterServer )
            {
            HandleMasterServerAttributesL( aAttributes );
            }
        else if ( element == IAUpdateEngineConfigConsts::KIAProvisioning )
            {
            HandleProvisioningAttributesL( aAttributes );
            }
        else if ( element == IAUpdateEngineConfigConsts::KIAClientRole )
            {
            HandleClientRoleAttributesL( aAttributes );
            }
        }
    }


void CIAUpdateEngineXmlSubParser::HandleStorageAttributesL( 
                                const Xml::RAttributeArray& aAttributes )
    {
    for ( TInt i = 0; i < aAttributes.Count(); ++ i )
        {
        const TDesC8& attributeName =
            aAttributes[ i ].Attribute().LocalName().DesC();
        
        const TDesC8& attributeValue =
            aAttributes[ i ].Value().DesC();

        HBufC* value( HBufC::NewLC( attributeValue.Length() ) );
        value->Des().Copy( attributeValue );

        if ( attributeName == IAUpdateEngineConfigConsts::KIAMaxSize )
            {
            iConfigData.SetStorageMaxSizeL( *value );
            }

        CleanupStack::PopAndDestroy( value );                    
        }    
    }
    
    
void CIAUpdateEngineXmlSubParser::HandleMasterServerAttributesL( 
                                const Xml::RAttributeArray& aAttributes )
    {
    for ( TInt i = 0; i < aAttributes.Count(); ++ i )
        {
        const TDesC8& attributeName =
            aAttributes[ i ].Attribute().LocalName().DesC();
        
        const TDesC8& attributeValue =
            aAttributes[ i ].Value().DesC();

        HBufC* value( HBufC::NewLC( attributeValue.Length() ) );
        value->Des().Copy( attributeValue );

        if ( attributeName == IAUpdateEngineConfigConsts::KIAUri )
            {
            iConfigData.SetMasterServerUriL( *value );
            }

        CleanupStack::PopAndDestroy( value );                    
        }    
    }

    
void CIAUpdateEngineXmlSubParser::HandleProvisioningAttributesL( 
                                const Xml::RAttributeArray& aAttributes )
    {
    for ( TInt i = 0; i < aAttributes.Count(); ++ i )
        {
        const TDesC8& attributeName =
            aAttributes[ i ].Attribute().LocalName().DesC();
        
        const TDesC8& attributeValue =
            aAttributes[ i ].Value().DesC();

        HBufC* value( HBufC::NewLC( attributeValue.Length() ) );
        value->Des().Copy( attributeValue );

        if ( attributeName == IAUpdateEngineConfigConsts::KIAValue )
            {
            iConfigData.SetProvisioningL( *value );
            }

        CleanupStack::PopAndDestroy( value );                    
        }    
    }
    

void CIAUpdateEngineXmlSubParser::HandleClientRoleAttributesL( 
                                const Xml::RAttributeArray& aAttributes )
    {
    for ( TInt i = 0; i < aAttributes.Count(); ++ i )
        {
        const TDesC8& attributeName =
            aAttributes[ i ].Attribute().LocalName().DesC();
        
        const TDesC8& attributeValue =
            aAttributes[ i ].Value().DesC();

        HBufC* value( HBufC::NewLC( attributeValue.Length() ) );
        value->Des().Copy( attributeValue );

        if ( attributeName == IAUpdateEngineConfigConsts::KIAValue )
            {
            iConfigData.SetClientRoleL( *value );
            }

        CleanupStack::PopAndDestroy( value );                    
        }    
    }
