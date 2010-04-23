/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateResult
*
*/


#include <iaupdateresult.h>


EXPORT_C CIAUpdateResult* CIAUpdateResult::NewL()
    {
    CIAUpdateResult* self =
        CIAUpdateResult::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CIAUpdateResult* CIAUpdateResult::NewLC()
    {
    CIAUpdateResult* self =
        new( ELeave ) CIAUpdateResult();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateResult::CIAUpdateResult()
: CBase()
    {
    // Nothing to do here.    
    }

void CIAUpdateResult::ConstructL()
    {
    // Nothing to do here.
    }


EXPORT_C CIAUpdateResult::~CIAUpdateResult()
    {
    // Nothing to do here.    
    }


EXPORT_C TInt CIAUpdateResult::SuccessCount() const
    {
    return iSuccessCount;
    }

EXPORT_C void CIAUpdateResult::SetSuccessCount( TInt aSuccessCount )
    {
    iSuccessCount = aSuccessCount;
    }


EXPORT_C TInt CIAUpdateResult::FailCount() const
    {
    return iFailCount;
    }

EXPORT_C void CIAUpdateResult::SetFailCount( TInt aFailCount )
    {
    iFailCount = aFailCount;
    }


EXPORT_C TInt CIAUpdateResult::CancelCount() const
    {
    return iCancelCount;
    }

EXPORT_C void CIAUpdateResult::SetCancelCount( TInt aCancelCount )
    {
    iCancelCount = aCancelCount;
    }

