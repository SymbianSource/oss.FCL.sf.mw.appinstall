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

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ErrorCategory()
// ---------------------------------------------------------------------------
//
inline Usif::TErrorCategory CSifUiErrorInfo::ErrorCategory() const
    {
    return iErrorCategory;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ErrorCode()
// ---------------------------------------------------------------------------
//
inline TInt CSifUiErrorInfo::ErrorCode() const
    {
    return iErrorCode;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ExtendedErrorCode()
// ---------------------------------------------------------------------------
//
inline TInt CSifUiErrorInfo::ExtendedErrorCode() const
    {
    return iExtendedErrorCode;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ErrorMessage()
// ---------------------------------------------------------------------------
//
inline const TDesC& CSifUiErrorInfo::ErrorMessage() const
    {
    if( iErrorMessage )
        {
        return *iErrorMessage;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSifUiErrorInfo::ErrorMessageDetails()
// ---------------------------------------------------------------------------
//
inline const TDesC& CSifUiErrorInfo::ErrorMessageDetails() const
    {
    if( iErrorMessageDetails )
        {
        return *iErrorMessageDetails;
        }
    return KNullDesC;
    }

