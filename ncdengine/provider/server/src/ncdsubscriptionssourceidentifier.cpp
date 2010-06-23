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


#include "ncdsubscriptionssourceidentifier.h"


// ======== MEMBER FUNCTIONS ========

CNcdSubscriptionsSourceIdentifier::CNcdSubscriptionsSourceIdentifier(
    TBool aRequireCapabilityCheck )
    : iRequireCapabilityCheck( aRequireCapabilityCheck )
    {
    }

void CNcdSubscriptionsSourceIdentifier::ConstructL(
    const TDesC& aUri,
    const TDesC& aNamespace )
    {
    iUri = aUri.AllocL();
    iNamespace = aNamespace.AllocL();
    }

CNcdSubscriptionsSourceIdentifier* CNcdSubscriptionsSourceIdentifier::NewL(
    const TDesC& aUri,
    const TDesC& aNamespace,
    TBool aRequireCapabilityCheck )
    {
    CNcdSubscriptionsSourceIdentifier* self =
        CNcdSubscriptionsSourceIdentifier::NewLC( aUri,
                                                  aNamespace,
                                                  aRequireCapabilityCheck );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSubscriptionsSourceIdentifier* CNcdSubscriptionsSourceIdentifier::NewLC(
    const TDesC& aUri,
    const TDesC& aNamespace,
    TBool aRequireCapabilityCheck )
    {
    CNcdSubscriptionsSourceIdentifier* self =
        new( ELeave ) CNcdSubscriptionsSourceIdentifier(
            aRequireCapabilityCheck );
    CleanupStack::PushL( self );
    self->ConstructL( aUri, aNamespace );
    return self;
    }

CNcdSubscriptionsSourceIdentifier::~CNcdSubscriptionsSourceIdentifier()
    {
    delete iUri;
    delete iNamespace;
    }

TBool CNcdSubscriptionsSourceIdentifier::CompareIdentifiers( 
    const CNcdSubscriptionsSourceIdentifier& aFirst, 
    const CNcdSubscriptionsSourceIdentifier& aSecond )
    {
    if ( aFirst.Uri() == aSecond.Uri() &&
         aFirst.Namespace() == aSecond.Namespace() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

const TDesC& CNcdSubscriptionsSourceIdentifier::Uri() const
    {
    return *iUri;
    }

const TDesC& CNcdSubscriptionsSourceIdentifier::Namespace() const
    {
    return *iNamespace;
    }

TBool CNcdSubscriptionsSourceIdentifier::RequiresCapabilityCheck() const
    {
    return iRequireCapabilityCheck;
    }
