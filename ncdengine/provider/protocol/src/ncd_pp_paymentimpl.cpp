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
* Description:   CNcdPreminetProtocolPaymentImpl implementation
*
*/


#include "ncd_pp_paymentimpl.h"
#include "ncd_pp_purchase.h"
#include "ncdprotocolutils.h"
#include "ncdpaymentmethod.h"
#include "catalogsutils.h"
#include "ncdstring.h"
#include "ncd_pp_smsdetailsimpl.h"
#include "catalogsdebug.h"

CNcdPreminetProtocolPaymentImpl* 
CNcdPreminetProtocolPaymentImpl::NewL() 
    {
    CNcdPreminetProtocolPaymentImpl* self =
        new (ELeave) CNcdPreminetProtocolPaymentImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolPaymentImpl* 
CNcdPreminetProtocolPaymentImpl::NewLC() 
    {
    CNcdPreminetProtocolPaymentImpl* self =
        new (ELeave) CNcdPreminetProtocolPaymentImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolPaymentImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iQueryId );
    iName = CNcdString::NewL();
    }

CNcdPreminetProtocolPaymentImpl::~CNcdPreminetProtocolPaymentImpl()
    {
    delete iQueryId;
    delete iName;
    iSmsDetails.ResetAndDestroy();
    }

void CNcdPreminetProtocolPaymentImpl::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    aStream.WriteInt32L( iMethod );

    // For name and queryid:
    // if not given in protocol, still constructed in ConstructL.
    // So they exist in any case and can be externalized.
    ExternalizeDesL( *iQueryId, aStream );
    iName->ExternalizeL( aStream );
    
    // And then there can be sms details or not
    const TInt KSmsDetailsCount( iSmsDetails.Count() );
    aStream.WriteInt32L( KSmsDetailsCount );
    
    TInt smsIndexer( 0 );
    while ( smsIndexer < KSmsDetailsCount )
        {
        MNcdPreminetProtocolSmsDetails*
            currentSmsDetail = iSmsDetails[ smsIndexer ];
        
        CNcdPreminetProtocolSmsDetailsImpl* currentCastedSmsDetail =
            static_cast<CNcdPreminetProtocolSmsDetailsImpl*>(
                currentSmsDetail );
        currentCastedSmsDetail->ExternalizeL( aStream );        
        ++smsIndexer;
        }
    }

void CNcdPreminetProtocolPaymentImpl::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iMethod = static_cast<MNcdPaymentMethod::TNcdPaymentMethodType>(
                  aStream.ReadInt32L() );
    
    InternalizeDesL( iQueryId, aStream );
    iName->InternalizeL( aStream );
    
    // And then there can be sms details or not
    iSmsDetails.ResetAndDestroy();
    const TInt KSmsDetailsCount( aStream.ReadInt32L() );
    TInt smsIndexer( 0 );
    while ( smsIndexer < KSmsDetailsCount )
        {
        CNcdPreminetProtocolSmsDetailsImpl* smsDetails = 
            CNcdPreminetProtocolSmsDetailsImpl::NewLC();
        iSmsDetails.AppendL( smsDetails );
        CleanupStack::Pop( smsDetails );
        smsDetails->InternalizeL( aStream );
        ++smsIndexer;
        }     
    }

MNcdPaymentMethod::TNcdPaymentMethodType
CNcdPreminetProtocolPaymentImpl::Method() const
    {
    return iMethod;
    }

const TDesC& 
CNcdPreminetProtocolPaymentImpl::QueryId() const
    {
    return *iQueryId;
    }

const CNcdString& 
CNcdPreminetProtocolPaymentImpl::Name() const
    {
    return *iName;
    }

const MNcdPreminetProtocolSmsDetails&
CNcdPreminetProtocolPaymentImpl::SmsDetailsL(TInt aIndex) const
    {
    DASSERT( aIndex >= 0 && aIndex < iSmsDetails.Count() );
    if ( aIndex < 0 || aIndex >= iSmsDetails.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iSmsDetails[aIndex];
    }

TInt CNcdPreminetProtocolPaymentImpl::SmsDetailsCount() const 
    {
    return iSmsDetails.Count();
    }
    
CNcdPreminetProtocolPaymentImpl* CNcdPreminetProtocolPaymentImpl::CloneL() const 
    {
    DLTRACEIN((""));
    CNcdPreminetProtocolPaymentImpl* clone =
        CNcdPreminetProtocolPaymentImpl::NewLC();
    clone->iMethod = iMethod;
    AssignDesL( clone->iQueryId, *iQueryId );
    clone->iName->SetKeyL( iName->Key() );
    clone->iName->SetDataL( iName->Data() );
    
    for ( TInt i = 0; i < iSmsDetails.Count(); i++ ) 
        {
        MNcdPreminetProtocolSmsDetails* details = iSmsDetails[i]->CloneL();
        CleanupStack::PushL( details );
        clone->iSmsDetails.AppendL( details );
        CleanupStack::Pop( details );
        }
    
    CleanupStack::Pop( clone );
    return clone;
    }
