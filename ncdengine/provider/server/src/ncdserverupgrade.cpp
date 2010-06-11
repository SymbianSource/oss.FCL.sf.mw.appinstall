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


#include "ncdserverupgrade.h"





// ======== MEMBER FUNCTIONS ========


CNcdServerUpgrade::CNcdServerUpgrade()
    {
    }

void CNcdServerUpgrade::ConstructL()
    {
    }


CNcdServerUpgrade* CNcdServerUpgrade::NewL()
    {
    CNcdServerUpgrade* self = 
        CNcdServerUpgrade::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdServerUpgrade* CNcdServerUpgrade::NewLC()
    {
    CNcdServerUpgrade* self =
        new( ELeave ) CNcdServerUpgrade();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdServerUpgrade::~CNcdServerUpgrade()
    {
    delete iDependencyId;
    }
    
void CNcdServerUpgrade::SetDependencyId( HBufC* aDependencyId )
    {
    delete iDependencyId;
    iDependencyId = aDependencyId;
    }
    
void CNcdServerUpgrade::SetValidityDelta( TInt aValidityDelta )
    {
    iValidityDelta = aValidityDelta;
    }
    
void CNcdServerUpgrade::SetAmountOfCredits( TReal32 aAmountOfCredits )
    {
    iAmountOfCredits = aAmountOfCredits;
    }
    
void CNcdServerUpgrade::SetNumberOfDownloads( TInt aNumberOfDownloads )
    {
    iNumberOfDownloads = aNumberOfDownloads;
    }




const TDesC& CNcdServerUpgrade::DependencyId() const
    {
    if ( iDependencyId == NULL )
        {
        return KNullDesC;
        }
    return *iDependencyId;
    }
    
TInt CNcdServerUpgrade::ValidityDelta() const
    {
    return iValidityDelta;
    }
    
TReal32 CNcdServerUpgrade::AmountOfCredits() const
    {
    return iAmountOfCredits;
    }
    
TInt CNcdServerUpgrade::NumberOfDownloads() const
    {
    return iNumberOfDownloads;
    }
