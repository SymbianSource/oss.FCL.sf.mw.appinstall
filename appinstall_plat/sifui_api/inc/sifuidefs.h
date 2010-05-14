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
* Description:  Variant map key names for CSifUi API and SW install
*               notification dialog plugin.
*/

#ifndef SIFUIDEFS_H
#define SIFUIDEFS_H

// SifUi plugin literal and variant map key names defined as _LIT macros.
// Note that _LIT macros defined in this file must match to the definitions
// in sifuidevicedialogdefinitions.h file. This header is used in Symbian
// code since Symbian code requires 16-bit descriptors, and definitions in
// Qt side (in sifuidevicedialogdefinitions.h) are 8-bit wide.

_LIT( KSifUiDeviceDialog, "com.nokia.sifui/1.0" );

enum TSifUiDeviceDialogType
    {
    ESifUiConfirmationQuery = 1,
    ESifUiProgressNote = 2,
    ESifUiCompleteNote = 3,
    ESifUiErrorNote = 4
    };

// Variant map keys for dialog type and title (common to all dialog types)
_LIT( KSifUiDialogType, "type" );                   // enum TSifUiDeviceDialogType

// Variant map keys for device dialog return values
_LIT( KSifUiQueryAccepted, "accept" );              // boolean
_LIT( KSifUiSelectedMemory, "drv" );                // drive letter

// Variant map keys for "confirmation query" dialog parameters
_LIT( KSifUiApplicationName, "app" );               // descriptor
_LIT( KSifUiApplicationIconHandle, "icon" );        // integer, bitmap handle
_LIT( KSifUiApplicationIconMaskHandle, "mask" );    // integer, bitmap handle
_LIT( KSifUiApplicationVersion, "ver" );            // descriptor
_LIT( KSifUiApplicationSize, "size" );              // integer
_LIT( KSifUiApplicationDetails, "details" );        // descriptor array
_LIT( KSifUiMemorySelection, "mem" );               // descriptor (comma separated drive letters)
_LIT( KSifUiCertificates, "cert" );                 // boolean ???

// Variant map keys for "progress note" dialog parameters
_LIT( KSifUiProgressNoteText, "txt" );              // string
_LIT( KSifUiProgressNoteFinalValue, "fin" );        // integer
_LIT( KSifUiProgressNoteValue, "val" );             // integer

// Variant map keys for "error" dialog parameters
_LIT( KSifUiErrorCode, "err" );                     // integer
_LIT( KSifUiErrorMessage, "msg" );                  // string
_LIT( KSifUiErrorDetails, "det" );                  // string

#endif  // SIFUIDEFS_H

