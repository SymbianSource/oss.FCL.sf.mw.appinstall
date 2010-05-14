/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdEntityRefParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_cp_cookieimpl.h"
#include "ncd_parser_cp_cookie.h"
#include "catalogsdebug.h"

CNcdConfigurationProtocolCookieParser* 
CNcdConfigurationProtocolCookieParser::NewL( MNcdParserObserverBundle& aObservers,
                                             MNcdSubParserObserver& aSubParserObserver,
                                             TInt aDepth,
                                             const Xml::RTagInfo& aElement,
                                             const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolCookieParser* self 
        = new(ELeave) CNcdConfigurationProtocolCookieParser( aObservers,
                                                             aSubParserObserver,
                                                             aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolCookieParser::CNcdConfigurationProtocolCookieParser( MNcdParserObserverBundle& aObservers,
                                                                              MNcdSubParserObserver& aSubParserObserver,
                                                                              TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth ),
      iCurrentCookie( 0 ),
      iCookies( 0 )
    {
    }

CNcdConfigurationProtocolCookieParser::~CNcdConfigurationProtocolCookieParser()
    {
    if( iCookies )
        {
        iCookies->ResetAndDestroy();
        delete iCookies;
        iCookies = 0;
        }
    
    if( iCurrentCookie )
        {
        delete iCurrentCookie;
        iCurrentCookie = 0;
        }
    }

void CNcdConfigurationProtocolCookieParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );
    iCookies = new(ELeave) CArrayPtrFlat<MNcdConfigurationProtocolCookie>( 4 );
    }

void CNcdConfigurationProtocolCookieParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                             const Xml::RAttributeArray& aAttributes, 
                                                             TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("cookie start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

    if( tag == KTagCookie )
        {
        iCurrentCookie = CNcdConfigurationProtocolCookie::NewL();
        
        // cookie attributes

        // key
        TPtrC8 key = AttributeValue( KAttrKey, aAttributes );
        if( key != KNullDesC8 )
            NcdProtocolUtils::AssignDesL( iCurrentCookie->iKey, key );
        
        // type
        TPtrC8 type = AttributeValue( KAttrType, aAttributes );
        if( type != KNullDesC8 )
            NcdProtocolUtils::AssignDesL( iCurrentCookie->iType, type );

        // scope
        TPtrC8 scope = AttributeValue( KAttrScope, aAttributes );
        if( scope != KNullDesC8 )
            NcdProtocolUtils::AssignDesL( iCurrentCookie->iScope, scope );

        // expDelta
        TPtrC8 expDelta = AttributeValue( KAttrExpirationDelta, aAttributes );
        if( expDelta != KNullDesC8 )
            {
            iCurrentCookie->iExpirationDelta = NcdProtocolUtils::DesDecToIntL( expDelta );
            }
        }
    
    else if( tag == KTagValue )
        {
        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }

void CNcdConfigurationProtocolCookieParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    
    else if( tag == KTagCookie && iCurrentCookie )
        {
        iCookies->AppendL( iCurrentCookie, 1 );
        iCurrentCookie = 0;
        }

    else if( tag == KTagValue && iBuffer )
        {
        HBufC* value = 0;
        NcdProtocolUtils::AssignDesL( value, *iBuffer );
        CleanupDeletePushL( value );
        iCurrentCookie->iValues.AppendL( value );
        CleanupStack::Pop( value );
        }
    
    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    }

CArrayPtr<MNcdConfigurationProtocolCookie>* CNcdConfigurationProtocolCookieParser::Cookies()
{
    CArrayPtr<MNcdConfigurationProtocolCookie>* returnPtr = iCookies;
    iCookies = 0;
    return returnPtr;
}

