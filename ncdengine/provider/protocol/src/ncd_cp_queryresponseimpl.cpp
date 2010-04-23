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


#include "ncd_cp_queryresponseimpl.h"
#include "ncdprotocolutils.h"
#include "ncdstring.h"


CNcdConfigurationProtocolQueryResponseValueImpl* 
CNcdConfigurationProtocolQueryResponseValueImpl::NewL()
    {
    CNcdConfigurationProtocolQueryResponseValueImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryResponseValueImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolQueryResponseValueImpl* 
CNcdConfigurationProtocolQueryResponseValueImpl::NewLC()
    {
    CNcdConfigurationProtocolQueryResponseValueImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryResponseValueImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdConfigurationProtocolQueryResponseValueImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    }

CNcdConfigurationProtocolQueryResponseValueImpl::~CNcdConfigurationProtocolQueryResponseValueImpl()
    {
    delete iId;
    iValues.ResetAndDestroy();
    }

CNcdConfigurationProtocolQueryResponseValueImpl::CNcdConfigurationProtocolQueryResponseValueImpl()
: CBase()
    {
    }

const TDesC& 
CNcdConfigurationProtocolQueryResponseValueImpl::Id() const
    {
    return *iId;
    }

MNcdQuery::TSemantics CNcdConfigurationProtocolQueryResponseValueImpl::Semantics() const
    {
    return iSemantics;
    }

TInt CNcdConfigurationProtocolQueryResponseValueImpl::ValueCount() const
    {
    return iValues.Count();
    }

TDesC& CNcdConfigurationProtocolQueryResponseValueImpl::ValueL(TInt aIndex) const
    {
    if ( aIndex < 0 || aIndex >= iValues.Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iValues[aIndex];
    }


// -----------------------------------------


CNcdConfigurationProtocolQueryResponseImpl* 
CNcdConfigurationProtocolQueryResponseImpl::NewL() 
    {
    CNcdConfigurationProtocolQueryResponseImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryResponseImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdConfigurationProtocolQueryResponseImpl* 
CNcdConfigurationProtocolQueryResponseImpl::NewLC() 
    {
    CNcdConfigurationProtocolQueryResponseImpl* self =
        new (ELeave) CNcdConfigurationProtocolQueryResponseImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdConfigurationProtocolQueryResponseImpl::CNcdConfigurationProtocolQueryResponseImpl()
: iCancel( EFalse )
{
}

void CNcdConfigurationProtocolQueryResponseImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    }

CNcdConfigurationProtocolQueryResponseImpl::~CNcdConfigurationProtocolQueryResponseImpl()
    {
    delete iId;
    iResponses.ResetAndDestroy();
    }

const TDesC& CNcdConfigurationProtocolQueryResponseImpl::Id() const
    {
    return *iId;
    }

MNcdQuery::TSemantics CNcdConfigurationProtocolQueryResponseImpl::Semantics() const
    {
    return iSemantics;
    }

TBool CNcdConfigurationProtocolQueryResponseImpl::Cancel() const
    {
    return iCancel;
    }

TInt CNcdConfigurationProtocolQueryResponseImpl::ResponseCount() const
    {
    return iResponses.Count();
    }

const MNcdConfigurationProtocolQueryResponseValue&
    CNcdConfigurationProtocolQueryResponseImpl::ResponseL( TInt aIndex ) const
    {
    // Panic is really better here.
    DASSERT( aIndex >= 0 && aIndex < iResponses.Count() );
    if ( aIndex < 0 || aIndex >= iResponses.Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iResponses[aIndex];
    }
