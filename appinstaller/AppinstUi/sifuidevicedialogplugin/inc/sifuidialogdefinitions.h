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

enum SifUiDeviceDialogMode {
    SifUiUnspecified = 0,
    SifUiInstalling = 1,
    SifUiUninstalling = 2
};

// Variant map keys for dialog type and title (common to all dialog types)
const QString KeySifUiDialogType = "type";              // enum SifUiDeviceDialogType
const QString KSifUiDialogMode = "mode";                // enum SifUiDeviceDialogMode
const QString KSifUiDialogTitle = "title";              // string

// Variant map keys for device dialog return values
const QString KSifUiQueryAccepted = "accept";           // boolean
const QString KSifUiSelectedMemoryIndex = "memi";       // integer

// Variant map keys for "confirmation query" dialog parameters
const QString KSifUiApplicationName = "app";            // string
const QString KSifUiApplicationIconHandle = "icon";     // integer, bitmap handle
const QString KSifUiApplicationIconMaskHandle = "mask"; // integer, bitmap handle
const QString KSifUiApplicationVersion = "ver";         // string
const QString KSifUiApplicationSize = "size";           // integer
const QString KSifUiApplicationDetails = "details";     // string list
const QString KSifUiMemorySelection = "mem";            // string list
const QString KSifUiCertificates = "cert";              // string list ??

// Variant map keys for "progress note" dialog parameters
const QString KSifUiProgressNoteText = "txt";           // string
const QString KSifUiProgressNoteFinalValue = "fin";     // integer
const QString KSifUiProgressNoteValue = "val";          // integer

// Variant map keys for "error" dialog parameters
const QString KSifUiErrorCode = "err";                  // integer


#endif // SIFUIDIALOGDEFINITIONS_H

