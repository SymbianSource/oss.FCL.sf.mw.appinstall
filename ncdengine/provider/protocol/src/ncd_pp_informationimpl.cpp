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


#include "ncdprotocolutils.h"
#include "ncd_pp_informationimpl.h"
#include "ncd_cp_cookie.h"
#include "ncd_cp_queryimpl.h"
#include "ncd_pp_expiredcacheddataimpl.h"
#include "ncd_cp_serverdetails.h"
#include "catalogsdebug.h"

void CNcdPreminetProtocolInformationImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL(iNamespace);
    
    // initialize resendAfter value, -1 means undefined
    iResendAfter = -1;
    }

CNcdPreminetProtocolInformationImpl::~CNcdPreminetProtocolInformationImpl()
    {
    DLTRACEIN(("Deleting cookies"));
    if( iCookies ) 
        {
        iCookies->ResetAndDestroy();
        delete iCookies;
        iCookies = 0;
        }
        
    DLTRACE(("Deleting messages"));
    if( iMessages )
        {
        iMessages->ResetAndDestroy();
        delete iMessages;
        iMessages = 0;
        }
        
    DLTRACE(("Deleting details"));
    if( iDetails )
        {
        iDetails->ResetAndDestroy();
        delete iDetails;
        iDetails = 0;
        }
    
    DLTRACE(("Deleting expired cached data"));
    delete iExpiredCachedData;
    iExpiredCachedData = 0;
    
    delete iServerDetails;
    iServerDetails = 0;
    
    delete iNamespace;
    iNamespace = 0;
    }

TInt CNcdPreminetProtocolInformationImpl::CookieCount() const
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

const MNcdConfigurationProtocolCookie&
    CNcdPreminetProtocolInformationImpl::CookieL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iCookies->Count() );
    if ( aIndex < 0 || aIndex >= iCookies->Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iCookies->At( aIndex ));
    }

TInt CNcdPreminetProtocolInformationImpl::MessageCount() const
    {
    if( iMessages )
        {
        return iMessages->Count();
        }
    else
        {
        return 0;
        }
    }

const MNcdConfigurationProtocolQuery&
    CNcdPreminetProtocolInformationImpl::MessageL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iMessages->Count() );
    if ( aIndex < 0 || aIndex >= iMessages->Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iMessages->At( aIndex ));
    }

TInt CNcdPreminetProtocolInformationImpl::DetailCount() const
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

const MNcdConfigurationProtocolDetail&
    CNcdPreminetProtocolInformationImpl::DetailL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDetails->Count() );
    if ( aIndex < 0 || aIndex >= iDetails->Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iDetails->At( aIndex ));
    }

TInt CNcdPreminetProtocolInformationImpl::ResendAfter() const
    {
    return iResendAfter;
    }

const MNcdPreminetProtocolExpiredCachedData* 
    CNcdPreminetProtocolInformationImpl::ExpiredCachedData() const 
    {
    return iExpiredCachedData;
    }

const MNcdConfigurationProtocolServerDetails* 
    CNcdPreminetProtocolInformationImpl::ServerDetails() const
    {
    return iServerDetails; 
    }


const TDesC& CNcdPreminetProtocolInformationImpl::Namespace() const 
    {
    DASSERT((iNamespace));
    return *iNamespace;
    }

