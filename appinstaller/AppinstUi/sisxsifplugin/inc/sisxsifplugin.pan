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
* Description:  SISX SIF plugin panic codes
*
*/


#ifndef SISXSIFPLUGIN_PAN
#define SISXSIFPLUGIN_PAN

#include <e32std.h>     // User

/**
 * Native SISX SIF (Software Install Framework) plugin panic codes
 */
enum TSisxSifPanicCode
    {
    ESisxSifInternalError = 1,
    ESisxSifNotInitialized,
    ESisxSifNullPointer,
    ESisxSifUnknownErrorCode
    };

inline void Panic( TSisxSifPanicCode aReason )
    {
    _LIT( category, "SisxSif" );
    User::Panic( category, aReason );
    }

#endif // SISXSIFPLUGIN_PAN
