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
// in sifuidevicedialogdefinitions.h and sifuiinstallindicatorparams.h files.
// Symbian code needs descriptor constants, as Qt code uses QString constants.

_LIT( KSifUiDeviceDialog, "com.nokia.sifui/1.0" );

enum TSifUiDeviceDialogType
    {
    ESifUiConfirmationQuery = 1,
    ESifUiProgressNote = 2,
    ESifUiCompleteNote = 3,
    ESifUiErrorNote = 4
    };

enum TSifUiDeviceDialogReturnValue
    {
    ESifUiContinue = 0,
    ESifUiCancel = 1,
    ESifUiIndicator = 3
    };

// Variant map keys for dialog type and title (common to all dialog types)
_LIT( KSifUiDialogType, "type" );                   // enum TSifUiDeviceDialogType

// Variant map keys for device dialog return values
_LIT( KSifUiQueryReturnValue, "ret" );              // enum TSifUiDeviceDialogReturnValue
_LIT( KSifUiSelectedMemory, "drv" );                // drive letter

// Variant map keys for "confirmation query" dialog parameters
_LIT( KSifUiApplicationName, "app" );               // descriptor
_LIT( KSifUiApplicationIconHandle, "icon" );        // integer, bitmap handle
_LIT( KSifUiApplicationIconMaskHandle, "mask" );    // integer, bitmap handle
_LIT( KSifUiApplicationVersion, "ver" );            // descriptor
_LIT( KSifUiApplicationSize, "size" );              // integer
_LIT( KSifUiApplicationDetails, "details" );        // descriptor array
_LIT( KSifUiMemorySelection, "mem" );               // descriptor (comma separated drive letters)
_LIT( KSifUiCertificates, "cert" );                 // binary (CSifUiCertificateInfo)

// Variant map keys for "progress note" dialog parameters
_LIT( KSifUiProgressNoteText, "txt" );              // string
_LIT( KSifUiProgressNoteFinalValue, "fin" );        // integer
_LIT( KSifUiProgressNoteValue, "val" );             // integer
_LIT( KSifUiProgressNotePhase, "phase" );           // enum CSifUi::TInstallingPhase

// Variant map keys for "progress note" and "installation complete note" buttons
_LIT( KSifUiProgressNoteIsHideButtonHidden, "hidebtn" );        // boolean
_LIT( KSifUiProgressNoteIsCancelButtonHidden, "cancelbtn" );    // boolean
_LIT( KSifUiCompleteNoteIsShowButtonHidden, "showbtn" );        // boolean
_LIT( KSifUiErrorNoteIsDetailsButtonHidden, "errdtlbtn" );      // boolean

// Variant map keys for "error" dialog parameters
_LIT( KSifUiErrorCategory, "errcat" );              // enum Usif::TErrorCategory
_LIT( KSifUiErrorCode, "errcode" );                 // integer
_LIT( KSifUiErrorCodeExtended, "errext" );          // integer
_LIT( KSifUiErrorMessage, "errmsg" );               // string
_LIT( KSifUiErrorDetails, "errdet" );               // string

// Variant map keys for "grant capabilities" dialog parameters
_LIT( KSifUiGrantCapabilities, "grcap" );           // binary (TCapabilitySet)

// Variant map keys for "select language" dialog parameters
_LIT( KSifUiSelectableLanguages, "lang" );          // binary (TPckg< RArray<TLanguage> >)
_LIT( KSifUiSelectedLanguageIndex, "langind" );     // integer

// Variant map keys for "select options" dialog parameters
_LIT( KSifUiSelectableOptions, "opts" );            // descriptor array
_LIT( KSifUiSelectedOptions, "optsind" );           // binary (TPckg< RArray<TInt> >)

#endif  // SIFUIDEFS_H

