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
* Description:   MNcdProtocolElementEntity declaration
 *
*/


#include "s32strm.h"

#include "ncdqueryoptionimpl.h"
#include "ncd_cp_queryoption.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "ncdstring.h"

// ======== MEMBER FUNCTIONS ========

CNcdQueryOption* CNcdQueryOption::NewL( RReadStream& aReadStream )
    {
    CNcdQueryOption* self = CNcdQueryOption::NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryOption* CNcdQueryOption::NewLC( RReadStream& aReadStream )
    {
    CNcdQueryOption* self = new ( ELeave ) CNcdQueryOption();
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdQueryOption* CNcdQueryOption::NewL(
    const MNcdConfigurationProtocolQueryOption& aOption)
    {
    CNcdQueryOption* self = CNcdQueryOption::NewLC( aOption );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryOption* CNcdQueryOption::NewLC(
    const MNcdConfigurationProtocolQueryOption& aOption)
    {
    CNcdQueryOption* self = new ( ELeave ) CNcdQueryOption();
    CleanupStack::PushL( self );
    self->InternalizeL( aOption );
    return self;
    }

CNcdQueryOption* CNcdQueryOption::NewL( const CNcdString& aPaymentMethodName,
                                        const TDesC8& aPaymentMethodType )
{
    CNcdQueryOption* self = 
        CNcdQueryOption::NewLC( aPaymentMethodName, aPaymentMethodType );
    CleanupStack::Pop( self );
    return self;
}

CNcdQueryOption* CNcdQueryOption::NewLC( const CNcdString& aPaymentMethodName,
                                         const TDesC8& aPaymentMethodType )
{
    CNcdQueryOption* self = new ( ELeave ) CNcdQueryOption();
    CleanupStack::PushL( self );
    self->ConstructL( aPaymentMethodName, aPaymentMethodType );
    return self;
}
    
void CNcdQueryOption::InternalizeL( RReadStream& aReadStream )
    {
    delete iValue;
    iValue = NULL;
    iValue = HBufC::NewL( aReadStream, KMaxTInt );
    delete iName;
    iName = NULL;
    iName = CNcdString::NewL( aReadStream );
    delete iData;
    iData = NULL;
    iData = HBufC8::NewL( aReadStream, KMaxTInt );
    }
    
void CNcdQueryOption::InternalizeL(
    const MNcdConfigurationProtocolQueryOption& aOption )
    {
    delete iValue;
    iValue = NULL;
    iValue = aOption.Value().AllocL();
    delete iName;
    iName = NULL;
    iName = CNcdString::NewL( aOption.Name() );
    AssignDesL( iData, KNullDesC8 );
    }
    
void CNcdQueryOption::ExternalizeL( RWriteStream& aWriteStream )
    {
    aWriteStream << *iValue;
    iName->ExternalizeL( aWriteStream );
    aWriteStream << *iData;
    }


const TDesC& CNcdQueryOption::Value() const
    {
    return *iValue;
    }

const CNcdString& CNcdQueryOption::Name() const
    {
    return *iName;
    }

const TDesC8& CNcdQueryOption::Data() const
    {
    return *iData;
    }

void CNcdQueryOption::SetDataL( const TDesC8& aData )
    {
    delete iData;
    iData = NULL;
    iData = aData.AllocL();
    }

CNcdQueryOption::~CNcdQueryOption()
    {
    delete iValue;
    delete iName;    
    delete iData;
    }

void CNcdQueryOption::ConstructL( const CNcdString& aPaymentMethodName,
                                  const TDesC8& aPaymentMethodType )
    {
	AssignDesL( iValue, KNullDesC );
	iName = CNcdString::NewL( aPaymentMethodName );
    AssignDesL( iData, aPaymentMethodType );
    }
