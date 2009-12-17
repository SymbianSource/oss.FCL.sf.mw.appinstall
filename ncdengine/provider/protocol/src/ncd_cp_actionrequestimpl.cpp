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
#include "ncdprotocoltypes.h"

#include "ncd_cp_actionrequestimpl.h"
#include "ncd_cp_query.h"
#include "ncd_cp_detail.h"

#include "catalogsdebug.h"

void CNcdConfigurationProtocolUpdateDetailsImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iVersion );
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    }

CNcdConfigurationProtocolUpdateDetailsImpl::~CNcdConfigurationProtocolUpdateDetailsImpl()
    {
    delete iId;
    delete iUri;
    delete iVersion;
    }

const TDesC& CNcdConfigurationProtocolUpdateDetailsImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdConfigurationProtocolUpdateDetailsImpl::Version() const
    {
    return *iVersion;
    }

const TDesC& CNcdConfigurationProtocolUpdateDetailsImpl::Uri() const
    {
    return *iUri;
    }

void CNcdConfigurationProtocolActionRequestImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iTarget );
    }

CNcdConfigurationProtocolActionRequestImpl::~CNcdConfigurationProtocolActionRequestImpl()
    {
    delete iTarget;
    iMessages.ResetAndDestroy();
    iDetails.ResetAndDestroy();
    delete iUpdateDetails;
    }

TBool CNcdConfigurationProtocolActionRequestImpl::Force() const
    {
    return iForce;
    }

MNcdConfigurationProtocolActionRequest::TNcdProtocolActionRequestType 
    CNcdConfigurationProtocolActionRequestImpl::Type() const
    {
    return iType;
    }

const TDesC& CNcdConfigurationProtocolActionRequestImpl::Target() const
    {
    return *iTarget;
    }

TInt CNcdConfigurationProtocolActionRequestImpl::MessageCount() const
    {
    return iMessages.Count();
    }

const MNcdConfigurationProtocolQuery& CNcdConfigurationProtocolActionRequestImpl::MessageL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iMessages.Count() );
    if ( aIndex < 0 || aIndex >= iMessages.Count() )
        {
        User::Leave( KErrArgument );
        }

    return *iMessages[aIndex];
    }

TInt CNcdConfigurationProtocolActionRequestImpl::DetailCount() const
    {
    return iDetails.Count();
    }

const MNcdConfigurationProtocolDetail& CNcdConfigurationProtocolActionRequestImpl::DetailL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDetails.Count() );
    if ( aIndex < 0 || aIndex >= iDetails.Count() )
        {
        User::Leave( KErrArgument );
        }

    return *iDetails[aIndex];
    }

const MNcdConfigurationProtocolUpdateDetails* CNcdConfigurationProtocolActionRequestImpl::UpdateDetails() const
{
    return iUpdateDetails;
}

