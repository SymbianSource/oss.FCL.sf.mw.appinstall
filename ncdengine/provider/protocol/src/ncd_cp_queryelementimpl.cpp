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


#include "ncd_cp_queryelementimpl.h"
#include "ncd_cp_queryoptionimpl.h"
#include "ncd_cp_detailimpl.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"
#include "ncdstring.h"

CNcdConfigurationProtocolQueryElementImpl::CNcdConfigurationProtocolQueryElementImpl()
: iOptional( EFalse )
{
}


void CNcdConfigurationProtocolQueryElementImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    iLabel = CNcdString::NewL();
    iMessage = CNcdString::NewL();
    iDescription = CNcdString::NewL();
    }

CNcdConfigurationProtocolQueryElementImpl::~CNcdConfigurationProtocolQueryElementImpl()
    {
    delete iId;
    delete iLabel;
    delete iMessage;
    delete iDescription;
    iOptions.ResetAndDestroy();
    delete iDetails;
    }

const TDesC& CNcdConfigurationProtocolQueryElementImpl::Id() const
    {
    return *iId;
    }
    
MNcdQueryItem::TSemantics
    CNcdConfigurationProtocolQueryElementImpl::Semantics() const
    {
    return iSemantics;
    }

MNcdConfigurationProtocolQueryElement::TNcdProtocolQueryElementType
    CNcdConfigurationProtocolQueryElementImpl::Type() const
    {
    return iType;
    }

TBool CNcdConfigurationProtocolQueryElementImpl::Optional() const
    {
    return iOptional;
    }

const CNcdString& CNcdConfigurationProtocolQueryElementImpl::Label() const
    {
    return *iLabel;
    }

const CNcdString& CNcdConfigurationProtocolQueryElementImpl::Message() const
    {
    return *iMessage;
    }

const CNcdString& CNcdConfigurationProtocolQueryElementImpl::Description() const
    {
    return *iDescription;
    }

TInt CNcdConfigurationProtocolQueryElementImpl::OptionCount() const
    {
    return iOptions.Count();
    }

const MNcdConfigurationProtocolQueryOption&
CNcdConfigurationProtocolQueryElementImpl::Option( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iOptions.Count() );
    return *iOptions[aIndex];
    }

const MNcdConfigurationProtocolDetail* 
CNcdConfigurationProtocolQueryElementImpl::Detail() const
    {
    return iDetails;
    }

