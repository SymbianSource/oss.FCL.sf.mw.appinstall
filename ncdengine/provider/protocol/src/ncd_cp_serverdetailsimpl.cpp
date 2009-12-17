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


#include "ncd_cp_serverdetailsimpl.h"
#include "ncd_cp_detail.h"
#include "ncdprotocolutils.h"

void CNcdConfigurationProtocolServerDetailsImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iVersion );
    }

CNcdConfigurationProtocolServerDetailsImpl::~CNcdConfigurationProtocolServerDetailsImpl()
    {
    delete iVersion;
    if( iCapabilities )
        {
        iCapabilities->ResetAndDestroy();
        delete iCapabilities;
        iCapabilities = 0;
        }
    iDetails.ResetAndDestroy();
    }

const TDesC& CNcdConfigurationProtocolServerDetailsImpl::Version() const
    {
    return *iVersion;
    }

TInt CNcdConfigurationProtocolServerDetailsImpl::DetailCount() const
    {
    return iDetails.Count();
    }

const MNcdConfigurationProtocolDetail&
    CNcdConfigurationProtocolServerDetailsImpl::DetailL( TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= iDetails.Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iDetails[aIndex];
    }

TInt CNcdConfigurationProtocolServerDetailsImpl::CapabilityCount() const
    {
    if( iCapabilities )
        {
        return iCapabilities->Count();
        }
    else
        {
        return 0;
        }
    }

const TDesC& CNcdConfigurationProtocolServerDetailsImpl::CapabilityL( TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= iCapabilities->Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iCapabilities->At( aIndex );
    }
