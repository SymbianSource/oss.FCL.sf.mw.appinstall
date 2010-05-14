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
* Description:  
*
*/


#include "ncd_cp_clientconfigurationimpl.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "ncd_cp_cookie.h"
#include "ncd_cp_detailimpl.h"

CNcdConfigurationProtocolClientConfigurationImpl* CNcdConfigurationProtocolClientConfigurationImpl::NewL()
    {
    CNcdConfigurationProtocolClientConfigurationImpl* self = new(ELeave) 
        CNcdConfigurationProtocolClientConfigurationImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolClientConfigurationImpl::CNcdConfigurationProtocolClientConfigurationImpl()
: iCookies( 0 ),
  iDetails( 0 )
{
}

void CNcdConfigurationProtocolClientConfigurationImpl::ConstructL()
    {
    }

CNcdConfigurationProtocolClientConfigurationImpl::~CNcdConfigurationProtocolClientConfigurationImpl()
    {
    if( iCookies ) 
        {
        iCookies->ResetAndDestroy();
        delete iCookies;
        iCookies = 0;
        }
    if( iDetails )
        {
        iDetails->ResetAndDestroy();
        delete iDetails;
        iDetails = 0;
        }
    }

TInt CNcdConfigurationProtocolClientConfigurationImpl::ExpirationDelta() const
    {
    return iExpirationDelta;
    }

TInt CNcdConfigurationProtocolClientConfigurationImpl::CookieCount() const
    {
    if( iCookies )
        {
        return iCookies->Count();
        }
    else
        {
        return 0;
        }
    }

MNcdConfigurationProtocolCookie& CNcdConfigurationProtocolClientConfigurationImpl::CookieL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iCookies->Count() );
    if ( aIndex < 0 || aIndex >= iCookies->Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iCookies->At( aIndex ));
    }

TInt CNcdConfigurationProtocolClientConfigurationImpl::DetailCount() const
    {
    if( iDetails )
        {
        return iDetails->Count();
        }
    else
        {
        return 0;
        }
    }

const MNcdConfigurationProtocolDetail& CNcdConfigurationProtocolClientConfigurationImpl::DetailL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDetails->Count() );
    if ( aIndex < 0 || aIndex >= iDetails->Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iDetails->At( aIndex ));
    }
