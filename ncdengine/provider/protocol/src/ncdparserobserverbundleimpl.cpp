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


#include "ncdparserobserverbundleimpl.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoldefaultobserver.h"

CNcdParserObserverBundleImpl* CNcdParserObserverBundleImpl::NewL(
    MNcdProtocolDefaultObserver* aDefaultObserver )
    {
    CNcdParserObserverBundleImpl* self = 
        new(ELeave) CNcdParserObserverBundleImpl(
        aDefaultObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CNcdParserObserverBundleImpl::ConstructL()
    {
    NcdProtocolUtils::AssignDesL( iSessionOrigin, KNullDesC );
    }

/**
 * @ Enable default observing when the observer implements 
 * the interfaces
 */
CNcdParserObserverBundleImpl::CNcdParserObserverBundleImpl( MNcdProtocolDefaultObserver*
                                                            aDefaultObserver )
    :
    iSubscriptionObserver( aDefaultObserver ),
    iInformationObserver( aDefaultObserver ),
    iDataBlocksObserver( aDefaultObserver ),
    iConfigurationProtocolObserver( aDefaultObserver ),
    iErrorObserver( aDefaultObserver ),
    iQueryObserver( aDefaultObserver )
    {
    }

CNcdParserObserverBundleImpl::~CNcdParserObserverBundleImpl()
    {
    delete iSessionOrigin;
    }


void CNcdParserObserverBundleImpl::SetParserObserver( 
    MNcdParserObserver* aObserver )
    {
    iObserver = aObserver;
    }
MNcdParserObserver* 
CNcdParserObserverBundleImpl::ParserObserver() const
    {
    return iObserver;
    }

void CNcdParserObserverBundleImpl::SetEntityObserver( 
    MNcdParserEntityObserver* aObserver )
    {
    iEntityObserver = aObserver;
    }
MNcdParserEntityObserver* 
CNcdParserObserverBundleImpl::EntityObserver() const
    {
    return iEntityObserver;
    }

void CNcdParserObserverBundleImpl::SetPurchaseObserver( 
    MNcdParserPurchaseObserver* aObserver )
    {
    iPurchaseObserver = aObserver;
    }
MNcdParserPurchaseObserver* 
CNcdParserObserverBundleImpl::PurchaseObserver() const
    {
    return iPurchaseObserver;
    }

void CNcdParserObserverBundleImpl::SetSubscriptionObserver( 
    MNcdParserSubscriptionObserver* aObserver )
    {
    iSubscriptionObserver = aObserver;
    }
MNcdParserSubscriptionObserver* 
CNcdParserObserverBundleImpl::SubscriptionObserver() const
    {
    return iSubscriptionObserver;
    }

void CNcdParserObserverBundleImpl::SetSessionObserver( 
    MNcdParserSessionObserver* aObserver )
    {
    iSessionObserver = aObserver;
    }
MNcdParserSessionObserver* 
CNcdParserObserverBundleImpl::SessionObserver() const
    {
    return iSessionObserver;
    }

void CNcdParserObserverBundleImpl::SetConfigurationProtocolObserver( MNcdParserConfigurationProtocolObserver* aObserver )
{
    iConfigurationProtocolObserver = aObserver;
}

MNcdParserConfigurationProtocolObserver* CNcdParserObserverBundleImpl::ConfigurationProtocolObserver() const
{
    return iConfigurationProtocolObserver;
}

void CNcdParserObserverBundleImpl::SetSessionOriginL( 
    const TDesC& aSessionOrigin )
    {
    NcdProtocolUtils::AssignDesL( iSessionOrigin, aSessionOrigin );
    }

const TDesC& CNcdParserObserverBundleImpl::SessionOrigin() const
    {
    return *iSessionOrigin;
    }

void CNcdParserObserverBundleImpl::SetInformationObserver( MNcdParserInformationObserver* aObserver )
{
    iInformationObserver = aObserver;
}

MNcdParserInformationObserver* CNcdParserObserverBundleImpl::InformationObserver() const
{
    return iInformationObserver;
}

void CNcdParserObserverBundleImpl::SetDataBlocksObserver( MNcdParserDataBlocksObserver* aObserver )
{
    iDataBlocksObserver = aObserver;
}

MNcdParserDataBlocksObserver* CNcdParserObserverBundleImpl::DataBlocksObserver() const
{
    return iDataBlocksObserver;
}

void CNcdParserObserverBundleImpl::SetErrorObserver( MNcdParserErrorObserver* aObserver )
{
    iErrorObserver = aObserver;
}

MNcdParserErrorObserver* CNcdParserObserverBundleImpl::ErrorObserver() const
    {
    return iErrorObserver;
    }

void CNcdParserObserverBundleImpl::SetQueryObserver( MNcdParserQueryObserver* aObserver )
{
    iQueryObserver = aObserver;
}

MNcdParserQueryObserver* CNcdParserObserverBundleImpl::QueryObserver() const
    {
    return iQueryObserver;
    }

