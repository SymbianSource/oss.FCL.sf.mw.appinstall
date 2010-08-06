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
* Description:  Variant map key names for SW install progress notifications
*               in status menu (universal indicator plugin).
*/

#ifndef SIFUIINSTALLINDICATORDEFINITIONS_H
#define SIFUIINSTALLINDICATORDEFINITIONS_H

// Indicator type string for CHbIndicatorSymbian::Activate():
_LIT( KSifUiInstallIndicatorType, "com.nokia.sifui.indi/1.0" );

// Parameter names for variant map:
_LIT( KSifUiInstallIndicatorAppName, "name" );      // string
_LIT( KSifUiInstallIndicatorPhase, "phase" );       // int CSifUi::TInstallingPhase
_LIT( KSifUiInstallIndicatorProgress, "prog" );     // int (0..100), shown as percentage

// CHbIndicatorSymbian::Activate() takes one CHbSymbianVariant parameter. Use
// CHbSymbianVariant::EVariantMap type and the above parameter names to pass
// application name, installing phase, and progress bar value to notification
// dialog. Usually it is necessary to pass all these details when opening the
// notification dialog. Later, notification dialog details can be updated by
// calling CHbIndicatorSymbian::Activate() again. If only application name
// should be updated, then also CHbSymbianVariant::EDes type can be used.
// And if only progress bar value should be updated, then also
// CHbSymbianVariant::EInt type can be used.

#endif  // SIFUIINSTALLINDICATORDEFINITIONS_H

