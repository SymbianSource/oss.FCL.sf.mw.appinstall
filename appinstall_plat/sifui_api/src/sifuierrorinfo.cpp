/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSifUiErrorInfo for installation error details
*
*/

#include "sifuierrorinfo.h"         // CSifUiErrorInfo


// ---------------------------------------------------------------------------
// CSifUiErrorInfo::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiErrorInfo* CSifUiErrorInfo::NewLC( Usif::TErrorCategory aErrorCategory,
        TInt aErrorCode, TInt aExtendedErrorCode, const TDesC& aErrorMessage,
        const TDesC& aErrorMessageDetails )
    {
    CSifUiErrorInfo* self = new (ELeave) CSifUiErrorInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aErrorCategory, aErrorCode, aExtendedErrorCode,
            aErrorMessage, aErrorMessageDetails );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiErrorInfo* CSifUiErrorInfo::NewL( Usif::TErrorCategory aErrorCategory,
        TInt aErrorCode, TInt aExtendedErrorCode, const TDesC& aErrorMessage,
        const TDesC& aErrorMessageDetails )
    {
    CSifUiErrorInfo* self = CSifUiErrorInfo::NewLC( aErrorCategory, aErrorCode,
            aExtendedErrorCode, aErrorMessage, aErrorMessageDetails );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::~CSifUiErrorInfo()
// ---------------------------------------------------------------------------
//
CSifUiErrorInfo::~CSifUiErrorInfo()
    {
    delete iErrorMessage;
    delete iErrorMessageDetails;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::CSifUiErrorInfo()
// ---------------------------------------------------------------------------
//
CSifUiErrorInfo::CSifUiErrorInfo()
    {
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUiErrorInfo::ConstructL( Usif::TErrorCategory aErrorCategory,
        TInt aErrorCode, TInt aExtendedErrorCode, const TDesC& aErrorMessage,
        const TDesC& aErrorMessageDetails )
    {
    iErrorCategory = aErrorCategory;
    iErrorCode = aErrorCode;
    iExtendedErrorCode = aExtendedErrorCode;
    iErrorMessage = aErrorMessage.AllocL();
    iErrorMessageDetails = aErrorMessageDetails.AllocL();
    }

