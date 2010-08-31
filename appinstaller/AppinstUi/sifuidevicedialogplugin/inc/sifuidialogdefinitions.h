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
* Description: Definitions for SIF UI device dialog plugin.
*
*/

#ifndef SIFUIDIALOGDEFINITIONS_H
#define SIFUIDIALOGDEFINITIONS_H

enum SifUiDeviceDialogType
    {
    SifUiUnspecifiedDialog = 0,
    SifUiConfirmationQuery = 1,
    SifUiProgressNote = 2,
    SifUiCompleteNote = 3,
    SifUiErrorNote = 4
    };

enum SifUiInstallingPhase
    {
    SifUiInstalling = 0,
    SifUiDownloading = 1,
    SifUiCheckingCerts = 2
    };

enum SifUiDeviceDialogReturnValue
    {
    SifUiContinue = 0,
    SifUiCancel = 1,
    SifUiIndicator = 3
    };

// Variant map keys for dialog type and title (common to all dialog types)
const QString KSifUiDialogType = "type";                // enum SifUiDeviceDialogType
const QString KSifUiDialogTitle = "title";              // string

// Variant map keys for device dialog return values
const QString KSifUiQueryReturnValue = "ret";           // enum SifUiDeviceDialogReturnValue
const QString KSifUiSelectedMemory = "drv";             // drive letter

// Variant map keys for "confirmation query" dialog parameters
const QString KSifUiApplicationName = "app";            // string
const QString KSifUiApplicationIconHandle = "icon";     // integer, bitmap handle
const QString KSifUiApplicationIconMaskHandle = "mask"; // integer, bitmap handle
const QString KSifUiApplicationVersion = "ver";         // string
const QString KSifUiApplicationSize = "size";           // integer
const QString KSifUiApplicationDetails = "details";     // string list
const QString KSifUiMemorySelection = "mem";            // string (comma separated drive letters)
const QString KSifUiCertificates = "cert";              // binary

// Variant map keys for "progress note" dialog parameters
const QString KSifUiProgressNoteText = "txt";           // string
const QString KSifUiProgressNoteFinalValue = "fin";     // integer
const QString KSifUiProgressNoteValue = "val";          // integer
const QString KSifUiProgressNotePhase = "phase";        // enum SifUiInstallingPhase

// Variant map keys for "progress note" and "installation complete note" buttons
const QString KSifUiProgressNoteIsHideButtonHidden = "hidebtn";     // boolean
const QString KSifUiProgressNoteIsCancelButtonHidden = "cancelbtn"; // boolean
const QString KSifUiCompleteNoteIsShowButtonHidden = "showbtn";     // boolean
const QString KSifUiErrorNoteIsDetailsButtonHidden = "errdtlbtn";   // boolean

// Variant map keys for "error" dialog parameters
const QString KSifUiErrorCategory = "errcat";           // enum Usif::TErrorCategory
const QString KSifUiErrorCode = "errcode";              // integer
const QString KSifUiErrorCodeExtended = "errext";       // integer
const QString KSifUiErrorMessage = "errmsg";            // string
const QString KSifUiErrorDetails = "errdet";            // string

// Variant map keys for "grant capabilities" dialog parameters
const QString KSifUiGrantCapabilities = "grcap";        // binary (TCapabilitySet)

// Variant map keys for "select language" dialog parameters
const QString KSifUiSelectableLanguages = "lang";       // binary (TPckg< RArray<TLanguage> >)
const QString KSifUiSelectedLanguageIndex = "langind";  // integer

// Variant map keys for "select options" dialog parameters
const QString KSifUiSelectableOptions = "opts";         // descriptor array
const QString KSifUiSelectedOptions = "optsind";        // binary (TPckg< RArray<TInt> >)

#endif // SIFUIDIALOGDEFINITIONS_H

