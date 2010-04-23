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
* Description: Definitions for software install progress indicator
*
*/

#ifndef SIFUIINSTALLINDICATORPARAMS_H
#define SIFUIINSTALLINDICATORPARAMS_H

// Install indicator plugin type string
const QString KSifUiIndicatorPlugin = "com.nokia.sifui.indi/1.0";

// Variant map keys for install indicator
const QString KSifUiIndicatorApplicationName = "app";       // string
const QString KSifUiIndicatorAppIconHandle = "icon";        // integer, bitmap handle
const QString KSifUiIndicatorAppIconMaskHandle = "mask";    // integer, bitmap handle
const QString KSifUiIndicatorProgressValue = "pval";        // integer
const QString KSifUiIndicatorProgressFinal = "pfin";        // integer

// Path and value name for QValueSpacePublisher/QValueSpaceSubscriber.
const QString KSifUiIndicatorPath = "/sifui";
const QString KSifUiIndicatorActive = "indicator_active";

#endif // SIFUIINSTALLINDICATORPARAMS_H

