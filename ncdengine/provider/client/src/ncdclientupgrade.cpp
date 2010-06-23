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


#include "ncdclientupgrade.h"





// ======== MEMBER FUNCTIONS ========


CNcdClientUpgrade::CNcdClientUpgrade()
    {
    }

void CNcdClientUpgrade::ConstructL()
    {
    }


CNcdClientUpgrade* CNcdClientUpgrade::NewL()
    {
    CNcdClientUpgrade* self = 
        CNcdClientUpgrade::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdClientUpgrade* CNcdClientUpgrade::NewLC()
    {
    CNcdClientUpgrade* self =
        new( ELeave ) CNcdClientUpgrade();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdClientUpgrade::~CNcdClientUpgrade()
    {
    delete iDependencyId;
    }

void CNcdClientUpgrade::SetDependencyId( HBufC* aDependencyId )
    {
    delete iDependencyId;
    iDependencyId = aDependencyId;
    }
    
void CNcdClientUpgrade::SetValidityDelta( TInt aValidityDelta )
    {
    iValidityDelta = aValidityDelta;
    }
    
void CNcdClientUpgrade::SetAmountOfCredits( TReal32 aAmountOfCredits )
    {
    iAmountOfCredits = aAmountOfCredits;
    }
    
void CNcdClientUpgrade::SetNumberOfDownloads( TInt aNumberOfDownloads )
    {
    iNumberOfDownloads = aNumberOfDownloads;
    }




const TDesC& CNcdClientUpgrade::DependencyId() const
    {
    if ( iDependencyId == NULL )
        {
        return KNullDesC;
        }
    return *iDependencyId;
    }
    
TInt CNcdClientUpgrade::ValidityDelta() const
    {
    return iValidityDelta;
    }
    
TReal32 CNcdClientUpgrade::AmountOfCredits() const
    {
    return iAmountOfCredits;
    }
    
TInt CNcdClientUpgrade::NumberOfDownloads() const
    {
    return iNumberOfDownloads;
    }
