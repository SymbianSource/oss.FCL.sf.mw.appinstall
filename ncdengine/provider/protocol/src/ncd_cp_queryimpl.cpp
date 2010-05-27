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


#include "ncd_cp_queryimpl.h"
#include "ncd_cp_queryelement.h"
#include "ncd_cp_queryelementimpl.h"
#include "ncdprotocolutils.h"
#include "ncdstring.h"

CNcdConfigurationProtocolQueryImpl* 
CNcdConfigurationProtocolQueryImpl::NewL() 
    {
    CNcdConfigurationProtocolQueryImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdConfigurationProtocolQueryImpl* 
CNcdConfigurationProtocolQueryImpl::NewLC() 
    {
    CNcdConfigurationProtocolQueryImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdConfigurationProtocolQueryImpl::CNcdConfigurationProtocolQueryImpl()
: iForce( EFalse ), iOptional( EFalse )
{
}

void CNcdConfigurationProtocolQueryImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iResponseUri );
    iBodyText = CNcdString::NewL( KNullDesC, KNullDesC );
    iTitle = CNcdString::NewL( KNullDesC, KNullDesC );
//     NcdProtocolUtils::AssignEmptyDesL( iBodyText );
//     NcdProtocolUtils::AssignEmptyDesL( iTitle );
    }

CNcdConfigurationProtocolQueryImpl::~CNcdConfigurationProtocolQueryImpl()
    {
    delete iId;
    delete iResponseUri;
    delete iBodyText;
    delete iTitle;
    iQueryElements.ResetAndDestroy();
    }

const TDesC& CNcdConfigurationProtocolQueryImpl::Id() const
    {
    return *iId;
    }

MNcdQuery::TSemantics CNcdConfigurationProtocolQueryImpl::Semantics() const
    {
    return iSemantics;
    }

MNcdConfigurationProtocolQuery::TNcdProtocolQueryTrigger
    CNcdConfigurationProtocolQueryImpl::Trigger() const
    {
    return iTrigger;
    }

TBool CNcdConfigurationProtocolQueryImpl::Force() const
    {
    return iForce;
    }

const TDesC& CNcdConfigurationProtocolQueryImpl::ResponseUri() const
    {
    return *iResponseUri;
    }

TBool CNcdConfigurationProtocolQueryImpl::Optional() const
    {
    return iOptional;
    }

const CNcdString& CNcdConfigurationProtocolQueryImpl::Title() const
    {
    return *iTitle;
    }

const CNcdString& CNcdConfigurationProtocolQueryImpl::BodyText() const
    {
    return *iBodyText;
    }

TInt CNcdConfigurationProtocolQueryImpl::QueryElementCount() const
    {
    return iQueryElements.Count();
    }

const MNcdConfigurationProtocolQueryElement&
    CNcdConfigurationProtocolQueryImpl::QueryElementL( TInt aIndex ) const
    {
    // Panic is really better here.
    DASSERT( aIndex >= 0 && aIndex < iQueryElements.Count() );
    if ( aIndex < 0 || aIndex >= iQueryElements.Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iQueryElements[aIndex];
    }
